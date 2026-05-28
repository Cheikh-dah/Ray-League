/*******************************************************************************************
*
* Rayket League - 3D Core Game Loop
*
********************************************************************************************/

#include "raylib.h"

// Define structs so the compiler knows what fields exist
struct Car3D {
    Vector3 position;
    Vector3 size;
    float speed;
    Color color;
};

struct Ball3D {
    Vector3 position;
    Vector3 velocity;
    float radius;
    Color color;
};

int main(void)
{
    // 1. Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Rayket League 3D");

    // Define the camera to look into our 3d world
    Camera3D camera = {};
    camera.position = Vector3{ 0.0f, 10.0f, 15.0f }; // Backed away up and out
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };     // Looking at center stage
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };         // Y-axis is up
    camera.fovy = 45.0f;                             // Field of view
    camera.projection = CAMERA_PERSPECTIVE;

    // Initialize 3D Entities properly
    Car3D player = {
        .position = Vector3{ -5.0f, 0.5f, 0.0f }, // Sitting on the grid floor
        .size = Vector3{ 2.0f, 1.0f, 3.0f },
        .speed = 15.0f,
        .color = BLUE
    };

    Ball3D ball = {
        .position = Vector3{ 0.0f, 1.0f, 0.0f },
        .velocity = Vector3{ 5.0f, 0.0f, -4.0f },
        .radius = 1.0f,
        .color = RED
    };

    int cameraMode = CAMERA_THIRD_PERSON;

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // 2. Main game loop
    while (!WindowShouldClose()) 
    {
        // --- UPDATE LOGIC ---
        float deltaTime = GetFrameTime();

        // 3D Movement Controls (Basic Axis Translation)
        if (IsKeyDown(KEY_W)) player.position.z -= player.speed * deltaTime;
        if (IsKeyDown(KEY_S)) player.position.z += player.speed * deltaTime;
        if (IsKeyDown(KEY_A)) player.position.x -= player.speed * deltaTime;
        if (IsKeyDown(KEY_D)) player.position.x += player.speed * deltaTime;

        // Ball Translation
        ball.position.x += ball.velocity.x * deltaTime;
        ball.position.z += ball.velocity.z * deltaTime;

        // Simple Arena Wall Deflections
        float arenaBoundary = 20.0f;
        if (ball.position.x + ball.radius >= arenaBoundary || ball.position.x - ball.radius <= -arenaBoundary) {
            ball.velocity.x *= -1.0f;
        }
        if (ball.position.z + ball.radius >= arenaBoundary || ball.position.z - ball.radius <= -arenaBoundary) {
            ball.velocity.z *= -1.0f;
        }

        // Update Camera positions to track player
        camera.target = player.position;
        UpdateCamera(&camera, cameraMode);

        // --- DRAWING LOGIC ---
        BeginDrawing();
            
            // Clear screen completely first
            ClearBackground(RAYWHITE); 

            // Everything here handles standard 3D rendering pipeline
            BeginMode3D(camera);
                
                DrawGrid(40, 1.0f); // Ground grid system
                
                // Draw Car
                DrawCubeV(player.position, player.size, player.color);
                DrawCubeWiresV(player.position, player.size, DARKBLUE);
                
                // Draw Ball
                DrawSphere(ball.position, ball.radius, ball.color);
                DrawSphereWires(ball.position, ball.radius, 16, 16, MAROON);

            EndMode3D(); 

            // Text rendering outside of 3D Mode back in screen pixel space
            DrawText("Rayket League 3D - WASD to Drive", 10, 10, 20, DARKGRAY);
            DrawFPS(10, 40);

        EndDrawing(); 
    }

    // 3. De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        
    //--------------------------------------------------------------------------------------

    return 0;
}
