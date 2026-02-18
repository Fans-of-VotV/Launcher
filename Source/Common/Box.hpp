#pragma once

#include <memory>
#include <stdint.h>
#include <type_traits>

// Manually control object initialization
template <typename T>
class Box {
public:
  constexpr Box() noexcept = default;
  constexpr ~Box() noexcept = default;

  // TODO figure this out?
  IMMOVABLE_CLASS(Box);

public:
  template <typename... Args>
  forceinline void Initialize(
    Args&&... args
  ) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
    std::construct_at(reinterpret_cast<T*>(m_storage), std::forward<Args>(args)...);
  }

  forceinline void Destroy() noexcept(noexcept(std::declval<T&>().~T())) {
    std::destroy_at(reinterpret_cast<T*>(m_storage));
  }

  T* Get() noexcept {
    return std::launder(reinterpret_cast<T*>(m_storage));
  }

  T const* Get() const noexcept {
    return std::launder(reinterpret_cast<T*>(m_storage));
  }

  auto operator->() noexcept {
    return Get();
  }
  auto operator->() const noexcept {
    return Get();
  }
  auto operator*() noexcept {
    return *Get();
  }
  auto operator*() const noexcept {
    return *Get();
  }
  auto operator&() noexcept {
    return Get();
  }
  auto operator&() const noexcept {
    return Get();
  }

private:
  alignas(T) uint8_t m_storage[sizeof(T)] = { 0 };
};
