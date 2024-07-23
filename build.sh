#!/bin/sh

if ! (type gcc > /dev/null); then
  apk add gcc libc-dev cunit cunit-dev
fi

rm -f libcsv.so || true
gcc -shared -o libcsv.so -fPIC libcsv.c libcsv_util.c

rm -f libcsv_unit_test || true
gcc libcsv_unit_tests.c libcsv.c libcsv_util.c -o libcsv_unit_tests -lcunit
