echo on

echo "Compiling shaders..."

/home/petermiller/Desktop/1.3.216.0/x86_64/bin/glslc -fshader-stage=vert shader.vert -o vert.spv

/home/petermiller/Desktop/1.3.216.0/x86_64/bin/glslc -fshader-stage=frag shader.frag -o frag.spv