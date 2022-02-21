#pragma once

#include <ostream>

namespace ogsp::math {

template <class T>
struct Vector2 final {
  T x = 0;
  T y = 0;

  Vector2<T> operator-() const {
    return {.x = -x, .y = -y};
  }
  Vector2<T> operator+(const Vector2<T> rhs) const {
    return {.x = x + rhs.x, .y = y + rhs.y};
  }
  Vector2<T> operator-(const Vector2<T> rhs) const {
    return {.x = x - rhs.x, .y = y - rhs.y};
  }
  Vector2<T> operator*(const T value) const {
    return {.x = x * value, .y = y * value};
  }
};

template <class T>
std::ostream& operator<<(std::ostream& ostream, const Vector2<T> vector) {
  return ostream << "{" << vector.x << ";" << vector.y << "}";
}

}  // namespace ogsp
