#!/usr/bin/env bash

ALL_FORMATABLE_FILES=$(find src include tests example -name '*.cpp' -o -name '*.hpp')
CLANG_FORMAT_ARGS="-style=file"

echo "*** Modbus Clang Format Wrapper **"
clang-format --version

if [[ "$DRY" = "TRUE" ]]; then
    diff_count=`clang-format --dry-run $CLANG_FORMAT_ARGS $ALL_FORMATABLE_FILES 2>&1 | wc -l`
    exit $(($diff_count > 0))
else
    clang-format -i --verbose $CLANG_FORMAT_ARGS $ALL_FORMATABLE_FILES
fi
