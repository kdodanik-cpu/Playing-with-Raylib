//
// Created by Krish on 2/15/2026.
//
#include "MovementSystem.h"


// Update all objects according to their velocities
void MovementSystem::MoveObjects(std::vector<Object*> objects)
{
  float deltaTime = GetFrameTime();
  for (Object* obj: objects)
  {
    obj->position += obj->speed * deltaTime;
  }
}

std::vector<Collision> MovementSystem::DetectCollision(std::vector<Object*> objects)
{
  std::vector<Collision> collisions;
  for (size_t i = 0; i + 1 < objects.size(); i++)
  {
    if (!objects[i]->doesCollide) continue;
    for (size_t j = i + 1; j < objects.size(); j++)
    {
      if (i == j) continue;
      if (!objects[j]->doesCollide) continue;
      if (CheckCollision(*objects[i], *objects[j]))
      {
        Collision collision{objects[i], objects[j]};
        collisions.push_back(collision);
      }
    }
  }
  return collisions;
}

void MovementSystem::ResolveCollision(std::vector<Collision>& collisions)
{
  for (Collision& collision : collisions)
  {
    Vector3 halfSizeA = collision.A->dimensions * 0.5f;
    Vector3 halfSizeB = collision.B->dimensions * 0.5f;
    Vector3 halfSizeSum = halfSizeA + halfSizeB;
    Vector3 currentDelta = collision.A->position - collision.B->position;
    Vector3 normal = {1.0f, 1.0f, 1.0f};
    if (currentDelta.x < 0.0f) normal.x *= -1;
    if (currentDelta.y < 0.0f) normal.y *= -1;
    if (currentDelta.z < 0.0f) normal.z *= -1;
    currentDelta = {fabsf(currentDelta.x), fabsf(currentDelta.y), fabsf(currentDelta.z)};
    Vector3 currentOverlap = halfSizeSum - currentDelta;


    if (currentOverlap.x < currentOverlap.y && currentOverlap.x < currentOverlap.z)
    {
      currentOverlap.z = 0.0f;
      currentOverlap.y = 0.0f;
    }

    if (currentOverlap.z < currentOverlap.y && currentOverlap.z < currentOverlap.x)
    {
      currentOverlap.x = 0.0f;
      currentOverlap.y = 0.0f;
    }

    if (currentOverlap.y < currentOverlap.x && currentOverlap.y < currentOverlap.z)
    {
      currentOverlap.z = 0.0f;
      currentOverlap.x = 0.0f;
    }

    float invMassSum = collision.A->invMass + collision.B->invMass;
    Vector3 correctionA = currentOverlap * normal * collision.A->invMass/invMassSum;
    Vector3 correctionB = currentOverlap * normal * collision.B->invMass/invMassSum;
    collision.A->position += correctionA;
    collision.B->position -= correctionB;
  }
}
