/*
 * =====================================================================================
 *
 *       Filename:  config.c
 *
 *    Description:  处理配置
 *
 *        Version:  1.0
 *        Created:  2012年09月14日 18时04分49秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  hepan
 *   Organization:  
 *
 * =====================================================================================
 */
#include "../../include/config.h"

struct config *loadconfig(){
    char *path = (char *)malloc(17);
    memcpy(path, "../config/config\0" ,17);
    char *line = (char *)malloc(1024);
    FILE *f = fopen(path, "r");
    struct config *config = (struct config *)malloc(sizeof(struct config));
    while (fgets(line, 1024, f) != NULL){
        char *start = line;
        char *end = start;
        char *key;
        char *value;
        while (TRUE){
            if (*end == '\n'){
                value = createstr(start, end);
                break;
            }
            
            if (*end == '='){
                key = createstr(start, end);
                start = ++end;
                continue;
            }
            end++;
       }
       if(strcmp(key, FLRPATH) == 0)
           config->FLRPATH = value;
       free(key);
       free(path);
       free(line);
       return config;
   }
}

void config_destroy(struct config *config){
    free(config->FLRPATH);
    free(config);
}

void comain(){
    struct config *config = loadconfig();
    printf("%s\n",config->FLRPATH);
    config_destroy(config);
}
