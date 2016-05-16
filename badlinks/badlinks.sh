#!/usr/bin/env bash
recurse() {
    if [ "$(ls -A $1)" ]; then
        for element in $1/*; do
            if [ -h "$element" -a ! -e "$element" -a `stat --format=%Y $element` -le $(( `date +%s` - 604800 )) ] 
                        then echo "$element"
            elif [ -d "$element" ] 
                then recurse $element
            fi
        done
    fi
}
recurse $1
