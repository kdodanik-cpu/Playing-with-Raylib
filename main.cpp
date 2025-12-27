#include <raylib.h>
#include <raymath.h>
#include <iostream>

constexpr float GRAVITY = 0.1f;
float acceleration = 10.f;
Vector3 speed = {0.0f, 0.0f, 0.0f};

struct Cube {
    Vector3 position;
    Vector3 dimensions;

    Cube() = delete; // Remove default constructor
    // According to ChatGPT using const references is more efficient (though it doesn't matter that much right now)
    Cube(const Vector3& pos, const Vector3& dims): position(pos), dimensions(dims) {};

    void Draw(Color const color) const {
        DrawCube(position, dimensions.x, dimensions.y, dimensions.z, color);
        DrawCubeWires(position, dimensions.x, dimensions.y, dimensions.z, BLACK);
    }

    Vector3 getMinCoord() const {return position - dimensions * 0.05f;}
    Vector3 getMaxCoord() const {return position + dimensions * 0.05f;}

};

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Isometric Camera Demo");

    // Create assets
    // Isometric / orthographic camera
    Camera camera = {0};
    camera.position = {10.0f, 10.0f, 10.0f};  // Camera position in world
    camera.target   = {0.0f, 0.0f, 0.0f};    // Where the camera looks
    camera.up       = {0.0f, 1.0f, 0.0f};    // Up vector
    camera.fovy     = 45.0f;                 // Field of view (unused in ortho)

    // Cube
    Vector3 cubePos = {0.0f, 1.0f, 0.0f};
    Vector3 cubeDim = {1.0f, 1.0f, 1.0f};
    Cube cube  = {cubePos, cubeDim};

    // Platform
    Vector3 platformDim ={ 1.0f, 0.1f, 1.0f};
    Vector3 platformPos = {0.0f, 5.0f, 0.0f};
    Cube platform  = {platformPos, platformDim};
    Vector3 nextPos = cubePos;

    SetTargetFPS(60);


    while (!WindowShouldClose()) {

        cube.position = cubePos;

        // Simple movement controls
        if (IsKeyDown(KEY_W)) speed.z = -10.0f;
        else if (IsKeyDown(KEY_S)) speed.z = 10.0f;
        else speed.z = 0.0f;
        if (IsKeyDown(KEY_A)) speed.x = -10.0f;
        else if (IsKeyDown(KEY_D)) speed.x = 10.0f;
        else speed.x = 0.0f;
        if (IsKeyDown(KEY_Z)) speed.y = 10.0f;
        else if (IsKeyDown(KEY_X)) speed.y = -10.0f;
        else speed.y = 0.0f;

        // Handle movement and collision
        nextPos.x = cubePos.x + speed.x * GetFrameTime();
        if (nextPos.x + cube.dimensions.x * 0.5f > platform.position.x - platform.dimensions.x * 0.5f &&
            nextPos.x - cube.dimensions.x * 0.5f < platform.position.x + platform.dimensions.x * 0.5f &&
            nextPos.z + cube.dimensions.z * 0.5f > platform.position.z - platform.dimensions.z * 0.5f &&
            nextPos.z - cube.dimensions.z * 0.5f < platform.position.z + platform.dimensions.z * 0.5f &&
            nextPos.y + cube.dimensions.y * 0.5f > platform.position.y - platform.dimensions.y * 0.5f &&
            nextPos.y - cube.dimensions.y * 0.5f < platform.position.y + platform.dimensions.y * 0.5f
            ) {
            if (speed.x < 0.0f)
                {cubePos.x = platform.position.x + platform.dimensions.x * 0.5f + cube.dimensions.x * 0.5f; }
            else if (speed.x > 0.0f)
                {cubePos.x = platform.position.x - platform.dimensions.x * 0.5f - cube.dimensions.x * 0.5f; }
            nextPos.x = cubePos.x;
            speed.x = 0.0f;
        } else cubePos.x = nextPos.x;

        nextPos.z = cubePos.z + speed.z * GetFrameTime();
        if (nextPos.x + cube.dimensions.x * 0.5f > platform.position.x - platform.dimensions.x * 0.5f &&
            nextPos.x - cube.dimensions.x * 0.05 < platform.position.x + platform.dimensions.x * 0.5f &&
            nextPos.z + cube.dimensions.z * 0.5f > platform.position.z - platform.dimensions.z * 0.5f &&
            nextPos.z - cube.dimensions.z * 0.5f < platform.position.z + platform.dimensions.z * 0.5f &&
            nextPos.y + cube.dimensions.y * 0.5f > platform.position.y - platform.dimensions.y * 0.5f &&
            nextPos.y - cube.dimensions.y * 0.5f < platform.position.y + platform.dimensions.y * 0.5f) {

            if (speed.z < 0.0f)
            {cubePos.z = platform.position.z + platform.dimensions.z * 0.5f + cube.dimensions.z * 0.5f; }
            else if (speed.z > 0.0f) {cubePos.z = platform.position.z - platform.dimensions.z * 0.5f - cube.dimensions.z * 0.5f; }
            speed.z = 0.0f;
            nextPos.z = cubePos.z;
        } else cubePos.z = nextPos.z;

        nextPos.y = cubePos.y + speed.y * GetFrameTime();
        if (nextPos.x + cube.dimensions.x * 0.5f > platform.position.x - platform.dimensions.x * 0.5f &&
            nextPos.x - cube.dimensions.x * 0.05 < platform.position.x + platform.dimensions.x * 0.5f &&
            nextPos.z + cube.dimensions.z * 0.5f > platform.position.z - platform.dimensions.z * 0.5f &&
            nextPos.z - cube.dimensions.z * 0.5f < platform.position.z + platform.dimensions.z * 0.5f &&
            nextPos.y + cube.dimensions.y * 0.5f > platform.position.y - platform.dimensions.y * 0.5f &&
            nextPos.y - cube.dimensions.y * 0.5f < platform.position.y + platform.dimensions.y * 0.5f) {

            if (speed.y < 0.0f) {cubePos.y =  platform.position.y + platform.dimensions.y * 0.5f + cube.dimensions.y * 0.5f; }
            else if (speed.y > 0.0f) {cubePos.y = platform.position.y - platform.dimensions.y * 0.5f - cube.dimensions.y * 0.5f; }
            speed.y = 0.0f;
            nextPos.y = cubePos.y;
        } else cubePos.y = nextPos.y;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);
        cube.Draw(RED);
        platform.Draw(YELLOW);
        DrawGrid(10, 1.0f); // 10x10 grid
        EndMode3D();
        DrawText("Use WASD to move the cube", 10, 10, 20, DARKGRAY);
        DrawFPS(600, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}