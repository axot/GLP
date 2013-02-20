#!/usr/bin/env zsh

n=80

function usage(){
  echo $0 [-n defualt:80]
  exit 1
}

while getopts :n:h opt
do
    case "$opt" in
    "n") n=$OPTARG
         ;;
    "h") usage
         ;;
    esac
done

max=`ls sdfs/*.mol | wc -l`
i=1
while [ $i -le $max ]
do
  j=$((i + n - 1));
  if [ $j -gt $max ]; then
    j=$((max))
  fi
  # not draw H atom, 8 columns each row
  obabel sdfs/{$i..$j}.mol -O sdfs/${i}_${j}.svg -d -xC -xc 8
  qlmanage -t -s 5000 -o sdfs sdfs/${i}_${j}.svg
  i=$((j+1))

done

