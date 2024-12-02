#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

template <typename T>
class bitset_reference {
  friend class bitset;
  template <typename>
  friend class bitset_reference;
  template <typename>
  friend class bitset_iterator;

public:
  bitset_reference() = delete;
  bitset_reference(const bitset_reference& other) = default;
  ~bitset_reference() = default;
  bitset_reference& operator=(const bitset_reference& other) = default;

  operator bitset_reference<const T>() const {
    return {_word, _bitOffset};
  }

  operator bool() const noexcept {
    return (_word & (static_cast<T>(1) << _bitOffset)) != 0;
  }

  bitset_reference& operator=(bool value) noexcept
    requires (!std::is_const_v<T>)
  {
    T bitMask = static_cast<T>(1) << _bitOffset;
    _word = value ? (_word | bitMask) : (_word & ~bitMask);
    return *this;
  }

  void flip() noexcept
    requires (!std::is_const_v<T>)
  {
    _word ^= (T{1} << _bitOffset);
  }

  friend class bitset;

private:
  T& _word;
  std::size_t _bitOffset;

  bitset_reference(T& word, std::size_t bitOffset)
      : _word(word)
      , _bitOffset(bitOffset) {}
};
