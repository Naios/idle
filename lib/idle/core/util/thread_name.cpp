
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

#include <algorithm>
#include <array>
#include <idle/core/util/string_view.hpp>
#include <idle/core/util/thread_name.hpp>

#ifndef NDEBUG
#  ifdef IDLE_PLATFORM_WINDOWS
#    include <Windows.h>
#    include <processthreadsapi.h>

#    pragma pack(push, 8)
typedef struct THREADNAME_INFO {
  DWORD dwType;     // Must be 0x1000.
  LPCSTR szName;    // Pointer to name (in user addr space).
  DWORD dwThreadID; // Thread ID (-1=caller thread).
  DWORD dwFlags;    // Reserved for future use, must be zero.
} THREADNAME_INFO;
#    pragma pack(pop)
#  else
#    include <pthread.h>
#  endif
#endif

namespace idle {
static thread_local std::array<char, 65> this_thread_name_{{}};
static thread_local std::size_t this_thread_name_length_{};

void set_this_thread_name(StringView name) noexcept {
  std::size_t const size = std::min(this_thread_name_.size() - 1, name.size());
  std::memcpy(this_thread_name_.data(), name.data(), size);
  this_thread_name_[size] = '\0';
  this_thread_name_length_ = size;

#ifndef NDEBUG
#  ifdef IDLE_PLATFORM_WINDOWS

  HANDLE const this_thread = this_thread_handle();

#    if (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
  // SetThreadDescription can be used also but requires Win10 (winapi 1607+)
  // HRESULT hr = SetThreadDescription(this_thread, L"simulation_thread");
  // if (SUCCEEDED(hr)) {
  //   return;
  // }
#    endif

  // https://docs.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-2008/xcb2z8hs(v=vs.90)?redirectedfrom=MSDN
  THREADNAME_INFO info;
  info.dwType = 0x1000;
  info.szName = this_thread_name_.data();
  info.dwThreadID = GetThreadId(this_thread_handle());
  info.dwFlags = 0;

  constexpr DWORD MS_VC_EXCEPTION = 0x406D1388;

  __try {

    RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR),
                   reinterpret_cast<ULONG_PTR*>(&info));
  } __except (EXCEPTION_EXECUTE_HANDLER) {
  }
#  else
  pthread_setname_np(this_thread_handle(), this_thread_name_.data());
#  endif
#endif
}

StringView this_thread_name() noexcept {
  return {this_thread_name_.data(), this_thread_name_length_};
}

std::thread::native_handle_type this_thread_handle() noexcept {
#ifdef IDLE_PLATFORM_WINDOWS
  return GetCurrentThread();
#else
  return pthread_self();
#endif
}
} // namespace idle
