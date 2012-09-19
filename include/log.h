/*
 * =====================================================================================
 *
 *       Filename:  log.h
 *
 *    Description:  日志文件头文件
 *
 *        Version:  1.0
 *        Created:  2012年09月06日 11时47分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  hepan
 *   Organization:  tigerknows 
 *
 * =====================================================================================
 */
#include "common.h"

#include "myUtil.h"

static int log_level = 0;

extern void log_error(char *s);

//extern static void log_debug(char *s);

extern void log_info(char *s);
