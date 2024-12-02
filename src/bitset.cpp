#include "bitset.h"

bitset::bitset() = default;

bitset::bitset(std::size_t size)
    : bit_count(size)
    , words(allocate_memory(size)) {}

bitset::word_type* bitset::allocate_memory(std::size_t size) {
  return size == 0 ? nullptr : new bitset::word_type[(size + BITS_PER_WORD - 1) / BITS_PER_WORD];
}

bitset::bitset(std::size_t size, bool value)
    : bitset(size) {
  std::size_t num_words = (size + BITS_PER_WORD - 1) / BITS_PER_WORD;
  std::fill(words, words + num_words, (value) ? std::numeric_limits<word_type>::max() : static_cast<word_type>(0));
  std::size_t remaining_bits = size % BITS_PER_WORD;
  if (remaining_bits > 0) {
    word_type mask = (static_cast<word_type>(1) << remaining_bits) - 1;
    words[num_words - 1] &= mask;
  }
}

bitset::bitset(const bitset& other)
    : bitset(other.size()) {
  iterator begin = {words, 0};
  std::copy(other.begin(), other.end(), begin);
}

bitset::bitset(const bitset::const_view& other)
    : bitset(other.size()) {
  iterator begin = {words, 0};
  std::copy(other.begin(), other.end(), begin);
}

bitset::bitset(std::string_view str)
    : bitset(str.size(), false) {
  iterator it = {words, 0};
  for (char c : str) {
    *it = c - '0';
    ++it;
  }
}

bitset::bitset(bitset::const_iterator first, bitset::const_iterator last)
    : bitset(const_view(first, last)) {}

bitset::~bitset() {
  delete[] words;
}

void bitset::swap(bitset& other) noexcept {
  std::swap(words, other.words);
  std::swap(bit_count, other.bit_count);
}

std::size_t bitset::size() const {
  return bit_count;
}

bool bitset::empty() const {
  return (bit_count == 0);
}

bitset::iterator bitset::begin() {
  return {words, 0};
}

bitset::const_iterator bitset::begin() const {
  return {words, 0};
}

bitset::iterator bitset::end() {
  return begin() + static_cast<iterator::difference_type>(size());
}

bitset::const_iterator bitset::end() const {
  return begin() + static_cast<const_iterator::difference_type>(size());
}

void swap(bitset& lhs, bitset& rhs) noexcept {
  lhs.swap(rhs);
}

bitset::operator view() {
  return {begin(), end()};
}

bitset::operator const_view() const {
  return {begin(), end()};
}

bitset::view bitset::subview(std::size_t offset, std::size_t count) {
  return this->operator view().subview(offset, count);
}

bitset::const_view bitset::subview(std::size_t offset, std::size_t count) const {
  return this->operator const_view().subview(offset, count);
}

bitset& bitset::operator&=(const const_view& other) & {
  view this_view = subview();
  this_view &= other;
  return *this;
}

bitset& bitset::operator|=(const const_view& other) & {
  view this_view = subview();
  this_view |= other;
  return *this;
}

bitset& bitset::operator^=(const const_view& other) & {
  view this_view = subview();
  this_view ^= other;
  return *this;
}

bitset::reference bitset::operator[](std::size_t index) {
  iterator it = begin();
  return it[static_cast<difference_type>(index)];
}

bitset::const_reference bitset::operator[](std::size_t index) const {
  const_iterator it = begin();
  return it[static_cast<difference_type>(index)];
}

bool bitset::all() const {
  return this->operator const_view().all();
}

bool bitset::any() const {
  return this->operator const_view().any();
}

std::size_t bitset::count() const {
  return this->operator const_view().count();
}

bitset& bitset::flip() & {
  this->operator view().flip();
  return *this;
}

bitset& bitset::set() & {
  this->operator view().set();
  return *this;
}

bitset& bitset::reset() & {
  this->operator view().reset();
  return *this;
}

bitset& bitset::operator=(const bitset& other) & {
  bitset temp = other;
  swap(temp);
  return *this;
}

bitset& bitset::operator=(std::string_view str) & {
  bitset temp = bitset(str);
  swap(temp);
  return *this;
}

bitset& bitset::operator=(const bitset::const_view& other) & {
  bitset temp = bitset(other);
  swap(temp);
  return *this;
}

bitset operator&(const bitset& lhs, const bitset& rhs) {
  bitset result = lhs;
  return (result &= rhs);
}

bitset operator|(const bitset& lhs, const bitset& rhs) {
  bitset result = lhs;
  return (result |= rhs);
}

bitset operator^(const bitset& lhs, const bitset& rhs) {
  bitset result = lhs;
  return (result ^= rhs);
}

bool operator==(const bitset& left, const bitset& right) {
  return bitset::const_view(left) == bitset::const_view(right);
}

bool operator!=(const bitset& left, const bitset& right) {
  return !(left == right);
}

std::string to_string(const bitset& bs) {
  return to_string(bitset::const_view(bs));
}

std::ostream& operator<<(std::ostream& out, const bitset& bs) {
  return out << bitset::const_view(bs);
}

bitset operator<<(const bitset& bs, std::size_t count) {
  bitset result = bs;
  return (result <<= count);
}

bitset operator>>(const bitset& bs, std::size_t count) {
  bitset result = bs;
  return (result >>= count);
}

bitset operator~(const bitset& bs) {
  bitset temp = bitset(bs.size(), true);
  auto bs_view = bitset::const_view(bs);
  temp ^= bs_view;
  return temp;
}

bitset operator^(const bitset::const_view& lhs, const bitset::const_view& rhs) {
  bitset result(lhs);
  result ^= rhs;
  return result;
}

bitset operator|(const bitset::const_view& lhs, const bitset::const_view& rhs) {
  bitset result(lhs);
  result |= rhs;
  return result;
}

bitset operator&(const bitset::const_view& lhs, const bitset::const_view& rhs) {
  bitset result(lhs);
  result &= rhs;
  return result;
}

bitset operator~(const bitset::const_view& bsv) {
  return ~bitset(bsv);
}

bitset operator<<(const bitset::const_view& bs_v, std::size_t count) {
  return bitset(bs_v) << count;
}

bitset operator>>(const bitset::const_view& bs_v, std::size_t count) {
  return bitset(bs_v) >> count;
}

bitset& bitset::shift(std::size_t count, bool is_right) & {
  std::size_t new_size = (is_right) ? (size() - count) : (size() + count);
  bitset new_bitset = bitset(new_size, false);
  iterator out_it = new_bitset.begin();
  iterator new_end = (is_right) ? (end() - static_cast<difference_type>(count)) : end();
  iterator it = begin();
  while (it < new_end) {
    auto remaining_bits = static_cast<std::size_t>(end() - it);
    std::size_t num_bits = std::min(BITS_PER_WORD, remaining_bits);
    out_it.change_n_bits(it.get_n_bits(num_bits), num_bits);
    std::advance(it, num_bits);
    std::advance(out_it, num_bits);
  }
  *this = new_bitset;
  return *this;
}

bitset& bitset::operator>>=(std::size_t count) & {
  if (empty()) {
    return *this;
  }
  if (size() < count) {
    *this = bitset();
    return *this;
  }
  return bitset::shift(count, true);
}

bitset& bitset::operator<<=(std::size_t count) & {
  return bitset::shift(count, false);
}
