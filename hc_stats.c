/*    (c) 2009 Jan Dlabal <dlabaljan@gmail.com>                          */
/*                                                                            */
/*     This file is part of HC (HoubySoft Calculator).                                  */
/*                                                                            */
/*     HC (HoubySoft Calculator) is free software: you can redistribute it and/or modify*/
/*     it under the terms of the GNU General Public License as published by   */
/*     the Free Software Foundation, either version 3 of the License, or      */
/*     any later version.                                                     */
/*                                                                            */
/*     HC (HoubySoft Calculator) is distributed in the hope that it will be useful,     */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of         */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          */
/*     GNU General Public License for more details.                           */
/*                                                                            */
/*     You should have received a copy of the GNU General Public License      */
/*     along with HC (HoubySoft Calculator). If not, see <http://www.gnu.org/licenses/>.*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <plplot/plplot.h>
#include <errno.h>
#include "hc.h"
#include "hc_functions.h"
#include "hc_complex.h"

#define hc_stack_element_stats hc_stack_element

char hc_stats(char *e)
{
  struct hc_stack_element_stats *first = malloc(sizeof(struct hc_stack_element_stats));
  struct hc_stack_element_stats *curr;

  M_APM numtmp = m_apm_init();
  M_APM numtmp2 = m_apm_init();
  M_APM numtmp3 = m_apm_init();
  M_APM numtmp4 = m_apm_init();
  M_APM n = m_apm_init();
  M_APM avg_re = m_apm_init();
  M_APM avg_im = m_apm_init();
  M_APM min_re = m_apm_init();
  M_APM min_im = m_apm_init();
  M_APM max_re = m_apm_init();
  M_APM max_im = m_apm_init();

  first->re = m_apm_init();
  first->im = m_apm_init();
  first->p = NULL;
  first->n = NULL;
  curr = first;

  unsigned int argc = 1;
  char *tmp;
  char *tmp_num_re,*tmp_num_im;
  while ((tmp = hc_get_arg(e,argc))!=NULL)
  {
    m_apm_copy(numtmp,n);
    m_apm_add(n,numtmp,MM_One);
    char *tmp_res = hc_result_(tmp);
    if (!tmp_res)
      exit(0); // FIX FIX FIX
    tmp_num_re = hc_real_part(tmp_res);
    tmp_num_im = hc_imag_part(tmp_res);
    m_apm_set_string(curr->re,tmp_num_re);
    if (tmp_num_im)
      m_apm_set_string(curr->im,tmp_num_im);
    else
      m_apm_copy(curr->im,MM_Zero);
    if (tmp_num_im)
      free(tmp_num_im);
    if (argc==1)
    {
      m_apm_copy(min_re,curr->re);
      m_apm_copy(min_im,curr->im);
      m_apm_copy(max_re,curr->re);
      m_apm_copy(max_im,curr->im);
    } else {
      if (m_apmc_lt(curr->re,curr->im,min_re,min_im))
      {
	m_apm_copy(min_re,curr->re);
	m_apm_copy(min_im,curr->im);
      }
      if (m_apmc_gt(curr->re,curr->im,max_re,max_im))
      {
	m_apm_copy(max_re,curr->re);
	m_apm_copy(max_im,curr->im);
      }
    }
    free(tmp_num_re); free(tmp_res); free(tmp);
    //m_apmc_multiply(numtmp2,numtmp3,MM_One,MM_Zero,curr->re,curr->im);
    m_apm_copy(numtmp2,curr->re); m_apm_copy(numtmp3,curr->im);
    m_apm_copy(numtmp4,avg_im); m_apm_copy(numtmp,avg_re);
    m_apmc_add(avg_re,avg_im,numtmp2,numtmp3,numtmp,numtmp4);
    curr->n = malloc(sizeof(struct hc_stack_element_stats));
    curr->n->p = curr;
    curr->n->re = m_apm_init();
    curr->n->im = m_apm_init();
    curr->n->n = NULL;
    if (curr->p!=NULL)
    {
      while (curr->p && !m_apmc_lt(curr->p->re,curr->p->im,curr->re,curr->im))
      {
	struct hc_stack_element_stats *tmp = curr->p;
	struct hc_stack_element_stats *tmp2 = curr->n;
	if (curr->p->p)
	{
	  curr->p->p->n = curr;
	  curr->p = curr->p->p;
	} else {
	  first = curr;
	  curr->p = NULL;
	}
	curr->n = tmp;
	curr->n->p = curr;
	curr->n->n = tmp2;
	curr->n->n->p = curr->n;
      }
      while (curr->n)
	curr = curr->n;
    } else {
      curr = curr->n;
    }
    argc++;
  }
  // End of initialization

  // n (number of elements)
  tmp = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,n,'.',0,0);
  char *n_str = hc_result_(tmp);
  free(tmp);
  tmp = hc_strip_0s(n_str);
  printf("n = %s\n",tmp);
  free(n_str); free(tmp);

  // Average
  m_apm_copy(numtmp,avg_re); m_apm_copy(numtmp2,avg_im);
  m_apmc_divide(avg_re,avg_im,HC_DEC_PLACES,numtmp,numtmp2,n,MM_Zero);
  tmp_num_re = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,avg_re,'.',0,0);
  tmp_num_im = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,avg_im,'.',0,0);
  tmp = malloc(strlen(tmp_num_re)+1+strlen(tmp_num_im)+1);
  if (!tmp)
    mem_error();
  strcpy(tmp,tmp_num_re);
  strcat(tmp,"i");
  strcat(tmp,tmp_num_im);
  char *avg_str = hc_result_(tmp);
  free(tmp);
  tmp = hc_strip_0s(avg_str);
  free(tmp_num_re); free(tmp_num_im); free(avg_str);
  printf("Average = %s\n",tmp);
  free(tmp);

  char iseven = m_apm_is_even(n);
  char iseven2;
  M_APM q1_idx = m_apm_init();
  M_APM q3_idx = m_apm_init();
  M_APM v_idx = m_apm_init();
  if (iseven)
  {
    m_apm_divide(v_idx,HC_DEC_PLACES,n,MM_Two);
    m_apm_divide(numtmp,HC_DEC_PLACES,v_idx,MM_Two);
    if (m_apm_is_integer(numtmp))
    {
      m_apm_copy(q1_idx,numtmp);
      m_apm_multiply(q3_idx,q1_idx,MM_Three);
      iseven2 = TRUE;
    } else {
      m_apm_copy(q1_idx,numtmp);
      m_apm_multiply(numtmp2,q1_idx,MM_Three);
      m_apm_ceil(q1_idx,numtmp);
      m_apm_ceil(q3_idx,numtmp2);
      iseven2 = FALSE;
    }
  } else {
    m_apm_add(numtmp,n,MM_One);
    m_apm_divide(v_idx,HC_DEC_PLACES,numtmp,MM_Two);
    m_apm_divide(q1_idx,HC_DEC_PLACES,v_idx,MM_Two);
    m_apm_multiply(q3_idx,q1_idx,MM_Three);
    if (m_apm_is_integer(q1_idx))
    {
      iseven2 = FALSE;
    } else {
      iseven2 = TRUE;
      m_apm_copy(numtmp,q1_idx);
      m_apm_floor(q1_idx,numtmp);
      m_apm_copy(numtmp,q3_idx);
      m_apm_floor(q3_idx,numtmp);
      if (m_apm_compare(q1_idx,MM_Zero)==0)
      {
	// 1-element list
	iseven2 = FALSE;
	m_apm_set_string(q1_idx,"1");
	m_apm_set_string(q3_idx,"1");
      }
    }
  }
  
  curr = first;
  m_apm_set_string(numtmp,"1");
  while (m_apm_compare(numtmp,q1_idx)!=0)
  {
    curr = curr->n;
    m_apm_copy(numtmp2,numtmp);
    m_apm_add(numtmp,numtmp2,MM_One);
  }
  if (!iseven2)
  {
    m_apm_copy(numtmp,curr->re);
    m_apm_copy(numtmp2,curr->im);
  } else {
    m_apm_copy(numtmp,curr->re);
    m_apm_copy(numtmp2,curr->im);
    m_apm_copy(numtmp3,curr->n->re);
    m_apm_copy(numtmp4,curr->n->im);
    m_apmc_add(avg_re,avg_im,numtmp,numtmp2,numtmp3,numtmp4);
    m_apm_copy(numtmp,avg_re);
    m_apm_copy(numtmp2,avg_im);
    m_apmc_divide(numtmp,numtmp2,HC_DEC_PLACES,avg_re,avg_im,MM_Two,MM_Zero);
  }
  tmp_num_re = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,numtmp,'.',0,0);
  tmp_num_im = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,numtmp2,'.',0,0);
  tmp = malloc(strlen(tmp_num_re)+1+strlen(tmp_num_im)+1);
  if (!tmp)
    mem_error();
  strcpy(tmp,tmp_num_re);
  strcat(tmp,"i");
  strcat(tmp,tmp_num_im);
  avg_str = hc_result_(tmp);
  free(tmp);
  tmp = hc_strip_0s(avg_str);
  free(tmp_num_re); free(tmp_num_im); free(avg_str);
  printf("Q1 = %s\n",tmp);
  free(tmp);

  m_apm_set_string(numtmp,"1");
  curr = first;
  while (m_apm_compare(numtmp,v_idx)!=0)
  {
    curr = curr->n;
    m_apm_copy(numtmp2,numtmp);
    m_apm_add(numtmp,numtmp2,MM_One);
  }
  if (!iseven)
  {
    m_apm_copy(numtmp,curr->re);
    m_apm_copy(numtmp2,curr->im);
  } else {
    m_apm_copy(numtmp,curr->re);
    m_apm_copy(numtmp2,curr->im);
    m_apm_copy(numtmp3,curr->n->re);
    m_apm_copy(numtmp4,curr->n->im);
    m_apmc_add(avg_re,avg_im,numtmp,numtmp2,numtmp3,numtmp4);
    m_apm_copy(numtmp,avg_re);
    m_apm_copy(numtmp2,avg_im);
    m_apmc_divide(numtmp,numtmp2,HC_DEC_PLACES,avg_re,avg_im,MM_Two,MM_Zero);
  }
  tmp_num_re = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,numtmp,'.',0,0);
  tmp_num_im = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,numtmp2,'.',0,0);
  tmp = malloc(strlen(tmp_num_re)+1+strlen(tmp_num_im)+1);
  if (!tmp)
    mem_error();
  strcpy(tmp,tmp_num_re);
  strcat(tmp,"i");
  strcat(tmp,tmp_num_im);
  avg_str = hc_result_(tmp);
  free(tmp);
  tmp = hc_strip_0s(avg_str);
  free(tmp_num_re); free(tmp_num_im); free(avg_str);
  printf("Median = %s\n",tmp);
  free(tmp);

  curr = first;
  m_apm_set_string(numtmp,"1");
  while (m_apm_compare(numtmp,q3_idx)!=0)
  {
    curr = curr->n;
    m_apm_copy(numtmp2,numtmp);
    m_apm_add(numtmp,numtmp2,MM_One);
  }
  if (!iseven2)
  {
    m_apm_copy(numtmp,curr->re);
    m_apm_copy(numtmp2,curr->im);
  } else {
    m_apm_copy(numtmp,curr->re);
    m_apm_copy(numtmp2,curr->im);
    m_apm_copy(numtmp3,curr->n->re);
    m_apm_copy(numtmp4,curr->n->im);
    m_apmc_add(avg_re,avg_im,numtmp,numtmp2,numtmp3,numtmp4);
    m_apm_copy(numtmp,avg_re);
    m_apm_copy(numtmp2,avg_im);
    m_apmc_divide(numtmp,numtmp2,HC_DEC_PLACES,avg_re,avg_im,MM_Two,MM_Zero);
  }
  tmp_num_re = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,numtmp,'.',0,0);
  tmp_num_im = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,numtmp2,'.',0,0);
  tmp = malloc(strlen(tmp_num_re)+1+strlen(tmp_num_im)+1);
  if (!tmp)
    mem_error();
  strcpy(tmp,tmp_num_re);
  strcat(tmp,"i");
  strcat(tmp,tmp_num_im);
  avg_str = hc_result_(tmp);
  free(tmp);
  tmp = hc_strip_0s(avg_str);
  free(tmp_num_re); free(tmp_num_im); free(avg_str);
  printf("Q3 = %s\n",tmp);
  free(tmp);

tmp_num_re = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,min_re,'.',0,0);
  tmp_num_im = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,min_im,'.',0,0);
  tmp = malloc(strlen(tmp_num_re)+1+strlen(tmp_num_im)+1);
  if (!tmp)
    mem_error();
  strcpy(tmp,tmp_num_re);
  strcat(tmp,"i");
  strcat(tmp,tmp_num_im);
  avg_str = hc_result_(tmp);
  free(tmp);
  tmp = hc_strip_0s(avg_str);
  free(tmp_num_re); free(tmp_num_im); free(avg_str);
  printf("Min = %s\n",tmp);
  free(tmp);

  tmp_num_re = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,max_re,'.',0,0);
  tmp_num_im = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,max_im,'.',0,0);
  tmp = malloc(strlen(tmp_num_re)+1+strlen(tmp_num_im)+1);
  if (!tmp)
    mem_error();
  strcpy(tmp,tmp_num_re);
  strcat(tmp,"i");
  strcat(tmp,tmp_num_im);
  avg_str = hc_result_(tmp);
  free(tmp);
  tmp = hc_strip_0s(avg_str);
  free(tmp_num_re); free(tmp_num_im); free(avg_str);
  printf("Max = %s\n",tmp);
  free(tmp);

#ifdef DBG
  curr = first;
  if (curr->p)
    printf("wtf?\n");
  while (curr->n)
  {
    printf("%s\n",m_apm_to_fixpt_stringexp(HC_DEC_PLACES,curr->re,'.',0,0));
    curr = curr->n;
  }
#endif

  while (first->n)
  {
    m_apm_free(first->re);m_apm_free(first->im);
    first = first->n;
    free(first->p);
  }
  
  m_apm_free(first->re);m_apm_free(first->im);
  free(first);

  m_apm_free(numtmp); m_apm_free(numtmp2); m_apm_free(numtmp3); m_apm_free(numtmp4);
  m_apm_free(n); m_apm_free(avg_re); m_apm_free(avg_im);
  m_apm_free(q1_idx); m_apm_free(q3_idx); m_apm_free(v_idx);
  m_apm_free(min_re); m_apm_free(min_im); m_apm_free(max_re); m_apm_free(max_im);

  return SUCCESS;
}


void plfbox(PLFLT x, PLFLT y25, PLFLT y50, PLFLT y75, PLFLT lw, PLFLT uw)
{
    PLFLT px[5], py[5], mx[2], my[2], wx[2], wy[2], barx[2], bary[2];
    PLFLT spacing;
    PLFLT xmin, xmax;
    PLFLT xmid, xwidth;

    spacing = .4; /* in x axis units */

    xmin = x + spacing / 2.;
    xmax = x + 1. - spacing / 2.;

    /* box */
    
    px[0] = xmin;
    py[0] = y25;
    px[1] = xmin;
    py[1] = y75;
    px[2] = xmax;
    py[2] = y75;
    px[3] = xmax;
    py[3] = y25;
    px[4] = xmin;
    py[4] = y25;

    plpsty(0);
    plfill(4, px, py);
    plcol0(1);
    pllsty(1);
    plline(5, px, py);


    /* median */

    mx[0] = xmin;
    my[0] = y50;
    mx[1] = xmax;
    my[1] = y50;

    pllsty(1);
    plline(2, mx, my);
    
    /* lower whisker */

    xmid = (xmin + xmax) / 2.;
    xwidth = xmax - xmin;
    wx[0] = xmid;
    wy[0] = lw;
    wx[1] = xmid;
    wy[1] = y25;
   
    pllsty(2); /* short dashes and gaps */
    plline(2, wx, wy);
    
    barx[0] = xmid - xwidth / 4.;
    bary[0] = lw;
    barx[1] = xmid + xwidth / 4.;
    bary[1] = lw;

    pllsty(1);
    plline(2, barx, bary);

    /* upper whisker */
    
    xmid = (xmin + xmax) / 2.;
    xwidth = xmax - xmin;
    wy[0] = y75;
    wy[1] = uw;
   
    pllsty(2); /* short dashes and gaps */
    plline(2, wx, wy);
    
    bary[0] = uw;
    bary[1] = uw;

    pllsty(1);
    plline(2, barx, bary);
}
