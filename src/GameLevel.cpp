#include "GameLevel.h"
#include <string>
#include <fstream>
#include <sstream>

#include <iostream>

GameLevel::GameLevel()
    : Bricks()
{
}

/**
 * Loads/reloads a level.
 */
void GameLevel::Load(const char *file, unsigned int levelWidth, unsigned int levelHeight)
{
    // clear old data
    this->Bricks.clear();

    // load file into vector

    std::ifstream fstream(file);
    if (fstream)
    {
        std::string line;
        std::vector<std::vector<unsigned int>> tileData;

        while (std::getline(fstream, line))
        {
            unsigned int tileCode;
            std::istringstream sstream(line);
            std::vector<unsigned int> row;

            while (sstream >> tileCode)
            {
                row.push_back(tileCode);
            }

            tileData.push_back(row);
        }

        if (tileData.size() > 0)
        {
            this->init(tileData, levelWidth, levelHeight);
        }
    }
    else
    {
        std::cout << "ERROR: could not open file: " << file << std::endl;
    }
}

void GameLevel::Draw(SpriteRenderer &renderer)
{
    for (GameObject &tile : this->Bricks)
    {
        if (!tile.Destroyed)
        {
            tile.Draw(renderer);
        }
    }
}

bool GameLevel::IsCompleted()
{
    for (GameObject &tile : this->Bricks)
    {
        if (!tile.IsSolid && !tile.Destroyed)
        {
            return false;
        }
    }

    return true;
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{
    // IDEA: could add offset for top left of level
    //       would be useful for adding margin around level.

    unsigned int rows = tileData.size(); // number of rows
    unsigned int columns = tileData[0].size(); // number of columns
    float brickWidth = levelWidth / static_cast<float>(columns);
    float brickHeight = levelHeight / static_cast<float>(rows);

    for (unsigned int y = 0; y < rows; ++y) // rows
    {
        for (unsigned int x = 0; x < columns; ++x) // columns
        {
            if (tileData[y][x] == 1) // solid brick
            {
                glm::vec2 pos(brickWidth * x, brickHeight * y);
                glm::vec2 size(brickWidth, brickHeight);

                GameObject brick(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f,0.8f,0.7f));
                brick.IsSolid = true;
                this->Bricks.push_back(brick);
            }
            else if (tileData[y][x] > 1) // breakable brick
            {
                glm::vec3 color = glm::vec3(1.0f); 
                if (tileData[y][x] == 2)
                    color = glm::vec3(0.2f, 0.6f, 1.0f);
                else if (tileData[y][x] == 3)
                    color = glm::vec3(0.0f, 0.7f, 0.0f);
                else if (tileData[y][x] == 4)
                    color = glm::vec3(0.8f, 0.8f, 0.4f);
                else if (tileData[y][x] == 5)
                    color = glm::vec3(1.0f, 0.5f, 0.0f);
                
                glm::vec2 pos(brickWidth * x, brickHeight * y);
                glm::vec2 size(brickWidth, brickHeight);
                GameObject brick(pos, size, ResourceManager::GetTexture("block"), color);
                this->Bricks.push_back(brick);
            }
            // else tile code is 0
                // empty space, so do nothing
        }
    }
}