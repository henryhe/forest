/*
 * =====================================================================================
 *
 *       Filename:  writelog.c
 *
 *    Description:  存储解析好的基站位置信息    
 *
 *        Version:  1.0
 *        Created:  2012年08月16日 11时15分02秒
 *       Revision:  1.0
 *       Compiler:  gcc
 *
 *         Author:  hepan
 *   Organization:  tigerknows
 *
 * =====================================================================================
 */
#include "../include/locate.h"

int rdindex()
{
    return 0;
}

void windex() 
{

}

struct keydata *rdKdata(struct record *r)
{
    return NULL;
}


void wkeydata()
{
    

}

void rddata()
{

}

void wedata()
{

}

void mvkey()
{

}

void cache_put(struct keydata *kd)
{
    //TODO index,读写文件的实现
}

struct keydata *cache_get(struct record *r)
{
    return NULL;
}

struct record *stoR(char *s)
{   
    struct record *r;
    return r;
}

void prodata()
{
    char *line = (char *)malloc(READ_BUFFER_SIZE);
    while (fgets(line, READ_BUFFER_SIZE, stdin) != NULL)
    {
        printf("%s",line);
        struct record *r = stoR(line);
        if (r == NULL)
            continue;
        struct keydata *kd = cache_get(r);
        if (kd == NULL)
        {
            kd = rdKdata(r);
            cache_put(kd);
        }
//        list_add(kd->key);
//        freeK(kd);
//        freeR(r);
    }
    free(line);
}

int main()
{   
    prodata(); 
    return 1;
}

