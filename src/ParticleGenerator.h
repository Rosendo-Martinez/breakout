#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"

struct Particle
{
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    float Life;

    Particle();
};

class ParticleGenerator
{
    public:
        ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);

        void Update(float dt, GameObject &object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
        void Draw();

    private:
        std::vector<Particle> particles;
        unsigned int amount;

        Shader shader;
        Texture2D texture;
        unsigned int VAO;

        void init();
        unsigned int firstUnusedParticle();
        void respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2(0.0f,0.0f));
};

#endif