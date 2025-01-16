#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "BallObject.h"
#include "ParticleGenerator.h"
#include <tuple>

typedef std::tuple<bool, Direction, glm::vec2> Collision;   

/**
 * Returns direction that vector points.
 */
Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

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
Collision CheckCollision(BallObject &one, GameObject &two)
{
    glm::vec2 center(one.Position + one.Radius);

    glm::vec2 aabbHalfSize(two.Size / 2.0f);
    glm::vec2 aabbCenter(two.Position + aabbHalfSize);

    glm::vec2 difference = center - aabbCenter;
    glm::vec2 clamped = glm::clamp(difference, -aabbHalfSize, aabbHalfSize);

    glm::vec2 closest = aabbCenter + clamped; // either point on edge of aabb, or point inside it
    difference = closest - center; 

    // length(difference) = 0       --> circle center inside aabb
    // length(difference) <= radius --> part of circle inside aabb (but not center)
    // length(difference) > radius  --> circle and aabb do NOT touch

    // WARNING: I don't think this CC can handle the case where the ball's center is inside
    //          the AABB, because difference would just be:
    //              difference = CIRCLE_CENTER - AABB_CENTER
    //          If circle is centred on AABB center then:
    //              difference = 0;
    //          However, it would work if circle is // reverse horizontal directiontouching/inside AABB, but not its center.


    if (glm::length(difference) <= one.Radius) // collision
    {
        return std::make_tuple(true, VectorDirection(difference), difference);
    }
    else // no collision
    {
        return std::make_tuple(false, UP, glm::vec2(0.0f,0.0f));
    }
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

ParticleGenerator *Particles;

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

    // Particle generator
    ResourceManager::LoadTexture("textures/particle.png", true, "particle");
    ResourceManager::LoadShader("shaders/particle.vs", "shaders/particle.fs", nullptr, "particle");
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);
    Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
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

    Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));

    if (Ball->Position.y >= this->Height) // player lost ball
    {
        this->ResetLevel();
        this->ResetPlayer();
    }
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

        // draw particles
        Particles->Draw();

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
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)) // collision occurred
            {
                if (!box.IsSolid) // destroy brick
                {
                    box.Destroyed = true;
                }

                // Collision resolution

                Direction dir = std::get<1>(collision);
                glm::vec2 diff = std::get<2>(collision);

                if (dir == LEFT || dir == RIGHT) // horizontal collision
                {
                    // reverse horizontal direction
                    Ball->Velocity.x = -Ball->Velocity.x;

                    // push ball out horizontally
                    float penetration = Ball->Radius - std::abs(diff.x);
                    if (dir == LEFT) // ball came from right side of brick
                    {
                        Ball->Position.x += penetration;
                    }
                    else // ball came from left side of brick
                    {
                        Ball->Position.x -= penetration;
                    }
                }
                else // vertical collision
                {
                    // reverse vertical direction
                    Ball->Velocity.y = -Ball->Velocity.y;

                    float penetration = Ball->Radius - std::abs(diff.y);
                    if (dir == UP) // ball came from top of brick
                    {
                        Ball->Position.y -= penetration;
                    }
                    else // ball came from bottom of brick
                    {
                        Ball->Position.y += penetration;
                    }
                }
            }
        }
    }

    // Ball-paddle collision
    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result))
    {
        float centerPaddle = Player->Position.x + (Player->Size.x/2.0f);
        float distance = (Ball->Position.x + Ball->Radius) - centerPaddle; // player_center - paddle_center
        float percentage = distance / (Player->Size.x / 2.0f); // sign (+/-) indicates which side (left/right) of paddle player is on

        // Ball direction changes, but speed stays the same.
        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
    }
}

void Game::ResetLevel()
{
    if (this->Level == 0)
        this->Levels[0].Load("levels/one.lvl", this->Width, this->Height / 2);
    else if (this->Level == 1)
        this->Levels[1].Load("levels/two.lvl", this->Width, this->Height / 2);
    else if (this->Level == 2)
        this->Levels[2].Load("levels/three.lvl", this->Width, this->Height / 2);
    else if (this->Level == 3)
        this->Levels[3].Load("levels/four.lvl", this->Width, this->Height / 2);
}

/**
 * Resets player, and ball.
 */
void Game::ResetPlayer()
{
    // Reset player (paddle)
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);

    // Reset ball
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f), INITIAL_BALL_VELOCITY);
}