#! /bin/bash
cd /home/tigerknows/work/cWorkspace/forest/t
unzip -c ../../locationlog/tk_locate_me_20120613.log4php.zip | ../build/./parselog 
unzip -c ../../locationlog/corm.log.2012-06-13.local3.zip | ../build/./parselog 
