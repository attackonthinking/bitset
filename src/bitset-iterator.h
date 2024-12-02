#pragma once

#include "bitset-reference.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <utility>

template <typename T>
class bitset_iterator {
  friend class bitset;
  template <typename>
  friend class bitset_reference;
  template <typename>
  friend class bitset_iterator;

public:
  using word_type = T;
  using iterator_category = std::random_access_iterator_tag;
  using value_type = bool;
  using difference_type = std::ptrdiff_t;
  using reference = bitset_reference<T>;
  using mutable_word_type = std::remove_const_t<word_type>;

  static constexpr std::size_t BITS_PER_WORD = std::numeric_limits<word_type>::digits;

  bitset_iterator() = default;
  ~bitset_iterator() = default;
  bitset_iterator(const bitset_iterator&) = default;
  bitset_iterator& operator=(const bitset_iterator&) = default;

  operator bitset_iterator<const word_type>() const {
    return {_wordPtr, _index};
  }

  reference operator*() const {
    return reference(*(_wordPtr + get_word()), get_offset());
  }

  bitset_iterator& operator++() {
    update(1);
    return *this;
  }

  bitset_iterator operator++(int) {
    bitset_iterator tmp = *this;
    ++(*this);
    return tmp;
  }

  bitset_iterator& operator--() {
    update(-1);
    return *this;
  }

  bitset_iterator operator--(int) {
    bitset_iterator tmp = *this;
    --(*this);
    return tmp;
  }

  bitset_iterator& operator+=(difference_type n) {
    update(n);
    return *this;
  }

  bitset_iterator& operator-=(difference_type n) {
    *this += -1 * n;
    return *this;
  }

  bitset_iterator operator+(difference_type n) {
    bitset_iterator temp = *this;
    return temp += n;
  }

  bitset_iterator operator-(difference_type n) {
    bitset_iterator temp = *this;
    return temp -= n;
  }

  friend bitset_iterator operator+(const bitset_iterator& bsi, const difference_type diff) {
    return bitset_iterator(bsi) += diff;
  }

  friend bitset_iterator operator+(const difference_type diff, const bitset_iterator& bsi) {
    return bsi + diff;
  }

  friend bitset_iterator operator-(const bitset_iterator& bsi, const difference_type diff) {
    return bitset_iterator(bsi) -= diff;
  }

  friend difference_type operator-(const bitset_iterator& lhs, const bitset_iterator& rhs) {
    return lhs._index - rhs._index;
  }

  friend bool operator==(const bitset_iterator<word_type>& lhs, const bitset_iterator<word_type>& rhs) {
    return lhs._index == rhs._index;
  }

  friend bool operator!=(const bitset_iterator<word_type>& lhs, const bitset_iterator<word_type>& rhs) {
    return !(lhs == rhs);
  }

  friend bool operator>(const bitset_iterator<word_type>& lhs, const bitset_iterator<word_type>& rhs) {
    return lhs._index > rhs._index;
  }

  friend bool operator<(const bitset_iterator<word_type>& lhs, const bitset_iterator<word_type>& rhs) {
    return lhs._index < rhs._index;
  }

  friend bool operator>=(const bitset_iterator<word_type>& lhs, const bitset_iterator<word_type>& rhs) {
    return !(lhs < rhs);
  }

  friend bool operator<=(const bitset_iterator<word_type>& lhs, const bitset_iterator<word_type>& rhs) {
    return !(lhs > rhs);
  }

  reference operator[](difference_type n) const {
    return *(*this + n);
  }

  word_type get_n_bits(std::size_t n) const {
    std::size_t bitOffset = get_offset();
    std::size_t wordCount = get_word();
    std::size_t window = BITS_PER_WORD - bitOffset;
    std::size_t part_size = std::min<size_t>(n, window);
    mutable_word_type mask = create_mask(part_size);
    mutable_word_type result = (_wordPtr[wordCount] >> bitOffset) & mask;
    if (n > window) {
      std::size_t remaining_bits = n - window;
      result |= (_wordPtr[wordCount + 1] & create_mask(remaining_bits)) << part_size;
    }
    return result;
  }

  void change_n_bits(const word_type& new_bits, std::size_t n) {
    std::size_t bitOffset = get_offset();
    std::size_t wordCount = get_word();
    std::size_t window = BITS_PER_WORD - bitOffset;
    std::size_t part_size = std::min<size_t>(n, window);
    set_bits(_wordPtr[wordCount], new_bits, create_mask(part_size), bitOffset);
    if (n > window) {
      std::size_t remaining_bits = n - window;
      set_bits(_wordPtr[wordCount + 1], new_bits >> window, create_mask(remaining_bits), 0);
    }
  }

  inline static word_type create_mask(std::size_t n) {
    if (n == BITS_PER_WORD) {
      return static_cast<word_type>(-1);
    }
    return (static_cast<word_type>(1) << n) - 1;
  }

private:
  word_type* _wordPtr;
  std::size_t _index;

  bitset_iterator(T* wordPtr, std::size_t index)
      : _wordPtr(wordPtr)
      , _index(index) {}

  size_t get_word() const {
    return _index / BITS_PER_WORD;
  }

  size_t get_offset() const {
    return _index % BITS_PER_WORD;
  }

  void update(difference_type delta) {
    _index += delta;
  }

  inline void set_bits(mutable_word_type& word, word_type new_bits, word_type mask, std::size_t offset) {
    word &= ~(mask << offset);
    word |= (new_bits & mask) << offset;
  }
};
