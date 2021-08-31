// clang-format off
import <ostream>;
import <type_traits>;
#include <boost/test/unit_test.hpp>
import mobula.util;
// clang-format on

namespace {
  enum class Bits { first = 0x01, second = 0x02, third = 0x04 };

  using Flags = mobula::Flags<Bits>;

  constexpr auto operator~(Bits b) noexcept {
    return Flags{~static_cast<std::underlying_type_t<Bits>>(b)};
  }

  constexpr auto operator&(Bits lhs, Bits rhs) noexcept {
    return Flags{lhs} & rhs;
  }

  constexpr auto operator^(Bits lhs, Bits rhs) noexcept {
    return Flags{lhs} ^ rhs;
  }

  constexpr auto operator|(Bits lhs, Bits rhs) noexcept {
    return Flags{lhs} | rhs;
  }

  constexpr auto &operator<<(std::ostream &os, Bits b) {
    switch (b) {
    case Bits::first:
      return os << "first";
    case Bits::second:
      return os << "second";
    case Bits::third:
      return os << "third";
    default:
      return os;
    }
  }
} // namespace

BOOST_AUTO_TEST_CASE(FlagsTest) {
  auto none = Flags{};
  auto all = Bits::first | Bits::second | Bits::third;
  BOOST_TEST(!none);
  BOOST_TEST(!(none & Bits::first));
  BOOST_TEST(!(none & Bits::second));
  BOOST_TEST(!(none & Bits::third));
  BOOST_TEST(all);
  BOOST_TEST(all & Bits::first);
  BOOST_TEST(all & Bits::second);
  BOOST_TEST(all & Bits::third);
  BOOST_TEST(~none == all);
  BOOST_TEST((none & none) == none);
  BOOST_TEST((none | none) == none);
  BOOST_TEST((none ^ none) == none);
  BOOST_TEST((none & all) == none);
  BOOST_TEST((none ^ all) == all);
  BOOST_TEST((none | all) == all);
  BOOST_TEST((all & all) == all);
  BOOST_TEST((all ^ all) == none);
  BOOST_TEST((all | all) == all);
  BOOST_TEST(hash_value(none) != hash_value(all));
}