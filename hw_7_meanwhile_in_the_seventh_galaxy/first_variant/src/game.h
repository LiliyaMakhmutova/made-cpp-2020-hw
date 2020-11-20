#ifndef MEANWHILE_IN_THE_SEVENTH_GALAXY_FIRST_VARIANT_GAME_H
#define MEANWHILE_IN_THE_SEVENTH_GALAXY_FIRST_VARIANT_GAME_H

#include "binary.h"
#include "xor_sum.h"

constexpr size_t MAX_HEAP_SIZE = 1000000;
constexpr size_t MAX_LEADING_ONE_NUM = Log2<MAX_HEAP_SIZE>::value;

template <bool Cond, ssize_t V_true, ssize_t V_false>
struct ternary {
  static constexpr ssize_t value = V_true;
};

template <ssize_t V_true, ssize_t V_false>
struct ternary<false, V_true, V_false> {
  static constexpr ssize_t value = V_false;
};

template <size_t i, ssize_t MaxValue, ssize_t Num>
struct GetLeadingOne {
  static constexpr ssize_t value =
      ternary<GetBit<i, Num>::value, GetLeadingOne<i + 1, i, Num>::value,
              GetLeadingOne<i + 1, MaxValue, Num>::value>::value;
};

template <size_t MaxValue, size_t Num>
struct GetLeadingOne<MAX_LEADING_ONE_NUM, MaxValue, Num> {
  static constexpr ssize_t value = MaxValue;
};

/*
Template implementation of the algorithm
located at https://informatics.mccme.ru/mod/page/view.php?id=19149
*/
template <size_t... Heaps>
struct Game {
  static constexpr bool first_player_wins = XorSum<Heaps...>::value;
  static constexpr ssize_t first_move_heap_index =
      ternary<first_player_wins, FindHeap<Heaps...>::max_idx, -1>::value;
  static constexpr ssize_t first_move_rocks_count =
      ternary<first_player_wins, FindHeap<Heaps...>::to_take, -1>::value;
};

template <size_t LeadingOnePos, size_t CurIdx, size_t MaxIdx, size_t CurMax,
          size_t Head, size_t... Tail>
struct getMaxNum {
  static constexpr size_t value = ternary<
      ((Head & ((1 << (LeadingOnePos + 1)) - 1)) >
       (CurMax & ((1 << (LeadingOnePos + 1)) - 1))),
      getMaxNum<LeadingOnePos, CurIdx + 1, CurIdx, Head, Tail...>::value,
      getMaxNum<LeadingOnePos, CurIdx + 1, MaxIdx, CurMax,
                Tail...>::value>::value;
  static constexpr size_t idx =
      ternary<((Head & ((1 << (LeadingOnePos + 1)) - 1)) >
               (CurMax & ((1 << (LeadingOnePos + 1)) - 1))),
              getMaxNum<LeadingOnePos, CurIdx + 1, CurIdx, Head, Tail...>::idx,
              getMaxNum<LeadingOnePos, CurIdx + 1, MaxIdx, CurMax,
                        Tail...>::idx>::value;
};

template <size_t LeadingOnePos, size_t CurIdx, size_t MaxIdx, size_t CurMax,
          size_t Head>
struct getMaxNum<LeadingOnePos, CurIdx, MaxIdx, CurMax, Head> {
  static constexpr size_t value =
      ternary<((Head & ((1 << (LeadingOnePos + 1)) - 1)) >
               (CurMax & ((1 << (LeadingOnePos + 1)) - 1))),
              Head, CurMax>::value;
  static constexpr size_t idx =
      ternary<((Head & ((1 << (LeadingOnePos + 1)) - 1)) >
               (CurMax & ((1 << (LeadingOnePos + 1)) - 1))),
              CurIdx, MaxIdx>::value;
};

template <size_t... Heaps>
struct FindHeap {
  static constexpr size_t xor_result = XorSum<Heaps...>::value;
  static constexpr size_t leading_one =
      GetLeadingOne<0, 0, XorSum<Heaps...>::value>::value;
  static constexpr size_t max_value =
      getMaxNum<leading_one, 0, 0, 0, Heaps...>::value;
  static constexpr size_t max_idx =
      getMaxNum<leading_one, 0, 0, 0, Heaps...>::idx;
  static constexpr size_t to_take = max_value - (xor_result ^ max_value);
};

#endif  /// MEANWHILE_IN_THE_SEVENTH_GALAXY_FIRST_VARIANT_GAME_H.