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
    struct hasmap mp;
    long msize;
}
