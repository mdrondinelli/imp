// clang-format off
export module mobula.engine.util:Flags;
import <cstddef>;
import <type_traits>;
// clang-format on

namespace mobula {
  export template<typename T>
  requires std::is_enum_v<T>
  class Flags {
  public:
    constexpr Flags() noexcept: bits_{} {}

    constexpr Flags(T bit) noexcept:
        bits_{static_cast<std::underlying_type_t<T>>(bit)} {}

    explicit constexpr Flags(std::underlying_type_t<T> bits) noexcept:
        bits_{bits} {}

    constexpr auto bits() const noexcept {
      return bits_;
    }

    constexpr operator bool() const noexcept {
      return bits_ != 0;
    }

    constexpr auto operator~() const noexcept {
      return Flags<T>{~bits_};
    }

    friend constexpr auto operator&(Flags<T> lhs, Flags<T> rhs) noexcept {
      return Flags<T>{lhs.bits_ & rhs.bits_};
    }

    friend constexpr auto operator^(Flags<T> lhs, Flags<T> rhs) noexcept {
      return Flags<T>{lhs.bits_ ^ rhs.bits_};
    }

    friend constexpr auto operator|(Flags<T> lhs, Flags<T> rhs) noexcept {
      return Flags<T>{lhs.bits_ | rhs.bits_};
    }

    friend constexpr auto &operator&=(Flags<T> &lhs, Flags<T> rhs) noexcept {
      lhs.bits_ &= rhs.bits_;
      return lhs;
    }

    friend constexpr auto &operator^=(Flags<T> &lhs, Flags<T> rhs) noexcept {
      lhs.bits_ ^= rhs.bits_;
      return lhs;
    }

    friend constexpr auto &operator|=(Flags<T> &lhs, Flags<T> rhs) noexcept {
      lhs.bits_ |= rhs.bits_;
      return lhs;
    }

    friend constexpr std::size_t hash_value(Flags<T> flags) noexcept {
      return flags.bits_;
    }

  private:
    std::underlying_type_t<T> bits_;
  };
} // namespace mobula