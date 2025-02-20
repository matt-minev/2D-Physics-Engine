#ifndef APPLICATION_H
#define APPLICATION_H

#include "./Graphics.h"
#include "./Physics/Particle.h"
#include <vector>

class Application {
    private:
        bool running = false;

        std::vector<Particle*> particles;
        Vec2 pushForce = Vec2(0.0, 0.0);

        Vec2 mouseCursor = Vec2(0, 0);
        bool leftMouseButtonDown = false;

        float k = 1500;
        float restLength = 200;

        const int NUM_PARTICLES = 4;

    public:
        Application() = default;
        ~Application() = default;
        bool IsRunning();
        void Setup();
        void Input();
        void Update();
        void Render();
        void Destroy();
};

#endif