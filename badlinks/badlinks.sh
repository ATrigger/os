#!/usr/bin/env bash
recurse() {
	for element in $1/*; do
		if [ -h "$element" -a ! -e "$element" -a `stat --format=%Y $element` -le $(( `date +%s` - 604800 )) ] 
    			    then echo "$element"
		elif [ -d "$element" ] 
			then recurse $element
		fi
	done
}
recurse $1