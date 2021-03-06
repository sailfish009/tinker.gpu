#pragma once
#include "macro.h"
#include <cstddef>
#include <type_traits>


namespace tinker {
/**
 * \ingroup atomic
 * \brief Add `value` to `buffer[offset]`.
 * \note `value` and `buffer` elements are of the same type.
 */
#pragma acc routine seq
template <class T>
void atomic_add(T value, T* buffer, size_t offset = 0)
{
   #pragma acc atomic update
   buffer[offset] += value;
}


/**
 * \ingroup atomic
 * \brief Add `value` to `buffer[offset]` via fixed-point arithmetic.
 * \tparam T Must be a floating point type.
 */
#pragma acc routine seq
template <class T>
void atomic_add(T value, fixed* buffer, size_t offset = 0)
{
   static_assert(
      std::is_same<T, float>::value || std::is_same<T, double>::value, "");
   // float -> (signed) long long -> fixed
   #pragma acc atomic update
   buffer[offset] +=
      static_cast<fixed>(static_cast<long long>(value * 0x100000000ull));
}


#pragma acc routine seq
inline void atomic_add(fixed value, fixed* buffer, size_t offset = 0)
{
   #pragma acc atomic update
   buffer[offset] += value;
}


/**
 * \ingroup atomic
 * \brief Add virial to the virial buffer.
 * \note Virial and virial buffer are of the same type.
 */
#pragma acc routine seq
template <class T>
void atomic_add(T vxx, T vyx, T vzx, T vyy, T vzy, T vzz, T (*buffer)[8],
                size_t offset = 0)
{
   atomic_add(vxx, buffer[offset], 0);
   atomic_add(vyx, buffer[offset], 1);
   atomic_add(vzx, buffer[offset], 2);
   atomic_add(vyy, buffer[offset], 3);
   atomic_add(vzy, buffer[offset], 4);
   atomic_add(vzz, buffer[offset], 5);
}


/**
 * \ingroup atomic
 * \brief Add virial to the virial buffer via fixed-point arithmetic.
 * \tparam T Must be a floating point type.
 */
#pragma acc routine seq
template <class T>
void atomic_add(T vxx, T vyx, T vzx, T vyy, T vzy, T vzz, fixed (*buffer)[8],
                size_t offset = 0)
{
   atomic_add(vxx, buffer[offset], 0);
   atomic_add(vyx, buffer[offset], 1);
   atomic_add(vzx, buffer[offset], 2);
   atomic_add(vyy, buffer[offset], 3);
   atomic_add(vzy, buffer[offset], 4);
   atomic_add(vzz, buffer[offset], 5);
}


#pragma acc routine seq
template <class T>
T to_flt_acc(fixed val)
{
   return static_cast<T>(static_cast<long long>(val)) / 0x100000000ull;
}


#pragma acc routine seq
template <class G, class T>
G acc_to(T val)
{
   if CONSTEXPR (std::is_same<G, fixed>::value)
      return static_cast<G>(static_cast<long long>(val * 0x100000000ull));
   else
      return static_cast<G>(val);
}
}
