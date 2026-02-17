#pragma once

#include <Windows.h>

typedef unsigned int uint;

#define nullhandle  nullptr
#define implicit    explicit(false)
#define forceinline __forceinline

#define IMMOVABLE_CLASS(ClassName)                                                                 \
  ClassName(const ClassName&) = delete;                                                            \
  ClassName& operator=(const ClassName&) = delete;                                                 \
  ClassName(ClassName&&) = delete;                                                                 \
  ClassName& operator=(ClassName&&) = delete;

#define STATIC_CLASS(ClassName)                                                                    \
  IMMOVABLE_CLASS(ClassName)                                                                       \
  ClassName() = delete;                                                                            \
  ~ClassName() = delete
