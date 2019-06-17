#pragma once

#include <iterator>
#include <limits>

namespace rainbow {

template <typename IntegralType = int>
class IntegralIterator final {
 public:
  // TODO: change difference_type to max integer type
  using difference_type = decltype(IntegralType{} - IntegralType{});
  using value_type = IntegralType;
  using pointer = const IntegralType*;
  using reference = const IntegralType&;
  using iterator_category = std::random_access_iterator_tag;

  inline IntegralIterator() : value_{0} {}
  inline IntegralIterator(IntegralType value) : value_(value) {}
  IntegralIterator(const IntegralIterator&) = default;
  IntegralIterator(IntegralIterator&&) = default;
  ~IntegralIterator() = default;

  IntegralIterator& operator=(const IntegralIterator&) = default;
  IntegralIterator& operator=(IntegralIterator&&) = default;

  inline IntegralIterator& operator++() {
    value_ += 1;
    return *this;
  }
  inline IntegralIterator operator++(int) { return IntegralIterator(value_++); }

  inline IntegralIterator& operator--() {
    value_ -= 1;
    return *this;
  }
  inline IntegralIterator operator--(int) { return IntegralIterator(value_--); }

  inline bool operator==(const IntegralIterator& other) const {
    return value_ == other.value_;
  }

  inline bool operator!=(const IntegralIterator& other) const {
    return value_ != other.value_;
  }

  inline reference operator*() const { return value_; }
  inline const IntegralType* operator->() const { return &value_; }

  // Random access requirements
  inline IntegralIterator& operator+=(difference_type difference) {
    value_ += difference;
    return *this;
  }
  inline IntegralIterator operator+(difference_type difference) const {
    return IntegralIterator{value_ + difference};
  }
  inline IntegralIterator& operator-=(difference_type difference) {
    value_ -= difference;
    return *this;
  }
  inline IntegralIterator operator-(difference_type difference) const {
    return IntegralIterator{value_ - difference};
  }
  inline difference_type operator-(IntegralIterator other) const {
    return value_ - other.value_;
  }
  inline reference operator[](difference_type difference) const {
    return value_ + difference;
  }
  inline bool operator<(IntegralIterator other) const {
    return value_ < other.value_;
  }
  inline bool operator>(IntegralIterator other) const {
    return value_ > other.value_;
  }
  inline bool operator<=(IntegralIterator other) const {
    return value_ <= other.value_;
  }
  inline bool operator>=(IntegralIterator other) const {
    return value_ >= other.value_;
  }

 private:
  IntegralType value_;
};

// Random access requirements
template <typename IntegralType>
IntegralIterator<IntegralType> operator+(
    typename IntegralIterator<IntegralType>::difference_type difference,
    const IntegralIterator<IntegralType>& iterator) {
  return IntegralIterator<IntegralType>{iterator + difference};
}

template <typename IntergralType = int>
class IntegralRange final {
 public:
  using value_type = typename IntegralIterator<IntergralType>::value_type;
  using reference = typename IntegralIterator<IntergralType>::reference;
  using const_reference = typename IntegralIterator<IntergralType>::reference;
  using iterator = IntegralIterator<IntergralType>;
  using const_iterator = IntegralIterator<IntergralType>;
  using difference_type =
      typename IntegralIterator<IntergralType>::difference_type;
  using size_type = typename IntegralIterator<IntergralType>::difference_type;

  IntegralRange() = default;
  IntegralRange(IntergralType end) : end_{end} {}
  IntegralRange(IntergralType begin, IntergralType end)
      : begin_{begin}, end_{end} {}
  IntegralRange(const IntegralRange& other) = default;
  ~IntegralRange() = default;

  inline IntegralIterator<IntergralType> begin() { return begin_; }
  inline IntegralIterator<IntergralType> end() { return end_; }

  inline IntegralIterator<IntergralType> cbegin() { return begin_; }
  inline IntegralIterator<IntergralType> cend() { return end_; }

  inline bool operator==(const IntegralRange& other) const {
    return begin_ == other.begin_ && end_ == other.end_;
  }
  inline bool operator!=(const IntegralRange& other) const {
    return begin_ != other.begin_ || end_ != other.end_;
  }

  void swap(IntegralRange& other) {
    std::swap(begin_, other.begin_);
    std::swap(end_, other.end_);
  }

  size_type size() const { return end_ - begin_; }
  size_type max_size() const {
    // TODO: This is just plain wrong!
    return std::numeric_limits<IntergralType>::max();
  }
  bool empty() const { return begin_ == end_; }

 private:
  IntegralIterator<IntergralType> begin_;
  IntegralIterator<IntergralType> end_;
};
}  // namespace rainbow

namespace std {

template <typename IntegralType>
void swap(rainbow::IntegralRange<IntegralType>& first,
          rainbow::IntegralRange<IntegralType>& second) {
  first.swap(second);
}

}  // namespace std
