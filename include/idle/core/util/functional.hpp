
/*
 *   _____    _ _        .      .    .
 *  |_   _|  | | |  .       .           .
 *    | |  __| | | ___         .    .        .
 *    | | / _` | |/ _ \                .
 *   _| || (_| | |  __/ github.com/Naios/idle
 *  |_____\__,_|_|\___| AGPL v3 (Early Access)
 *
 * Copyright(c) 2018 - 2021 Denis Blank <denis.blank at outlook dot com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef IDLE_CORE_UTIL_FUNCTIONAL_HPP_INCLUDED
#define IDLE_CORE_UTIL_FUNCTIONAL_HPP_INCLUDED

#include <type_traits>
#include <utility>

namespace idle {
namespace detail {
template <typename Ret, typename... Args>
using fn_ptr_t = Ret (*)(Args...);
}

template <typename T, T Value>
struct StaticBind;
template <typename Class, typename Ret, typename... Args,
          Ret (Class::*Value)(Args...)>
struct StaticBind<Ret (Class::*)(Args...), Value> {
protected:
  template <typename OtherRet, typename OtherClass, typename... OtherArgs>
  static constexpr OtherRet rebind_to(OtherClass& self, OtherArgs... args) {
    static_assert(!std::is_const<OtherClass>::value,
                  "Can't bind a mutable member function to a const one!");
    return (self.*Value)(std::forward<OtherArgs>(args)...);
  }

public:
  template <typename OtherRet, typename OtherClass, typename... OtherArgs>
  constexpr operator detail::fn_ptr_t<OtherRet, OtherClass&, //
                                      OtherArgs...>() const noexcept {
    return &rebind_to<OtherRet, OtherClass, OtherArgs...>;
  }

  constexpr Ret operator()(Class& self, Args... args) const {
    return (self.*Value)(std::move(args)...);
  }
};
template <typename Class, typename Ret, typename... Args,
          Ret (Class::*Value)(Args...) const>
struct StaticBind<Ret (Class::*)(Args...) const, Value> {
protected:
  template <typename OtherRet, typename OtherClass, typename... OtherArgs>
  static constexpr OtherRet rebind_to(OtherClass& self, OtherArgs... args) {
    return (self.*Value)(std::forward<OtherArgs>(args)...);
  }

public:
  template <typename OtherRet, typename OtherClass, typename... OtherArgs>
  constexpr operator detail::fn_ptr_t<OtherRet, OtherClass&, //
                                      OtherArgs...>() const noexcept {
    return &rebind_to<OtherRet, OtherClass, OtherArgs...>;
  }

  constexpr Ret operator()(Class const& self, Args... args) const {
    return (self.*Value)(std::move(args)...);
  }
};

template <typename Ret, typename... Args, Ret (*Value)(Args...)>
struct StaticBind<Ret (*)(Args...), Value> {
protected:
  template <typename OtherRet, typename... OtherArgs>
  static constexpr OtherRet rebind_to(OtherArgs... args) {
    return (Value)(std::forward<OtherArgs>(args)...);
  }

public:
  template <typename OtherRet, typename... OtherArgs>
  constexpr operator detail::fn_ptr_t<OtherRet, OtherArgs...>() const noexcept {
    return &rebind_to<OtherRet, OtherArgs...>;
  }

  constexpr Ret operator()(Args... args) const {
    return (Value)(std::move(args)...);
  }
};

template <typename T, T Value>
struct StaticReturn {
protected:
  template <typename OtherRet, typename... OtherArgs>
  static constexpr OtherRet rebind_to(OtherArgs...) {
    return Value;
  }

public:
  template <typename... Args>
  T operator()(Args&&...) const {
    return Value;
  }

  template <typename OtherRet, typename... OtherArgs>
  constexpr operator detail::fn_ptr_t<OtherRet, OtherArgs...>() const noexcept {
    return &rebind_to<OtherRet, OtherArgs...>;
  }
};
} // namespace idle

/// Specifies a class which calls an underlying class member function pointer
/// and also is compile-time convertible to a compatible function pointer.
///
/// This idiom can be improved through C++17 auto template parameters.
#define IDLE_STATIC_BIND(EXPR) (::idle::StaticBind<decltype(EXPR), (EXPR)>{})

/// Specifies a callable type which returns the given value and additionally is
/// compile-time convertible to a compatible function pointer.
///
/// This idiom can be improved through C++17 auto template parameters.
///
/// \note The type of the EXPR is limited to integral types until C++20!
#define IDLE_STATIC_RETURN(EXPR)                                               \
  (::idle::StaticReturn<decltype(EXPR), (EXPR)>{})

#endif // IDLE_CORE_UTIL_FUNCTIONAL_HPP_INCLUDED
