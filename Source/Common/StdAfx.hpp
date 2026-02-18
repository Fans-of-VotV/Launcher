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

#include <type_traits>

template <typename T, typename... Types>
constexpr bool is_any_of_v = (std::is_same_v<T, Types> || ...);

template <typename T, typename... Types>
struct is_any_of : std::bool_constant<is_any_of_v<T, Types...>> {};

#include "Common/Logging/Logging.hpp"
#include "Common/Logging/Win32.hpp"
