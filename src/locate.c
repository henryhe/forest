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

void freeR(void *r)
{
    free(((struct record *)r)->key);
    free(((struct record *)r)->e);
    free((struct record *)r);
}

void freeK(struct keydata *k)
{
  free(k->key);
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
 * input    : 一条位置信息
 * output   : stdout
 */
void printR(struct record r){
    printf("%s ", r.key);
    printf("%ld ", r.time);
    printf("%lf %lf %lf ", r.x, r.y, r.p);
    printf("%d ", r.type);
    printf("%s\n", r.e);
}
