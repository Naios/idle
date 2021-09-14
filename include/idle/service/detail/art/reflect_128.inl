
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
#define IDLE_DETAIL_FIELD_64(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_63(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_65(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_64(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_66(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_65(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_67(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_66(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_68(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_67(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_69(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_68(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_70(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_69(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_71(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_70(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_72(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_71(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_73(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_72(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_74(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_73(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_75(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_74(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_76(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_75(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_77(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_76(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_78(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_77(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_79(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_78(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_80(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_79(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_81(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_80(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_82(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_81(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_83(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_82(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_84(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_83(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_85(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_84(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_86(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_85(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_87(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_86(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_88(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_87(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_89(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_88(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_90(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_89(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_91(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_90(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_92(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_91(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_93(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_92(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_94(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_93(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_95(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_94(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_96(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_95(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_97(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_96(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_98(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_97(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_99(TYPE, I, ...)                                     \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_98(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_100(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_99(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_101(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_100(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_102(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_101(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_103(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_102(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_104(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_103(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_105(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_104(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_106(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_105(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_107(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_106(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_108(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_107(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_109(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_108(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_110(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_109(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_111(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_110(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_112(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_111(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_113(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_112(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_114(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_113(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_115(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_114(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_116(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_115(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_117(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_116(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_118(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_117(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_119(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_118(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_120(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_119(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_121(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_120(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_122(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_121(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_123(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_122(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_124(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_123(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_125(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_124(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_126(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_125(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_127(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_126(TYPE, __VA_ARGS__)

#define IDLE_DETAIL_FIELD_COUNT(                                               \
    A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16,     \
    A17, A18, A19, A20, A21, A22, A23, A24, A25, A26, A27, A28, A29, A30, A31, \
    A32, A33, A34, A35, A36, A37, A38, A39, A40, A41, A42, A43, A44, A45, A46, \
    A47, A48, A49, A50, A51, A52, A53, A54, A55, A56, A57, A58, A59, A60, A61, \
    A62, A63, A64, A65, A66, A67, A68, A69, A70, A71, A72, A73, A74, A75, A76, \
    A77, A78, A79, A80, A81, A82, A83, A84, A85, A86, A87, A88, A89, A90, A91, \
    A92, A93, A94, A95, A96, A97, A98, A99, A100, A101, A102, A103, A104,      \
    A105, A106, A107, A108, A109, A110, A111, A112, A113, A114, A115, A116,    \
    A117, A118, A119, A120, A121, A122, A123, A124, A125, A126, A127, NAME,    \
    ...)                                                                       \
  NAME

#define IDLE_DETAIL_REFLECT(TYPE, ...)                                         \
  IDLE_DETAIL_FIELD_COUNT(                                                     \
      __VA_ARGS__, IDLE_DETAIL_FIELD_127, IDLE_DETAIL_FIELD_126,               \
      IDLE_DETAIL_FIELD_125, IDLE_DETAIL_FIELD_124, IDLE_DETAIL_FIELD_123,     \
      IDLE_DETAIL_FIELD_122, IDLE_DETAIL_FIELD_121, IDLE_DETAIL_FIELD_120,     \
      IDLE_DETAIL_FIELD_119, IDLE_DETAIL_FIELD_118, IDLE_DETAIL_FIELD_117,     \
      IDLE_DETAIL_FIELD_116, IDLE_DETAIL_FIELD_115, IDLE_DETAIL_FIELD_114,     \
      IDLE_DETAIL_FIELD_113, IDLE_DETAIL_FIELD_112, IDLE_DETAIL_FIELD_111,     \
      IDLE_DETAIL_FIELD_110, IDLE_DETAIL_FIELD_109, IDLE_DETAIL_FIELD_108,     \
      IDLE_DETAIL_FIELD_107, IDLE_DETAIL_FIELD_106, IDLE_DETAIL_FIELD_105,     \
      IDLE_DETAIL_FIELD_104, IDLE_DETAIL_FIELD_103, IDLE_DETAIL_FIELD_102,     \
      IDLE_DETAIL_FIELD_101, IDLE_DETAIL_FIELD_100, IDLE_DETAIL_FIELD_99,      \
      IDLE_DETAIL_FIELD_98, IDLE_DETAIL_FIELD_97, IDLE_DETAIL_FIELD_96,        \
      IDLE_DETAIL_FIELD_95, IDLE_DETAIL_FIELD_94, IDLE_DETAIL_FIELD_93,        \
      IDLE_DETAIL_FIELD_92, IDLE_DETAIL_FIELD_91, IDLE_DETAIL_FIELD_90,        \
      IDLE_DETAIL_FIELD_89, IDLE_DETAIL_FIELD_88, IDLE_DETAIL_FIELD_87,        \
      IDLE_DETAIL_FIELD_86, IDLE_DETAIL_FIELD_85, IDLE_DETAIL_FIELD_84,        \
      IDLE_DETAIL_FIELD_83, IDLE_DETAIL_FIELD_82, IDLE_DETAIL_FIELD_81,        \
      IDLE_DETAIL_FIELD_80, IDLE_DETAIL_FIELD_79, IDLE_DETAIL_FIELD_78,        \
      IDLE_DETAIL_FIELD_77, IDLE_DETAIL_FIELD_76, IDLE_DETAIL_FIELD_75,        \
      IDLE_DETAIL_FIELD_74, IDLE_DETAIL_FIELD_73, IDLE_DETAIL_FIELD_72,        \
      IDLE_DETAIL_FIELD_71, IDLE_DETAIL_FIELD_70, IDLE_DETAIL_FIELD_69,        \
      IDLE_DETAIL_FIELD_68, IDLE_DETAIL_FIELD_67, IDLE_DETAIL_FIELD_66,        \
      IDLE_DETAIL_FIELD_65, IDLE_DETAIL_FIELD_64, IDLE_DETAIL_FIELD_63,        \
      IDLE_DETAIL_FIELD_62, IDLE_DETAIL_FIELD_61, IDLE_DETAIL_FIELD_60,        \
      IDLE_DETAIL_FIELD_59, IDLE_DETAIL_FIELD_58, IDLE_DETAIL_FIELD_57,        \
      IDLE_DETAIL_FIELD_56, IDLE_DETAIL_FIELD_55, IDLE_DETAIL_FIELD_54,        \
      IDLE_DETAIL_FIELD_53, IDLE_DETAIL_FIELD_52, IDLE_DETAIL_FIELD_51,        \
      IDLE_DETAIL_FIELD_50, IDLE_DETAIL_FIELD_49, IDLE_DETAIL_FIELD_48,        \
      IDLE_DETAIL_FIELD_47, IDLE_DETAIL_FIELD_46, IDLE_DETAIL_FIELD_45,        \
      IDLE_DETAIL_FIELD_44, IDLE_DETAIL_FIELD_43, IDLE_DETAIL_FIELD_42,        \
      IDLE_DETAIL_FIELD_41, IDLE_DETAIL_FIELD_40, IDLE_DETAIL_FIELD_39,        \
      IDLE_DETAIL_FIELD_38, IDLE_DETAIL_FIELD_37, IDLE_DETAIL_FIELD_36,        \
      IDLE_DETAIL_FIELD_35, IDLE_DETAIL_FIELD_34, IDLE_DETAIL_FIELD_33,        \
      IDLE_DETAIL_FIELD_32, IDLE_DETAIL_FIELD_31, IDLE_DETAIL_FIELD_30,        \
      IDLE_DETAIL_FIELD_29, IDLE_DETAIL_FIELD_28, IDLE_DETAIL_FIELD_27,        \
      IDLE_DETAIL_FIELD_26, IDLE_DETAIL_FIELD_25, IDLE_DETAIL_FIELD_24,        \
      IDLE_DETAIL_FIELD_23, IDLE_DETAIL_FIELD_22, IDLE_DETAIL_FIELD_21,        \
      IDLE_DETAIL_FIELD_20, IDLE_DETAIL_FIELD_19, IDLE_DETAIL_FIELD_18,        \
      IDLE_DETAIL_FIELD_17, IDLE_DETAIL_FIELD_16, IDLE_DETAIL_FIELD_15,        \
      IDLE_DETAIL_FIELD_14, IDLE_DETAIL_FIELD_13, IDLE_DETAIL_FIELD_12,        \
      IDLE_DETAIL_FIELD_11, IDLE_DETAIL_FIELD_10, IDLE_DETAIL_FIELD_9,         \
      IDLE_DETAIL_FIELD_8, IDLE_DETAIL_FIELD_7, IDLE_DETAIL_FIELD_6,           \
      IDLE_DETAIL_FIELD_5, IDLE_DETAIL_FIELD_4, IDLE_DETAIL_FIELD_3,           \
      IDLE_DETAIL_FIELD_2, IDLE_DETAIL_FIELD_1)                                \
  (TYPE, __VA_ARGS__)
