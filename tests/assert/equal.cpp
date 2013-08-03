#include <iostream>
#include <string>

#include <cstdlib>

#include <unittest/assert.hpp>
#include <unittest/error.hpp>

struct custom { };

void v1 () {
  using unittest::v1::error;
  namespace assert = unittest::v1::assert;

  try { assert::equal(1, 1); }
  catch (...) {
    std::clog << "unexpected error was thrown" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  try { assert::equal(1, 2); }
  catch (error const& e) {
    if (std::string { "equal" } != e.type()) {
      std::clog << "unexpected error '" << e.type() << "' was thrown"
                << std::endl;
      std::exit(EXIT_FAILURE);
    }
  } catch (...) {
    std::clog << "unexpected error thrown" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  try { assert::equal(custom(), custom()); }
  catch (error const& e) {
    if (std::string { "equal" } != e.type()) {
      std::clog << "unexpected error '" << e.type() << "' was thrown"
                << std::endl;
      std::exit(EXIT_FAILURE);
    }
  } catch (...) {
    std::clog << "unexpected error was thrown" << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

int main () {
  v1();
  return EXIT_SUCCESS;
}
