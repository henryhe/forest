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

void freeR(struct record *r)
{
    free(r->key);
    free(r->e);
    free(r);
}

void freeK(struct keydata *k)
{
  free(k->key);
}

/*
 * function:根据两个指针之间的内容，生成新的字符串，新申请的空间，需要free
 * input   :两个字符指针，start,end;start指向需要内容的第一个字符，end指向末尾字符的后边一个空间
 * output  :返回新生成的字符串，malloc了空间，需要free
 */
char *createstr(char *start, char *end)
{
    int len = end - start ;
    char *result  = (char *)malloc(len + 1);
    memcpy(result, start, len);
    *(result + len) = '\0';
    return result;
}

/* 
 * function：判断kye的类型
 * args：char *key
 * return:代表类型的字符，b:基站；w:wifi
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
 * function:截取一个字符串的子串，转化为int
 * args:char *ori 原始字符串，int from 字串起始位置，int to 字串结束位置
 * return:对应的整数
 */
int getint(char *ori, int from , int to)
{	
    int n = to - from + 1;
	ori += from;
	char* temp = (char*)malloc(n+1);
	memcpy(temp, ori, n);
	*(temp + n) = '\0';
	int result = atoi(temp);
	free(temp);
	return result;
}
