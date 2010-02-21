/*    (c) 2009-2010 Jan Dlabal <dlabaljan@gmail.com>                          */
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
#include <m_apm.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "hc_complex.h"
#include "hc.h"


char *hc_condition_result(char *e)
{
  int center_len = 2;
  char *center = strstr(e,"==");
  int cond_type = HC_COND_E;
  center_len = 2;
  if (!center)
  {
    center = strstr(e,"!=");
    cond_type = HC_COND_NE;
  }
  if (!center)
  {
    center = strstr(e,">=");
    cond_type = HC_COND_GE;
  }
  if (!center)
  {
    center = strstr(e,"<=");
    cond_type = HC_COND_SE;
  }
  if (!center)
  {
    center = strstr(e,"<");
    center_len = 1;
    cond_type = HC_COND_S;
  }
  if (!center)
  {
    center = strstr(e,">");
    center_len = 1;
    cond_type = HC_COND_G;
  }
  if (!center)
    error_nq("Cannot found condition operator.");
  char *first_part = malloc((center - e + 1)*sizeof(char));
  strncpy(first_part, e, center - e);
  first_part[center - e]=0;
  char *first_result = hc_result_(first_part);
  free(first_part);
  char *second_part = malloc(strlen(center+center_len)+1);
  strcpy(second_part, center+center_len);
  char *second_result = hc_result_(second_part);
  free(second_part);
  if (!first_result || !second_result)
  {
    if (first_result)
      free(first_result);
    if (second_result)
      free(second_result);
    return NULL;
  }
  M_APM first_re,first_im,second_re,second_im;
  first_re = m_apm_init(); first_im = m_apm_init(); second_re = m_apm_init(); second_im = m_apm_init();
  char *tmp = hc_real_part(first_result);
  m_apm_set_string(first_re,tmp);
  free(tmp); tmp = hc_imag_part(first_result);
  if (tmp)
  {
    m_apm_set_string(first_im,tmp);
    free(tmp);
  }
  else
    m_apm_copy(first_im,MM_Zero);
  tmp = hc_real_part(second_result);
  m_apm_set_string(second_re,tmp);
  free(tmp); tmp = hc_imag_part(second_result);
  if (tmp)
  {
    m_apm_set_string(second_im,tmp);
    free(tmp);
  }
  else
    m_apm_copy(second_im,MM_Zero);
  free(first_result); free(second_result);
  char result;
  switch (cond_type)
  {
  case HC_COND_E:
    if (m_apmc_eq(first_re,first_im,second_re,second_im))
      result = 1;
    else
      result = 0;
    break;
    
  case HC_COND_NE:
    if (m_apmc_eq(first_re,first_im,second_re,second_im))
      result = 0;
    else
      result = 1;
    break;
    
  case HC_COND_GE:
    if (m_apmc_ge(first_re,first_im,second_re,second_im))
      result = 1;
    else
      result = 0;
    break;
    
  case HC_COND_SE:
    if (m_apmc_le(first_re,first_im,second_re,second_im))
      result = 1;
    else
      result = 0;
    break;
    
  case HC_COND_S:
    if (m_apmc_lt(first_re,first_im,second_re,second_im))
      result = 1;
    else
      result = 0;
    break;
    
  case HC_COND_G:
    if (m_apmc_gt(first_re,first_im,second_re,second_im))
      result = 1;
    else
      result = 0;
    break;
  }
  m_apm_free(first_re); m_apm_free(first_im); m_apm_free(second_re); m_apm_free(second_im);
  char *r = malloc(2);
  sprintf(r,"%i",result);
  return r;
}


void hc_exec_struct(char *f)
{
  char *e = malloc(strlen(f)+1);
  char *fme = e; // needed because the starting address of e gets modified
  if (!e)
    mem_error();
  strcpy(e,f);
  HC_EXECS type;
  if (strstr(e,"if ")==e)
    type = HC_EXEC_IF;

  // Strip the structure name now that we don't need it
  while (!isspace(e[0]))
    e++;
  e++;

  char *cond;

  switch (type)
  {
  case HC_EXEC_IF:
    if (e[0]!='(') // need a condition in parenthesis
    {
      syntax_error2();
      free(fme);
      return;
    }
    e++;
    int par = 1;
    int end = 1;
    while (par!=0 && e[end]!='\0')
    {
      if (e[end]=='(')
	par++;
      if (e[end]==')')
	par--;
      end++;
    }
    if (par)
    {
      syntax_error2();
      free(fme);
      return;
    }
    e[end-1]=0;
    cond = hc_result(e);
    if (cond && strcmp(cond,"0")!=0)
    {
      // execute
      free(hc_result((char *)e+sizeof(char)*(end+1)));
    }
    break;
  }

  free(fme);
  return;
}
