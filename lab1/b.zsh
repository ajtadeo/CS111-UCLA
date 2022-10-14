#! /bin/zsh
STD_IN=$(</dev/stdin)
echo $STD_IN | awk '{print toupper($0)}'