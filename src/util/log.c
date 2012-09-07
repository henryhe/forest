/*
 * =====================================================================================
 *
 *       Filename:  log.c
 *
 *    Description:  用于输出日志
 *
 *        Version:  1.0
 *        Created:  2012年09月06日 11时44分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  hepan 
 *   Organization:  tigerknows 
 *
 * =====================================================================================
 */
#include "../../include/log.h"

static void log_info(char *s){
    printf("%s [INFO] : %s",getnowtime(),s);
}

void main(){
    printf("hp log\n");
    log_info("hahahah\n");
}

