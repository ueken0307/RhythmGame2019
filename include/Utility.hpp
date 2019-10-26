#ifndef UTILITY_HPP
#define UTILITY_HPP

#ifdef _DEBUG
#define DEBUG_PRINTF(fmt, ...) printf(fmt, __VA_ARGS__);
#else
#define DEBUG_PRINTF(fmt, ...)
#endif // _DEBUG

#include <Siv3D.hpp>


constexpr double constexprAbs(double v) {
  return -v;
}

constexpr int constexprAbs(int v) {
  return -v;
}


// value (start ~ end)
constexpr double v2per(double start, double end, double value) {
  return constexprAbs(value - start) / constexprAbs(end - start);
}

//0 < percent < 1
constexpr double per2v(double start, double end, double percent) {
  return start + (end - start) * percent;
}

// srcValue (srcStart ~ srcEnd)
constexpr double convertRange(double srcStart, double srcEnd, double srcValue, double dstStart, double dstEnd) {
  return per2v(dstStart, dstEnd, v2per(srcStart, srcEnd, srcValue));
}

#endif // !UTILITY_HPP
