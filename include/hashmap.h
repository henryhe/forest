/**
 *author hepan
 *des 在list基础上实现hashmap，为了统一，要求所有key为char*类型，value不限制
 */
#include "common.h"
#include "list.h"

#ifndef hashmap_include_flag

#define hashmap_include_flag
#define KEY_SIZE 1024
struct hmap_e{
	char *key;
	void *value;
	struct hmap_e *next; 
};

struct hmap{
	long size;
    int hsize;
	struct hmap_e **em;
};
//释放hmap中value的回调函数
typedef int (*hfrcb)(void *);
#endif

extern struct hmap_e *hmap_e_create(char *key, void *value);

extern struct hmap* hmap_create();

extern struct hmap* hmap_create_ws(int hsize);

//extern void hmap_destroy(struct hmap *mp);
extern void hmap_destroy(struct hmap *mp, hfrcb fr);

int hmap_key_destroy(struct hmap_e *es, hfrcb fr);

extern void hmap_put(struct hmap *mp, char *key, void *value);

extern int hmap_put_wcb(struct hmap *mp, char *key, void * value, hfrcb fr);

extern void *hmap_get(struct hmap *mp, char *key);

extern struct hmap_e  *hmap_get_e(struct hmap *mp, char *key);

extern struct hmap_e *hmap_del(struct hmap *mp, char *key);
