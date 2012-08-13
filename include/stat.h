/*
 * =====================================================================================
 *
 *       Filename:  stat.h
 *
 *    Description:  用于定义统计和报表的结构等
 *
 *        Version:  1.0
 *        Created:  2012年08月10日 16时42分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME hepan 
 *   Organization:  
 *
 * =====================================================================================
 */
struct stat
{   
    long linenum;//读入日志的行数
    long lognum;//有效的日志数目
    long basekeynum;//获取的主基站数目
    long nkeynum;//获取的周边基站数目
    long wkeynum;//获取的wifi基站数目
};

