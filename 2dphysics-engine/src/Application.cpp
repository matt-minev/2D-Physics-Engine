#include "Application.h"

#include "./Physics/Constants.h"
#include "./Physics/Force.h"

bool Application::IsRunning() {
    return running;
}

///////////////////////////////////////////////////////////////////////////////
// Setup function (executed once in the beginning of the simulation)
///////////////////////////////////////////////////////////////////////////////
void Application::Setup() {
    running = Graphics::OpenWindow();

    anchor = Vec2(Graphics::Width() / 2.0, 30);

    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        Particle* bob = new Particle(anchor.x, anchor.y + (i * restLength), 2.0);
        bob->radius = 6;
        particles.push_back(bob);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Input processing
///////////////////////////////////////////////////////////////////////////////
void Application::Input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    running = false;

                if (event.key.keysym.sym == SDLK_UP)
                    pushForce.y = -50 * PIXELS_PER_METER;
                if (event.key.keysym.sym == SDLK_RIGHT)
                    pushForce.x = 50 * PIXELS_PER_METER;
                if (event.key.keysym.sym == SDLK_DOWN)
                    pushForce.y = 50 * PIXELS_PER_METER;
                if (event.key.keysym.sym == SDLK_LEFT)
                    pushForce.x = -50 * PIXELS_PER_METER;

                break;

            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_UP)
                    pushForce.y = 0;
                if (event.key.keysym.sym == SDLK_RIGHT)
                    pushForce.x = 0;
                if (event.key.keysym.sym == SDLK_DOWN)
                    pushForce.y = 0;
                if (event.key.keysym.sym == SDLK_LEFT)
                    pushForce.x = 0;

                break;

            case SDL_MOUSEMOTION:
                mouseCursor.x = event.motion.x;
                mouseCursor.y = event.motion.y;

                break;

            case SDL_MOUSEBUTTONDOWN:
                if (!leftMouseButtonDown && event.button.button == SDL_BUTTON_LEFT) 
                {
                    leftMouseButtonDown = true;
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    mouseCursor.x = x;
                    mouseCursor.y = y;
                }

                break;

            case SDL_MOUSEBUTTONUP:
                if (leftMouseButtonDown && event.button.button == SDL_BUTTON_LEFT) 
                {
                    leftMouseButtonDown = false;
                    int lastParticle = NUM_PARTICLES - 1;
                    Vec2 impulseDirection = (particles[lastParticle]->position - mouseCursor).UnitVector();
                    float impulseMagnitude = (particles[lastParticle]->position - mouseCursor).Magnitude() * 5.0;
                    particles[lastParticle]->velocity = impulseDirection * impulseMagnitude;
                }

                break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Update function (called several times per second to update objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Update() {
    // Wait some time until the target frame time in milliseconds is reached
    static int timePreviousFrame;
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - timePreviousFrame);
    if (timeToWait > 0)
        SDL_Delay(timeToWait);

    // Calculate the deltatime in seconds
    float deltaTime = (SDL_GetTicks() - timePreviousFrame) / 1000.0f;
    if (deltaTime > 0.016) { 
        deltaTime = 0.016; 
    }

    // Set  the time of the current frame to be used in the next one
    timePreviousFrame = SDL_GetTicks();

    // Apply forces to the particles
    for (auto particle: particles)
    {
        // Apply a "push" force to my particle
        particle->AddForce(pushForce);

        // Apply a "drag" force to the particle
        Vec2 drag = Force::GenerateDragForce(*particle, 0.002);
        particle->AddForce(drag);

        // Apply a "weight" force to the particle
        Vec2 weight = Vec2(0.0, particle->mass * 9.8 * PIXELS_PER_METER);
        particle->AddForce(weight);
    }

    // Apply a "spring" force to the particle connected to the anchor
    Vec2 springForce = Force::GenerateSpringForce(*particles[0], anchor, restLength, k);
    particles[0]->AddForce(springForce);

    // Connect the particles with the one before in a chain of springs
    for (int i = 1; i < NUM_PARTICLES; i++)
    {
        int currParticle = i;
        int prevParticle = i - 1;
        Vec2 springForce = Force::GenerateSpringForce(*particles[currParticle], *particles[prevParticle], restLength, k);
        particles[currParticle]->AddForce(springForce);
        particles[prevParticle]->AddForce(-springForce);
    }

    // Integrate the acceleration and the velocity to find the new position
    for (auto particle : particles)
    {
        particle->Integrate(deltaTime);
    }

    // Hardcoded flip in velocity if the particle touches the limits of the screen window
    // Not real physics simulation
    for (auto particle : particles)
    {
        if (particle->position.x - particle->radius <= 0)
        {
            particle->position.x = particle->radius;
            particle->velocity.x *= -0.9;
        }
        else if (particle->position.x + particle->radius >= Graphics::Width())
        {
            particle->position.x = Graphics::Width() - particle->radius;
            particle->velocity.x *= -0.9;
        }

        if (particle->position.y - particle->radius <= 0)
        {
            particle->position.y = particle->radius;
            particle->velocity.y *= -0.9;
        }
        else if (particle->position.y + particle->radius >= Graphics::Height())
        {
            particle->position.y = Graphics::Height() - particle->radius;
            particle->velocity.y *= -0.9;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Render function (called several times per second to draw objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Render() {
    Graphics::ClearScreen(0xFF0F0721);

    if (leftMouseButtonDown) 
    {
        int lastParticle = NUM_PARTICLES - 1;
        Graphics::DrawLine(particles[lastParticle]->position.x, particles[lastParticle]->position.y, mouseCursor.x, mouseCursor.y, 0xFF0000FF);
    }

    // Draw the anchor and the spring to the first bob
    Graphics::DrawFillCircle(anchor.x, anchor.y, 5, 0xFF001155);
    Graphics::DrawLine(anchor.x, anchor.y, particles[0]->position.x, particles[0]->position.y, 0xFFF313131);

    // Draw all the springs from one particle to the next
    for (int i = 0; i < NUM_PARTICLES - 1; i++)
    {
        int currParticle = i;
        int nextParticle = i + 1;
        Graphics::DrawLine(particles[currParticle]->position.x, particles[currParticle]->position.y, particles[nextParticle]->position.x, particles[nextParticle]->position.y, 0xFFF313131);
    }

    // Draw all the bob particles
    for (auto particle: particles)
    {
        Graphics::DrawFillCircle(particle->position.x, particle->position.y, particle->radius, 0xFFEEBB00);
    }

    Graphics::RenderFrame();
}

///////////////////////////////////////////////////////////////////////////////
// Destroy function to delete objects and close the window
///////////////////////////////////////////////////////////////////////////////
void Application::Destroy() {

    for (auto particle: particles)
    {
        delete particle;
    }

    Graphics::CloseWindow();
}