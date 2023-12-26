g++ --std=c++23 -O3 -Wall ./CubeCrafters/CubeCrafters.cpp ./glad.c -I ./CubeCrafters/include -I ./Library/include -L ./Library/lib -lglfw3 \
-lssl -lcrypto -o ./build/CubeCrafters.out