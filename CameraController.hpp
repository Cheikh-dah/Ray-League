#pragma once
#include "raylib.h"
#include "raymath.h"

class CameraController {
public:
    Camera3D camera;

private:
    float followSpeed;
    Vector3 offset; 

public:
    CameraController(Vector3 startPos, Vector3 startTarget, float smoothSpeed)
        : followSpeed(smoothSpeed)
    {
        camera.position   = startPos;
        camera.target     = startTarget;
        camera.up         = Vector3{ 0.0f, 1.0f, 0.0f };
        camera.fovy       = 45.0f;
        camera.projection = CAMERA_PERSPECTIVE;

        offset = Vector3Subtract(startPos, startTarget);
    }

    // Updated to accept exactly 2 arguments matching your main loop call
    void Update(Vector3 targetPosition, float deltaTime)
    {
        Vector3 desiredPosition = Vector3Add(targetPosition, offset);

        // Smoothly interpolate camera position toward the player
        camera.position = Vector3Lerp(camera.position, desiredPosition, followSpeed * deltaTime);

        // Lock camera focus onto the car
        camera.target = targetPosition;
    }
};