//
// Created by Krish on 2/15/2026.
//

#ifndef MAIN_CPP_BASICS_H
#define MAIN_CPP_BASICS_H
#include <algorithm>
#include <iostream>
#include <raylib.h>
#include <raymath.h>

inline void PrintMatrixK(const Matrix& m)
{
  TraceLog(LOG_INFO, "%f %f %f %f", m.m0, m.m4, m.m8,  m.m12);
  TraceLog(LOG_INFO, "%f %f %f %f", m.m1, m.m5, m.m9,  m.m13);
  TraceLog(LOG_INFO, "%f %f %f %f", m.m2, m.m6, m.m10, m.m14);
  TraceLog(LOG_INFO, "%f %f %f %f", m.m3, m.m7, m.m11, m.m15);
}


class Player
{
  Vector3 m_position;
  float m_scale;
  BoundingBox m_boundingBox;
  bool m_doesCollide;
  Model m_model;
  Vector3 m_speed;
  float m_yaw = 0.0f;
  float m_pitch = 0.0f;
public:
  Player(Vector3 pos, float scale, Model model)
    : m_position(pos), m_scale(scale), m_doesCollide(false), m_model(model), m_speed(Vector3(0, 0, 0))
  {
    m_boundingBox = GetModelBoundingBox(m_model);
    m_model.transform = MatrixIdentity();
  }
  // Getters
  Vector3 GetPosition() const { return m_position; }
  Vector3 GetSpeed () const {return m_speed; }
  const Model& GetModel() const { return m_model; }
  float GetYaw () const { return m_yaw; }
  float GetPitch () const { return m_pitch; }
  // Setters
  void SetPosition(Vector3 newPosition) {m_position = newPosition;}
  void SetSpeed(Vector3 newSpeed) {m_speed = newSpeed;}
  void Update()
  {
    Vector2 mouseDelta = GetMouseDelta();
    // Scale to sensitivity
    m_yaw -= mouseDelta.x * 0.003f;
    m_pitch += mouseDelta.y * 0.003f;
    m_pitch = std::clamp(m_pitch, -1.2f, 1.2f);

    Matrix rotY = MatrixRotateY(m_yaw);
    Matrix rotX = MatrixRotateX(-m_pitch);
    Matrix rotation = rotY * rotX;
    //m_model.transform = rotation;

    m_position += m_speed * GetFrameTime();
  }

};


struct Object
{
  // Member variables
  Vector3 position;
  Vector3 dimensions;
  float invMass;
  Color color;
  Vector3 speed;
  bool isResting;
  bool doesCollide;
  private:
  inline static size_t count = 0;
public:
  // Constructor
  Object(Vector3 pos, Vector3 dims, Color col, Vector3 speed = {0, 0, 0},
         bool resting = false, bool collides = false)
      : position(pos), dimensions(dims), invMass(0), color(col), speed(speed)
  {
    isResting = resting;
    doesCollide = collides;
    count++;
  }
  ~Object(){count -= 1;}

};


inline void printVector(Vector3 vec)
{
  std::cout <<"x: "<< vec.x << " y: " << vec.y << " z: " << vec.z << std::endl;
}


#endif // MAIN_CPP_BASICS_H
