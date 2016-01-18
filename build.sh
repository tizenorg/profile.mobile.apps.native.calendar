#!/bin/bash

except=""
#except="
# WNaviFrame.cpp
# WSlideShowView.cpp
# CalMainView.cpp
# CalMonthControl.cpp
# CalListControl.cpp
# "

gbs build -A armv7l --include-all --debug $1 2>&1 | tee build.txt

echo -e '\E[0;31m'"\033[1m  Checking... waiting...\n\n \033[0m"

sed -n '/warning/p' build.txt > build_1.txt
sed -n '/calendar/p' build_1.txt > build_2.txt
sed '/edc/d' build_2.txt > build_3.txt
sed 's/:.*$//' build_3.txt > build_4.txt
sed 's/^.*\///' build_4.txt > build_5.txt
sort -u build_5.txt > build_6.txt

#echo $except
result=0
while read line;
do
    if [[ $except != *$line* ]]; then
        result=1
        echo -e '\E[40;32m'"\033[1m  Found warnings you have just created in $line \033[0m"
        echo -e '\E[40;32m'"\033[1m  Check below files...\033[0m"
        grep -rn $line build_3.txt
    fi
done < build_6.txt

if [ $result = 1 ]; then
    echo -e '\E[40;31m'"\033[1m DO NOT UPLOADING... \033[0m"
    echo -e '\E[40;31m'"\033[1m Please fix warnings before uploading.\033[0m"
	exit 255
else
    echo -e '\E[40;37m'"\033[1m No warnings are detected. continue your job.\033[0m"
fi

rm build.txt
rm build_1.txt
rm build_2.txt
rm build_3.txt
rm build_4.txt
rm build_5.txt
rm build_6.txt
