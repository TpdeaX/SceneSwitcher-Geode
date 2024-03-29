#pragma once
#include <Geode/Geode.hpp>

#define EXPAND(x) x
#define FOR_EACH_1(what, delimiter, x, ...) what(x)
#define FOR_EACH_2(what, delimiter, x, ...)\
  what(x) delimiter \
  EXPAND(FOR_EACH_1(what, delimiter, __VA_ARGS__))
#define FOR_EACH_3(what, delimiter, x, ...)\
  what(x) delimiter \
  EXPAND(FOR_EACH_2(what, delimiter, __VA_ARGS__))
#define FOR_EACH_4(what, delimiter, x, ...)\
  what(x) delimiter \
  EXPAND(FOR_EACH_3(what, delimiter, __VA_ARGS__))
#define FOR_EACH_5(what, delimiter, x, ...)\
  what(x) delimiter \
  EXPAND(FOR_EACH_4(what, delimiter, __VA_ARGS__))
#define FOR_EACH_6(what, delimiter, x, ...)\
  what(x) delimiter \
  EXPAND(FOR_EACH_5(what, delimiter, __VA_ARGS__))
#define FOR_EACH_7(what, delimiter, x, ...)\
  what(x) delimiter \
  EXPAND(FOR_EACH_6(what, delimiter, __VA_ARGS__))
#define FOR_EACH_8(what, delimiter, x, ...)\
  what(x) delimiter \
  EXPAND(FOR_EACH_7(what, delimiter, __VA_ARGS__))
#define FOR_EACH_9(what, delimiter, x, ...)\
  what(x) delimiter \
  EXPAND(FOR_EACH_8(what, delimiter, __VA_ARGS__))
#define FOR_EACH_10(what, delimiter, x, ...)\
  what(x) delimiter \
  EXPAND(FOR_EACH_9(what, delimiter, __VA_ARGS__))
#define FOR_EACH_11(what, delimiter, x, ...)\
  what(x) delimiter \
  EXPAND(FOR_EACH_10(what, delimiter, __VA_ARGS__))
#define FOR_EACH_12(what, delimiter, x, ...)\
  what(x) delimiter\
  EXPAND(FOR_EACH_11(what, delimiter, __VA_ARGS__))
#define FOR_EACH_13(what, delimiter, x, ...)\
  what(x) delimiter \
  EXPAND(FOR_EACH_12(what, delimiter, __VA_ARGS__))
#define FOR_EACH_14(what, delimiter, x, ...)\
  what(x) delimiter \
  EXPAND(FOR_EACH_13(what, delimiter, __VA_ARGS__))
#define FOR_EACH_15(what, delimiter, x, ...)\
  what(x) delimiter \
  EXPAND(FOR_EACH_14(what, delimiter, __VA_ARGS__))
#define FOR_EACH_16(what, delimiter, x, ...)\
  what(x) delimiter \
  EXPAND(FOR_EACH_15(what, delimiter, __VA_ARGS__))

#define FOR_EACH_NARG(...) FOR_EACH_NARG_(__VA_ARGS__, FOR_EACH_RSEQ_N())
#define FOR_EACH_NARG_(...) EXPAND(FOR_EACH_ARG_N(__VA_ARGS__))
#define FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...) N
#define FOR_EACH_RSEQ_N() 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define CONCATENATE(x,y) x##y
#define FOR_EACH_(N, what, delimiter, ...) EXPAND(CONCATENATE(FOR_EACH_, N)(what, delimiter, __VA_ARGS__))


#define FOR_EACH(what, delimiter, ...) FOR_EACH_(FOR_EACH_NARG(__VA_ARGS__), what, delimiter, __VA_ARGS__)

#define ALT_H(str) case hash(str):
#define ALT(...) FOR_EACH(ALT_H, , __VA_ARGS__)
#define RES(res, ...) \
    ALT(res, __VA_ARGS__)

#define RES_SINGLE(res) \
    ALT(res); \
    break;