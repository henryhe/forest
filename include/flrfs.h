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
#include "myUtil.h"
#ifndef flrfs_include_flag

#define indexRsize 20

#define readRnum 10

#define FKEY_SIZE 30

#define pdtail '#'

#define LIVE 'l'

#define DEAD 'd'

#define FILENUM 6

#endif

struct indexR{
    char *key;
    int filename;
    int offset;
    char flag;
};

struct index{
    char *path;
    struct list *list;
    long size;
};

struct fR{
    char *key;
    void *data;
};

static int fnames[FILENUM] = {128, 256, 512, 1024, 2048,4096};

static int frs[FILENUM] = {0, 0, 0, 0, 0, 0};

extern int saveindex(struct list *index,char *path);

extern struct index *loadindex(char *path);

extern struct list_e *locatekey(char *key, struct index *index);

/* 
 * function : 读取输入key的已有数据
 * intput   : flr文件存放路径，对应的index，目标key
 * output   ：包含key和data域的fR结构
 */
extern struct fR *flr_read(char *path, struct index *index, char *key);

extern int flr_write(char *path, struct index *index, char *key, void *data);

struct index *createindex();

void datatoindex(struct index *index, void *data, long dsize);

extern int getfilename(int size);

extern void addtoindex(struct list_e *pre, struct indexR *r);
