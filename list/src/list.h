#pragma once
#include <iterator>
#include <utility>

namespace task {

template <class T, class Alloc = std::allocator<T>>
class list {
 public:
  struct node {
    explicit node(T value, node* prev)
        : value(value), prev(prev), next(nullptr) {}
    explicit node(T&& value, node* prev) noexcept : node(prev) {
      value.swap(this.value);
    }
    explicit node(node* prev) : prev(prev), next(nullptr) {}
    struct node* prev;
    T value;
    struct node* next;
  };

  class iterator {
   public:
    using difference_type = ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::bidirectional_iterator_tag;

    iterator() : cur(list::end_iter) {}
    iterator(const iterator& other) : cur(other.cur) {}
    iterator& operator=(const iterator& other) { cur = other.cur; }
    iterator& operator++() {
      auto tmp = *this;
      go_next();
      return tmp;
    }
    iterator operator++(int shift) {
      auto tmp = *this;
      for (int i = 0; i < shift; i++) {
        go_next();
      }
      return tmp;
    }
    reference operator*() const { &(cur->data); }
    pointer operator->() const { return cur; }
    iterator& operator--() {
      auto tmp = *this;
      go_prev();
      return tmp;
    }
    iterator operator--(int) {
      auto tmp = *this;
      go_prev();
      return tmp;
    }
    bool operator==(iterator other) const { return cur == other.cur; }
    bool operator!=(iterator other) const { !(operator=(other)); }

   private:
    pointer cur;

    void go_next() {
      if (!cur || !cur->next || cur == list::end_iter) {
        cur = list::end_iter;
      } else {
        cur = cur->next;
      }
    }
    void go_prev() {
      if (!cur || !cur->prev || cur == list::end_iter) {
        cur = list::end_iter;
      } else {
        cur = cur->prev;
      }
    }
  };

  class const_iterator {
    friend class list;

   public:
    using difference_type = ptrdiff_t;
    using const_value_type = const T;
    using const_pointer = const T*;
    using const_reference = const T&;
    using iterator_category = std::bidirectional_iterator_tag;

    const_iterator(const_pointer cur) : cur(cur) {}
    const_iterator(const const_iterator& other) : cur(other.cur) {}
    const_iterator& operator=(const const_iterator& other) { cur = other.cur; }
    const_iterator& operator++() {
      auto tmp = *this;
      go_next();
      return tmp;
    }
    const_iterator operator++(int shift) {
      auto tmp = *this;
      for (int i = 0; i < shift; i++) {
        go_next();
      }
      return tmp;
    }
    const_reference operator*() const { &(cur->data); }
    const T* operator->() const { return cur; }
    const_iterator& operator--() {
      auto tmp = *this;
      go_prev();
      return tmp;
    }
    const_iterator operator--(int shift) {
      auto tmp = *this;
      for (int i = 0; i < shift; i++) {
        go_next();
      }
      return tmp;
    }
    bool operator==(const_iterator other) const { return cur == other.cur; }
    bool operator!=(const_iterator other) const { return cur != other.cur; }

   private:
    const_pointer cur;
    void go_next() {
      if (!cur || !cur->next || cur == list::end_iter) {
        cur = list::end_iter;
      } else {
        cur = cur->next;
      }
    }
    void go_prev() {
      if (!cur || !cur->prev || cur == list::end_iter) {
        cur = list::end_iter;
      } else {
        cur = cur->prev;
      }
    }
  };

  friend class iterator;
  friend class const_iterator;

  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  list() : head(nullptr), tail(nullptr), size_(0), allocator(Alloc()) {}
  explicit list(const Alloc& alloc)
      : head(nullptr), tail(nullptr), size_(0), allocator(alloc) {}
  list(size_t count, const T& value, const Alloc& alloc = Alloc());
  explicit list(size_t count, const Alloc& alloc = Alloc());

  ~list();

  list(const list& other);
  list(list&& other) noexcept : list() { other.swap(*this); }
  list& operator=(const list& other);
  list& operator=(list&& other);

  Alloc get_allocator() const { return allocator; }

  T& front() { return head->value; }
  const T& front() const { return head->value; }

  T& back() { return tail->value; }
  const T& back() const { return tail->value; }

  iterator begin() { return iterator(head); }
  iterator end() { return iterator(end_iter); }

  const_iterator cbegin() const { return const_iterator(head); }
  const_iterator cend() const { return const_iterator(end_iter); }

  /*
  reverse_iterator rbegin();
  reverse_iterator rend();

  const_reverse_iterator crbegin() const;
  const_reverse_iterator crend() const;
  */

  bool empty() const { return size_ == 0; };
  size_t size() const { return size_; };
  size_t max_size() const {
    return 100500;  // (memavail() - sizeof(list)) / sizeof(node);
  };
  void clear();

  // The container is extended by inserting new elements before the element at
  // the specified position.
  iterator insert(const_iterator pos, const T& value) {
    auto cur = pos->cur;
    size_++;
    if (cur == head) {
      push_front(value);
      return iterator(head);
    } else {
      node* new_node = new (allocator) node(value, cur->prev);
      new_node->next = cur;
      new_node->prev = cur->prev;
      cur->prev->next = new_node;
      return iterator(new_node);
    }
  }

  iterator insert(const_iterator pos, T&& value) {
    auto cur = pos->cur;
    size_++;
    if (cur == head) {
      push_front(value);
      return const_iterator(head);
    } else {
      node* new_node = new (allocator) node(std::forward<T>(value), cur->prev);
      new_node->next = cur;
      new_node->prev = cur->prev;
      cur->prev->next = new_node;
      return const_iterator(new_node);
    }
  }

  iterator insert(const_iterator pos, size_t count, const T& value) {
    auto cur = pos->cur;
    auto cur_head = new (allocator) node(std::forward<T>(value), cur->prev);
    size_ += count;
    if (cur == head) {
      for (int i = 0; i < count - 1; i++) {
        push_front(value);
      }
      return iterator(cur->prev);
    } else {
      node* new_head = new (allocator) node(std::forward<T>(value), cur->prev);
      cur->prev->next = new_head;
      node* new_cur = new_head;
      for (int i = 0; i < count - 1; i++) {
        new_cur->next = new (allocator) node(std::forward<T>(value), new_cur);
        new_cur = new_cur->next;
      }
      new_cur->next = cur;
      new_cur->prev = cur->prev;
      cur->prev = new_cur;
      iterator(new_cur);
    }
  }

  iterator erase(const_iterator pos) {
    if (size_ == 0) {
      // raise an exception
    }
    if (size_ == 1) {
      pop_back();
      return iterator(list::end_ptr);
    }

    auto cur = pos->cur;
    size_--;
    if (cur == head) {
      pop_front();
      return iterator(head);
    } else if (cur == tail) {
      pop_back();
      return iterator(tail);
    } else {
      auto ret = cur->next;
      cur->prev->next = cur->next;
      cur->next->prev = cur->prev;
      delete cur;
      return iterator(ret);
    }
  }

  iterator erase(const_iterator first, const_iterator last) {
    auto cur = first->cur;
    auto last_elem = last->cur;
    int count = 0;
    while (cur != last_elem) {
      auto tmp = cur->next;
      erase(iterator(cur));
      cur = tmp;
      count++;
    }
    size_ -= count;
    return iterator(cur);
  }

  void push_back(const T& value);
  void push_back(T&& value);
  void pop_back();

  void push_front(const T& value);
  void push_front(T&& value);
  void pop_front();

  template <class... Args>
  iterator emplace(const_iterator pos, Args&&... args) {
    auto cur = pos->cur;
    size_++;
    if (cur == head) {
      node* new_node = new (allocator) node(T(std::forward<Args>(args)...));
      new_node->next = head;
      new_node->prev = nullptr;
      head = new_node;
      return const_iterator(head);
    } else {
      node* new_node = new (allocator) node(T(std::forward<Args>(args)...));
      new_node->next = cur;
      new_node->prev = cur->prev;
      cur->prev->next = new_node;
      return const_iterator(new_node);
    }
  }

  template <class... Args>
  void emplace_back(Args&&... args) {
    node* new_node = new (allocator) node(T(std::forward<Args>(args)...));

    if (head == nullptr) {
      head = tail = new_node;
    } else {
      tail->next = new_node;
      tail = new_node;
      tail->next = nullptr;
    }
    size_++;
  }

  template <class... Args>
  void emplace_front(Args&&... args) {
    node* new_node = new (allocator) node(T(std::forward<Args>(args)...));

    if (head == nullptr) {
      head = tail = new_node;
    } else {
      head->prev = new_node;
      new_node->next = head;
      head = new_node;
    }
    size_++;
  }

  void resize(size_t count);
  void swap(list& other);

  void merge(list& other);
  void splice(const_iterator pos, list& other);
  void remove(const T& value);
  void reverse();
  void unique();
  void sort();

  // Your code goes here?..

 private:
  node* head;
  node* tail;
  size_t size_;
  Alloc allocator;
  static inline node* end_iter = new node(nullptr);
  void clear(node* n);
};

///////////////////////////////////////////////////////////////

template <class T, class Alloc>
list<T, Alloc>::list(size_t count, const T& value, const Alloc& alloc) {
  allocator = alloc;
  head = new (allocator) node(value, nullptr);
  node* cur = head;
  for (int i = 0; i < count - 1; i++) {
    cur->next = new (allocator) node(value, cur);
    cur = cur->next;
  }
  tail = cur;
  size_ = count;
}

template <class T, class Alloc>
list<T, Alloc>::list(size_t count, const Alloc& alloc) {
  allocator = alloc;
  head = new (allocator) node(nullptr);
  node* cur = head;
  for (int i = 0; i < count - 1; i++) {
    cur->next = new (allocator) node(cur);
    cur = cur->next;
  }
  tail = cur;
  size_ = count;
}

template <class T, class Alloc>
list<T, Alloc>::~list() {
  clear();
}

template <class T, class Alloc>
list<T, Alloc>::list(const list& other) {
  node* cur = other.head;
  while (cur != nullptr) {
    push_back(cur->value);
    cur = cur->next;
    size_++;
  }
}

template <class T, class Alloc>
list<T, Alloc>& list<T, Alloc>::operator=(const list& other) {
  if (this != other) {
    list tmp(other);
    tmp.swap(*this);
    // destructor of tmp with prev contents of our object will be envoked
    // automatically
  }

  return *this;
}

template <class T, class Alloc>
list<T, Alloc>& list<T, Alloc>::operator=(list&& other) {
  list(std::move(other)).swap(*this);
  return *this;
}

template <class T, class Alloc>
void list<T, Alloc>::clear() {
  while (head) {
    auto next = head->next;
    head->~node();
    delete head;
    head = next;
  }
  tail = nullptr;
  size_ = 0;
}

template <class T, class Alloc>
void list<T, Alloc>::clear(node* n) {
  int count = 0;
  tail = n->prev;
  while (n) {
    T* next = n->next;
    n->~node();
    delete n;
    n = next;
    count++;
  }
  size_ -= count;
}


template <class T, class Alloc>
void list<T, Alloc>::push_back(const T& value) {
  node* new_node = new (allocator) node(value, tail);

  if (head == nullptr) {
    head = tail = new_node;
  } else {
    tail->next = new_node;
    tail = new_node;
  }
  size_++;
}

template <class T, class Alloc>
void list<T, Alloc>::push_back(T&& value) {
  T val = value;
  node* new_node = new /*(allocator)*/ node(val/*std::forward<T>(value)*/, tail);

  if (head == nullptr) {
    head = tail = new_node;
  } else {
    tail->next = new_node;
    tail = new_node;
  }
  size_++;
}

template <class T, class Alloc>
void list<T, Alloc>::pop_back() {
  if (size_ == 0) {
    // throw exception
  }
  node* tmp = tail;
  tail = tail->prev;
  tail->next = nullptr;
  delete tmp;
  size_--;
}

template <class T, class Alloc>
void list<T, Alloc>::push_front(const T& value) {
  node* new_node = new (allocator) node(value, nullptr);

  if (head == nullptr) {
    head = tail = new_node;
  } else {
    head->prev = new_node;
    new_node->next = head;
    head = new_node;
  }
  size_++;
}

template <class T, class Alloc>
void list<T, Alloc>::push_front(T&& value) {
  node* new_node = new (allocator) node(std::forward<T>(value), nullptr);

  if (head == nullptr) {
    head = tail = new_node;
  } else {
    head->prev = new_node;
    new_node->next = head;
    head = new_node;
  }
  size_++;
}

template <class T, class Alloc>
void list<T, Alloc>::pop_front() {
  if (size_ == 0) {
    // throw exception
  }
  node* tmp = head;
  head = head->next;
  head->prev = nullptr;
  delete tmp;
  size_--;
}

template <class T, class Alloc>
void list<T, Alloc>::resize(size_t count) {
  // same as stl
  if (count < size_) {
    node* cur = head;
    for (int i = 0; i < count; i++) {
      cur = cur->next;
    }
    clear(cur);
  } else if (count > size_) {
    for (int i = 0; i < count; i++) {
      push_back(T());
    }
  }
}

template <class T, class Alloc>
void list<T, Alloc>::swap(list& other) {
  list(std::move(other)).swap(*this);
}

template <class T, class Alloc>
void list<T, Alloc>::merge(list& other) {
  if (other.empty()) {
    return;
  }
  node *new_list_head, this_cur, other_cur;
  this_cur = head;
  other_cur = other->head;

  if (this->head->value < other->head->value) {
    new_list_head = this->head;
    this_cur = this_cur->next;
  } else {
    new_list_head = other->head;
    other_cur = other_cur->next;
  }

  node* cur = new_list_head;

  while (this_cur && other_cur) {
    if (this_cur->value < other_cur->value) {
      cur->next = this_cur;
      cur->next->prev = cur;
      this_cur = this_cur->next;
    } else {
      cur->next = other_cur;
      cur->next->prev = cur;
      other_cur = other_cur->next;
    }
    cur = cur->next;
  }

  node* this_tail = this_cur;

  while (this_cur) {
    cur->next = this_cur;
    cur->next->prev = cur;
    this_cur = this_cur->next;
    this_tail = cur;
    cur = cur->next;
  }

  node* other_tail = other_cur;

  while (other_cur) {
    cur->next = other_cur;
    cur->next->prev = cur;
    other_cur = other_cur->next;
    other_tail = cur;
    cur = cur->next;
  }

  head = new_list_head;
  tail = this_tail ? this_tail : other_tail;
  tail->next = nullptr;
  size_ += other->size_;
  other->head = other->tail = nullptr;
  other->size_ = 0;
}

template <class T, class Alloc>
void list<T, Alloc>::remove(const T& value) {
  if (size_ == 0) {
    // exception ??
  } else if (size_ == 1) {
    if (head->value == value) {
      delete head;
      head = tail = nullptr;
      size_ = 0;
    }
    return;
  }

  node* cur = head;
  while (cur != nullptr) {
    if (cur->value == value) {
      break;
    }
    cur = cur->next;
  }
  if (cur) {
    if (cur == head) {
      node* tmp = head;
      head = head->next;
      delete tmp;
      return;
    }
    if (cur == tail) {
      node* tmp = tail;
      tail = tail->prev;
      delete tmp;
      return;
    }
    cur->prev->next = cur->next;
    cur->next->prev = cur->prev;
    delete cur;
    return;
  }
}

template <class T, class Alloc>
void list<T, Alloc>::reverse() {
  node* tmp = nullptr;
  node* cur = head;

  while (cur != nullptr) {
    tmp = cur->prev;
    cur->prev = cur->next;
    cur->next = tmp;
    cur = cur->prev;
  }

  if (tmp != nullptr) {
    head = tmp->prev;
  }
}

template <class T, class Alloc>
void list<T, Alloc>::unique() {
  if (size_ == 0 || size_ == 1) {
    return;
  }

  node* cur = head;
  node* next;

  while (cur->next != nullptr) {
    if (cur->value == cur->next->value) {
      remove(cur->value);
    } else {
      cur = cur->next;
    }
  }
}

template <class T, class Alloc>
void list<T, Alloc>::sort() {
  int i;
  bool swapped;
  node* ptr;
  node* lptr = nullptr;

  if (size_ == 0) {
    return;
  }

  node* start = head;
  do {
    swapped = 0;
    ptr = start;

    while (ptr->next != lptr) {
      if (ptr->data > ptr->next->data) {
        swap(ptr->data, ptr->next->data);
        swapped = 1;
      }
      ptr = ptr->next;
    }
    lptr = ptr;
  } while (swapped);
}

}  // namespace task