#include "Application.h"

#include "./Physics/Constants.h"
#include "./Physics/Force.h"
#include "./Physics/CollisionDetection.h"
#include "./Physics/Contact.h"

bool Application::IsRunning()
{
    return running;
}

///////////////////////////////////////////////////////////////////////////////
// Setup function (executed once in the beginning of the simulation)
///////////////////////////////////////////////////////////////////////////////
void Application::Setup()
{
    running = Graphics::OpenWindow();

    // Create a physics world with gravity of -9.8 m/s2
    world = new World(-9.8);

    // Add a floor and walls to contain objects objects
    Body* floor = new Body(BoxShape(Graphics::Width() - 50, 50), Graphics::Width() / 2.0, Graphics::Height() - 50, 0.0);
    Body* leftWall = new Body(BoxShape(50, Graphics::Height() - 100), 50, Graphics::Height() / 2.0 - 25, 0.0);
    Body* rightWall = new Body(BoxShape(50, Graphics::Height() - 100), Graphics::Width() - 50, Graphics::Height() / 2.0 - 25, 0.0);
    floor->restitution = 0.8;
    leftWall->restitution = 0.2;
    rightWall->restitution = 0.2;
    floor->SetTexture("./assets/metal.png");
    leftWall->SetTexture("./assets/metal.png");
    rightWall->SetTexture("./assets/metal.png");
    world->AddBody(floor);
    world->AddBody(leftWall);
    world->AddBody(rightWall);

    /*// Add a static box so other objects can collide
    Body* bigBox = new Body(BoxShape(200, 200), Graphics::Width() / 2.0, Graphics::Height() / 2.0, 0.0);
    bigBox->SetTexture("./assets/crate.png");
    bigBox->restitution = 0.7;
    bigBox->rotation = 1.4;
    bigBox->shape->UpdateVertices(bigBox->rotation, bigBox->position);
    world->AddBody(bigBox);*/
}

///////////////////////////////////////////////////////////////////////////////
// Input processing
///////////////////////////////////////////////////////////////////////////////
void Application::Input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                running = false;
            if (event.key.keysym.sym == SDLK_d)
                debug = !debug;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);

                Body* ball = new Body(CircleShape(30), x, y, 0.62);
                ball->SetTexture("./assets/basketball.png");
                ball->restitution = 0.75;
                ball->friction = 0.1;

                world->AddBody(ball);
            }

            if (event.button.button == SDL_BUTTON_RIGHT)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);

                Body* tennisBall = new Body(CircleShape(15), x, y, 0.058);
                tennisBall->SetTexture("./assets/tennisball.png");
                tennisBall->restitution = 0.85;
                tennisBall->friction = 0.1;

                world->AddBody(tennisBall);
            }

            if (event.button.button == SDL_BUTTON_MIDDLE)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);

                Body* ball = new Body(CircleShape(30), Graphics::Width() / 2.0, 100, 0.62);
                ball->SetTexture("./assets/basketball.png");
                ball->restitution = 0.75;
                ball->friction = 0.1;

                Body* tennisBall = new Body(CircleShape(15), Graphics::Width() / 2.0 + 0.05, 50, 0.058);
                tennisBall->SetTexture("./assets/tennisball.png");
                tennisBall->restitution = 0.85;
                tennisBall->friction = 0.1;

                world->AddBody(ball);
                world->AddBody(tennisBall);
            }

            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Update function (called several times per second to update objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Update()
{
    Graphics::ClearScreen(0xFF0F0721);

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

    // Update world bodies (integration, collision detection & resolution, etc.)
    world->Update(deltaTime);
}

///////////////////////////////////////////////////////////////////////////////
// Render function (called several times per second to draw objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Render()
{
    // Draw all bodies
    for (auto& body : world->GetBodies()) 
    {
        if (body->shape->GetType() == CIRCLE) 
        {
            CircleShape* circleShape = (CircleShape*)body->shape;
            if (!debug && body->texture) 
            {
                Graphics::DrawTexture(body->position.x, body->position.y, circleShape->radius * 2, circleShape->radius * 2, body->rotation, body->texture);
            }
            else if (debug) 
            {
                Graphics::DrawCircle(body->position.x, body->position.y, circleShape->radius, body->rotation, 0xFF0000FF);
            }
        }
        if (body->shape->GetType() == BOX) 
        {
            BoxShape* boxShape = (BoxShape*)body->shape;
            if (!debug && body->texture) 
            {
                Graphics::DrawTexture(body->position.x, body->position.y, boxShape->width, boxShape->height, body->rotation, body->texture);
            }
            else if (debug) 
            {
                Graphics::DrawPolygon(body->position.x, body->position.y, boxShape->worldVertices, 0xFF0000FF);
            }
        }
        if (body->shape->GetType() == POLYGON) 
        {
            PolygonShape* polygonShape = (PolygonShape*)body->shape;
            if (!debug && body->texture) 
            {
                Graphics::DrawTexture(body->position.x, body->position.y, polygonShape->width, polygonShape->height, body->rotation, body->texture);
            }
            else if (debug) 
            {
                Graphics::DrawPolygon(body->position.x, body->position.y, polygonShape->worldVertices, 0xFF0000FF);
            }
        }
    }

    Graphics::RenderFrame();
}

///////////////////////////////////////////////////////////////////////////////
// Destroy function to delete objects and close the window
///////////////////////////////////////////////////////////////////////////////
void Application::Destroy() {
    delete world;

    Graphics::CloseWindow();
}