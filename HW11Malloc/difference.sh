#!/bin/bash

for filename in expected/*.txt ;
do
	filename="${filename#expected/}"
	cp "expected/$filename" .
	if [ -e "actual/$filename" ]
	then
		diff -ty --strip-trailing-cr $filename "actual/$filename" > "diff/$filename"
	fi
	rm "$filename"
done
