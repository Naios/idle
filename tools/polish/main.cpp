
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
#include <atomic>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <regex>
#include <set>
#include <string>
#include <thread>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

using boost::filesystem::directory_iterator;
using boost::filesystem::path;
using boost::filesystem::recursive_directory_iterator;
using boost::process::search_path;
using fmt::format;
using fmt::print;

using namespace std::literals;

struct file_action {
  path file;
  path base;

  bool include_guard{true};
  bool license{true};
  bool format{true};
};

using lines_t = std::vector<std::string>;
using pass_t = bool (*)(file_action const& action, lines_t& lines);
using passes_t = std::vector<pass_t>;

int init(int argc, char** argv);
void run();
void eventually_queue(path const& file, path const& base);
unsigned apply_passes(file_action const& file, lines_t& lines);
void apply_clang_format(path const& file);
void apply_action_to_source(file_action const& file);
void generate_all_include_headers(path const& base);

bool is_cxx_file(path const& file);
bool is_cxx_header_file(path const& file);
bool is_cxx_source_file(path const& file);
bool is_cxx_inline_file(path const& file);
bool is_cxx_in_file(path const& file);
bool is_cmake_file(path const& file);

std::string read_file(path const& file);

lines_t read_file_lines(std::istream& stream);

void trim_empty(lines_t& lines);

passes_t init_passes();

static passes_t const passes = init_passes();
static boost::asio::io_context context;
static std::unordered_map<std::string, lines_t> license_texts;
static path clang_format;
static path cmake_format;
static std::atomic<std::size_t> updated{0U};
static std::atomic<std::size_t> untouched{0U};

int main(int argc, char** argv) {
  if (int ret = init(argc, argv)) {
    return ret;
  }

  run();

  std::size_t const count = updated.load(std::memory_order_seq_cst);
  std::size_t const ok = untouched.load(std::memory_order_seq_cst);

  print(std::cout,
        "Successfully updated {} source file{}. {} are up-to date.\n", count,
        (count == 1) ? "" : "s", ok);

  return EXIT_SUCCESS;
}

static path find_program(std::string const& name, std::string const& flag) {
  auto const program = search_path(name);

  if (program.empty()) {
    print(std::cerr, "Could not find {} (use {} to specify)!\n", name, flag);
  } else {
    if (!exists(program)) {
      print(std::cerr, "{} at '{}' does not exist!\n", name, program);
      return {};
    }
    return program;
  }

  return {};
}

int init(int argc, char** argv) {
  std::vector<std::string> const args(argv, argv + argc);

  if (args.size() < 2) {
    print(std::cerr,
          R"(Supply at least one path:
            -L "license_file.hpp"
            -L "license_file.cmake"
            -I "include_dir"
            -S "source_dir"
            -M "cmake_dir")");
    return EXIT_FAILURE;
  }

  if ((args.size() - 1) % 2) {
    print(std::cerr, "Invalid arguments!\n");
    return EXIT_FAILURE;
  }

  for (std::size_t i = 1U; i < argc; i += 2) {
    if (args[i] == "-L") {
      path license = args[i + 1];

      if (!exists(license)) {
        print(std::cerr, "Invalid license file \'{}\'!\n", license);
        return EXIT_FAILURE;
      }

      {
        std::ifstream stream(license.generic_string());

        if (!stream.is_open()) {
          print(std::cerr, "Failed to open license file {} for reading!\n",
                license);
          return EXIT_FAILURE;
        }

        auto& lines = license_texts[license.extension().generic_string()];

        lines = read_file_lines(stream);
        trim_empty(lines);
      }

    } else if (args[i] == "-I") {
      path dir = args[i + 1];

      if (!exists(dir)) {
        print(std::cerr, "Invalid include dir \'{}\'!\n", dir);
        return EXIT_FAILURE;
      }

      recursive_directory_iterator const end;
      for (recursive_directory_iterator itr(dir); itr != end; ++itr) {
        if (!is_directory(*itr)) {
          eventually_queue(*itr, dir);
        }
      }

      post(context, [dir] {
        generate_all_include_headers(dir); //
      });

    } else if (args[i] == "-S") {
      path dir = args[i + 1];

      if (!exists(dir)) {
        print(std::cerr, "Invalid source dir \'{}\'!\n", dir);
        return EXIT_FAILURE;
      }

      recursive_directory_iterator const end;
      for (recursive_directory_iterator itr(dir); itr != end; ++itr) {
        if (!is_directory(*itr)) {
          eventually_queue(*itr, dir);
        }
      }
    } else if (args[i] == "-M") {
      path dir = args[i + 1];

      if (!exists(dir)) {
        print(std::cerr, "Invalid cmake dir \'{}\'!\n", dir);
        return EXIT_FAILURE;
      }

      recursive_directory_iterator const end;
      for (recursive_directory_iterator itr(dir); itr != end; ++itr) {
        if (!is_directory(*itr) && is_cmake_file(*itr)) {
          eventually_queue(*itr, dir);
        }
      }
    } else if (args[i] == "-C") {
      clang_format = args[i + 1];
    } else if (args[i] == "-F") {
      cmake_format = args[i + 1];
    }
  }

  if (license_texts.empty()) {
    print(std::cerr, "Warning: Empty license file or license not specified!\n");
  }

  if (clang_format.empty()) {
    clang_format = find_program("clang-format", "-C");
  }
  if (cmake_format.empty()) {
    cmake_format = find_program("cmake-format", "-F");
  }

  return EXIT_SUCCESS;
}

void run() {
  std::vector<std::thread> threads;

  std::size_t const count = std::min(std::thread::hardware_concurrency(), 0U);
  for (std::size_t i = 0; i < count; ++i) {
    threads.emplace_back([&]() mutable {
      context.run(); //
    });
  }

  context.run();

  for (std::thread& t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }
}

void eventually_queue(path const& file, path const& base) {
  if (!file.has_extension()) {
    // We never handle files without extensions
    return;
  }

  assert(file.has_stem());
  if (is_directory(file.parent_path() / file.stem())) {
    // We never handle files where the name is also a directory,
    // since those files are auto-generated
    return;
  }

  if (!is_cxx_file(file) && !is_cmake_file(file)) {
    return;
  }

  file_action action;
  action.file = file;
  action.base = base;

  action.include_guard = is_cxx_header_file(file);

  post(context, [action = std::move(action)] {
    apply_action_to_source(action); //
  });
}

// CMake uses:
// .c .C .c++ .cc .cpp .cxx .cu .m .M .mm .h .hh .h++ .hm .hpp .hxx .in .txx

bool is_cxx_header_file(path const& file) {
  static std::regex const regex("^\\.(h|hpp)$");
  return std::regex_match(file.extension().generic_string(), regex);
}

bool is_cxx_source_file(path const& file) {
  static std::regex const regex("^\\.(c|cc|cpp)$");
  return std::regex_match(file.extension().generic_string(), regex);
}

bool is_cxx_inline_file(path const& file) {
  static std::regex const regex("^\\.(inl)$");
  return std::regex_match(file.extension().generic_string(), regex);
}

bool is_cxx_in_file(path const& file) {
  static std::regex const regex("^\\.(in)$");
  return std::regex_match(file.extension().generic_string(), regex);
}

bool is_cxx_file(path const& file) {
  return is_cxx_header_file(file)    //
         || is_cxx_source_file(file) //
         || is_cxx_inline_file(file) //
         || is_cxx_in_file(file);
}

bool is_cmake_file(path const& file) {
  return (file.extension() == ".cmake") ||
         (file.filename() == "CMakeLists.txt");
}

unsigned apply_passes(file_action const& action, lines_t& lines) {
  unsigned fixes = 0U;
  for (pass_t const pass : passes) {
    if (pass(action, lines)) {
      ++fixes;
    }
  }

  return fixes;
}

void apply_clang_format(path const& file) {
  assert(!clang_format.empty());

  boost::process::child c(clang_format, "-i", file.generic_string());
  c.wait();
}

void apply_cmake_format(path const& file) {
  assert(!cmake_format.empty());

  boost::process::child c(cmake_format, "-i", file.generic_string());
  c.wait();
}

static bool check_difference(path const& file, lines_t const& original,
                             lines_t const& patched) {
  if (original.size() != patched.size()) {
    print(stderr, fmt::fg(fmt::terminal_color::red),
          "File {} (lines: {}) has mismatching line count ({} required)\n",
          file, original.size(), patched.size());
    return true;
  }

  std::size_t const size = patched.size();
  for (std::size_t i = 0; i < size; ++i) {
    if (original[i] != patched[i]) {
      print(stderr, fmt::fg(fmt::terminal_color::bright_yellow),
            "File {} differs from patch\n", file);
      print(stderr, fmt::fg(fmt::terminal_color::bright_red), "  {}: - '{}'\n",
            i, original[i]);
      print(stderr, fmt::fg(fmt::terminal_color::bright_green),
            "  {}: + '{}'\n", i, patched[i]);

      return true;
    }
  }

  return false;
}

void write_to_file(std::ofstream& out, path const& file, lines_t& lines) {
  if (!out.is_open()) {
    print(std::cerr, "Error: failed to open file {} for writing!\n", file);
    return;
  }

  for (std::string const& line : lines) {
    out << line << "\n";
  }
}

void apply_action_to_source(file_action const& action) {
  std::ifstream stream(action.file.generic_string());

  if (!stream.is_open()) {
    print(std::cerr, "Failed to open file {} for reading!\n", action.file);
    return;
  }

  lines_t lines = read_file_lines(stream);
  stream.close();
  lines_t const original = lines;

  if (unsigned fixes = apply_passes(action, lines)) {
    check_difference(action.file, original, lines);

    // Write back
    std::ofstream out(action.file.generic_string(), std::ios::trunc);
    write_to_file(out, action.file, lines);

    print(std::cout, "Fixed file {} ({} {} applied)\n", action.file, fixes,
          (fixes > 1) ? "passes" : "pass");

    updated.fetch_add(1, std::memory_order_relaxed);
  } else {
    untouched.fetch_add(1, std::memory_order_relaxed);
  }

  if (!clang_format.empty() && is_cxx_file(action.file)) {
    apply_clang_format(action.file);
  } else if (!cmake_format.empty() && is_cmake_file(action.file)) {
    apply_cmake_format(action.file);
  }
}

path header_of_directory(path dir) {
  return (dir += ".hpp");
}

struct nesting_less {
  bool operator()(path const& left, path const& right) const {
    return left < right;
  }
};

struct all_include_headers {
  std::set<path, nesting_less> included;
  std::set<path> ignored;
};

static all_include_headers gather_all_include_headers(path const& base) {
  all_include_headers headers;

  recursive_directory_iterator const end;
  for (recursive_directory_iterator itr(base); itr != end; ++itr) {
    if (is_directory(*itr)) {
      path const name = itr->path().filename();
      if (name == "detail") {
        continue;
      }
      if (name == "external") {
        continue;
      }
      if (name == "embed") {
        continue;
      }

      if (itr->path().parent_path() == base) {
        // idle.hpp header
        // path const dir = itr->path();
        // path const file = header_of_directory(dir / itr->path().filename());
        // headers.ignored.insert(file);
      } else {
        path const file = header_of_directory(*itr);
        headers.included.insert(file);
      }
    }
  }

  return headers;
}

static void write_include_header(path const& base, path const& header,
                                 path const& gather_dir,
                                 all_include_headers const& all) {
  file_action action;
  action.base = base;
  action.file = header;

  lines_t lines;

  directory_iterator const end;
  for (directory_iterator itr(gather_dir); itr != end; ++itr) {
    if (!is_directory(*itr)) {
      path const current = *itr;

      if (!is_cxx_header_file(current)) {
        continue;
      }

      if (all.ignored.find(current) != all.ignored.end()) {
        continue;
      }

      path const rel = relative(current, base);
      lines.emplace_back(format("#include <{}>", rel.generic_string()));
    }
  }

  std::sort(lines.begin(), lines.end());
  lines.emplace_back("");

  apply_passes(action, lines);

  {
    std::ifstream in(action.file.generic_string());
    if (in.is_open()) {
      lines_t current = read_file_lines(in);

      if (!check_difference(action.file, current, lines)) {
        untouched.fetch_add(1, std::memory_order_relaxed);
        return;
      }
    }
  }

  {
    std::ofstream out(action.file.generic_string(), std::ios::trunc);
    if (!out.is_open()) {
      print(std::cerr, "Failed to open file {} for writing!\n", action.file);
      return;
    }

    write_to_file(out, action.file, lines);
  }

  print(std::cout, "Updated directory include header {}\n", action.file);
  updated.fetch_add(1, std::memory_order_relaxed);

  post(context, [file = action.file] {
    apply_clang_format(file); //
  });
}

void generate_all_include_headers(path const& base) {
  all_include_headers const all = gather_all_include_headers(base);

  for (path const& file : all.included) {
    path const gather = file.parent_path() / file.stem();
    write_include_header(base, file, gather, all);
  }

  for (path const& file : all.ignored) {
    path const gather = file.parent_path();
    write_include_header(base, file, gather, all);
  }
}

std::string read_file(path const& file) {
  std::ifstream stream(file.generic_string());
  return std::string(std::istreambuf_iterator<char>(stream),
                     std::istreambuf_iterator<char>());
}

lines_t read_file_lines(std::istream& stream) {
  lines_t buffer;
  std::string line;
  while (getline(stream, line)) {
    buffer.emplace_back(std::move(line));
  }
  buffer.emplace_back(std::move(line));
  return buffer;
}

template <typename Iterator>
Iterator first_non_blank(Iterator begin, Iterator end) {
  for (auto itr = begin; itr != end; ++itr) {
    if (!itr->empty()) {
      return itr;
    }
  }

  return end;
}

template <typename Iterator>
auto reverse_to_itr(Iterator itr) {
  return std::next(itr).base();
}

void trim_empty(lines_t& lines) {
  {
    // Trim from front
    auto const itr = first_non_blank(lines.begin(), lines.end());
    if (itr != lines.begin()) {
      lines.erase(lines.begin(), itr);
    }
  }

  {
    // Trim from end
    auto const itr = first_non_blank(lines.rbegin(), lines.rend());
    if (itr != lines.rend()) {
      auto const fwd = reverse_to_itr(itr);
      auto const next = std::next(fwd);

      if (next != lines.end()) {
        lines.erase(next, lines.end());
      }
    }
  }
}

template <typename Iterator>
Iterator find_first_in_lines(Iterator itr, Iterator end, std::string const& str,
                             bool stop_on_non_empty_lines = false) {
  for (; itr != end; ++itr) {
    std::string& text = *itr;

    std::size_t const offset = text.find_first_not_of(' ');

    if (offset == std::string::npos) {
      continue;
    }

    if (offset == text.find(str.data(), offset, str.size())) {
      return itr;
    }

    if (stop_on_non_empty_lines) {
      return end;
    }
  }
  return end;
}

struct newline_begin_end_pass {
  static bool apply(file_action const& action, lines_t& lines) {
    if (lines.empty()) {
      return false;
    }
    if (!is_cxx_file(action.file)) {
      return false;
    }

    bool changed = false;

    // Insert a newline in the front if missing
    if (!lines.front().empty()) {
      lines.emplace(lines.begin(), "");
      changed = true;
    }

    // Insert a newline in the back if missing
    if (!lines.back().empty()) {
      lines.push_back("");
      changed = true;
    }

    if (changed) {
      return true; // For debugging purposes
    } else {
      return false;
    }
  }
};

static std::string get_uniform_extension_for_file_format(path const& file) {
  if (is_cxx_file(file)) {
    return ".cpp";
  }
  if (is_cmake_file(file)) {
    return ".cmake";
  }

  return {};
}

struct license_text_pass {
  static bool apply(file_action const& action, lines_t& lines) {
    if (!action.license) {
      return false;
    }

    auto const uniform_ext = get_uniform_extension_for_file_format(action.file);
    if (uniform_ext.empty()) {
      return false;
    }

    auto const itr = license_texts.find(uniform_ext);
    if (itr == license_texts.end()) {
      return false;
    }

    lines_t const& text = itr->second;

    std::string entry = text.front();
    boost::algorithm::trim(entry);
    if (text.front().size() > 2) {
      entry = entry.substr(0, 2);
    }
    std::string leave = text.back();
    boost::algorithm::trim(leave);

    auto opening = find_first_in_lines(lines.begin(), lines.end(), entry, true);
    if (opening == lines.end()) {
      insert_license(text, lines, lines.begin());
      return true;
    }

    auto const closing = find_first_in_lines(opening, lines.end(), leave,
                                             false);

    if (closing == lines.end()) {
      print(std::cerr, "Error: File {} end of license not found!\n",
            action.file);
      return false;
    }

    auto const behind_closing = std::next(closing);
    if (!std::equal(opening, behind_closing, text.begin(), text.end())) {
      opening = lines.erase(opening, behind_closing);
      insert_license(text, lines, opening);
      return true;
    }

    return false;
  }

private:
  static void insert_license(lines_t const& license, lines_t& lines,
                             lines_t::iterator begin) {

    auto itr = lines.insert(begin, license.begin(), license.end());
    auto dist = std::distance(license.begin(), license.end());
    std::advance(itr, dist);

    if (itr != lines.end()) {
      if (!itr->empty()) {
        lines.insert(itr, "");
      }
    }
  }
};

struct include_guard_pass {
  static bool apply(file_action const& action, lines_t& lines) {
    if (!action.include_guard) {
      return false;
    }
    if (!is_cxx_header_file(action.file)) {
      return false;
    }

    auto if_loc = find_first_in_lines(lines.begin(), lines.end(), "#ifndef"s);
    auto def_loc = find_first_in_lines(lines.begin(), lines.end(), "#define"s);
    auto end_loc = find_first_in_lines(lines.rbegin(), lines.rend(), "#endif"s);

    if (((if_loc == lines.end()) != (def_loc == lines.end())) ||
        ((if_loc == lines.end()) != (end_loc == lines.rend()))) {
      print(std::cerr,
            "Warning: File {} does have mismatching include guards!\n",
            action.file);
      return false;
    }

    if ((if_loc != lines.end()) && (def_loc != std::next(if_loc))) {
      print(std::cerr,
            "Warning: File {} define of include guard not detected!\n",
            action.file);
      return false;
    }

    std::string const name = gen_guard_name(action.file, action.base);

    std::string stmt_if = format("#ifndef {}", name);
    std::string stmt_def = format("#define {}", name);
    std::string stmt_end = format("#endif // {}", name);

    if (if_loc == lines.end()) {
      // Freshly insert include guards
      auto license_begin = find_first_in_lines(lines.begin(), lines.end(),
                                               "/*"s, true);

      std::array<std::string, 3> guard = {"", std::move(stmt_if),
                                          std::move(stmt_def)};

      if (license_begin == lines.end()) {
        // No license detected
        lines.insert(lines.begin(), guard.begin(), guard.end());
      } else {
        // Insert after license
        auto license_end = find_first_in_lines(lines.begin(), lines.end(),
                                               "*/"s);

        if (license_end == lines.end()) {
          print(std::cerr, "Warning: File {} could not find end of license!\n",
                action.file);
          return false;
        } else {
          lines.insert(std::next(license_end), guard.begin(), guard.end());
        }
      }

      lines.emplace_back(std::move(stmt_end));
      lines.emplace_back("");
      return true;
    } else {
      // Update include guards
      bool const match_if = *if_loc == stmt_if;
      bool const match_def = *def_loc == stmt_def;
      bool const match_end = *end_loc == stmt_end;

      if (!match_if || !match_def || !match_end) {
        *if_loc = stmt_if;
        *def_loc = stmt_def;
        *end_loc = stmt_end;
        return true;
      } else {
        return false;
      }
    }
  }

private:
  static std::string gen_guard_name(path const& file, path const& base) {
    std::string result = relative(file, base).generic_string();

    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char in) -> unsigned char {
                     switch (in) {
                       case '/':
                       case ' ':
                       case '.':
                       case '-':
                         return '_';
                       default:
                         return static_cast<unsigned char>(std::toupper(in));
                     }
                   });

    if (!result.empty() && !std::isalpha(result[0])) {
      return format(FMT_STRING("FILE_{}_INCLUDED"), std::move(result));
    } else {
      return format(FMT_STRING("{}_INCLUDED"), std::move(result));
    }
  }
};

passes_t init_passes() {
  return {
      &license_text_pass::apply,     //
      &include_guard_pass::apply,    //
      &newline_begin_end_pass::apply //
  };
}
