all : ./bin/main.exe

./bin/Game.o : ./src/Game.h ./src/Game.cpp ./src/ResourceManager.h ./src/SpriteRenderer.h
	g++ -c ./src/Game.cpp -o ./bin/Game.o -I./dep/glad/include -I./dep/

./bin/Texture.o : ./src/Texture.h ./src/Texture.cpp
	g++ -c ./src/Texture.cpp -o ./bin/Texture.o -I./dep/glad/include

./bin/Shader.o : ./src/Shader.h ./src/Shader.cpp
	g++ -c ./src/Shader.cpp -o ./bin/Shader.o -I./dep/glad/include -I./dep/

./bin/ResourceManager.o : ./src/ResourceManager.h ./src/ResourceManager.cpp ./src/Texture.h ./src/Shader.h
	g++ -c ./src/ResourceManager.cpp -o ./bin/ResourceManager.o -I./dep/glad/include -I./dep/

./bin/SpriteRenderer.o : ./src/Shader.h ./src/Texture.h
	g++ -c ./src/SpriteRenderer.cpp -o ./bin/SpriteRenderer.o -I./dep/glad/include -I./dep/

./bin/main.exe : ./src/Game.h ./src/ResourceManager.h ./bin/Game.o ./bin/Texture.o ./bin/Shader.o ./bin/ResourceManager.o ./bin/SpriteRenderer.o ./bin/GameLevel.o ./bin/GameObject.o ./bin/BallObject.o ./bin/ParticleGenerator.o
	g++ ./src/main.cpp ./dep/glad/src/glad.c  ./bin/Game.o ./bin/Texture.o ./bin/Shader.o ./bin/ResourceManager.o ./bin/SpriteRenderer.o ./bin/GameLevel.o ./bin/GameObject.o ./bin/BallObject.o ./bin/ParticleGenerator.o -o ./bin/main.exe -I./dep/glad/include -I./dep/ -lglfw -ldl

./bin/GameLevel.o : ./src/GameLevel.h ./src/GameLevel.cpp
	g++ -c ./src/GameLevel.cpp -o ./bin/GameLevel.o -I./dep/glad/include -I./dep/

./bin/GameObject.o : ./src/GameObject.h ./src/GameObject.cpp 
	g++ -c ./src/GameObject.cpp -o ./bin/GameObject.o -I./dep/glad/include -I./dep/

./bin/BallObject.o : ./src/BallObject.h ./src/BallObject.cpp 
	g++ -c ./src/BallObject.cpp -o ./bin/BallObject.o -I./dep/glad/include -I./dep/

./bin/ParticleGenerator.o : ./src/ParticleGenerator.cpp ./src/ParticleGenerator.h
	g++ -c ./src/ParticleGenerator.cpp -o ./bin/ParticleGenerator.o -I./dep/glad/include -I./dep/

clean:
	rm -f ./bin/*.o ./bin/main.exe

run: all
	./bin/main.exe