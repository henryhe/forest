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

#define indexRsize 20

#define readRnum 10

#define keysize 11

#endif

struct indexR{
    char *key;
    int filename;
    int offset;
    char flag;
};

struct index{
    struct list *list;
    long size;
};

struct flR{
    char *key;
    int num;
    struct list *dlist;
};

extern int saveindex(struct list *index,char *path);

extern struct index *loadindex(char *path);

extern struct list_e *locatekey(char *key, struct index *index);

extern struct flR *readflR(struct indexR *r);

extern int writeflR(struct indexR *r);

extern int writeflRs(struct list *list,struct index *index);

extern struct list *readflRs(struct list *list,struct index *index);

struct index *createindex();

void datatoindex(struct index *index, void *data, long dsize);

extern int getfilename(struct flR *flr);

extern void addtoindex(list_e *pre, struct indexR *r);
