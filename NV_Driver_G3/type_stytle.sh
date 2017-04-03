#!/bin/sh

for f in $(find . -name '*.c' -or -name '*.h' -type f)
do
	enca -L zh_CN -x UTF-8 $f
	fromdos $f
	#indent -linux -l80 $f
	astyle --style=linux --pad-oper --max-instatement-indent=40 --unpad-paren --align-pointer=name --add-brackets --indent-preprocessor --indent-col1-comments --indent=tab --suffix=none --max-code-length=80 --pad-header -m0 --lineend=linux -N --quiet $f
     #astyle --style=linux --quiet --pad-oper --max-instatement-indent=40 --unpad-paren --align-pointer=name --add-brackets --indent-preprocessor --indent-col1-comments --indent=tab --suffix=none --max-code-length=80 --pad-header -m0 --lineend=linux $f
done

# after formate the code,we need to rm '*.orig' files
for f in $(find . -name '*.orig' -type f)
do
     rm $f
done

for f in $(find . -name '*.c~' -or -name '*.h~' -type f)
do
     rm $f
done
