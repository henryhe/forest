/*
 * =====================================================================================
 *
 *       Filename:  flrfs.c
 *
 *    Description:  定长记录文件系统,每个文件的名字为文件内记录最大长度
 *
 *        Version:  1.0
 *        Created:  2012年08月28日 15时01分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  hepan 
 *   Organization:  tigerknows 
 *
 * =====================================================================================
 */
#include "../../include/flrfs.h"

int writeindex(struct list *list, char *path){
    FILE *out = fopen(path, "wb");
    if (out == NULL){
        printf("[ERROR]: writeindex error, can not open %s\n", path);
        exit(0);
    }
    
    void *data = malloc(readRnum * indexRsize);    
    void *flag = data; 
    struct list_e *e = list->head;
    while (e) {
        struct indexR *r = (struct indexR *)e->data;
        memcpy(flag, r->key, keysize);
        flag += keysize;
        memcpy(flag, &(r->filename),sizeof(int));
        flag += sizeof(int);
        memcpy(flag, &(r->offset),sizeof(int));
        flag += sizeof(int);
        memcpy(flag, &(r->flag),sizeof(char));
        flag += sizeof(char);
        e = e->next;
    }
    fwrite(data, readRnum * indexRsize , 1, out);
    free(data);
    fclose(out);
}

struct index *createindex(){
    struct index *index = (struct index *)malloc(sizeof(struct index));
    index->list = NULL;
    index->list = list_create(); 
    return index;
}

struct indexR *createindexR(char *key, int filename, long offset, short flag){
    struct indexR *r = (struct indexR *)malloc(sizeof(struct indexR));
    r->key = key;
    r->filename = filename;
    r->offset = offset;
    r->flag = flag;
    return r;
}

void datatoindex(struct index *index, void *data, long dsize){
    long sizeflag = 0;
    void *pos = data;
    while (sizeflag < indexRsize * readRnum){
        char *key = createstr(pos, pos + keysize);
        pos += keysize ;
        int *f = (int *)malloc(sizeof(int));
        memcpy(f, pos, sizeof(int));
        pos += sizeof(int);
        int *o = (int *)malloc(sizeof(int));
        memcpy(o, pos, sizeof(int));
        pos += sizeof(int);
        char *fl = (char *)malloc(sizeof(char));
        memcpy(fl, pos, sizeof(char));
        pos += sizeof(char);
        struct indexR *r = createindexR(key, *f, *o, *fl);
        struct list_e *e = listnode_create(r);
        list_add(index->list, e);
        free(f);
        free(o);
        free(fl);
        sizeflag += indexRsize;
    }
    index->size += dsize;
}



struct index *readindex(char *path){
    FILE *in = fopen(path,"rb");
    struct index *index = createindex();
    void *data = malloc(readRnum * indexRsize);
    while (TRUE){
        int readsize = fread(data, readRnum * indexRsize, 1, in);
        datatoindex(index, data, readsize); 
        if(readsize < readRnum * indexRsize)
            break;
    }
    free(data);
    fclose(in);
    return index;
    
}


void freeindexR(struct indexR *r){
    free(r->key);
    free(r);
}

void freeindex(struct index *index){
    list_destroy(index->list, freeindexR);
    free(index);
}

char *getnowtime()
{
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    return asctime(timeinfo);
}

void printindexR(struct indexR *r){
//    printf("%s %d %d %c\n", r->key, r->filename, r->offset, r->flag);
}

int main(){
    printf("hello world,i am hp flrfs\n");
    int i;
    struct list *list = list_create();
    struct index index;
    index.list = list;
    index.size = 0;
    for(i = 0; i < readRnum; i++){
        struct indexR *r = (struct indexR *)malloc(sizeof(struct indexR));
        char *key = (char *)malloc(keysize);
        memcpy(key,"460.0.123.3",keysize);
        r->key = key;
        r->filename = 51200000;
        r->offset = 10241024;
        r->flag = '1';
        struct list_e *e = listnode_create(r);
        list_add(index.list,e);
    }
    printf("start to write.\n");
    printf("time %s\n",getnowtime());
    writeindex(index.list, "temp");
    printf("time %s\n",getnowtime());
    list_destroy(index.list,freeindexR);
    printf("write end.\n");
    struct index *index1 = readindex("temp");
    struct list_e * e = index1->list->head;
    while (e){
        struct indexR *r = (struct indexR *)e->data;
        printindexR(r);
        e = e->next;
    }
    freeindex(index1);
}
