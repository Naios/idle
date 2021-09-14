
#ifndef RANG_HPP_INCLUDED
#define RANG_HPP_INCLUDED

// This file is an evil stub for fixing build issues with
// IMAGE_DEBUG_TYPE_EX_DLLCHARACTERISTICS that is included implicitly by rang
// through windows.h.
// Since we don't need color support anyway this should be fine.

namespace rang {
namespace control {
static constexpr char const forceColor[] = "";
} // namespace control
namespace fg {
static constexpr char const cyan[] = "";
static constexpr char const yellow[] = "";
} // namespace fg
namespace bg {
static constexpr char const blue[] = "";
} // namespace bg
namespace style {
static constexpr char const reset[] = "";
} // namespace style
} // namespace rang

#endif // RANG_HPP_INCLUDED
