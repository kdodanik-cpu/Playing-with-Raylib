//
// Created by Krish on 2/15/2026.
//

#ifndef MAIN_CPP_MOVEMENTSYSTEM_H
#define MAIN_CPP_MOVEMENTSYSTEM_H
#include "basics.h"
#include <vector>

struct Collision
{
  Object* A;
  Object* B;
  Vector3 previousA = A->position - A->speed;
  Vector3 previousB = B->position - B->speed;
  Vector3 previousMinA = (A->position - A->speed) - A->dimensions* 0.5f;
  Vector3 previousMaxA = (A->position -A->speed) + A->dimensions*0.5f;
  Vector3 previousMinB = (B->position - B->speed) - B->dimensions* 0.5f;
  Vector3 previousMaxB = (B->position -B->speed) + B->dimensions*0.5f;
};


class MovementSystem
{
private:
  static bool CheckCollision(const Object &A, const Object &B)
  {
    Vector3 halfSizeA = A.dimensions * 0.5f;
    Vector3 halfSizeB = B.dimensions * 0.5f;

    Vector3 halfSizeSum = halfSizeA + halfSizeB;
    Vector3 centerDistance = A.position - B.position;
    centerDistance = (centerDistance);
    centerDistance = {fabsf(centerDistance.x),fabsf(centerDistance.y),fabsf(centerDistance.z)};
    Vector3 overlap = Vector3Subtract(halfSizeSum, centerDistance);
    return(overlap.x > 0.0f && overlap.y > 0.0f && overlap.z > 0.0f);
  };

  public:
  static void MoveObjects(std::vector<Object*> objects);
  static std::vector<Collision> DetectCollision(std::vector<Object*> objects);
  static void ResolveCollision(std::vector<Collision>& objects);
};

#endif // MAIN_CPP_MOVEMENTSYSTEM_H
