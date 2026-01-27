#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <vector>
#include <ranges>


// The most basic unit will be objects
struct Object {
    // Member variables
    Vector3 position;
    Vector3 dimensions;
    Color color;
    Vector3 speed;
    bool isResting;
    bool doesCollide;
    // Constructor
    Object(
        Vector3 pos,
        Vector3 dims,
        Color col,
        Vector3 speed ={0, 0, 0},
        bool resting = false,
        bool collides = false)
        :
    position(pos),
    dimensions(dims),
    speed(speed),
    color(col)
     {
         isResting = resting;
         doesCollide = collides;
     }
};

class MovementSystem {
    public:
    static void HandleMovement(Object& A, Object& B) {
        Vector3 minA = A.position - A.dimensions * 0.5f;
        Vector3 maxA = A.position + A.dimensions * 0.5f;
        Vector3 minB = B.position - B.dimensions * 0.5f;
        Vector3 maxB = B.position + B.dimensions * 0.5f;

        Vector3 nextMinA = minA + A.speed * GetFrameTime();
        Vector3 nextMaxA = maxA + A.speed * GetFrameTime();
        Vector3 nextMinB = minB + B.speed * GetFrameTime();
        Vector3 nextMaxB = maxB + B.speed * GetFrameTime();

        // X resolution
        // Other axes alligned to allow colision next frame?
        if (nextMaxA.y > nextMinB.y &&
            nextMinA.y < nextMaxB.y &&
            nextMaxA.z > nextMinB.z &&
            nextMinA.z < nextMaxB.z) {
            const float EPS = 0.001f;
            if (
                maxA.x <= minB.x - EPS &&
                nextMaxA.x > nextMinB.x) A.speed.x = 0.0f;
            else if (
                minA.x >= maxB.x - EPS &&
                nextMinA.x < nextMaxB.x) A.speed.x = 0.0f;
        }
        A.position.x += A.speed.x * GetFrameTime();

        // Z resolution
        // Other axes alligned to allow colision next frame?
        if (nextMaxA.y > nextMinB.y &&
            nextMinA.y < nextMaxB.y &&
            nextMaxA.x > nextMinB.x &&
            nextMinA.x < nextMaxB.x) {
            const float EPS = 0.001f;
            if (
                maxA.z <= minB.z - EPS &&
                nextMaxA.z > nextMinB.z) A.speed.z = 0.0f;
            else if (
                minA.z >= maxB.z - EPS &&
                nextMinA.z < nextMaxB.z) A.speed.z = 0.0f;
            }
        A.position.z += A.speed.z * GetFrameTime();

        // Y resolution
        // Other axes alligned to allow colision next frame?
        if (nextMaxA.z > nextMinB.z &&
            nextMinA.z < nextMaxB.z &&
            nextMaxA.x > nextMinB.x &&
            nextMinA.x < nextMaxB.x) {
            const float EPS = 0.001f;
            if (
                maxA.y <= minB.y - EPS &&
                nextMaxA.y > nextMinB.y) A.speed.z = 0.0f;
            else if (
                minA.y >= maxB.y - EPS &&
                nextMinA.y < nextMaxB.y) A.speed.z = 0.0f;
            }
        A.position.y += A.speed.y * GetFrameTime();

    }
};

class Renderer {
    public :
     static void Draw(const std::vector<Object*>& objects) {
         for (Object* obj : objects) {
             DrawCube(obj->position, obj->dimensions.x, obj->dimensions.y, obj->dimensions.z, obj->color);
         }
    }
};

// Functions to handle movement
Vector3 GetCameraForwardXZ(const Camera& cam)
{
    Vector3 forward = Vector3Subtract(cam.target, cam.position);
    forward.y = 0.0f;
    return Vector3Normalize(forward);
}

Vector3 GetCameraRightXZ(const Camera& cam)
{
    Vector3 forward = GetCameraForwardXZ(cam);
    return Vector3Normalize(Vector3CrossProduct(forward, {0.0f, 1.0f, 0.0f}));
}

const float MIN_HEIGHT = 0.5f;

int main() {
    const int screenWidth = 1500;
    const int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "Isometric Camera Demo");

    // Create assets
    // Isometric / orthographic camera
    Camera camera = {0};
    camera.position = { 11.0f, 11.0f, 11.0f };   // equal X, Y, Z
    camera.target   = { 0.0f, 0.0f, 0.0f };
    camera.up       = { 0.0f, 1.0f, 0.0f };
    camera.fovy     = 15.0f;                    // ignored for orthographic
    camera.projection = CAMERA_ORTHOGRAPHIC;
    // Player Cube
    Vector3 cubePos = {0.0f, 1.0f, 0.0f};
    Vector3 cubeDim = {0.5f, 1.0f, 0.5f};
    Object player = {cubePos, cubeDim, RED, {0.0f, 0.0f , 0.0f}};

    // Colliders
    Vector3 groundDimensions = {30.0f, 0.05f, 30.0f};
    Vector3 groundPos = {0.0f, 0.475f, 0.0f};
    Object collider({5.0f, 0.5f, 5.0f}, cubeDim, YELLOW);

    std::vector<Object*> objects = {&player, &collider};

    // Initialize nextPos to starting player position
    Vector3 nextPos = player.position;
    SetTargetFPS(60);
    DisableCursor();
    float GameOverTimer = 0.0f;
    float textTimer = 0.0f;
    // GAME LOOP

        while (!WindowShouldClose()) {
            if (GameOverTimer < 3.0f){
                if (IsCursorOnScreen()) DisableCursor();
                Vector3 move = {0};
                Vector3 forward = GetCameraForwardXZ(camera);
                Vector3 right   = GetCameraRightXZ(camera);

                if (IsKeyDown(KEY_W)) move = Vector3Add(move, forward);
                if (IsKeyDown(KEY_S)) move = Vector3Subtract(move, forward);
                if (IsKeyDown(KEY_D)) move = Vector3Add(move, right);
                if (IsKeyDown(KEY_A)) move = Vector3Subtract(move, right);
                if (IsKeyPressed(KEY_SPACE)) player.speed.y = 2.0f;

                move = Vector3Normalize(move);

                player.speed.x = move.x * 5.0f;
                player.speed.z = move.z * 5.0f;
                MovementSystem::HandleMovement(player, collider);
                //player.position += player.speed * GetFrameTime();


                // Change player color depending on state
                if (player.isResting) player.color = GREEN; else player.color = RED;

                // Make camera follow player
                Vector3 offset = {11.0f, 11.0f, 11.0f};
                camera.position = Vector3Add(player.position, offset);
                camera.target = player.position;

                // Game over condition
                if (player.position.y < 1.0f && !player.isResting) {
                    GameOverTimer += 1.0f * GetFrameTime();
                } else {GameOverTimer = 0.0f;}

                // Raylib functions to setup everything
                BeginDrawing();
                ClearBackground(RAYWHITE);
                BeginMode3D(camera);
                Renderer::Draw(objects);
                DrawGrid(10, 1.0f); // 10x10 grid
                EndMode3D();
                DrawText("Use WASD to move the cube", 10, 10, 20, DARKGRAY);
                DrawText(TextFormat("Player position Y %.2f", player.position.y), 500, 500, 20, BLACK );
                DrawText(TextFormat("Player speed X %.2f", player.speed.x), 500, 700, 20, BLACK );
                DrawText(TextFormat("Collider position X %.2f", collider.position.x), 500, 600, 20, BLACK);
                if (player.position.x < 6.0f &&
                    player.position.x > 4.0f &&
                    player.position.z < 6.0f &&
                    player.position.z > 4.0f &&
                    player.position.y > 0.9f &&
                    player.position.y < 1.5f) {

                    if (IsKeyPressed(KEY_E)) { textTimer = 3.0f;}
                    }
                if (textTimer > 0.0f) {
                    DrawText("Hello",
                        GetWorldToScreen({5.0f, 1.0f, 5.0f},camera).x,
                        GetWorldToScreen({5.0f, 1.0f, 5.0f},camera).y - 50,                            15,
                        RED);
                    textTimer -= 1.0f * GetFrameTime();
                    }
                if (textTimer < 0.0f) textTimer = 0.0f;
                DrawFPS(600, 10);
                EndDrawing();
            } else {
                if (IsCursorHidden())EnableCursor();
                BeginDrawing();
                ClearBackground(BLACK);
                DrawText("GAME OVER",
                    GetScreenWidth()/2 - MeasureText("GAME OVER", 50)/2,
                    GetScreenHeight()/2 - 200, 50,
                    RED);
                DrawRectangle(GetScreenWidth()/2 - 75, GetScreenHeight()/2 - 75/2, 150, 75, GRAY);

                Color textColor;

                if (GetMousePosition().x > GetScreenWidth()/2 -75 &&
                    GetMousePosition().x < GetScreenWidth()/2 + 75 &&
                    GetMousePosition().y > GetScreenHeight()/2 -75/2 &&
                    GetMousePosition().y < GetScreenHeight()/2 + 75/2) {
                    textColor = RED;
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        player.position = {
                            0.0f,
                            collider.position.y + groundDimensions.y * 0.5f + player.dimensions.y * 0.5f,
                            0.f};
                        GameOverTimer = 0.0f;
                    }

                } else {textColor = GREEN;}

                DrawText("Restart",
                    GetScreenWidth()/2 - MeasureText("Restart", 20)/2,
                    GetScreenHeight()/2,
                    20,
                    textColor );
                EndDrawing();
            }
        }

            CloseWindow();
            return 0;
        }

