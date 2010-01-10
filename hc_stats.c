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

  first->re = m_apm_init();
  first->im = m_apm_init();
  first->eff = m_apm_init();
  first->p = NULL;
  first->n = NULL;
  curr = first;

  unsigned int argc = 1;
  char *tmp;
  char *tmp_num_re,*tmp_num_im,*eff;
  while ((tmp = hc_get_arg(e,argc))!=NULL)
  {
    char *separator = strchr(tmp,':');
    if (separator==NULL)
    {
      // assume effective is 1
      m_apm_copy(curr->eff,MM_One);
    } else {
      separator = '\0';
      eff = hc_result_(separator+sizeof(char));
      // FIX FIX FIX exit()'s
      if (!eff)
	exit(0);
      if (strchr(eff,'i')!=NULL)
	exit(0);
      m_apm_set_string(curr->eff,eff);
      free(eff);
    }
    m_apm_copy(numtmp,n);
    m_apm_add(n,numtmp,curr->eff);
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
    free(tmp_num_re); free(tmp_res); free(tmp);
    m_apmc_multiply(numtmp2,numtmp3,curr->eff,MM_Zero,curr->re,curr->im);
    m_apm_copy(numtmp4,avg_im); m_apm_copy(numtmp,avg_re);
    m_apmc_add(avg_re,avg_im,numtmp2,numtmp3,numtmp,numtmp4);
    curr->n = malloc(sizeof(struct hc_stack_element_stats));
    curr->n->p = curr;
    curr->n->re = m_apm_init();
    curr->n->im = m_apm_init();
    curr->n->eff = m_apm_init();
    curr->n->n = NULL;
    curr = curr->n;
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

  return SUCCESS;
}
