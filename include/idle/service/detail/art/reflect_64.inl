
/*
 *   _____    _ _        .      .    .
 *  |_   _|  | | |  .       .           .
 *    | |  __| | | ___         .    .        .
 *    | | / _` | |/ _ \                .
 *   _| || (_| | |  __/ github.com/Naios/idle
 *  |_____\__,_|_|\___| AGPL v3 (Early Access)
 *
 * Copyright(c) 2018 - 2021 Denis Blank <denis.blank at outlook dot com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define IDLE_DETAIL_FIELD_1(TYPE, I) IDLE_DETAIL_FIELD(TYPE, I)
#define IDLE_DETAIL_FIELD_2(TYPE, I, ...)                                      \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_1(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_3(TYPE, I, ...)                                      \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_2(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_4(TYPE, I, ...)                                      \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_3(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_5(TYPE, I, ...)                                      \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_4(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_6(TYPE, I, ...)                                      \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_5(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_7(TYPE, I, ...)                                      \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_6(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_8(TYPE, I, ...)                                      \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_7(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_9(TYPE, I, ...)                                      \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_8(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_10(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_9(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_11(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_10(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_12(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_11(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_13(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_12(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_14(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_13(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_15(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_14(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_16(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_15(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_17(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_16(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_18(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_17(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_19(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_18(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_20(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_19(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_21(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_20(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_22(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_21(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_23(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_22(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_24(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_23(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_25(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_24(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_26(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_25(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_27(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_26(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_28(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_27(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_29(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_28(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_30(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_29(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_31(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_30(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_32(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_31(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_33(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_32(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_34(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_33(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_35(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_34(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_36(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_35(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_37(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_36(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_38(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_37(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_39(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_38(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_40(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_39(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_41(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_40(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_42(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_41(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_43(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_42(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_44(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_43(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_45(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_44(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_46(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_45(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_47(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_46(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_48(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_47(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_49(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_48(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_50(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_49(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_51(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_50(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_52(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_51(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_53(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_52(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_54(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_53(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_55(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_54(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_56(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_55(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_57(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_56(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_58(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_57(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_59(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_58(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_60(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_59(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_61(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_60(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_62(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_61(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_63(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_62(TYPE, __VA_ARGS__)

#define IDLE_DETAIL_FIELD_COUNT(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11,  \
                                A12, A13, A14, A15, A16, A17, A18, A19, A20,   \
                                A21, A22, A23, A24, A25, A26, A27, A28, A29,   \
                                A30, A31, A32, A33, A34, A35, A36, A37, A38,   \
                                A39, A40, A41, A42, A43, A44, A45, A46, A47,   \
                                A48, A49, A50, A51, A52, A53, A54, A55, A56,   \
                                A57, A58, A59, A60, A61, A62, A63, NAME, ...)  \
  NAME

#define IDLE_DETAIL_REFLECT(TYPE, ...)                                         \
  IDLE_DETAIL_FIELD_COUNT(                                                     \
      __VA_ARGS__, IDLE_DETAIL_FIELD_63, IDLE_DETAIL_FIELD_62,                 \
      IDLE_DETAIL_FIELD_61, IDLE_DETAIL_FIELD_60, IDLE_DETAIL_FIELD_59,        \
      IDLE_DETAIL_FIELD_58, IDLE_DETAIL_FIELD_57, IDLE_DETAIL_FIELD_56,        \
      IDLE_DETAIL_FIELD_55, IDLE_DETAIL_FIELD_54, IDLE_DETAIL_FIELD_53,        \
      IDLE_DETAIL_FIELD_52, IDLE_DETAIL_FIELD_51, IDLE_DETAIL_FIELD_50,        \
      IDLE_DETAIL_FIELD_49, IDLE_DETAIL_FIELD_48, IDLE_DETAIL_FIELD_47,        \
      IDLE_DETAIL_FIELD_46, IDLE_DETAIL_FIELD_45, IDLE_DETAIL_FIELD_44,        \
      IDLE_DETAIL_FIELD_43, IDLE_DETAIL_FIELD_42, IDLE_DETAIL_FIELD_41,        \
      IDLE_DETAIL_FIELD_40, IDLE_DETAIL_FIELD_39, IDLE_DETAIL_FIELD_38,        \
      IDLE_DETAIL_FIELD_37, IDLE_DETAIL_FIELD_36, IDLE_DETAIL_FIELD_35,        \
      IDLE_DETAIL_FIELD_34, IDLE_DETAIL_FIELD_33, IDLE_DETAIL_FIELD_32,        \
      IDLE_DETAIL_FIELD_31, IDLE_DETAIL_FIELD_30, IDLE_DETAIL_FIELD_29,        \
      IDLE_DETAIL_FIELD_28, IDLE_DETAIL_FIELD_27, IDLE_DETAIL_FIELD_26,        \
      IDLE_DETAIL_FIELD_25, IDLE_DETAIL_FIELD_24, IDLE_DETAIL_FIELD_23,        \
      IDLE_DETAIL_FIELD_22, IDLE_DETAIL_FIELD_21, IDLE_DETAIL_FIELD_20,        \
      IDLE_DETAIL_FIELD_19, IDLE_DETAIL_FIELD_18, IDLE_DETAIL_FIELD_17,        \
      IDLE_DETAIL_FIELD_16, IDLE_DETAIL_FIELD_15, IDLE_DETAIL_FIELD_14,        \
      IDLE_DETAIL_FIELD_13, IDLE_DETAIL_FIELD_12, IDLE_DETAIL_FIELD_11,        \
      IDLE_DETAIL_FIELD_10, IDLE_DETAIL_FIELD_9, IDLE_DETAIL_FIELD_8,          \
      IDLE_DETAIL_FIELD_7, IDLE_DETAIL_FIELD_6, IDLE_DETAIL_FIELD_5,           \
      IDLE_DETAIL_FIELD_4, IDLE_DETAIL_FIELD_3, IDLE_DETAIL_FIELD_2,           \
      IDLE_DETAIL_FIELD_1)                                                     \
  (TYPE, __VA_ARGS__)
