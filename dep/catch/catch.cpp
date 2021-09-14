
#include <catch2/catch.hpp>

int main(int argc, char* argv[]) {
  int const result = Catch::Session().run(argc, argv);

  return result;
}
