
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
#define IDLE_DETAIL_FIELD_128(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_127(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_129(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_128(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_130(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_129(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_131(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_130(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_132(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_131(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_133(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_132(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_134(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_133(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_135(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_134(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_136(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_135(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_137(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_136(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_138(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_137(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_139(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_138(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_140(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_139(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_141(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_140(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_142(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_141(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_143(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_142(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_144(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_143(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_145(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_144(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_146(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_145(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_147(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_146(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_148(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_147(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_149(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_148(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_150(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_149(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_151(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_150(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_152(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_151(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_153(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_152(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_154(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_153(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_155(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_154(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_156(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_155(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_157(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_156(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_158(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_157(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_159(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_158(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_160(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_159(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_161(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_160(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_162(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_161(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_163(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_162(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_164(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_163(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_165(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_164(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_166(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_165(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_167(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_166(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_168(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_167(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_169(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_168(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_170(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_169(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_171(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_170(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_172(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_171(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_173(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_172(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_174(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_173(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_175(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_174(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_176(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_175(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_177(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_176(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_178(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_177(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_179(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_178(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_180(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_179(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_181(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_180(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_182(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_181(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_183(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_182(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_184(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_183(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_185(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_184(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_186(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_185(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_187(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_186(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_188(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_187(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_189(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_188(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_190(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_189(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_191(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_190(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_192(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_191(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_193(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_192(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_194(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_193(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_195(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_194(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_196(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_195(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_197(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_196(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_198(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_197(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_199(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_198(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_200(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_199(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_201(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_200(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_202(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_201(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_203(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_202(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_204(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_203(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_205(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_204(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_206(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_205(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_207(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_206(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_208(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_207(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_209(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_208(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_210(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_209(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_211(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_210(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_212(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_211(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_213(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_212(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_214(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_213(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_215(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_214(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_216(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_215(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_217(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_216(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_218(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_217(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_219(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_218(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_220(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_219(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_221(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_220(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_222(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_221(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_223(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_222(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_224(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_223(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_225(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_224(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_226(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_225(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_227(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_226(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_228(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_227(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_229(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_228(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_230(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_229(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_231(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_230(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_232(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_231(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_233(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_232(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_234(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_233(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_235(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_234(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_236(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_235(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_237(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_236(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_238(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_237(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_239(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_238(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_240(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_239(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_241(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_240(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_242(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_241(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_243(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_242(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_244(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_243(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_245(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_244(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_246(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_245(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_247(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_246(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_248(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_247(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_249(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_248(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_250(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_249(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_251(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_250(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_252(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_251(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_253(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_252(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_254(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_253(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_255(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_254(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_256(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_255(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_257(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_256(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_258(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_257(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_259(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_258(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_260(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_259(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_261(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_260(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_262(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_261(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_263(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_262(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_264(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_263(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_265(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_264(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_266(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_265(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_267(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_266(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_268(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_267(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_269(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_268(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_270(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_269(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_271(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_270(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_272(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_271(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_273(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_272(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_274(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_273(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_275(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_274(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_276(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_275(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_277(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_276(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_278(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_277(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_279(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_278(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_280(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_279(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_281(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_280(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_282(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_281(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_283(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_282(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_284(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_283(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_285(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_284(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_286(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_285(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_287(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_286(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_288(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_287(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_289(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_288(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_290(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_289(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_291(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_290(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_292(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_291(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_293(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_292(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_294(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_293(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_295(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_294(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_296(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_295(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_297(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_296(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_298(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_297(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_299(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_298(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_300(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_299(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_301(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_300(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_302(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_301(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_303(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_302(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_304(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_303(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_305(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_304(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_306(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_305(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_307(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_306(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_308(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_307(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_309(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_308(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_310(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_309(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_311(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_310(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_312(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_311(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_313(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_312(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_314(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_313(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_315(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_314(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_316(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_315(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_317(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_316(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_318(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_317(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_319(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_318(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_320(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_319(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_321(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_320(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_322(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_321(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_323(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_322(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_324(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_323(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_325(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_324(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_326(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_325(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_327(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_326(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_328(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_327(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_329(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_328(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_330(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_329(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_331(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_330(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_332(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_331(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_333(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_332(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_334(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_333(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_335(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_334(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_336(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_335(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_337(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_336(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_338(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_337(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_339(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_338(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_340(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_339(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_341(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_340(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_342(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_341(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_343(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_342(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_344(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_343(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_345(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_344(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_346(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_345(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_347(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_346(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_348(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_347(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_349(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_348(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_350(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_349(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_351(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_350(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_352(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_351(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_353(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_352(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_354(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_353(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_355(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_354(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_356(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_355(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_357(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_356(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_358(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_357(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_359(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_358(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_360(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_359(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_361(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_360(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_362(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_361(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_363(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_362(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_364(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_363(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_365(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_364(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_366(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_365(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_367(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_366(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_368(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_367(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_369(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_368(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_370(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_369(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_371(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_370(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_372(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_371(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_373(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_372(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_374(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_373(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_375(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_374(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_376(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_375(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_377(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_376(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_378(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_377(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_379(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_378(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_380(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_379(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_381(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_380(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_382(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_381(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_383(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_382(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_384(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_383(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_385(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_384(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_386(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_385(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_387(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_386(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_388(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_387(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_389(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_388(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_390(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_389(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_391(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_390(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_392(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_391(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_393(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_392(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_394(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_393(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_395(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_394(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_396(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_395(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_397(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_396(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_398(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_397(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_399(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_398(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_400(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_399(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_401(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_400(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_402(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_401(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_403(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_402(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_404(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_403(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_405(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_404(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_406(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_405(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_407(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_406(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_408(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_407(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_409(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_408(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_410(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_409(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_411(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_410(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_412(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_411(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_413(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_412(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_414(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_413(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_415(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_414(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_416(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_415(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_417(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_416(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_418(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_417(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_419(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_418(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_420(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_419(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_421(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_420(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_422(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_421(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_423(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_422(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_424(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_423(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_425(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_424(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_426(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_425(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_427(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_426(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_428(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_427(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_429(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_428(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_430(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_429(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_431(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_430(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_432(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_431(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_433(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_432(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_434(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_433(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_435(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_434(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_436(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_435(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_437(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_436(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_438(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_437(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_439(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_438(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_440(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_439(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_441(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_440(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_442(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_441(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_443(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_442(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_444(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_443(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_445(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_444(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_446(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_445(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_447(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_446(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_448(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_447(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_449(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_448(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_450(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_449(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_451(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_450(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_452(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_451(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_453(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_452(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_454(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_453(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_455(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_454(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_456(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_455(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_457(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_456(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_458(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_457(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_459(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_458(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_460(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_459(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_461(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_460(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_462(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_461(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_463(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_462(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_464(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_463(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_465(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_464(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_466(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_465(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_467(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_466(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_468(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_467(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_469(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_468(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_470(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_469(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_471(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_470(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_472(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_471(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_473(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_472(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_474(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_473(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_475(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_474(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_476(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_475(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_477(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_476(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_478(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_477(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_479(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_478(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_480(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_479(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_481(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_480(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_482(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_481(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_483(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_482(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_484(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_483(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_485(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_484(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_486(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_485(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_487(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_486(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_488(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_487(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_489(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_488(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_490(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_489(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_491(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_490(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_492(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_491(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_493(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_492(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_494(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_493(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_495(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_494(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_496(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_495(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_497(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_496(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_498(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_497(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_499(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_498(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_500(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_499(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_501(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_500(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_502(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_501(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_503(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_502(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_504(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_503(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_505(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_504(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_506(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_505(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_507(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_506(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_508(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_507(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_509(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_508(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_510(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_509(TYPE, __VA_ARGS__)
#define IDLE_DETAIL_FIELD_511(TYPE, I, ...)                                    \
  IDLE_DETAIL_FIELD(TYPE, I), IDLE_DETAIL_FIELD_510(TYPE, __VA_ARGS__)

#define IDLE_DETAIL_FIELD_COUNT(                                               \
    A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16,     \
    A17, A18, A19, A20, A21, A22, A23, A24, A25, A26, A27, A28, A29, A30, A31, \
    A32, A33, A34, A35, A36, A37, A38, A39, A40, A41, A42, A43, A44, A45, A46, \
    A47, A48, A49, A50, A51, A52, A53, A54, A55, A56, A57, A58, A59, A60, A61, \
    A62, A63, A64, A65, A66, A67, A68, A69, A70, A71, A72, A73, A74, A75, A76, \
    A77, A78, A79, A80, A81, A82, A83, A84, A85, A86, A87, A88, A89, A90, A91, \
    A92, A93, A94, A95, A96, A97, A98, A99, A100, A101, A102, A103, A104,      \
    A105, A106, A107, A108, A109, A110, A111, A112, A113, A114, A115, A116,    \
    A117, A118, A119, A120, A121, A122, A123, A124, A125, A126, A127, A128,    \
    A129, A130, A131, A132, A133, A134, A135, A136, A137, A138, A139, A140,    \
    A141, A142, A143, A144, A145, A146, A147, A148, A149, A150, A151, A152,    \
    A153, A154, A155, A156, A157, A158, A159, A160, A161, A162, A163, A164,    \
    A165, A166, A167, A168, A169, A170, A171, A172, A173, A174, A175, A176,    \
    A177, A178, A179, A180, A181, A182, A183, A184, A185, A186, A187, A188,    \
    A189, A190, A191, A192, A193, A194, A195, A196, A197, A198, A199, A200,    \
    A201, A202, A203, A204, A205, A206, A207, A208, A209, A210, A211, A212,    \
    A213, A214, A215, A216, A217, A218, A219, A220, A221, A222, A223, A224,    \
    A225, A226, A227, A228, A229, A230, A231, A232, A233, A234, A235, A236,    \
    A237, A238, A239, A240, A241, A242, A243, A244, A245, A246, A247, A248,    \
    A249, A250, A251, A252, A253, A254, A255, A256, A257, A258, A259, A260,    \
    A261, A262, A263, A264, A265, A266, A267, A268, A269, A270, A271, A272,    \
    A273, A274, A275, A276, A277, A278, A279, A280, A281, A282, A283, A284,    \
    A285, A286, A287, A288, A289, A290, A291, A292, A293, A294, A295, A296,    \
    A297, A298, A299, A300, A301, A302, A303, A304, A305, A306, A307, A308,    \
    A309, A310, A311, A312, A313, A314, A315, A316, A317, A318, A319, A320,    \
    A321, A322, A323, A324, A325, A326, A327, A328, A329, A330, A331, A332,    \
    A333, A334, A335, A336, A337, A338, A339, A340, A341, A342, A343, A344,    \
    A345, A346, A347, A348, A349, A350, A351, A352, A353, A354, A355, A356,    \
    A357, A358, A359, A360, A361, A362, A363, A364, A365, A366, A367, A368,    \
    A369, A370, A371, A372, A373, A374, A375, A376, A377, A378, A379, A380,    \
    A381, A382, A383, A384, A385, A386, A387, A388, A389, A390, A391, A392,    \
    A393, A394, A395, A396, A397, A398, A399, A400, A401, A402, A403, A404,    \
    A405, A406, A407, A408, A409, A410, A411, A412, A413, A414, A415, A416,    \
    A417, A418, A419, A420, A421, A422, A423, A424, A425, A426, A427, A428,    \
    A429, A430, A431, A432, A433, A434, A435, A436, A437, A438, A439, A440,    \
    A441, A442, A443, A444, A445, A446, A447, A448, A449, A450, A451, A452,    \
    A453, A454, A455, A456, A457, A458, A459, A460, A461, A462, A463, A464,    \
    A465, A466, A467, A468, A469, A470, A471, A472, A473, A474, A475, A476,    \
    A477, A478, A479, A480, A481, A482, A483, A484, A485, A486, A487, A488,    \
    A489, A490, A491, A492, A493, A494, A495, A496, A497, A498, A499, A500,    \
    A501, A502, A503, A504, A505, A506, A507, A508, A509, A510, A511, NAME,    \
    ...)                                                                       \
  NAME

#define IDLE_DETAIL_REFLECT(TYPE, ...)                                         \
  IDLE_DETAIL_FIELD_COUNT(                                                     \
      __VA_ARGS__, IDLE_DETAIL_FIELD_511, IDLE_DETAIL_FIELD_510,               \
      IDLE_DETAIL_FIELD_509, IDLE_DETAIL_FIELD_508, IDLE_DETAIL_FIELD_507,     \
      IDLE_DETAIL_FIELD_506, IDLE_DETAIL_FIELD_505, IDLE_DETAIL_FIELD_504,     \
      IDLE_DETAIL_FIELD_503, IDLE_DETAIL_FIELD_502, IDLE_DETAIL_FIELD_501,     \
      IDLE_DETAIL_FIELD_500, IDLE_DETAIL_FIELD_499, IDLE_DETAIL_FIELD_498,     \
      IDLE_DETAIL_FIELD_497, IDLE_DETAIL_FIELD_496, IDLE_DETAIL_FIELD_495,     \
      IDLE_DETAIL_FIELD_494, IDLE_DETAIL_FIELD_493, IDLE_DETAIL_FIELD_492,     \
      IDLE_DETAIL_FIELD_491, IDLE_DETAIL_FIELD_490, IDLE_DETAIL_FIELD_489,     \
      IDLE_DETAIL_FIELD_488, IDLE_DETAIL_FIELD_487, IDLE_DETAIL_FIELD_486,     \
      IDLE_DETAIL_FIELD_485, IDLE_DETAIL_FIELD_484, IDLE_DETAIL_FIELD_483,     \
      IDLE_DETAIL_FIELD_482, IDLE_DETAIL_FIELD_481, IDLE_DETAIL_FIELD_480,     \
      IDLE_DETAIL_FIELD_479, IDLE_DETAIL_FIELD_478, IDLE_DETAIL_FIELD_477,     \
      IDLE_DETAIL_FIELD_476, IDLE_DETAIL_FIELD_475, IDLE_DETAIL_FIELD_474,     \
      IDLE_DETAIL_FIELD_473, IDLE_DETAIL_FIELD_472, IDLE_DETAIL_FIELD_471,     \
      IDLE_DETAIL_FIELD_470, IDLE_DETAIL_FIELD_469, IDLE_DETAIL_FIELD_468,     \
      IDLE_DETAIL_FIELD_467, IDLE_DETAIL_FIELD_466, IDLE_DETAIL_FIELD_465,     \
      IDLE_DETAIL_FIELD_464, IDLE_DETAIL_FIELD_463, IDLE_DETAIL_FIELD_462,     \
      IDLE_DETAIL_FIELD_461, IDLE_DETAIL_FIELD_460, IDLE_DETAIL_FIELD_459,     \
      IDLE_DETAIL_FIELD_458, IDLE_DETAIL_FIELD_457, IDLE_DETAIL_FIELD_456,     \
      IDLE_DETAIL_FIELD_455, IDLE_DETAIL_FIELD_454, IDLE_DETAIL_FIELD_453,     \
      IDLE_DETAIL_FIELD_452, IDLE_DETAIL_FIELD_451, IDLE_DETAIL_FIELD_450,     \
      IDLE_DETAIL_FIELD_449, IDLE_DETAIL_FIELD_448, IDLE_DETAIL_FIELD_447,     \
      IDLE_DETAIL_FIELD_446, IDLE_DETAIL_FIELD_445, IDLE_DETAIL_FIELD_444,     \
      IDLE_DETAIL_FIELD_443, IDLE_DETAIL_FIELD_442, IDLE_DETAIL_FIELD_441,     \
      IDLE_DETAIL_FIELD_440, IDLE_DETAIL_FIELD_439, IDLE_DETAIL_FIELD_438,     \
      IDLE_DETAIL_FIELD_437, IDLE_DETAIL_FIELD_436, IDLE_DETAIL_FIELD_435,     \
      IDLE_DETAIL_FIELD_434, IDLE_DETAIL_FIELD_433, IDLE_DETAIL_FIELD_432,     \
      IDLE_DETAIL_FIELD_431, IDLE_DETAIL_FIELD_430, IDLE_DETAIL_FIELD_429,     \
      IDLE_DETAIL_FIELD_428, IDLE_DETAIL_FIELD_427, IDLE_DETAIL_FIELD_426,     \
      IDLE_DETAIL_FIELD_425, IDLE_DETAIL_FIELD_424, IDLE_DETAIL_FIELD_423,     \
      IDLE_DETAIL_FIELD_422, IDLE_DETAIL_FIELD_421, IDLE_DETAIL_FIELD_420,     \
      IDLE_DETAIL_FIELD_419, IDLE_DETAIL_FIELD_418, IDLE_DETAIL_FIELD_417,     \
      IDLE_DETAIL_FIELD_416, IDLE_DETAIL_FIELD_415, IDLE_DETAIL_FIELD_414,     \
      IDLE_DETAIL_FIELD_413, IDLE_DETAIL_FIELD_412, IDLE_DETAIL_FIELD_411,     \
      IDLE_DETAIL_FIELD_410, IDLE_DETAIL_FIELD_409, IDLE_DETAIL_FIELD_408,     \
      IDLE_DETAIL_FIELD_407, IDLE_DETAIL_FIELD_406, IDLE_DETAIL_FIELD_405,     \
      IDLE_DETAIL_FIELD_404, IDLE_DETAIL_FIELD_403, IDLE_DETAIL_FIELD_402,     \
      IDLE_DETAIL_FIELD_401, IDLE_DETAIL_FIELD_400, IDLE_DETAIL_FIELD_399,     \
      IDLE_DETAIL_FIELD_398, IDLE_DETAIL_FIELD_397, IDLE_DETAIL_FIELD_396,     \
      IDLE_DETAIL_FIELD_395, IDLE_DETAIL_FIELD_394, IDLE_DETAIL_FIELD_393,     \
      IDLE_DETAIL_FIELD_392, IDLE_DETAIL_FIELD_391, IDLE_DETAIL_FIELD_390,     \
      IDLE_DETAIL_FIELD_389, IDLE_DETAIL_FIELD_388, IDLE_DETAIL_FIELD_387,     \
      IDLE_DETAIL_FIELD_386, IDLE_DETAIL_FIELD_385, IDLE_DETAIL_FIELD_384,     \
      IDLE_DETAIL_FIELD_383, IDLE_DETAIL_FIELD_382, IDLE_DETAIL_FIELD_381,     \
      IDLE_DETAIL_FIELD_380, IDLE_DETAIL_FIELD_379, IDLE_DETAIL_FIELD_378,     \
      IDLE_DETAIL_FIELD_377, IDLE_DETAIL_FIELD_376, IDLE_DETAIL_FIELD_375,     \
      IDLE_DETAIL_FIELD_374, IDLE_DETAIL_FIELD_373, IDLE_DETAIL_FIELD_372,     \
      IDLE_DETAIL_FIELD_371, IDLE_DETAIL_FIELD_370, IDLE_DETAIL_FIELD_369,     \
      IDLE_DETAIL_FIELD_368, IDLE_DETAIL_FIELD_367, IDLE_DETAIL_FIELD_366,     \
      IDLE_DETAIL_FIELD_365, IDLE_DETAIL_FIELD_364, IDLE_DETAIL_FIELD_363,     \
      IDLE_DETAIL_FIELD_362, IDLE_DETAIL_FIELD_361, IDLE_DETAIL_FIELD_360,     \
      IDLE_DETAIL_FIELD_359, IDLE_DETAIL_FIELD_358, IDLE_DETAIL_FIELD_357,     \
      IDLE_DETAIL_FIELD_356, IDLE_DETAIL_FIELD_355, IDLE_DETAIL_FIELD_354,     \
      IDLE_DETAIL_FIELD_353, IDLE_DETAIL_FIELD_352, IDLE_DETAIL_FIELD_351,     \
      IDLE_DETAIL_FIELD_350, IDLE_DETAIL_FIELD_349, IDLE_DETAIL_FIELD_348,     \
      IDLE_DETAIL_FIELD_347, IDLE_DETAIL_FIELD_346, IDLE_DETAIL_FIELD_345,     \
      IDLE_DETAIL_FIELD_344, IDLE_DETAIL_FIELD_343, IDLE_DETAIL_FIELD_342,     \
      IDLE_DETAIL_FIELD_341, IDLE_DETAIL_FIELD_340, IDLE_DETAIL_FIELD_339,     \
      IDLE_DETAIL_FIELD_338, IDLE_DETAIL_FIELD_337, IDLE_DETAIL_FIELD_336,     \
      IDLE_DETAIL_FIELD_335, IDLE_DETAIL_FIELD_334, IDLE_DETAIL_FIELD_333,     \
      IDLE_DETAIL_FIELD_332, IDLE_DETAIL_FIELD_331, IDLE_DETAIL_FIELD_330,     \
      IDLE_DETAIL_FIELD_329, IDLE_DETAIL_FIELD_328, IDLE_DETAIL_FIELD_327,     \
      IDLE_DETAIL_FIELD_326, IDLE_DETAIL_FIELD_325, IDLE_DETAIL_FIELD_324,     \
      IDLE_DETAIL_FIELD_323, IDLE_DETAIL_FIELD_322, IDLE_DETAIL_FIELD_321,     \
      IDLE_DETAIL_FIELD_320, IDLE_DETAIL_FIELD_319, IDLE_DETAIL_FIELD_318,     \
      IDLE_DETAIL_FIELD_317, IDLE_DETAIL_FIELD_316, IDLE_DETAIL_FIELD_315,     \
      IDLE_DETAIL_FIELD_314, IDLE_DETAIL_FIELD_313, IDLE_DETAIL_FIELD_312,     \
      IDLE_DETAIL_FIELD_311, IDLE_DETAIL_FIELD_310, IDLE_DETAIL_FIELD_309,     \
      IDLE_DETAIL_FIELD_308, IDLE_DETAIL_FIELD_307, IDLE_DETAIL_FIELD_306,     \
      IDLE_DETAIL_FIELD_305, IDLE_DETAIL_FIELD_304, IDLE_DETAIL_FIELD_303,     \
      IDLE_DETAIL_FIELD_302, IDLE_DETAIL_FIELD_301, IDLE_DETAIL_FIELD_300,     \
      IDLE_DETAIL_FIELD_299, IDLE_DETAIL_FIELD_298, IDLE_DETAIL_FIELD_297,     \
      IDLE_DETAIL_FIELD_296, IDLE_DETAIL_FIELD_295, IDLE_DETAIL_FIELD_294,     \
      IDLE_DETAIL_FIELD_293, IDLE_DETAIL_FIELD_292, IDLE_DETAIL_FIELD_291,     \
      IDLE_DETAIL_FIELD_290, IDLE_DETAIL_FIELD_289, IDLE_DETAIL_FIELD_288,     \
      IDLE_DETAIL_FIELD_287, IDLE_DETAIL_FIELD_286, IDLE_DETAIL_FIELD_285,     \
      IDLE_DETAIL_FIELD_284, IDLE_DETAIL_FIELD_283, IDLE_DETAIL_FIELD_282,     \
      IDLE_DETAIL_FIELD_281, IDLE_DETAIL_FIELD_280, IDLE_DETAIL_FIELD_279,     \
      IDLE_DETAIL_FIELD_278, IDLE_DETAIL_FIELD_277, IDLE_DETAIL_FIELD_276,     \
      IDLE_DETAIL_FIELD_275, IDLE_DETAIL_FIELD_274, IDLE_DETAIL_FIELD_273,     \
      IDLE_DETAIL_FIELD_272, IDLE_DETAIL_FIELD_271, IDLE_DETAIL_FIELD_270,     \
      IDLE_DETAIL_FIELD_269, IDLE_DETAIL_FIELD_268, IDLE_DETAIL_FIELD_267,     \
      IDLE_DETAIL_FIELD_266, IDLE_DETAIL_FIELD_265, IDLE_DETAIL_FIELD_264,     \
      IDLE_DETAIL_FIELD_263, IDLE_DETAIL_FIELD_262, IDLE_DETAIL_FIELD_261,     \
      IDLE_DETAIL_FIELD_260, IDLE_DETAIL_FIELD_259, IDLE_DETAIL_FIELD_258,     \
      IDLE_DETAIL_FIELD_257, IDLE_DETAIL_FIELD_256, IDLE_DETAIL_FIELD_255,     \
      IDLE_DETAIL_FIELD_254, IDLE_DETAIL_FIELD_253, IDLE_DETAIL_FIELD_252,     \
      IDLE_DETAIL_FIELD_251, IDLE_DETAIL_FIELD_250, IDLE_DETAIL_FIELD_249,     \
      IDLE_DETAIL_FIELD_248, IDLE_DETAIL_FIELD_247, IDLE_DETAIL_FIELD_246,     \
      IDLE_DETAIL_FIELD_245, IDLE_DETAIL_FIELD_244, IDLE_DETAIL_FIELD_243,     \
      IDLE_DETAIL_FIELD_242, IDLE_DETAIL_FIELD_241, IDLE_DETAIL_FIELD_240,     \
      IDLE_DETAIL_FIELD_239, IDLE_DETAIL_FIELD_238, IDLE_DETAIL_FIELD_237,     \
      IDLE_DETAIL_FIELD_236, IDLE_DETAIL_FIELD_235, IDLE_DETAIL_FIELD_234,     \
      IDLE_DETAIL_FIELD_233, IDLE_DETAIL_FIELD_232, IDLE_DETAIL_FIELD_231,     \
      IDLE_DETAIL_FIELD_230, IDLE_DETAIL_FIELD_229, IDLE_DETAIL_FIELD_228,     \
      IDLE_DETAIL_FIELD_227, IDLE_DETAIL_FIELD_226, IDLE_DETAIL_FIELD_225,     \
      IDLE_DETAIL_FIELD_224, IDLE_DETAIL_FIELD_223, IDLE_DETAIL_FIELD_222,     \
      IDLE_DETAIL_FIELD_221, IDLE_DETAIL_FIELD_220, IDLE_DETAIL_FIELD_219,     \
      IDLE_DETAIL_FIELD_218, IDLE_DETAIL_FIELD_217, IDLE_DETAIL_FIELD_216,     \
      IDLE_DETAIL_FIELD_215, IDLE_DETAIL_FIELD_214, IDLE_DETAIL_FIELD_213,     \
      IDLE_DETAIL_FIELD_212, IDLE_DETAIL_FIELD_211, IDLE_DETAIL_FIELD_210,     \
      IDLE_DETAIL_FIELD_209, IDLE_DETAIL_FIELD_208, IDLE_DETAIL_FIELD_207,     \
      IDLE_DETAIL_FIELD_206, IDLE_DETAIL_FIELD_205, IDLE_DETAIL_FIELD_204,     \
      IDLE_DETAIL_FIELD_203, IDLE_DETAIL_FIELD_202, IDLE_DETAIL_FIELD_201,     \
      IDLE_DETAIL_FIELD_200, IDLE_DETAIL_FIELD_199, IDLE_DETAIL_FIELD_198,     \
      IDLE_DETAIL_FIELD_197, IDLE_DETAIL_FIELD_196, IDLE_DETAIL_FIELD_195,     \
      IDLE_DETAIL_FIELD_194, IDLE_DETAIL_FIELD_193, IDLE_DETAIL_FIELD_192,     \
      IDLE_DETAIL_FIELD_191, IDLE_DETAIL_FIELD_190, IDLE_DETAIL_FIELD_189,     \
      IDLE_DETAIL_FIELD_188, IDLE_DETAIL_FIELD_187, IDLE_DETAIL_FIELD_186,     \
      IDLE_DETAIL_FIELD_185, IDLE_DETAIL_FIELD_184, IDLE_DETAIL_FIELD_183,     \
      IDLE_DETAIL_FIELD_182, IDLE_DETAIL_FIELD_181, IDLE_DETAIL_FIELD_180,     \
      IDLE_DETAIL_FIELD_179, IDLE_DETAIL_FIELD_178, IDLE_DETAIL_FIELD_177,     \
      IDLE_DETAIL_FIELD_176, IDLE_DETAIL_FIELD_175, IDLE_DETAIL_FIELD_174,     \
      IDLE_DETAIL_FIELD_173, IDLE_DETAIL_FIELD_172, IDLE_DETAIL_FIELD_171,     \
      IDLE_DETAIL_FIELD_170, IDLE_DETAIL_FIELD_169, IDLE_DETAIL_FIELD_168,     \
      IDLE_DETAIL_FIELD_167, IDLE_DETAIL_FIELD_166, IDLE_DETAIL_FIELD_165,     \
      IDLE_DETAIL_FIELD_164, IDLE_DETAIL_FIELD_163, IDLE_DETAIL_FIELD_162,     \
      IDLE_DETAIL_FIELD_161, IDLE_DETAIL_FIELD_160, IDLE_DETAIL_FIELD_159,     \
      IDLE_DETAIL_FIELD_158, IDLE_DETAIL_FIELD_157, IDLE_DETAIL_FIELD_156,     \
      IDLE_DETAIL_FIELD_155, IDLE_DETAIL_FIELD_154, IDLE_DETAIL_FIELD_153,     \
      IDLE_DETAIL_FIELD_152, IDLE_DETAIL_FIELD_151, IDLE_DETAIL_FIELD_150,     \
      IDLE_DETAIL_FIELD_149, IDLE_DETAIL_FIELD_148, IDLE_DETAIL_FIELD_147,     \
      IDLE_DETAIL_FIELD_146, IDLE_DETAIL_FIELD_145, IDLE_DETAIL_FIELD_144,     \
      IDLE_DETAIL_FIELD_143, IDLE_DETAIL_FIELD_142, IDLE_DETAIL_FIELD_141,     \
      IDLE_DETAIL_FIELD_140, IDLE_DETAIL_FIELD_139, IDLE_DETAIL_FIELD_138,     \
      IDLE_DETAIL_FIELD_137, IDLE_DETAIL_FIELD_136, IDLE_DETAIL_FIELD_135,     \
      IDLE_DETAIL_FIELD_134, IDLE_DETAIL_FIELD_133, IDLE_DETAIL_FIELD_132,     \
      IDLE_DETAIL_FIELD_131, IDLE_DETAIL_FIELD_130, IDLE_DETAIL_FIELD_129,     \
      IDLE_DETAIL_FIELD_128, IDLE_DETAIL_FIELD_127, IDLE_DETAIL_FIELD_126,     \
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
