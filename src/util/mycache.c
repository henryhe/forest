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
    cache->limsize = limsize;
    cache->bsize = 0;
    return cache;
}

void cache_destroy(struct mycache *cache, hfrcb hfr){
    hmap_destroy(cache->mp, hfr);
    list_destroy(cache->hotlist, NULL);
    free(cache);
}

int cache_put(struct mycache *cache, char *key, void *value,int size){
    if (cache->bsize + size >= cache->limsize)
        return -1;
    hmap_put_wcb(cache->mp, key, value, size, NULL);//TODO 合并的函数还没写
    //处理hotkeylist和keysizelist
    struct list_e *pre = listnode_create(NULL);//虚拟的头，便于代码编写
    pre->next = cache->hotlist->head;
    int index = -1;
    while (pre->next){
        index++;
        if (strcmp(key, (char *)pre->next->data) == 0)
            break;
        pre = pre->next;
    }
    if (index < 0 || pre->next == NULL){//hotlist中没有要放入key的情况
        struct list_e *e = listnode_create(key);
        list_add(cache->hotlist, e);
    }else{
        list_totail(cache->hotlist, pre, pre->next);
    }
    cache->bsize += size;
    return size;
}

int main()
{
    printf("hello world ,I am hp cache\n");
    while(1){
    struct mycache *cache = cache_create(10000,4000000);
    cache_destroy(cache,NULL);
    }
    getchar();
}

