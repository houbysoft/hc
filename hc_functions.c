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
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include "hc.h"
#include "hc_functions.h"
#include "hc_complex.h"
#include "hc_list.h"
#include "hc_utils.h"


#define b10(c) (isdigit(c) ? c - 48 : tolower(c) - 87) // '0' == 48, 'a' == 97

char *gamma_coefficients = NULL;
unsigned int gamma_coefficients_lp = 0;


char *strreplace_(char *in, char *old, char *new)
{
#ifdef DBG
  printf("strreplace_ received %s\n",in);
#endif
  char *p = strstr(in,old);
  char *result;
  if (!p || strcmp(old,new)==0)
  {
    result = strdup(in);
    if (!result)
      mem_error();
    return result;
  } else {
    result = malloc(sizeof(char) * (strlen(in)+1-strlen(old)+strlen(new)));
    if (!result)
      mem_error();
    memset(result,0,strlen(in)+1-strlen(old)+strlen(new));
    strncpy(result,in,p-in);
    strcat(result,new);
    p+= strlen(old);
    strcat(result,p);
    char *tmp=malloc(2);
    strcpy(tmp,"#");
    while (strstr(result,old)!=NULL && strcmp(tmp,result)!=0)
    {
      if (tmp)
        free(tmp);
      tmp = result;
      result = strreplace_(result,old,new);
    }
    free(tmp);
    return result;
  }
}


char *strreplace(char *in, char *old, char *new)
{
#ifdef DBG
  printf("strreplace received %s\n",in);
#endif
  char *p = strstr(in,old);
  char *result;
  if (!p || strcmp(old,new)==0)
  {
    result = strdup(in);
    if (!result)
      mem_error();
    return result;
  }
  char *tnew=NULL;
  if (!is_string(new) && !is_vector(new))
  {
    tnew = malloc(strlen(new)+3);
    if (!tnew)
      mem_error();
    strcpy(tnew,"(");
    strcat(tnew,new);
    strcat(tnew,")");
    new = tnew;
  } else {
    new = tnew = strdup(new);
  }
  while (p)
  {
    char dorpl = FALSE;
    if (p!=in && p!=(in+strlen(in)-1))
    {
      if ((!isalnum((int)(p-sizeof(char))[0])) && (!isalnum((int)(p+(sizeof(char)*strlen(old)))[0])))
      {
        dorpl = TRUE;
      }
    } else {
      if ((p!=in) && (!isalnum((int)(p-sizeof(char))[0])))
      {
        dorpl = TRUE;
      } else {
        if ((p!=(in+strlen(in)-1)) && (!isalnum((int)(p+(sizeof(char)*strlen(old)))[0])))
        {
          dorpl = TRUE;
        } else {
          if (p==in && p==(in+strlen(in)-1))
          {
            dorpl = TRUE;
          } else {
            dorpl = FALSE;
          }
        }
      }
    }
    char *p_tmp = in;
    int inquotes = FALSE;
    while (p_tmp != p)
    {
      if (p_tmp[0]=='\"')
      {
        inquotes = inquotes == FALSE ? TRUE : FALSE;
      }
      p_tmp++;
    }
    if (inquotes)
    {
      dorpl = FALSE;
    }
    
    if (dorpl)
    {
      result = malloc(sizeof(char) * (strlen(in)+1-strlen(old)+strlen(new)));
      if (!result)
        mem_error();
      memset(result,0,strlen(in)+1-strlen(old)+strlen(new));
      strncpy(result,in,p-in);
      strcat(result,new);
      p+= strlen(old);
      strcat(result,p);
      char *tmp=malloc(2);
      strcpy(tmp,"#");
      while (strstr(result,old)!=NULL && strcmp(tmp,result)!=0)
      {
        if (tmp)
          free(tmp);
        tmp = result;
        result = strreplace(result,old,new);
      }
      free(tmp);
      free(tnew);
      return result;
    } else {
      p = strstr(p+sizeof(char), old);
    }
  }
  result = strdup(in);
  if (!result)
    mem_error();
  free(tnew);
  return result;
}


char *hc_get_arg(char *e, int pos)
{
  int i=0;
  int par=0;
  int sq_par=0;
  int p=0;
  char *tmp = strdup(e);
  if (!tmp)
    mem_error();
  char *last_pointer = tmp;
  char *result = NULL;
  char ignore = FALSE;

  for (; tmp[i]!=0; i++)
  {
    if (tmp[i]=='\"')
    {
      ignore = ignore == FALSE ? TRUE : FALSE;
      continue;
    }
    if (ignore)
      continue;
    if (tmp[i]=='(')
      par++;
    if (tmp[i]==')')
      par--;
    if (tmp[i]=='[')
      sq_par++;
    if (tmp[i]==']')
      sq_par--;
    if ((tmp[i]==',')&&(par==0)&&(sq_par==0))
    {
      p++;
      tmp[i]=0;
      if (p==pos)
      {
        if (last_pointer!=tmp)
        {
          result = strdup(last_pointer);
          if (!result)
            mem_error();
          free(tmp);
          return result;
        } else {
          return last_pointer;
        }
      } else {
        last_pointer = tmp + sizeof(char)*i + sizeof(char);
      }
    }
  }
  if (p+1==pos)
  {
    result = strdup(last_pointer);
    if (!result)
      mem_error();
    free(tmp);
    return result;
  } else {
    free(tmp);
    return NULL;
  }
}


char *hc_get_arg_r(char *e, int pos)
{
  char *r1 = hc_get_arg(e,pos);
  char *r2 = hc_result_(r1);
  free(r1);
  return r2;
}


int hc_modulus(M_APM result, char *e)
{
  if (strchr(e,',')==NULL)
    arg_error("mod() needs two arguments.");
  
  /* 2-arg INIT */
  char *arg1,*arg2;
  arg1 = hc_get_arg_r(e,1);  arg2 = hc_get_arg_r(e,2);
  if (arg1 == NULL || arg2 == NULL)
    return FAIL;
  /* 2-arg INIT END */
  char *ipart = hc_imag_part(arg1);
  if (ipart)
  {
    free(ipart);
    free(arg1); free(arg2);
    arg_error("mod() : real arguments are required.");
  } else {
    ipart = hc_imag_part(arg2);
    if (ipart)
    {
      free(ipart);
      free(arg1); free(arg2);
      arg_error("mod() : real arguments are required.");
    }
  }
  M_APM a,b,tmp;
  tmp = m_apm_init();
  a = m_apm_init(); m_apm_set_string(a,arg1);
  b = m_apm_init(); m_apm_set_string(b,arg2);
  m_apm_integer_div_rem(tmp,result,a,b);

  m_apm_free(a); m_apm_free(b); m_apm_free(tmp);
  free(arg1); free(arg2);
  return SUCCESS;
}


int hc_binomc(M_APM result, char *e)
{
  if (strchr(e,',')==NULL)
    arg_error("nCr() needs two arguments.");
 
  /* 2-arg INIT */
  char *arg1,*arg2;
  arg1 = hc_get_arg_r(e,1);  arg2 = hc_get_arg_r(e,2);
  if (arg1 == NULL || arg2 == NULL)
    return FAIL;
  /* 2-arg INIT END */

  char *ipart = hc_imag_part(arg1);
  if (ipart)
  {
    free(ipart);
    free(arg1); free(arg2);
    arg_error("nCr() arguments must be real numbers.");
  } else {
    ipart = hc_imag_part(arg2);
    if (ipart)
    {
      free(ipart);
      free(arg1); free(arg2);
      arg_error("nCr() arguments must be real numbers.");
    }
  }

  M_APM n,k;
  n = m_apm_init();
  k = m_apm_init();
  m_apm_set_string(n,arg1);
  m_apm_set_string(k,arg2);
  
  free(arg1);
  free(arg2);

  if (m_apm_compare(k,n)==1)
  {
    m_apm_free(n); m_apm_free(k);
    m_apm_set_string(result,"0");
    return SUCCESS;
  }

  M_APM copy_tmp,copy_tmp2,accum,i;
  copy_tmp = m_apm_init();
  copy_tmp2 = m_apm_init();
  accum = m_apm_init();
  i = m_apm_init();
  m_apm_divide(copy_tmp,HC_DEC_PLACES,n,MM_Two);

  if (m_apm_compare(k,copy_tmp)==1)
  {
    m_apm_copy(copy_tmp,k);
    m_apm_subtract(k,n,copy_tmp);
  }

  m_apm_set_string(accum,"1");
  m_apm_set_string(i,"1");
  while (m_apm_compare(i,k)!=1)
  {
    m_apm_subtract(copy_tmp,n,k);
    m_apm_add(copy_tmp2,copy_tmp,i);
    m_apm_multiply(copy_tmp,copy_tmp2,accum);
    m_apm_divide(accum,HC_DEC_PLACES,copy_tmp,i);
    m_apm_copy(copy_tmp,i);
    m_apm_add(i,copy_tmp,MM_One);
  }
  
  m_apm_copy(result,accum);
  m_apm_free(n); m_apm_free(k); m_apm_free(accum); m_apm_free(i); m_apm_free(copy_tmp); m_apm_free(copy_tmp2);
  //return accum + 0.5; // avoid rounding error
  return SUCCESS;
}


int hc_permutations(M_APM result, char *e)
{
  if (strchr(e,',')==NULL)
    arg_error("nPr() needs two arguments.");
 
  /* 2-arg INIT */
  char *arg1,*arg2;
  arg1 = hc_get_arg_r(e,1);  arg2 = hc_get_arg_r(e,2);
  if (arg1 == NULL || arg2 == NULL)
    return FAIL;
  /* 2-arg INIT END */

  char *ipart = hc_imag_part(arg1);
  if (ipart)
  {
    free(ipart);
    free(arg1); free(arg2);
    arg_error("nPr() arguments must be real numbers.");
  } else {
    ipart = hc_imag_part(arg2);
    if (ipart)
    {
      free(ipart);
      free(arg1); free(arg2);
      arg_error("nPr() arguments must be real numbers.");
    }
  }

  M_APM n,k;
  n = m_apm_init();
  k = m_apm_init();
  m_apm_set_string(n,arg1);
  m_apm_set_string(k,arg2);
  free(arg1); free(arg2);

  if (m_apm_compare(n,MM_One)==-1)
  {
    m_apm_free(n); m_apm_free(k);
    arg_error("nPr() : n needs to be positive.");
  }
  if (m_apm_compare(k,MM_Zero)==-1)
  {
    m_apm_free(n); m_apm_free(k);
    arg_error("nPr() : k needs to be positive or 0.");
  }
  if (m_apm_compare(k,n)==1)
  {
    m_apm_free(n); m_apm_free(k);
    arg_error("nPr() : k must be equal or smaller than n.");
  }
  if (!m_apm_is_integer(k))
  {
    m_apm_free(n); m_apm_free(k);
    arg_error("nPr() : second argument needs to be an integer.");
  }

  M_APM i,copy_tmp,copy_tmp2;
  i = m_apm_init();
  copy_tmp = m_apm_init();
  copy_tmp2 = m_apm_init();
  m_apm_set_string(i,"0");
  m_apm_set_string(result,"1");

  while (m_apm_compare(i,k)!=0)
  {
    m_apm_subtract(copy_tmp,n,i);
    m_apm_copy(copy_tmp2,result);
    m_apm_multiply(result,copy_tmp2,copy_tmp);
    m_apm_copy(copy_tmp2,i);
    m_apm_add(i,copy_tmp2,MM_One);
  }

  m_apm_free(n); m_apm_free(k); m_apm_free(i); m_apm_free(copy_tmp); m_apm_free(copy_tmp2);

  return SUCCESS;
}


int hc_sum(M_APM result_re, M_APM result_im, char *e)
{
  /* 3-arg INIT mod */
  char *arg1,*arg2,*arg3;
  arg1 = hc_get_arg(e,1);  arg2 = hc_get_arg_r(e,2);  arg3 = hc_get_arg_r(e,3);
  if (arg1 == NULL || arg2 == NULL || arg3 == NULL)
  {
    free(arg1); free(arg2); free(arg3);
    arg_error("sum() needs 3 arguments (expr,low,high).");
  }
  /* 3-arg INIT END */

  M_APM min,max,res_re,res_im,copy_tmp_re,copy_tmp_im,copy_tmp2_re,copy_tmp2_im;
  min = m_apm_init();
  max = m_apm_init();
  res_re = m_apm_init();
  res_im = m_apm_init();
  copy_tmp_re = m_apm_init();
  copy_tmp_im = m_apm_init();
  copy_tmp2_re = m_apm_init();
  copy_tmp2_im = m_apm_init();
  m_apm_set_string(min,arg2);
  m_apm_set_string(max,arg3);
  m_apm_set_string(res_re,"0");
  m_apm_set_string(res_im,"0");

  free(arg2); free(arg3);

  if ((m_apm_is_integer(min)==0) || (m_apm_is_integer(max)==0))
    arg_error("sum() : bounds need to be integers.");

  char *tmp_expr = malloc(sizeof(char) *(strlen(arg1)+1));
  char *tmp_i_str = NULL;
  char *tmp_fme;
  char *tmp_res;

  while (m_apm_compare(min,max)!=1)
  {
    memset(tmp_expr,0,strlen(arg1)+1);
    free(tmp_i_str);
    tmp_i_str = m_apm_to_fixpt_stringexp(-1,min,'.',0,0);
    strcpy(tmp_expr,arg1);
    tmp_fme = strreplace(tmp_expr,"x",tmp_i_str);
    tmp_res = hc_result_(tmp_fme);
    free(tmp_fme);
    if (tmp_res == NULL)
    {
      free(tmp_expr);
      free(tmp_i_str);
      free(arg1);
      m_apm_free(min); m_apm_free(max); m_apm_free(res_re); m_apm_free(res_im); m_apm_free(copy_tmp_re); m_apm_free(copy_tmp_im); m_apm_free(copy_tmp2_re); m_apm_free(copy_tmp2_im);
      return FAIL;
    }
    m_apm_copy(copy_tmp_re,res_re);
    m_apm_copy(copy_tmp_im,res_im);
    char *tmp_res_re = hc_real_part(tmp_res);
    char *tmp_res_im = hc_imag_part(tmp_res);
    m_apm_set_string(copy_tmp2_re,tmp_res_re);
    if (tmp_res_im)
      m_apm_set_string(copy_tmp2_im,tmp_res_im);
    else
      m_apm_set_string(copy_tmp2_im,"0");
    m_apmc_add(res_re,res_im,copy_tmp_re,copy_tmp_im,copy_tmp2_re,copy_tmp2_im);
    free(tmp_res); free(tmp_res_re); if (tmp_res_im) free(tmp_res_im);
    m_apm_copy(copy_tmp_re,min);
    m_apm_add(min,copy_tmp_re,MM_One);
  }

  free(arg1);
  free(tmp_expr);
  free(tmp_i_str);
  m_apm_copy(result_re,res_re);
  m_apm_copy(result_im,res_im);
  m_apm_free(min); m_apm_free(max); m_apm_free(res_re); m_apm_free(res_im); m_apm_free(copy_tmp_re); m_apm_free(copy_tmp_im); m_apm_free(copy_tmp2_re); m_apm_free(copy_tmp2_im);
  return SUCCESS;
}


int hc_product(M_APM result_re, M_APM result_im, char *e)
{
  /* 3-arg INIT mod */
  char *arg1,*arg2,*arg3;
  arg1 = hc_get_arg(e,1);  arg2 = hc_get_arg_r(e,2);  arg3 = hc_get_arg_r(e,3);
  if (arg1 == NULL || arg2 == NULL || arg3 == NULL)
  {
    free(arg1); free(arg2); free(arg3);
    arg_error("product() needs 3 arguments (expr,low,high).");
  }
  /* 3-arg INIT END */

  M_APM min,max,res_re,res_im,copy_tmp_re,copy_tmp_im,copy_tmp2_re,copy_tmp2_im;
  min = m_apm_init();
  max = m_apm_init();
  res_re = m_apm_init();
  res_im = m_apm_init();
  copy_tmp_re = m_apm_init();
  copy_tmp_im = m_apm_init();
  copy_tmp2_re = m_apm_init();
  copy_tmp2_im = m_apm_init();
  m_apm_set_string(min,arg2);
  m_apm_set_string(max,arg3);

  free(arg2); free(arg3);

  if ((m_apm_is_integer(min)==0) || (m_apm_is_integer(max)==0))
    arg_error("product() : bounds need to be integers.");

  char *tmp_expr = malloc(sizeof(char) *(strlen(arg1)+1));
  char *tmp_i_str = NULL;
  char *tmp_fme;
  char *tmp_res;
  char init = 0;

  while (m_apm_compare(min,max)!=1)
  {
    memset(tmp_expr,0,strlen(arg1)+1);
    free(tmp_i_str);
    tmp_i_str = m_apm_to_fixpt_stringexp(-1,min,'.',0,0);
    strcpy(tmp_expr,arg1);
    tmp_fme = strreplace(tmp_expr,"x",tmp_i_str);
    tmp_res = hc_result_(tmp_fme);
    free(tmp_fme);
    if (tmp_res == NULL)
    {
      free(tmp_expr);
      free(tmp_i_str);
      free(arg1);
      m_apm_free(min); m_apm_free(max); m_apm_free(res_re); m_apm_free(res_im); m_apm_free(copy_tmp_re); m_apm_free(copy_tmp_im); m_apm_free(copy_tmp2_re); m_apm_free(copy_tmp2_im);
      return FAIL;
    }
    m_apm_copy(copy_tmp_re,res_re);
    m_apm_copy(copy_tmp_im,res_im);
    char *tmp_res_re = hc_real_part(tmp_res);
    char *tmp_res_im = hc_imag_part(tmp_res);
    m_apm_set_string(copy_tmp2_re,tmp_res_re);
    if (tmp_res_im)
      m_apm_set_string(copy_tmp2_im,tmp_res_im);
    else
      m_apm_set_string(copy_tmp2_im,"0");
    if (init)
      m_apmc_multiply(res_re,res_im,copy_tmp_re,copy_tmp_im,copy_tmp2_re,copy_tmp2_im);
    else
    {
      m_apm_copy(res_re,copy_tmp2_re);
      m_apm_copy(res_im,copy_tmp2_im);
      init = 1;
    }
    free(tmp_res); free(tmp_res_re); if (tmp_res_im) free(tmp_res_im);
    m_apm_copy(copy_tmp_re,min);
    m_apm_add(min,copy_tmp_re,MM_One);
  }

  free(arg1);
  free(tmp_expr);
  free(tmp_i_str);
  m_apm_copy(result_re,res_re);
  m_apm_copy(result_im,res_im);
  m_apm_free(min); m_apm_free(max); m_apm_free(res_re); m_apm_free(res_im); m_apm_free(copy_tmp_re); m_apm_free(copy_tmp_im); m_apm_free(copy_tmp2_re); m_apm_free(copy_tmp2_im);
  return SUCCESS;
}


int hc_gamma(M_APM result_re, M_APM result_im, char *e)
{
  char *arg = hc_get_arg_r(e,1);
  if (!arg || !is_num(arg))
    arg_error("gamma() needs a number argument");

  // This code uses the Spouge approximation method to calculate the Gamma function
  // First of all, we check whether it is necessary to use the reflection formula:
  //  pi/(sin(pi*z)*gamma(1 - z))
  char *real = hc_real_part(arg);
  char *imag = hc_imag_part(arg);
  if (real[0] == '-') { // -> real part is negative, reflection formula needs to be used
    if (!imag && is_int(real))
    {
      free(real);
      free(arg);
      arg_error("gamma() : complex infinity result");
    }
    free(real);
    free(imag);
    char *r_expr = malloc(strlen("pi/(sin(pi*)*gamma(1-))") + 2 * strlen(arg) + 1); if (!r_expr) mem_error();
    sprintf(r_expr, "pi/(sin(pi*%s)*gamma(1-%s))", arg, arg);
    char *r = hc_result_(r_expr);
    free(r_expr);
    free(arg);
    if (!r)
      return FAIL;
    else {
      hc_set_from_string(result_re, result_im, r);
      free(r);
      return SUCCESS;
    }
  }
  free(real);
  free(imag);

  char *argMone_expr = malloc(strlen(arg)+3); if (!argMone_expr) mem_error();
  sprintf(argMone_expr,"%s-1",arg);
  char *argMone = hc_result_(argMone_expr);
  free(argMone_expr);
  if (!argMone)
  {
    free(arg);
    return FAIL;
  }

  // If this is reached, reflection formula is not used.
  // First, the parameter is calculated from the desired precision using the formula : parameter(n) = ceil(n/log10(2pi))
  char *par_expr = malloc(hc_need_space_int(hc.precision) + strlen("ceil(/log10(2pi))") + 1); if (!par_expr) mem_error();
  sprintf(par_expr,"ceil(%i/log10(2pi))",hc.precision);
  char *par = hc_result_(par_expr);
  free(par_expr);
  if (!par)
  {
    free(arg); free(argMone);
    return FAIL;
  }

  // Second, the coefficients cache is prepared, if necessary
  // The format for the caching string (coefficients) is coef_1:...:coef_n
  if (!gamma_coefficients || hc.precision != gamma_coefficients_lp)
  {
    free(gamma_coefficients);
    gamma_coefficients = malloc(1);
    gamma_coefficients[0] = '\0';
    gamma_coefficients_lp = hc.precision;
  }

  // Then, the series are computed, computing & caching additional coefficients as necessary
  char *coef = NULL;
  unsigned int coef_idx = 0;
  unsigned int k = 1;
  unsigned int last_k = atoi(par) - 1;
  char *series_expr = malloc(2); if (!series_expr) mem_error();
  strcpy(series_expr, "1");
  while (k <= last_k)
  {
    coef = gamma_coefficients + coef_idx;
    if (coef[0] == ':')
    {
      coef_idx += 1;
      coef = gamma_coefficients + coef_idx;
    }
    if (coef[0] == '\0')
    {
      // Generate the kth coefficient
      char *c_expr = malloc(strlen("(2pi)^(-1/2)*(-1)^(-1)/(-1)!*(-+)^(-1/2)*exp(-+)") + 5*hc_need_space_int(k) + 2*strlen(par) + 1); if (!c_expr) mem_error();
      sprintf(c_expr,"(2pi)^(-1/2)*(-1)^(%i-1)/(%i-1)!*(-%i+%s)^(%i-1/2)*exp(-%i+%s)",k,k,k,par,k,k,par);
      char *c_new = hc_result_(c_expr);
      free(c_expr);
      if (!c_new)
      {
        free(series_expr); free(arg); free(argMone); free(par);
        return FAIL;
      }
      gamma_coefficients = realloc(gamma_coefficients, strlen(gamma_coefficients) + 1 + strlen(c_new) + 1); if (!gamma_coefficients) mem_error();
      if (gamma_coefficients[0] != '\0')
        strcat(gamma_coefficients,":");
      strcat(gamma_coefficients,c_new);
      free(c_new);
      coef = gamma_coefficients + coef_idx;
      if (coef[0] == ':') {
        coef_idx++;
        coef = gamma_coefficients + coef_idx;
      }
    }
    unsigned int i=0;
    while (coef[i] && coef[i]!=':')
      i++;
    char *el = malloc(strlen("/(+)") + i + strlen(arg) + hc_need_space_int(k) + 1); if (!el) mem_error();
    strncpy(el,coef,i);
    sprintf(el + i,"/(%s+%i)",argMone,k);
    char *el_r = hc_result_(el);
    free(el);
    if (!el_r)
    {
      free(series_expr); free(arg); free(argMone); free(par);
      return FAIL;
    }
    series_expr = realloc(series_expr,strlen(series_expr)+1+strlen(el_r)+1); if (!series_expr) mem_error();
    strcat(series_expr,"+");
    strcat(series_expr,el_r);
    free(el_r);
    coef_idx += i;
    k++;
  }

  char *series_result = hc_result_(series_expr);
  free(series_expr);
  if (!series_result)
  {
    free(par); free(arg); free(argMone);
    return FAIL;
  }

  // Finally, we return the result according to the formula (where z is the argument):
  // (z-1+par)^(z-1/2)*exp(-z+1-par)*sqrt(2pi)*series_result
  char *r_expr = malloc(strlen("(-1+)^(-1/2)*exp(-()+1-())*sqrt(2pi)*") + 3*strlen(arg) + 2*strlen(par) + strlen(series_result) + 1); if (!r_expr) mem_error();
  sprintf(r_expr,"(%s-1+%s)^(%s-1/2)*exp(-(%s)+1-(%s))*sqrt(2pi)*%s",arg,par,arg,arg,par,series_result);
  char *r = hc_result_(r_expr);
  free(r_expr); free(par); free(series_result); free(arg); free(argMone);
  if (!r)
    return FAIL;
  else {
    hc_set_from_string(result_re, result_im, r);
    free(r);
    return SUCCESS;
  }
}


int hc_gcd(M_APM result, char *e)
{
  if (strchr(e,',')==NULL)
    arg_error("gcd() needs two arguments.");

  /* 2-arg INIT */
  char *arg1,*arg2;
  arg1 = hc_get_arg_r(e,1);  arg2 = hc_get_arg_r(e,2);
  if (arg1 == NULL || arg2 == NULL)
    return FAIL;
  /* 2-arg INIT END */
  M_APM a,b;
  a = m_apm_init(); m_apm_set_string(a,arg1);
  b = m_apm_init(); m_apm_set_string(b,arg2);
  m_apm_gcd(result,a,b);

  m_apm_free(a); m_apm_free(b); free(arg1); free(arg2);
  return SUCCESS;
}


int hc_lcm(M_APM result, char *e)
{
  if (strchr(e,',')==NULL)
    arg_error("lcm() needs two arguments.");

  /* 2-arg INIT */
  char *arg1,*arg2;
  arg1 = hc_get_arg_r(e,1);  arg2 = hc_get_arg_r(e,2);
  if (arg1 == NULL || arg2 == NULL)
    return FAIL;
  /* 2-arg INIT END */
  M_APM a,b;
  a = m_apm_init(); m_apm_set_string(a,arg1);
  b = m_apm_init(); m_apm_set_string(b,arg2);
  m_apm_lcm(result,a,b);

  m_apm_free(a); m_apm_free(b); free(arg1); free(arg2);
  return SUCCESS;
}


int hc_fibo(M_APM result, char *e)
{
  if (strlen(e)==0)
    arg_error("fibo() needs an argument.");
  char *fme = hc_result_(e);
  if (fme == NULL)
    return FAIL;
  char *ipart = hc_imag_part(fme);
  if (ipart)
  {
    free(ipart); free(fme);
    arg_error("fibo() : argument must be a strictly positive integer");
  }
  M_APM target = m_apm_init();
  M_APM curr = m_apm_init();
  M_APM curr2 = m_apm_init();
  M_APM a = m_apm_init();
  M_APM b = m_apm_init();
  M_APM c = m_apm_init();
  m_apm_set_string(target,fme);
  free(fme);

  if (!m_apm_is_integer(target))
    arg_error("fibo() : argument must be a strictly positive integer");

  if (m_apm_compare(MM_Zero,target) != -1)
    arg_error("fibo() : argument must be a strictly positive integer");

  if (m_apm_compare(MM_One,target) == 0)
  {
    m_apm_free(target);
    m_apm_free(curr);
    m_apm_free(curr2);
    m_apm_free(a);
    m_apm_free(b);
    m_apm_set_string(result,"1");
    m_apm_free(c);
    return SUCCESS;
  }

  m_apm_set_string(curr,"1");
  m_apm_set_string(a,"0");
  m_apm_set_string(b,"1");

  while (m_apm_compare(target, curr) != 0)
  {
    m_apm_add(c,a,b);
    m_apm_copy(a,b);
    m_apm_copy(b,c);
    m_apm_add(curr2,curr,MM_One);
    m_apm_copy(curr,curr2);
  }

  m_apm_free(target);
  m_apm_free(curr);
  m_apm_free(curr2);
  m_apm_free(a);
  m_apm_free(b);
  m_apm_copy(result,c);
  m_apm_free(c);

  return SUCCESS;
}


int hc_f2c(M_APM result, char *e)
{
  if (strlen(e)==0)
    arg_error("ftoc() needs an argument.");
  char *fme = hc_result_(e);
  if (fme == NULL) return FAIL;
  char *ipart = hc_imag_part(fme);
  if (ipart)
  {
    free(ipart); free(fme);
    arg_error("ftoc() : argument must be a real number.");
  }
  M_APM f = m_apm_init();
  M_APM x = m_apm_init();
  m_apm_set_string(f,fme);
  m_apm_set_string(x,"32");
  m_apm_subtract(result,f,x);
  
  m_apm_set_string(x,"5");
  m_apm_multiply(f,x,result);

  m_apm_set_string(x,"9");
  m_apm_divide(result,HC_DEC_PLACES,f,x);

  free(fme); m_apm_free(f); m_apm_free(x);
  return SUCCESS;
}


int hc_c2f(M_APM result, char *e)
{
  if (strlen(e)==0)
    arg_error("ctof() needs an argument.");
  char *fme = hc_result_(e);
  if (fme == NULL)
    return FAIL;
  char *ipart = hc_imag_part(fme);
  if (ipart)
  {
    free(ipart); free(fme);
    arg_error("ctof() : argument must be a real number.");
  }
  M_APM tmp = m_apm_init();
  M_APM x = m_apm_init();
  m_apm_set_string(tmp,fme);
  m_apm_set_string(x,"9");
  m_apm_multiply(result,x,tmp);
  
  m_apm_set_string(x,"5");
  m_apm_divide(tmp,HC_DEC_PLACES,result,x);

  m_apm_set_string(x,"32");
  m_apm_add(result,x,tmp);

  free(fme); m_apm_free(tmp); m_apm_free(x);
  return SUCCESS;
}


int hc_c2k(M_APM result, char *e)
{
  if (strlen(e)==0)
    arg_error("ctok() needs an argument.");
  char *fme = hc_result_(e);
  if (fme == NULL)
    return FAIL;
  char *ipart = hc_imag_part(fme);
  if (ipart)
  {
    free(ipart); free(fme);
    arg_error("ctok() : argument must be a real number.");
  }
  M_APM tmp = m_apm_init();
  M_APM x = m_apm_init();

  m_apm_set_string(tmp,fme);
  m_apm_set_string(x,"273.15");
  m_apm_add(result,x,tmp);
  m_apm_set_string(x,"-273.15");
  if (m_apm_compare(tmp,x)==-1)
    warning("ctok() : input value shouldn't be lower than -273.15 degrees Celsius (absolute zero)");
  m_apm_free(tmp);
  m_apm_free(x);
  free(fme);
  return SUCCESS;
}


int hc_k2c(M_APM result, char *e)
{
  if (strlen(e)==0)
    arg_error("ktoc() needs an argument.");
  char *fme = hc_result_(e);
  if (fme == NULL)
    return FAIL;
  char *ipart = hc_imag_part(fme);
  if (ipart)
  {
    free(ipart); free(fme);
    arg_error("ktoc() : argument must be a real number.");
  }
  M_APM tmp = m_apm_init();
  M_APM x = m_apm_init();

  m_apm_set_string(tmp,fme);
  m_apm_set_string(x,"273.15");
  m_apm_subtract(result,tmp,x);
  m_apm_set_string(x,"-273.15");
  if (m_apm_compare(tmp,MM_Zero)==-1)
    warning("ktoc() : input value shouldn't be lower than zero");
  m_apm_free(tmp);
  m_apm_free(x);
  free(fme);
  return SUCCESS;
}


int hc_f2k(M_APM result, char *e)
{
  if (strlen(e)==0)
    arg_error("ftok() needs an argument.");
  if (hc_f2c(result,e) == FAIL)
    return FAIL;
  m_apm_to_string(e,HC_DEC_PLACES,result);
  if (hc_c2k(result,e) == FAIL)
    return FAIL;
  return SUCCESS;
}


int hc_k2f(M_APM result, char *e)
{
  if (strlen(e)==0)
    arg_error("ktof() needs an argument.");
  if (hc_k2c(result,e) == FAIL)
    return FAIL;
  m_apm_to_string(e,HC_DEC_PLACES,result);
  if (hc_c2f(result,e) == FAIL)
    return FAIL;
  return SUCCESS;
}


int hc_char2code(M_APM result, char *e)
{
  e = hc_result_(e);
  if (!e)
    return FAIL;
  char *fme = get_string(e);
  if (!fme || strlen(fme)!=1)
  {
    free(e); free(fme);
    arg_error("chartocode() : argument must be a one-character string.");
  }

  m_apm_set_long(result,(long)fme[0]);

  free(e); free(fme);
  return SUCCESS;
}


char *hc_code2char(char *e)
{
  e = hc_result_(e);
  if (!e)
    return FAIL;
  if (!is_int(e))
  {
    free(e);
    arg_error("codetochar() : argument must be an integer in the range 0-255 inclusive.");
  }
  errno = 0;
  int charcode = strtol(e,(char **) NULL,10);
  free(e);
  if (errno != 0 || charcode < 0 || charcode >= 256)
  {
    arg_error("codetochar() : argument must be an integer in the range 0-255 inclusive.");
  }

  char *r = malloc(4);
  if (!r)
    mem_error();

  sprintf(r,"\"%c\"",charcode);

  return r;
}


// Source : http://algorithmist.com/index.php/Euler's_Phi_function, modified to be in high precision
int hc_totient(M_APM result, char *e)
{
  if (strlen(e)==0)
    arg_error("totient() needs an argument.");
  char *fme = hc_result_(e);
  if (fme == NULL)
    return FAIL;

  char *ipart = hc_imag_part(fme);
  if (ipart)
  {
    free(ipart);
    free(fme);
    arg_error("totient() : argument has to be a positive integer.");
  }

  M_APM i,j,copy_tmp,copy_tmp2;
  i = m_apm_init();
  m_apm_set_string(i,fme);
  free(fme);

  if ((m_apm_compare(i,MM_Zero)!=1) || (!m_apm_is_integer(i)))
  {
    m_apm_free(i);
    arg_error("totient() : argument has to be a positive integer.");
  }

  j = m_apm_init();

  if (m_apm_compare(i,MM_One)==0)
  {
    m_apm_set_string(result,"1");
    m_apm_free(i); m_apm_free(j);
    return SUCCESS;
  }

  copy_tmp = m_apm_init();
  copy_tmp2 = m_apm_init(); 
  m_apm_copy(result,i);

  /* Check for divisibility by every prime number below the square root. */
  /* Start with 2. */
  if (m_apm_is_even(i))
  {
    m_apm_divide(copy_tmp,HC_DEC_PLACES,result,MM_Two);
    m_apm_copy(copy_tmp2,result);
    m_apm_subtract(result,copy_tmp2,copy_tmp);
    do {
      m_apm_copy(copy_tmp,i);
      m_apm_divide(i,HC_DEC_PLACES,copy_tmp,MM_Two);
    }
    while (m_apm_is_even(i)) ;
  }
  
  /* Since this doesn't use a list of primes, check every odd number. Ideally, skip past composite numbers.*/
  m_apm_set_string(j,"3");
  m_apm_integer_pow(copy_tmp,HC_DEC_PLACES,j,2);
  while (m_apm_compare(copy_tmp,i)!=1)
  {
    m_apm_integer_div_rem(copy_tmp2,copy_tmp,i,j);
    if (m_apm_sign(copy_tmp)==0)
    {
      m_apm_divide(copy_tmp,HC_DEC_PLACES,result,j);
      m_apm_copy(copy_tmp2,result);
      m_apm_subtract(result,copy_tmp2,copy_tmp);
      do {
        m_apm_copy(copy_tmp,i);
        m_apm_divide(i,HC_DEC_PLACES,copy_tmp,j);
        m_apm_integer_div_rem(copy_tmp2,copy_tmp,i,j);
      }
      while (m_apm_sign(copy_tmp)==0) ;
    }
    m_apm_copy(copy_tmp,j);
    m_apm_add(j,copy_tmp,MM_Two);
    m_apm_integer_pow(copy_tmp,HC_DEC_PLACES,j,2);
  }
  
  /* If i>1, then it's the last factor at this point. */
  if (m_apm_compare(i,MM_One)==1)
  {
    m_apm_divide(copy_tmp,HC_DEC_PLACES,result,i);
    m_apm_copy(copy_tmp2,result);
    m_apm_subtract(result,copy_tmp2,copy_tmp);
  }
  
  m_apm_free(i); m_apm_free(j); m_apm_free(copy_tmp); m_apm_free(copy_tmp2);

  return SUCCESS;
}


int hc_atomicnumber(M_APM result, char *e)
{
  char *tmp_e = hc_result_(e);
  if (!tmp_e || !(e = get_string(tmp_e)))
  {
    free(tmp_e);
    arg_error("atomicnumber() : argument has to be a string.");
  }

  free(tmp_e);

  double Z = hc_get_element_info(e, ATOMIC_NUMBER);
  free(e);
  if (!Z)
  {
    return FAIL;
  } else {
    m_apm_set_double(result, Z);
    return SUCCESS;
  }
}


int hc_mmass(M_APM result, char *e)
{
  char *tmp_e = hc_result_(e);
  if (!tmp_e || !(e = get_string(tmp_e)))
  {
    free(tmp_e);
    arg_error("mmass() : argument has to be a string.");
  }

  free(tmp_e);

  if (!check_completeness(e))
  {
    free(e);
    arg_error("mmass() : syntax error.");
  }


  double res = 0;
  double tmp = 0;
  unsigned int i=0;

  for (; i<strlen(e); i++)
  {
    if (e[i]=='(')
    {
      tmp = hc_mmass_par(e, &i);
      if (!tmp)
      {
        free(e);
        return FAIL;
      }
      res += tmp;
    }
    if (isupper(e[i]))
    {
      tmp = hc_mmass_el(e, &i);
      if (!tmp)
      {
        free(e);
        return FAIL;
      }
      res += tmp;
    }
  }

  free(e);
  m_apm_set_double(result,res);
  return SUCCESS;
}



char hc_reim(char type, M_APM re, M_APM im, char *e)
{
  e = hc_result_(e);
  if (!e || !is_num(e))
  {
    free(e);
    return FAIL;
  }
  char *tmp;
  m_apm_copy(im,MM_Zero);
  if (type == REAL)
  {
    tmp = hc_real_part(e);
    m_apm_set_string(re,tmp);
  } else {
    tmp = hc_imag_part(e);
    if (tmp)
      m_apm_set_string(re,tmp);
    else
      m_apm_copy(re,MM_Zero);
  }
  free(tmp); free(e);
  return SUCCESS;
}



void hc_from_rad(M_APM result)
{
  if (hc.angle=='r')
    // RAD
    return;

  M_APM tmp = m_apm_init();
  M_APM tmp2 = m_apm_init();
  if (hc.angle=='d')
  {
    // DEG
    m_apm_set_string(tmp2,"180");
    m_apm_multiply(tmp,result,tmp2);
    m_apm_divide(tmp2,HC_DEC_PLACES+1,tmp,MM_PI); // avoid getting imprecise results (for example 29.999... for asin(.5))
    m_apm_round(result,HC_DEC_PLACES,tmp2);
    m_apm_free(tmp);
    m_apm_free(tmp2);
    return;
  } else {
    // GRAD
    m_apm_set_string(tmp2,"200");
    m_apm_multiply(tmp,result,tmp2);
    m_apm_divide(tmp2,HC_DEC_PLACES+1,tmp,MM_PI);
    m_apm_round(result,HC_DEC_PLACES,tmp2);
    m_apm_free(tmp);
    m_apm_free(tmp2);
    return;
  }
}


void hc_to_rad(M_APM result)
{
  if (hc.angle=='r')
    // RAD
    return;

  M_APM tmp = m_apm_init();
  M_APM tmp2 = m_apm_init();

  if (hc.angle=='d')
  {
    // DEG
    m_apm_set_string(tmp2,"180");
    m_apm_multiply(tmp,result,MM_PI);
    m_apm_divide(result,HC_DEC_PLACES,tmp,tmp2);
    m_apm_free(tmp); m_apm_free(tmp2);
  } else {
    // GRAD
    m_apm_set_string(tmp2,"200");
    m_apm_multiply(tmp,result,MM_PI);
    m_apm_divide(result,HC_DEC_PLACES,tmp,tmp2);
    m_apm_free(tmp); m_apm_free(tmp2);
  }
}


int hc_need_space_int(int i)
{
  // Returns the needed space WITHOUT the terminating NULL byte, but WITH the usually prepended '-' or '+'
  if (!i)
    return 2;

  unsigned int j=1;
  int space = 1;
  while (j<=abs(i))
  {
    space++;
    j*=10;
  }
  return space;
}


char *hc_2sci(char *n)
{
  if (!n)
    return NULL;
  char *tmp = hc_strip_0s(n);
  free(n);
  n = tmp;
  char *r;
  M_APM _n_ = m_apm_init(); m_apm_set_string(_n_,n);
  int exp = m_apm_exponent(_n_);
  m_apm_free(_n_);
  if ((n[0]=='-' && n[1]=='0') || n[0]=='0')
  {
    r = malloc(strlen(n)+1+hc_need_space_int(exp)+1);
    if (!r)
      mem_error();
    unsigned int i = 1, j=2;
    while (n[i]=='0' || n[i]=='-' || n[i]=='.')
      i++;
    if (n[i] == '\0')
      i--;
    if (n[0]=='-')
    {
      r[0] = '-';
      r[1] = n[i];
      r[2] = '.';
    } else {
      r[0] = n[i];
      r[1] = '.';
    }
    if (n[0]=='-')
      j++;
    if (n[i]!='\0')
      i++;
    char digits = 0;
    while (n[i]!='\0')
    {
      r[j++] = n[i++];
      digits = 1;
    }
    if (!digits)
      j--;
    r[j] = 0;
    r = hc_strip_0s_real(r);
    strcat(r,"E");
    char *_exp_ = malloc(hc_need_space_int(exp)+1);
    sprintf(_exp_,"%i",exp);
    strcat(r,_exp_);
    free(_exp_);
    free(n);
    return r;
  } else {
    r = malloc(strlen(n)+1+hc_need_space_int(exp)+1);
    if (!r)
      mem_error();
    unsigned int i=1,j=2;
    if (n[0]=='-')
    {
      r[0] = '-';
      r[1] = n[1];
      r[2] = '.';
    } else {
      r[0] = n[0];
      r[1] = '.';
    }
    if (n[0]=='-')
    {
      i++; j++;
    }
    char digits = 0;
    while (n[i]!='\0')
    {
      digits = 1;
      if (n[i]=='.')
        i++;
      r[j++] = n[i++];
    }
    if (!digits)
      j--;
    r[j] = 0;
    r = hc_strip_0s_real(r);
    strcat(r,"E");
    char *_exp_ = malloc(hc_need_space_int(exp)+1);
    sprintf(_exp_,"%i",exp);
    strcat(r,_exp_);
    free(_exp_);
    free(n);
    return r;
  }
}


char *hc_2eng(char *n)
{
  if (!n)
    return NULL;
  M_APM _n_ = m_apm_init(); m_apm_set_string(_n_,n);
  int exp = m_apm_exponent(_n_);
  m_apm_free(_n_);
  char *r = hc_2sci(n);
  char *tmp,*x;

  switch (exp % 3)
  {
  case 0:
    return r;
    break;

  case -2:
  case 1:
    // take two numbers, then add exponent -1
    if (r[0]=='-')
    {
      if (isdigit(r[3]))
      {
        r[2] = r[3];
        if (isdigit(r[4]))
          r[3] = '.';
        else
          memmove(r+3,r+4,strlen(r)-4);
      }
      else
      {
        memmove(r+3,r+2,strlen(r)-2);
        r[2] = '0';
      }
    } else {
      if (isdigit(r[2]) && (isdigit(r[1]) || r[1] == '.'))
      {
        r[1] = r[2];
        if (isdigit(r[3]))
          r[2] = '.';
        else
          memmove(r+2,r+3,strlen(r)-3);
      }
      else
      {
        memmove(r+2,r+1,strlen(r)-1);
        r[1] = '0';
      }
    }
    x = malloc(hc_need_space_int(exp-1)+1);
    sprintf(x,"%i",exp-1);
    r[strchr(r,'E')-r+1] = 0;
    tmp = malloc(strlen(x)+strlen(r)+1);
    strcpy(tmp,r);
    strcat(tmp,x);
    free(r); free(x);
    return tmp;
    break;

  case 2:
  case -1:
    // take three numbers, then add exponent -2
    if (r[0]=='-')
    {
      if (r[2]=='.' && isdigit(r[3]) && isdigit(r[4]))
      {
        r[2] = r[3];
        r[3] = r[4];
        if (isdigit(r[5]))
          r[4] = '.';
        else
          memmove(r+4,r+5,strlen(r)-5);
      }
      else
      {
        if (r[2]=='.' && isdigit(r[3]))
        {
          r[2] = r[3];
          r[3] = '0';
        } else {
          memmove(r+4,r+2,strlen(r)-2);
          r[2] = '0';
          r[3] = '0';
        }
      }
    } else {
      if (r[1]=='.' && isdigit(r[2]) && isdigit(r[3]))
      {
        r[1] = r[2];
        r[2] = r[3];
        if (isdigit(r[4]))
          r[3] = '.';
        else
          memmove(r+3,r+4,strlen(r)-4);
      }
      else
      {
        if (r[1]=='.' && isdigit(r[2]))
        {
          r[1] = r[2];
          r[2] = '0';
        } else {
          memmove(r+3,r+1,strlen(r)-1);
          r[1] = '0';
          r[2] = '0';
        }
      }
    }
    x = malloc(hc_need_space_int(exp-2)+1);
    sprintf(x,"%i",exp-2);
    r[strchr(r,'E')-r+1] = 0;
    tmp = malloc(strlen(x)+strlen(r)+1);
    strcpy(tmp,r);
    strcat(tmp,x);
    free(r); free(x);
    return tmp;
    break;
  }

  return NULL;
}


// Use to strip zeros from a real number (in place)
char *hc_strip_0s_real(char *e)
{
  if (strchr(e,'.'))
  {
    int i = strlen(e)-1;
    while(i && e[i] == '0')
      e[i--]=0;
    if (e[i] == '.')
      e[i]=0;
  }
  return e;
}


char *hc_strip_0s(char *e)
{
  char *re,*im;
  re = hc_real_part(e);
  im = hc_imag_part(e);
  int i;
  if (strchr(re,'.'))
  {
    i = strlen(re)-1;
    while (re[i]=='0')
      re[i--]=0;
    if (re[i]=='.')
      re[i]=0;
  }
  if (im)
  {
    if (strchr(im,'.'))
    {
      i = strlen(im)-1;
      while (im[i]=='0')
        im[i--]=0;
      if (im[i]=='.')
        im[i]=0;
    }
  }
  i = strlen(re)+1;
  if (im)
    i += strlen(im)+1;
  char *r = malloc(i);
  if (!r)
    mem_error();
  strcpy(r,re);
  if (im)
  {
    strcat(r,"i");
    strcat(r,im);
  }
  free(re); free(im);
  return r;
}


// Returns a version of r ready to output to screen.
// Frees r.
char *hc_format_vec(char *r) {
  r[strlen(r)-1] = '\0';
  char *r_new = malloc(2);
  if (!r_new)
    mem_error();
  strcpy(r_new,"[");
  long idx = 1;
  char *curarg = hc_get_arg((char *)(r + sizeof(char)),idx);
  while (curarg)
  {
    if (is_num(curarg))
    {
      char *tmp = curarg;
      curarg = hc_strip_0s(curarg);
      free(tmp);
    } else if (is_vector(curarg)) {
      curarg = hc_format_vec(curarg);
    }
    r_new = realloc(r_new, strlen(r_new)+1+strlen(curarg)+1);
    strcat(r_new,curarg);
    strcat(r_new,",");
    free(curarg);
    curarg = hc_get_arg((char *)(r + sizeof(char)),++idx);
  }
  r_new[strlen(r_new)-1] = ']';
  free(r);
  return r_new;
}


// Returns a version of r ready to output to screen.
// Frees r.
// if save_as_last_result is TRUE, saves r as the last result
char *hc_format_num(char *r, char save_as_last_result)
{
  char *r_re = hc_real_part(r);
  char *r_im = hc_imag_part(r);
  free(r);
  if (save_as_last_result)
  {
    m_apm_set_string(hc_lans_mapm_re,r_re);
    if (r_im)
      m_apm_set_string(hc_lans_mapm_im,r_im);
    else
      m_apm_set_string(hc_lans_mapm_im,"0");
  }
  if (!r_im)
  {
    r = r_re;
    int i=strlen(r)-1;
    while (i && r[i]=='0')
      r[i--]=0;
    if (i && r[i]=='.')
      r[i]=0;
    switch (hc.exp)
    {
    case 's':
      r = hc_2sci(r);
      break;

    case 'e':
      r = hc_2eng(r);
      break;

    default:
      break;
    }
  } else {
    int i=strlen(r_re)-1;
    while (r_re[i]=='0')
      r_re[i--]=0;
    if (r_re[i]=='.')
      r_re[i]=0;
    
    i = strlen(r_im)-1;
    while (r_im[i]=='0')
      r_im[i--]=0;
    if (r_im[i]=='.')
      r_im[i]=0;
    
    switch (hc.exp)
    {
    case 's': // SCI
      r_re = hc_2sci(r_re);
      r_im = hc_2sci(r_im);
      break;
    
    case 'e': // ENG
      r_re = hc_2eng(r_re);
      r_im = hc_2eng(r_im);
      break;
    
    default:
      break;
    }
    
    r = malloc(strlen(r_re)+1+strlen(r_im)+1);
    // r_re i r_im \0
    strcpy(r,r_re);
    strcat(r,"i");
    strcat(r,r_im);
    free(r_re); free(r_im);
  }
  return r;
}


int hc_rand(M_APM res, char *f_expr)
{
  char *t1 = hc_get_arg(f_expr,1);
  char *tmp;
  if (!t1)
    tmp = NULL;
  if (t1)
  {
    if (strlen(t1)==0)
    {
      free(t1);
      tmp = NULL;
    } else {
      tmp = hc_result_(t1);
      free(t1);
      if (!tmp)
        return FAIL;
      t1 = hc_imag_part(tmp);
      if (t1)
      {
        free(t1);
        arg_error("rand() : if provided, argument must be a real, strictly positive integer.");
      }
    }
  }

  if (tmp!=NULL)
  {
    M_APM rand_tmp = m_apm_init();
    M_APM rand_1 = m_apm_init();
    M_APM rand_2 = m_apm_init();
    m_apm_set_string(rand_tmp,tmp);
    if (!m_apm_is_integer(rand_tmp) || m_apm_compare(rand_tmp,MM_Zero)!=1)
    {
      m_apm_free(rand_tmp);
      m_apm_free(rand_1);
      m_apm_free(rand_2);
      arg_error("rand() : if provided, argument must be a real, strictly positive integer.");
    }
    m_apm_get_random(rand_1);
    m_apm_multiply(rand_2,rand_1,rand_tmp);
    m_apm_floor(rand_1,rand_2);
    m_apm_add(res,rand_1,MM_One);
    m_apm_free(rand_tmp); m_apm_free(rand_1); m_apm_free(rand_2); free(tmp);
  } else {
    m_apm_get_random(res);
  }
  return SUCCESS;
}


int hc_log(M_APM re, M_APM im, char *e)
{
  char *base = hc_get_arg_r(e,1);
  char *x = hc_get_arg_r(e,2);
  if (!base || !x || !is_num(base) || !is_num(x))
  {
    free(base); free(x);
    arg_error("log() : two number arguments (base, x) are required");
  }
  char *r_expr = malloc(strlen(base)+strlen(x)+8+1+1); // ln(x)/ln(base)
  if (!r_expr) mem_error();
  sprintf(r_expr, "ln(%s)/ln(%s)", x, base);
  char *r = hc_result_(r_expr);
  free(r_expr); free(base); free(x);
  if (!r)
    return FAIL;
  else
  {
    hc_set_from_string(re, im, r);
    return SUCCESS;
  }
}


int hc_dotp(M_APM re, M_APM im, char *f_expr)
{
  char *v1_orig = hc_get_arg(f_expr,1);
  char *v2_orig = hc_get_arg(f_expr,2);
  if (!v1_orig || !v2_orig || !is_vector(v1_orig) || !is_vector(v2_orig))
  {
    free(v1_orig); free(v2_orig);
    arg_error("dotp() needs 2 arguments (v1, v2).");
  }
  char *v1 = list_clean(v1_orig);
  char *v2 = list_clean(v2_orig);

  long idx = 1;
  char *curarg1 = hc_get_arg(v1,idx);
  char *curarg2 = hc_get_arg(v2,idx);
  char *res = malloc(1);
  res[0] = '\0';
  while (curarg1 && curarg2)
  {
    char *tmp = malloc(strlen(res)+1+strlen(curarg1)+1+strlen(curarg2)+1);
    if (!tmp)
      mem_error();
    if (strlen(res))
      sprintf(tmp,"%s+%s*%s",res,curarg1,curarg2);
    else
      sprintf(tmp,"%s*%s",curarg1,curarg2);
    char *tmpres = hc_result_(tmp);
    if (!tmpres)
    {
      free(tmp); free(res); free(curarg1); free(curarg2); free(v1_orig); free(v2_orig);
      return FAIL;
    }
    free(tmp);
    free(res);
    res = tmpres;
    free(curarg1);
    free(curarg2);
    curarg1 = hc_get_arg(v1,++idx);
    curarg2 = hc_get_arg(v2,idx);
  }

  free(v1_orig);
  free(v2_orig);

  if (curarg1 || curarg2)
  {
    free(curarg1); free(curarg2); free(res);
    arg_error("dotp() : vectors must be of equal length.");
  }

  char *res_re = hc_real_part(res);
  char *res_im = hc_imag_part(res);
  m_apm_set_string(re,res_re);
  if (res_im)
    m_apm_set_string(im,res_im);

  free(res);
  free(res_re);
  free(res_im);

  return SUCCESS;
}


char *hc_crossp(char *f_expr)
{
  char *v1_orig = hc_get_arg(f_expr,1);
  char *v2_orig = hc_get_arg(f_expr,2);
  if (!v1_orig || !v2_orig || !is_vector(v1_orig) || !is_vector(v2_orig))
  {
    free(v1_orig); free(v2_orig);
    arg_error("crossp() needs two 2-dimensional or 3-dimensional vectors as arguments.");
  }
  char *v1 = list_clean(v1_orig);
  char *v2 = list_clean(v2_orig);

  char *v1x = hc_get_arg(v1,1);
  char *v1y = hc_get_arg(v1,2);
  char *v1z = hc_get_arg(v1,3);
  char *v1tmp = hc_get_arg(v1,4);

  char *v2x = hc_get_arg(v2,1);
  char *v2y = hc_get_arg(v2,2);
  char *v2z = hc_get_arg(v2,3);
  char *v2tmp = hc_get_arg(v2,4);

  free(v1_orig); free(v2_orig);

  if (!v1x || !v2x || !v1y || !v2y || v1tmp || v2tmp || (v2z == NULL && v1z != NULL) || (v1z == NULL && v2z != NULL))
  {
    free(v1x); free(v1y); free(v1z); free(v1tmp); free(v2x); free(v2y); free(v2z); free(v2tmp);
    arg_error("crossp() needs two 2-dimensional or 3-dimensional vectors as arguments.");
  }

  unsigned int alloc = strlen("[*-*,*-*,*-*]")+2*strlen(v1x)+2*strlen(v2x)+2*strlen(v1y)+2*strlen(v2y)+1;
  if (v1z)
    alloc+=2*strlen(v1z) + 2*strlen(v2z);
  char *r = malloc(alloc);
  if (!r)
    mem_error();

  if (v1z)
  {
    // 3-dimensional vectors
    sprintf(r,"[%s*%s-%s*%s,%s*%s-%s*%s,%s*%s-%s*%s]",v1y,v2z,v1z,v2y,v1z,v2x,v1x,v2z,v1x,v2y,v1y,v2x);
  } else {
    // 2-dimensional vectors
    sprintf(r,"[0,0,%s*%s-%s*%s]",v1x,v2y,v1y,v2x);
  }

  free(v1x); free(v1y); free(v1z); free(v2x); free(v2y); free(v2z);

  return r;
}


char *hc_join(char *f_expr)
{
  char *v1_orig = hc_get_arg_r(f_expr,1);
  char *v2_orig = hc_get_arg_r(f_expr,2);
  if (!v1_orig || !v2_orig || !is_vector(v1_orig) || !is_vector(v2_orig))
  {
    free(v1_orig); free(v2_orig);
    arg_error("join() needs two vector arguments.");
  }
  char *v1 = list_clean(v1_orig);
  char *v2 = list_clean(v2_orig);
  char *sep = (v1[0]==0||v2[0]==0) ? "" : ",";

  char *r = malloc(strlen(v1)+strlen(sep)+strlen(v2)+3);
  if (!r)
    mem_error();
  sprintf(r,"[%s%s%s]",v1,sep,v2);

  free(v1_orig); free(v2_orig);
  return r;
}


char *hc_quicksort(char *list, char *cmp)
{
  unsigned long idx = 1;
  char *pivot = hc_get_arg(list,idx);
  if (!pivot) // empty list
  {
    char *r = malloc(1);
    r[0] = '\0';
    return r;
  }
  unsigned int pivotlen = strlen(pivot);
  char *larger = malloc(1), *smaller = malloc(1);
  unsigned long largeralloc = 1, smalleralloc = 1;
  larger[0] = smaller[0] = '\0';
  char *cur = hc_get_arg(list,++idx);
  while (cur)
  {
    char *eval = NULL;
    if (!cmp)
    {
      eval = malloc(strlen(cur)+1+pivotlen+1);
      if (!eval) mem_error();
      sprintf(eval, "%s<%s", cur, pivot);
    } else {
      eval = malloc(strlen(cmp)+3+strlen(cur)+pivotlen+1);
      if (!eval) mem_error();
      sprintf(eval, "%s(%s,%s)", cmp, cur, pivot);
    }

    char *evalr = hc_result_(eval);
    free(eval);
    if (!evalr)
    {
      free(cur); free(larger); free(smaller); free(pivot);
      return NULL;
    }

    if ((!cmp && (evalr[0]=='0' && is_positive_int(evalr))) || (cmp && !(evalr[0]=='-' && evalr[1]=='1' && evalr[2]=='.' && is_int(evalr)))) // cur is larger (or equal) to pivot, put it in the 'larger' list
    {
      largeralloc += strlen(cur) + 1;
      larger = realloc(larger, largeralloc);
      if (!larger) mem_error();
      strcat(larger, cur);
      strcat(larger, ",");
    } else { // curres is smaller than the pivot, put it in the 'smaller' list
      smalleralloc += strlen(cur) + 1;
      smaller = realloc(smaller, smalleralloc);
      if (!smaller) mem_error();
      strcat(smaller, cur);
      strcat(smaller, ",");
    }
    free(cur); free(evalr);

    cur = hc_get_arg(list,++idx);
  }

  if (idx == 2) // means that 1 was the last valid; the list contained only one element, pivot; return it
  {
    free(smaller); free(larger);
    return pivot;
  }

  if (strlen(larger))
    larger[strlen(larger) - 1]='\0';
  if (strlen(smaller))
    smaller[strlen(smaller) - 1]='\0';
  char *smallersorted = hc_quicksort(smaller,cmp); free(smaller);
  char *largersorted = hc_quicksort(larger,cmp); free(larger);
  if (!smallersorted || !largersorted)
  {
    free(pivot); free(smallersorted); free(largersorted);
    return NULL;
  }
  char *res = malloc(strlen(smallersorted)+1+pivotlen+1+strlen(largersorted)+1);
  if (!res) mem_error();
  sprintf(res, "%s%s%s%s%s", smallersorted, strlen(smallersorted)>0 ? "," : "",pivot, strlen(largersorted)>0 ? "," : "", largersorted);
  free(pivot); free(smallersorted); free(largersorted);
  return res;
}


char *hc_sort(char *e)
{
  char *sort = hc_get_arg_r(e,1);
  if (!sort || !is_list(sort))
  {
    if (sort)
    {
      free(sort);
      arg_error("sort() : first argument must be a list");
    } else {
      return NULL;
    }
  }
  char *cmp = hc_get_arg(e,2);
  char *tmp = hc_quicksort(list_clean(sort),cmp);
  free(sort); free(cmp);
  if (!tmp)
    return NULL;
  char *result = malloc(1+strlen(tmp)+1+1);
  if (!result) mem_error();
  sprintf(result, "[%s]", tmp);
  free(tmp);
  return result;
}


int hc_input(M_APM re, M_APM im, char *f_expr)
{
  char *str=NULL;
  if (f_expr && strlen(f_expr) && is_string(f_expr))
  {
    str = get_string(f_expr);
    if (!str)
      mem_error();    
  } else {
    str = malloc(strlen("Enter expression : ")+1);
    if (!str)
      mem_error();
    strcpy(str,"Enter expression : ");
  }

  char *res = prompt(str);
  free(str);
  if (!res)
    return FAIL;
  char *tmp = hc_result_(res);
  if (!tmp)
    return FAIL;
  free(res);
  res = tmp;
  char *r_re = hc_real_part(res);
  char *r_im = hc_imag_part(res);
  m_apm_set_string(re,r_re);
  free(r_re);
  if (r_im)
  {
    m_apm_set_string(im,r_im);
        free(r_im);
  } else {
    m_apm_set_string(im,"0");
  }
  free(res);
  return SUCCESS;
}


char *hc_constantlist(char *e, char *el)
{
  char *nstr = hc_get_arg_r(e,1);
  if (!nstr || !strlen(nstr) || !is_positive_int(nstr))
  {
    free(nstr);
    arg_error("zeros()/ones() needs a positive integer as argument");
  }
  errno = 0;
  long int n = strtol(nstr, NULL, 10);
  free(nstr);
  if (errno)
  {
    free(nstr);
    arg_error("zeros()/ones() needs a positive integer as argument");
  }
  char *r = malloc(3 + n * (strlen(el) + 1));
  if (!r) mem_error();
  r[0]='[';
  r[1]='\0';
  long int i = 0;
  for (; i<n; i++)
  {
    strcat(r,el);
    strcat(r,",");
  }
  r[strlen(r)-1] = ']';
  return r;
}


int hc_output(int mode, char *f_expr)
{
  static FILE *fw=NULL;
  static char *fname=NULL;
  int pos;
  if (mode==PRINT)
  {
    pos = 1;
    if (fw)
      fclose(fw);
    fw = NULL;
    free(fname);
    fname = NULL;
  }
  else
  {
    char *fname_tmp = hc_get_arg_r(f_expr,1);
    if (!is_string(fname_tmp))
    {
      free(fname_tmp);
      arg_error("write() : first argument must be a string containing a valid filename.");
    }
    char *tmp = get_string(fname_tmp); 
    free(fname_tmp);
    if (fname && strcmp(tmp,fname)==0)
    {
      free(tmp); // no need to do anything, just reuse the old file descriptor
    } else {
      if (fw)
        fclose(fw);
      fw = fopen(tmp,"a");
      free(fname);
      fname = tmp;
      if (!fw)
      {
        error_nq("write() : Error, can't open file for writing!\n");
        return FAIL;
      }
    }
    pos = 2;
  }
  char *printme = malloc(1);
  printme[0] = '\0';
  int alloc = 0;
  char *t1 = hc_get_arg_r(f_expr,pos);
  char done = 0;
  while (t1)
  {
    done = 1;
    char *tmp;
    if (!t1)
    {
      free(printme);
      return FAIL;
    }
    if (strlen(t1)==0)
    {
      free(t1);
      free(printme);
      return FAIL;
    } else {
      if (is_string(t1))
      {
        tmp = get_string(t1);
      } else {
        tmp = hc_strip_0s(t1);
      }
      free(t1);
      if (!tmp)
      {
        free(printme);
        return FAIL;
      } else {
        t1 = malloc(strlen(tmp)+2);
        strcpy(t1,tmp);
        strcat(t1," ");
        alloc += strlen(t1) + 1;
        printme = realloc(printme,alloc);
        strcat(printme,t1);
        free(t1);
        free(tmp);
      }
    }
    t1 = hc_get_arg_r(f_expr,++pos);
  }
  if (mode==PRINT)
  {
    if (strlen(printme))
      notify_console(printme);
  }
  else
    fwrite(printme,sizeof(char),strlen(printme),fw);
  free(printme);
  if (mode==PRINT)
  {
    notify_console("\n");
  } else if (mode!=PRINT)
  {
    fprintf(fw,"\n");
  }
  if (done)
    return SUCCESS;
  return FAIL;
}


int hc_length_core(M_APM resM, unsigned int *resL, char type, char *e)
{
  e = hc_result_(e);
  if (!e)
    return FAIL;

  if (is_vector(e))
  {
    unsigned int i = 0;
    unsigned int len = 1; // for the last element, which is not followed by a ','
    int par = 0;
    char *tmp = list_clean(e);
    char ignore = FALSE;
    for (; i < strlen(tmp); i++)
    {
      if (tmp[i]=='\"')
      {
        ignore = ignore == FALSE ? TRUE : FALSE;
        continue;
      }
      if (ignore)
        continue;
      if (tmp[i] == '[')
        par++;
      if (tmp[i] == ']')
        par--;
      if (!par && tmp[i] == ',')
        len++;
    }
    if (type == HC_MAPM) {
      m_apm_set_long(resM,len);
    } else if (type == HC_UINT) {
      *resL = len;
    }
    free(e);
  } else if (is_string(e))
  {
    char *tmp = get_string(e);
    free(e);
    if (type == HC_MAPM) {
      m_apm_set_long(resM,(long) strlen(tmp));
    } else if (type == HC_UINT) {
      *resL = (unsigned int) strlen(tmp);
    }
    free(tmp);
  } else {
    free(e);
    arg_error("length() : only strings and vectors have lengths");
  }

  return SUCCESS;
}


int hc_length(M_APM result, char *e)
{
  return hc_length_core(result, NULL, HC_MAPM, e);
}


int hc_count(M_APM result, char *e)
{
  char *list_orig = hc_get_arg_r(e,1);
  char *element = hc_get_arg_r(e,2);
  if (!list_orig || !is_list(list_orig))
  {
    free(element);
    arg_error("count() : first argument needs to be a list");
  }
  if (!element)
  {
    free(list_orig);
    arg_error("count() : second argument (element) required");
  }
  char *list = list_clean(list_orig);

  char *cur = NULL;
  unsigned long int idx = 0;
  long occurences = 0;
  unsigned int elementlen = strlen(element);

  while ((cur = hc_get_arg(list,++idx)) != NULL)
  {
    char *tmpexpr = malloc(elementlen + 2 + strlen(cur) + 1);
    if (!tmpexpr) mem_error();
    sprintf(tmpexpr, "%s==%s", element, cur);
    char *tmpres = hc_result_(tmpexpr);
    free(tmpexpr);
    if (!tmpres)
    {
      free(cur); free(list_orig); free(element);
      return FAIL;
    }
    if (tmpres[0]=='1' && tmpres[1]=='.')
    {
      occurences += 1;
    }
    free(tmpres);
    free(cur);
  }

  free(list_orig);
  free(element);
  m_apm_set_long(result, occurences);
  return SUCCESS;
}


char *hc_range(char *e)
{
  char *a = hc_get_arg_r(e,1);
  if (!a || !is_positive_int(a))
  {
    free(a);
    arg_error("range() : first argument needs to be a positive integer");
  }
  char *b = hc_get_arg_r(e,2);
  if (b && !is_positive_int(b))
  {
    free(b);
    arg_error("hc_range() : second argument needs to be a positive integer");
  }
  unsigned int current = b != NULL ? atoi(a) : 0;
  unsigned int upperbound = b != NULL ? atoi(b) : atoi(a);
  free(a); free(b);
  unsigned long int alloc = 128;
  char *r = malloc(alloc);
  if (!r) mem_error();
  r[0] = r[1] = 0; // need to set r[1] to 0 as well if range(0) is called

  while (current < upperbound)
  {
    sprintf((char *)(r + strlen(r)), ",%u", current++);
    if (strlen(r) + hc_need_space_int(current) + 1 >= alloc)
    {
      alloc += 10 * (hc_need_space_int(current) + 1); // allocate it 10 times to avoid calling this often
      r = realloc(r,alloc);
      if (!r) mem_error();
    }
  }

  r[0] = '[';
  if (strlen(r) + 1 >= alloc)
  {
    r = realloc(r,alloc + 1);
    if (!r) mem_error();
  }
  r[strlen(r)+1] = '\0';
  r[strlen(r)] = ']';
  return r;
}


char hc_get_type(char *e)
{
  if (!e)
  {
    return HC_VAR_INVALID;
  } else if (is_string(e))
  {
    return HC_VAR_STR;
  } else if (is_vector(e))
  {
    return HC_VAR_VEC;
  } else if (is_num(e))
  {
    return HC_VAR_NUM;
  } else if (e && !strlen(e))
  {
    return HC_VAR_EMPTY;
  } else {
    return HC_VAR_INVALID;
  }
}


char *strip_spaces(char *e)
{
  char *r = e;
  while (r[0] && isspace(r[0]))
    r++;
  return r;
}


char last_char(char *e)
{
  char *r = e + strlen(e) - 1;
  if (r <= e)
    return e[0];
  while (r > e && isspace(r[0]))
    r--;
  return r[0];
}


char check_completeness(char *e)
{
  int par=0, par2=0, pos=0;
  char ignore=FALSE;
  for (; e[pos]!='\0'; pos++)
  {
    if (e[pos]=='\"')
    {
      ignore = ignore == FALSE ? TRUE : FALSE;
      continue;
    }
    if (ignore)
      continue;
    if (e[pos]=='(')
      par++;
    if (e[pos]==')')
      par--;
    if (e[pos]=='{')
      par2++;
    if (e[pos]=='}')
      par2--;
  }
  if (par>0 || par2>0)
    return FALSE;
  return TRUE;
}



char *get_string(char *e)
{
  if (!is_string(e))
    return NULL;
  e = strip_spaces(e);
  char *r = malloc(strlen(e)-2+1);
  strncpy(r,(char *)(e+sizeof(char)),strlen(e+sizeof(char))-1);
  r[strlen(e+sizeof(char))-1] = '\0';
  if (strrchr(r,'\"'))
    strrchr(r,'\"')[0] = '\0';
  r = realloc(r,strlen(r)+1);
  return r;
}



char *strchr_outofblock(char *e, char c)
{
  int par=0;
  int par2=0;
  char ignore = FALSE;
  while (e[0])
  {
    if (e[0]=='\"')
      ignore = ignore == FALSE ? TRUE : FALSE;
    if (!ignore)
    {
      if (e[0]=='[')
        par++;
      if (e[0]==']')
        par--;
      if (e[0]=='(')
        par2++;
      if (e[0]==')')
        par2--;
      if (par==0 && par2==0&& e[0]==c)
        return e;
    }
    e++;
  }
  return NULL;
}



char *str_multiply(char *str, M_APM n)
{
  M_APM i = m_apm_init();
  M_APM i2 = m_apm_init();
  m_apm_copy(i,MM_Zero);
  char *r = malloc(2);
  if (!r)
    mem_error();
  strcpy(r,"\"");
  int alloc = 2;
  while (m_apm_compare(i,n)!=0)
  {
    alloc += strlen(str);
    r = realloc(r,alloc);
    if (!r)
      mem_error();
    strcat(r,str);
    m_apm_copy(i2,i);
    m_apm_add(i,i2,MM_One);
  }
  m_apm_free(i);
  m_apm_free(i2);
  r = realloc(r,alloc+1);
  strcat(r,"\"");
  return r;
}


char *hc_tobin(char *e) {
  e = hc_result_(e);
  if (!e)
    return FAIL;
  if (!is_positive_int(e)) {
    free(e);
    arg_error("tobin() only accepts positive integer arguments");
  } else {
    unsigned int tmp = atoi(e);
    unsigned int r_size = 1 + 2 + sizeof(tmp) * 8 + 1 + 1;
    char *r_orig = malloc(r_size);
    char *r = r_orig;
    if (!r) mem_error();

    r += (r_size - 1);
    *r-- = '\0';
    *r-- = '\"';
    for (int i = sizeof(tmp) * 8 - 1; i >= 0; i--) {
      *r-- = (tmp & 1) + '0';
      tmp >>= 1;
    }
    *r-- = 'b';
    *r-- = '0';
    *r-- = '\"';

    // remove zeros
    int i = 3;
    while (r_orig[i] == '0' && r_orig[i+1] != '\0')
      i++;

    unsigned int len = 3 + strlen(r_orig+i);
    memmove(r_orig + 3, r_orig + i, strlen(r_orig + i));
    r_orig[len] = '\0';
    return r_orig;
  }
}


char *hc_tohex(char *e) {
  e = hc_result_(e);
  if (!e)
    return FAIL;
  if (!is_positive_int(e)) {
    free(e);
    arg_error("tohex() only accepts positive integer arguments");
  } else {
    unsigned int tmp = atoi(e);
    char *r = malloc(1 + 2 + hc_need_space_int(tmp) + 1 + 1); // hc_need_space_int() gives space necessary for the decimal representation of its argument. Since the hexadecimal representation will always be smaller or the same length as the decimal representation, this can be safely used here.
    if (!r) mem_error();
    sprintf(r, "\"0x%X\"", tmp);
    return r;
  }
}


char *hc_2str(char *e)
{
  e = hc_result_(e);
  if (!e)
    return FAIL;
  if (is_string(e))
    return e;
  else
  {
    char *r = hc_strip_0s(e);
    free(e);
    e = malloc(strlen(r)+3);
    if (!e)
      mem_error();
    sprintf(e,"\"%s\"",r);
    free(r);
    return e;
  }
}


int hc_2num(M_APM re, M_APM im, char *e)
{
  e = hc_result_(e);
  if (!e)
    return FAIL;
  int r = hc_2num_nr(re, im, e);
  free(e);
  return r;
}


int hc_2num_nr(M_APM re, M_APM im, char *e)
{
  char *r_re = NULL;
  char *r_im = NULL;
  if (is_string(e))
  {
    char *tmp = get_string(e);
    r_re = hc_real_part(tmp);
    r_im = hc_imag_part(tmp);
    free(tmp);
  } else {
    r_re = hc_real_part(e);
    r_im = hc_imag_part(e);
  }
  m_apm_set_string(re,r_re);
  if (r_im)
    m_apm_set_string(im,r_im);
  else
    m_apm_copy(im,MM_Zero);
  free(r_re);
  free(r_im);
  return SUCCESS;
}


char contains_char(char *str)
{
  int i = 0;
  for (; str[i]!='\0'; i++)
  {
    if (isalpha(str[i]))
      return TRUE;
  }
  return FALSE;
}


char is_num(char *str)
{
  unsigned int i = 0;
  for (; i < strlen(str); i++)
  {
    if (!isdigit(str[i]) && str[i]!='.' && tolower(str[i])!='e' && tolower(str[i])!='i' && str[i]!='+' && str[i]!='-')
    {
      return FALSE;
    }
  }
  if (i == 0) // empty string
    return FALSE;
  else
    return TRUE;
}


char is_real_num(char *str)
{
  unsigned int i = 0;
  for (; i < strlen(str); i++)
  {
    if (!isdigit(str[i]) && str[i]!='.' && tolower(str[i])!='e' && str[i]!='+' && str[i]!='-')
    {
      return FALSE;
    }
  }
  return TRUE;
}


// returns true if str contains a base-10 integer in normal (non-scientific) notation
char is_int(char *str)
{
  if (str[0] == '\0')
    return FALSE;
  unsigned int i = 0;
  char sawradix = FALSE;
  for (; i < strlen(str); i++)
  {
    if (str[i] == '.')
    {
      sawradix = TRUE;
      continue;
    }
    if (!sawradix)
    {
      if (!isdigit(str[i]) && str[i]!='.' && str[i]!='+' && str[i]!='-')
        return FALSE;
    } else {
      if (str[i]!='0')
        return FALSE;
    }
  }
  return TRUE;
}


// same as is_int() (above), but checks if the first character is a negative sign
// TRUE for positive or zero
char is_positive_int(char *str)
{
  if (str[0] == '-')
    return FALSE;
  else
    return is_int(str);
}


char *hc_2dec(char base, char *in)
{
  int i = strlen(in)-1;
  int power = 0;
  unsigned int decp = i;
  M_APM res = m_apm_init(); m_apm_copy(res,MM_Zero);
  M_APM t1 = m_apm_init();
  M_APM t2 = m_apm_init();
  M_APM t3 = m_apm_init();
  for (; i>=0; i--, power++)
  {
    if (in[i]=='.')
    {
      decp = i;
      power--;
    } else if (in[i]=='-' && i == 0) {
      m_apm_copy(t1,res);
      m_apm_negate(res,t1);
    } else {
      m_apm_set_long(t1,(long)base);
      m_apm_integer_pow(t2,HC_DEC_PLACES,t1,power);
      m_apm_set_long(t1,(long)b10(in[i]));
      m_apm_multiply(t3,t1,t2);
      m_apm_copy(t1,res);
      m_apm_add(res,t1,t3);
    }
  }
  if (decp != strlen(in) - 1)
  {
    m_apm_set_long(t2,(long)base);
    m_apm_integer_pow(t3,HC_DEC_PLACES, t2, strlen(in)- 1 - decp);
    m_apm_copy(t2,res);
    m_apm_divide(res,HC_DEC_PLACES,t2,t3);
  }
  char *tmp = m_apm_to_fixpt_stringexp(HC_DEC_PLACES, res, '.', 0, 0);
  m_apm_free(t1); m_apm_free(t2); m_apm_free(t3); m_apm_free(res);
  return tmp;
}


char *hc_strrepr(struct hc_stack_element *el)
{
  char *r = NULL;
  char *r1 = NULL;
  char *r2 = NULL;

  int precision;
  if (hc.flags & PRINTFULL)
    precision = HC_DEC_PLACES;
  else
    precision = hc.precision;

  switch (el->type)
  {
  case HC_VAR_NUM:
    r1 = m_apm_to_fixpt_stringexp(precision,el->re,'.',0,0);
    if (m_apm_compare(el->im,MM_Zero)!=0)
    {
      r2 = m_apm_to_fixpt_stringexp(precision,el->im,'.',0,0);
      r = malloc(strlen(r1)+1+strlen(r2)+1);
      sprintf(r,"%si%s",r1,r2);
      free(r1); free(r2);
    } else {
      r = r1;
    }
    break;

  case HC_VAR_STR:
  case HC_VAR_VEC:
  case HC_VAR_EMPTY:
    r = strdup(el->str);
    break;
  }

  return r;
}


inline void hc_round(M_APM result, int decp, M_APM in)
{
  m_apm_round(result, decp + (m_apm_exponent(in) > 0 ? m_apm_exponent(in) : 0), in);
}
