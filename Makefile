all: parselog prodata

parselog: parselog.o locate.o hashmap.o list.o myUtil.o
	gcc -o build/parselog build/locate.o build/parselog.o build/hashmap.o build/list.o build/myUtil.o

parselog.o: src/parselog.c include/locate.h
	gcc -o build/parselog.o -c src/parselog.c

prodata: prodata.o mycache.o locate.o hashmap.o list.o myUtil.o flrfs.o log.o config.o
	gcc -g -o build/prodata build/mycache.o build/locate.o build/prodata.o build/hashmap.o build/list.o build/myUtil.o build/flrfs.o build/log.o build/config.o

prodata.o: src/prodata.c include/locate.h
	gcc -o build/prodata.o -c src/prodata.c

mycache.o: src/util/mycache.c include/mycache.h
	gcc -o build/mycache.o -c src/util/mycache.c

locate.o: src/locate.c include/locate.h
	gcc -o build/locate.o -c src/locate.c

hashmap.o: src/util/hashmap.c include/hashmap.h
	gcc -o build/hashmap.o -c src/util/hashmap.c

list.o: src/util/list.c include/list.h
	gcc -o build/list.o  -c src/util/list.c

myUtil.o: src/util/myUtil.c include/myUtil.h
	gcc -o build/myUtil.o  -c src/util/myUtil.c

flrfs.o: src/util/flrfs.c include/flrfs.h
	gcc -o build/flrfs.o  -c src/util/flrfs.c

log.o: src/util/log.c include/log.h
	gcc -o build/log.o  -c src/util/log.c

config.o: src/util/config.c include/config.h
	gcc -o build/config.o  -c src/util/config.c
