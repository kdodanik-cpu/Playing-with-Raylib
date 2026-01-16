#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <vector>
#include <random>
#include <ranges>
#include <ctime>

constexpr float GRAVITY = 0.1f;
float acceleration = 10.f;
Vector3 speed = {0.0f, 0.0f, 0.0f};

struct Player {
    // Constructor
    Player(const Vector3& pos, const Vector3& dims)
        : position(pos), dimensions(dims){}
    // Members
    Vector3 position;
    Vector3 dimensions;
    Color color = RED;
    bool isResting = false;

};

struct Collider {
    Vector3 position;
    Vector3 dimensions;
    Color color;
    // According to ChatGPT using const references is more efficient (though it doesn't matter that much right now)
    Collider(const Vector3& pos, const Vector3& dims)
    : position(pos), dimensions(dims), color(SKYBLUE) {}

};

// Game world struct
struct World {
    // Constructor
    World(std::vector<Collider>& colliders, Player& player)
        : colliders(colliders), player(player) {}
    // Members
    std::vector<Collider>& colliders;
    Player& player;

    // Functions to resolve collision

    // Repeated same logic for Z axis
    void resolveX(Vector3& nextPos, Vector3& speed)
    {
        // Current max and min bounds for player
        Vector3 maxPlayerPos = player.position + player.dimensions * 0.5f;
        Vector3 minPlayerPos = player.position - player.dimensions * 0.5f;
        // Obtain the next max and min bounds of the player
        Vector3 nextMaxPlayerPos = nextPos + player.dimensions * 0.5f;
        Vector3 nextMinPlayerPos = nextPos - player.dimensions * 0.5f;
        for (const Collider& collider : colliders) {
            Vector3 maxColliderPos = collider.position + collider.dimensions * 0.5f;
            Vector3 minColliderPos = collider.position - collider.dimensions * 0.5f;
            // Check Z and Y gating for next position
            if (
                // Y overlap
                nextMaxPlayerPos.y > minColliderPos.y &&
                nextMinPlayerPos.y < maxColliderPos.y &&
                // Z overlap
                nextMaxPlayerPos.z > minColliderPos.z &&
                nextMinPlayerPos.z < maxColliderPos.z
            ) {
                if (
                    maxPlayerPos.x <= minColliderPos.x && // Player still left of the collider in current pos?
                    nextMaxPlayerPos.x > minColliderPos.x // Will next predicted position penetrate? (Approach vulnerable to tunneling)
                ) {
                    nextPos.x = minColliderPos.x - player.dimensions.x * 0.5f; // If true, clamp nextPos to appropriate bounds
                    speed.x = 0.0f;
                    std::cout << "left side collision occured\n";
                } else if (
                    minPlayerPos.x >= maxColliderPos.x && // Player still to the right of the collider?
                    nextMinPlayerPos.x < maxColliderPos.x // Will next prediction position penetrate
                ) {
                    nextPos.x = maxColliderPos.x + player.dimensions.x * 0.5f;
                    speed.x = 0.0f;
                    std::cout << "right side collision occured\n";
                }

            }
        }
        // Resolve X
        player.position.x = nextPos.x;
    }

    void resolveZ(Vector3& nextPos, Vector3& speed) {
        Vector3 maxPlayerPos = player.position + player.dimensions * 0.5f;
        Vector3 minPlayerPos = player.position - player.dimensions * 0.5f;
        Vector3 nextMaxPlayerPos = nextPos + player.dimensions * 0.5f;
        Vector3 nextMinPlayerPos = nextPos - player.dimensions * 0.5f;

        for (const Collider& collider : colliders) {
            Vector3 maxColliderPos = collider.position + collider.dimensions * 0.5f;
            Vector3 minColliderPos = collider.position - collider.dimensions * 0.5f;

            // Check X and Y overlaps for next position
            if (
                nextMaxPlayerPos.y > minColliderPos.y &&
                nextMinPlayerPos.y < maxColliderPos.y &&
                nextMaxPlayerPos.x > minColliderPos.x &&
                nextMinPlayerPos.x < maxColliderPos.x
                ) {

                // Check collision for Z axis
                if (
                    speed.z < 0.0f &&
                    minPlayerPos.z >= maxColliderPos.z &&
                    nextMinPlayerPos.z < maxColliderPos.z
                ) {
                    speed.z = 0.0f;
                    nextPos.z = maxColliderPos.z + player.dimensions.z * 0.5f;
                    std::cout << "Rear side collision occured\n";
                } else if (
                    speed.z > 0 &&
                    maxPlayerPos.z <= minColliderPos.z &&
                    nextMaxPlayerPos.z > minColliderPos.z
                ) {
                    speed.z = 0.0f;
                    nextPos.z = minColliderPos.z - player.dimensions.z * 0.5f;
                    std::cout << "Front collision occured\n";
                }
            }
        }
        player.position.z = nextPos.z;
    }

    void resolveY(Vector3& nextPos, Vector3& speed) {
        Vector3 maxPlayerPos = player.position + player.dimensions * 0.5f;
        Vector3 minPlayerPos = player.position - player.dimensions * 0.5f;
        Vector3 nextMaxPlayerPos = nextPos + player.dimensions * 0.5f;
        Vector3 nextMinPlayerPos = nextPos - player.dimensions * 0.5f;

        for (const Collider& collider : colliders) {
            Vector3 maxColliderPos = collider.position + collider.dimensions * 0.5f;
            Vector3 minColliderPos = collider.position - collider.dimensions * 0.5f;

            if (
                // Will next pos trigger overlap with any iterated collider?
                maxPlayerPos.x > minColliderPos.x &&
                minPlayerPos.x < maxColliderPos.x &&
                maxPlayerPos.z > minColliderPos.z &&
                minPlayerPos.z < maxColliderPos.z &&
                nextMaxPlayerPos.y > minColliderPos.y &&
                nextMinPlayerPos.y < maxColliderPos.y
                ) { //If so...
                std::cout << "Current minPlayerY: " << minPlayerPos.y << "\n";
                std::cout << "nextMinPlayerY: " << nextMinPlayerPos.y << "\n";
                std::cout << "maxColliderY: " << maxColliderPos.y << "\n";
                if (// Will the player collide from the bottom of the collider?
                    //speed.y > 0.0f &&
                    maxPlayerPos.y <= minColliderPos.y &&
                    nextMaxPlayerPos.y > minColliderPos.y
                    ) {
                    speed.y = 0.0f;
                    nextPos.y = minColliderPos.y - player.dimensions.y * 0.5f;
                } else if (// Or from the top?
                    //speed.y < 0.0f &&
                    const float EPS = 0.001f; // Need to use this, otherwise imprecision will cause this not to trigger when it shouldn't
                    minPlayerPos.y >= maxColliderPos.y -EPS &&
                    nextMinPlayerPos.y < maxColliderPos.y
                    ) {
                    speed.y = 0;
                    nextPos.y = maxColliderPos.y + player.dimensions.y * 0.5f;
                    player.isResting = true;
                    //std::cout << collider.position.x << ", " << collider.position.y << ", " << collider.position.z << "\n";
                }
            }
        }
        player.position.y = nextPos.y;
    }
};

struct Renderer {
    const World& world;
    Renderer(const World& world): world(world) {};

    void Draw() const {

        for (Collider collider : world.colliders) {
            DrawCube(collider.position,
                collider.dimensions.x,
                collider.dimensions.y,
                collider.dimensions.z,
                collider.color);
            DrawCubeWires(collider.position,
                collider.dimensions.x,
                collider.dimensions.y,
                collider.dimensions.z,
                BLACK);
        }

        DrawCube(world.player.position,
            world.player.dimensions.x,
            world.player.dimensions.y,
            world.player.dimensions.z,
            world.player.color);
        DrawCubeWires(world.player.position,
            world.player.dimensions.x,
            world.player.dimensions.y,
            world.player.dimensions.z,
            BLACK);

    };
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
    // Cube
    Vector3 cubePos = {0.0f, 1.0f, 0.0f};
    Vector3 cubeDim = {0.5f, 1.0f, 0.5f};
    Player player(cubePos, cubeDim);
    // Generate colliders for the game
    // Ground level
    Vector3 groundDimensions = {30.0f, 0.05f, 30.0f};
    Vector3 groundPos = {0.0f, 0.475f, 0.0f};
    Collider groundCollider = {groundPos, groundDimensions};
    std::vector<Collider> colliders = {groundCollider};
    int numberOfPlatforms = 15;
    // Randomly generated platforms
    /*auto frand = [](float max) {
        return (float(rand()) / float(RAND_MAX)) * max;
    };*/
    auto frandSigned = [](float range) {
        return ((float(rand()) / float(RAND_MAX)) * 2.0f - 1.0f) * range;
    };

    srand((unsigned int)time(nullptr));
    for (int i = 0; i < numberOfPlatforms; i++) {

        Vector3 pos ={
            frandSigned(groundCollider.position.x + groundDimensions.x * 0.5f),
            0.5f,
            frandSigned(groundCollider.position.z + groundDimensions.z * 0.5f),
        };
        Vector3 dim = {
            1.0f,
            10.0f,
            1.0f,
        };

        Collider collider = {pos, dim};
        collider.color = RED;
        colliders.push_back(collider);
    }

    // Initialize nextPos to starting player position
    Vector3 nextPos = player.position;
    World world(colliders, player);
    Renderer renderer(world);
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

                move = Vector3Normalize(move);

                speed.x = move.x * 10.0f;
                speed.z = move.z * 10.0f;

                player.isResting = false; // reset at the start of each, resolveY will determine whether jump allowed or not
                /*// Simple movement controls
                if (IsKeyDown(KEY_W)) speed.z = -10.0f;
                else if (IsKeyDown(KEY_S)) speed.z = 10.0f;
                else speed.z = 0.0f;
                if (IsKeyDown(KEY_A)) speed.x = -10.0f;
                else if (IsKeyDown(KEY_D)) speed.x = 10.0f;
                else speed.x = 0.0f;*/

                // Handle lateral movement and collision
                nextPos.x = player.position.x + speed.x * GetFrameTime();
                world.resolveX(nextPos, speed);
                nextPos.z = player.position.z + speed.z * GetFrameTime();
                world.resolveZ(nextPos, speed);
                // Handle vertical movement and collision
                // Apply gravity
                speed.y -= 10.5f * GetFrameTime();

                nextPos.y = player.position.y + speed.y * GetFrameTime();
                world.resolveY(nextPos, speed);
                // Now that Y has determined if player is resting, add jump logic
                if (IsKeyPressed(KEY_SPACE) && player.isResting) speed.y = 7.0f;

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
                renderer.Draw();
                DrawCube({5.0f, 1.0f, 5.0f}, 0.5f, 0.5f, 0.5f, BLACK);
                DrawGrid(10, 1.0f); // 10x10 grid
                EndMode3D();
                DrawText("Use WASD to move the cube", 10, 10, 20, DARKGRAY);
                if (player.position.x < 6.0f &&
                    player.position.x > 4.0f &&
                    player.position.z < 6.0f &&
                    player.position.z > 4.0f &&
                    player.position.y > 0.9f &&
                    player.position.y < 1.5f) {
                    DrawText("Press E to speak", 500, 500, 20, YELLOW );
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
                std::cout << GameOverTimer << std::endl;
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
                            groundCollider.position.y + groundDimensions.y * 0.5f + player.dimensions.y * 0.5f,
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

