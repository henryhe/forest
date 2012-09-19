/*
 * =====================================================================================
 *
 *       Filename:  myUtil.h
 *
 *    Description:  myUtil头文件
 *
 *        Version:  1.0
 *        Created:  2012年09月14日 17时29分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  hepan
 *   Organization:  tigerknows 
 *
 * =====================================================================================
 */
#include "common.h"

/*
 * function : 根据两个指针之间的内容，生成新的字符串，新申请的空间，需要free
 * input    : 两个字符指针，start,end;start指向需要内容的第一个字符，end指向末尾字符的后边一个空间
 * output   : 返回新生成的字符串，malloc了空间，需要free
 */
extern char *createstr(char *start, char *end);

/* 
 * function : 截取一个字符串的子串，转化为int
 * input    : char *ori 原始字符串，int from 字串起始位置，int to 字串结束位置
 * output   : 对应的整数
 */
extern int getint(char *ori, int from , int to);

/* 
 * funciton : 获得当前时间 
 * input    : 无
 * output   : 字符串返回
 */
extern char *getnowtime();

/* 
 * function : 拼接两个字符串
 * input    : 需要拼接的两个字符串，按参数位置拼接
 * output   ：拼好的新申请空间存放的字符串
 */
extern char *mystrcat(char *s1, char *s2);

/*
 * function : 得到整数的位数
 * input    : 要计算的数字
 * output   : 得到的位数
 */
extern int getdigitnum(int i);

/* 
 * function : 整数转化为字符串
 * input    : 需要转化的整数
 * output   : 结果字符串
 */
extern char *myitoa(int i);
