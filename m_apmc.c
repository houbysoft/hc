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


#include"m_apmc.h" /* Automatically includes m_apm.h */

static M_APM _op1_;
static M_APM _op2_;
static M_APM _re1_;
static M_APM _im1_;
static M_APM _re2_;
static M_APM _im2_;
static M_APM _arg_;
static M_APM _re_;
static M_APM _im_;
static M_APM _res_;
static M_APM _num_;
static M_APM _val_;

static M_APM _half_;
static M_APM _mhalf_;

static M_APM __a__;
static M_APM __b__;
static M_APM __c__;
static M_APM __d__;
static M_APM __u__;
static M_APM __v__;
static M_APM __x__;
static M_APM __y__;



/* This routine must be used to initialize the library */
void m_apmc_init(void)
{
  _op1_ = m_apm_init();
  _op2_ = m_apm_init();
  _re1_ = m_apm_init();
  _re2_ = m_apm_init();
  _im1_ = m_apm_init();
  _im2_ = m_apm_init();
  _arg_ = m_apm_init();
  _re_  = m_apm_init();
  _im_  = m_apm_init();
  _res_ = m_apm_init();
  _num_ = m_apm_init();
  _val_ = m_apm_init();
  _half_= m_apm_init();
  _mhalf_= m_apm_init();
  m_apm_set_string (_half_, "0.5");
  m_apm_set_string (_mhalf_, "-0.5");
  __a__ = m_apm_init();
  __b__ = m_apm_init();
  __c__ = m_apm_init();
  __d__ = m_apm_init();
  __u__ = m_apm_init();
  __v__ = m_apm_init();
  __x__ = m_apm_init();
  __y__ = m_apm_init();
}

/* Just perform clean up operations */
void m_apmc_done(void)
{
  m_apm_free(_op1_);
  m_apm_free(_op2_);
  m_apm_free(_re1_);
  m_apm_free(_re2_);
  m_apm_free(_im1_);
  m_apm_free(_im2_);
  m_apm_free(_arg_);
  m_apm_free(_re_);
  m_apm_free(_im_);
  m_apm_free(_res_);
  m_apm_free(_num_);
  m_apm_free(_val_);
  m_apm_free(_half_);
  m_apm_free(_mhalf_);
  m_apm_free(__a__);
  m_apm_free(__b__);
  m_apm_free(__c__);
  m_apm_free(__d__);
  m_apm_free(__u__);
  m_apm_free(__v__);
  m_apm_free(__x__);
  m_apm_free(__y__);
}

/* Round x to the nearest integer. Half-way cases are always rounded */
/* away from  zero.                                                  */
static void m_apm_rnd (M_APM y, M_APM x)
{
  m_apm_integer_div_rem(_num_, _val_, x, MM_One);
  if ( m_apm_compare (_val_, _half_) >= 0 )
    m_apm_add (y, _num_, MM_One);
  else if ( m_apm_compare (_val_, _mhalf_) <= 0 )
    m_apm_subtract (y, _num_, MM_One);
  else
    m_apm_copy (y, _num_);
}

static int m_apm_length (M_APM x)
{
  int expn = m_apm_exponent (x);

  return ( expn <= 0 ? 0 : expn );
}

/* Sum the complex numbers a+ib and c+id */
void m_apmc_add (M_APM x, M_APM y, M_APM a, M_APM b, M_APM c, M_APM d)
{
  m_apm_add (x, a, c);
  m_apm_add (y, b, d);
}

/* Compute the difference (a+ib) - (c+id) */
void m_apmc_sub (M_APM x, M_APM y, M_APM a, M_APM b, M_APM c, M_APM d)
{
  m_apm_subtract (x, a, c);
  m_apm_subtract (y, b, d);
}

/* Compute the product (a+ib) * (c+id) */
void m_apmc_mul (M_APM x, M_APM y, M_APM a, M_APM b, M_APM c, M_APM d)
{
  m_apm_multiply (_op1_, a, c);
  m_apm_multiply (_op2_, b, d);
  m_apm_subtract (x, _op1_, _op2_);
  m_apm_multiply (_op1_, b, c);
  m_apm_multiply (_op2_, a, d);
  m_apm_add (y, _op1_, _op2_);
}

/* Compute (a+ib)/(c+id). Return -1 if c==d==0, else 0. */
int m_apmc_div (M_APM x, M_APM y, int dp, M_APM a, M_APM b, M_APM c, M_APM d)
{
  m_apm_multiply (_op1_, c, c);
  m_apm_multiply (_op2_, d, d);
  m_apm_add (_num_, _op1_, _op2_);
  if ( m_apm_sign (_num_) == 0 )
    return -1;
  else
    {
      m_apm_multiply (_op1_, a, c);
      m_apm_multiply (_op2_, b, d);
      m_apm_add (_re_, _op1_, _op2_);
      m_apm_multiply (_op1_, c, b);
      m_apm_multiply (_op2_, d, a);
      m_apm_subtract (_im_, _op1_, _op2_);
      m_apm_divide (x, dp, _re_, _num_);
      m_apm_divide (y, dp, _im_, _num_);
      return 0;
    }
}

/* Perform the gaussian division of a+ib by c+id. */
/* Return -1 if c==d==0, else 0.                  */
int m_apmc_gdiv (M_APM x, M_APM y, int dp, M_APM a, M_APM b, M_APM c, M_APM d)
{
  m_apm_rnd (_re1_, a);
  m_apm_rnd (_im1_, b);
  m_apm_rnd (_re2_, c);
  m_apm_rnd (_im2_, d);
  m_apm_multiply (_op1_, _re2_, _re2_);
  m_apm_multiply (_op2_, _im2_, _im2_);
  m_apm_add (_num_, _op1_, _op2_);
  if ( m_apm_sign (_num_) == 0 )
    return -1;
  else
    {
      m_apm_multiply (_op1_, _re1_, _re2_);
      m_apm_multiply (_op2_, _im1_, _im2_);
      m_apm_add (_re_, _op1_, _op2_);
      m_apm_multiply (_op1_, _re2_, _im1_);
      m_apm_multiply (_op2_, _im2_, _re1_);
      m_apm_subtract (_im_, _op1_, _op2_);
      m_apm_divide (_op1_, dp, _re_, _num_);
      m_apm_divide (_op2_, dp, _im_, _num_);
      m_apm_rnd (x, _op1_);
      m_apm_rnd (y, _op2_);
      return 0;
    }
}

int m_apmc_gmod (M_APM x, M_APM y, int dp, M_APM a, M_APM b, M_APM c, M_APM d)
{
  m_apm_rnd (_re1_, a);
  m_apm_rnd (_im1_, b);
  m_apm_rnd (_re2_, c);
  m_apm_rnd (_im2_, d);
  m_apm_multiply (_op1_, _re2_, _re2_);
  m_apm_multiply (_op2_, _im2_, _im2_);
  m_apm_add (_num_, _op1_, _op2_);
  if ( m_apm_sign (_num_) == 0 )
    return -1;
  else
    {
      m_apm_multiply (_op1_, _re1_, _re2_);
      m_apm_multiply (_op2_, _im1_, _im2_);
      m_apm_add (_re_, _op1_, _op2_);
      m_apm_multiply (_op1_, _re2_, _im1_);
      m_apm_multiply (_op2_, _im2_, _re1_);
      m_apm_subtract (_im_, _op1_, _op2_);
      m_apm_divide (_op1_, dp, _re_, _num_);
      m_apm_divide (_op2_, dp, _im_, _num_);
      m_apm_rnd (_re_, _op1_);
      m_apm_rnd (_im_, _op2_);
      /* Now _num_ can be reused */
      m_apm_multiply (_op1_, _re_, _re2_);
      m_apm_multiply (_op2_, _im_, _im2_);
      m_apm_subtract (_val_, _re1_, _op1_);
      m_apm_add (_num_, _val_, _op2_); 
      m_apm_rnd (x, _num_);
      m_apm_multiply (_op1_, _im_, _re2_);
      m_apm_multiply (_op2_, _re_, _im2_);
      m_apm_subtract (_val_, _im1_, _op1_);
      m_apm_subtract (_num_, _val_, _op2_);
      m_apm_rnd (y, _num_);
      return 0;
    }
}

int m_apmc_idiv (M_APM x, M_APM y, int dp, M_APM a, M_APM b, M_APM c, M_APM d)
{
  m_apm_rnd (_re1_, a);
  m_apm_rnd (_im1_, b);
  m_apm_rnd (_re2_, c);
  m_apm_rnd (_im2_, d);
  m_apm_multiply (_op1_, _re2_, _re2_);
  m_apm_multiply (_op2_, _im2_, _im2_);
  m_apm_add (_num_, _op1_, _op2_);
  if ( m_apm_sign (_num_) == 0 )
    return -1;
  else
    {
      m_apm_multiply (_op1_, _re1_, _re2_);
      m_apm_multiply (_op2_, _im1_, _im2_);
      m_apm_add (_re_, _op1_, _op2_);
      m_apm_multiply (_op1_, _re2_, _im1_);
      m_apm_multiply (_op2_, _im2_, _re1_);
      m_apm_subtract (_im_, _op1_, _op2_);
      m_apm_divide (_op1_, dp, _re_, _num_);
      m_apm_divide (_op2_, dp, _im_, _num_);
      m_apm_integer_div_rem(x, _val_, _op1_, MM_One);
      m_apm_integer_div_rem(y, _val_, _op2_, MM_One);
      return 0;
    }
}

int m_apmc_mod (M_APM x, M_APM y, int dp, M_APM a, M_APM b, M_APM c, M_APM d)
{
  m_apm_rnd (_re1_, a);
  m_apm_rnd (_im1_, b);
  m_apm_rnd (_re2_, c);
  m_apm_rnd (_im2_, d);
  m_apm_multiply (_op1_, _re2_, _re2_);
  m_apm_multiply (_op2_, _im2_, _im2_);
  m_apm_add (_num_, _op1_, _op2_);
  if ( m_apm_sign (_num_) == 0 )
    return -1;
  else
    {
      m_apm_multiply (_op1_, _re1_, _re2_);
      m_apm_multiply (_op2_, _im1_, _im2_);
      m_apm_add (_re_, _op1_, _op2_);
      m_apm_multiply (_op1_, _re2_, _im1_);
      m_apm_multiply (_op2_, _im2_, _re1_);
      m_apm_subtract (_im_, _op1_, _op2_);
      m_apm_divide (_op1_, dp, _re_, _num_);
      m_apm_divide (_op2_, dp, _im_, _num_);
      m_apm_integer_div_rem(_re_, _val_, _op1_, MM_One);
      m_apm_integer_div_rem(_im_, _val_, _op2_, MM_One);
      /* Now _num_ can be reused */
      m_apm_multiply (_op1_, _re_, _re2_);
      m_apm_multiply (_op2_, _im_, _im2_);
      m_apm_subtract (_val_, _re1_, _op1_);
      m_apm_add (_num_, _val_, _op2_); 
      m_apm_rnd (x, _num_);
      m_apm_multiply (_op1_, _im_, _re2_);
      m_apm_multiply (_op2_, _re_, _im2_);
      m_apm_subtract (_val_, _im1_, _op1_);
      m_apm_subtract (_num_, _val_, _op2_);
      m_apm_rnd (y, _num_);
      return 0;
    }
}

/* Compute (a+ib)^n  by using the De Morgan's formula. */
int m_apmc_ipow (M_APM x, M_APM y, int dp, M_APM a, M_APM b, int n)
{
  int newscale;

  m_apm_multiply (_op1_, a, a);
  m_apm_multiply (_op2_, b, b);
  m_apm_add (_val_, _op1_, _op2_);
  m_apm_sqrt (_num_, dp + 10, _val_);
  if ( m_apm_sign (_num_) == 0 )
    {
      if ( n <= 0 )
	return -1;
      else
	{
	  m_apm_copy (x, MM_Zero);
	  m_apm_copy (y, MM_Zero);
	  return 0;
	}
    }
  else
    {
      m_apm_integer_pow (_val_, dp, _num_, n);
      newscale = dp + m_apm_length (_val_) + 1;
      m_apm_arctan2 (_op1_, newscale, b, a);
      m_apm_set_long (_op2_, n);
      m_apm_multiply (_arg_, _op1_, _op2_);
      m_apm_cos (_op1_, newscale, _arg_);
      m_apm_sin (_op2_, newscale, _arg_);
      m_apm_multiply (x, _val_, _op1_);
      m_apm_multiply (y, _val_, _op2_);
      /* Must I add a rounding operation ? */
      return 0;
    }
}

/* Return the 'i'th branch of the 'n'th root of a+ib. If  n == 0  or */
/* a == b == 0  and  n < 0  then return -1, else return 0.           */
int m_apmc_root (M_APM x, M_APM y, int dp, M_APM a, M_APM b, int i, int n)
{
  int newscale;

  if ( n == 0 )
    return -1;
  else
    {
      m_apm_multiply (_op1_, a, a);
      m_apm_multiply (_op2_, b, b);
      m_apm_add (_val_, _op1_, _op2_);
      m_apm_sqrt (_num_, dp + 10, _val_);
    }
  if ( m_apm_sign (_num_) == 0 )
    {
      if ( n < 0 )
	return -1;
      else
	{
	  m_apm_copy (x, MM_Zero);
	  m_apm_copy (y, MM_Zero);
	  return 0;
	}
    }
  else
    {
      m_apm_set_long (_op1_, n);
      m_apm_reciprocal (_op2_, dp + 10, _op1_);
      /* Now _op2_ == 1/n */
      m_apm_pow (_val_, dp, _num_, _op2_);
      /* Now _val_ = |a+ib|^(1/n) */
      newscale = dp + m_apm_length (_val_) + 1;
      m_apm_arctan2 (_op1_, newscale, b, a);
      /* Now _num_ can be reused */
      i %= n;
      m_apm_set_long (_num_, i);
      m_apm_multiply (_res_, MM_2_PI, _num_);
      m_apm_add (_num_, _op1_, _res_);
      m_apm_multiply (_arg_, _num_, _op2_);
      m_apm_cos (_op1_, newscale, _arg_);
      m_apm_sin (_op2_, newscale, _arg_);
      m_apm_multiply (x, _val_, _op1_);
      m_apm_multiply (y, _val_, _op2_);
      /* Must I add a rounding operation ? */
      return 0;
    }
}

int m_apmc_pow (M_APM x, M_APM y, int dp, M_APM a, M_APM b, M_APM c, M_APM d)
{
  int newscale;

  m_apm_multiply (_op1_, a, a);
  m_apm_multiply (_op2_, b, b);
  m_apm_add (_num_, _op1_, _op2_);
  m_apm_sqrt (_val_, dp + 10, _num_);
  if ( m_apm_sign (_val_) == 0 )
    {
      if ( m_apm_sign (c) <= 0 )
	return -1;
      else
	{
	  m_apm_copy (x, MM_Zero);
	  m_apm_copy (y, MM_Zero);
	  return 0;
	}
    }
  else
    {
      m_apm_log (_num_, dp + 2, _val_);
      m_apm_atan2 (_arg_, dp + 2, b, a);
      /* Set up |x+iy| */
      m_apm_multiply (_op1_, c, _num_);
      m_apm_multiply (_op2_, d, _arg_);
      m_apm_subtract (_res_, _op1_, _op2_);
      m_apm_exp (_val_, dp + 2, _res_);
      /* *** */
      newscale = dp + m_apm_length (_val_) + 1;
      /* *** */
      m_apm_multiply (_op1_, a, a);
      m_apm_multiply (_op2_, b, b);
      m_apm_add (_num_, _op1_, _op2_);
      m_apm_sqrt (_res_, newscale, _num_);
      m_apm_log (_num_, newscale, _res_);
      m_apm_atan2 (_arg_, newscale, b, a);
      m_apm_multiply (_op1_, c, _arg_);
      m_apm_multiply (_op2_, d, _num_);
      m_apm_add (_res_, _op1_, _op2_);
      m_apm_cos (_op1_, newscale, _res_);
      m_apm_sin (_op2_, newscale, _res_);
      m_apm_multiply (x, _val_, _op1_);
      m_apm_multiply (y, _val_, _op2_);
      /* Must I add a rounding operation ? */
      return 0;
    }
}

void m_apmc_sqr (M_APM x, M_APM y, M_APM a, M_APM b)
{
  m_apm_multiply (_op1_, a, a);
  m_apm_multiply (_op2_, b, b);
  m_apm_subtract (x, _op1_, _op2_);
  m_apm_multiply (_val_, a, b);
  m_apm_multiply (y, _val_, MM_Two);
}

void m_apmc_sqrt (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  int newscale = 2 * dp;

  m_apm_multiply (_op1_, a, a);
  m_apm_multiply (_op2_, b, b);
  m_apm_add (_val_, _op1_, _op2_);
  m_apm_sqrt (_num_, newscale, _val_);
  /* Set up x */
  m_apm_add (_res_, _num_, a);
  m_apm_divide (_re_, newscale, _res_, MM_Two);
  m_apm_sqrt (x, dp, _re_);
  /* Set up y */
  m_apm_subtract (_res_, _num_, a);
  m_apm_divide (_im_, newscale, _res_, MM_Two);
  m_apm_sqrt (_val_, dp, _im_);
  if ( m_apm_sign (b) < 0 )
    m_apm_negate (y, _val_);
  else
    m_apm_copy (y, _val_);
}


int m_apmc_is0 (M_APM a, M_APM b)
{
  return ( m_apm_sign (a) == 0  &&  m_apm_sign (b) == 0 );
}

int m_apmc_not0 (M_APM a, M_APM b)
{
  return ( m_apm_sign (a) != 0  ||  m_apm_sign (b) != 0 );
}

int m_apmc_eq (M_APM a, M_APM b, M_APM c, M_APM d)
{
  return ( m_apm_compare (a, c) == 0  &&  m_apm_compare (b, d) == 0 );
}

int m_apmc_neq (M_APM a, M_APM b, M_APM c, M_APM d)
{
  return ( m_apm_compare (a, c) != 0  ||  m_apm_compare (b, d) != 0 );
}

int m_apmc_gt (M_APM a, M_APM b, M_APM c, M_APM d)
{
  if ( m_apm_compare (a, c) >= 0  &&  m_apm_compare (b, d) >= 0 )
    {
      if ( m_apm_compare (a, c) + m_apm_compare (b, d) > 0 )
	return 1;
      else
	return 0;
    }
  else
    return 0;
}

int m_apmc_ge (M_APM a, M_APM b, M_APM c, M_APM d)
{
  return ( m_apm_compare (a, c) >= 0  &&  m_apm_compare (b, d) >= 0 );
}

int m_apmc_lt (M_APM a, M_APM b, M_APM c, M_APM d)
{
  if ( m_apm_compare (a, c) <= 0  &&  m_apm_compare (b, d) <= 0 )
    {
      if ( m_apm_compare (a, c) + m_apm_compare (b, d) < 0 )
	return 1;
      else
	return 0;
    }
  else
    return 0;
}

int m_apmc_le (M_APM a, M_APM b, M_APM c, M_APM d)
{
  return ( m_apm_compare (a, c) <= 0  &&  m_apm_compare (b, d) <= 0 );
}


void m_apmc_abs (M_APM r, int dp, M_APM a, M_APM b)
{
  m_apm_multiply (_op1_, a, a);
  m_apm_multiply (_op2_, b, b);
  m_apm_add (_res_, _op1_, _op2_);
  m_apm_sqrt (r, dp, _res_);
}

int m_apmc_arg (M_APM arg, int dp, M_APM a, M_APM b)
{
  if ( m_apm_sign (a) == 0  &&  m_apm_sign (b) == 0 )
    return -1;
  else
    {
      m_apm_arctan2 (arg, dp + 15, b, a);
      return 0;
    }
}

int  m_apmc_reciprocal (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  m_apm_multiply (_op1_, a, a);
  m_apm_multiply (_op2_, b, b);
  m_apm_add (_val_, _op1_, _op2_);
  if ( m_apm_sign (_val_) == 0 )
    return -1;
  else
    {
      m_apm_divide (x, dp, a, _val_);
      m_apm_negate (_im_, b);
      m_apm_divide (y, dp, _im_, _val_);
      return 0;
    }
}

/* Exponential and logarithmic functions */
void m_apmc_exp (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  int newscale = dp + 3;

  m_apm_exp (_num_, newscale, a);
  m_apm_cos (_re_, newscale, b);
  m_apm_sin (_im_, newscale, b);
  m_apm_multiply (x, _num_, _re_);
  m_apm_multiply (y, _num_, _im_);
}


int  m_apmc_log (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  m_apm_multiply (_op1_, a, a);
  m_apm_multiply (_op2_, b, b);
  m_apm_add (_val_, _op1_, _op2_);
  m_apm_sqrt (_num_, dp + 10, _val_);
  if ( m_apm_sign (_num_) == 0 )
    return -1;
  else
    {
      m_apm_log (x, dp + 3, _num_);
      m_apm_arctan2 (y, dp + 15, b, a);
      return 0;
    }
}

void m_apmc_exp10 (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  int newscale = dp + 3;

  m_apm_pow (_num_, newscale, MM_Ten, a);
  m_apm_multiply (_arg_, MM_LOG_10_BASE_E, b);
  m_apm_cos (_re_, newscale, _arg_);
  m_apm_sin (_im_, newscale, _arg_);
  m_apm_multiply (x, _num_, _re_);
  m_apm_multiply (y, _num_, _im_);
}

int  m_apmc_log10 (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  int newscale = dp + 3;

  m_apm_multiply (_op1_, a, a);
  m_apm_multiply (_op2_, b, b);
  m_apm_add (_val_, _op1_, _op2_);
  m_apm_sqrt (_num_, dp + 10, _val_);
  if ( m_apm_sign (_num_) == 0 )
    return -1;
  else
    {
      m_apm_log10 (x, newscale, _num_);
      m_apm_arctan2 (_im_, newscale + 12, b, a);
      m_apm_divide (y, newscale, _im_, MM_LOG_10_BASE_E);
      return 0;
    }
}

void m_apmc_exp2 (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  int newscale = dp + 3;

  m_apm_pow (_num_, newscale, MM_Two, a);
  m_apm_multiply (_arg_, MM_LOG_2_BASE_E, b);
  m_apm_cos (_re_, newscale, _arg_);
  m_apm_sin (_im_, newscale, _arg_);
  m_apm_multiply (x, _num_, _re_);
  m_apm_multiply (y, _num_, _im_);
}

int  m_apmc_log2 (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  int newscale = dp + 3;

  m_apm_multiply (_op1_, a, a);
  m_apm_multiply (_op2_, b, b);
  m_apm_add (_val_, _op1_, _op2_);
  m_apm_sqrt (_num_, dp + 10, _val_);
  if ( m_apm_sign (_num_) == 0 )
    return -1;
  else
    {
      m_apm_log (_re_, newscale, _num_);
      m_apm_divide (x, newscale, _re_, MM_LOG_2_BASE_E);
      m_apm_arctan2 (_im_, newscale + 12, b, a);
      m_apm_divide (y, newscale, _im_, MM_LOG_2_BASE_E);
      return 0;
    }
}

/* Trigonometric and inverse trigonometric functions */
void m_apmc_sin (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  int newscale = dp + 3;

  /* Set up x */
  m_apm_sin  (_re_, newscale, a);
  m_apm_cosh (_im_, newscale, b);
  m_apm_multiply (x, _re_, _im_);
  /* Set up y */
  m_apm_cos  (_re_, newscale, a);
  m_apm_sinh (_im_, newscale, b);
  m_apm_multiply (y, _re_, _im_);
}

void m_apmc_cos (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  int newscale = dp + 3;

  /* Set up x */
  m_apm_cos  (_re_, newscale, a);
  m_apm_cosh (_im_, newscale, b);
  m_apm_multiply (x, _re_, _im_);
  /* Set up y */
  m_apm_sin  (_re_, newscale, a);
  m_apm_sinh (_im_, newscale, b);
  m_apm_multiply (_res_, _re_, _im_);
  m_apm_negate (y, _res_);
}

int  m_apmc_tan (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  m_apm_sin  (_re1_, 2 * dp + 10, a);
  m_apm_cos  (_re2_, 2 * dp + 10, a);
  m_apm_sinh (_im1_, 2 * dp + 10, b);
  m_apm_cosh (_im2_, 2 * dp + 10, b);
  m_apm_multiply (_op1_, _re2_, _re2_); /* _op1_ == cos (a)^2 */
  m_apm_multiply (_op2_, _im1_, _im1_); /* _op2_ == sinh(b)^2 */
  m_apm_add (_num_, _op1_, _op2_);
  if ( m_apm_exponent (_num_) < -2 * dp -2)
    return -1;
  else
    {
      m_apm_multiply (_op1_, _re1_, _re2_);
      m_apm_divide (x, dp, _op1_, _num_);
      m_apm_multiply (_op2_, _im1_, _im2_);
      m_apm_divide (y, dp, _op2_, _num_);
      return 0;
    }
}

void m_apmc_arcsin (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  int expn;

  m_apm_multiply (_op1_, a, a);
  m_apm_multiply (_op2_, b, b);
  m_apm_add (_val_, _op1_, _op2_);
  /* We need a rescaling */
  expn = m_apm_length (_val_) / 2;
  m_apmc_sqr  (__a__, __b__, a, b);
  m_apmc_sub  (__c__, __d__, MM_One, MM_Zero, __a__, __b__);
  m_apmc_sqrt (__u__, __v__, dp + 4 + expn, __c__, __d__);
  m_apmc_mul  (__a__, __b__, MM_Zero, MM_One, a, b);
  m_apmc_add  (__c__, __d__, __u__, __v__, __a__, __b__);
/*   m_apmc_4debug ("arcsin", __c__, __d__, dp + 4 + expn); // Only for debug */
  m_apmc_abs  (__x__, dp + 4 + expn, __c__, __d__);
  if ( (expn > 3) && (m_apm_exponent (__x__) < - 3  ||  m_apm_sign(__x__) == 0) )
    m_apmc_sub  (__c__, __d__, __a__, __b__, __u__, __v__);
  m_apmc_log  (__u__, __v__, dp, __c__, __d__);
  m_apmc_mul  (__x__, __y__, MM_Zero, MM_One, __u__, __v__);
  m_apm_negate (x, __x__);
  m_apm_negate (y, __y__);
}

void m_apmc_arccos (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  int expn;

  m_apm_multiply (_op1_, a, a);
  m_apm_multiply (_op2_, b, b);
  m_apm_add (_val_, _op1_, _op2_);
  /* We need a rescaling */
  expn = m_apm_length (_val_) / 2;
  m_apmc_sqr  (__a__, __b__, a, b);
  m_apmc_sub  (__c__, __d__, MM_One, MM_Zero, __a__, __b__);
  m_apmc_sqrt (__u__, __v__, dp + 4 + expn, __c__, __d__);
  m_apmc_mul  (__c__, __d__, MM_Zero, MM_One, __u__, __v__);
  m_apmc_add  (__a__, __b__, __c__, __d__, a, b);
/*   m_apmc_4debug ("arccos", __a__, __b__, dp + 4 + expn); // Only for debug */
  m_apmc_abs  (__x__, dp + 4 + expn, __a__, __b__);
  if ( (expn > 3) && (m_apm_exponent (__x__) < - 3  ||  m_apm_sign(__x__) == 0) )
    m_apmc_sub  (__a__, __b__, a, b, __c__, __d__);
  m_apmc_log  (__u__, __v__, dp, __a__, __b__);
  m_apmc_mul  (__x__, __y__, MM_Zero, MM_One, __u__, __v__);
  m_apm_negate (x, __x__);
  m_apm_negate (y, __y__);
}

int m_apmc_arctan (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  m_apmc_mul (__a__, __b__, MM_Zero, MM_One, a, b);
  m_apmc_add (__c__, __d__, MM_One, MM_Zero, __a__, __b__);
  m_apmc_sub (__u__, __v__, MM_One, MM_Zero, __a__, __b__);
  if ( m_apmc_div (__a__, __b__, dp, __c__, __d__, __u__, __v__) == -1 )
    return -1;
  else
    {
      m_apmc_sqrt (__c__, __d__, dp, __a__, __b__);
      if ( m_apmc_log (__u__, __v__, dp, __c__, __d__) == -1 )
	return -1;
      else
	{
	  m_apmc_mul (__x__, __y__, MM_Zero, MM_One, __u__, __v__);
	  m_apm_negate (x, __x__);
	  m_apm_negate (y, __y__);
	  return 0;
	}
    }
}

/* Hyperbolic and inverse hyperbolic functions */
void m_apmc_sinh (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  int newscale = dp + 3;

  m_apm_sinh (_re1_, newscale, a);
  m_apm_cos  (_im1_, newscale, b);
  m_apm_multiply (x, _re1_, _im1_);
  m_apm_cosh (_re2_, newscale, a);
  m_apm_sin  (_im2_, newscale, b);  
  m_apm_multiply (y, _re2_, _im2_);
}

void m_apmc_cosh (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  int newscale = dp + 3;

  m_apm_cosh (_re1_, newscale, a);
  m_apm_cos  (_im1_, newscale, b);
  m_apm_multiply (x, _re1_, _im1_);
  m_apm_sinh (_re2_, newscale, a);
  m_apm_sin  (_im2_, newscale, b);  
  m_apm_multiply (y, _re2_, _im2_);
}

int  m_apmc_tanh (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  m_apm_sinh (_re1_, 2 * dp + 10, a);
  m_apm_cosh (_re2_, 2 * dp + 10, a);
  m_apm_sin  (_im1_, 2 * dp + 10, b);
  m_apm_cos  (_im2_, 2 * dp + 10, b);
  m_apm_multiply (_op1_, _re2_, _re2_); /* _op1_ == cosh(a)^2 */
  m_apm_multiply (_op2_, _im1_, _im1_); /* _op2_ == sin (b)^2  */
  m_apm_subtract (_num_, _op1_, _op2_);
  /* Now _op1_ and _op2_ can be overwritten */
  if ( m_apm_exponent (_num_) < -2 * dp -2)
    return -1;
  else
    {
      m_apm_multiply (_op1_, _re1_, _re2_);
      m_apm_divide (x, dp, _op1_, _num_);
      m_apm_multiply (_op2_, _im1_, _im2_);
      m_apm_divide (y, dp, _op2_, _num_);
      return 0;
    }
}

void m_apmc_arcsinh (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  int expn;

  /* Ok, the addition of this test looks quite ugly */
  /* but it is necessary.  :'(      (Sigh sigh !)   */
  if ( m_apm_sign (b) == 0 )
    {
      m_apm_copy (y, MM_Zero);
      m_apm_arcsinh (x, dp, a);
/*       m_apm_absolute_value (_re_, a); */
/*       /\* m_apm_arcsinh() is not well implemented *\/ */
/*       /\* right now (MAPM 4.7).                   *\/ */
/*       /\* I am waiting for a patch from Mike.     *\/ */
/*       m_apm_arcsinh (_val_, dp, _re_); */
/*       if ( m_apm_sign (a) < 0 ) */
/* 	m_apm_negate (x, _val_); */
/*       else */
/* 	m_apm_copy (x, _val_); */
    }
  else
    {
      m_apm_multiply (_op1_, a, a);
      m_apm_multiply (_op2_, b, b);
      m_apm_add (_val_, _op1_, _op2_);
      /* We need a rescaling */
      expn = m_apm_length (_val_) / 2;
      m_apmc_sqr  (__a__, __b__, a, b);
      m_apmc_add  (__c__, __d__, MM_One, MM_Zero, __a__, __b__);
      m_apmc_sqrt (__u__, __v__, dp + 4 + expn, __c__, __d__);
      m_apmc_add  (__a__, __b__, a, b, __u__, __v__);
      /*   m_apmc_4debug ("arcsinh", __a__, __b__, dp + 4 + expn); // Only for debug */
      m_apmc_abs  (__x__, dp + 4 + expn, __a__, __b__);
      if ( (expn > 3) && (m_apm_exponent (__x__) < - 3  ||  m_apm_sign(__x__) == 0) )
	m_apmc_sub  (__a__, __b__, a, b, __u__, __v__);
      m_apmc_log  (x, y, dp, __a__, __b__);
    }
}

void m_apmc_arccosh (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  int expn;

  m_apm_multiply (_op1_, a, a);
  m_apm_multiply (_op2_, b, b);
  m_apm_add (_val_, _op1_, _op2_);
  /* We need a rescaling */
  expn = m_apm_length (_val_) / 2;
  m_apmc_sqr  (__a__, __b__, a, b);
  m_apmc_sub  (__c__, __d__, __a__, __b__, MM_One, MM_Zero);
  m_apmc_sqrt (__u__, __v__, dp + 4 + expn, __c__, __d__);
  m_apmc_add  (__a__, __b__, a, b, __u__, __v__);
/*   m_apmc_4debug ("arccosh", __a__, __b__, dp + 4 + expn); // Only for debug */
  m_apmc_abs  (__x__, dp + 4 + expn, __a__, __b__);
  if ( (expn > 3) && (m_apm_exponent (__x__) < - 3  ||  m_apm_sign(__x__) == 0) )
    m_apmc_sub  (__a__, __b__, a, b, __u__, __v__);
  m_apmc_log  (x, y, dp, __a__, __b__);
}

int  m_apmc_arctanh (M_APM x, M_APM y, int dp, M_APM a, M_APM b)
{
  m_apmc_add (__a__, __b__, MM_One, MM_Zero, a, b);
  m_apmc_sub (__c__, __d__, MM_One, MM_Zero, a, b);
  if ( m_apmc_div (__u__, __v__, dp, __a__, __b__, __c__, __d__) == -1 )
    return -1;
  else
    {
      m_apmc_sqrt (__a__, __b__, dp, __u__, __v__);
      if ( m_apmc_log (x, y, dp, __a__, __b__) == -1 )
	return -1;
      else
	return 0;
    }
}

/* Others functions */
void m_apmc_floor (M_APM x, M_APM y, M_APM a, M_APM b)
{
  m_apm_floor (x, a);
  m_apm_floor (y, b);
}

void m_apmc_ceil (M_APM x, M_APM y, M_APM a, M_APM b)
{
  m_apm_ceil (x, a);
  m_apm_ceil (y, b);
}

void m_apmc_round (M_APM x, M_APM y, M_APM a, M_APM b)
{
  m_apm_rnd (x, a);
  m_apm_rnd (y, b);
}

void m_apmc_fix (M_APM x, M_APM y, M_APM a, M_APM b)
{
  m_apm_integer_div_rem (x, _val_, a, MM_One);
  m_apm_integer_div_rem (y, _val_, b, MM_One);
}

void m_apmc_frac (M_APM x, M_APM y, M_APM a, M_APM b)
{
  m_apm_integer_div_rem (_val_, x, a, MM_One);
  m_apm_integer_div_rem (_val_, y, b, MM_One);
}

#include<stdio.h>
#include<stdlib.h>

void m_apmc_4debug (const char* s, M_APM x, M_APM y, int dp)
{
  char *re, *im;

  m_apm_round (_re_, dp, x);
  m_apm_round (_im_, dp, y);
  re = m_apm_to_fixpt_stringexp(-1, _re_, '.', '\0', 0);
  im = m_apm_to_fixpt_stringexp(-1, _im_, '.', '\0', 0);
  if ( !s )
    fprintf (stderr, "\n### (%s, %s)\n\n", re, im);
  else
    fprintf (stderr, "\n### %s: (%s, %s)\n\n", s, re, im);
  if ( (re) )
    free ((void*)re);
  if ( (im) )
    free ((void*)im);
}
