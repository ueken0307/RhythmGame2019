#ifndef UTILITY_HPP
#define UTILITY_HPP

#ifdef _DEBUG
#define DEBUG_PRINTF(fmt, ...) printf(fmt, __VA_ARGS__);
#else
#define DEBUG_PRINTF(fmt, ...)
#endif // _DEBUG

#endif // !UTILITY_HPP
