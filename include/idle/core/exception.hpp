
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

#ifndef IDLE_CORE_EXCEPTION_HPP_INCLUDED
#define IDLE_CORE_EXCEPTION_HPP_INCLUDED

#include <exception>
#include <system_error>
#include <idle/core/api.hpp>

namespace idle {
/// Represents the base class for all exceptions thrown by idle itself
class IDLE_API(idle) Exception : public std::exception {
public:
  Exception();
};

/// Represents a non fatal exception which is ignored when
/// being unhandled by an asynchronous control flow.
///
/// An exception inheriting from async_exception doesn't cause
/// an application abort when being unhandled.
class IDLE_API(idle) AsyncException : public Exception {
public:
  AsyncException();
};
} // namespace idle

#endif // IDLE_CORE_EXCEPTION_HPP_INCLUDED
