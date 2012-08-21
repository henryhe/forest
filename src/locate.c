/*
 * =====================================================================================
 *
 *       Filename:  locate.c
 *
 *    Description:  locate中的共用函数
 *
 *        Version:  1.0
 *        Created:  2012年08月21日 18时12分53秒
 *       Revision:  1.0 
 *       Compiler:  gcc
 *
 *         Author:  hepan 
 *   Organization:  tigerknows 
 *
 * =====================================================================================
 */
#include "../include/locate.h"

void freeR(struct record *r)
{
    free(r->key);
    free(r->e);
    free(r);
}
