#!/bin/sh

SERVICE_LIST=`cd src/cpp-services/; ls *.cc | sed {s/\.cc//g} ; cd ../scm-services/; ls *.scm | sed {s/\.scm//g}`

check_service() {

    STATUS="OK"

    if [ ! -d "tests/$1" ] ; then
        echo "tests/$1 is mising"
        STATUS="NOK"
    fi
    if [ ! -f "tests/$1/testCases.txt" ] ; then
        echo "tests/$1/testCases.txt is mising"
        STATUS="NOK"
    fi
    if [ ! -d "tests/$1/baseline" ] ; then
        echo "tests/$1/baseline is mising"
        STATUS="NOK"
    fi
    if [ ! -f "docs/$1.md" ] ; then
        echo "docs/$1.md is mising"
        STATUS="NOK"
    fi

    if [ $STATUS = "NOK" ] ; then
        echo "ERROR"
        exit 1
    fi
}

for SERVICE in $SERVICE_LIST; do
    check_service $SERVICE
done
