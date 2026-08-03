/*******************************************************************************************
 *
 * Rayket League - 3D Core Game Loop (Production Fixed-Timestep Configuration)
 *
 ********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "CameraController.hpp"

struct Car3D {
    Vector3 position;
    Quaternion orientation; // Added to support clean structural 3D rotations
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
    // 1. Initialization Context
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    InitWindow(screenWidth, screenHeight, "Rayket League 3D - Fixed Accumulator");
    
    // Initialize your decoupled camera component
    CameraController studioCam(Vector3{ 0.0f, 10.0f, 15.0f }, Vector3{ 0.0f, 0.0f, 0.0f }, 5.0f);
    
    // Initialize 3D Entities
    Car3D player = {
        .position = Vector3{ -5.0f, 0.5f, 0.0f }, 
        .orientation = QuaternionIdentity(), // Facing default direction array alignment
        .size = Vector3{ 2.0f, 1.0f, 3.0f },
        .speed = 15.0f,
        .color = BLUE
    };
    
    Ball3D ball = {
        .position = Vector3{ 0.0f, 1.0f, 0.0f },
        .velocity = Vector3{ 12.0f, 0.0f, -8.0f }, // Adjusted velocity tracking performance
        .radius = 1.0f,
        .color = RED
    };
    
    const float arenaBoundary = 20.0f;
    
    // Fixed Time-Step Engine Accumulator Setup (Satisfies Section 3.1 of PRD)
    float dtAccumulator = 0.0f;
    const float physicsTimeStep = 1.0f / 120.0f; // Locked 120Hz physics tick kernel
    
    SetTargetFPS(60); // Visual framerate remains completely variable
    //--------------------------------------------------------------------------------------
    
    // 2. Main Game Loop Execution Architecture
    while (!WindowShouldClose()) 
    {
        // Capture frame slice processing time and append it to the accumulator balance pool
        dtAccumulator += GetFrameTime();
        
        // --- FIXED TICK DETERMINISTIC SIMULATION PASS ---
        // This execution loop guarantees perfectly repeatable steps regardless of frame drops
        while (dtAccumulator >= physicsTimeStep)
        {
            // --- 1. CAPTURE & CONSTRUCT MOVEMENT DIRECTION ---
            Vector3 camForward = Vector3Normalize(Vector3Subtract(studioCam.camera.target, studioCam.camera.position));
            camForward.y = 0.0f; // Flatten tracking vector to keep car bound to the arena floor
            camForward = Vector3Normalize(camForward);
            
            Vector3 camRight = Vector3CrossProduct(camForward, Vector3{ 0.0f, 1.0f, 0.0f });
            Vector3 targetMoveDir = Vector3Zero();
            
            // Accumulate clean input vectors
            if (IsKeyDown(KEY_W)) targetMoveDir = Vector3Add(targetMoveDir, camForward);
            if (IsKeyDown(KEY_S)) targetMoveDir = Vector3Subtract(targetMoveDir, camForward);
            if (IsKeyDown(KEY_A)) targetMoveDir = Vector3Subtract(targetMoveDir, camRight);
            if (IsKeyDown(KEY_D)) targetMoveDir = Vector3Add(targetMoveDir, camRight);
            
            // --- 2. EVALUATE STEP POSITION & ROTATION KINEMATICS ---
            if (Vector3LengthSqr(targetMoveDir) > 0.0f) 
            {
                targetMoveDir = Vector3Normalize(targetMoveDir);
                
                // Position calculations are linked to the constant simulation frame slice step
                player.position = Vector3Add(player.position, Vector3Scale(targetMoveDir, player.speed * physicsTimeStep));
                
                // Compute look rotation vector matrix and assign to spatial quaternion structure
                float targetAngle = atan2f(targetMoveDir.x, targetMoveDir.z);
                player.orientation = QuaternionFromAxisAngle(Vector3{ 0.0f, 1.0f, 0.0f }, targetAngle);
            }
            
            // Fixed Ball Translation Step
            ball.position = Vector3Add(ball.position, Vector3Scale(ball.velocity, physicsTimeStep));
            
            // Simple Arena Wall Deflections (Ball)
            if (ball.position.x + ball.radius >= arenaBoundary)  { ball.position.x = arenaBoundary - ball.radius;  ball.velocity.x *= -1.0f; }
            if (ball.position.x - ball.radius <= -arenaBoundary) { ball.position.x = -arenaBoundary + ball.radius; ball.velocity.x *= -1.0f; }
            if (ball.position.z + ball.radius >= arenaBoundary)  { ball.position.z = arenaBoundary - ball.radius;  ball.velocity.z *= -1.0f; }
            if (ball.position.z - ball.radius <= -arenaBoundary) { ball.position.z = -arenaBoundary + ball.radius; ball.velocity.z *= -1.0f; }
            
            // Arena Boundaries Clamp (Player AABB bounding box)
            if (player.position.x + player.size.x / 2.0f >= arenaBoundary)  player.position.x = arenaBoundary - player.size.x / 2.0f;
            if (player.position.x - player.size.x / 2.0f <= -arenaBoundary) player.position.x = -arenaBoundary + player.size.x / 2.0f;
            if (player.position.z + player.size.z / 2.0f >= arenaBoundary)  player.position.z = arenaBoundary - player.size.z / 2.0f;
            if (player.position.z - player.size.z / 2.0f <= -arenaBoundary) player.position.z = -arenaBoundary + player.size.z / 2.0f;
            
            // Subtract exact timestep cost parameter from accumulator balance
            dtAccumulator -= physicsTimeStep;
        }
        
        // --- VARIABLE RENDERING POST-PROCESSING & UPDATES ---
        // Camera smoothly tracks our calculated, fixed position coordinates
        studioCam.Update(player.position, GetFrameTime());
        
        BeginDrawing();
        
            ClearBackground(RAYWHITE); 
            
            BeginMode3D(studioCam.camera);
            
                // Draw a nice green stadium pitch base layout
                DrawCube(Vector3{ 0.0f, -0.05f, 0.0f }, 40.0f, 0.1f, 40.0f, DARKGREEN);
                DrawGrid(40, 1.0f); 
                
                // Render Car using its Orientation Transform Matrix
                Matrix carTransform = QuaternionToMatrix(player.orientation);
                carTransform.m12 = player.position.x;
                carTransform.m13 = player.position.y;
                carTransform.m14 = player.position.z;
                
                PushMatrix();
                    // Concat our visual model location with our active view matrix context
                    SetMatrixModelview(MatrixMultiply(GetMatrixModelview(), carTransform));
                    DrawCube(Vector3Zero(), player.size.x, player.size.y, player.size.z, player.color);
                    DrawCubeWires(Vector3Zero(), player.size.x, player.size.y, player.size.z, DARKBLUE);
                PopMatrix();
                
                // Draw Ball
                DrawSphere(ball.position, ball.radius, ball.color);
                DrawSphereWires(ball.position, ball.radius, 16, 16, MAROON);
                
            EndMode3D(); 
            
            // 2D Interface Information Matrix
            DrawText("Rayket League 3D Engine - Core Fixed Target System", 10, 10, 20, DARKGRAY);
            DrawText(TextFormat("Simulation Rate: 120Hz Fixed Loop | Car Vector Coord: [X: %.2f, Z: %.2f]", player.position.x, player.position.z), 10, 35, 14, GRAY);
            DrawFPS(10, 55);
            
        EndDrawing(); 
    }
    
    // 3. De-Initialization Pipeline
    CloseWindow();        
    return 0;
}

