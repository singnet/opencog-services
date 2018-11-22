#!/bin/bash

CPP_SERVICE_LIST=`cd src/cpp-services/; ls *.cc | sed {s/\.cc//g}`
SCM_SERVICE_LIST=`cd src/scm-services/; ls *.scm | sed {s/\.scm//g}`

build_service_doc() {

    if [ -f "docs/$1.md" ] ; then
        cat docs/templates/service.template.html | sed {s/__SERVICE_NAME__/$1/g} > docs/$1.html
    else
        echo "docs/$1.md is mising"
    fi
}

build_service_index() {

    if [ $2 = CPP ] ; then
        SRC_DIR=cpp-services
        EXT=cc
    else
        SRC_DIR=scm-services
        EXT=scm
    fi

    TAGS=`sed --expression 's/\([A-Z]\)/\\\ \L\1/g' \
              --expression 's/^\\\ //' <<< $1`

    cat docs/templates/index_service.template.html \
        | sed {s/__SERVICE_NAME__/$1/g} \
        | sed {s/__SERVICE_EXTENSION__/$EXT/g} \
        | sed {s/__SRC_DIR__/$SRC_DIR/g} \
        | sed {s/__SERVICE_TAGS__/"$TAGS"/g} \
        >> docs/index.html
}

for SERVICE in $CPP_SERVICE_LIST; do
    build_service_doc $SERVICE CPP
done
for SERVICE in $SCM_SERVICE_LIST; do
    build_service_doc $SERVICE SCM
done

cat docs/templates/index_1.template.html > docs/index.html
for SERVICE in $CPP_SERVICE_LIST; do
    build_service_index $SERVICE CPP
done
for SERVICE in $SCM_SERVICE_LIST; do
    build_service_index $SERVICE SCM
done
cat docs/templates/index_2.template.html >> docs/index.html
