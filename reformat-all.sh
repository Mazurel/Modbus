#!/bin/sh

find . -name '*.cpp' -o -name '*.hpp' | xargs clang-format -i -style=file
