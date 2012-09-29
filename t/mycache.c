/*
 * =====================================================================================
 *
 *       Filename:  mycache.c
 *
 *    Description:  测试mycache.c
 *
 *        Version:  1.0
 *        Created:  2012年09月29日 17时57分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  hepan 
 *   Organization:  tigerknows 
 *
 * =====================================================================================
 */
#include "../include/mycache.h"

int main()
{
    printf("hello world ,I am hp cache\n");
    while(1){
        struct mycache *cache = cache_create(10000,4);
        cache_destroy(cache,NULL);
    }
    getchar();
}

