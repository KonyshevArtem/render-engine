#include "library/math/vector.h"

#include <iostream>

int main() {
  ogsp::math::Vector2<float> zero;
  ogsp::math::Vector2<float> one = {.x = 1, .y = 1};

  std::cout << zero << " " << one << " " <<  one + one << std::endl;
  return 0;
}
