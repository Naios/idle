#include <boost/system/error_code.hpp>

namespace idle {
/// Reference the generic_category and system_category,
/// otherwise they are removed by the MSVC linker
/// as revealed by: DUMPBIN /SYMBOLS boost.lib
boost::system::error_category const& reference_categories___() {
  return boost::system::generic_category();
}
boost::system::error_category const& reference_system_category___() {
  return boost::system::system_category();
}
} // namespace idle
