#pragma once
#include <exception>
#include <vector>

template <typename T>
class ChunkAllocator {
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

  template <typename T>
  class ChunkList {
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

   public:
    ChunkList() : head(nullptr), size(0) {}
    ~ChunkList() { clear(); }

    void clear() {
      while (head) {
        Chunk<T>* p = head;
        head = head->next;
        delete p;
      }
    }

    T* allocate(const size_t n, const size_t chunk_size) {
      if (head) {
        T* result;
        Chunk<T>* cur_chunk = head;
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
        head = new Chunk<T>(chunk_size);
        return head->reserve(n);
      }
    }

   private:
    Chunk<T>* head;
    size_t size;   
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

 

  explicit ChunkAllocator(size_type chunk_size)
      : chunk_size(chunk_size),
        ref_count(new ReferenceCount()) {
    ref_count->inc();
  }

  ChunkAllocator()
      : chunk_size(BASIC_SIZE),
        ref_count(new ReferenceCount()) {
    ref_count->inc();
  }

  // copy constructor
  ChunkAllocator(const ChunkAllocator<value_type>& another)
      : chunk_list(another.chunk_list),
        ref_count(another.ref_count),
        chunk_size(another.chunk_size) {
    ref_count->inc();
  }

  // assignment operator
  ChunkAllocator<value_type>& operator=(
      const ChunkAllocator<value_type>& another) {
    if (this != &another) {
      if (ref_count && ref_count->dec() == 0) {
        delete ref_count;
        chunk_list->clear();
      }
      chunk_list = another.chunk_list;
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
        ref_count = nullptr;
        // linked list's destructor invokes automatically
      }
    }
  }

  pointer allocate(const size_type n) {
    long n_bytes = n * sizeof(T);
    if (n_bytes > chunk_size) {
      throw std::bad_alloc();
    }

    chunk_list.allocate(n, chunk_size);
  }

  void deallocate(pointer p, const size_type n) {}

  template <typename T, typename... Args>
  void construct(T* p, const Args&&... args) {
    new (p) T(std::forward<const Args>(args)...);
  }

  void destroy(pointer p) { p->~T(); }

 private:
  ChunkList <value_type> chunk_list;
  size_type chunk_size;
  ReferenceCount* ref_count = nullptr;
  static const size_type BASIC_SIZE = 1024;

  size_type getChunkSize() { return chunk_size; }
};