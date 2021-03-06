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


#include <m_apm.h>
#include "hc.h"
#include "hc_chemistry.h"

#ifndef HC_FUNCTIONS_H
#define HC_FUNCTIONS_H

extern int hc_binomc(M_APM, char *);
extern int hc_permutations(M_APM, char *);
extern int hc_sum(M_APM, M_APM, char *);
extern int hc_product(M_APM, M_APM, char *);
extern int hc_gamma(M_APM, M_APM, char *);
extern int hc_modulus(M_APM, char *);
extern int hc_gcd(M_APM, char *);
extern int hc_lcm(M_APM, char *);
extern int hc_fibo(M_APM, char *);
extern int hc_totient(M_APM, char *);
extern int hc_f2c(M_APM, char *);
extern int hc_c2f(M_APM, char *);
extern int hc_k2c(M_APM, char *);
extern int hc_c2k(M_APM, char *);
extern int hc_k2f(M_APM, char *);
extern int hc_f2k(M_APM, char *);
extern int hc_mi2km(M_APM, char *);
extern int hc_km2mi(M_APM, char *);
extern int hc_ml2floz(M_APM, char *);
extern int hc_floz2ml(M_APM, char *);
extern int hc_inch2cm(M_APM, char *);
extern int hc_cm2inch(M_APM, char *);
extern int hc_ft2m(M_APM, char *);
extern int hc_m2ft(M_APM, char *);
extern int hc_lbs2kg(M_APM, char *);
extern int hc_kg2lbs(M_APM, char *);
extern int hc_char2code(M_APM, char *);
extern char *hc_code2char(char *);
extern int hc_rand(M_APM, char *);
extern int hc_input(M_APM, M_APM, char *);
extern int hc_log(M_APM, M_APM, char *);
#define PRINT 0
#define WRITE 1
extern int hc_output(int, char *);
extern int hc_mmass(M_APM, char *);
extern int hc_atomicnumber(M_APM result, char *e);
extern int hc_dotp(M_APM, M_APM, char *);
extern char *hc_crossp(char *);
extern char *hc_join(char *);
extern char *hc_sort(char *);
extern int hc_length_core(M_APM, unsigned int *, char, char *);
extern int hc_length(M_APM, char *);
extern int hc_count(M_APM, char *);
extern char *hc_constantlist(char *e, char *el);
extern char *hc_range(char *e);

extern void hc_from_rad(M_APM);
extern void hc_to_rad(M_APM);

extern char *strreplace(char *in, char *old, char *new);
extern char *strreplace_(char *in, char *old, char *new); // same as strreplace, but does not include any measures against replacing inside variables etc. (ie. raw string replace)
extern char *hc_get_arg(char *e, int pos);
extern char *hc_get_arg_r(char *e, int pos); // same as hc_get_arg() but also automatically computes result
extern int hc_need_space_int(int i);
extern void is_zero_free();
extern char contains_char(char *);

// Statistics functions
extern char hc_stats(char *e, char g);
extern char *hc_linreg(char *e);

extern char *hc_2sci(char *);
extern char *hc_2eng(char *);
extern char *hc_strip_0s(char *e);
extern char *hc_strip_0s_real(char *e);
extern char *hc_format_num(char *r, char save_as_last_result);
extern char *hc_format_vec(char *r);

extern char *hc_2str(char *);
extern int hc_2num(M_APM, M_APM, char *);
extern int hc_2num_nr(M_APM, M_APM, char *);

extern char *hc_2dec(char base, char *in);

extern char *hc_result_mul(char *e);

extern char *hc_strrepr(struct hc_stack_element *el);

extern inline void hc_round(M_APM, int, M_APM);

extern char *str_multiply(char *, M_APM);
extern char *strip_spaces(char *e);
extern char last_char(char *e);
extern char check_completeness(char *e);
extern char *get_string(char *);
extern char *strchr_outofblock(char *, char);
extern char hc_get_type(char *e);

extern char hc_reim(char part, M_APM re, M_APM im, char *e);

extern char *hc_tobin(char *);
extern char *hc_tohex(char *);

extern char *gamma_coefficients;

#endif
