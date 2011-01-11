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


#ifndef HC_CHEMISTRY
#define HC_CHEMISTRY

#define ATOMIC_WEIGHT 1

#define EL_H 104
#define EL_HE 3325
#define EL_LI 3453
#define EL_BE 3139
#define EL_B 98
#define EL_C 99
#define EL_N 110
#define EL_O 111
#define EL_F 102
#define EL_NE 3511
#define EL_NA 3507
#define EL_MG 3482
#define EL_AL 3115
#define EL_SI 3670
#define EL_P 112
#define EL_S 115
#define EL_CL 3177
#define EL_AR 3121
#define EL_K 107
#define EL_CA 3166
#define EL_SC 3664
#define EL_TI 3701
#define EL_V 118
#define EL_CR 3183
#define EL_MN 3489
#define EL_FE 3263
#define EL_CO 3180
#define EL_NI 3515
#define EL_CU 3186
#define EL_ZN 3892
#define EL_GA 3290
#define EL_GE 3294
#define EL_AS 3122
#define EL_SE 3666
#define EL_BR 3152
#define EL_KR 3431
#define EL_RB 3632
#define EL_SR 3679
#define EL_Y 121
#define EL_ZR 3896
#define EL_NB 3508
#define EL_MO 3490
#define EL_TC 3695
#define EL_RU 3651
#define EL_RH 3638
#define EL_PD 3572
#define EL_AG 3110
#define EL_CD 3169
#define EL_IN 3365
#define EL_SN 3675
#define EL_SB 3663
#define EL_TE 3697
#define EL_I 105
#define EL_XE 3821
#define EL_CS 3184
#define EL_BA 3135
#define EL_LA 3445
#define EL_CE 3170
#define EL_PR 3586
#define EL_ND 3510
#define EL_PM 3581
#define EL_SM 3674
#define EL_EU 3248
#define EL_GD 3293
#define EL_TB 3694
#define EL_DY 3221
#define EL_HO 3335
#define EL_ER 3245
#define EL_TM 3705
#define EL_YB 3849
#define EL_LU 3465
#define EL_HF 3326
#define EL_TA 3693
#define EL_W 119
#define EL_RE 3635
#define EL_OS 3556
#define EL_IR 3369
#define EL_PT 3588
#define EL_AU 3124
#define EL_HG 3327
#define EL_TL 3704
#define EL_PB 3570
#define EL_BI 3143
#define EL_PO 3583
#define EL_AT 3123
#define EL_RN 3644
#define EL_FR 3276
#define EL_RA 3631
#define EL_AC 3106
#define EL_TH 3700
#define EL_PA 3569
#define EL_U 117
#define EL_NP 3522
#define EL_PU 3589
#define EL_AM 3116
#define EL_CM 3178
#define EL_BK 3145
#define EL_CF 3171
#define EL_ES 3246
#define EL_FM 3271
#define EL_MD 3479
#define EL_NO 3521
#define EL_LR 3462
#define EL_RF 3636
#define EL_DB 3198
#define EL_SG 3668
#define EL_BH 3142
#define EL_HS 3339
#define EL_MT 3495
#define EL_DS 3215
#define EL_RG 3637

extern double hc_get_element_info(char *el, int info);
extern double hc_mmass_el(char *e, int *i);
extern double hc_mmass_par(char *e, int *i);

#endif
