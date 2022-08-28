
set echo on

#./post-build.sh

echo "Building assemblies..."

cFilenames="/home/petermiller/Desktop/Ash/ffff/main.cpp"

echo "Files:" $cFilenames

assembly="engine"
compilerFlags="-g   -fPIC "
# -fms-extensions 
# -Wall -Werror
includeFlags="-Isrc " #-I$VULKAN_SDK/include
linkerFlags=" -lxcb -lX11 -lX11-xcb -lxkbcommon  -lm -L/usr/X11R6/lib" 
defines="-D_DEBUG  -D_KUSE_SIMD"

clang $cFilenames $compilerFlags -o $assembly $defines $includeFlags $linkerFlags 

./engine

#-lvulkan

#-L$VULKAN_SDK/lib