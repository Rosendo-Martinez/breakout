#ifndef GAME_H
#define GAME_H

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

        Game(unsigned int width, unsigned int height);
        ~Game();

        // loads assets
        void Init();

        // game loop
        void ProcessInput(float dt); // why does this need dt?
        void Update(float dt); // this makes sense why it would need dt.
        void Render();
};

#endif