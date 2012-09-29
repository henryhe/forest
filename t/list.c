/*
 * =====================================================================================
 *
 *       Filename:  list.c
 *
 *    Description:  测试list
 *
 *        Version:  1.0
 *        Created:  2012年09月29日 17时54分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  hepan 
 *   Organization:  tigerknows 
 *
 * =====================================================================================
 */
#include "../include/list.h"

void ltestfree(void *data)
{
    free(data);
}

int main(){
	while(1){
		struct list* list = list_create();
		int i;
		for( i = 0; i < 10; i++)
		{
			char *p = (char *) malloc(10);
			strcpy(p,"123456789\0");
			struct list_e *e = listnode_create(p);
			list_add(list, e);
		}
		list_print(list);
		list_destroy(list, NULL);
	}
}

