/*    (c) 2009-2011 Jan Dlabal <dlabaljan@gmail.com>                          */
/*                                                                            */
/*     This file is part of HC (HoubySoft Calculator).                        */
/*                                                                            */
/*     HC (HoubySoft Calculator) is free software: you can redistribute it    */
/*     and/or modify it under the terms of the GNU General Public License as  */
/*     published by the Free Software Foundation, either version 3 of the     */
/*     License, or any later version.                                         */
/*                                                                            */
/*     HC (HoubySoft Calculator) is distributed in the hope that it will be   */
/*     useful, but WITHOUT ANY WARRANTY; without even the implied warranty of */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          */
/*     GNU General Public License for more details.                           */
/*                                                                            */
/*     You should have received a copy of the GNU General Public License      */
/*     along with HC (HoubySoft Calculator). If not, see                      */
/*     <http://www.gnu.org/licenses/>.                                        */


#ifndef HASH_H
#define HASH_H

#define HASH_ANS 96742
#define HASH_NCR 108893
#define HASH_NPR 109296
#define HASH_ABS 96370
#define HASH_EXP 100893
#define HASH_FACTORIAL 37459
#define HASH_LN 3458
#define HASH_LOG10 130435
#define HASH_ASIN 64258
#define HASH_ARCSIN 51264
#define HASH_ACOS 49073
#define HASH_ARCCOS 36079
#define HASH_ATAN 64971
#define HASH_ARCTAN 51977
#define HASH_SIN 113880
#define HASH_COS 98695
#define HASH_TAN 114593
#define HASH_SINH 31159
#define HASH_COSH 120300
#define HASH_TANH 53262
#define HASH_SQRT 38983
#define HASH_CBRT 107788
#define HASH_MOD 108290
#define HASH_SUM 114251
#define HASH_PRODUCT 15925
#define HASH_GCD 102152
#define HASH_LCM 106966
#define HASH_GRAPH 77454
#define HASH_GMUL 97639
#define HASH_GRAPH3 21652
#define HASH_SLPFLD 22724
#define HASH_GRAPHPEQ 76031
#define HASH_STATS 21903
#define HASH_BOXPLOT 85922
#define HASH_FLOOR 108372
#define HASH_CEIL 110384
#define HASH_ROUND 88198
#define HASH_FIBO 63418
#define HASH_TOTIENT 24463
#define HASH_RAND 133645
#define HASH_PRINT 138610
#define HASH_WRITE 24885
#define HASH_INPUT 317
#define HASH_CONVERT 81061
#define HASH_CM 3178
#define HASH_INCH 17651
#define HASH_C 99
#define HASH_F 102
#define HASH_K 107
#define HASH_FLOZ 66715
#define HASH_ML 3487
#define HASH_FT 3278
#define HASH_M 109
#define HASH_KM 3426
#define HASH_MI 3484
#define HASH_CTOF 124979
#define HASH_FTOC 74380
#define HASH_CTOK 124984
#define HASH_KTOC 83366
#define HASH_FTOK 74388
#define HASH_KTOF 83369
#define HASH_MITOKM 6420
#define HASH_KMTOMI 50387
#define HASH_MLTOFLOZ 101498
#define HASH_FLOZTOML 26317
#define HASH_INCHTOCM 45445
#define HASH_CMTOINCH 83526
#define HASH_LBSTOKG 71739
#define HASH_KGTOLBS 557
#define HASH_FTTOM 71562
#define HASH_MTOFT 92558
#define HASH_MMASS 10938
#define HASH_ATOMICNUMBER 48118
#define HASH_CROSSP 142
#define HASH_DOTP 10161
#define HASH_STR 114225
#define HASH_NUM 109446
#define HASH_IF 3357
#define HASH_FOR 101577
#define HASH_WHILE 6665
#define HASH_RPN 113136
#define HASH_PRECISION 16713
#define HASH_P 112
#define HASH_SCI 113689
#define HASH_ENG 100574
#define HASH_NORMAL 102415
#define HASH_EXPF 48467
#define HASH_ANGLE 21563
#define HASH_DEG 99334
#define HASH_RAD 112661
#define HASH_GRAD 101816
#define HASH_KEYS 69277
#define HASH_CLEAR 127145
#define HASH_PLDEV 94751
#define HASH_BPN 97760
#define HASH_3DPOINTS 65383
#define HASH_LOAD 107919
#define HASH_TSTEP 93610
#define HASH_XMIN2D 1221
#define HASH_XMAX2D 52441
#define HASH_YMIN2D 76696
#define HASH_YMAX2D 127916
#define HASH_XMIN3D 1252
#define HASH_XMAX3D 52472
#define HASH_YMIN3D 76727
#define HASH_YMAX3D 127947
#define HASH_ZMIN3D 12233
#define HASH_ZMAX3D 63453
#define HASH_XMINSF 3238
#define HASH_XMAXSF 54458
#define HASH_YMINSF 78713
#define HASH_YMAXSF 129933
#define HASH_TMINPEQ 91763
#define HASH_TMAXPEQ 139924
#define HASH_XMINPEQ 54209
#define HASH_XMAXPEQ 102370
#define HASH_YMINPEQ 14461
#define HASH_YMAXPEQ 62622
#define HASH_PEQSTEP 68033
#define HASH_JOIN 48595
#define HASH_LENGTH 109802
#define HASH_RE 3635
#define HASH_REAL 137094
#define HASH_IM 3364
#define HASH_IMAG 16627
#define HASH_CHARTOCODE 101699
#define HASH_CODETOCHAR 51450
#define HASH_HELP 119467
#define HASH_ANSWER 56847
#define HASH_E 101
#define HASH_G 103
#define HASH_PHI 110961
#define HASH_PI 3577
#define HASH_CONVERSIONS 89347
#define HASH_LOGIC 132222
#define HASH_COMPLEX 104874
#define HASH_DIRECTIVES 91009
#define HASH_OPERATORS 17064
#define HASH_ZEROS 37200
#define HASH_ONES 56501
#define HASH_NA 3507
#define HASH_SORT 37061
#define HASH_MAP 107868
#define HASH_RANGE 84088
#define HASH_LAMBDA 19813
#define HASH_GRAPHVALUES 88423
#define HASH_GRAPHV 21719
#define HASH_GRAPHPOINTS 11991
#define HASH_GRAPHP 21713
#define HASH_LINREG 34828
#define HASH_COUNT 92330

#endif
