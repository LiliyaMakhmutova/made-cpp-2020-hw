#pragma once
#include <exception>
#include <vector>

template <typename T>
class ChunkAllocator {
  template <typename T>
  struct Chunk {
    struct Block {
      explicit Block(size_t start, size_t size)
          : start(start), end(start + size - 1) {}
      Block() = delete;
      size_t getSize() { return end - start + 1; }

      size_t start;
      size_t end;
    };
    explicit Chunk(size_t size)
        : chunk_size(size),
          next(nullptr),
          mem((T*)(new uint8_t[size * sizeof(T)])) {}

    T* reserve(size_t size) {
      if (blocks.size() == 0) {
        blocks.push_back(Block(0, size));
        return (T*)((uint8_t*)mem);
      }
      if (blocks.back().end + 1 + size <= chunk_size) {
        blocks.push_back(Block(blocks.back().end + 1, size));
        return (T*)((uint8_t*)mem + blocks.back().end + 1);
      }
      return nullptr;
    }

    Chunk<T>* next;
    std::vector<Block> blocks;
    size_t chunk_size;
    T* mem;
  };

  class ReferenceCount {
   public:
    ReferenceCount() : ref_count(0) {}
    void inc() { ++ref_count; }
    int dec() { return --ref_count; }
    int getCount() const { return ref_count; }

   private:
    long long ref_count;
  };

 public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  template <typename U>
  struct rebind {
    typedef ChunkAllocator<U> other;
  };

  explicit ChunkAllocator(size_t chunk_size)
      : chunk_head(nullptr),
        chunk_size(chunk_size),
        ref_count(new ReferenceCount()) {
    ref_count->inc();
  }

  ChunkAllocator()
      : chunk_head(nullptr),
        chunk_size(BASIC_SIZE),
        ref_count(new ReferenceCount()) {
    ref_count->inc();
  }

  // copy constructor
  ChunkAllocator(const ChunkAllocator<T>& another)
      : chunk_head(another.chunk_head),
        ref_count(another.ref_count),
        chunk_size(another.chunk_size) {
    ref_count->inc();
  }

  // assignment operator
  ChunkAllocator<T>& operator=(const ChunkAllocator<T>& another) {
    if (this != &another) {
      if (ref_count && ref_count->dec() == 0) {
        delete ref_count;
        clear();
      }
      chunk_head = another.chunk_head;
      ref_count = another.ref_count;
      chunk_size = another.chunk_size;
      ref_count->inc();
    }
    return *this;
  }

  ~ChunkAllocator() {
    if (ref_count) {
      if (ref_count->dec() <= 0) {
        delete ref_count;
        // delete linked list
        clear();
        ref_count = nullptr;
      }
    }
  }

  T* allocate(const size_t n) {
    long n_bytes = n * sizeof(T);
    if (n_bytes > chunk_size) {
      throw std::bad_alloc();
    }

    if (chunk_head) {
      T* result;
      Chunk<T>* cur_chunk = chunk_head;
      Chunk<T>* prev_chunk = nullptr;
      while (cur_chunk != nullptr) {
        if ((result = cur_chunk->reserve(n)) != nullptr) {
          return result;
        }
        prev_chunk = cur_chunk;
        cur_chunk = cur_chunk->next;
      }
      prev_chunk->next = new Chunk<T>(chunk_size);
      return prev_chunk->next->reserve(n);
    } else {
      chunk_head = new Chunk<T>(chunk_size);
      return chunk_head->reserve(n);
    }
  }

  void deallocate(T* p, const size_t n) {}

  template <typename T, typename... Args>
  void construct(T* p, const Args&&... args) {
    new (p) T(args...);
  }

  void destroy(T* p) { p->~T(); }

 private:
  Chunk<T>* chunk_head;
  size_t chunk_size;
  ReferenceCount* ref_count = nullptr;
  static const size_t BASIC_SIZE = 1024;

  size_t getChunkSize() { return chunk_size; }
  void clear() {
    while (chunk_head) {
      Chunk<T>* p = chunk_head;
      chunk_head = chunk_head->next;
      delete p;
    }
  }
};