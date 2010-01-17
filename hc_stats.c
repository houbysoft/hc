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
#ifdef HCG_E
#undef notify
#endif


void plfbox(PLFLT x, PLFLT y25, PLFLT y50, PLFLT y75, PLFLT lw, PLFLT uw);


// hc_stats()
//   arguments:
//     - e : list of comma separated arguments passed by the user
//     - g : passed by the internal functions, TRUE if we should draw a boxplot, FALSE if we only should display information
//     - ef : TRUE if e is a list of arguments followed by their effective, FALSE if e is just a list of arguments
char hc_stats(char *e, char g, char ef)
{
  struct hc_stack_element_stats *first = malloc(sizeof(struct hc_stack_element_stats));
  struct hc_stack_element_stats *curr;

  M_APM numtmp = m_apm_init();
  M_APM numtmp2 = m_apm_init();
  M_APM numtmp3 = m_apm_init();
  M_APM numtmp4 = m_apm_init();
  M_APM n = m_apm_init();
  M_APM sumx_re = m_apm_init(); m_apm_copy(sumx_re,MM_Zero);
  M_APM sumx_im = m_apm_init(); m_apm_copy(sumx_im,MM_Zero);
  M_APM sumx2_re = m_apm_init(); m_apm_copy(sumx2_re,MM_Zero);
  M_APM sumx2_im = m_apm_init(); m_apm_copy(sumx2_im,MM_Zero);
  M_APM avg_re = m_apm_init();
  M_APM avg_im = m_apm_init();
  M_APM min_re = m_apm_init();
  M_APM min_im = m_apm_init();
  M_APM max_re = m_apm_init();
  M_APM max_im = m_apm_init();
  PLFLT pl_min,pl_q1,pl_q2,pl_q3,pl_max;

  char *out_string = NULL;

  first->re = m_apm_init();
  first->im = m_apm_init();
  first->ef = m_apm_init();
  first->p = NULL;
  first->n = NULL;
  curr = first;

  unsigned int argc = 1;
  char *tmp;
  char *tmp_num_re,*tmp_num_im;
  while ((tmp = hc_get_arg(e,argc))!=NULL)
  {
    char *tmp_res = hc_result_(tmp);
    if (!tmp_res)
    {
      free(tmp);
      m_apm_free(numtmp); m_apm_free(numtmp2); m_apm_free(numtmp3); m_apm_free(numtmp4); m_apm_free(n); m_apm_free(avg_re); m_apm_free(avg_im); m_apm_free(min_re); m_apm_free(min_im); m_apm_free(max_re); m_apm_free(max_im); m_apm_free(sumx_re); m_apm_free(sumx_im); m_apm_free(sumx2_re); m_apm_free(sumx2_im);
      while (first->n)
      {
	m_apm_free(first->re);m_apm_free(first->im);m_apm_free(first->ef);
	first = first->n;
	free(first->p);
      }
      m_apm_free(first->re);m_apm_free(first->im);m_apm_free(first->ef);
      free(first);
      return FAIL;
    }
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
    if (ef)
    {
      argc++;
      tmp = hc_get_arg(e,argc);
      if (!tmp)
      {
	m_apm_free(numtmp); m_apm_free(numtmp2); m_apm_free(numtmp3); m_apm_free(numtmp4); m_apm_free(n); m_apm_free(avg_re); m_apm_free(avg_im); m_apm_free(min_re); m_apm_free(min_im); m_apm_free(max_re); m_apm_free(max_im); m_apm_free(sumx_re); m_apm_free(sumx_im); m_apm_free(sumx2_re); m_apm_free(sumx2_im);
	while (first->n)
	{
	  m_apm_free(first->re);m_apm_free(first->im);m_apm_free(first->ef);
	first = first->n;
	free(first->p);
	}
	m_apm_free(first->re);m_apm_free(first->im);m_apm_free(first->ef);
	free(first);
	return FAIL;
      }
      tmp_num_re = hc_real_part(tmp);
      tmp_num_im = hc_imag_part(tmp);
      if (tmp_num_im)
      {
	free(tmp); free(tmp_num_re); free(tmp_num_im);
	m_apm_free(numtmp); m_apm_free(numtmp2); m_apm_free(numtmp3); m_apm_free(numtmp4); m_apm_free(n); m_apm_free(avg_re); m_apm_free(avg_im); m_apm_free(min_re); m_apm_free(min_im); m_apm_free(max_re); m_apm_free(max_im); m_apm_free(sumx_re); m_apm_free(sumx_im); m_apm_free(sumx2_re); m_apm_free(sumx2_im);
	while (first->n)
	{
	  m_apm_free(first->re);m_apm_free(first->im);m_apm_free(first->ef);
	first = first->n;
	free(first->p);
	}
	m_apm_free(first->re);m_apm_free(first->im);m_apm_free(first->ef);
	free(first);
	return FAIL;
      }
      m_apm_set_string(curr->ef,tmp_num_re);
      free(tmp_num_re); free(tmp);
    } else {
      m_apm_set_string(curr->ef,"1");
    }
    m_apm_copy(numtmp,sumx_re);
    m_apm_copy(numtmp2,sumx_im);
    m_apmc_multiply(numtmp3,numtmp4,curr->re,curr->im,curr->ef,MM_Zero);
    m_apmc_add(sumx_re,sumx_im,numtmp3,numtmp4,numtmp,numtmp2);
    m_apmc_sqr(numtmp,numtmp2,curr->re,curr->im);
    m_apmc_multiply(numtmp3,numtmp4,numtmp,numtmp2,curr->ef,MM_Zero);
    m_apm_copy(numtmp,sumx2_re);
    m_apm_copy(numtmp2,sumx2_im);
    m_apmc_add(sumx2_re,sumx2_im,numtmp,numtmp2,numtmp3,numtmp4);
    m_apm_copy(numtmp,n);
    m_apm_add(n,numtmp,curr->ef);
    m_apmc_multiply(numtmp2,numtmp3,curr->ef,MM_Zero,curr->re,curr->im);
    m_apm_copy(numtmp4,avg_im); m_apm_copy(numtmp,avg_re);
    m_apmc_add(avg_re,avg_im,numtmp2,numtmp3,numtmp,numtmp4);
    curr->n = malloc(sizeof(struct hc_stack_element_stats));
    curr->n->p = curr;
    curr->n->re = m_apm_init();
    curr->n->im = m_apm_init();
    curr->n->ef = m_apm_init();
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
  if (m_apm_compare(n,MM_Zero)==0)
  {
    m_apm_free(numtmp); m_apm_free(numtmp2); m_apm_free(numtmp3); m_apm_free(numtmp4); m_apm_free(n); m_apm_free(avg_re); m_apm_free(avg_im); m_apm_free(min_re); m_apm_free(min_im); m_apm_free(max_re); m_apm_free(max_im); m_apm_free(sumx_re); m_apm_free(sumx_im); m_apm_free(sumx2_re); m_apm_free(sumx2_im);
    while (first->n)
    {
      m_apm_free(first->re);m_apm_free(first->im);m_apm_free(first->ef);
      first = first->n;
      free(first->p);
    }
    m_apm_free(first->re);m_apm_free(first->im);m_apm_free(first->ef);
    free(first);
    return FAIL;
  }
  tmp = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,n,'.',0,0);
  char *n_str = hc_result_(tmp);
  free(tmp);
  tmp = hc_strip_0s(n_str);
  out_string = realloc(out_string, strlen(tmp)+strlen("n = \n")+1);
  sprintf(out_string,"n = %s\n",tmp);
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
  avg_str = tmp;
  tmp = malloc(strlen("Average = \n")+strlen(avg_str)+1);
  sprintf(tmp,"Average = %s\n",avg_str);
  free(avg_str);
  out_string = realloc(out_string,strlen(out_string)+strlen(tmp)+1);
  strcat(out_string,tmp);
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
  m_apm_set_string(numtmp3,"0");
  while (m_apm_compare(numtmp,q1_idx)!=0)
  {
    m_apm_copy(numtmp2,numtmp3);
    m_apm_add(numtmp3,numtmp2,MM_One);
    if (m_apm_compare(numtmp3,curr->ef)==0)
    {
      m_apm_set_string(numtmp3,"0");
      curr = curr->n;
    }
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
    m_apm_copy(numtmp4,numtmp3);
    m_apm_add(numtmp3,numtmp4,MM_One);
    if (m_apm_compare(numtmp3,curr->ef)==0)
    {
      m_apm_copy(numtmp3,curr->n->re);
      m_apm_copy(numtmp4,curr->n->im);
    } else {
      m_apm_copy(numtmp3,curr->re);
      m_apm_copy(numtmp4,curr->im);
    }
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
  pl_q1 = strtod(tmp_num_re,NULL);
  if (g && m_apm_sign(numtmp2)!=0)
    boxplot_cplx_error();
  strcpy(tmp,tmp_num_re);
  strcat(tmp,"i");
  strcat(tmp,tmp_num_im);
  avg_str = hc_result_(tmp);
  free(tmp);
  tmp = hc_strip_0s(avg_str);
  free(tmp_num_re); free(tmp_num_im); free(avg_str);
  avg_str = tmp;
  tmp = malloc(strlen("Q1 = \n")+strlen(avg_str)+1);
  sprintf(tmp,"Q1 = %s\n",avg_str);
  free(avg_str);
  out_string = realloc(out_string,strlen(out_string)+strlen(tmp)+1);
  strcat(out_string,tmp);
  free(tmp);
  
  curr = first;
  m_apm_set_string(numtmp,"1");
  m_apm_set_string(numtmp3,"0");
  while (m_apm_compare(numtmp,v_idx)!=0)
  {
    m_apm_copy(numtmp2,numtmp3);
    m_apm_add(numtmp3,numtmp2,MM_One);
    if (m_apm_compare(numtmp3,curr->ef)==0)
    {
      m_apm_set_string(numtmp3,"0");
      curr = curr->n;
    }
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
    m_apm_copy(numtmp4,numtmp3);
    m_apm_add(numtmp3,numtmp4,MM_One);
    if (m_apm_compare(numtmp3,curr->ef)==0)
    {
      m_apm_copy(numtmp3,curr->n->re);
      m_apm_copy(numtmp4,curr->n->im);
    } else {
      m_apm_copy(numtmp3,curr->re);
      m_apm_copy(numtmp4,curr->im);
    }
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
  pl_q2 = strtod(tmp_num_re,NULL);
  if (g && m_apm_sign(numtmp2)!=0)
    boxplot_cplx_error();
  strcpy(tmp,tmp_num_re);
  strcat(tmp,"i");
  strcat(tmp,tmp_num_im);
  avg_str = hc_result_(tmp);
  free(tmp);
  tmp = hc_strip_0s(avg_str);
  free(tmp_num_re); free(tmp_num_im); free(avg_str);
  avg_str = tmp;
  tmp = malloc(strlen("Median = \n")+strlen(avg_str)+1);
  sprintf(tmp,"Median = %s\n",avg_str);
  free(avg_str);
  out_string = realloc(out_string,strlen(out_string)+strlen(tmp)+1);
  strcat(out_string,tmp);
  free(tmp);

  curr = first;
  m_apm_set_string(numtmp,"1");
  m_apm_set_string(numtmp3,"0");
  while (m_apm_compare(numtmp,q3_idx)!=0)
  {
    m_apm_copy(numtmp2,numtmp3);
    m_apm_add(numtmp3,numtmp2,MM_One);
    if (m_apm_compare(numtmp3,curr->ef)==0)
    {
      m_apm_set_string(numtmp3,"0");
      curr = curr->n;
    }
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
    m_apm_copy(numtmp4,numtmp3);
    m_apm_add(numtmp3,numtmp4,MM_One);
    if (m_apm_compare(numtmp3,curr->ef)==0)
    {
      m_apm_copy(numtmp3,curr->n->re);
      m_apm_copy(numtmp4,curr->n->im);
    } else {
      m_apm_copy(numtmp3,curr->re);
      m_apm_copy(numtmp4,curr->im);
    }
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
  pl_q3 = strtod(tmp_num_re,NULL);
  if (g && m_apm_sign(numtmp2)!=0)
    boxplot_cplx_error();
  strcpy(tmp,tmp_num_re);
  strcat(tmp,"i");
  strcat(tmp,tmp_num_im);
  avg_str = hc_result_(tmp);
  free(tmp);
  tmp = hc_strip_0s(avg_str);
  free(tmp_num_re); free(tmp_num_im); free(avg_str);
  avg_str = tmp;
  tmp = malloc(strlen("Q3 = \n")+strlen(avg_str)+1);
  if (!tmp)
    mem_error();
  sprintf(tmp,"Q3 = %s\n",avg_str);
  free(avg_str);
  out_string = realloc(out_string,strlen(out_string)+strlen(tmp)+1);
  strcat(out_string,tmp);
  free(tmp);

  tmp_num_re = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,min_re,'.',0,0);
  tmp_num_im = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,min_im,'.',0,0);
  tmp = malloc(strlen(tmp_num_re)+1+strlen(tmp_num_im)+1);
  if (!tmp)
    mem_error();
  pl_min = strtod(tmp_num_re,NULL);
  if (g && m_apm_sign(min_im)!=0)
    boxplot_cplx_error();
  strcpy(tmp,tmp_num_re);
  strcat(tmp,"i");
  strcat(tmp,tmp_num_im);
  avg_str = hc_result_(tmp);
  free(tmp);
  tmp = hc_strip_0s(avg_str);
  free(tmp_num_re); free(tmp_num_im); free(avg_str);
  avg_str = tmp;
  tmp = malloc(strlen("Min = \n")+strlen(tmp)+1);
  if (!tmp)
    mem_error();
  sprintf(tmp,"Min = %s\n",avg_str);
  free(avg_str);
  out_string = realloc(out_string,strlen(out_string)+strlen(tmp)+1);
  strcat(out_string,tmp);
  free(tmp);

  tmp_num_re = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,max_re,'.',0,0);
  tmp_num_im = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,max_im,'.',0,0);
  tmp = malloc(strlen(tmp_num_re)+1+strlen(tmp_num_im)+1);
  if (!tmp)
    mem_error();
  pl_max = strtod(tmp_num_re,NULL);
  if (g && m_apm_sign(max_im)!=0)
    boxplot_cplx_error();
  strcpy(tmp,tmp_num_re);
  strcat(tmp,"i");
  strcat(tmp,tmp_num_im);
  avg_str = hc_result_(tmp);
  free(tmp);
  tmp = hc_strip_0s(avg_str);
  free(tmp_num_re); free(tmp_num_im); free(avg_str);
  avg_str = tmp;
  tmp = malloc(strlen("Max = \n")+strlen(tmp)+1);
  if (!tmp)
    mem_error();
  sprintf(tmp,"Max = %s\n",avg_str);
  free(avg_str);
  out_string = realloc(out_string,strlen(out_string)+strlen(tmp)+1);
  strcat(out_string,tmp);
  free(tmp);

  tmp_num_re = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,sumx_re,'.',0,0);
  tmp_num_im = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,sumx_im,'.',0,0);
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
  avg_str = tmp;
  tmp = malloc(strlen("sum(x) = \n")+strlen(tmp)+1);
  if (!tmp)
    mem_error();
  sprintf(tmp,"sum(x) = %s\n",avg_str);
  free(avg_str);
  out_string = realloc(out_string,strlen(out_string)+strlen(tmp)+1);
  strcat(out_string,tmp);
  free(tmp);

  tmp_num_re = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,sumx2_re,'.',0,0);
  tmp_num_im = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,sumx2_im,'.',0,0);
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
  avg_str = tmp;
  tmp = malloc(strlen("sum(x^2) = \n")+strlen(tmp)+1);
  if (!tmp)
    mem_error();
  sprintf(tmp,"sum(x^2) = %s\n",avg_str);
  free(avg_str);
  out_string = realloc(out_string,strlen(out_string)+strlen(tmp)+1);
  strcat(out_string,tmp);
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
    m_apm_free(first->re);m_apm_free(first->im);m_apm_free(first->ef);
    first = first->n;
    free(first->p);
  }
  
  m_apm_free(first->re);m_apm_free(first->im);m_apm_free(first->ef);
  free(first);

  m_apm_free(numtmp); m_apm_free(numtmp2); m_apm_free(numtmp3); m_apm_free(numtmp4);
  m_apm_free(n); m_apm_free(avg_re); m_apm_free(avg_im);
  m_apm_free(q1_idx); m_apm_free(q3_idx); m_apm_free(v_idx);
  m_apm_free(min_re); m_apm_free(min_im); m_apm_free(max_re); m_apm_free(max_im);
  m_apm_free(sumx_re); m_apm_free(sumx_im); m_apm_free(sumx2_re); m_apm_free(sumx2_im);

  if (g)
  {
#ifndef HCG
    if (!hc.plplot_dev_override)
#ifndef WIN32
      plsdev("pngcairo");
#else
    plsdev("wingcc");
#endif
#else
#ifndef WIN32
    plsdev("pngcairo");
    plsfnam("tmp-graph.png");
#else
    plsdev("wingcc");
#endif
#endif
    
    plinit();
    
    pladv(0);
    plvsta();
    
    plwind(-2, 5, pl_min - (pl_max - pl_min) * 0.1, pl_max + (pl_max - pl_min) * 0.1);
    plcol0(15);
    plbox("bc", 1.0, 0, "bcgnst", 0, 0);
    pllab("", "", "#frHoubySoft Calculator - Box plot");
    plcol0(9);
    plfbox(1, pl_q1, pl_q2, pl_q3, pl_min, pl_max);
  
    plend();
#ifdef HCG
#ifndef WIN32
    hcg_disp_graph("tmp-graph.png");
    remove("tmp-graph.png");
#endif
#endif
  } else {
    notify(out_string);
    free(out_string);
  }
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
