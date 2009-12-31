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
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hc.h"
#include "hc_functions.h"
#include "hc_complex.h"


double hc_strtod(const char *e, char *unused)
{
  return strtod(e,NULL);
}


char *strreplace(char *in, char *old, char *new)
{
#ifdef DBG
  printf("strreplace received %s\n",in);
#endif
  char *tnew = malloc(strlen(new)+3);
  if (!tnew)
    mem_error();
  strcpy(tnew,"(");
  strcat(tnew,new);
  strcat(tnew,")");
  new = tnew;
  char *p = strstr(in,old);
  char *result;
  if (!p)
  {
    result = malloc(sizeof(char) * (strlen(in) + 1));
    if (!result)
      mem_error();
    strcpy(result,in);
    free(tnew);
    return result;
  } else {
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
      int result_malloc = sizeof(char)*(strlen(in)-strlen(p+1)+1);
      result = malloc(result_malloc);
      if (!result)
	mem_error();
      strncpy(result,in,(strlen(in)-strlen(p+1)));
      result[(strlen(in)-strlen(p+1))]=0;
      char *ptr = in+sizeof(char)*strlen(in)-strlen(p+1);
      while (isalpha((char)(ptr-sizeof(char))[0]) && isalpha(ptr[0]))
      {
	char apme[2];
	apme[0] = ptr[0];
	apme[1] = 0;
	result = realloc(result,++result_malloc);
	strcat(result,apme);
	ptr++;
      }
      //char *cat = strreplace(in+sizeof(char)*(strlen(in)-strlen(p+1)),old,new);
      char *cat = strreplace(ptr,old,new);
      char *tmp = malloc(sizeof(char) * (strlen(cat)+strlen(result)+1));
      if (!tmp)
	mem_error();
      strcpy(tmp,result);
      strcat(tmp,cat);
      free(result);
      free(cat);
      free(tnew);
      return tmp;
    }
  }
}


char *hc_get_arg(char *e, int pos)
{
  int i=0;
  int par=0;
  int p=0;
  //int e_len = strlen(e);
  char *tmp = malloc((strlen(e)+1)*sizeof(char));
  if (!tmp)
    mem_error();
  strcpy(tmp,e);
  char *last_pointer = tmp;
  char *result = NULL;

  while (tmp[i]!=0)
  {
    if (tmp[i]=='(')
      par++;
    if (tmp[i]==')')
      par--;
    if ((tmp[i]==',')&&(par==0))
    {
      p++;
      tmp[i]=0;
      if (p==pos)
      {
	if (last_pointer!=tmp)
	{
	  result = malloc(sizeof(char) * (strlen(last_pointer)+1));
	  if (!result)
	    mem_error();
	  strcpy(result,last_pointer);
	  free(tmp);
	  return result;
	} else {
	  return last_pointer;
	}
      } else {
	last_pointer = tmp + sizeof(char)*i + sizeof(char);
      }
    }
    i++;
  }
  if (p+1==pos)
  {
    result = malloc(sizeof(char) * (strlen(last_pointer)+1));
    if (!result)
      mem_error();
    strcpy(result,last_pointer);
    free(tmp);
    return result;
  } else {
    free(tmp);
    return NULL;
  }
}


int hc_modulus(M_APM result, char *e)
{
  if (strchr(e,',')==NULL)
    arg_error("mod() needs two arguments.");
  
  /* 2-arg INIT */
  char *arg1,*arg2,*t1,*t2;
  t1 = hc_get_arg(e,1);  t2 = hc_get_arg(e,2);
  arg1 = (char *)hc_plusminus(t1);  arg2 = (char *)hc_plusminus(t2);
  free(t1); free(t2);
  t1 = arg1; t2 = arg2;
  arg1 = hc_result_(arg1);  arg2 = hc_result_(arg2);
  free(t1); free(t2);
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
  char *arg1,*arg2,*t1,*t2;
  t1 = hc_get_arg(e,1);  t2 = hc_get_arg(e,2);
  arg1 = (char *)hc_plusminus(t1);  arg2 = (char *)hc_plusminus(t2);
  free(t1); free(t2);
  t1 = arg1; t2 = arg2;
  arg1 = hc_result_(arg1);  arg2 = hc_result_(arg2);
  free(t1); free(t2);
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
  char *arg1,*arg2,*t1,*t2;
  t1 = hc_get_arg(e,1);  t2 = hc_get_arg(e,2);
  arg1 = (char *)hc_plusminus(t1);  arg2 = (char *)hc_plusminus(t2);
  free(t1); free(t2);
  t1 = arg1; t2 = arg2;
  arg1 = hc_result_(arg1);  arg2 = hc_result_(arg2);
  free(t1); free(t2);
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
  char *arg1,*arg2,*arg3,*t1,*t2,*t3;
  t1 = hc_get_arg(e,1);  t2 = hc_get_arg(e,2); t3 = hc_get_arg(e,3);
  if (t1 == NULL || t2 == NULL || t3 == NULL)
    arg_error("sum() needs 3 arguments (expr,low,high).");
  arg1 = (char *)hc_plusminus(t1);  arg2 = (char *)hc_plusminus(t2); arg3 = (char *)hc_plusminus(t3);
  free(t1); free(t2); free(t3);
  t1 = arg1; t2 = arg2; t3 = arg3;
  arg2 = hc_result_(arg2);  arg3 = hc_result_(arg3);
  if (arg2 == NULL || arg3 == NULL)
    return FAIL;
  free(t2); free(t3);
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
  char *tmp_i_str = malloc(sizeof(char) * (sizeof(int)*8+1));
  char *tmp_fme;
  char *tmp_res;

  while (m_apm_compare(min,max)!=1)
  {
    memset(tmp_expr,0,strlen(arg1)+1);
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
  char *arg1,*arg2,*arg3,*t1,*t2,*t3;
  t1 = hc_get_arg(e,1);  t2 = hc_get_arg(e,2); t3 = hc_get_arg(e,3);
  if (t1 == NULL || t2 == NULL || t3 == NULL)
    arg_error("sum() needs 3 arguments (expr,low,high).");
  arg1 = (char *)hc_plusminus(t1);  arg2 = (char *)hc_plusminus(t2); arg3 = (char *)hc_plusminus(t3);
  free(t1); free(t2); free(t3);
  t1 = arg1; t2 = arg2; t3 = arg3;
  arg2 = hc_result_(arg2);  arg3 = hc_result_(arg3);
  if (arg2 == NULL || arg3 == NULL)
    return FAIL;
  free(t2); free(t3);
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
  char *tmp_i_str = malloc(sizeof(char) * (sizeof(int)*8+1));
  char *tmp_fme;
  char *tmp_res;
  char init = 0;

  while (m_apm_compare(min,max)!=1)
  {
    memset(tmp_expr,0,strlen(arg1)+1);
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


int hc_gcd(M_APM result, char *e)
{
  if (strchr(e,',')==NULL)
    arg_error("gcd() needs two arguments.");

  /* 2-arg INIT */
  char *arg1,*arg2,*t1,*t2;
  t1 = hc_get_arg(e,1);  t2 = hc_get_arg(e,2);
  arg1 = (char *)hc_plusminus(t1);  arg2 = (char *)hc_plusminus(t2);
  free(t1); free(t2);
  t1 = arg1; t2 = arg2;
  arg1 = hc_result_(arg1);  arg2 = hc_result_(arg2);
  free(t1); free(t2);
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
  char *arg1,*arg2,*t1,*t2;
  t1 = hc_get_arg(e,1);  t2 = hc_get_arg(e,2);
  arg1 = (char *)hc_plusminus(t1);  arg2 = (char *)hc_plusminus(t2);
  free(t1); free(t2);
  t1 = arg1; t2 = arg2;
  arg1 = hc_result_(arg1);  arg2 = hc_result_(arg2);
  free(t1); free(t2);
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
  if (fme == NULL)
    return FAIL;
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


int hc_km2mi(M_APM result, char *e)
{
  if (strlen(e)==0)
    arg_error("kmtomi() needs an argument.");
  char *fme = hc_result_(e);
  if (fme == NULL)
    return FAIL;
  char *ipart = hc_imag_part(fme);
  if (ipart)
  {
    free(ipart); free(fme);
    arg_error("kmtomi() : argument must be a real number.");
  }
  M_APM tmp = m_apm_init();
  M_APM x = m_apm_init();
  m_apm_set_string(x,"0.621371192"); // Source : Google calculator
  m_apm_set_string(tmp,fme);
  m_apm_multiply(result,tmp,x);
  m_apm_free(x); m_apm_free(tmp); free(fme);
  return SUCCESS;
}


int hc_mi2km(M_APM result, char *e)
{
  if (strlen(e)==0)
    arg_error("mitokm() needs an argument.");
  char *fme = hc_result_(e);
  if (fme == NULL)
    return FAIL;
  char *ipart = hc_imag_part(fme);
  if (ipart)
  {
    free(ipart); free(fme);
    arg_error("mitokm() : argument must be a real number.");
  }
  M_APM tmp = m_apm_init();
  M_APM x = m_apm_init();
  m_apm_set_string(x,"0.621371192"); // Source : Google calculator
  m_apm_set_string(tmp,fme);
  m_apm_divide(result,HC_DEC_PLACES,tmp,x);
  m_apm_free(x); m_apm_free(tmp); free(fme);
  return SUCCESS;
}


int hc_floz2ml(M_APM result, char *e)
{
  if (strlen(e)==0)
    arg_error("floztoml() needs an argument.");
  char *fme = hc_result_(e);
  if (fme == NULL)
    return FAIL;
  char *ipart = hc_imag_part(fme);
  if (ipart)
  {
    free(ipart); free(fme);
    arg_error("floztoml() : argument must be a real number.");
  }
  M_APM tmp = m_apm_init();
  M_APM x = m_apm_init();

  m_apm_set_string(tmp,fme);
  m_apm_set_string(x,"29.5735296875");
  m_apm_multiply(result,x,tmp);

  m_apm_free(x); m_apm_free(tmp); free(fme);
  return SUCCESS;
}


int hc_ml2floz(M_APM result, char *e)
{
  if (strlen(e)==0)
    arg_error("mltofloz() needs an argument.");
  char *fme = hc_result_(e);
  if (fme == NULL)
    return FAIL;
  char *ipart = hc_imag_part(fme);
  if (ipart)
  {
    free(ipart); free(fme);
    arg_error("mltofloz() : argument must be a real number.");
  }
  M_APM tmp = m_apm_init();
  M_APM x = m_apm_init();

  m_apm_set_string(tmp,fme);
  m_apm_set_string(x,"29.5735296875");
  m_apm_divide(result,HC_DEC_PLACES,tmp,x);

  m_apm_free(x); m_apm_free(tmp); free(fme);
  return SUCCESS;
}


int hc_inch2cm(M_APM result, char *e)
{
  if (strlen(e)==0)
    arg_error("inchtocm() needs an argument.");
  char *fme = hc_result_(e);
  if (fme == NULL)
    return FAIL;
  char *ipart = hc_imag_part(fme);
  if (ipart)
  {
    free(ipart); free(fme);
    arg_error("inchtocm() : argument must be a real number.");
  }
  M_APM tmp = m_apm_init();
  M_APM x = m_apm_init();

  m_apm_set_string(tmp,fme);
  m_apm_set_string(x,"2.54");
  m_apm_multiply(result,tmp,x);

  m_apm_free(x); m_apm_free(tmp); free(fme);
  return SUCCESS;
}


int hc_cm2inch(M_APM result, char *e)
{
  if (strlen(e)==0)
    arg_error("cmtoinch() needs an argument.");
  char *fme = hc_result_(e);
  if (fme == NULL)
    return FAIL;
  char *ipart = hc_imag_part(fme);
  if (ipart)
  {
    free(ipart); free(fme);
    arg_error("cmtoinch() : argument must be a real number.");
  }
  M_APM tmp = m_apm_init();
  M_APM x = m_apm_init();

  m_apm_set_string(tmp,fme);
  m_apm_set_string(x,"2.54");
  m_apm_divide(result,HC_DEC_PLACES,tmp,x);

  m_apm_free(x); m_apm_free(tmp); free(fme);
  return SUCCESS;
}


int hc_ft2m(M_APM result, char *e)
{
  if (strlen(e)==0)
    arg_error("fttom() needs an argument.");
  char *fme = hc_result_(e);
  if (fme == NULL)
    return FAIL;
  char *ipart = hc_imag_part(fme);
  if (ipart)
  {
    free(ipart); free(fme);
    arg_error("fttom() : argument must be a real number.");
  }
  M_APM tmp = m_apm_init();
  M_APM x = m_apm_init();

  m_apm_set_string(tmp,fme);
  m_apm_set_string(x,"0.3048");
  m_apm_multiply(result,tmp,x);

  m_apm_free(x); m_apm_free(tmp); free(fme);
  return SUCCESS;
}


int hc_m2ft(M_APM result, char *e)
{
  if (strlen(e)==0)
    arg_error("mtoft() needs an argument.");
  char *fme = hc_result_(e);
  if (fme == NULL)
    return FAIL;
  char *ipart = hc_imag_part(fme);
  if (ipart)
  {
    free(ipart); free(fme);
    arg_error("mtoft() : argument must be a real number.");
  }
  M_APM tmp = m_apm_init();
  M_APM x = m_apm_init();

  m_apm_set_string(tmp,fme);
  m_apm_set_string(x,"0.3048");
  m_apm_divide(result,HC_DEC_PLACES,tmp,x);

  m_apm_free(x); m_apm_free(tmp); free(fme);
  return SUCCESS;
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
  copy_tmp = m_apm_init();
  copy_tmp2 = m_apm_init();

  if (m_apm_compare(i,MM_One)==0)
  {
    m_apm_set_string(result,"1");
    m_apm_free(i); m_apm_free(j);
    return SUCCESS;
  }
  
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
  char *r;
  M_APM _n_ = m_apm_init(); m_apm_set_string(_n_,n);
  int exp = m_apm_exponent(_n_);
  m_apm_free(_n_);
  if ((n[0]=='-' && n[1]=='0') || n[0]=='0')
  {
    r = malloc(strlen(n)+1+hc_need_space_int(exp)+1);
    if (!r)
      mem_error();
    unsigned int i = 2;
    while (n[i]=='0' || n[i]=='-' || n[i]=='.')
      i++;
    if (n[0]=='-')
    {
      r[0] = '-';
      r[1] = n[i];
      r[2] = '.';
    } else {
      r[0] = n[i];
      r[1] = '.';
    }
    unsigned int j = 2;
    if (n[0]=='-')
      j++;
    i++;
    while (n[i]!='\0')
      r[j++] = n[i++];
    r[j] = 'E';
    r[j+1] = 0;
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
    if (n[0]=='-')
    {
      r[0] = '-';
      r[1] = n[1];
      r[2] = '.';
    } else {
      r[0] = n[0];
      r[1] = '.';
    }
    unsigned int i = 1, j = 2;
    if (n[0]=='-')
    {
      i++; j++;
    }
    while (n[i]!='\0')
    {
      if (n[i]=='.')
	i++;
      r[j++] = n[i++];
    }
    r[j] = 'E';
    r[j+1] = 0;
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
  char *tmp;

  switch (exp % 3)
  {
  case 0:
    return r;
    break;

  case 1:
    // take two numbers, then add exponent -1
    if (r[0]=='-')
    {
      r[2] = r[3];
      r[3] = '.';
    } else {
      r[1] = r[2];
      r[2] = '.';
    }
    char *x = malloc(hc_need_space_int(exp-1)+1);
    sprintf(x,"%i",exp-1);
    r[strchr(r,'E')-r+1] = 0;
    tmp = malloc(strlen(x)+strlen(r)+1);
    strcpy(tmp,r);
    strcat(tmp,x);
    free(r); free(x);
    return tmp;
    break;

  case 2:
    // take three numbers, then add exponent -2
    break;

  case -1:
    // take three numbers, then add exponent -2
    break;

  case -2:
    // take two numbers, then add exponent -1
    break;
  }


  if (n[0]=='0')
  {
    r = malloc(strlen(n)+1+hc_need_space_int(exp)+1);
    if (!r)
      mem_error();
    unsigned int i = 2;
    while (n[i]=='0')
      i++;
    r[0] = n[i];
    r[1] = '.';
    unsigned int j = 2;
    i++;
    while (n[i]!='\0')
      r[j++] = n[i++];
    r[j] = 'E';
    r[j+1] = 0;
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
    r[0] = n[0];
    r[1] = '.';
    unsigned int i = 1, j = 2;
    while (n[i]!='\0')
    {
      if (n[i]=='.')
	i++;
      r[j++] = n[i++];
    }
    r[j] = 'E';
    r[j+1] = 0;
    char *_exp_ = malloc(hc_need_space_int(exp)+1);
    sprintf(_exp_,"%i",exp);
    strcat(r,_exp_);
    free(_exp_);
    free(n);
    return r;
  }
}

