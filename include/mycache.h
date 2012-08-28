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

struct mycache{
    struct hasmap *mp;//保存key value实体的缓存
    struct list *dlist;//保存各个key热度顺序的list，用于写入文件
    long bsize;
};

extern struct mycache *cache_create(long hsize, long bsize);

extern void cache_destroy(struct mycache *cache, hfrcb fr);

extern int cache_put(struct mycache *cache, char *key, void *value);

extern void *cache_get(struct mycache *cache, char *key);

extern int cache_wtoD(struct mycache *cache, long writesize);
