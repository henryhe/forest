/*
 * =====================================================================================
 *
 *       Filename:  flrfs.h
 *
 *    Description:  定长数据存储系统
 *
 *        Version:  1.0
 *        Created:  2012年08月27日 17时57分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  hepan 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "common.h"
#include "list.h"
#include "locate.h"

#ifndef flrfs_include_flag

#define indexRsize 25

#define readRnum 10000000

#define keysize 10

#endif

struct indexR{
    char *key;
    int filename;
    long offset;
    short flag;
};

struct index{
    struct list *list;
    long size;
};

struct flR{
    char *key;
    struct list *dlist;
};

extern int writeindex(struct list *index,char *path);

extern struct index *loadindex(char *path);

extern struct flR *readflR(char *key, struct index *index);

extern int writeflR(char *key, struct index *index);

extern int writeflRs(struct list *list,struct index *index);

extern struct list *readflRs(struct list *list,struct index *index);

int getfilename(struct flR *r);

