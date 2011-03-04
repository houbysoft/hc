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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "hc.h"
#include "hc_functions.h"
#include "hc_complex.h"
#include "hc_graph.h"
#include "hc_list.h"
#include "hc_utils.h"
#ifdef HCG_E
#undef notify
#endif



// hc_stats()
//   arguments:
//     - e : list of comma separated arguments passed by the user
//     - g : passed by the internal functions, TRUE if we should draw a boxplot, FALSE if we only should display information
char hc_stats(char *f, char g)
{
  if (!is_list(f))
  {
    arg_error("stats() : argument must be a list of values");
  }
  char *e = list_clean(f);
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
  double pl_min,pl_q1,pl_q2,pl_q3,pl_max;

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
    char *tmp_res_val=NULL,*tmp_res_eff=NULL;
    if (is_list(tmp_res))
    {
      char *cleanlist = list_clean(tmp_res);
      tmp_res_val = hc_get_arg(cleanlist,1);
      tmp_res_eff = hc_get_arg(cleanlist,2);
    } else {
      tmp_res_val = tmp_res;
    }
    tmp_num_re = hc_real_part(tmp_res_val);
    tmp_num_im = hc_imag_part(tmp_res_val);
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
    if (tmp_res_eff)
    {
      free(tmp_res_val);
      tmp_num_re = hc_real_part(tmp_res_eff);
      tmp_num_im = hc_imag_part(tmp_res_eff);
      free(tmp_res_eff);
      if (tmp_num_im)
      {
	free(tmp_num_re); free(tmp_num_im);
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
      free(tmp_num_re);
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
    hc_graph_boxplot("#frHC - Box plot", "", "", pl_min, pl_max, pl_q1, pl_q2, pl_q3);
  } else {
    notify(out_string);
    free(out_string);
  }
  return SUCCESS;
}


// Returns a vector containing two elements; the first one is the slope and the second one is the intercept of the best fit line through the data given
char *hc_linreg(char *e)
{
  if (!is_list(e))
  {
    arg_error("linreg() : first argument must be a list of lists containing an x and y value each");
  }

  unsigned int m_Sxy, m_Sx, m_Sy, m_Sx2;
  m_Sxy = m_Sx = m_Sy = m_Sx2 = 1;
  char *Sxy=malloc(m_Sxy), *Sx=malloc(m_Sx), *Sy=malloc(m_Sy), *Sx2=malloc(m_Sx2);
  char *slope=NULL, *intercept=NULL;
  char *data= list_clean(e);
  // N -- length of input data
  // Sxy -- sum of products
  // Sx -- sum of x values
  // Sy -- sum of y values
  // Sx2 -- sum of x^2 values
  if (!Sxy || !Sx || !Sy || !Sx2) mem_error();
  Sxy[0] = Sx[0] = Sy[0] = Sx2[0] = '\0';

  unsigned int idx = 0;
  char *cur = NULL;
  while ((cur = hc_get_arg(data,++idx)) != NULL)
  {
    if (!is_list(cur))
    {
      free(cur);
      free(Sxy); free(Sx); free(Sy); free(Sx2);
      arg_error("linreg() : invalid data; a list of lists of x and y pairs is expected");
    }
    char *pt = list_clean(cur);
    char *x = hc_get_arg_r(pt,1); char *y = hc_get_arg_r(pt,2);
    free(cur);
    if (!x || !y)
    {
      free(x); free(y);
      free(Sxy); free(Sx); free(Sy); free(Sx2);
      arg_error("linreg() : invalid data; a list of lists of x and y pairs is expected");
    }
    unsigned int xlen = strlen(x), ylen = strlen(y);

    m_Sxy += xlen + 1 + ylen + 1;
    Sxy = realloc(Sxy, m_Sxy); if (!Sxy) mem_error();
    strcat(Sxy,x); strcat(Sxy,"*"); strcat(Sxy,y); strcat(Sxy,"+");

    m_Sx += xlen + 1;
    Sx = realloc(Sx, m_Sx); if (!Sx) mem_error();
    strcat(Sx,x); strcat(Sx,"+");

    m_Sy += ylen + 1;
    Sy = realloc(Sy, m_Sy); if (!Sy) mem_error();
    strcat(Sy,y); strcat(Sy,"+");

    m_Sx2 += xlen + 2 + 1;
    Sx2 = realloc(Sx2, m_Sx2); if (!Sx2) mem_error();
    strcat(Sx2,x); strcat(Sx2,"^2+");

    free(x); free(y);
  }

  // delete the trailing + signs
  Sxy[strlen(Sxy)-1]='\0';
  Sx[strlen(Sx)-1]='\0';
  Sy[strlen(Sy)-1]='\0';
  Sx2[strlen(Sx2)-1]='\0';

  char *r_Sxy=hc_result_(Sxy), *r_Sx=hc_result_(Sx), *r_Sy=hc_result_(Sy), *r_Sx2=hc_result_(Sx2);
  free(Sxy); free(Sx); free(Sy); free(Sx2);

  if (!r_Sxy || !r_Sx || !r_Sy || !r_Sx2)
  {
    free(r_Sxy); free(r_Sx); free(r_Sy); free(r_Sx2);
    return NULL;
  }

  char *N = malloc(hc_need_space_int(idx-1)+1);
  if (!N) mem_error();
  sprintf(N,"%u",idx-1);

  // Need space for (n * r_Sxy - r_Sx * r_Sy) / (n * r_Sx2 - (r_Sx)^2)
  slope = malloc(1+strlen(N)+1+strlen(r_Sxy)+1+strlen(r_Sx)+1+strlen(r_Sy)+1+1+1+strlen(N)+1+strlen(r_Sx2)+1+1+strlen(r_Sx)+1+2+1+1);
  if (!slope) mem_error();
  sprintf(slope,"(%s*%s-%s*%s)/(%s*%s-(%s)^2)",N,r_Sxy,r_Sx,r_Sy,N,r_Sx2,r_Sx);

  char *r_slope = hc_result_(slope);
  free(slope);
  if (!r_slope)
  {
    free(r_Sxy); free(r_Sx); free(r_Sy); free(r_Sx2); free(N);
    return NULL;
  }

  // Need space for (r_Sy - r_slope * r_Sx) / n
  intercept = malloc(1+strlen(r_Sy)+1+strlen(r_slope)+1+strlen(r_Sx)+1+1+strlen(N)+1);
  if (!intercept) mem_error();
  sprintf(intercept,"(%s-%s*%s)/%s",r_Sy,r_slope,r_Sx,N);

  char *r_intercept = hc_result_(intercept);
  free(intercept);
  free(r_Sxy); free(r_Sx); free(r_Sy); free(r_Sx2); free(N);
  if (!r_intercept)
  {
    return NULL;
  }

  char *R = malloc(1+strlen(r_slope)+1+strlen(r_intercept)+1+1);
  if (!R) mem_error();
  sprintf(R,"[%s,%s]",r_slope,r_intercept);

  free(r_slope); free(r_intercept);

  return R;
}
