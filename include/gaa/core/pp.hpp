/*
convenient preprocessor macro, mostly using in Eigen expr function and X-Macro
*/

#pragma once

#define GAA_PP_STRIP_PARAM(...) __VA_ARGS__
#define GAA_RMB GAA_PP_STRIP_PARAM

#define GAA_PP_gen_1(PLAIN) PLAIN##_1
#define GAA_PP_gen_2(PLAIN) GAA_PP_gen_1(PLAIN), PLAIN##_2
#define GAA_PP_gen_3(PLAIN) GAA_PP_gen_2(PLAIN), PLAIN##_3
#define GAA_PP_gen_4(PLAIN) GAA_PP_gen_3(PLAIN), PLAIN##_4
#define GAA_PP_gen_5(PLAIN) GAA_PP_gen_4(PLAIN), PLAIN##_5
#define GAA_PP_gen_6(PLAIN) GAA_PP_gen_5(PLAIN), PLAIN##_6
#define GAA_PP_gen_7(PLAIN) GAA_PP_gen_6(PLAIN), PLAIN##_7
#define GAA_PP_gen_8(PLAIN) GAA_PP_gen_7(PLAIN), PLAIN##_8
#define GAA_PP_gen_9(PLAIN) GAA_PP_gen_8(PLAIN), PLAIN##_9

#define GAA_PP_REPEAT(N, PLAIN) GAA_PP_gen_##N(PLAIN)

#define GAA_PP_gen2_1(P1, P2) P1##_1 P2##_1
#define GAA_PP_gen2_2(P1, P2) GAA_PP_gen2_1(P1, P2), P1##_2 P2##_2
#define GAA_PP_gen2_3(P1, P2) GAA_PP_gen2_2(P1, P2), P1##_3 P2##_3
#define GAA_PP_gen2_4(P1, P2) GAA_PP_gen2_3(P1, P2), P1##_4 P2##_4
#define GAA_PP_gen2_5(P1, P2) GAA_PP_gen2_4(P1, P2), P1##_5 P2##_5
#define GAA_PP_gen2_6(P1, P2) GAA_PP_gen2_5(P1, P2), P1##_6 P2##_6
#define GAA_PP_gen2_7(P1, P2) GAA_PP_gen2_6(P1, P2), P1##_7 P2##_7
#define GAA_PP_gen2_8(P1, P2) GAA_PP_gen2_7(P1, P2), P1##_8 P2##_8
#define GAA_PP_gen2_9(P1, P2) GAA_PP_gen2_8(P1, P2), P1##_9 P2##_9

#define GAA_PP_REPEAT2(N, P1, P2) GAA_PP_gen2_##N(P1, P2)

#define GAA_PP_gen_suf_1(P1, SUF) P1##_1 GAA_PP_STRIP_PARAM SUF
#define GAA_PP_gen_suf_2(P1, P2)                                               \
  GAA_PP_gen_suf_1(P1, P2), P1##_2 GAA_PP_STRIP_PARAM SUF
#define GAA_PP_gen_suf_3(P1, P2)                                               \
  GAA_PP_gen_suf_2(P1, P2), P1##_3 GAA_PP_STRIP_PARAM SUF
#define GAA_PP_gen_suf_4(P1, P2)                                               \
  GAA_PP_gen_suf_3(P1, P2), P1##_4 GAA_PP_STRIP_PARAM SUF
#define GAA_PP_gen_suf_5(P1, P2)                                               \
  GAA_PP_gen_suf_4(P1, P2), P1##_5 GAA_PP_STRIP_PARAM SUF
#define GAA_PP_gen_suf_6(P1, P2)                                               \
  GAA_PP_gen_suf_5(P1, P2), P1##_6 GAA_PP_STRIP_PARAM SUF
#define GAA_PP_gen_suf_7(P1, P2)                                               \
  GAA_PP_gen_suf_6(P1, P2), P1##_7 GAA_PP_STRIP_PARAM SUF
#define GAA_PP_gen_suf_8(P1, P2)                                               \
  GAA_PP_gen_suf_7(P1, P2), P1##_8 GAA_PP_STRIP_PARAM SUF
#define GAA_PP_gen_suf_9(P1, P2)                                               \
  GAA_PP_gen_suf_8(P1, P2), P1##_9 GAA_PP_STRIP_PARAM SUF

#define GAA_PP_REPEAT_SUFFIX(N, P1, SUF) GAA_PP_gen_suf_##N(P1, SUF)