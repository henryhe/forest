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

int reindex()
{
    return 0;
}

void windex() 
{

}

struct kdata *rkeydata(struct record *r)
{
    return NULL;
}


void wkeydata()
{
    

}

void redata()
{

}

void wedata()
{

}

void mvkey()
{

}

struct record *ltoR(char *)
{   
    struct record *r;
    return r;
}

int main(){
    printf("hello world\n");
    char *line = (char *)malloc(READ_BUFFER_SIZE);
    while (fgets(line, READ_BUFFER_SIZE, stdin) != NULL)
    {
        printf("%s",line);
        struct *r = stoR(line);
        struct *kdata = rkeydata(r);//TODO free
        freeR(r);

    }
    free(line);
    return 1;
}

