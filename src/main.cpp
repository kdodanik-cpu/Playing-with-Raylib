#include <iostream>
#include <ranges>
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "renderer.h"
#include "MovementSystem.h"
#define MAX_FALL_SPEED 10.0f

extern const int screenWidth = 1500;
extern const int screenHeight = 1000;

// Functions to handle movement
Vector3 GetCameraForwardXZ(const Camera &cam)
{
  Vector3 forward = Vector3Subtract(cam.target, cam.position);
  forward.y = 0.0f;
  return Vector3Normalize(forward);
}

Vector3 GetCameraRightXZ(const Camera &cam)
{
  Vector3 forward = GetCameraForwardXZ(cam);
  return Vector3Normalize(Vector3CrossProduct(forward, {0.0f, 1.0f, 0.0f}));
}

const float MIN_HEIGHT = 0.5f;

int main()
{// Debug settings
  std::cout.precision(7);
  std::cout.setf(std::ios::fixed);
  Renderer renderer;
  //Texture reolverTexture = LoadTexture("../assets/revolver_low_lambert1_BaseColor.png");
  InitWindow(screenWidth, screenHeight, "Isometric Camera Demo");
  const float GRAVITY = 2.0f;
  // Create assets
  // Isometric / orthographic camera
  // Define the camera to look into our 3d world (position, target, up vector)
  Camera camera = { 0 };
  camera.position = { 2.0f, 2.0f, 4.0f };// Camera position
  Vector3 offsetTarget = {0.0f, 0.0f, -4.0f};
  camera.target = {0.0f, 0.0f, 0.0f};      // Camera looking at point
  camera.up = { 0.0f, 1.0f,     0.0f };          // Camera up vector (rotation towards target)
  camera.fovy = 60.0f;                                // Camera field-of-view Y
  camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
  float yaw = 0.0f; // yaw is rotation of the horizontal axis
  float pitch = 0.0f; // pitch on the vertical axis
  // Player Cube
  Vector3 cubePos = {0.0f, 10.0f, 0.0f};
  Vector3 cubeDim = {0.5f, 1.0f, 0.5f};
  Object player = {cubePos, cubeDim, RED, {0.0f, 0.0f, 0.0f}};
  player.doesCollide = true;
  player.invMass = 1.0f;

  // Colliders
  Vector3 groundDimensions = {30.0f, 0.700f, 30.0f};
  Vector3 groundPos = {0.0f, 0.5f, 0.0f};
  Object ground(groundPos, groundDimensions, SKYBLUE, {0.0f, 0.0f, 0.0f});
  ground.doesCollide = true;
  Object collider({5.0f, 10.0f, 0.0f}, cubeDim, YELLOW);
  collider.doesCollide = true;
  collider.speed= {-1.0f, 0.0f, 0.0f};
  collider.invMass = 2.0f;

  Object otherCollider = {{-5.0f, 10.0f, 0.0f}, cubeDim, GREEN};
  otherCollider.doesCollide = true;

  Object anotherCollider = {{10.0f, 10.0f, 0.0f}, cubeDim, GRAY};
  anotherCollider.doesCollide = true;
  std::vector<Object *> objects = {&player, &ground, &otherCollider,  &collider, &anotherCollider};
  Model revolver = LoadModel("../assets/revolver.obj");
  Texture2D revolverTexture = LoadTexture("../assets/revolver_low_lambert1_Metallic-revolver_low_lambert1_Roughne.png");
  revolver.materials->maps->texture = revolverTexture;

  Player player1(cubePos, 0.5f, revolver);
  Vector3 currentPlayerSpeed = {0.0f, 0.0f, 0.0f};
  Vector3 startPosition = player1.GetPosition();
  renderer.InitCamera(FP, startPosition);
  // Initialize nextPos to starting player position
  SetTargetFPS(60);
  // GAME LOOP
  while (!WindowShouldClose())
  {
      if (IsCursorOnScreen()) HideCursor();
      Vector3 move = {0};
      Vector3 forward = GetCameraForwardXZ(renderer.GetCamera());
      Vector3 right = GetCameraRightXZ(renderer.GetCamera());

      if (IsKeyDown(KEY_W))
        move = Vector3Add(move, forward);
      if (IsKeyDown(KEY_S))
        move = Vector3Subtract(move, forward);
      if (IsKeyDown(KEY_D))
        move = Vector3Add(move, right);
      if (IsKeyDown(KEY_A))
        move = Vector3Subtract(move, right);
      if (IsKeyPressed(KEY_SPACE))
        player.speed.y = 20.0f;
      move = Vector3Normalize(move);
    player1.SetSpeed(move*5.0f);
    player1.Update();
      // Apply gravity to player
    player.speed.y = std::max(player.speed.y  - 0.4f, -MAX_FALL_SPEED);
    // 1) Move once per frame according to velocity
      MovementSystem::MoveObjects(objects);
    // 2) Detect and resolve collisions
      std::vector<Collision> collisions = MovementSystem::DetectCollision(objects);
      MovementSystem::ResolveCollision(collisions);
    // 3) Additional passes just in case resolution push something where it shouldn't have
      collisions = MovementSystem::DetectCollision(objects);
      MovementSystem::ResolveCollision(collisions);
      collisions = MovementSystem::DetectCollision(objects);
      MovementSystem::ResolveCollision(collisions);
      collider.speed.x = -0.4f;
    renderer.Update(player1);
    renderer.Render(objects, player1);
    }
  CloseWindow();
  return 0;
}
