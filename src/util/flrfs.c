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
    
    void *data = malloc(list->size * indexRsize);    
    void *flag = data; 
    struct list_e *e = list->head;
    while (e) {
        struct indexR *r = (struct indexR *)e->data;
        memcpy(flag,r->key,strlen(r->key));
        flag += strlen(r->key);
        memcpy(flag,&(r->filename),sizeof(int));
        flag += sizeof(int);
        memcpy(flag,&(r->offset),sizeof(long));
        flag += sizeof(long);
        memcpy(flag,&(r->flag),sizeof(short));
        flag += sizeof(short);
        e = e->next;
    }
    fwrite(data, (list->size) * indexRsize , 1, out);
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
    while (sizeflag < dsize){
        char *key = createstr(data, data + keysize + 1);
        pos += keysize;
        int *f = (int *)malloc(sizeof(int));
        memcpy(f, pos, sizeof(int));
        pos += sizeof(int);
        long *o = (long *)malloc(sizeof(long));
        memcpy(o, pos, sizeof(long));
        pos += sizeof(long);
        short *fl = (short *)malloc(sizeof(short));
        memcpy(fl, pos, sizeof(short));
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
    printf("%s %d %ld %d\n", r->key, r->filename, r->offset, r->flag);
}

int main(){
    printf("hello world,i am hp flrfs\n");
    int i;
    struct list *list = list_create();
    struct index index;
    index.list = list;
    index.size = 0;
    for(i = 0; i < 10000000; i++){
        struct indexR *r = (struct indexR *)malloc(sizeof(struct indexR));
        char *key = (char *)malloc(10);
        memcpy(key,"460.0.0.0\0",10);
        r->key = key;
        r->filename = 512;
        r->offset = 1024;
        r->flag = 1;
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
