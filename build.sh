#!/bin/bash
set -o errexit

conan install . -if Build -of Build --build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -B Build .
cmake --build Build
