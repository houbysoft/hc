/*
   This file is part of MAPMX :  a package of mathematical routines
   built on the top of the multiple precision arithmetic library called MAPM
   - version 1.0
   Copyright (C)  2003    Ivano Primi ( ivano.primi@tin.it )
    
   This package is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This software package is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this package; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
   You can contact the author of this software by paper mail writing to
   the next address
	Ivano Primi
	via Colle Cannetacce 50/A
	C.A.P. 00038 - Valmontone (ROMA)
	Italy                                                          .

   If you prefer the electronic mail you can write to the address
	ivano.primi@tin.it        				       .
*/


#ifndef _M_APMC_H_
#define _M_APMC_H_

#include<m_apm.h>

/* This routine must be used to initialize the library */
void m_apmc_init(void);
/* This one just performs clean up operations */
void m_apmc_done(void);

void m_apmc_add (M_APM x, M_APM y, M_APM a, M_APM b, M_APM c, M_APM d);
void m_apmc_sub (M_APM x, M_APM y, M_APM a, M_APM b, M_APM c, M_APM d);
void m_apmc_mul (M_APM x, M_APM y, M_APM a, M_APM b, M_APM c, M_APM d);
int m_apmc_div (M_APM x, M_APM y, int dp, M_APM a, M_APM b, M_APM c, M_APM d);
int m_apmc_gdiv (M_APM x, M_APM y, int dp, M_APM a, M_APM b, M_APM c, M_APM d);
int m_apmc_gmod (M_APM x, M_APM y, int dp, M_APM a, M_APM b, M_APM c, M_APM d);
int m_apmc_idiv (M_APM x, M_APM y, int dp, M_APM a, M_APM b, M_APM c, M_APM d);
int m_apmc_mod (M_APM x, M_APM y, int dp, M_APM a, M_APM b, M_APM c, M_APM d);
int m_apmc_ipow (M_APM x, M_APM y, int dp, M_APM a, M_APM b, int n);
int m_apmc_root (M_APM x, M_APM y, int dp, M_APM a, M_APM b, int i, int n);
int m_apmc_pow (M_APM x, M_APM y, int dp, M_APM a, M_APM b, M_APM c, M_APM d);
void m_apmc_sqr (M_APM x, M_APM y, M_APM a, M_APM b);
void m_apmc_sqrt (M_APM x, M_APM y, int dp, M_APM a, M_APM b);

int m_apmc_is0 (M_APM a, M_APM b);
int m_apmc_not0 (M_APM a, M_APM b);
int m_apmc_eq (M_APM a, M_APM b, M_APM c, M_APM d);
int m_apmc_neq (M_APM a, M_APM b, M_APM c, M_APM d);
int m_apmc_gt (M_APM a, M_APM b, M_APM c, M_APM d);
int m_apmc_ge (M_APM a, M_APM b, M_APM c, M_APM d);
int m_apmc_lt (M_APM a, M_APM b, M_APM c, M_APM d);
int m_apmc_le (M_APM a, M_APM b, M_APM c, M_APM d);

void m_apmc_abs (M_APM r, int dp, M_APM a, M_APM b);
int  m_apmc_arg (M_APM arg, int dp, M_APM a, M_APM b);
int  m_apmc_reciprocal (M_APM x, M_APM y, int dp, M_APM a, M_APM b);

/* Exponential and logarithmic functions */
void m_apmc_exp (M_APM x, M_APM y, int dp, M_APM a, M_APM b);
int  m_apmc_log (M_APM x, M_APM y, int dp, M_APM a, M_APM b);
void m_apmc_exp10 (M_APM x, M_APM y, int dp, M_APM a, M_APM b);
int  m_apmc_log10 (M_APM x, M_APM y, int dp, M_APM a, M_APM b);
void m_apmc_exp2 (M_APM x, M_APM y, int dp, M_APM a, M_APM b);
int  m_apmc_log2 (M_APM x, M_APM y, int dp, M_APM a, M_APM b);
/* Trigonometric and inverse trigonometric functions */
void m_apmc_sin (M_APM x, M_APM y, int dp, M_APM a, M_APM b);
void m_apmc_cos (M_APM x, M_APM y, int dp, M_APM a, M_APM b);
int  m_apmc_tan (M_APM x, M_APM y, int dp, M_APM a, M_APM b);
void  m_apmc_arcsin (M_APM x, M_APM y, int dp, M_APM a, M_APM b);
void  m_apmc_arccos (M_APM x, M_APM y, int dp, M_APM a, M_APM b);
int  m_apmc_arctan (M_APM x, M_APM y, int dp, M_APM a, M_APM b);
/* Hyperbolic and inverse hyperbolic functions */
void m_apmc_sinh (M_APM x, M_APM y, int dp, M_APM a, M_APM b);
void m_apmc_cosh (M_APM x, M_APM y, int dp, M_APM a, M_APM b);
int  m_apmc_tanh (M_APM x, M_APM y, int dp, M_APM a, M_APM b);
void  m_apmc_arcsinh (M_APM x, M_APM y, int dp, M_APM a, M_APM b);
void  m_apmc_arccosh (M_APM x, M_APM y, int dp, M_APM a, M_APM b);
int  m_apmc_arctanh (M_APM x, M_APM y, int dp, M_APM a, M_APM b);
/* Others functions */
void m_apmc_floor (M_APM x, M_APM y, M_APM a, M_APM b);
void m_apmc_ceil (M_APM x, M_APM y, M_APM a, M_APM b);
void m_apmc_round (M_APM x, M_APM y, M_APM a, M_APM b);
void m_apmc_fix (M_APM x, M_APM y, M_APM a, M_APM b);
void m_apmc_frac (M_APM x, M_APM y, M_APM a, M_APM b);

/* A function useful for debugging purposes */
void m_apmc_4debug (const char* s, M_APM x, M_APM y, int dp);

/* Some aliases */
#define m_apmc_subtract    m_apmc_sub
#define m_apmc_multiply    m_apmc_mul
#define m_apmc_divide      m_apmc_div
#define m_apmc_integer_pow m_apmc_ipow
#define m_apmc_asin        m_apmc_arcsin
#define m_apmc_acos        m_apmc_arccos
#define m_apmc_atan        m_apmc_arctan
#define m_apmc_asinh       m_apmc_arcsinh
#define m_apmc_acosh       m_apmc_arccosh
#define m_apmc_atanh       m_apmc_arctanh

#endif /* _M_APMC_H_ */
