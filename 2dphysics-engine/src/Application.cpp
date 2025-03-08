#include "Application.h"

#include "./Physics/Constants.h"
#include "./Physics/Force.h"
#include "./Physics/CollisionDetection.h"
#include "./Physics/Contact.h"
#include "./Physics/World.h"

bool Application::IsRunning() {
    return running;
}

///////////////////////////////////////////////////////////////////////////////
// Setup function (executed once in the beginning of the simulation)
///////////////////////////////////////////////////////////////////////////////
void Application::Setup() {
    running = Graphics::OpenWindow();

	// Create a physics world with gravity of -9.8 m/s^2
	world = new World(-9.8);

	// Add two bodies
	Body* a = new Body(CircleShape(30), Graphics::Width() / 2.0, Graphics::Height() / 2.0, 0.0f);
	Body* b = new Body(CircleShape(20), a->position.x - 100, a->position.y, 1.0f);
	world->AddBody(a);
	world->AddBody(b);
    
    // Add a joint constraint
	JointConstraint* joint = new JointConstraint(a, b, a->position);
	world->AddConstraint(joint);
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

                Body* ball = new Body(CircleShape(30), x, y, 1.0);
                ball->SetTexture("./assets/basketball.png");
                ball->restitution = 0.7;
                ball->friction = 0.1;

				world->AddBody(ball);
            }

            if (event.button.button == SDL_BUTTON_RIGHT) 
            {
                int x, y;
                SDL_GetMouseState(&x, &y);

                Body* box = new Body(BoxShape(60, 60), x, y, 1.0);
                box->SetTexture("./assets/crate.png");
                box->restitution = 0.2;
                
				world->AddBody(box);
            }

            if (event.button.button == SDL_BUTTON_MIDDLE)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);

                Body* bowBall = new Body(CircleShape(25), x, y, 20.0);
                bowBall->SetTexture("./assets/bowlingball.png");
                bowBall->restitution = 0.1;
                bowBall->friction = 0.1;

				world->AddBody(bowBall);
            }

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

	// Update world bodies (integration, collision detection & resolution, etc.)
	world->Update(deltaTime);
}

///////////////////////////////////////////////////////////////////////////////
// Render function (called several times per second to draw objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Render() {
    // Draw all bodies
    for (auto body : world->GetBodies()) 
    {
        if (body->shape->GetType() == CIRCLE)
        {
            CircleShape* circleShape = (CircleShape*) body->shape;

			if (!debug && body->texture)
			{
				Graphics::DrawTexture(body->position.x, body->position.y, circleShape->radius * 2, circleShape->radius * 2, body->rotation, body->texture);
			}
            else
            {
                Graphics::DrawCircle(body->position.x, body->position.y, circleShape->radius, body->rotation, 0xFF00FF00);
            }
        }
        
        if (body->shape->GetType() == BOX)
        {
            BoxShape* boxShape = (BoxShape*) body->shape;

            if (!debug && body->texture)
            {
				Graphics::DrawTexture(body->position.x, body->position.y, boxShape->width, boxShape->height, body->rotation, body->texture);
			}
            else
            {
                Graphics::DrawPolygon(body->position.x, body->position.y, boxShape->worldVertices, 0xFF00FF00);
            }
        }

        if (body->shape->GetType() == POLYGON)
        {
            PolygonShape* polygonShape = (PolygonShape*) body->shape;

            if (!debug)
            {
				Graphics::DrawFillPolygon(body->position.x, body->position.y, polygonShape->worldVertices, 0xFF00FF00);
			}
            else
            {
                Graphics::DrawPolygon(body->position.x, body->position.y, polygonShape->worldVertices, 0xFF00FF00);
            }
        }
    }

	Body* a = world->GetBodies()[0];
	Body* b = world->GetBodies()[1];
    Graphics::DrawLine(a->position.x, a->position.y, b->position.x, b->position.y, 0xFFFFFFFF);

    Graphics::RenderFrame();
}

///////////////////////////////////////////////////////////////////////////////
// Destroy function to delete objects and close the window
///////////////////////////////////////////////////////////////////////////////
void Application::Destroy() {
    delete world;

    Graphics::CloseWindow();
}