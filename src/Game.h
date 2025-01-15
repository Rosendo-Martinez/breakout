#ifndef GAME_H
#define GAME_H

#include "GameLevel.h"
#include <GLFW/glfw3.h>

/**
 * Up is +y
 * Down is -y
 * Left is -x
 * Right is +x
 * 
 * Note: In screen coordinate, UP points down due to +y pointing down!
 */
enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

enum GameState
{
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN // what about lose?
};

class Game
{
    public:
        GameState State;
        bool Keys[1024];
        unsigned int Width, Height;
        std::vector<GameLevel> Levels;
        unsigned int Level;

        Game(unsigned int width, unsigned int height);
        ~Game();

        // loads assets
        void Init();

        void DoCollisions();
        void ResetLevel();
        void ResetPlayer();

        // game loop
        void ProcessInput(float dt); // why does this need dt?
        void Update(float dt); // this makes sense why it would need dt.
        void Render();
};

#endif