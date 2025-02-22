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

    Body* a = new Body(100, 100, 1.0);
    Body* b = new Body(300, 100, 1.0);
    Body* c = new Body(300, 300, 1.0);
    Body* d = new Body(100, 300, 1.0);

    a->radius = 6;
    b->radius = 6;
    c->radius = 6;
    d->radius = 6;

    bodies.push_back(a);
    bodies.push_back(b);
    bodies.push_back(c);
    bodies.push_back(d);
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
                    int lastBody = NUM_BODIES - 1;
                    Vec2 impulseDirection = (bodies[lastBody]->position - mouseCursor).UnitVector();
                    float impulseMagnitude = (bodies[lastBody]->position - mouseCursor).Magnitude() * 5.0;
                    bodies[lastBody]->velocity = impulseDirection * impulseMagnitude;
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

    // Apply forces to the bodies
    for (auto body: bodies)
    {
        // Apply a "push" force to my body
        body->AddForce(pushForce);

        // Apply a "drag" force to the body
        Vec2 drag = Force::GenerateDragForce(*body, 0.003);
        body->AddForce(drag);

        // Apply a "weight" force to the body
        Vec2 weight = Vec2(0.0, body->mass * 9.8 * PIXELS_PER_METER);
        body->AddForce(weight);
    }

    // Attach bodies with springs
    Vec2 ab = Force::GenerateSpringForce(*bodies[0], *bodies[1], restLength, k); // a <-> b
    bodies[0]->AddForce(ab);
    bodies[1]->AddForce(-ab);

    Vec2 bc = Force::GenerateSpringForce(*bodies[1], *bodies[2], restLength, k); // b <-> c
    bodies[1]->AddForce(bc);
    bodies[2]->AddForce(-bc);

    Vec2 cd = Force::GenerateSpringForce(*bodies[2], *bodies[3], restLength, k); // c <-> d
    bodies[2]->AddForce(cd);
    bodies[3]->AddForce(-cd);

    Vec2 da = Force::GenerateSpringForce(*bodies[3], *bodies[0], restLength, k); // d <-> a
    bodies[3]->AddForce(da);
    bodies[0]->AddForce(-da);

    Vec2 ac = Force::GenerateSpringForce(*bodies[0], *bodies[2], restLength, k); // a <-> c
    bodies[0]->AddForce(ac);
    bodies[2]->AddForce(-ac);

    Vec2 bd = Force::GenerateSpringForce(*bodies[1], *bodies[3], restLength, k); // b <-> d
    bodies[1]->AddForce(bd);
    bodies[3]->AddForce(-bd);


    // Integrate the acceleration and the velocity to find the new position
    for (auto body : bodies)
    {
        body->Integrate(deltaTime);
    }

    // Hardcoded flip in velocity if the body touches the limits of the screen window
    // Not real physics simulation
    for (auto body : bodies)
    {
        if (body->position.x - body->radius <= 0)
        {
            body->position.x = body->radius;
            body->velocity.x *= -0.9;
        }
        else if (body->position.x + body->radius >= Graphics::Width())
        {
            body->position.x = Graphics::Width() - body->radius;
            body->velocity.x *= -0.9;
        }

        if (body->position.y - body->radius <= 0)
        {
            body->position.y = body->radius;
            body->velocity.y *= -0.9;
        }
        else if (body->position.y + body->radius >= Graphics::Height())
        {
            body->position.y = Graphics::Height() - body->radius;
            body->velocity.y *= -0.9;
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
        int lastBody = NUM_BODIES - 1;
        Graphics::DrawLine(bodies[lastBody]->position.x, bodies[lastBody]->position.y, mouseCursor.x, mouseCursor.y, 0xFF0000FF);
    }

    // Draw all springs
    Graphics::DrawLine(bodies[0]->position.x, bodies[0]->position.y, bodies[1]->position.x, bodies[1]->position.y, 0xFF313131);
    Graphics::DrawLine(bodies[1]->position.x, bodies[1]->position.y, bodies[2]->position.x, bodies[2]->position.y, 0xFF313131);
    Graphics::DrawLine(bodies[2]->position.x, bodies[2]->position.y, bodies[3]->position.x, bodies[3]->position.y, 0xFF313131);
    Graphics::DrawLine(bodies[3]->position.x, bodies[3]->position.y, bodies[0]->position.x, bodies[0]->position.y, 0xFF313131);
    Graphics::DrawLine(bodies[0]->position.x, bodies[0]->position.y, bodies[2]->position.x, bodies[2]->position.y, 0xFF313131);
    Graphics::DrawLine(bodies[1]->position.x, bodies[1]->position.y, bodies[3]->position.x, bodies[3]->position.y, 0xFF313131);

    // Draw all bodies
    Graphics::DrawFillCircle(bodies[0]->position.x, bodies[0]->position.y, bodies[0]->radius, 0xFFEEBB00);
    Graphics::DrawFillCircle(bodies[1]->position.x, bodies[1]->position.y, bodies[1]->radius, 0xFFEEBB00);
    Graphics::DrawFillCircle(bodies[2]->position.x, bodies[2]->position.y, bodies[2]->radius, 0xFFEEBB00);
    Graphics::DrawFillCircle(bodies[3]->position.x, bodies[3]->position.y, bodies[3]->radius, 0xFFEEBB00);

    Graphics::RenderFrame();
}

///////////////////////////////////////////////////////////////////////////////
// Destroy function to delete objects and close the window
///////////////////////////////////////////////////////////////////////////////
void Application::Destroy() {

    for (auto body: bodies)
    {
        delete body;
    }

    Graphics::CloseWindow();
}