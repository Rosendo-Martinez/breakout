#include "ParticleGenerator.h"

Particle::Particle()
    : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f)
{
}

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount)
    : shader(shader), texture(texture), amount(amount)
{
    this->init();
}

void ParticleGenerator::Update(float dt, GameObject &object, unsigned int newParticles, glm::vec2 offset)
{
    // add new particles
    for (unsigned int i = 0; i < newParticles; i++)
    {
        unsigned int unusedParticle = this->firstUnusedParticle();
        this->respawnParticle(this->particles[unusedParticle], object, offset);
    }

    // update all particles
    for (unsigned int i = 0; i < this->amount; i++)
    {
        Particle& p = this->particles[i];
        p.Life -= dt;
        if (p.Life > 0.0f) // ITS ALIVE
        {
            p.Position -= p.Velocity * dt;
            p.Color.a -= dt * 2.5f;
        }
    }
}

void ParticleGenerator::Draw()
{
    // draw set up
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    this->shader.Use();
    this->texture.Bind();
    glBindVertexArray(this->VAO);

    // draw particles
    for (Particle& p : this->particles) // little p, big P
    {
        if (p.Life > 0.0f) // ITS ALIVE
        {
            this->shader.SetVector2f("offset", p.Position);
            this->shader.SetVector4f("color", p.Color);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

    // restore pre-draw opengl state
    glBindVertexArray(0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
    const float particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    unsigned int VBO;

    // create ids
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    
    // bind
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // fill buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

    // add attributes
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0); // <vec2 pos, vec2 texCoord>
    glEnableVertexAttribArray(0);

    // unbind
    glBindVertexArray(0);

    // create particles 
    for (unsigned int i = 0; i < this->amount; i++)
    {
        this->particles.push_back(Particle());
    }
}

unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle()
{
    // likely that last unused particle is next to other unused particles
    for (unsigned int i = lastUsedParticle; i < this->amount; i++)
    {
        if (this->particles[i].Life <= 0.0f)
        {
            lastUsedParticle = i;
            return i;
        }
    }

    for (unsigned int i = 0; i < lastUsedParticle; i++)
    {
        if (this->particles[i].Life <= 0.0f)
        {
            lastUsedParticle = i;
            return i;
        }
    }

    // no unused particle found
    lastUsedParticle = 0;
    return 0;
}


void ParticleGenerator::respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset)
{
    float random = ((rand() % 100) - 50) / 10.0f; // random offset = vec2(random,random)
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle.Position = object.Position + random + offset;
    particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f); // random grey scale color [black, white]
    particle.Life = 1.0f;
    particle.Velocity = object.Velocity * 0.1f;
}