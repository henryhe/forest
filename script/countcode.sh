#! /bin/bash
find . \( -name *.c -o -name *.h \) | xargs cat | grep -v ^$ | wc -l
