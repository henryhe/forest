#include "common.h"
#include "list.h"
#include "hashmap.h"
#include "myUtil.h"
#include "stat.h"

#ifndef locate_include_flag

#define READ_BUFFER_SIZE 1024*21*5//读入日志行时的最大程度，取决于安卓行为日志的最大长度20K，实际情况发现有更长的
#define RECORD_SIZE 80//一条记录的长度
#define RECORD_LIM 30000000//同时在内存的入基站，wifi信息的上限


struct log
{//一条log的结构
	long time;
	struct hmap *paramp;//基本参数的map
    struct list *wifikeylist;//wifi mac地址的list
	struct list *wifisslist;//sifi 信号的list和key一一对应
	struct list *n8blaclist;//基站laclist
	struct list *n8bcilist;//基站ci的list
	struct list *n8bsslist;//基站信号的list
};

struct record
{//一条有效的记录
	long time;//提交时间
	char *key;//基站或者wifi的key
	float x,y,p;//此条日志提交的gps坐标，和精度
	int type;//类型，wifi为0，主基站为1，其他基站为2
    double pro;//正确度，保留字段
	char *e;
};

struct pdata 
{//一条位置信息
	long time;//提交时间
	float x,y,p;//此条日志提交的gps坐标，和精度
	int type;//类型，wifi为0，主基站为1，其他基站为2
    double pro;//正确度，保留字段
};

static int rdatasize = sizeof(long) + sizeof(float) * 3 + sizeof(int) + sizeof(double);//一个key对应的每条位置信息的大小

struct keydata
{//一个key对应的一批数据
    char *key;
    long bsize;
    struct list *list;
};
#endif

extern int freeR(void *w);

extern int freeKD(void *t);

/* 
 * function ：判断kye的类型
 * input    ：char *key
 * output   : 代表类型的字符，b:基站；w:wifi
 */
extern char getkeytype(char *key);

/*
 * function : 打印R的所有信息
 * input    : 一条位置信息
 * output   : stdout
 */
extern void printR(struct record r);
