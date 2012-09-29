#! /bin/bash
cd /home/tigerknows/work/cWorkspace/forest/t
unzip -c ../../locationlog/corm.log.2012-06-13.local3.zip | ../build/./parselog | ../build/./prodata
