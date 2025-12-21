#include <raylib.h>
#include <raymath.h>

constexpr float GRAVITY = 0.1f;
float acceleration = 10.f;
float speed = 0.0f;
int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Isometric Camera Demo");

    // Isometric / orthographic camera
    Camera camera = {0};
    camera.position = {10.0f, 10.0f, 10.0f};  // Camera position in world
    camera.target   = {0.0f, 0.0f, 0.0f};    // Where the camera looks
    camera.up       = {0.0f, 1.0f, 0.0f};    // Up vector
    camera.fovy     = 45.0f;                 // Field of view (unused in ortho)

    Vector3 cubePos = {0.0f, 1.0f, 0.0f}; // Starting position of a cube

    Vector3 platformDim ={ 1.0f, 0.1f, 1.0f};   // Define platform position
    Vector3 platformPos = {0.0f, 5.0f, 0.0f};   // Define platform dimensions

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Simple movement controls
        if (IsKeyDown(KEY_W)) cubePos.z -= 0.1f;
        if (IsKeyDown(KEY_S)) cubePos.z += 0.1f;
        if (IsKeyDown(KEY_A)) cubePos.x -= 0.1f;
        if (IsKeyDown(KEY_D)) cubePos.x += 0.1f;

        // Apply gravity
        speed -= GRAVITY;

        // Jump
        if (IsKeyPressed(KEY_SPACE) && cubePos.y == 1.0f)
            speed = 1.0f;

        // Predict next position
        float nextY = cubePos.y + speed;

        // Ground collision
        if (nextY < 1.0f) {
            cubePos.y = 1.0f;
            speed = 0.0f;
        } else {
            cubePos.y = nextY;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        DrawCube(cubePos, 1.0f, 1.0f, 1.0f, RED);
        DrawCubeWires(cubePos, 1.0f, 1.0f, 1.0f, MAROON);
        DrawGrid(10, 1.0f); // 10x10 grid

        // Drawing the platform
        DrawCube(platformPos, platformDim.x, platformDim.y, platformDim.z,YELLOW);

        // Determine the coordinates occupied by the platform for collision
        Vector3 platformMaxCoord = platformPos + platformDim;
        Vector3 platformMinCoord = platformPos - platformDim;
        // Collision check
        if (cubePos.x >= platformMinCoord.x && cubePos.x <= platformMaxCoord.x &&
            cubePos.y >= platformMinCoord.y && cubePos.y <= platformMaxCoord.y &&
            cubePos.z >= platformMinCoord.z && cubePos.z <= platformMaxCoord.z) {
            speed = 0.0f;
        }




        EndMode3D();
        DrawText("Use WASD to move the cube", 10, 10, 20, DARKGRAY);
        DrawFPS(600, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}