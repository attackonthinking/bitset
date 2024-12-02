#pragma once

#include "bitset-iterator.h"
#include "bitset-reference.h"
#include "bitset-view.h"

#include <cstddef>
#include <cstdint>
#include <string_view>

class bitset {
public:
  using value_type = bool;
  using word_type = uint64_t;
  using reference = bitset_reference<word_type>;
  using const_reference = bitset_reference<const word_type>;
  using iterator = bitset_iterator<word_type>;
  using const_iterator = bitset_iterator<const word_type>;
  using view = bitset_view<word_type>;
  using const_view = bitset_view<const word_type>;
  using difference_type = std::ptrdiff_t;

  static constexpr std::size_t npos = -1;
  static constexpr std::size_t BITS_PER_WORD = std::numeric_limits<word_type>::digits;

  bitset();
  bitset(std::size_t size, bool value);
  bitset(const bitset& other);

  explicit bitset(std::string_view str);
  explicit bitset(const const_view& other);
  bitset(const_iterator first, const_iterator last);

  bitset& operator=(const bitset& other) &;
  bitset& operator=(std::string_view str) &;
  bitset& operator=(const const_view& other) &;

  ~bitset();

  void swap(bitset& other) noexcept;

  std::size_t size() const;

  bool empty() const;

  reference operator[](std::size_t index);
  const_reference operator[](std::size_t index) const;

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  bitset& operator&=(const const_view& other) &;
  bitset& operator|=(const const_view& other) &;
  bitset& operator^=(const const_view& other) &;

  bitset& operator<<=(std::size_t count) &;
  bitset& operator>>=(std::size_t count) &;

  bitset& flip() &;

  bitset& set() &;
  bitset& reset() &;

  bool all() const;
  bool any() const;
  std::size_t count() const;

  operator const_view() const;
  operator view();

  view subview(std::size_t offset = 0, std::size_t count = npos);
  const_view subview(std::size_t offset = 0, std::size_t count = npos) const;

private:
  bitset& shift(size_t count, bool is_right) &;
  size_t bit_count = 0;
  word_type* words = nullptr;

  bitset(std::size_t size);
  static word_type* allocate_memory(std::size_t size);
};

bool operator==(const bitset& left, const bitset& right);
bool operator!=(const bitset& left, const bitset& right);

bitset operator&(const bitset& lhs, const bitset& rhs);
bitset operator|(const bitset& lhs, const bitset& rhs);
bitset operator^(const bitset& lhs, const bitset& rhs);

bitset operator&(const bitset::const_view& lhs, const bitset::const_view& rhs);
bitset operator|(const bitset::const_view& lhs, const bitset::const_view& rhs);
bitset operator^(const bitset::const_view& lhs, const bitset::const_view& rhs);
bitset operator~(const bitset::const_view& bs);

bitset operator~(const bitset& bs);
bitset operator<<(const bitset& bs, std::size_t count);
bitset operator>>(const bitset& bs, std::size_t count);
bitset operator<<(const bitset::const_view& bs_v, std::size_t count);
bitset operator>>(const bitset::const_view& bs_v, std::size_t count);

void swap(bitset& lhs, bitset& rhs) noexcept;

std::string to_string(const bitset& bs);
std::ostream& operator<<(std::ostream& out, const bitset& bs);
