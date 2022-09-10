
set echo on

echo "Building assemblies..."

cFilenames="/home/petermiller/Desktop/4coder/hello/src/main.cpp"

echo "Files:" $cFilenames
cd src
assembly="engine"
compilerFlags="-g -Wall  -fPIC "

#-Warc  -Wextra
# -fms-extensions 
# -Wall -Werror
includeFlags="-Isrc -I$VULKAN_SDK/include "
linkerFlags=" -lxcb -lX11 -lX11-xcb -lxkbcommon  -std=c++20 -lm -L/usr/X11R6/lib   " 
defines="-D_DEBUG  -D_KUSE_SIMD"

clang++ $cFilenames $compilerFlags -o $assembly $defines -g $includeFlags $linkerFlags 

#cppcheck --enable=all #/home/petermiller/Desktop/4coder/hello/src/descriptor_sets.h  #--force


#-lvulkan

#-L$VULKAN_SDK/lib
