//
// Created by Krish on 2/15/2026.
//

#ifndef MAIN_CPP_RENDERER_H
#define MAIN_CPP_RENDERER_H
#include <vector>
#include "basics.h"

class Renderer
{
public:
  static void Draw(const std::vector<Object*>& objects);
};

#endif // MAIN_CPP_RENDERER_H