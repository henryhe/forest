/**
*author hepan
*用于接受标准输入的日志，然后得到提交的数据，解析为各行都是有效数据的格式，格式如下
*时间 基站（wifi）号  x坐标  y坐标 pt  获取类型
*/
#include "../include/locate.h"

//返回日志来源，包含“&_x=”的日志判定为来源于定位，加上‘&’是因为cormorant日志里有些日志
//包含“lu=”的即判定为cormorant行为日志来的信息

//用语统计的结构
static struct stat s;

int logtype(char *line) 
{
	char *p = line;
	while (*p != '\0') 
	{	
		//来自tk的日志
		if (*p == '&' && *(p+1) == '_' && *(p+2) == 'x' && *(p+3) == '=')
			return 1;
		//来自corm的日志
		if (*p == 'l' && *(p+1) == 'u' && *(p+2) == '=')
			return 2;
		p++;
	}
	return 0;
}

//截取一个字符串的字串，转化为int
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

//将格式如2012-07-15 00:00:00,063的字符串转化为1900年到该时间的秒数
long gettime( char* line )
{
	char *timeinfo = (char *)malloc(24);
	memcpy(timeinfo,line,23);
    *(timeinfo+23) = '\0';
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

//计算basekey，如果参数不全，或者不符合日志要求
char *getbasekey(char *mcc, char *mnc, char *lac, char *ci, char *vs, char *m)
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
    s.basekeynum++;
    return basekey;
}

//得到来自tk日志的结果
void dealtklog(char *basekey,  char *mcc, char *mnc, struct log *log)
{
    if (basekey != NULL)
    {
        struct record r;
        r->time = log->time;

    }    
}

void dealcorlog(){

}

//对有效日志进行处理
void deallog(struct log *log, int type, char *line)
{
    char *vs = hmap_get(log->paramp, "vs");
    char *m = hmap_get(log->paramp,"m");
    char *e = hmap_get(log->paramp,"e");
    if (pubfilted(vs,m))
        return;
    //获取每条日志里不同记录的公共数据
    //basekey在getbasekey中申请的空间，将在record中free
    if (type == 1)
    {
        char *mcc = hmap_get(log->paramp, "mcc");
        char *mnc = hmap_get(log->paramp, "mnc");
        char *lac = hmap_get(log->paramp, "lac");
        char *ci = hmap_get(log->paramp, "ci");
        char *basekey = getbasekey(mcc,mnc,lac,ci,vs,m);
        dealtklog(basekey,mcc,mnc,log);
    }else{
        dealcorlog();
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
//	FILE *fp = fopen("test","r+");
	while (fgets(line,READ_BUFFER_SIZE,stdin) != NULL )
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
	printf("read basekeys : %ld\n",s.basekeynum);

}


void main(int argc, char *argv[])
{
    parselog();    
}
