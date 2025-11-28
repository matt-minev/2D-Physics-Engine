# Impact Engine – Usage Guide & Documentation

Impact Engine is a 2D game physics engine written in C++ that uses SDL for windowing and rendering along with SDL2_gfx for drawing primitives. It provides a complete framework for simulating rigid body dynamics, collision detection/resolution, and basic force interactions. This guide covers the engine’s architecture, details every class and function, and explains how to integrate and extend the engine.

---

## Table of Contents

1. [Introduction](#introduction)
2. [Project Structure Overview](#project-structure-overview)
3. [Getting Started](#getting-started)
4. [Module-by-Module Documentation](#module-by-module-documentation)  
   4.1 [Main Entry Point (Main.cpp)](#main-entry-point-maincpp)  
   4.2 [Application Class (Application.h / Application.cpp)](#application-class-applicationh--applicationcpp)  
   4.3 [Graphics Module (Graphics.h / Graphics.cpp)](#graphics-module-graphicsh--graphicscpp)  
   4.4 [Physics Module](#physics-module)  
   &nbsp;&nbsp;&nbsp;&nbsp;4.4.1 [World](#world)  
   &nbsp;&nbsp;&nbsp;&nbsp;4.4.2 [Body](#body)  
   &nbsp;&nbsp;&nbsp;&nbsp;4.4.3 [Force](#force)  
   &nbsp;&nbsp;&nbsp;&nbsp;4.4.4 [CollisionDetection](#collisiondetection)  
   &nbsp;&nbsp;&nbsp;&nbsp;4.4.5 [Contact](#contact)  
   4.5 [Shape Module (Shape.h / Shape.cpp)](#shape-module-shapeh--shapecpp)  
   4.6 [Vector Math (Vec2)](#vector-math-vec2)
5. [Usage and Examples](#usage-and-examples)
6. [Extending Impact Engine](#extending-impact-engine)
7. [Conclusion](#conclusion)

---

## Introduction

Impact Engine is designed to simulate 2D physics using rigid bodies that can be circles, boxes, or general convex polygons. It integrates forces (like gravity, wind, friction, and even springs) and handles collisions between bodies. A typical simulation follows this cycle:

- **Setup:** The engine initializes the graphics window, creates a physics world, and sets up static and dynamic bodies.
- **Input:** User input (mouse clicks and key presses) spawns new bodies or toggles debug modes.
- **Update:** Physics integration (applying forces, integrating velocities/positions, collision detection/resolution) is performed.
- **Render:** Bodies are drawn using either textures (if provided) or debug primitives.
- **Cleanup:** When the simulation ends, resources are freed and the window is closed.

---

## Project Structure Overview

The engine is organized into several files and modules:

- **Main.cpp:** Entry point that creates an Application object and runs the simulation loop.
- **Application:** Manages the overall simulation flow (setup, input handling, update, render, and destroy).
- **Graphics:** Contains static functions and variables to manage the SDL window, renderer, and drawing routines.
- **Physics Module:**
  - **World:** Holds all physics bodies, applies forces, updates physics, and performs collision checks.
  - **Body:** Represents a rigid body with physical properties (position, velocity, mass, etc.) and a pointer to its shape.
  - **Force:** Contains helper functions to generate forces (drag, friction, spring, gravitational).
  - **CollisionDetection:** Provides static methods for detecting collisions between bodies (circle-circle, polygon-polygon, polygon-circle).
  - **Contact:** Represents collision information (contact points, normal, penetration depth) and resolves collisions.
- **Shape Module:**
  - **Shape (Abstract):** Base class for all shapes.
  - **CircleShape:** Implements circular geometry.
  - **PolygonShape:** Implements convex polygons (also used for boxes).
  - **BoxShape:** Specialized polygon representing a rectangle.
- **Vec2:** Implements basic 2D vector mathematics including addition, subtraction, rotation, dot and cross products.

---

## Getting Started

To compile and run Impact Engine, follow the steps from Impact Engine's official GitHub repository. (Or the ones below.)

### Windows

1. **Visual Studio 2022:**

   - Download and install [Visual Studio 2022](https://visualstudio.microsoft.com/vs/). During installation, ensure you select the **Desktop Development with C++** workload so that the Visual C++ compiler is installed.

2. **Project Setup:**

   - You have two options:
     - **Option 1:** Download the complete Visual Studio project as provided.
     - **Option 2:** Create your own Visual Studio project and copy the `src`, `assets`, and `lib` folders into your project directory. Then add these folders to your project via the Visual Studio Solution Explorer.

3. **SDL2 & SDL2_image:**

   - Download the development libraries for Visual C++ from:
     - [SDL2 Download](https://github.com/libsdl-org/SDL/releases)
     - [SDL2_image Download](https://github.com/libsdl-org/SDL_image/releases)
   - Make sure to install the latest 2.x.x versions of both libraries and not 3.x.x.
   - Extract SDL2 to a global folder (e.g., `C:\SDL2`).
   - Extract SDL2_image and copy its contents into the same SDL2 folder, merging with the existing files. This ensures that both libraries reside under the `include` and `lib` directories.

4. **Linking SDL Libraries to the Project:**
   - Right-click on your project name in the **Solution Explorer** and choose **Properties**.
   - **Configuration:** Select the desired configuration (recommended: Debug Win32 to support 32-bit machines).
   - **VC++ Directories:**
     - **Include Directories:** Add `C:\SDL2\include`
     - **Library Directories:** Add `C:\SDL2\lib\x86` (using x86 ensures compatibility with 32-bit systems)
   - **Linker Settings:**
     - Go to **Linker → Input → Additional Dependencies** and add:
       - `SDL2.lib`
       - `SDL2main.lib`
       - `SDL2_image.lib`
   - **DLL Files:**
     - Copy the DLLs (`SDL2.dll` and `SDL2_image.dll`) from `C:\SDL2\lib\x86` (or x64, if applicable) into the same directory as your `.vcxproj` file.

---

### Linux

1. **Install Dependencies:**

   Open your terminal and run:

   ```bash
   sudo apt update
   sudo apt install build-essential
   sudo apt install libsdl2-dev
   sudo apt install libsdl2-image-dev
   sudo apt install libsdl2-gfx-dev
   ```

2. **Compile and Run:**

   Use the provided Makefile (or create one) to compile:

   ```bash
   make build
   make run
   ```

   Clean up with:

   ```bash
   make clean
   ```

---

### macOS

1. **Install Dependencies with Homebrew:**

   Open Terminal and run:

   ```bash
   brew update
   brew install sdl2
   brew install sdl2_image
   brew install sdl2_gfx
   ```

2. **Compile and Run:**

   Use the provided Makefile to build and run the engine:

   ```bash
   make build
   make run
   ```

   Clean up with:

   ```bash
   make clean
   ```

---

## Module-by-Module Documentation

### Main Entry Point (Main.cpp)

- **main() Function:**
  - **Purpose:**
    - Creates an instance of the `Application` class.
    - Calls the setup routine.
    - Enters a loop that continually processes input, updates the simulation, and renders the scene.
    - On exit, calls the destroy routine to clean up resources.
  - **Flow:**
    ```cpp
    int main(int argc, char *args[]) {
        Application app;
        app.Setup();
        while (app.IsRunning()) {
            app.Input();
            app.Update();
            app.Render();
        }
        app.Destroy();
        return 0;
    }
    ```

---

### Application Class (Application.h / Application.cpp)

The `Application` class orchestrates the overall simulation.

#### Data Members:

- **bool debug:**
  - Toggles debug mode (shows wireframes instead of textures).
- **bool running:**
  - Indicates whether the simulation is active.
- **World\* world:**
  - Pointer to the physics world containing all bodies.

#### Methods:

1. **IsRunning()**
   - **Signature:** `bool IsRunning();`
   - **Description:** Returns the current status of the simulation loop.
2. **Setup()**

   - **Purpose:** Initializes the simulation.
   - **Key Actions:**
     - Opens the graphics window via `Graphics::OpenWindow()`.
     - Creates a new `World` with gravity (note: a negative gravity value is expected, e.g. -9.8).
     - Adds static bodies (floor, left and right walls) to contain objects.
     - Adds a static box with a texture to act as an obstacle.
     - Applies a global wind force to all bodies.
   - **Code Highlights:**
     ```cpp
     void Application::Setup() {
         running = Graphics::OpenWindow();
         world = new World(-9.8);
         // Create floor and walls
         Body* floor = new Body(BoxShape(Graphics::Width() - 50, 50), Graphics::Width()/2.0, Graphics::Height()-50, 0.0);
         // ... similar for leftWall and rightWall, then add them to world.
         // Create a big static box with texture
         Body* bigBox = new Body(BoxShape(200, 200), Graphics::Width()/2.0, Graphics::Height()/2.0, 0.0);
         bigBox->SetTexture("./assets/crate.png");
         // Add wind force to all objects
         Vec2 wind(0.5 * PIXELS_PER_METER, 0.0);
         world->AddForce(wind);
     }
     ```

3. **Input()**

   - **Purpose:** Processes user input and events.
   - **Key Actions:**
     - Polls SDL events.
     - Exits simulation on SDL_QUIT or when the Escape key is pressed.
     - Toggles debug mode with the ‘D’ key.
     - On mouse button events:
       - **Left Click:** Spawns a circular body (ball) with basketball texture.
       - **Right Click:** Spawns a box body with crate texture.
       - **Middle Click:** Spawns a heavy circular body (“bowling ball”).
   - **Code Highlights:**
     ```cpp
     void Application::Input() {
         SDL_Event event;
         while (SDL_PollEvent(&event)) {
             // Handle quit and key events
             /// For mouse clicks, get mouse coordinates and create new Body objects.
         }
     }
     ```

4. **Update()**

   - **Purpose:** Advances the physics simulation.
   - **Key Actions:**
     - Clears the screen using `Graphics::ClearScreen()`.
     - Manages frame timing to maintain a fixed timestep (60 FPS).
     - Computes delta time and limits it to avoid large time steps.
     - Calls `world->Update(deltaTime)` which:
       - Applies forces (gravity, wind, etc.) to each body.
       - Integrates linear and angular motion.
       - Runs multiple iterations of collision detection and resolution.
   - **Code Highlights:**
     ```cpp
     void Application::Update() {
         Graphics::ClearScreen(0xFF0F0721);
         // Frame timing logic...
         float deltaTime = (SDL_GetTicks() - timePreviousFrame) / 1000.0f;
         // Clamp deltaTime
         world->Update(deltaTime);
     }
     ```

5. **Render()**

   - **Purpose:** Draws all bodies to the screen.
   - **Key Actions:**
     - Iterates over every body in the world.
     - Depending on the shape type and whether debug mode is active:
       - For circles: draws either a textured circle or a drawn circle with a line indicating rotation.
       - For boxes and polygons: either draws a textured quad or a wireframe polygon.
     - Finally, presents the rendered frame via `Graphics::RenderFrame()`.
   - **Code Highlights:**
     ```cpp
     void Application::Render() {
         for (auto body : world->GetBodies()) {
             if (body->shape->GetType() == CIRCLE) {
                 // Draw circle using texture or debug primitive.
             }
             // Similar handling for BOX and POLYGON.
         }
         Graphics::RenderFrame();
     }
     ```

6. **Destroy()**
   - **Purpose:** Cleans up all resources.
   - **Key Actions:**
     - Deletes the physics world and all its bodies.
     - Closes the SDL window via `Graphics::CloseWindow()`.
   - **Code Highlights:**
     ```cpp
     void Application::Destroy() {
         delete world;
         Graphics::CloseWindow();
     }
     ```

---

### Graphics Module (Graphics.h / Graphics.cpp)

The `Graphics` module encapsulates all SDL window and rendering operations.

#### Key Data Members (static):

- **windowWidth & windowHeight:** Current dimensions of the SDL window.
- **SDL_Window\* window:** The SDL window object.
- **SDL_Renderer\* renderer:** The SDL renderer object.

#### Main Functions:

- **Width() / Height():**
  - Return the window dimensions.
- **OpenWindow():**
  - Initializes SDL, obtains the current display mode, creates a borderless window, and sets up the renderer. Returns true on success.
- **CloseWindow():**
  - Cleans up the renderer, window, and quits SDL.
- **ClearScreen(Uint32 color):**
  - Clears the renderer using a specified color.
- **RenderFrame():**
  - Presents the rendered frame.
- **Draw Primitives:**
  - **DrawLine(), DrawCircle(), DrawFillCircle(), DrawRect(), DrawFillRect(), DrawPolygon(), DrawFillPolygon():**
    - Use SDL2_gfx functions to draw various shapes.
- **DrawTexture():**
  - Renders an SDL_Texture at a given position with rotation.

---

### Physics Module

This module contains all classes and methods for simulating physics.

#### World

- **Data Members:**
  - **G:** Gravity (note that the engine expects a negative value to represent downward force).
  - **std::vector\<Body\*\> bodies:** List of all bodies in the simulation.
  - **std::vector\<Vec2\> forces, std::vector\<float\> torques:** Global forces and torques to apply.
- **Key Methods:**
  - **Constructor:** Initializes the world with gravity.
  - **AddBody(Body\*):** Adds a new body to the simulation.
  - **GetBodies():** Returns a reference to the bodies vector.
  - **AddForce()/AddTorque():** Queues a force/torque to be applied.
  - **Update(float dt):**
    - Applies weight (gravity scaled by mass and PIXELS_PER_METER) and queued forces/torques to each body.
    - Calls each body’s `Update()` to perform integration.
    - Runs collision detection for multiple iterations to resolve penetrations.
  - **CheckCollisions():**
    - Iterates over all pairs of bodies.
    - Uses `CollisionDetection::IsColliding()` to determine collisions.
    - Calls `Contact::ResolveCollision()` to resolve detected collisions.

#### Body

Represents an individual rigid body.

- **Data Members:**
  - **Position, velocity, acceleration (Vec2):** Linear motion parameters.
  - **Rotation, angularVelocity, angularAcceleration (float):** Angular motion parameters.
  - **sumForces (Vec2) and sumTorque (float):** Accumulated forces and torques.
  - **mass, invMass:** Mass properties (with invMass being zero for static bodies).
  - **I, invI:** Moment of inertia and its inverse.
  - **restitution and friction:** Coefficients controlling collision response.
  - **Shape\* shape:** Pointer to the geometry (circle, polygon, or box).
  - **SDL_Texture\* texture:** Optional texture for rendering.
  - **isColliding:** Flag used during collision checks.
- **Key Methods:**
  - **Constructor:** Clones the shape, initializes motion parameters, calculates inverse mass and inertia.
  - **Destructor:** Frees the shape and destroys the texture.
  - **SetTexture(const char\*):** Loads an image file and creates an SDL texture.
  - **IsStatic():** Checks if the body is static (invMass ≈ 0).
  - **AddForce() / AddTorque():** Accumulates forces and torque.
  - **ClearForces() / ClearTorque():** Resets the accumulated values.
  - **ApplyImpulse():** Applies an impulse directly to velocity (with an overload that applies angular impulse based on an offset vector).
  - **IntegrateLinear(float dt):**
    - Computes acceleration from forces.
    - Uses implicit Euler integration to update velocity and position.
    - Clears forces after integration.
  - **IntegrateAngular(float dt):**
    - Computes angular acceleration from torque.
    - Updates angular velocity and rotation.
    - Clears torque after integration.
  - **Update(float dt):**
    - Calls both integration routines.
    - Updates the shape’s world vertices using the current rotation and position.

#### Force

Contains helper functions to generate different force types:

- **GenerateDragForce(const Body\&, float k):**
  - Returns a drag force proportional to the square of the velocity.
- **GenerateFrictionForce(const Body\&, float k):**
  - Returns a constant friction force opposite to the direction of motion.
- **GenerateSpringForce():**
  - Overloaded to compute a spring force given an anchor point or between two bodies.
- **GenerateGravitationalForce():**
  - Computes the gravitational attraction between two bodies, clamping the distance for visual effects.

#### CollisionDetection

Provides static methods to test collisions:

- **IsColliding(Body\* a, Body\* b, Contact\& contact):**
  - Routes to the correct collision method based on shape types.
- **IsCollidingCircleCircle(), IsCollidingPolygonPolygon(), IsCollidingPolygonCircle():**
  - Implement specific collision detection algorithms.
  - When a collision is detected, they populate a `Contact` structure with collision normal, depth, and contact points.

#### Contact

Represents collision details and resolves collisions.

- **Data Members:**
  - Pointers to the colliding bodies, contact start/end points, collision normal, and penetration depth.
- **Methods:**
  - **ResolvePenetration():**
    - Moves bodies apart based on their inverse masses to correct interpenetration.
  - **ResolveCollision():**
    - First calls positional correction.
    - Then calculates collision impulses along the normal and tangent to resolve both elasticity (restitution) and friction.
    - Applies the impulse to both bodies.

---

### Shape Module (Shape.h / Shape.cpp)

This module defines the abstract base for geometric shapes and their concrete implementations.

#### Shape (Abstract Base Class)

- **Pure Virtual Methods:**
  - `GetType() const`: Returns the shape type (CIRCLE, POLYGON, BOX).
  - `Clone() const`: Creates a copy of the shape.
  - `UpdateVertices(float angle, const Vec2& position)`: Updates vertices (if applicable) based on rotation and translation.
  - `GetMomentOfInertia() const`: Computes the moment of inertia for the shape.

#### CircleShape

- **Data Members:**
  - `float radius`: The radius of the circle.
- **Methods:**
  - **Constructor/Destructor:**
    - Logs creation and destruction (for debugging).
  - **GetType():**
    - Returns CIRCLE.
  - **Clone():**
    - Returns a new CircleShape with the same radius.
  - **UpdateVertices():**
    - Does nothing since circles do not have vertices.
  - **GetMomentOfInertia():**
    - Returns 0.5 \* (radius²) (to be multiplied by mass later).

#### PolygonShape

- **Data Members:**
  - `std::vector<Vec2> localVertices`: Vertices defined in the object’s local space.
  - `std::vector<Vec2> worldVertices`: Transformed vertices in world space.
- **Methods:**
  - **Constructor:**
    - Initializes vertices from a given list.
  - **Destructor:**
    - Logs destruction.
  - **GetType():**
    - Returns POLYGON.
  - **Clone():**
    - Creates a new PolygonShape with the same vertices.
  - **EdgeAt(int index):**
    - Returns the edge (vector difference) between a vertex and its next neighbor.
  - **FindMinSeparation():**
    - Used in collision detection to determine penetration depth and collision normal.
  - **UpdateVertices(float angle, const Vec2& position):**
    - Rotates each local vertex and then translates it by the body’s position.
  - **GetMomentOfInertia():**
    - (Currently a placeholder returning 5000.0f; intended to be computed properly.)

#### BoxShape

- **Derived from PolygonShape:**
  - Represents a rectangle.
- **Data Members:**
  - `float width, height`: Dimensions of the box.
- **Methods:**
  - **Constructor:**
    - Initializes local vertices corresponding to a centered rectangle.
  - **GetType():**
    - Returns BOX.
  - **Clone():**
    - Creates a new BoxShape with the same dimensions.
  - **GetMomentOfInertia():**
    - Returns (1/12) \* (w² + h²) (again to be multiplied by mass).

---

### Vector Math (Vec2)

The `Vec2` class provides basic 2D vector operations that are used throughout the engine.

#### Data Members:

- `float x, y`: The components of the vector.

#### Key Methods:

- **Constructors:**
  - Default and parameterized constructors.
- **Add(), Sub(), Scale():**
  - In-place operations modifying the vector.
- **Rotate(float angle):**
  - Returns a new vector rotated by a given angle (in radians).
- **Magnitude() and MagnitudeSquared():**
  - Compute the length (or its square) of the vector.
- **Normalize() and UnitVector():**
  - Normalize the vector (in place or returning a normalized copy).
- **Normal():**
  - Returns a perpendicular (normal) vector.
- **Dot() and Cross():**
  - Compute the dot and cross products.
- **Operator Overloads:**
  - Support arithmetic operators (`+`, `-`, `*`, `/`, and their assignment versions) and equality comparisons.

---

## Usage and Examples

Once compiled, running Impact Engine opens a full-screen window where the simulation takes place. Here’s how to interact with the engine:

- **Spawning Bodies via Mouse:**
  - **Left Mouse Button:**
    - Creates a dynamic circular body (“ball”) with a basketball texture.
  - **Right Mouse Button:**
    - Creates a dynamic box body with a crate texture.
  - **Middle Mouse Button:**
    - Creates a heavier circular body (bowling ball) with its respective texture.
- **Keyboard Commands:**
  - **Escape Key / Window Close:**
    - Exits the simulation.
  - **‘D’ Key:**
    - Toggles debug mode to switch between textured rendering and wireframe (debug) rendering.
- **Physics Behavior:**
  - Gravity (set during world creation) and additional forces (e.g., wind applied in `Setup()`) affect all dynamic bodies.
  - Collisions between bodies are detected and resolved over multiple iterations each update, ensuring stable physics responses.

---

## Extending Impact Engine

Impact Engine is designed to be modular. To extend its functionality, consider the following:

- **Custom Forces:**
  - Add new force generation functions to the `Force` module.
- **Additional Shapes:**
  - Derive new shapes from the abstract `Shape` class.
- **Improved Collision Detection:**
  - Enhance the algorithms in `CollisionDetection` or implement new ones for concave shapes.
- **Graphics Enhancements:**
  - Integrate new drawing methods in the `Graphics` module or add support for different rendering effects.
- **User Input:**
  - Modify the `Application::Input()` method to include more interactive controls.

---

## Conclusion

Impact Engine provides a complete and extensible framework for 2D physics simulation. By understanding the roles of each module—from window management in Graphics to the physics simulation in World and Body—you can both use the engine in your projects and extend it as needed. This documentation has covered every function, method, and variable, ensuring that you have a full understanding of how Impact Engine works.

Feel free to consult this guide as you build new simulations or modify the engine to suit your project’s needs.
