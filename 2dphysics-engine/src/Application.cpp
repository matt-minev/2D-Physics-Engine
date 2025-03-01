#include "Application.h"

#include <windows.h>

#include "./Physics/Constants.h"
#include "./Physics/Force.h"
#include "./Physics/CollisionDetection.h"
#include "./Physics/Contact.h"

bool Application::IsRunning() {
    return running;
}

///////////////////////////////////////////////////////////////////////////////
// Setup function (executed once in the beginning of the simulation)
///////////////////////////////////////////////////////////////////////////////
void Application::Setup() {
    running = Graphics::OpenWindow();

    // Add a floor and walls to contain objects objects
    Body* floor = new Body(BoxShape(Graphics::Width() - 50, 50), Graphics::Width() / 2.0, Graphics::Height() - 50, 0.0);
    Body* leftWall = new Body(BoxShape(50, Graphics::Height() - 100), 50, Graphics::Height() / 2.0 - 25, 0.0);
    Body* rightWall = new Body(BoxShape(50, Graphics::Height() - 100), Graphics::Width() - 50, Graphics::Height() / 2.0 - 25, 0.0);
    floor->restitution = 0.5;
    leftWall->restitution = 0.2;
    rightWall->restitution = 0.2;
    bodies.push_back(floor);
    bodies.push_back(leftWall);
    bodies.push_back(rightWall);

    // Add a static box so other objects can collide
    Body* bigBox = new Body(BoxShape(200, 200), Graphics::Width() / 2.0, Graphics::Height() / 2.0, 0.0);
    bigBox->restitution = 0.7;
    bigBox->rotation = 1.4;
    bodies.push_back(bigBox);
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
            int x, y;
            SDL_GetMouseState(&x, &y);

            std::vector<Vec2> vertices =
            {
                Vec2(50, 80),
                Vec2(-30, 100),
                Vec2(-70, 40),
                Vec2(-50, -50),
                Vec2(0, -80),
                Vec2(60, -50),
                Vec2(90, 20)
            };

            Body* poly = new Body(PolygonShape(vertices), x, y, 2.0);
            poly->restitution = 0.1;
            poly->friction = 0.7;
            bodies.push_back(poly);
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Update function (called several times per second to update objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Update() {
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

    // Apply forces to the bodies
    for (auto body: bodies)
    {
    	// Apply a "weight" force to the body
        Vec2 weight = Vec2(0.0, body->mass * 9.8 * PIXELS_PER_METER);
        body->AddForce(weight);
    }

    // Integrate the acceleration and the velocity to find the new position
    for (auto body : bodies)
    {
        body->Update(deltaTime);
    }

    // Check all the rigidbodies with the other rigidbodies for collision
    for (int i = 0; i <= bodies.size() - 1; i++)
    {
	    for (int j = i + 1; j < bodies.size(); j++)
	    {
            Body* a = bodies[i];
            Body* b = bodies[j];

            a->isColliding = false;
            b->isColliding = false;

            Contact contact;

            if (CollisionDetection::IsColliding(a, b, contact))
            {
                // Resolve the collision using the projection method
                contact.ResolveCollision();

                if (debug)
                {
                    // Draw debug contact information
                    Graphics::DrawFillCircle(contact.start.x, contact.start.y, 3, 0xFFFF00FF);
                    Graphics::DrawFillCircle(contact.end.x, contact.end.y, 3, 0xFFFF00FF);
                    Graphics::DrawLine(contact.start.x, contact.start.y, contact.start.x + contact.normal.x * 15, contact.start.y + contact.normal.y * 15, 0xFFFF00FF);
                }
                
                a->isColliding = true;
                b->isColliding = true;
            }
	    }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Render function (called several times per second to draw objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Render() {
    // Draw all bodies
    for (auto body : bodies) 
    {
        if (body->shape->GetType() == CIRCLE)
        {
            CircleShape* circleShape = (CircleShape*) body->shape;
            Graphics::DrawCircle(body->position.x, body->position.y, circleShape->radius, body->rotation, 0xFF00FF00);
        }
        
        if (body->shape->GetType() == BOX)
        {
            BoxShape* boxShape = (BoxShape*) body->shape;
            Graphics::DrawPolygon(body->position.x, body->position.y, boxShape->worldVertices, 0xFF00FF00);
        }

        if (body->shape->GetType() == POLYGON)
        {
            PolygonShape* polygonShape = (PolygonShape*) body->shape;
            Graphics::DrawPolygon(body->position.x, body->position.y, polygonShape->worldVertices, 0xFF00FF00);
        }
    }

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