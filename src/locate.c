/*
 * =====================================================================================
 *
 *       Filename:  locate.c
 *
 *    Description:  locate中的公用函数
 *
 *        Version:  1.0
 *        Created:  2012年08月21日 18时12分53秒
 *       Revision:  1.0 
 *       Compiler:  gcc
 *
 *         Author:  hepan 
 *   Organization:  tigerknows 
 *
 * =====================================================================================
 */
#include "../include/locate.h"

int freeR(void *t)
{
    struct record *r = (struct record *)t;
    if (r->key != NULL)
        free(r->key);
    if (r->e != NULL)
        free(r->e);
    free(r);
    return 1;
}

int freeP(void *t){
    struct pdata *p = (struct pdata *)t;
    free(p);
    return 1;
}

/* 
 * function : 释放keydata结构
 * input    : keydata结构
 * output   ：所释放的keydata所占内存大小 
 */
int freeKD(void *t)
{
  struct keydata *kd = (struct keydata *)t;
  int size = kd->bsize;
  free(kd->key);
  list_destroy(kd->list, freeP);
  free(kd);
  return size;
}

/* 
 * function ：判断kye的类型
 * input    ：char *key
 * output   : 代表类型的字符，b:基站；w:wifi
 */
char getkeytype(char *key)
{
    char *p = key;
    while (TRUE)
    {
        if (*p == '\0')
            break;
        if (*p == '.')
            return 'b';
        if (*p == ':')
            return 'w';
        p++;
    }
    return '0';
}

/*
 * function : 打印R的所有信息
 * input    : 一条位置信息的结构
 * output   : stdout
 */
void printR(struct record r){
    printf("%s ", r.key);
    printf("%ld ", r.time);
    printf("%lf %lf %lf ", r.x, r.y, r.p);
    printf("%d ", r.type);
    printf("%s\n", r.e);
}

/*
 * function : 打印R的所有信息
 * input    : 指向一条位置信息结构的指针
 * output   : stdout
 */
void printRp(struct record *r){
    printf("%s ", r->key);
    printf("%ld ", r->time);
    printf("%lf %lf %lf ", r->x, r->y, r->p);
    printf("%d ", r->type);
    printf("%s\n", r->e);
}
