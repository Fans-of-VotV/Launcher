#pragma once

#include <stddef.h>

struct InheritReference_Tag {
  constexpr explicit InheritReference_Tag() = default;
};
constexpr inline auto InheritReference = InheritReference_Tag();

struct AddNewReference_Tag {
  constexpr explicit AddNewReference_Tag() = default;
};
constexpr inline auto AddNewReference = AddNewReference_Tag();

// Auto-releasable Win32 Common Object pointer
template <class T>
class CO final {
public:
  constexpr CO() = default;
  constexpr implicit CO(std::nullptr_t) noexcept : m_object(nullptr) {}

  constexpr CO(T* object, InheritReference_Tag) : m_object(object) {}

  inline CO(T* object, AddNewReference_Tag) : m_object(object) {
    if (m_object)
      m_object->AddRef();
  }

  inline CO(CO const& other) : CO(other.m_object, AddNewReference) {}

  constexpr CO(CO&& other) noexcept : m_object(other.m_object) {
    other.m_object = nullptr;
  }

  inline ~CO() {
    Release();
  }

  inline CO& operator=(CO const& other) {
    Release();
    m_object = other.m_object;
    if (m_object)
      m_object->AddRef();
    return *this;
  }

  constexpr CO& operator=(CO&& other) noexcept {
    Release();
    m_object = other.m_object;
    other.m_object = nullptr;
    return *this;
  }

public:
  inline T* operator->() const noexcept {
    return m_object;
  }
  inline T** operator&() noexcept {
    return &m_object;
  }

  inline bool operator==(std::nullptr_t) const noexcept {
    return m_object == nullptr;
  }
  inline bool operator!=(std::nullptr_t) const noexcept {
    return m_object != nullptr;
  }

  inline operator bool() const noexcept {
    return !!m_object;
  }

  inline T* Get() const noexcept {
    return m_object;
  }

  inline T* Release() {
    if (m_object)
      m_object->Release();

    auto result = m_object;
    m_object = nullptr;
    return result;
  }

private:
  T* m_object = nullptr;
};

template <class...>
class _CommonObjectInheritanceImpl;

template <class C, class... Other>
class _CommonObjectInheritanceImpl<C, Other...>
  : public C
  , public _CommonObjectInheritanceImpl<Other...> {};

template <>
class _CommonObjectInheritanceImpl<> {};

template <class... Interfaces>
class CommonObject : public _CommonObjectInheritanceImpl<Interfaces...> {
  IMMOVABLE_CLASS(CommonObject);

protected:
  CommonObject() = default;

public:
  virtual ~CommonObject() = default;

public:
  ULONG STDMETHODCALLTYPE AddRef() override {
    return ++m_refCount;
  }

  ULONG STDMETHODCALLTYPE Release() override {
    ULONG newCount = --m_refCount;

    if (newCount == 0)
      delete this;

    return newCount;
  }

  HRESULT STDMETHODCALLTYPE QueryInterface(IID const& iid, void** outObject) override {
    if (outObject == nullptr)
      return E_POINTER;

    bool casted =
      ((__uuidof(Interfaces) == iid && ((*outObject = implicit_cast<Interfaces*>(this)), true)) ||
       ...);

    return casted ? S_OK : E_NOINTERFACE;
  }

private:
  ULONG m_refCount = 1;
};

#define DECLARE_COMMON_OBJECT_MAKER(CLASS)                                                         \
public:                                                                                            \
  template <typename... ArgsT>                                                                     \
  static CO<CLASS> Create(ArgsT&&... args) {                                                       \
    return CO(new CLASS(std::forward<ArgsT>(args)...), InheritReference);                          \
  }
