#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "BallObject.h"

/**
 * AABB-AABB collision detection.
 */
bool CheckCollision(GameObject &one, GameObject &two)
{
    bool collisionX = (one.Position.x + one.Size.x >= two.Position.x) && (two.Position.x + two.Size.x >= one.Position.x); 
    bool collisionY = (one.Position.y + one.Size.y >= two.Position.y) && (two.Position.y + two.Size.y >= one.Position.y); 

    return collisionX && collisionY;
}

/**
 * Circle-AABB collision detection.
 */
bool CheckCollision(BallObject &one, GameObject &two)
{
    glm::vec2 center(one.Position + one.Radius);

    glm::vec2 aabbHalfSize(two.Size / 2.0f);
    glm::vec2 aabbCenter(two.Position + aabbHalfSize);

    glm::vec2 difference = center - aabbCenter;
    glm::vec2 clamped = glm::clamp(difference, -aabbHalfSize, aabbHalfSize);

    glm::vec2 closest = aabbCenter + clamped; // either point on edge of aabb, or point inside it
    difference = closest - center; 

    // length(difference) = 0      --> circle center inside aabb
    // length(difference) < radius --> part of circle inside aabb (but not center)
    // length(difference) > radius --> circle and aabb do NOT touch
    return glm::length(difference) < one.Radius;
}

const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);

GameObject *Player;

const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;

BallObject *Ball;

Game::Game(unsigned  int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height) // initialize state
{
}

Game::~Game()
{
}

SpriteRenderer *Renderer;

void Game::Init()
{
    // Load & configure resources

    // Shader program
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.fs", nullptr, "sprite");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

    // Renderer
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

    // Textures
    ResourceManager::LoadTexture("textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("textures/awesomeface.png", true, "face");
    ResourceManager::LoadTexture("textures/block.png", false, "block");
    ResourceManager::LoadTexture("textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("textures/paddle.png", true, "paddle");

    // Levels
    GameLevel one; one.Load("levels/one.lvl", this->Width, this->Height / 2);
    GameLevel two; two.Load("levels/two.lvl", this->Width, this->Height / 2);
    GameLevel three; three.Load("levels/three.lvl", this->Width, this->Height / 2);
    GameLevel four; four.Load("levels/four.lvl", this->Width, this->Height / 2);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;

    // Player
    glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

    // Ball
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        float distanceMoved = PLAYER_VELOCITY * dt;

        if (this->Keys[GLFW_KEY_A]) // left
        {
            if (Player->Position.x >= 0.0f)
            {
                Player->Position.x -= distanceMoved;

                // left boundary
                if (Player->Position.x < 0.0f)
                {
                    Player->Position.x = 0.0f;

                }

                if (Ball->Stuck)
                {
                    Ball->Position = Player->Position + glm::vec2(Player->Size.x / 2.0f - Ball->Radius, -Ball->Radius * 2.0f);
                }
            }
        }
        if (this->Keys[GLFW_KEY_D]) // right
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
            {
                Player->Position.x += distanceMoved;

                // right boundary
                if (Player->Position.x > this->Width - Player->Size.x)
                {
                    Player->Position.x = this->Width - Player->Size.x;
                }

                if (Ball->Stuck)
                {
                    Ball->Position = Player->Position + glm::vec2(Player->Size.x / 2.0f - Ball->Radius, -Ball->Radius * 2.0f);
                }
            }
        }
        if (this->Keys[GLFW_KEY_SPACE]) // unstick ball from player (paddle)
        {
            Ball->Stuck = false;
        }
    }
}

void Game::Update(float dt)
{
    Ball->Move(dt, this->Width);
    this->DoCollisions();
}

void Game::Render()
{
    if (this->State == GAME_ACTIVE)
    {
        // draw background
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f,0.0f), glm::vec2(this->Width,this->Height), 0.0f);

        // draw level
        this->Levels[this->Level].Draw(*Renderer);

        // draw player (paddle)
        Player->Draw(*Renderer);

        // draw ball
        Ball->Draw(*Renderer);
    }
}

void Game::DoCollisions()
{
    // Ball-brick collision
    for (GameObject &box : this->Levels[this->Level].Bricks)
    {
        if (!box.Destroyed)
        {
            if (CheckCollision(*Ball, box))
            {
                if (!box.IsSolid)
                {
                    box.Destroyed = true;
                }
            }
        }
    }
}