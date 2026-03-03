//
// Created by Krish on 2/15/2026.
//
#include "basics.h"
#include "renderer.h"
#include <algorithm>
#include <iostream>
extern const int screenWidth;
extern const int screenHeight;


/*void Renderer::Draw3D(const std::vector<Object*> &objects, const Player& player)
{
  BeginMode3D(m_camera);
  for (Object *obj : objects)
  {
    DrawCube(obj->position, obj->dimensions.x, obj->dimensions.y,
             obj->dimensions.z, obj->color);
    DrawCubeWires(obj->position, obj->dimensions.x, obj->dimensions.y,
                  obj->dimensions.z, BLACK);
  }

  Vector3 forward = Vector3Normalize(m_camera.target - m_camera.position);
  Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, Vector3{0,1,0}));
  Vector3 up = Vector3CrossProduct(right, forward);
  // How far right, upward or forward you want the gun to be in FP view
  Vector3 localOffset = {0.75f, -1.0f, 1.75f};
  // convert to world space
  Vector3 worldOffset =
      Vector3Scale(right, localOffset.x) +
      Vector3Scale(up, localOffset.y) +
      Vector3Scale(forward, localOffset.z);

  Vector3 gunPos = m_camera.position + worldOffset;

  DrawModelEx(player.GetModel(),
    gunPos,
    Vector3{1.0f, 0.0f, 0.0f},
    m_yaw,
    Vector3{0.5f, 0.5f, 0.5f},
    WHITE);

  // Final transform
  // DrawModel(player.GetModel(),
  //   gunPos,
  //   0.5f,
  //   WHITE);

  EndMode3D();
}*/


void Renderer::Draw3D(const std::vector<Object*>& objects, const Player& player)
{
  BeginMode3D(m_camera);
  for (Object* obj : objects)
  {
    DrawCube(obj->position, obj->dimensions.x, obj->dimensions.y, obj->dimensions.z, obj->color);
    DrawCubeWires(obj->position, obj->dimensions.x, obj->dimensions.y, obj->dimensions.z, BLACK);
  }
  EndMode3D();

  // Draw the gun on top of everything else
  Camera3D gunCamera = {};
  gunCamera.position = {0.0f};
  gunCamera.target = {0.0f, 0.0f, 1.f};
  gunCamera.up = {0.0f, 1.0f, 0.0f};
  gunCamera.fovy = 60.0f;
  gunCamera.projection = CAMERA_PERSPECTIVE;


  Vector3 gunPos = {-0.25f, -0.5f, 1.75f};

  BeginMode3D(gunCamera);
  DrawModelEx(player.GetModel(),
        gunPos,
        Vector3{ 0.0f, 1.0f, 0.0f },
        15.0f,
        Vector3{0.3f, 0.3f, 0.3f},
        WHITE);

  EndMode3D();

}





void Renderer::Draw2D()
{

}


void Renderer::Render(const std::vector<Object *> &objects, const Player& player)
{
  BeginDrawing();
  ClearBackground(WHITE);
  Draw3D(objects, player);
  Draw2D();
  EndDrawing();
}

void Renderer::InitCamera(Mode mode, Vector3& playerPos)
{
  m_mode = mode;
  if (mode == FP)
  {
    Vector3 offset = {0.0, 2.0f, -1.0f};
    m_camera.position = playerPos + offset;
    Vector3 forwardVector =
    {cosf(m_pitch) * sinf(m_yaw),
    sinf(m_pitch),
    cosf(m_pitch) * cosf(m_yaw)};
    m_camera.target = m_camera.position + forwardVector;
    m_camera.fovy = 100.0f;
    m_camera.up = {0.0f, 2.0f, 0.0f};
    m_camera.projection = CAMERA_PERSPECTIVE;
  }
  else if (mode == DEBUG)
  {
    Vector3 offset = {2.0f, 2.0f, 2.0f};
    m_camera.position = playerPos + offset;
    m_camera.target = playerPos;
    m_camera.up = {0.0f, 2.0f, 0.0f};
    m_camera.fovy = 45.0f;
    m_camera.projection = CAMERA_ORTHOGRAPHIC;
  }
}

void Renderer::Update(const Player& player)
{

  if (IsKeyPressed(KEY_F1))
  {
    if (m_mode == FP)m_mode = DEBUG;
    else if (m_mode == DEBUG) m_mode = FP;
  }
  // Determine whether the camera is looking toward
  Vector2 mouseDelta = GetMouseDelta();
  m_yaw -= mouseDelta.x * 0.003f;
  m_pitch -= mouseDelta.y * 0.003f;
  m_pitch = std::clamp(m_pitch,-1.5f,1.5f);

  Vector3 forwardVector =
  {cosf(player.GetPitch()) * sinf(player.GetYaw()),
  sinf(player.GetPitch()),
  cosf(player.GetPitch()) * cosf(player.GetYaw())};
  m_camera.target = m_camera.position + forwardVector;

  // Define behavior according to camera mode
  if (m_mode == DEBUG)
  {
    m_camera.projection = CAMERA_PERSPECTIVE;
    float mouseScroll = GetMouseWheelMove();
    m_zoom += mouseScroll * .25f;
    //m_camera.fovy -= mouseScroll * 1.25f;
    m_camera.position += Vector3Scale(forwardVector, m_zoom);
    m_zoom = 0.0f;
  }
  else if (m_mode == FP)
  {
    m_camera.projection = CAMERA_PERSPECTIVE;
    Vector3 offset = {0.0f, 1.0f, 0.0f};
    m_camera.position = player.GetPosition() + offset;
    m_camera.fovy = 100.0f;
  }

  // Keep "crosshair" centered at the end of each frame
  SetMousePosition(screenWidth*0.5f,screenHeight*0.5f);
}
