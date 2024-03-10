#!/bin/bash
set -o errexit

conan install . --output-folder=Build --build=missing
(cd Build && cmake -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..)
cmake --build Build
