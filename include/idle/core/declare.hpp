
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

#ifndef IDLE_CORE_DECLARE_HPP_INCLUDED
#define IDLE_CORE_DECLARE_HPP_INCLUDED

#include <type_traits>
#include <utility>
#include <idle/core/api.hpp>
#include <idle/core/detail/preprocessor.hpp>
#include <idle/core/detail/section.hpp>
#include <idle/core/fwd.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/service.hpp>
#include <idle/core/support.hpp>
#include <idle/core/util/intrusive_list.hpp>

namespace idle {
namespace detail {
using service_creator_ptr_t = Ref<Service> (*const)(Inheritance);
class ServiceDeclaration;
struct DeclarationListTag {};
using DeclarationList = intrusive_forward_list<ServiceDeclaration,
                                               DeclarationListTag>;
class IDLE_API(idle) ServiceDeclaration : public DeclarationList::node {
public:
  explicit ServiceDeclaration(service_creator_ptr_t creator);
  ServiceDeclaration(ServiceDeclaration const&) = delete;
  ServiceDeclaration(ServiceDeclaration&&) = delete;
  ServiceDeclaration& operator=(ServiceDeclaration const&) = delete;
  ServiceDeclaration& operator=(ServiceDeclaration&&) = delete;
  Ref<Service> operator()(Inheritance inh) const {
    return creator_(std::move(inh));
  }

private:
  service_creator_ptr_t creator_;
};
} // namespace detail
} // namespace idle

#if defined(IDLE_EXECUTABLE)
#  define IDLE_DECLARE(CLASS_NAME_OR_PART)                                     \
    static ::idle::detail::ServiceDeclaration const IDLE_DETAIL_CAT(           \
        sd_##CLASS_NAME_OR_PART,                                               \
        __LINE__)(static_cast<::idle::detail::service_creator_ptr_t>(          \
        [](::idle::Inheritance parent) -> ::idle::Ref<::idle::Service> {       \
          return ::idle::spawn<::std::conditional_t<                           \
              ::idle::is_service<CLASS_NAME_OR_PART>::value,                   \
              CLASS_NAME_OR_PART,                                              \
              ::idle::DefaultService<CLASS_NAME_OR_PART>>>(std::move(parent)); \
        }));
#elif defined(IDLE_HAS_DYNAMIC_LINKING)
#  if !defined(IDLE_DETAIL_EXPORT_SECTION_NAME)
#    define IDLE_DETAIL_EXPORT_SECTION_NAME idlesvex
#  endif // IDLE_DETAIL_EXPORT_SECTION_NAME
#  if !defined(IDLE_DETAIL_EXPORT_SECTION_NAME_STR)
#    define IDLE_DETAIL_EXPORT_SECTION_NAME_STR                                \
      IDLE_DETAIL_STRINGIFY(IDLE_DETAIL_EXPORT_SECTION_NAME)
#  endif // IDLE_DETAIL_EXPORT_SECTION_NAME

#  define IDLE_DETAIL_DECLARE_CREATOR(CLASS_NAME_OR_PART)                      \
    ::idle::Ref<::idle::Service> IDLE_DETAIL_CAT(                              \
        idle_create_##CLASS_NAME_OR_PART,                                      \
        __LINE__)(::idle::Inheritance parent) {                                \
      return ::idle::spawn<::std::conditional_t<                               \
          ::idle::is_service<CLASS_NAME_OR_PART>::value, CLASS_NAME_OR_PART,   \
          ::idle::DefaultService<CLASS_NAME_OR_PART>>>(std::move(parent));     \
    }

#  ifdef IDLE_COMPILER_MSVC
#    define IDLE_DECLARE(CLASS_NAME_OR_PART)                                   \
      IDLE_DETAIL_DECLARE_CREATOR(CLASS_NAME_OR_PART)                          \
      IDLE_DETAIL_SECTIONED_ALIAS(                                             \
          IDLE_DETAIL_CAT(idle_create_##CLASS_NAME_OR_PART, __LINE__),         \
          IDLE_DETAIL_CAT(idle_alias_create_##CLASS_NAME_OR_PART, __LINE__),   \
          IDLE_DETAIL_EXPORT_SECTION_NAME)
#  else
#    define IDLE_DECLARE(CLASS_NAME_OR_PART)                                   \
      IDLE_DETAIL_SECTION(IDLE_DETAIL_EXPORT_SECTION_NAME, read)               \
      IDLE_DETAIL_DECLARE_CREATOR(CLASS_NAME_OR_PART)
#  endif
#elif !defined(IDLE_HAS_SUPPRESSED_IDLE_EXECUTABLE) && !defined(DOXYGEN)
#  define IDLE_DECLARE(...)                                                    \
    static_assert(                                                             \
        __LINE__ == 0,                                                         \
        "Bad usage of IDLE_DECLARE! IDLE_DECLARE can only "                    \
        "be used in a compilation unit linked to a shared library, "           \
        "or an executable or object library that defines 'IDLE_EXECUTABLE'! "  \
        "Otherwise I do not know which export stub I shall generate here! "    \
        "Define 'IDLE_HAS_SUPPRESSED_IDLE_EXECUTABLE' to suppress this "       \
        "error.");
#else
  /// Can be used in a compilation unit to create an instance
  /// of the corresponding service immediately when the plugin is loaded.
  ///
  /// It is also also possible to export a Part which is then instantiated as
  /// a DefaultService.
  ///
  /// \attention The name passed to this macro must be a valid C identifier!
#  define IDLE_DECLARE(...)
#endif

#endif // IDLE_CORE_DECLARE_HPP_INCLUDED
