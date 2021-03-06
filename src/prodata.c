/*
 * =====================================================================================
 *
 *       Filename:  writelog.c
 *
 *    Description:  存储解析好的基站位置信息    
 *
 *        Version:  1.0
 *        Created:  2012年08月16日 11时15分02秒
 *       Revision:  1.0
 *       Compiler:  gcc
 *
 *         Author:  hepan
 *   Organization:  tigerknows
 *
 * =====================================================================================
 */
#include "../include/locate.h"
#include "../include/mycache.h"
#include "../include/flrfs.h"
#include "../include/log.h"
#include "../include/config.h"
#include "../include/myUtil.h"

/* 
 * function : 解析输入的行为一条数据记录 
 * input    : 空格分隔的字符串数据
 * output   : record结构体
 */
struct record *stoR(char *str){  
    char *start = str;
    char *end = str;
    //周边的信息行抛弃
    if (*start == 'n' && *(start + 1) == 'e')
        return NULL;
    struct record *r = (struct record *)malloc(sizeof(struct record));
    //field state：读入字段的顺序
    int fs = 0;
    while (1){
        if (*end == '\0'){
            char *s = createstr(start, end);
            r->e = s;
            break;
        }
        if (*end == ' '){
            char *s = createstr(start, end);
            switch(fs){
                case 0:
                    r->key = s;
                    break;
                case 1:
                {
                    long time = atol(s);
                    r->time = time;
                    free(s);
                }
                    break;
                case 2:
                {
                    float x = atof(s);
                    r->x = x;
                    free(s);
                }
                    break;
                case 3:
                {
                    float y = atof(s);
                    r->y = y;
                    free(s);
                }
                    break;
                case 4:
                {
                    float p = atof(s);
                    r->p= p;
                    free(s);
                }
                    break;
                case 5:
                {
                    int t = atoi(s);
                    r->type = t;
                    free(s);
                }
                    break;
                case 6:
                    r->e = s;
                    break;
            }
            fs++;
            start = ++end;
            continue;
        }
        end++;
    }
    r->pro = 1.0;
    return r;
}

/* 
 * function : 将record结构转化为可直接存储得的pdata结构，并释放之前的record
 * input    : record结构
 * output   ：对应pdata结构
 */
struct pdata *RtoP(struct record *r){
    struct pdata *p = (struct pdata *)malloc(sizeof(struct pdata));
    p->time = r->time;
    p->x = r->x;
    p->y = r->y;
    p->p = r->p;
    p->pro = 1.0;    
    return p;
}

/* 
 * function : 将一个排列好的内存块转化为pdata结构
 * input    : 指向排列好的内存块指针
 * output   : pdata结构 */
struct pdata *dtoP(void *data){
    void *pos =data;
    struct pdata *p = (struct pdata *)malloc(sizeof(struct pdata));
    int size = sizeof(long);
    memcpy(&(p->time), pos, size);
    pos += size;
    size = sizeof(float);
    memcpy(&(p->x), pos, size);
    pos += size;
    memcpy(&(p->y), pos, size);
    pos += size;
    memcpy(&(p->p), pos, size);
    pos += size;
    size = sizeof(int);
    memcpy(&(p->type), pos, size);
    pos += size;
    size = sizeof(double);
    memcpy(&(p->pro), pos, size);
    pos += size;
    if (*(char *)pos != flrtail){
        log_error("run read in data");
        return NULL;
    }
    return p;
}

/*
 * function : 将从flrfs系统的数据转化为keydata
 * input    : flrfs系统的一条定长数据
 * output   : keydata
 */
struct keydata *toKD(char *key, struct fR *fr){
    if (strcmp(key, fr->key) != 0){
        log_error("key conflict : ");        
        return NULL;
    }
    struct keydata *kd = (struct keydata *)malloc(sizeof(struct keydata));
    kd->key = (char *)malloc(strlen(kd->key) + 1);
    strcpy(kd->key, fr->key);
    int rnum =  kd->bsize / rdatasize;
    kd->list = list_create();
    void *pos = fr->data;
    void *temp = malloc(rdatasize);
    int i;
    for (i = 0; i < rnum; i++){
        memcpy(temp, pos, rdatasize);
        struct pdata *p = dtoP(temp);
        struct list_e *e = listnode_create(e);
        list_add(kd->list, e);
        pos += rdatasize;
        i++;
    }
    free(temp);
    return kd;
}

/*
 * function : 合并keydata和新的record
 * input    : keydata 和 record,无需在外部free掉新的record,更新cache的bsize
 * output   : 新的keydata
 */
struct keydata *mergeKD(struct keydata *kd, struct record *r, struct mycache *cache){
    struct pdata *p = RtoP(r);
    struct list_e *e = listnode_create(p);
    list_add(kd->list, e);
    kd->bsize += rdatasize;
    cache->bsize += rdatasize;
    return kd;
}

/*
 * function : 根据传入的record生成新的keydata
 * input    : record
 * output   : keydata
 */
struct keydata *createKD(struct record *r){
    struct keydata *kd = (struct keydata *)malloc(sizeof(struct keydata));
    kd->key = (char *)malloc(strlen(r->key) + 1);
    strcpy(kd->key, r->key);
    kd->list = list_create();
    struct pdata *p = RtoP(r);
    kd->bsize = rdatasize;
    struct list_e *e = listnode_create(p);
    list_add(kd->list, e);
    return kd;
}

/* 
 * function : 从flrfs系统读取数据
 * input    : flrfs系统对应的index，和目标record
 * output   : record对应的已经在flrfs中的数据 
 */
struct fR *readKD(char *flrpath, struct index *index, struct record *r){
    struct fR *fr = flr_read(flrpath, index, r->key);
    return fr;
}

/* 
 * function : 根据传入的record 得到对应的key的所有数据
 * descri   : 首先去flrfs系统读取相关数据，如果没有则生成一个新的keydata返回
 * input    : 对应flrfs系统的index，目标record
 * output   : 对应key的最新数据
 */
struct keydata *getKD(char *flrpath, struct index *index, struct record *r, struct mycache *cache){
    struct fR *fr = readKD(flrpath,index, r);
    if (fr != NULL){
        struct keydata *kd = toKD(r->key, fr);
        freefR(fr);
        if (kd == NULL)
            return createKD(r);
        return mergeKD(kd, r, cache);
    }
    return createKD(r);
}

/* function : 将keydata转化为准备写入flr文件的2进制流,包含key的拓宽到定长，不包含整个数据流的拓展
 * input    : keydata
 * output   : 对应的2进制流 */
struct fR *KtofR(struct keydata *k){
    int expectsize =  k->bsize;
    struct fR *fr = (struct fR *)malloc(sizeof(struct fR));
    fr->key = (char *)malloc(strlen(k->key) + 1);
    fr->data = malloc(expectsize);
    fr->bsize = k->bsize;
    strcpy(fr->key, k->key);
    void *pos = fr->data;
    int count = 0;
    struct list_e *e = k->list->head;
    while (e){
        struct pdata *p = (struct pdata *)e->data;
         
        int size = sizeof(long);
        memcpy(pos, &(p->time), size);
        pos += size;
        count += size;
        
        size = sizeof(float);
        memcpy(pos, &(p->x), size);
        pos += size;
        memcpy(pos, &(p->y), size);
        pos += size;
        memcpy(pos, &(p->p), size);
        pos += size;
        count += size * 3;

        size = sizeof(int);
        memcpy(pos, &(p->type), size);
        pos += size;
        count += size;
        
        size = sizeof(double);
        memcpy(pos, &(p->pro), size);
        pos += size;
        count += size;
        e = e->next;
    }
    if (count != expectsize){
        char *s = mystrcat("size not match when write to flr :", k->key);
        log_error(s);
        free(s);
        return NULL;   
    }
    return fr;
}

/* 
 * function : 将从cache里边换出的数据写入到flr
 * input    ：由keydata组成的list 
 * output   : 无，拼接为数据流，写入到硬盘
 */
void writetoflr(char *flrpath, struct index *index, struct list *list){
    struct list_e *e = list->head;
    while (e){
        struct keydata *k = (struct keydata *)e->data;
        struct fR *fr = KtofR(k);
        flr_write(flrpath, index, k->key, fr);
        freefR(fr);
        e = e->next;
    }
}

/* 
 * function : 计算一个keydata的占用空间
 * input    : keydata
 * output   : 占用空间大小
 */

int sizeofK(void *t){
    struct keydata *k = (struct keydata *)t;
    return strlen(k->key) + sizeof(k->bsize) + k->list->size * sizeof(struct pdata);
}

void cache_wtoflr(struct mycache *cache, struct index *index, char *path) {
    struct list *list = cache_valuelist(cache);
    writetoflr(path, index, list);
}


struct mycache *prodata()
{
    char *line = (char *)malloc(READ_BUFFER_SIZE);
    struct mycache *cache = cache_create(10240,1);
    struct config *config = loadconfig();
    char *indexpath = mystrcat(config->flrpath, "index");
    struct index *index = loadindex(indexpath);
    struct record *r = NULL;
    FILE *fp = fopen("case", "r");
    //while (fgets(line, READ_BUFFER_SIZE, stdin) != NULL)
    while (fgets(line, READ_BUFFER_SIZE, fp) != NULL) {
        printf("%s",line);
        r = stoR(line);
        if (r == NULL)
            continue;
        struct keydata *kd = (struct keydata *)cache_get(cache, r->key);
        if (kd == NULL){
            kd = getKD(config->flrpath, index, r, cache);
            char *key = (char *)malloc(strlen(kd->key) + 1);
            strcpy(key, kd->key);
            if (cache_put(cache, key, kd, kd->bsize,freeKD) == -1) {
                cache_wtoflr(cache, index, config->flrpath);
                saveindex(index);
                cache_clear(cache, freeKD);
                cache_put(cache, key, kd, kd->bsize, freeKD); 
            }
        }else{
            mergeKD(kd, r, cache);
        }
        freeR(r);
    }
    fclose(fp);
    cache_wtoflr(cache, index, config->flrpath);
    cache_clear(cache, freeKD);
    saveindex(index);
    destroyindex(index, freeiR);
    config_destroy(config);
    free(line);
    return cache;
}

int main()
{   
    struct mycache *cache = prodata();

}

