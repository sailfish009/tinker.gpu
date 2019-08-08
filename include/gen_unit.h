#ifndef TINKER_GEN_UNIT_H_
#define TINKER_GEN_UNIT_H_

#include "macro.h"
#include <cassert>
#include <memory>
#include <vector>

TINKER_NAMESPACE_BEGIN
enum class GenericUnitVersion { V0, V1 };

template <GenericUnitVersion VERS>
struct GenericUnitAlloc;

template <>
struct GenericUnitAlloc<GenericUnitVersion::V0> {
  struct Dealloc {
    void operator()(void*) {}
  };

  struct Alloc {
    void operator()(void**, size_t) {}
  };

  struct Copyin {
    void operator()(void*, const void*, size_t) {}
  };
};

/**
 * @brief
 * resource handle
 *
 * analogous to to fortran i/o unit that can be used as signed integers
 *
 * @tparam VERSION
 * mainly used for identifying whether to allocate memory on device and store
 * the device pointer that corresponds to the host object; can be extended to
 * specify different de/allocation methods
 */
template <class T, GenericUnitVersion VERSION = GenericUnitVersion::V0>
class GenericUnit {
private:
  int m_unit;

  static constexpr int USE_DPTR = (VERSION == GenericUnitVersion::V0 ? 0 : 1);

  typedef std::vector<std::unique_ptr<T>> hostptr_vec;
  static hostptr_vec& hostptrs() {
    static hostptr_vec o;
    return o;
  }

  const T& obj() const {
    assert(0 <= m_unit && m_unit < hostptrs().size() &&
           "const T& GenericUnit::obj() const");
    return *hostptrs()[m_unit];
  }

  T& obj() {
    assert(0 <= m_unit && m_unit < hostptrs().size() &&
           "T& GenericUnit::obj()");
    return *hostptrs()[m_unit];
  }

  typedef typename GenericUnitAlloc<VERSION>::Dealloc Dealloc;
  typedef std::vector<std::unique_ptr<T, Dealloc>> dptr_vec;
  static dptr_vec& deviceptrs() {
    assert(USE_DPTR);
    static dptr_vec o;
    return o;
  }

  typedef typename GenericUnitAlloc<VERSION>::Alloc Alloc;
  typedef typename GenericUnitAlloc<VERSION>::Copyin Copyin;

public:
  /// @brief
  /// get the number of open units
  static int size() {
    if_constexpr(USE_DPTR) assert(hostptrs().size() == deviceptrs().size());
    return hostptrs().size();
  }

  /// @brief
  /// close all of the units and reset @c size() to 0
  static void clear() {
    // call ~T() here
    hostptrs().clear();
    // call Dealloc(T*) here
    if_constexpr(USE_DPTR) deviceptrs().clear();
  }

  /// @brief
  /// resize the capacity for the objects on host;
  /// cannot be called if device pointers are used
  static void resize(int s) {
    assert(!USE_DPTR);
    for (int i = size(); i < s; ++i)
      hostptrs().emplace_back(new T);
  }

  /// @brief
  /// similar to inquiring a new fortran i/o unit
  ///
  /// @return
  /// the new unit
  static GenericUnit alloc_new() {
    hostptrs().emplace_back(new T);
    if_constexpr(USE_DPTR) {
      T* ptr;
      Alloc alloc;
      alloc(reinterpret_cast<void**>(&ptr), sizeof(T));
      Dealloc dealloc;
      deviceptrs().emplace_back(ptr, dealloc);
    }
    return size() - 1;
  }

public:
  GenericUnit()
      : m_unit(-1) {}

  GenericUnit(int u)
      : m_unit(u) {}

  operator int() const { return m_unit; }

  /// @brief
  /// get the (const) reference to the object on host
  /// @{
  const T& operator*() const { return obj(); }
  T& operator*() { return obj(); }
  /// @}

  /// @brief
  /// get the (const) pointer to the object on host
  /// @{
  const T* operator->() const { return &obj(); }
  T* operator->() { return &obj(); }
  /// @}

  /// @brief
  /// get device pointer to the object
  /// @{
  const T* deviceptr() const {
    assert(0 <= m_unit && m_unit < deviceptrs().size() &&
           "const T* GenericUnit::deviceptr() const");
    return deviceptrs()[m_unit].get();
  }
  T* deviceptr() {
    assert(0 <= m_unit && m_unit < deviceptrs().size() &&
           "T* GenericUnit::deviceptr()");
    return deviceptrs()[m_unit].get();
  }
  /// @}

  /// @brief
  /// initialized the object on device by an object on host
  ///
  /// @param hobj
  /// the reference to the same object on host
  /// that can be accessed by the same unit number
  void init_deviceptr(const T& hobj) {
    assert(&hobj == &this->obj());
    Copyin copyin;
    copyin(this->deviceptr(), &this->obj(), sizeof(T));
  }
};
TINKER_NAMESPACE_END

#endif
