#pragma once

#include "bitset-iterator.h"
#include "bitset-reference.h"
#include "bitset.h"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

template <typename T>
class bitset_view {
public:
  using value_type = bool;
  using word_type = T;
  using reference = bitset_reference<word_type>;
  using const_reference = bitset_reference<const word_type>;
  using iterator = bitset_iterator<word_type>;
  using const_iterator = bitset_iterator<const word_type>;
  using view = bitset_view<word_type>;
  using const_view = bitset_view<const word_type>;

  static constexpr std::size_t npos = -1;
  static constexpr std::size_t BITS_PER_WORD = std::numeric_limits<word_type>::digits;
  static constexpr word_type BIT_MASK = std::numeric_limits<word_type>::max();
  static constexpr word_type BIT_MASK_ZERO = ~BIT_MASK;

  bitset_view() = default;
  ~bitset_view() = default;
  bitset_view(const bitset_view& other) = default;
  bitset_view& operator=(const bitset_view& other) = default;

  bitset_view(const iterator begin, const iterator end)
      : _first(begin)
      , _last(end) {}

  operator bitset_view<const word_type>() const {
    return {begin(), end()};
  }

  void swap(bitset_view& other) noexcept {
    std::swap(_first, other._first);
    std::swap(_last, other._last);
  }

  reference operator[](std::size_t index) const {
    return _first[index];
  }

  std::size_t size() const {
    return _last - _first;
  }

  bool empty() const {
    return _last == _first;
  }

  iterator begin() const {
    return _first;
  }

  iterator end() const {
    return _last;
  }

  view operator&=(const const_view& other) const
    requires (!std::is_const_v<word_type>)
  {
    return bitwise_operation(other, std::bit_and<word_type>());
  }

  view operator|=(const const_view& other) const
    requires (!std::is_const_v<word_type>)
  {
    return bitwise_operation(other, std::bit_or<word_type>());
  }

  view operator^=(const const_view& other) const
    requires (!std::is_const_v<word_type>)
  {
    return bitwise_operation(other, std::bit_xor<word_type>());
  }

  view flip() const
    requires (!std::is_const_v<word_type>)
  {
    return bitwise_modify(std::bit_xor<word_type>(), BIT_MASK);
  }

  view set() const
    requires (!std::is_const_v<word_type>)
  {
    return bitwise_modify(std::bit_or<word_type>(), BIT_MASK);
  }

  view reset() const
    requires (!std::is_const_v<word_type>)
  {
    return bitwise_modify(std::bit_and<word_type>(), BIT_MASK_ZERO);
  }

  std::size_t count() const {
    std::size_t result = 0;
    bitwise_process([&](auto&, word_type current_bits, auto) {
      result += std::popcount(current_bits);
      return false;
    });
    return result;
  }

  view subview(std::size_t offset = 0, std::size_t count = npos) const {
    if (offset > size()) {
      return bitset_view<word_type>(end(), end());
    }
    iterator new_first = begin() + offset;
    iterator new_second = (count > size() - offset) ? end() : (new_first + count);
    return bitset_view<word_type>(new_first, new_second);
  }

  bool operator==(const bitset_view<const word_type>& other) const {
    if (size() != other.size()) {
      return false;
    }
    bool result = true;
    const_iterator it_other = other.begin();
    auto check_function = [&](auto&, word_type current_bits, std::size_t num_bits) {
      word_type word_other = it_other.get_n_bits(num_bits);
      if (word_other != current_bits) {
        result = false;
        return true;
      }
      std::advance(it_other, num_bits);
      return false;
    };
    bitwise_process(check_function);
    return result;
  }

  bool operator!=(const bitset_view<const word_type>& other) const {
    return !(*this == other);
  }

  bool all() const {
    return bitwise_check([](word_type bits) { return ~bits != 0; }, false);
  }

  bool any() const {
    if (empty()) {
      return false;
    }
    return bitwise_check([](word_type bits) { return bits != 0; }, true);
  }

private:
  iterator _first;
  iterator _last;

  template <typename Function>
  void bitwise_process(Function function) const {
    iterator it = begin();
    while (it < end()) {
      auto remaining_bits = static_cast<std::size_t>(end() - it);
      std::size_t num_bits = std::min(BITS_PER_WORD, remaining_bits);
      word_type current_bits = it.get_n_bits(num_bits);
      if (function(it, current_bits, num_bits)) {
        break;
      }
      std::advance(it, num_bits);
    }
  }

  template <typename Operation>
  view bitwise_operation(const const_view& other, Operation operation) const {
    const_iterator it_other = other.begin();
    bitwise_process([&](iterator& it_this, word_type current_bits, std::size_t num_bits) {
      word_type word_other = it_other.get_n_bits(num_bits);
      it_this.change_n_bits(operation(current_bits, word_other), num_bits);
      std::advance(it_other, num_bits);
      return false;
    });
    return *this;
  }

  template <typename Operation>
  view bitwise_modify(Operation operation, word_type mask) const {
    bitwise_process([&](iterator& it, word_type current_bits, std::size_t num_bits) {
      it.change_n_bits(operation(current_bits, mask), num_bits);
      return false;
    });
    return *this;
  }

  template <typename Operation>
  bool bitwise_check(Operation function, bool ans) const {
    bool result = !ans;
    auto checker = [&](auto&, word_type current_bits, std::size_t num_bits) {
      if (!ans && (num_bits < BITS_PER_WORD)) {
        result = ((current_bits ^ iterator::create_mask(num_bits)) == 0);
        return true;
      }
      if (function(current_bits)) {
        result = ans;
        return true;
      }
      return false;
    };
    bitwise_process(checker);
    return result;
  }
};

template <typename T>
void swap(bitset_view<T>& lhs, bitset_view<T>& rhs) noexcept {
  lhs.swap(rhs);
}

template <typename T>
std::string to_string(const bitset_view<T>& view) {
  std::string res;
  for (bitset_iterator<const T> it = view.begin(); it < view.end(); ++it) {
    res += (*it) ? '1' : '0';
  }
  return res;
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const bitset_view<T>& view) {
  for (auto it = view.begin(); it != view.end(); ++it) {
    out << ((*it) ? '1' : '0');
  }
  return out;
}
