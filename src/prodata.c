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

void prodata()
{
    struct mycache *cache = cache_create(1024,1);
    char *line = (char *)malloc(READ_BUFFER_SIZE);
    int i = 0;
    FILE *f = fopen("test","r");
    while (fgets(line, READ_BUFFER_SIZE, f) != NULL)
    {
        printf("%s",line);
        struct record *r = stoR(line);
        if (r == NULL )
            continue;
        printRp(r);
        char *key = (char *)malloc(5);
        sprintf(key, "%d", i);
        cache_put(cache, key, r, 50, testm);
        i++;
        if (i > 5000)
            break;
    }
    i = 0;
    printf("put over ,to get \n");
    getchar();
    for(; i <= 5000; i++){
        char *key = (char *)malloc(5);
        sprintf(key, "%d", i);
        struct record *r = (struct record *)cache_get(cache, key);
        printRp(r);
    }
    cache_destroy(cache,freeR);
    getchar();
    getchar();
    getchar();
    free(line);
}

int main()
{   
    prodata(); 
    return 1;
}

