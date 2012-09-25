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

#define FKEY_LENGTH 30
//定长文件每条记录的结尾，及填充字符
#define flrtail '#'

#define keytail '$'

#define LIVE 'l'

#define DEAD 'd'

//一共有多少个用于存储的定长文件
#define FILENUM 6

#define AVAI '1'

#define UNAVAI '0'
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
    long bsize;
};

struct fR{
    char *key;
    int bsize;//data的大小
    void *data;//除去key的有效内容
};

static int fnames[FILENUM] = {128, 256, 512, 1024, 2048, 4096};

static short IRSIZE = sizeof(int) * 2 + sizeof(char) + FKEY_LENGTH;

extern int saveindex(struct index *index);

extern struct index *loadindex(char *path);

extern struct list_e *locatekey(char *key, struct index *index);

/* 
 * function : 读取输入key的已有数据
 * intput   : flr文件存放路径，对应的index，目标key
 * output   ：包含key和data域的fR结构
 */
extern struct fR *flr_read(char *path, struct index *index, char *key);

extern int flr_write(char *path, struct index *index, char *key, struct fR *fr);

struct index *createindex(char *path);

void datatoindex(struct index *index, void *data, long dsize);

extern int getfilename(int size);

extern void addtoindex(struct list_e *pre, struct indexR *r);

extern int freefR(void *t);

extern int freeiR(void *t);

extern void destroyindex(struct index *index, lfrcb fr);
