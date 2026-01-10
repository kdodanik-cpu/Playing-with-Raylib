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
    // Members
    Vector3 position;
    Vector3 dimensions;
    Color color = RED;
    bool isResting = false;
    // Constructor
    Player(const Vector3& pos, const Vector3& dims)
        : position(pos), dimensions(dims){}
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
        : collidersPtr(&colliders), playerPtr(&player) {}

    // Members
    std::vector<Collider>* collidersPtr;
    Player* playerPtr;


    // Functions to resolve collision
    void resolveX(Vector3& nextPos, Vector3& speed)
    {
        // Current max and min bounds for player
        Vector3 maxPlayerPos = playerPtr->position + playerPtr->dimensions * 0.5f;
        Vector3 minPlayerPos = playerPtr->position - playerPtr->dimensions * 0.5f;
        // Obtain the next max and min bounds of the player
        Vector3 nextMaxPlayerPos = nextPos + playerPtr->dimensions * 0.5f;
         Vector3 nextMinPlayerPos = nextPos - playerPtr->dimensions * 0.5f;
        for (const Collider& collider : *collidersPtr) {
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
                    nextPos.x = minColliderPos.x - playerPtr->dimensions.x * 0.5f; // If true, clamp nextPos to appropriate bounds
                    speed.x = 0.0f;
                    std::cout << "left side collision occured\n";
                    } else if (
                        minPlayerPos.x >= maxColliderPos.x && // Player still to the right of the collider?
                        nextMinPlayerPos.x < maxColliderPos.x // Will next prediction position penetrate
                        ) {
                        nextPos.x = maxColliderPos.x + playerPtr->dimensions.x * 0.5f;
                        speed.x = 0.0f;
                        std::cout << "right side collision occured\n";
                    }

                   }
        }
        // Resolve X
        playerPtr->position.x = nextPos.x;
    }

    // Repeated same logic for Z axis
    void resolveZ(Vector3& nextPos, Vector3& speed) {

        Vector3 maxPlayerPos = playerPtr->position + playerPtr->dimensions * 0.5f;
        Vector3 minPlayerPos = playerPtr->position - playerPtr->dimensions * 0.5f;
        Vector3 nextMaxPlayerPos = nextPos + playerPtr->dimensions * 0.5f;
        Vector3 nextMinPlayerPos = nextPos - playerPtr->dimensions * 0.5f;

        for (const Collider& collider : *collidersPtr) {
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
                    nextPos.z = maxColliderPos.z + playerPtr->dimensions.z * 0.5f;
                    std::cout << "Rear side collision occured\n";
                } else if (
                    speed.z > 0 &&
                    maxPlayerPos.z <= minColliderPos.z &&
                    nextMaxPlayerPos.z > minColliderPos.z
                ) {
                    speed.z = 0.0f;
                    nextPos.z = minColliderPos.z - playerPtr->dimensions.z * 0.5f;
                    std::cout << "Front collision occured\n";
                }
            }
        }
        playerPtr->position.z = nextPos.z;
    }

    void resolveY(Vector3& nextPos, Vector3& speed) {
        Vector3 maxPlayerPos = playerPtr->position + playerPtr->dimensions * 0.5f;
        Vector3 minPlayerPos = playerPtr->position - playerPtr->dimensions * 0.5f;
        Vector3 nextMaxPlayerPos = nextPos + playerPtr->dimensions * 0.5f;
        Vector3 nextMinPlayerPos = nextPos - playerPtr->dimensions * 0.5f;

        for (const Collider& collider : *collidersPtr) {
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
                    nextPos.y = minColliderPos.y - playerPtr->dimensions.y * 0.5f;
                } else if (// Or from the top?
                    //speed.y < 0.0f &&
                    const float EPS = 0.001f; // Need to use this, otherwise imprecision will cause this not to trigger when it shouldn't
                    minPlayerPos.y >= maxColliderPos.y -EPS &&
                    nextMinPlayerPos.y < maxColliderPos.y
                    ) {
                    speed.y = 0;
                    nextPos.y = maxColliderPos.y + playerPtr->dimensions.y * 0.5f;
                    playerPtr->isResting = true;
                    //std::cout << collider.position.x << ", " << collider.position.y << ", " << collider.position.z << "\n";
                }

            }

        }
        playerPtr->position.y = nextPos.y;
    }

};

struct Renderer {
    World world;
    Renderer(const World& world): world(world) {};

    void Draw() const {

        for (Collider collider : *world.collidersPtr) {
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

        DrawCube(world.playerPtr->position,
            world.playerPtr->dimensions.x,
            world.playerPtr->dimensions.y,
            world.playerPtr->dimensions.z,
            world.playerPtr->color);
        DrawCubeWires(world.playerPtr->position,
            world.playerPtr->dimensions.x,
            world.playerPtr->dimensions.y,
            world.playerPtr->dimensions.z,
            BLACK);

    };
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
    Player player(cubePos, cubeDim);


    // Generate colliders for the game
    // Ground level
    Vector3 groundDimensions = {30.0f, 0.05f, 30.0f};
    Vector3 groundPos = {0.0f, 0.475f, 0.0f};
    Collider groundCollider = {groundPos, groundDimensions};
    std::vector<Collider> colliders = {groundCollider};
    int numberOfPlatforms = 1000;
    // Randomly generated platforms
    auto frand = [](float max) {
        return (float(rand()) / float(RAND_MAX)) * max;
    };
    auto frandSigned = [](float range) {
        return ((float(rand()) / float(RAND_MAX)) * 2.0f - 1.0f) * range;
    };

    srand((unsigned int)time(nullptr));
    for (int i = 0; i < numberOfPlatforms; i++) {

        Vector3 pos ={
            frandSigned(40.0f),
            frandSigned(7.0f),
            frandSigned(40.0f),
        };
        Vector3 dim = {
            frand(2.0f),
            0.05f,
            frand(2.0f),
        };

        Collider collider = {pos, dim};
        colliders.push_back(collider);
    }

    // Initialize nextPos to starting player position
    Vector3 nextPos = player.position;
    World world(colliders, player);
    Renderer renderer(world);
    SetTargetFPS(60);
    float timer = 0.0f;
    // GAME LOOP
    while (!WindowShouldClose() && timer < 3.0f) {
        player.isResting = false; // reset at the start of each, resolveY will determine whether jump allowed or not
        // Simple movement controls
        if (IsKeyDown(KEY_W)) speed.z = -10.0f;
        else if (IsKeyDown(KEY_S)) speed.z = 10.0f;
        else speed.z = 0.0f;
        if (IsKeyDown(KEY_A)) speed.x = -10.0f;
        else if (IsKeyDown(KEY_D)) speed.x = 10.0f;
        else speed.x = 0.0f;

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

        // Make the camera follow the player
        Vector3 offset = {10.0f, 10.0f, 10.0f}; // Keep the same distance from the player the camera was initialized with
        camera.position = player.position + offset;
        camera.target = player.position;
        // Game over condition
        if (player.position.y < 1.0f && !player.isResting) {
            timer += 1.0f * GetFrameTime();
        } else {timer = 0.0f;}

        // Raylib functions to setup everything
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);
        renderer.Draw();
        DrawGrid(10, 1.0f); // 10x10 grid
        EndMode3D();
        DrawText("Use WASD to move the cube", 10, 10, 20, DARKGRAY);
        DrawFPS(600, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}