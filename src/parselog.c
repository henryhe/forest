 /*
  * =====================================================================================
  *
  *       Filename:  parselog.c
  *
  *    Description:  存储解析好的基站位置信息,*用于接受标准输入的日志，然后得到提交的数据，
  *                  解析为各行都是有效数据的格式，格式如下:
  *                  基站（wifi）号 采集时间  x坐标  y坐标 pt  获取类型
  *
  *        Version:  1.0
  *        Created:  2012年07月20日 13时16分02秒
  *       Revision:  1.0
  *       Compiler:  gcc
  *
  *         Author:  hepan
  *   Organization:  tigerknows
  *
  * =====================================================================================
 */
#include "../include/locate.h"

//记录统计信息
static struct stat s;

/**
  * 返回日志来源，包含“&_x=”的日志判定为来源于定位，加上‘&’是因为cormorant日志里有些日志
  * 包含“lu=”的即判定为cormorant行为日志来的信息
  * 返回值为0，1，2，0为无效数据，1为tk的日志，2为cor的日志
  */
int logtype(char *line) 
{
	char *p = line;
	while (*p != '\0') 
	{	
		//来自tk的日志
		if (*p == '&' && *(p + 1) == '_' && *(p + 2) == 'x' && *(p + 3) == '=')
			return 1;
		//来自corm的日志
		if (*p == 'l' && *(p + 1) == 'u' && *(p + 2) == '=')
			return 2;
		p++;
	}
	return 0;
}

/*
 * 根据两个指针之间的内容，生成新的字符串，新申请的空间，需要free
 * 注：start指向需要内容的第一个字符，end指向末尾字符的后边一个空间
 */
char *createstr(char *start, char *end)
{
    int len = end - start ;
    char *result  = (char *)malloc(len + 1);
    memcpy(result, start, len);
    *(result + len) = '\0';
    return result;
}

/* 
 * function：判断kye的类型
 * args：char *key
 * return:代表类型的字符，b:基站；w:wifi
 * */
char getkeytype(char *key)
{
    char *p = key;
    while (TRUE)
    {
        if (*p == '\0')
            break;
        if (*p == '.')
            return 'b';
        if (*p == ':')
            return 'w';
        p++;
    }
    return '0';
}

/* 
 * function:截取一个字符串的子串，转化为int
 * args:char *ori 原始字符串，int from 字串起始位置，int to 字串结束位置
 * return:对应的整数
 * */
int getint(char *ori, int from , int to)
{	
    int n = to - from + 1;
	ori += from;
	char* temp = (char*)malloc(n+1);
	memcpy(temp, ori, n);
	*(temp + n) = '\0';
	int result = atoi(temp);
	free(temp);
	return result;
}

//将格式如2012-07-15 00:00:00的字符串转化为1900年到该时间的秒数
long gettime( char* line )
{
	char *timeinfo = (char *)malloc(20);
	memcpy(timeinfo,line,19);
    *(timeinfo + 19) = '\0';
	struct tm t;
    t.tm_year = getint(timeinfo, 0, 3) - 1900 ;
	t.tm_mon  = getint(timeinfo, 5, 6) - 1;
	t.tm_mday  = getint(timeinfo, 8, 9);
	t.tm_hour = getint(timeinfo, 11, 12);
	t.tm_min  = getint(timeinfo, 14, 15);
	t.tm_sec  = getint(timeinfo, 17, 18);
	t.tm_isdst = 0;
	free(timeinfo);
	time_t timep = mktime(&t);
	return timep;
}

void dealpara(struct log *log, char *key, char *value )
{
	if (strcmp(key, "wifi_mac[]\0") == 0)
    {
		struct list_e *e = listnode_create(value);
		list_add(log->wifikeylist, e);
        free(key);
    } else if (strcmp(key, "wifi_ss[]\0") == 0)
            {
				struct list_e *e = listnode_create(value);
				list_add(log->wifisslist, e);
				free(key);
            } else if (strcmp(key, "n8b_lac[]\0") == 0)
                    {
						struct list_e *e = listnode_create(value);
						list_add(log->n8blaclist, e);
						free(key);
                    } else if (strcmp(key, "n8b_ci[]\0") == 0)
                            {
                                struct list_e *e = listnode_create(value);
                                list_add(log->n8bcilist, e);
                                free(key);
                            }else if (strcmp(key, "n8b_ss[]\0") == 0)
                                   {
                                        struct list_e *e = listnode_create(value);
                                        list_add(log->n8bsslist, e);
                                        free(key);
                                   }else{
											hmap_put(log->paramp, key, value);
                                        }
}

void freelog(struct log *log)
{
	hmap_destroy(log->paramp);
	list_destroy(log->wifikeylist);
	list_destroy(log->wifisslist);
	list_destroy(log->n8blaclist);
	list_destroy(log->n8bcilist);
	list_destroy(log->n8bsslist);
	free(log);
}

//初始化log结构
struct log *log_init(){
	struct log* log = (struct log *)malloc(sizeof(struct log));
	log->paramp = hmap_create();
	log->wifikeylist = list_create();
	log->wifisslist = list_create();
	log->n8blaclist = list_create();
	log->n8bcilist = list_create();
	log->n8bsslist = list_create();
    return log;
}


struct log *tklog( char *line )
{
	struct log* log = log_init();
	log->time = gettime(line);
	//获取日志中大括号内的参数
	//用start和end两个指针查找key和value
	char *start = line + 32;//略过时间的部分
	while ( *(start-1) != '{' ) start ++;
	char *end = start;
	char *key,*value;
	int haskey = 0;
	while (TRUE)
	{
		if ( *end == '=')
        {
			//某些特殊情况洗下，参数的值中有=
			if( *(start - 1) == '=' )
			{
				end++;
				continue;
			}
            int len = end - start;
            key = (char *)malloc(len + 1);
            memcpy(key , start, len);
            *(key + len) = '\0';
            start = ++end;
			haskey = 1;
            continue;
        }

		if (*end == '&')
        {
			//某些特殊情况洗下，参数的值中有&
			if(haskey == 0)
			{
				start = ++ end;
				continue;
			}
            int len = end -start;
            value = ( char *)malloc(len + 1);
            memcpy(value , start, len);
            * (value + len) = '\0';
			dealpara(log, key, value);
			haskey = 0;
            start = ++end;
            continue;
        }

        if(*end == '}')
        {
            int len = end -start;
            value = ( char *)malloc(len + 1);
            memcpy(value , start , len);
            *(value + len) = '\0';
     	    dealpara(log, key, value);
            break;
        }
		end++;
	}
	return log;
}


struct log *corlog(char *line)
{
	//初始化log结构,cormorant的日志没有5个list，初始化只是为了统一free
	struct log *log = log_init();
	//由于cormorant的日志时间信息年份没有前两位，为了和tk的日志一致处理，此处添加上
	char *timeline = (char *)malloc(24);
	*timeline = '2';
	*(timeline + 1)= '0';
	memcpy(timeline + 2, line, 21);
	*(timeline + 23) = '\0';

	log->time = gettime ( timeline );
	free(timeline);
	//获取日志中大括号内的参数
	//用start和end两个指针查找key和value
	char *start = line + 32;//略过时间的部分
	while (*(start-1) != '{') start ++;
	char *end = start;
	char *key,*value;
	int haskey = 0;
	//printf("%s\n",line);
	while (TRUE)
	{
		if (*end == '=')
        {
			//某些特殊情况洗下，参数的值中有=
			if(*(start - 1) == '=')
			{
				end++;
				continue;
			}
            int len = end - start;
            key = (char *)malloc(len + 1);
            memcpy(key , start, len);
            *(key + len) = '\0';
            start = ++end;
			haskey = 1;
            continue;
        }

		if (*end == ' ' && *(end - 1) == ',')
        {
			//某些特殊情况洗下，参数的值中有&
			if(haskey == 0)
			{
				start = ++ end;
				continue;
			}
            int len = end - start -1;//此处-1是因为cor日志的分隔符号为逗号+空格
            value = (char *)malloc(len + 1);
            memcpy(value, start, len);
            *(value + len) = '\0';
			dealpara(log, key, value);
			haskey = 0;
            start = ++end;
            continue;
        }

		if( *end == '}' )
        {
            int len = end -start;
            value = (char *)malloc(len + 1);
            memcpy(value , start , len);
            *(value + len) = '\0';
			//由于有很小几率日志中有乱码导致解析失败
			int templen = strlen(value);
			if(templen != len)
			{
				freelog(log);
				return NULL;
			}
     	    dealpara(log, key, value);
            break;
        }
		end++;
	}
	return log;
}

int isnum(char *s){
    int count = 0;
    while (*s != '\0')
    {
        if (*s == '.' || *s == ' ')
        {
            s++;
            continue;
        }

        if( *s == '0' || *s == '1' || *s == '2' || *s == '3' ||
            *s == '4' || *s == '5' || *s == '6' || *s == '7' || 
            *s == '8' || *s == '9')
           {
            s++;
            count ++;
            continue;
           }
        s++;
     }
    if (count > 0 )
        return TRUE;
    return FALSE;
}

struct log* parseline(char *line, int logtype)
{	
	if (logtype == 1)
		return tklog(line);
	if (logtype == 2) 
		return corlog(line);
	return NULL;
}

//根据vs过滤如果是安卓版本，且低于2.30.20110526A版本的被过滤掉
int vsfilted(char *vs)
{
    if (vs == NULL)
        return FALSE;
    int len = strlen(vs);
    if (*(vs + len - 1) != 'A')
        return FALSE;
    if (strcmp(vs,"2.30.20110526A") == -1)
        return TRUE;
    return FALSE;
}

//根据m的值去除掉测试数据
int mfilted(char *m){
    if (m == NULL)
        return FALSE;
    if (strcmp(m,"TigerMapTest") == 0 || strcmp(m,"chomsky") == 0)
        return TRUE;
    return FALSE;
}
//根据公共参数过滤日志
int pubfilted(char *vs, char *m)
{
    if (vsfilted(vs) || mfilted(m))    
        return TRUE;
}


//根据输入的mcc，mnc等拼接基站id，不做任何其他处理，所以mnc = 01
//的情况需要提前去掉0
char *catkey(char *mcc, char *mnc, char *lac, char *ci)
{
    int mcc_len = strlen(mcc);
    int mnc_len = strlen(mnc);
    int lac_len = strlen(lac);
    int ci_len = strlen(ci);
    char *key = (char *)malloc(mcc_len + mnc_len + lac_len + ci_len + 3 + 1);
    int offset = 0;
    memcpy(key,mcc,mcc_len);
    offset += mcc_len;
    *(key + offset) = '.';
    offset++;
    memcpy(key + offset,mnc,mnc_len);
    offset += mnc_len;
    *(key + offset) = '.';
    offset++;
    memcpy(key + offset,lac,lac_len);
    offset += lac_len;
    *(key + offset) = '.';
    offset++;
    memcpy(key + offset,ci,ci_len);
    offset += ci_len;
    *(key + offset) = '\0';
    return key;
}

/**计算basekey，如果参数不全，或者不符合日志要求，返回NULL
 * 返回的basekey是新申请的空间需要free
 */
char *getbkey(char *mcc, char *mnc, char *lac, char *ci, char *vs, char *m)
{
    if (mcc == NULL || mnc == NULL || lac == NULL || ci == NULL)
        return NULL;
    int i_mcc = atoi(mcc);
    int i_mnc = atoi(mnc);
    int i_lac = atoi(lac);
    int i_ci = atoi(ci);
    if( i_mcc <= 0 || i_mnc < 0 || i_lac <= 0 || i_ci <= 0 
        || i_lac == 65535 || i_ci == 65535)
       return NULL;
    //此处的最大值只是遵从原来的代码，获取java中short和int的最大值
    if( i_mcc > 32767 || i_mnc > 32767 || i_lac > 2147483647 || i_ci > 2147483647)
        return NULL;
    int vslen = strlen(vs);
    if(i_mcc == 460 && (i_mnc == 3 || i_mnc == 5) && *(vs + vslen -1) == 'A' 
       &&  (strcmp(vs,"3.02.20120325A") == -1 ||(strcmp(vs,"3.03.20120329A")==0 && strcmp(m,"ANDqQFUnusual-SF") == 0)))
        return NULL;
    //拼接基站key
    //由于mnc提交上来的参数有些有0所以重新生成
    char *f_mnc = (char *)malloc(strlen(mnc));
    sprintf(f_mnc,"%d",i_mnc);
    char *basekey = catkey(mcc,f_mnc,lac,ci);
    free(f_mnc);
    return basekey;
}

/**拼接计算周边基站的key，由于主基站已经坐过基本参数的判断，所以
 *此处只需要判断lac和ci是否合法，然后返回key，返回的key是重新申请
 *的空间，需要free
 */
char *getnkey(char *mcc, char *mnc, char *lac, char *ci)
{
    int i_mnc = atoi(mnc);
    int i_lac = atoi(lac);
    int i_ci = atoi(ci);
    if ( i_lac <= 0 || i_ci <= 0 || i_lac == 65536 || 
         i_ci == 65536 || i_lac > 2147483647 || i_ci > 2147483647)
        return NULL;
    char *f_mnc = (char *)malloc(strlen(mnc));
    sprintf(f_mnc,"%d",i_mnc);
    char *nkey = catkey(mcc,f_mnc,lac,ci);
    free(f_mnc);
    return nkey;

}

/**检查wifi的key是否合法
 *
 */
int iswkey(char *key)
{
    if (strlen(key) != 17)
        return FALSE;
    if (strcmp(key,"00:00:00:00:00:00") == 0 || strcmp(key,"ff:ff:ff:ff:ff:ff") == 0)
        return FALSE;
    return TRUE;
}

//根据gps信息判断是否要过滤掉
int gpsfilted(double x,double y,double p){
    if (x >= 180.0 || x <= -180.0 || y >= 90.0 || y <= -90.0 || p > 500)
         return TRUE;
    if (x > -0.000001 && x < 0.000001)
         return TRUE;
    if (y > -0.000001 && y < 0.000001)
          return TRUE;
    return FALSE;
}

void printne(struct list *list)
{
    if (list->size < 2)
        return;
    printf("ne ");
    list_print(list);
}

void printR(struct record r){
    s.keynum++;
    printf("%s ", r.key);
    printf("%ld ", r.time);
    printf("%lf %lf %lf ", r.x, r.y, r.p);
    printf("%d ", r.type);
    printf("%s\n", r.e);
}

//得到来自tk日志的结果
void dealtklog(char *basekey,  char *mcc, char *mnc, struct log *log)
{
    char *_x = hmap_get(log->paramp,"_x");
    char *_y = hmap_get(log->paramp,"_y");
    char *_p = hmap_get(log->paramp,"_pt");
    char *_e = hmap_get(log->paramp,"e");
    if(_x == NULL || _y == NULL || _p == NULL || _e == NULL)
        return;
    if(!isnum(_x) || !isnum(_y) || !isnum(_p) )
        return;
    double x = strtod(_x,NULL);
    double y = strtod(_y,NULL);
    double p = strtod(_p,NULL);
    if (gpsfilted(x,y,p))
        return;
    char *e = (char *)malloc(sizeof(_e) + 1);
    strcpy(e, _e);
    //初始化一个record结构,放在栈中不许要free
    struct record r;
    r.time = log->time;
    r.x = x;
    r.y = y;
    r.p = p;
    r.e = e;
    //保存周边信息的list
    struct list* nelist = list_create();
    char *nekey;
    //如果有合法的basekey，则处理主基站和其他基站
    if (basekey != NULL)
    {
        r.key = basekey;
        r.type = 1;
        printR(r);
        //周边关系的主key
        nekey = createstr(r.key, r.key + strlen(r.key));
        struct list_e *e = listnode_create(nekey);
        list_add(nelist,e);
        //主基站处理完毕，扫描周边基站
        if (log->n8blaclist->size == log->n8bcilist->size)
        {
            int i;
            for (i = 0; i< log->n8blaclist->size; i++)
            {   //nkey是新拼接的所以学要在出for的时候free
                char *lac = (char*) (list_get(log->n8blaclist,i))->data;
                char *ci = (char*) (list_get(log->n8bcilist,i))->data;
                char *nkey = getnkey(mcc,mnc,lac,ci);
                if (nkey == NULL)
                    continue;
                r.key = nkey;
                r.type = 2;
                printR(r);
                nekey = createstr(r.key, r.key + strlen(r.key));
                struct list_e *e = listnode_create(nekey);
                list_add(nelist,e);
                free(nkey);
             }
        }
    } 
    //扫描wifi
    int i;
    for (i = 0; i < log->wifikeylist->size; i++)
    {   //wifikey是参数map中的指针，会在map中free掉
        char *wifikey = (char *)(list_get(log->wifikeylist, i))->data;
        if (!iswkey(wifikey))
            continue;
        r.key = wifikey;
        r.type = 0;
        s.wkeynum++;
        printR(r);
        nekey = createstr(r.key, r.key + strlen(r.key));
        struct list_e *e = listnode_create(nekey);
        list_add(nelist,e);
    }
    printne(nelist);
    list_destroy(nelist);
    free(e); 
}

/* 根据传入的基站key，做过滤，去掉多余的0，返回可用的key */
char *getbkeyfs(char *ori, char *vs, char *m)
{
    char *mcc = NULL;
    char *mnc = NULL;
    char *lac = NULL;
    char *ci = NULL;
    char *start = ori;
    char *end = start;
    int varflag;
    while (TRUE)
    {
        if (*end == '\0')
        {   
            if (varflag != 3)
            {
                if (mcc != NULL)
                    free(mcc);
                if (mnc != NULL)
                    free(mnc);
                if (lac != NULL)
                    free(lac);
                return NULL;
            }
            ci = createstr(start, end);
            char *bkey = getbkey(mcc, mnc, lac, ci, vs, m);
            free(mcc);
            free(mnc);
            free(lac);
            free(ci);
            return bkey;
        }
        if (*end == '.')
        {
            if (varflag == 0)
                mcc = createstr(start, end);
            if (varflag == 1)
                mnc = createstr(start, end);
            if(varflag == 2)
                lac = createstr(start, end);
            varflag++;
            start = ++end;
            continue;
        }
        end++;
    }

}

struct list *getkeylist(char *data, char *vs, char *m)
{
    struct list *list = list_create();
    int num = 0;
    char *start = data;
    char *end = start;
    while (TRUE)
    {
        if (*end == '\0')
            break;
        if (*end == '@')
        {
            char *key = createstr(start, end);
            char t = getkeytype(key);
            if (t == 'b'){
                char *bkey = getbkeyfs(key, vs, m);
                if(bkey != NULL)
                {
                    struct list_e *e = listnode_create(bkey);
                    list_add(list,e);
                }              
            }else if (t == 'w')
                {
                    char *wkey = (char*)malloc(strlen(key) + 1);
                    strcpy(wkey, key);
                    if(iswkey(wkey))
                    {
                        struct list_e *e = listnode_create(wkey);
                        list_add(list,e);
                    }else{
                        free(wkey);
                    }
                }
            free(key);
            start = ++end;
            continue;
        }
        if (*end == ';')
        {
            start = ++end;
            continue;
        }
        end++;
    }
    return list;
}

void dealcorlog(char *vs, char *m, struct log *log )
{
    char *lu = hmap_get(log->paramp, "lu");
    char *e = hmap_get(log->paramp, "e");
    char *start  = lu;
    char *end = lu;
    int varflag = 0;//标记一条提交信息参数读取状态，按参数顺序赋值，如0为即将读取时间，1为即将读取y
    int gpsflag = 0;//0标记gps数据没有，或者不合法，1表示正常
    struct record r;
    r.e = e;
    while (TRUE)
    {
        if (*end == '\0')
        {
            if (gpsflag == 0)
                break;
            char *value = createstr(start, end);
            struct list* klist = getkeylist(value, vs, m);
            struct list* nelist = list_create();
            char *nekey;
            int i;
            for (i = 0; i < klist->size; i++)
            {
                char *key = (char *) (list_get(klist, i))->data;
                r.key = key;
                char t = getkeytype(key);
                if (t == 'b')
                    if (i == 0)
                        r.type = 1;//主基站
                    else
                        r.type = 2;
                else
                    r.type = 0;//wifi
                printR(r);
                nekey = createstr(r.key, r.key + strlen(r.key));
                struct list_e *e = listnode_create(nekey);
                list_add(nelist,e);
            }
            printne(nelist);
            list_destroy(nelist);
            list_destroy(klist);
            free(value);
            break;
        }

        if (*end == '|')
        {
            if (gpsflag == 0)
            {
                varflag = 0;
                start = ++end;
                continue;
            }
            char *value = createstr(start, end);
            struct list* klist = getkeylist(value, vs, m);
            struct list* nelist = list_create();
            char *nekey;
            int i;
            for (i = 0; i < klist->size; i++)
            {
                char *key = (char *) (list_get(klist, i))->data;
                r.key = key;
                char t = getkeytype(key);
                if (t == 'b')
                    if (i == 0)
                        r.type = 1;//主基站
                    else
                        r.type = 2;
                else
                    r.type = 0;//wifi
                printR(r);
                nekey = createstr(r.key, r.key + strlen(r.key));
                struct list_e *e = listnode_create(nekey);
                list_add(nelist,e);
            }
            printne(nelist);
            list_destroy(nelist);
            list_destroy(klist);
            free(value);
            start = ++end;
            varflag = 0;
            continue;
        }

        if (*end == ',')
        {
            char *value = createstr(start, end);
            if (varflag == 0)
                r.time = gettime(value);
            if (varflag == 1)
                r.y = strtod(value,NULL);
            if (varflag == 2)
                r.x = strtod(value,NULL);
            if (varflag == 3)
            {
                r.p = strtod(value,NULL);
                if (!gpsfilted(r.x, r.y, r.p)) 
                    gpsflag = 1;
            }
            varflag++;
            start = ++end;
            free(value);
            continue;
        }
        end++;
    }
}

//对有效日志进行处理
void deallog(struct log *log, int type, char *line)
{
    char *vs = hmap_get(log->paramp, "vs");
    char *m = hmap_get(log->paramp, "m");
    char *e = hmap_get(log->paramp, "e");
    if (pubfilted(vs,m))
        return;
    //获取每条日志里不同记录的公共数据
    //basekey在getbkey中申请的空间，将在record中free
    if (type == 1)
    {
        char *mcc = hmap_get(log->paramp, "mcc");
        char *mnc = hmap_get(log->paramp, "mnc");
        char *lac = hmap_get(log->paramp, "lac");
        char *ci = hmap_get(log->paramp, "ci");
        char *basekey = getbkey(mcc,mnc,lac,ci,vs,m);
        dealtklog(basekey,mcc,mnc,log);
        free(basekey);
    }else{
        dealcorlog(vs, m, log);
    }
}


char *getnowtime()
{
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	return asctime(timeinfo);
}

void parselog()
{
	printf("start: read log: %s",getnowtime());
	char *line = (char *)malloc(READ_BUFFER_SIZE);
//    FILE *fp = fopen("test","r+");
	while (fgets(line,READ_BUFFER_SIZE,stdin) != NULL)
//	while(fgets(line, READ_BUFFER_SIZE, fp) != NULL)
	{
		s.linenum++;
		int type = logtype(line);
		if(type > 0)
		{
			struct log *log = parseline(line, type);
			if(log != NULL)
			{	
				s.lognum++;
     //           printf("%s",line);
				deallog(log,type,line);
				freelog(log);
			}
		}
	} 
//	fclose(fp);
	free(line);
	printf("end: read log  :%s",getnowtime());
	printf("read lines : %ld\n",s.linenum);
	printf("read logs : %ld\n",s.lognum);
	printf("read keys : %ld\n",s.keynum);

}


void main(int argc, char *argv[])
{
    parselog();    
}
