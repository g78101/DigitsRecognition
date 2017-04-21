#bin/sh

preValue=?
lastEndNumber=????

count=0
underlineSign=_
PREFIXFILENAME=9999
FILE_LAST_NAME=.bmp

array=`find . -name "$preValue$underlineSign$PREFIXFILENAME*$FILE_LAST_NAME"`

for i in $array;
do
	count=$(($count + 1))
	originNum=$(($lastEndNumber + $count))

    newFileName=../t10k-images/$preValue$underlineSign$originNum$FILE_LAST_NAME
    mv $i $newFileName
done

echo "value:"$preValue" fileNumber:"$originNum" count:"$count