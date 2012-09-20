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

int saveindex(struct list *list, char *path){
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
        memcpy(flag, r->key,FKEY_SIZE);
        flag += FKEY_SIZE;
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
    return 1;
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
        struct indexR *r = (struct indexR *)malloc(sizeof(struct indexR));
        char *key = createstr(pos, pos + FKEY_SIZE);
        r->key = key;
        pos += FKEY_SIZE;
        int size = sizeof(int);
        memcpy(&(r->filename), pos, size);
        pos += size;
        memcpy(&(r->offset), pos, size);
        pos += size;
        size = sizeof(char);
        memcpy(&(r->flag), pos, size);
        pos += size;
        struct list_e *e = listnode_create(r);
        list_add(index->list, e);
        sizeflag += indexRsize;
    }
    index->size += dsize;
}



struct index *loadindex(char *path){
    FILE *in = fopen(path,"rb");
    struct index *index = createindex();
    index->path = path;
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
    free(index->path);
    free(index);
}


/* 
 * function ： 获得key的在index的信息，如果有返回key的indexR，
 *             如果不存在返回前一个，所以调用的时候获得返回结果需要比较
 * input    :  目标key, index
 * output   :  目标key对应的indexR，或者目标key应该插入位置前边的indexR
 * */
struct list_e *locatekey(char *key, struct index *index){
    int size = index->list->size;
    if(index->list->head == NULL)
       return NULL;
//    struct indexR *start = (struct indexR *)index->list->head;
//    struct indexR *end = (struct indexR *)index->list->tail;
    int start = 0;
    int end = size - 1;
    int mid = 0;
    while (TRUE){
        mid = (start + end) / 2;
        struct list_e *me  = list_get(index->list, mid);
        struct indexR *mR = (struct indexR *)me->data; 
        if (strcmp(key, mR->key) == 0)
            return me;
        if (start == mid){//如果区间最后只包含两个元素，如果不是在list的尾巴，即可确定返回
            struct list_e *ne = me->next;
            if (ne == NULL)
                return me;
            struct indexR *nR = ne->data;
            if (strcmp(key,nR->key) >= 0)
                return ne;
            else
                return me;
        }
        if (strcmp(key, mR->key) > 0){
            start = mid;
            continue;
        }else{
            end = mid;
            continue;
        }    
    }
}

void addtoindex(struct list_e *pre, struct indexR *r){
    struct list_e *new = listnode_create(r);
    struct list_e *next = pre->next;
    pre->next = new;
    new->next = next;
}

void printindexR(struct indexR *r){
    printf("%s %d %d %c\n", r->key, r->filename, r->offset, r->flag);
}

/* 
 * function : 根据flr的path和int型的文件名，生成读取文件的路径
 * input    : flr的path和int型文件名
 * output   ：读取文件的路径  
 */
char *getfilepath(char *flrpath, int filename){
    char *fn = myitoa(filename);
    char *path = mystrcat(flrpath, fn);
    free(fn);
    free(path);
    return path;

}

/* 
 * function : 将传入的数据扩展到定长的长度
 * input    ：传入的数据，应该是包括了所有信息的2进制流，
 *            只需要在末尾加上结尾的符号 
 * output   : 合适长度的data
 */
void *expanddata(int length, void *data){
    int size = sizeof(data);
    if (length == size)
        return data;
    void *res = malloc(length);
    memcpy(res, data, size);
    void *pos = res + size;
    memset(pos, flrtail, length -size);
    free(data);
    return res;
}

/* 
 * function : 读取输入key的已有数据
 * intput   : flr文件存放路径，对应的index，目标key
 * output   ：包含key和data域的fR结构(data中未包含key)
 */
struct fR *flr_read(char *flrpath, struct index *index, char *key){
    struct indexR *ir = (struct indexR *)(locatekey(key, index)->data);
    if (strcmp(key, ir->key) != 0)//文件系统中没有此key的数据
        return NULL;
    char *path = getfilepath(flrpath, ir->filename);
    FILE *f = fopen(path, "rb+");
    fseek(f, ir->offset, SEEK_SET);
    void *raw = malloc(ir->filename);
    fread(raw, ir->filename, 1, f);
    fseek(f, ir->offset + ir->filename - 1, SEEK_SET);
    char flag = DEAD;
    fwrite(&flag, 1, 1, f);
    free(path);
    fclose(f);
    int size = 0;
    void *pos = raw;
    while (size < ir->filename){
        if (*(char *)pos == flrtail){
            void *re = malloc(size + 1);
            memcpy(re, raw + FKEY_SIZE, size + 1);
            free(raw);
            return re;
        }
        size++;
    }
    //结尾不是约定的字符，不返回结果
    return NULL;
}

/* 
 * function : 写入输入key的传入数据
 * intput   :flr文件存放路径，对应的index，需要写入的key，
 *           需要写入的data数据（包含了key，是可以直接写入的data）
 * output   ：包含key和data域的fR结构
 */
int flr_write(char *flrpath, struct index *index, char *key, void *data){
    int size = sizeof(data);
    struct list_e *tar  = locatekey(key, index);
    char *tarkey = ((struct indexR *)(tar->data))->key;
    int filename = getfilename(size);
    char *path = getfilepath(flrpath, filename);
    data = expanddata(filename, data);
    if (strcmp(tarkey, key) != 0){
        //创建新的index节点
        struct indexR *ir = (struct indexR *)malloc(sizeof(struct indexR));
        ir->key = key;
        ir->filename = filename;
        ir->flag = AVAI;
        //写入文件系统
        FILE *fp = fopen(path, "rb+");
        fseek(fp, 0L, SEEK_END);
        ir->offset = ftell(fp);
        fwrite(data, filename, 1, fp);
        fclose(fp);
        struct list_e *e = listnode_create(ir);
        struct list_e *n = tar->next;
        tar->next = e;
        e->next = n;      
    }else{
        struct indexR *ir = (struct indexR *)tar->data;
        //更新index
        FILE *fp = fopen(path, "rb+");
        if (filename == ir->filename){//还在原来的flr文件
            fseek(fp, ir->offset, SEEK_SET);
            fwrite(data, filename, 1, fp);
        }else{//移动到其他文件
            ir->filename = filename;
            fseek(fp, 0L, SEEK_END);
            ir->offset = ftell(fp);
            fwrite(data, filename, 1, fp);
        }
        fclose(fp);
    }
    free(path);
    //写入文件
}

/* 
 * function : 根据传入的size得到正确的文件名
 * input    : 记录所占空间
 * output   : 正确的文件名字 
 */
int getfilename(int size){
    int i;
    for (i = 0; i < FILENUM; i++){
        if( size <= fnames[i])
            return fnames[i];
    }
    return fnames[FILENUM -1];
}

int fmain(){
    printf("hello world,i am hp flrfs\n");
    int i;
    struct list *list = list_create();
    struct index index;
    index.list = list;
    index.size = 0;
    for(i = 0; i < readRnum; i++){
        struct indexR *r = (struct indexR *)malloc(sizeof(struct indexR));
        char *key = (char *)malloc(FKEY_SIZE);
        memcpy(key,"460.0.123.3",FKEY_SIZE);
        r->key = key;
        r->filename = 51200000;
        r->offset = 10241024;
        r->flag = '1';
        struct list_e *e = listnode_create(r);
        list_add(index.list,e);
    }
    printf("start to write.\n");
//    printf("time %s\n", getnowtime());
    saveindex(index.list, "temp");
//    printf("time %s\n",getnowtime());
    list_destroy(index.list,freeindexR);
    printf("write end.\n");
    struct index *index1 = loadindex("temp");
    struct list_e * e = index1->list->head;
    while (e){
        struct indexR *r = (struct indexR *)e->data;
        printindexR(r);
        e = e->next;
    }
    freeindex(index1);
}
