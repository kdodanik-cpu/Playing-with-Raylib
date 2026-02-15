//
// Created by Krish on 2/15/2026.
//

#ifndef MAIN_CPP_BASICS_H
#define MAIN_CPP_BASICS_H
#include <raylib.h>
#include <raymath.h>
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
  // Constructor
  Object(Vector3 pos, Vector3 dims, Color col, Vector3 speed = {0, 0, 0},
         bool resting = false, bool collides = false)
      : position(pos), dimensions(dims), invMass(0), speed(speed), color(col)
  {
    isResting = resting;
    doesCollide = collides;
  }
};

#endif // MAIN_CPP_BASICS_H
