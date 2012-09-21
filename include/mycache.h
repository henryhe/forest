/*
 * =====================================================================================
 *
 *       Filename:  mycache.h
 *
 *    Description:  cache的头文件
 *
 *        Version:  1.0
 *        Created:  2012年08月24日 15时21分03秒
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
#include "hashmap.h"

#ifndef mycache_def_flag
//在cache kick out的时候计算kickout元素所占空间的大小
typedef int (*caccb)(void *);

#endif
struct mycache{
    struct hmap *mp;//保存key value实体的缓存
    struct list *hotlist;//保存各个key热度顺序的list，用于写入文件
    long limsize;
    long bsize;
};

extern struct mycache *cache_create(int limsize, long bsize);

extern void cache_destroy(struct mycache *cache, hfrcb hfr);

extern int cache_put(struct mycache *cache, char *key, void *value, int size, hfrcb fr);

extern void *cache_get(struct mycache *cache, char *key);

extern struct list *cache_kickout(struct mycache *cache, char *path, float fra, caccb cac);

void hotthekey(struct mycache *cache, char *key);
