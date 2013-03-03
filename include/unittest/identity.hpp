#ifndef UNITTEST_IDENTITY_HPP
#define UNITTEST_IDENTITY_HPP
#pragma once

#include <unittest/utility.hpp>
#include <unittest/export.hpp>
#include <unittest/error.hpp>

#include <algorithm>
#include <sstream>
#include <utility>
#include <memory>
#include <regex>

#include <cstdint>

namespace unittest {
inline namespace v1 {

class UNITTEST_EXPORT_API identity final {
  using cstring = const char*;

  std::int64_t statement;

  identity () noexcept;

  void assert_is_not_null(intptr_t, cstring=nullptr);
  void assert_is_null (intptr_t, cstring=nullptr);

  void assert_is_not (intptr_t, intptr_t, cstring=nullptr);
  void assert_is (intptr_t, intptr_t, cstring=nullptr);

  template <typename T, typename U>
  std::string message (
    T const& first, U const& second,
    cstring center, cstring msg=nullptr
  ) {
    std::ostringstream stream;
    if (msg) { stream << msg; }
    else { stream << first << " " << center << " " << second; }
    return stream.str();
  }

public:

  identity& operator = (identity const&) noexcept = delete;
  identity& operator = (identity&&) noexcept = delete;
  identity (identity const&) noexcept = delete;
  identity (identity&&) noexcept = delete;

  ~identity () noexcept;

  static identity& instance () noexcept;
  void reset () noexcept;

  /* assert_equal */
  template <typename T, typename U>
  auto assert_equal (T const& lhs, U const& rhs, cstring msg=nullptr)
  -> typename enable_if<trait::eq<T, U>>::type {
    this->statement += 1;
    if (lhs == rhs) { return; }
    throw exception {
      "assert_equal",
      this->message(lhs, rhs, "is not equal to", msg),
      this->statement
    };
  }

  template <typename T, typename U>
  auto assert_equal (T const&, U const&, cstring=nullptr)
  -> typename disable_if<trait::eq<T, U>>::type {
    this->statement += 1;
    throw exception {
      "assert_equal",
      "Given types do not implement operator ==", 
      this->statement
    };
  }

  /* assert_not_equal */
  template <typename T, typename U>
  auto assert_not_equal (T const& lhs, U const& rhs, cstring msg=nullptr)
  -> typename enable_if<trait::ne<T, U>>::type {
    this->statement += 1;
    if (lhs != rhs) { return; }
    throw exception {
      "assert_not_equal",
      this->message(lhs, rhs, "is equal to", msg),
      this->statement
    };
  }

  template <typename T, typename U>
  auto assert_not_equal (T const&, U const&, cstring=nullptr)
  -> typename disable_if<trait::ne<T, U>>::type {
    this->statement += 1;
    throw exception {
      "assert_not_equal",
      "Given types do not implement operator !=",
      this->statement
    };
  }

  void assert_true (bool, cstring=nullptr);
  void assert_false (bool, cstring=nullptr);

  /* assert_is */
  template <typename T>
  void assert_is (T const& lhs, T const& rhs, cstring msg=nullptr) {
    this->assert_is(std::addressof(lhs), std::addressof(rhs), msg);
  }

  template <typename T>
  void assert_is (T* lhs, T* rhs, cstring msg=nullptr) {
    auto lhs_ = reinterpret_cast<intptr_t>(lhs);
    auto rhs_ = reinterpret_cast<intptr_t>(rhs);
    this->assert_is(lhs_, rhs_, msg);
  }

  /* assert_is_not */
  template <typename T>
  void assert_is_not (T const& lhs, T const& rhs, cstring msg=nullptr) {
    auto one = std::addressof(lhs);
    auto two = std::addressof(rhs);
    this->assert_is_not(one, two, msg);
  }

  template <typename T>
  void assert_is_not (T* lhs, T* rhs, cstring msg=nullptr) {
    auto lhs_ = reinterpret_cast<intptr_t>(lhs);
    auto rhs_ = reinterpret_cast<intptr_t>(rhs);
    this->assert_is_not(lhs_, rhs_, msg);
  }

  /* assert_is_null */
  template <typename T>
  void assert_is_null (T* ptr, cstring msg=nullptr) {
    this->assert_is_null(reinterpret_cast<intptr_t>(ptr), msg);
  }

  /* assert_is_not_null */
  template <typename T>
  void assert_is_not_null (T* ptr, cstring msg=nullptr) {
    this->assert_is_not_null(reinterpret_cast<intptr_t>(ptr), msg);
  }

  /* assert_in */
  template <typename T, typename U>
  auto assert_in (T const& lhs, U const& rhs, cstring msg=nullptr)
  -> typename enable_if<trait::begin<U>, trait::end<U>>::type {
    this->statement += 1;
    auto begin = std::begin(rhs);
    auto end = std::end(rhs);
    auto result = std::find(begin, end, lhs);
    if (result != end) { return; }
    throw exception {
      "assert_in",
      this->message(lhs, rhs, "is not in", msg),
      this->statement
    };
  }

  template <typename T, typename U>
  auto assert_in (T const&, U const& rhs, cstring=nullptr)
  -> typename disable_if<trait::begin<U>, trait::end<U>>::type {
    this->statement += 1;
    std::ostringstream stream;
    stream << rhs << " is not an iterable type" << std::endl;
    throw exception { "assert_in", stream.str(), this->statement };
  }

  /* assert_not_in */
  template <typename T, typename U>
  auto assert_not_in (T const& lhs, U const& rhs, cstring msg=nullptr)
  -> typename enable_if<trait::begin<U>, trait::end<U>>::type {
    this->statement += 1;
    auto begin = std::begin(rhs);
    auto end = std::end(rhs);
    auto result = std::find(begin, end, lhs);
    if (result == end) { return; }
    throw exception {
      "assert_not_in",
      this->message(lhs, rhs, "is in", msg),
      this->statement
    };
  }

  template <typename T, typename U>
  auto assert_not_in (T const&, U const& rhs, cstring=nullptr)
  -> typename disable_if<trait::begin<U>, trait::end<U>>::type {
    this->statement += 1;
    std::ostringstream stream;
    stream << rhs << " is not an iterable type" << std::endl;
    throw exception { "assert_not_in", stream.str(), this->statement };
  }

  /* assert_throws */
  template <typename T>
  void assert_throws (std::function<void()>&& call, cstring msg=nullptr) {
    this->statement += 1;
    try { call(); }
    catch (T const&) { return; }
    throw exception {
      "assert_throws",
      msg ? msg : "Expected exception was not thrown",
      this->statement
    };
  }

  /* assert_almost_equal */
  void assert_almost_equal (float, double, int=4, cstring=nullptr) = delete;
  void assert_almost_equal (double, float, int=4, cstring=nullptr) = delete;

  void assert_almost_equal (double, double, int=4, cstring=nullptr);
  void assert_almost_equal (float, float, int=4, cstring=nullptr);

  /* assert_not_almost_equal */
  void assert_not_almost_equal (float, double, int=4, cstring=nullptr) = delete;
  void assert_not_almost_equal (double, float, int=4, cstring=nullptr) = delete;

  void assert_not_almost_equal (double, double, int=4, cstring=nullptr);
  void assert_not_almost_equal (float, float, int=4, cstring=nullptr);

  /* assert_greater */
  template <typename T, typename U>
  auto assert_greater (T const& lhs, U const& rhs, cstring msg=nullptr)
  -> typename enable_if<trait::gt<T, U>>::type {
    this->statement += 1;
    if (lhs > rhs) { return; }
    throw exception {
      "assert_greater",
      this->message(lhs, rhs, "is not greater than ", msg),
      this->statement
    };
  }

  template <typename T, typename U>
  auto assert_greater (T const&, U const&, cstring=nullptr)
  -> typename disable_if<trait::gt<T, U>>::type {
    this->statement += 1;
    throw exception {
      "assert_greater",
      "Given types do not implement operator >",
      this->statement
    };
  }

  /* assert_greater_equal */
  template <typename T, typename U>
  auto assert_greater_equal (T const& lhs, U const& rhs, cstring msg=nullptr)
  -> typename enable_if<trait::ge<T, U>>::type {
    this->statement += 1;
    if (lhs >= rhs) { return; }
    throw exception {
      "assert_greater_equal",
      this->message(lhs, rhs, "is not greater than or equal to", msg),
      this->statement
    };
  }

  template <typename T, typename U>
  auto assert_greater_equal (T const&, U const&, cstring=nullptr)
  -> typename disable_if<trait::ge<T, U>>::type {
    this->statement += 1;
    throw exception {
      "assert_greater_equal",
      "Given types do not implement operator >=",
      this->statement
    };
  }

  /* assert_less */
  template <typename T, typename U>
  auto assert_less (T const& lhs, U const& rhs, cstring msg=nullptr)
  -> typename enable_if<trait::lt<T, U>>::type {
    this->statement += 1;
    if (lhs < rhs) { return; }
    throw exception {
      "assert_less",
      this->message(lhs, rhs, "is not less than", msg),
      this->statement
    };
  }

  template <typename T, typename U>
  auto assert_less (T const&, U const&, cstring=nullptr)
  -> typename disable_if<trait::lt<T, U>>::type {
    this->statement += 1;
    throw exception {
      "assert_less",
      "Given types do not implement operator <",
      this->statement
    };
  }

  /* assert_less_equal */
  template <typename T, typename U>
  auto assert_less_equal (T const& lhs, U const& rhs, cstring msg=nullptr)
  -> typename enable_if<trait::le<T, U>>::type {
    this->statement += 1;
    if (lhs <= rhs) { return; }
    throw exception {
      "assert_less_equal",
      this->message(lhs, rhs, "is not less than or equal to", msg),
      this->statement
    };
  }

  template <typename T, typename U>
  auto assert_less_equal (T const&, U const&, cstring=nullptr)
  -> typename disable_if<trait::le<T, U>>::type {
    this->statement += 1;
    throw exception {
      "assert_less_equal",
      "Given types do not implement operator <=",
      this->statement
    };
  }

  /* assert_regex */
  void assert_regex(
    std::string const&,
    std::string const&,
    std::regex_constants::syntax_option_type=std::regex_constants::ECMAScript,
    std::regex_constants::match_flag_type=std::regex_constants::match_default,
    cstring=nullptr
  );

  /* assert_not_regex */
  void assert_not_regex(
    std::string const&,
    std::string const&,
    std::regex_constants::syntax_option_type=std::regex_constants::ECMAScript,
    std::regex_constants::match_flag_type=std::regex_constants::match_default,
    cstring=nullptr
  );

  /* assert_count_equal */

  /* assert_associative_equal */
  /* assert_sequence_equal */

  void fail (cstring=nullptr);
};

extern UNITTEST_EXPORT_API identity& self;

}} /* namespace unittest::v1 */

#endif /* UNITTEST_IDENTITY_HPP */
