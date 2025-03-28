#ifndef APPLICATION_H
#define APPLICATION_H

#include "./Graphics.h"
#include "./Physics/World.h"
#include <vector>

class Application {
    private:
        const int WINDOW_WIDTH = 1440;
        const int WINDOW_HEIGHT = 900;

        bool debug = false;
        bool running = false;

        World* world;

        SDL_Texture* bgTexture;

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