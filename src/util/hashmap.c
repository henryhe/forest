#include "../../include/hashmap.h"
//创建一个元素
struct hmap_e *hmap_e_create(char *key, void *value)
{
	struct hmap_e *e = (struct hmap_e *)malloc(sizeof(struct hmap_e));
	e->key = key;
	e->value = value;
	e->next = NULL;
	return e;
}

struct hmap *hmap_create()
{
	struct hmap *mp = (struct hmap *)malloc(sizeof(struct hmap));
	mp->size = 0;
    mp->hsize = KEY_SIZE;
    mp->em = (struct hmap_e **)calloc(KEY_SIZE, sizeof(struct hmap_e *));
	int i;
	for(i = 0; i < mp->hsize; i++){
		mp->em[i] = NULL;
    }
	return mp;
}

struct hmap *hmap_create_ws(int hsize)
{
	struct hmap *mp = (struct hmap *)malloc(sizeof(struct hmap));
	mp->size = 0;
    mp->hsize = hsize;
    mp->em = (struct hmap_e **)calloc(hsize, sizeof(struct hmap_e *));
	int i;
	for(i = 0; i < mp->hsize; i++){
		mp->em[i] = NULL;
    }
	return mp;
}

void hmap_print(struct hmap *mp)
{
	printf("to print :");
	printf("map size :%d \n", (int)mp->size);
    int i;
	for(i = 0; i < mp->hsize; i++)
	{
		struct hmap_e *e= mp->em[i];
		while (e)
		{
			printf("%s:%s ", e->key, (char *)e->value);
			e = e->next;
		}
	}
	printf("\n");
	
}

void hmap_destroy(struct hmap *mp, hfrcb fr)
{
	int i;
	for(i = 0; i < mp->hsize; i++){
		mp->size -= hmap_key_destroy(mp->em[i], fr);
    }
    free(mp->em);
	assert(mp->size == 0);
	free(mp);
}

//释放每个key下的element
int hmap_key_destroy(struct hmap_e *e, hfrcb fr)
{
	struct hmap_e *pos = e;
	struct hmap_e *next = pos;
	int count = 0;//计数器：返回free的元素个数
	while (pos)
	{	
		next = pos->next;
		free(pos->key);
        if (fr != NULL)
            (*fr)(pos->value);
        else
    		free(pos->value);
		free(pos);
		count++;
		pos = next;
	}
	return count;
}

static long hash_string(const char *key, int len)
{
	long h = 0;
	int i,off = 0;
	for(i = 0; i < len; i++)
		h = 33*h + key[off++];
	if(h < 0) 
		h *= -1;
	return h;
}

void hmap_put(struct hmap *mp, char *key, void * value)
{
    int key_len = strlen(key);
	assert(key && key_len>0);
    long hash = hash_string(key,key_len) % (mp->hsize);
//	printf(" put %s  %ld :%s \n " ,key,hash,(char *) value);
    //判断是否已经放入hashmap中，如果放入则替换内容
	struct hmap_e *e  = hmap_get_e(mp, key);
	if (e != NULL)
	{
		if (e->value != value)
		{
			free(key);
			free(e->value);
			e->value = value;
		} else {
			free(key);
			free(value);
		}
		return;
	}
    //没有放入，则添加
	e = hmap_e_create(key, value);
    //把新元素放到每个key队列的头部
    e->next = mp->em[hash];
    mp->em[hash] = e;
    mp->size++;
}

int hmap_put_wcb(struct hmap *mp, char *key, void *value, hfrcb fr)
{
//    hmap_print(mp);
    if (fr == NULL)
        return 0;
    int key_len = strlen(key);
	assert(key && key_len>0);
    long hash = hash_string(key,key_len) % (mp->hsize);
//	printf(" put %s  %ld :%s \n " ,key,hash,(char *) value);
    //判断是否已经放入hashmap中，如果放入则替换内容
	struct hmap_e *e  = hmap_get_e(mp, key);
	if (e != NULL)
	{   
        (*fr)(e->value);
        e->value = value;
		return 1;
	}
    //没有放入，则添加
	e = hmap_e_create(key, value);
    //把新元素放到每个key队列的头部
    e->next = mp->em[hash];
    mp->em[hash] = e;
    mp->size++;
  //  hmap_print(mp);
    return 1;
}

struct hmap_e *hmap_get_e(struct hmap *mp, char *key)
{
	int key_len = strlen(key);
	assert(key && key_len>0);
	long hash = hash_string(key,key_len) % (mp->hsize);
	struct hmap_e *e = mp->em[hash];
	while (e)
	{
		if(strcmp(key, e->key) == 0)
			return e;
        e = e->next;
	}
	return NULL;
}

void *hmap_get(struct hmap *mp, char *key)
{   
    struct hmap_e *e = hmap_get_e(mp, key);
    if (e != NULL)
        return e->value;
	return NULL;
}
/* function : 返回所有value组成的list,返回的value并没有复制，所以无需在外部
 *            释放
 * input    : hmap
 * output   : 所有value组成的list
 */
struct list *hmap_valuelist(struct hmap *mp) {
    struct list *list = list_create();
    int i;
    for (i = 0; i < mp->hsize; i++) {
        struct hmap_e *e = mp->em[i];
        while (e) {
            void *v = e->value;
            struct list_e *n = listnode_create(v);
            list_add(list, n);
            e = e->next;
        }
    } 
    return list;
}
//删除目标key的节点
struct hmap_e *hmap_del(struct hmap *mp, char *key)
{
	int key_len = strlen(key);
	assert(key && key_len>0);
	long hash = hash_string(key,key_len) % (mp->hsize);
	struct hmap_e *e = mp->em[hash];
    struct hmap_e *pre = hmap_e_create(NULL,NULL);
    pre->next = e;
	while (e)
	{
		if(strcmp(key, e->key) == 0){
            pre->next = e->next;
            mp->size--;
            e->next = NULL;
            free(pre);
            //如果此槽中只剩下一个元素，pre的方式会不能删除掉
            if (mp->em[hash] == e)
                mp->em[hash] = NULL;
            return e;
        }
        pre = e;
        e = e->next;
	}
    free(pre);
	return NULL;
}

void printHsize(struct hmap *mp){
   int i;
   for (i = 0; i < mp->hsize; i++){
       if (mp->em[i] == NULL)
           continue;
       struct hmap_e *e = mp->em[i];
       int count = 0;
       while (e){
           count++;
           e=e->next;
       }
       printf("em[%d]:%d  ", i, count);
       if (i % 50 == 0)
           printf("\n");
   } 
}

