//
// Created by Krish on 2/15/2026.
//
#include "basics.h"
#include "renderer.h"

void Renderer::Draw(const std::vector<Object *> &objects)
{
  for (Object *obj : objects)
  {
    DrawCube(obj->position, obj->dimensions.x, obj->dimensions.y,
             obj->dimensions.z, obj->color);
    DrawCubeWires(obj->position, obj->dimensions.x, obj->dimensions.y,
                  obj->dimensions.z, BLACK);
  }
}
