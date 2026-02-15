#include <iostream>
#include <ranges>
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "renderer.h"
#include "MovementSystem.h"

#define MAX_FALL_SPEED 10.0f
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
{
  // Debug settings
  std::cout.precision(7);
  std::cout.setf(std::ios::fixed);
  const int screenWidth = 1500;
  const int screenHeight = 1000;
  InitWindow(screenWidth, screenHeight, "Isometric Camera Demo");

  const float GRAVITY = 2.0f;

  // Create assets
  // Isometric / orthographic camera

  // Define the camera to look into our 3d world (position, target, up vector)
  Camera camera = { 0 };
  camera.position = { 0.0f, 2.0f, 4.0f };// Camera position
  Vector3 offsetTarget = {0.0f, 0.0f, -4.0f};
  camera.target = Vector3Subtract(camera.position, offsetTarget);      // Camera looking at point
  camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
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

  std::vector<Object *> objects = {&player, &ground, &otherCollider,  &collider};

  // Initialize nextPos to starting player position
  Vector3 nextPos = player.position;
  SetTargetFPS(60);
  float GameOverTimer = 0.0f;
  float textTimer = 0.0f;
  Color debugColor = BLACK;
  // GAME LOOP

  while (!WindowShouldClose())
  {
    if (GameOverTimer < 3.0f)
    {
      if (IsCursorOnScreen()) HideCursor();
      Vector3 move = {0};
      Vector3 forward = GetCameraForwardXZ(camera);
      Vector3 right = GetCameraRightXZ(camera);


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

      player.speed.x = move.x * 5.0f;
      player.speed.z = move.z * 5.0f;

      if (player.position.y > 0.5f) player.speed.y = std::max(player.speed.y - 0.4f, -MAX_FALL_SPEED);

      MovementSystem::MoveObjects(objects);
      std::vector<Collision> collisions = MovementSystem::DetectCollision(objects);
      MovementSystem::ResolveCollision(collisions);
      collisions = MovementSystem::DetectCollision(objects);
      MovementSystem::ResolveCollision(collisions);
      collisions = MovementSystem::DetectCollision(objects);
      MovementSystem::ResolveCollision(collisions);
      collider.speed.x = -0.4f;

      if (player.isResting)
        player.color = GREEN;
      else
        player.color = RED;

      Vector3 offset = {0.0f, player.dimensions.y * 0.8f, -0.5f};
      camera.position = player.position + offset;

      Vector2 mousePos = GetMouseDelta();
      float sensitivity = 0.003f;
      yaw -= mousePos.x * sensitivity;
      pitch -= mousePos.y * sensitivity;
      pitch = Clamp(pitch, -1.5f, 1.5f);

      Vector3 forwardVector =
        {cosf(pitch) * sinf(yaw),
        sinf(pitch),
        cosf(pitch) * cosf(yaw)};

      camera.target = camera.position + forwardVector;

      std::cout << "Mouse pos x " << GetMousePosition().x << std::endl;

      // Game over condition
      if (player.position.y < 1.0f && !player.isResting)
      {
        GameOverTimer += 1.0f * GetFrameTime();
      }
      else
      {
        GameOverTimer = 0.0f;
      }

      // Raylib functions to setup everything
      BeginDrawing();
      ClearBackground(RAYWHITE);
      BeginMode3D(camera);
      Renderer::Draw(objects);
      DrawGrid(10, 1.0f); // 10x10 grid
      EndMode3D();
      if (player.position.y - 0.5f*player.dimensions.y < ground.position.y + 0.5f*ground.dimensions.y)
      {
        debugColor = YELLOW;
      }else debugColor = BLACK;

      DrawText(TextFormat("Camera position x: %.2f", camera.position.x), 10, 10, 20, DARKGRAY);
      DrawText(TextFormat("Camera position y: %.2f", camera.position.y), 10, 50, 20, DARKGRAY);
      DrawText(TextFormat("Camera position z: %.2f", camera.position.z), 10, 90, 20, DARKGRAY);
      DrawText(TextFormat("Camera target x: %.2f", camera.target.x), 10, 110, 20, DARKGRAY);
      DrawText(TextFormat("Camera target y: %.2f", camera.target.y), 10, 140, 20, DARKGRAY);
      DrawText(TextFormat("Camera target z: %.2f", camera.target.z), 10, 190, 20, DARKGRAY);
      DrawText(TextFormat("Player position minY %.7f", player.position.y - 0.5f* player.dimensions.y), 500,
               500, 20, debugColor);
      DrawText(TextFormat("Player speed Y %.7f", player.speed.y), 500, 700, 20,
               BLACK);
      DrawText(TextFormat("Collider position maxY %.7f", ground.position.y + 0.5f * ground.dimensions.y), 500,
               600, 20, BLACK);
      if (player.position.x < 6.0f && player.position.x > 4.0f &&
          player.position.z < 6.0f && player.position.z > 4.0f &&
          player.position.y > 0.9f && player.position.y < 1.5f)
      {

        if (IsKeyPressed(KEY_E))
        {
          textTimer = 3.0f;
        }
      }
      if (textTimer > 0.0f)
      {
        DrawText("Hello", GetWorldToScreen({5.0f, 1.0f, 5.0f}, camera).x,
                 GetWorldToScreen({5.0f, 1.0f, 5.0f}, camera).y - 50, 15, RED);
        textTimer -= 1.0f * GetFrameTime();
      }
      if (textTimer < 0.0f)
        textTimer = 0.0f;
      DrawFPS(600, 10);
      EndDrawing();
    }
    else
    {
      if (IsCursorHidden())
        EnableCursor();
      BeginDrawing();
      ClearBackground(BLACK);
      DrawText("GAME OVER",
               GetScreenWidth() / 2 - MeasureText("GAME OVER", 50) / 2,
               GetScreenHeight() / 2 - 200, 50, RED);
      DrawRectangle(GetScreenWidth() / 2 - 75, GetScreenHeight() / 2 - 75 / 2,
                    150, 75, GRAY);

      Color textColor;

      if (GetMousePosition().x > GetScreenWidth() / 2 - 75 &&
          GetMousePosition().x < GetScreenWidth() / 2 + 75 &&
          GetMousePosition().y > GetScreenHeight() / 2 - 75 / 2 &&
          GetMousePosition().y < GetScreenHeight() / 2 + 75 / 2)
      {
        textColor = RED;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
          player.position = cubePos;
          GameOverTimer = 0.0f;
        }
      }
      else
      {
        textColor = GREEN;
      }

      DrawText("Restart", GetScreenWidth() / 2 - MeasureText("Restart", 20) / 2,
               GetScreenHeight() / 2, 20, textColor);
      EndDrawing();
    }
  }
  CloseWindow();
  return 0;
}
