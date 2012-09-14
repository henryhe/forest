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

/* 
 * function : 解析输入的行为一条数据记录 
 * input    : 空格分隔的字符串数据
 * output   : record结构体
 */
struct record *stoR(char *s){  
    char *start = s;
    char *end = s;
    //周边的信息行抛弃
    if (*start == 'n' && *(start + 1) == 'e')
        return NULL;
    struct record *r = (struct record *)malloc(sizeof(struct record));
    //field state：读入字段的顺序
    int fs = 0;
    while (TRUE){
        if (*end == '\0'){
            char *s = createstr(start, end);
            r->e = s;
            break;
        }
        if (*end == ' ' ){
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
                    free(s);
                    break;
            }
            fs++;
            start = ++end;
            continue;
        }
        end++;
    }
    return r;
}

void printRp(struct record *r){
    printf("%s ", r->key);
    printf("%ld ", r->time);
    printf("%lf %lf %lf ", r->x, r->y, r->p);
    printf("%d ", r->type);
    printf("%s\n", r->e);
}

int testm(void * r,void *d){
    return 0;
}

/*
 * function : 将从flrfs系统的数据转化为keydata
 * input    : flrfs系统的一条定长数据
 * output   : keydata
 */
struct keydata *toKD(struct fR *fr){
    return NULL;
}

/* function : 合并keydata和新的record
 * input    : keydata 和 record 
 * output   : 新的keydata*/
struct keydata *mergeKD(struct keydata *kd, struct record *r){
    return NULL;
}

/*
 * function : 根据传入的record生成新的keydata
 * input    : record
 * output   : keydata
 */
struct keydata *createKD(struct record *r){
    return NULL;
}

/* function : 从flrfs系统读取数据
 * input    : flrfs系统对应的index，和目标record
 * output   : record对应的已经在flrfs中的数据 */
struct fR *readKD(struct index *index, struct record *r){
    struct fR *fr = flr_read(index, r->key);
    return fr;
}

/* 
 * function : 根据传入的record 得到对应的key的所有数据
 * descri   : 首先去flrfs系统读取相关数据，如果没有则生成一个新的keydata返回
 * input    : 对应flrfs系统的index，目标record
 * output   : 对应key的最新数据
 */
struct keydata *getKD(struct index *index, struct record *r){
    struct fR *fr = readKD(index, r);
    if (fr != NULL){
        struct keydata *kd = toKD(fr);
        return mergeKD(kd, r);
    }
    return createKD(r);
}

struct mycache * prodata()
{
    char *line = (char *)malloc(READ_BUFFER_SIZE);
    struct mycache *cache = cache_create(1024,1);
    struct index *index = loadindex("index");
    FILE *f = fopen("test","r");
    while (fgets(line, READ_BUFFER_SIZE, f) != NULL)
//    while (fgets(line, READ_BUFFER_SIZE, stdin) != NULL)
    {
        printf("%s",line);
        struct record *r = stoR(line);
        if (r == NULL )
            continue;
        printRp(r);
        struct keydata *kd = (struct keydata *)cache_get(cache, r->key);
        if (kd == NULL)
            kd = getKD(index, r);
        cache_put(cache, r->key, kd, kd->bsize,NULL);
    }
    fclose(f);
    free(line);
    return cache;
}

int main()
{   
    struct mycache *cache = prodata(); 
    cache_destroy(cache,freeR);
    return 1;
}

