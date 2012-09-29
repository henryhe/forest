/*
 * =====================================================================================
 *
 *       Filename:  hmap.c
 *
 *    Description:  测试hmap
 *
 *        Version:  1.0
 *        Created:  2012年09月29日 17时48分29秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  hepan
 *   Organization:  tigerknows 
 *
 * =====================================================================================
 */
#include "../include/list.h"
#include "../include/hashmap.h"

int ftestfree(void *data){
    free(data);
    return 0;
}

void main(){
	while(1)
	{
		struct hmap *mp = hmap_create_ws(1024);
		int i;
		for (i = 0; i < 10; i++)
		{
			char *p = (char *)malloc(2);
			sprintf(p,"%d",i);
			printf("p:%s\n",p);
			char *q = (char *)malloc(8);
			strcpy(q,"abcdefg\0");
//			free(p);
//			free(q);
			hmap_put(mp, p, q);
			hmap_print(mp);
		}
		hmap_destroy(mp,ftestfree);
//		hmap_destroy_cb(mp,test_free);
	}
}


