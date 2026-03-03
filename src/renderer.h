//
// Created by Krish on 2/15/2026.
//

#ifndef MAIN_CPP_RENDERER_H
#define MAIN_CPP_RENDERER_H
#include <vector>
#include "basics.h"

// Renderer draws things onto screen whether interpreting 3D or 2D coords
// To interpret 3D coordinates it needs a Camera from Raylib
// Different modes should set behavior for the camera
// 2D and 3D rendering are interpreted different but the whole process
// can be condensed in a single function call in main


enum Mode{DEBUG = 0, FP}; // Modes for the camera will define its behavior

class Renderer
{
  Camera m_camera = {0};
  Mode m_mode;
  float m_zoom = 0.0f;
  float m_yaw = 0.0f;
  float m_pitch = 0.0f;
  void Draw3D(const std::vector<Object*>& objects, const Player& player);
  void Draw2D();
public:
  // 3D draw function
  void InitCamera(Mode mode, Vector3 &playerPos);
  void Render(const std::vector<Object *> &objects, const Player& player);
  void Update(const Player& player);
  Camera& GetCamera() { return m_camera; }
};

#endif // MAIN_CPP_RENDERER_H