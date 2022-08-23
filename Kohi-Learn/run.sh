
set echo on

#./post-build.sh

echo "Building assemblies..."

cFilenames="/home/petermiller/Desktop/Kohi-Learn/main.cpp"

echo "Files:" $cFilenames

assembly="engine"
compilerFlags="-g   -fPIC -Werror=vla"
# -fms-extensions 
# -Wall -Werror
includeFlags="-Isrc -I$VULKAN_SDK/include"
linkerFlags="-lvulkan -lxcb -lX11 -lX11-xcb -lxkbcommon -L$VULKAN_SDK/lib -lm -L/usr/X11R6/lib" 
defines="-D_DEBUG  -D_KUSE_SIMD"

clang $cFilenames $compilerFlags -o $assembly $defines $includeFlags $linkerFlags 

./engine