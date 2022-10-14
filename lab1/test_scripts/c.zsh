#! /bin/zsh
STD_IN=$(</dev/stdin)
echo $STD_IN | sed 's/ /_/g'