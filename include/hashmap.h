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

typedef void (*hfrcb)(void *);
#endif

extern struct hmap_e *hmap_e_create(char *key, void *value);

extern struct hmap* hmap_create();

//extern void hmap_destroy(struct hmap *mp);
//释放一个key下的所有元素，并返回返回的元素数目
extern void hmap_destroy(struct hmap *mp, hfrcb fr);

int hmap_key_destroy(struct hmap_e *es, hfrcb fr);

extern void hmap_put(struct hmap *mp, char *key, void *value);

extern void *hmap_get(struct hmap *mp, char *key);
