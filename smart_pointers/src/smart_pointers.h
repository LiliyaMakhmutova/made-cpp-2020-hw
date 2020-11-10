#pragma once
#include <exception>
#include <iostream>
#include <memory>
#include <utility>

std::shared_ptr<int> a;

namespace task {

/*
Конструктор из обычного указателя, конструктор перемещения, перемещающий
оператор присваивания, деструктор
Оператор *, оператор ->, метод get().
Методы release, reset, swap.
*/
template <class T>
class UniquePtr {
 public:
  // UniquePtr(const T* ptr) : ptr(ptr) {}
  UniquePtr(T* ptr) : ptr(ptr) {}
  // UniquePtr(const T& ptr) : ptr(ptr) {}
  // UniquePtr(T& ptr) : ptr(ptr) {}
  // UniquePtr(const T& ptr) : ptr(ptr) {}
  // UniquePtr(T*&& other) noexcept : ptr(other.release()) { }

  UniquePtr(UniquePtr<T>&& other) : ptr(nullptr) { this->swap(other); }

  /*UniquePtr<T>& operator=(UniquePtr<T>&& other) noexcept {
    this->swap(other);
    return *this;
  }*/

  UniquePtr<T>& operator=(UniquePtr<T>&& other) noexcept {
    reset(other.release());
    return *this;
  }

  UniquePtr(const UniquePtr<T>&) = delete;
  UniquePtr<T>& operator=(const UniquePtr<T>&) = delete;

  ~UniquePtr() { delete ptr; }

  T* operator->() const { return ptr; }
  T& operator*() const { return *ptr; }
  T* get() const { return ptr; }

  T* release() noexcept {
    T* result = nullptr;
    std::swap(result, ptr);
    return result;
  }

  void swap(UniquePtr<T>& other) noexcept { std::swap(ptr, other.ptr); }

  void reset() {
    T* tmp = release();
    delete tmp;
  }

  void reset(T* new_ptr) {
    T* tmp = release();
    delete tmp;
    ptr = new_ptr;
  }

 private:
  T* ptr;
};

/*
    Конструктор из обычного указателя, конструктор копирования, конструктор
   перемещения, копирующий и перемещающий операторы присваивания, деструктор
    Конструктор из WeakPtr
    Оператор *, оператор ->, метод get()
    Метод use_count, возвращающий число SharedPtr’ов, указывающих на этот же
   объект Методы reset и swap
*/

template <class T>
class WeakPtr;

class ReferenceCount {
 public:
  ReferenceCount() : ref_count(1) {}
  // void operator++() { ref_count++; }
  // void operator--() { ref_count--; }
  void inc() { ref_count++; }
  void dec() { ref_count--; }
  int get_count() const { return ref_count; }
  void reset() { ref_count = 0; }

 private:
  long ref_count;
};

template <class T>
class SharedPtr {
  friend class WeakPtr<T>;

 public:
  explicit SharedPtr() = default;

  explicit SharedPtr(T* ptr) : ptr(ptr), ref_count(new ReferenceCount()) {}

  SharedPtr(const SharedPtr<T>& other) {
    other.ref_count->inc();
    ptr = other.ptr;
    ref_count = other.ref_count;
  }

  SharedPtr(SharedPtr<T>&& other) noexcept {
    ptr = other.ptr;
    ref_count = other.ref_count;

    other.ptr = nullptr;
    other.ref_count = nullptr;
  }

  explicit SharedPtr(const WeakPtr<T>& other) {
    if (other.ptr && other.ref_count->get_count()) {
      ptr = other.ptr;
      ref_count = other.ref_count;
    } else {
      throw std::bad_weak_ptr();
    }
  }

  SharedPtr<T>& operator=(const SharedPtr<T>& other) noexcept {
    if (ref_count) {
      ref_count->dec();
    }
    SharedPtr<T>(other).swap(*this);
    return *this;
  }

  /*SharedPtr<T>& operator=(SharedPtr<T>&& other) noexcept {
    this->swap(other);
    return *this;
  }*/
  SharedPtr<T>& operator=(SharedPtr<T>&& other) noexcept {    
    SharedPtr<T>(std::move(other)).swap(*this);
    return *this;
  }

  void swap(SharedPtr<T>& other) noexcept {
    std::swap(ptr, other.ptr);
    std::swap(ref_count, other.ref_count);
  }

  T* operator->() const { return ptr; }
  T& operator*() const { return *ptr; }
  T* get() const { return ptr; }

  void reset() { SharedPtr<T>().swap(*this); }

  void reset(T* ptr) { SharedPtr<T>(ptr).swap(*this); }

  long use_count() { return ref_count->get_count(); }

  ~SharedPtr() noexcept {
    if (ref_count) {
      ref_count->dec();
      if (ref_count->get_count() == 0) {
        delete ref_count;
        delete ptr;
      }
    }
  }

 private:
  T* ptr;
  ReferenceCount* ref_count;
};

/*
    Конструктор из SharedPtr, конструктор копирования, конструктор перемещения,
   копирующий и перемещающий операторы присваивания, оператор присваивания
   SharedPtr, деструктор. Метод expired() и метод use_count(), возвращающий
   число SharedPtr’ов, указывающих на этот же объект. Метод lock(), возвращающий
   SharedPtr Методы reset и swap
*/
template <class T>
class WeakPtr {
  friend class SharedPtr<T>;

 public:
  WeakPtr() noexcept {}

  WeakPtr(const SharedPtr<T>& other) noexcept {
    if (other.ref_count) {
      ptr = other.ptr;
      ref_count = other.ref_count;
      weak_ref_count = new ReferenceCount();
    }
  }

  WeakPtr(const WeakPtr<T>& other) noexcept {
    if (other.ref_count) {
      ptr = other.ptr;
      ref_count = other.ref_count;
      weak_ref_count = other.weak_ref_count;
      weak_ref_count->inc();
    }
  }

  WeakPtr(WeakPtr<T>&& other) noexcept {
    ptr = other.ptr;
    ref_count = other.ref_count;
    weak_ref_count = other.weak_ref_count;

    other.ptr = nullptr;
    other.ref_count = nullptr;
    other.weak_ref_count = nullptr;
  }

  WeakPtr<T>& operator=(const WeakPtr<T>& other) {
    if (ref_count) {
      ref_count->dec();
    }
    WeakPtr<T>(other).swap(*this);
    return *this;
  }

  WeakPtr<T>& operator=(WeakPtr<T>&& other) noexcept {
    WeakPtr<T>(std::move(other)).swap(*this);
    return *this;
  }

  WeakPtr<T>& operator=(const SharedPtr<T>& other) noexcept {
    WeakPtr<T>(other).swap(*this);
    return *this;
  }

  ~WeakPtr() {
    if (weak_ref_count) {
      weak_ref_count->dec();
    }
  }

  bool expired() const noexcept { return use_count() == 0; }

  long use_count() const noexcept {
    if (!ref_count) {
      return 0;
    }
    return ref_count->get_count();
  }

  SharedPtr<T> lock() {
    return expired() ? SharedPtr<T>() : SharedPtr<T>(*this);
  }

  void reset() noexcept { WeakPtr<T>().swap(*this); }

  void swap(WeakPtr<T>& other) noexcept {
    std::swap(ptr, other.ptr);
    std::swap(ref_count, other.ref_count);
    std::swap(weak_ref_count, other.weak_ref_count);
  }

 private:
  T* ptr;
  ReferenceCount* ref_count;
  ReferenceCount* weak_ref_count;
};

// Your template function definitions may go here...

}  // namespace task

#include "smart_pointers.tpp"