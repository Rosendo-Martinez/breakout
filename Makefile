all : ./bin/main.exe

./bin/Game.o : ./src/Game.h ./src/Game.cpp
	g++ -c ./src/Game.cpp -o ./bin/Game.o

./bin/Texture.o : ./src/Texture.h ./src/Texture.cpp
	g++ -c ./src/Texture.cpp -o ./bin/Texture.o -I./dep/glad/include

./bin/Shader.o : ./src/Shader.h ./src/Shader.cpp
	g++ -c ./src/Shader.cpp -o ./bin/Shader.o -I./dep/glad/include -I./dep/

./bin/ResourceManager.o : ./src/ResourceManager.h ./src/ResourceManager.cpp ./src/Texture.h ./src/Shader.h
	g++ -c ./src/ResourceManager.cpp -o ./bin/ResourceManager.o -I./dep/glad/include -I./dep/

./bin/main.exe : ./src/Game.h ./src/ResourceManager.h ./bin/Game.o ./bin/Texture.o ./bin/Shader.o ./bin/ResourceManager.o
	g++ ./src/main.cpp ./dep/glad/src/glad.c  ./bin/Game.o ./bin/Texture.o ./bin/Shader.o ./bin/ResourceManager.o -o ./bin/main.exe -I./dep/glad/include -I./dep/ -lglfw -ldl

clean:
	rm -f ./bin/*.o ./bin/main.exe

run:
	./bin/main.exe