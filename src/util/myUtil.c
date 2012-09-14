/*
 * =====================================================================================
 *
 *       Filename:  myUtil.c
 *
 *    Description:  常用util
 *
 *        Version:  1.0
 *        Created:  2012年09月14日 17时28分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  hepan
 *   Organization:  tigerknows 
 *
 * =====================================================================================
 */
#include "../../include/myUtil.h"

/*
 * function : 根据两个指针之间的内容，生成新的字符串，新申请的空间，需要free
 * input    : 两个字符指针，start,end;start指向需要内容的第一个字符，end指向末尾字符的后边一个空间
 * output   : 返回新生成的字符串，malloc了空间，需要free
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
 * function : 截取一个字符串的子串，转化为int
 * input    : char *ori 原始字符串，int from 字串起始位置，int to 字串结束位置
 * output   : 对应的整数
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

/* 
 * funciton : 获得当前时间 
 * input    : 无
 * output   : 字符串返回
 */
char *getnowtime()
{      
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char *s = asctime(timeinfo);
    int len = strlen(s);
    char *t = (char *)malloc(len);
    memcpy(t, s, len - 1);
    *(t + len) = '\0';
    return t;
}  

