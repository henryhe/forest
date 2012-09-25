/*
 * =====================================================================================
 *
 *       Filename:  mycache.c
 *
 *    Description:  cache的c文件
 *
 *        Version:  1.0
 *        Created:  2012年08月24日 16时04分59秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  hepan 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "../../include/mycache.h"

struct mycache *cache_create(int hsize, long limsize){
    struct mycache *cache = (struct mycache *)malloc(sizeof(struct mycache));
    cache->mp = hmap_create_ws(hsize);
    cache->hotlist = list_create();
    cache->limsize = limsize * 1024 * 1024 * 1024;
   // cache->limsize = limsize;
    cache->bsize = 0;
    return cache;
}

void cache_destroy(struct mycache *cache, hfrcb hfr){
    hmap_destroy(cache->mp, hfr);
    list_destroy(cache->hotlist, NULL);
    free(cache);
}

int cache_put(struct mycache *cache, char *key, void *value,int size, hfrcb fr){
    if (cache->bsize + size >= cache->limsize)
        return -1;
    hmap_put_wcb(cache->mp, key, value, fr);
    hotthekey(cache, key);
    //bsize 的计算包括放到cache里存放到map里，和hotkeylist的开销
    cache->bsize += size + strlen(key) * 2  + sizeof(struct hmap_e *) + sizeof(struct list_e *);
    return size;
}

/* 
 * function : 更新cache里的key的热度，刚使用的放到链表尾部，
 *            移除的时候从头部开始移除 
 * input    : cache，和涉及元素的key
 * output   : 无
 */
void hotthekey(struct mycache *cache, char *key){
    //处理hotkeylist和keysizelist
    struct list_e *temp = listnode_create(NULL);//虚拟的头，便于代码编写
    struct list_e *pre= temp;//虚拟的头，便于代码编写
    pre->next = cache->hotlist->head;
    int index = -1;
    while (pre->next){
        index++;
        if (strcmp(key, (char *)pre->next->data) == 0)
            break;
        pre = pre->next;
    }
    if (index < 0 || pre->next == NULL){//hotlist中没有要放入key的情况
        char *newkey = (char *)malloc(strlen(key) + 1);
        strcpy(newkey, key);
        struct list_e *e = listnode_create(newkey);
        list_add(cache->hotlist, e);
    }else{
        list_totail(cache->hotlist, pre, pre->next);
    }
    free(temp);//因为data域为NULL所以直接free即可
}

void *cache_get(struct mycache *cache, char *key){
    void *result = hmap_get(cache->mp, key);
    if (result != NULL)
        hotthekey(cache, key);
    return result;
}

struct list *cache_kickout(struct mycache *cache, char *path, float fra, caccb cac){
    hmap_print(cache->mp);
    int kosize = cache->bsize * fra;
    int count = 0;
    struct list_e *e = cache->hotlist->head;
    struct list *list = list_create();
    while (e){
        if (count >= kosize)
            break;
        struct hmap_e *he = hmap_del(cache->mp, (char *)e->data);
        struct list_e *le = listnode_create(he->value);
        list_add(list, le);
        cache->hotlist->head = cache->hotlist->head->next;
        free(e->data);
        free(e);
        free(he->key);
        free(he);
        count += (*cac)(he->value) + strlen(he->key)*2 + sizeof(struct hmap_e *) + sizeof(struct list_e *);
        e = cache->hotlist->head;
    }
    cache->bsize -= count;
    return list;
}

int cmain()
{
    printf("hello world ,I am hp cache\n");
    while(1){
        struct mycache *cache = cache_create(10000,4);
        cache_destroy(cache,NULL);
    }
    getchar();
}
