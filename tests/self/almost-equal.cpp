#include <unittest/identity.hpp>
#include <unittest/error.hpp>

#include <iostream>
#include <string>

#include <cstdlib>

void v1 () {
  using unittest::v1::exception;
  using unittest::v1::self;

  try { self.assert_almost_equal(234.01012f, 234.01010f); }
  catch (...) {
    std::clog << "unexpected exception was thrown" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  try { self.assert_almost_equal(267.0002f, 267.000167f, 5); }
  catch (exception const& e) {
    if (std::string { "assert_almost_equal" } != e.type()) {
      std::clog << "unexpected exception '" << e.type() << "' was thrown"
                << std::endl;
      std::exit(EXIT_FAILURE);
    }
    return;
  } catch (...) {
    std::clog << "unexpected exception was thrown" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  std::clog << "assert_almost_equal exception was not thrown as expected!"
            << std::endl;
  std::exit(EXIT_FAILURE);
}

int main () {
  v1();
  return EXIT_SUCCESS;
}
