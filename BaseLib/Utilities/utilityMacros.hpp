/**
 * Utility preprocessor macros.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#define BW_CONCAT_IMPL(A, B) A##B
#define BW_CONCAT(A, B) BW_CONCAT_IMPL(A, B)

#define BW_ARG_COUNT_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, COUNT, ...) COUNT
#define BW_ARG_COUNT(...) BW_ARG_COUNT_IMPL(__VA_ARGS__ __VA_OPT__(,) 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define BW_FOR_EACH_0(MACRO)
#define BW_FOR_EACH_1(MACRO, A1) MACRO(A1)
#define BW_FOR_EACH_2(MACRO, A1, A2) MACRO(A1) MACRO(A2)
#define BW_FOR_EACH_3(MACRO, A1, A2, A3) MACRO(A1) MACRO(A2) MACRO(A3)
#define BW_FOR_EACH_4(MACRO, A1, A2, A3, A4) MACRO(A1) MACRO(A2) MACRO(A3) MACRO(A4)
#define BW_FOR_EACH_5(MACRO, A1, A2, A3, A4, A5) MACRO(A1) MACRO(A2) MACRO(A3) MACRO(A4) MACRO(A5)
#define BW_FOR_EACH_6(MACRO, A1, A2, A3, A4, A5, A6) MACRO(A1) MACRO(A2) MACRO(A3) MACRO(A4) MACRO(A5) MACRO(A6)
#define BW_FOR_EACH_7(MACRO, A1, A2, A3, A4, A5, A6, A7)                                                              \
    MACRO(A1) MACRO(A2) MACRO(A3) MACRO(A4) MACRO(A5) MACRO(A6) MACRO(A7)
#define BW_FOR_EACH_8(MACRO, A1, A2, A3, A4, A5, A6, A7, A8)                                                          \
    MACRO(A1) MACRO(A2) MACRO(A3) MACRO(A4) MACRO(A5) MACRO(A6) MACRO(A7) MACRO(A8)

#define BW_FOR_EACH(MACRO, ...) BW_CONCAT(BW_FOR_EACH_, BW_ARG_COUNT(__VA_ARGS__))(MACRO, __VA_ARGS__)