#!/usr/bin/env bash

XML="$(cat ./cmake/proto.svg)"

coll="$(\
    cat $1 | \
    sed '/^[^ ]/d' | \
    sed 's/  //g' | \
    sed 's/ /_/g' | \
    sed 's/\(\.\|:_\|%_(\|_of_\|_[a-z]*)\)/ /g' | \
    sed '/.*no_data_found/d' | \
    sed 's/\([a-z]*\) *\([0-9]*\) \([0-9]\) *\([0-9]*\) *\([0-9]*\)/\1 \2.\3%/'
    )"

name=""
shield_link="https://img.shields.io/badge/NAME-VALUE-COLOR.svg"

content="${XML}"
for file in $(ls $2 | grep '.svg'); do
    rm $2/${file}
done

for word in $(echo ${coll}); do
    if [ "$word" = "lines" ] || [ "$word" = "functions" ]; then
        name=${word}
        link="$(echo ${shield_link} | sed "s/NAME/Lcov%20${name}/g")"
        content=$(echo ${XML} | sed "s/NAME/Lcov ${word}/g")
    else
        color="#4c1"
        shield_color="green"
        val=$(echo ${word} | sed 's/\.[0-9]%//g')
        if [[ "${val}" -lt "90" ]]; then color="#dfb317"; shield_color="yellow"; fi
        if [[ "${val}" -lt "75" ]]; then color="#e05d44"; shield_color="red"; fi
        val="$( echo ${word} | sed 's/%/%25/g' )"

        content=$(echo ${content} | sed "s/VALUE/${word}/g")
        content=$(echo ${content} | sed "s/COLOR/${color}/g")
        link="$(echo ${link} | sed "s/VALUE/${val}/g")"
        link="$(echo ${link} | sed "s/COLOR/${shield_color}/g")"
        echo ${content} >> $2/${name}.svg
        echo "[![Lcov ${name}](${link})](./.badges/${name}.svg)"
    fi
done
