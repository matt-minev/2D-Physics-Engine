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

    // Add a floor to stop falling objects
	Body* floor = new Body(BoxShape(Graphics::Width() - 50, 50), Graphics::Width() / 2.0, Graphics::Height() - 50, 0.0);
    floor->restitution = 0.2;
    bodies.push_back(floor);

    // Add a wall to stop falling objects
    Body* wallLeft = new Body(BoxShape(20, Graphics::Height()-20), 0, Graphics::Height() / 2.0, 0.0);
    floor->restitution = 0.2;
    bodies.push_back(wallLeft);

    // Add a wall to stop falling objects
    Body* wallRight = new Body(BoxShape(20, Graphics::Height()-20), Graphics::Width(), Graphics::Height() / 2, 0.0);
    floor->restitution = 0.2;
    bodies.push_back(wallRight);

	// Add a static box so other boxes can collide
	Body* bigBox = new Body(BoxShape(200, 200), Graphics::Width() / 2.0, Graphics::Height() / 2.0, 0.0);
    bigBox->rotation = 1.4;
    bigBox->restitution = 0.5;
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
            break;
        case SDL_MOUSEBUTTONDOWN:
            int x, y;
            SDL_GetMouseState(&x, &y);
			Body* box = new Body(BoxShape(50, 50), x, y, 1.0);
            bodies.push_back(box);
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
     
     //    // Apply a "wind" force to the body
     //    Vec2 wind = Vec2(2.0 * PIXELS_PER_METER, 0.0);
     //    body->AddForce(wind);
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

                // Draw debug contact information
                Graphics::DrawFillCircle(contact.start.x, contact.start.y, 3, 0xFFFF00FF);
                Graphics::DrawFillCircle(contact.end.x, contact.end.y, 3, 0xFFFF00FF);
                Graphics::DrawLine(contact.start.x, contact.start.y, contact.start.x + contact.normal.x * 15, contact.start.y + contact.normal.y * 15, 0xFFFF00FF);

                a->isColliding = true;
                b->isColliding = true;
            }
	    }
    }

    // Hardcoded flip in velocity if the body touches the limits of the screen window
    // Not real physics simulation
    for (auto body : bodies)
    {
        if (body->shape->GetType() == CIRCLE)
        {
            CircleShape* circleShape = (CircleShape*) body->shape;
            if (body->position.x - circleShape->radius <= 0)
            {
                body->position.x = circleShape->radius;
                body->velocity.x *= -0.9;
            }
            else if (body->position.x + circleShape->radius >= Graphics::Width())
            {
                body->position.x = Graphics::Width() - circleShape->radius;
                body->velocity.x *= -0.9;
            }

            if (body->position.y - circleShape->radius <= 0)
            {
                body->position.y = circleShape->radius;
                body->velocity.y *= -0.9;
            }
            else if (body->position.y + circleShape->radius >= Graphics::Height())
            {
                body->position.y = Graphics::Height() - circleShape->radius;
                body->velocity.y *= -0.9;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Render function (called several times per second to draw objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Render() {
    // Draw all bodies
    for (auto body : bodies) {
        Uint32 color = body->isColliding ? 0xFF0000FF : 0xFFFFFFFF;

        if (body->shape->GetType() == CIRCLE)
        {
            CircleShape* circleShape = (CircleShape*) body->shape;
            Graphics::DrawFillCircle(body->position.x, body->position.y, circleShape->radius, color);
        }
        
        if (body->shape->GetType() == BOX)
        {
            BoxShape* boxShape = (BoxShape*)body->shape;
            Graphics::DrawPolygon(body->position.x, body->position.y, boxShape->worldVertices, color);
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