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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <m_apm.h>
#include "hc.h"
#include "hc_functions.h"
#include "hash.h"
#include "hc_graph.h"
#define NHASH 29989 // use a large prime number
#define MULT 31


const char *hc_fnames[] = {
  "ans",
  "abs",
  "acos",
  "asin",
  "atan",
  "cbrt",
  "cmtoinch",
  "cos",
  "cosh",
  "ctof",
  "ctok",
  "exp",
  "factorial",
  "fibo",
  "floztoml",
  "ftoc",
  "ftok",
  "fttom",
  "gcd",
  "inchtocm",
  "kmtomi",
  "ktoc",
  "ktof",
  "lcm",
  "ln",
  "log10",
  "mitokm",
  "mltofloz",
  "mod",
  "mtoft",
  "nCr",
  "nPr",
  "product",
  "sin",
  "sinh",
  "sqrt",
  "sum",
  "tan",
  "tanh",
  "totient",
  // CONSTANTS
  "pi",
  "c",
  "e",
  "phi",
  "g",
  "answer",
  // OTHERS
  "help",
  "credits",
  "constants",
  "conversions"
};


unsigned int simple_hash(char *p)
{
  unsigned int h = 0;
  for(; *p; p++)
    h = MULT * h + tolower(*p);
  return h % NHASH;
}


#define isoperator(c) ((c=='*') || (c=='/') || (c=='+') || (c=='-') || (c=='(') || (c==')') || (c==PW_SIGN) || (c=='=') || (c==',') || (c=='!') || (c=='%'))
#define isoperator_np(c) ((c=='*') || (c=='/') || (c=='+') || (c=='-') || (c=='=') || (c==PW_SIGN) || (c==',') || (c=='!') || (c=='%'))
#define isdirection(x) (x[0]=='\\')
#define isvarassign(x) (strchr(x,'=')!=NULL)


char *hc_i2p(char *f);
char *hc_postfix_result(char *e);
char hc_check(char *e);
char *hc_plusminus(char *e);
void hc_process_direction(char *d);
void hc_load_cfg();
void hc_save_cfg();


M_APM hc_lans_mapm;
M_APM MM_MOne; // Minus one
struct hc_config hc;
char announce_errors=TRUE;
int hc_nested;


char *hc_result(char *e)
{
  announce_errors = TRUE; // This variable is used so that syntax errors etc. are not announced multiple times if multiple errors are detected
  hc_nested = 0; // This is set to prevent infinite recursion

  static char init=0;
  if (!init)
  {
    atexit(hc_save_cfg);
    hc_load_cfg();
    hc_lans_mapm = m_apm_init();
    MM_MOne = m_apm_init();
    m_apm_set_double(MM_MOne,(double)-1);
    hc_var_first = malloc(sizeof (struct hc_ventry));
    if (!hc_var_first)
      mem_error();
    hc_var_first->next = NULL;
    hc_var_first->type = 0;
    hc_var_first->value = NULL;
    hc_var_first->name = NULL;
    hc_var_first->args = NULL;
    init = 1;
  }

  m_apm_trim_mem_usage();

  char *r=NULL;

  if (isdirection(e))
  {
    hc_process_direction((char *)e+1);
    r = malloc(1);
    *r = 0;
    if (!r)
      mem_error();
  } else {
    if (isvarassign(e))
    {
      hc_varassign(e);
      r = NULL;
    } else {
      r = hc_result_(e);
      if (r)
      {
	m_apm_set_string(hc_lans_mapm,r);
	int i=strlen(r)-1;
	if (hc.sci == FALSE)
	{
	  while (r[i]=='0')
	    r[i--]=0;
	  if (r[i]=='.')
	    r[i]=0;
	}
      }
    }
  }
  
  return r;
}


// do NOT call this if you want to use the hc core in your own open source project, call hc_result() instead
char *hc_result_(char *f)
{
  hc_nested++;

  if (hc_nested>HC_NESTED_MAX && !hc.bypass_nested)
  {
    recursion_error();
    hc_nested--;
    return NULL;
  }

#ifdef DBG
  printf("hc_nested = %i\n",hc_nested);
#endif

  char *e = malloc(strlen(f)+1);
  if (!e)
    mem_error();
  strcpy(e,f);
  char *e_fme = e;

  // Constants
  // Pi
  e = strreplace(e,"pi","3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384461");
  free(e_fme);
  e_fme = e;

  // Speed of Light
  e = strreplace(e,"c","299792458");
  free(e_fme);
  e_fme = e;

  // e
  e = strreplace(e,"e","2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274274663919320030599218174136");
  free(e_fme);
  e_fme = e;

  // phi
  e = strreplace(e,"phi"," 1.61803398874989484820458683436563811772030917980576286213544862270526046281890244970720720418939113748475408807538689175212663386");
  free(e_fme);
  e_fme = e;

  // g - Gravitational acceleration (std value)
  e = strreplace(e,"g","9.80655");
  free(e_fme);
  e_fme = e;

  // answer - h2g2!
  e = strreplace(e,"answer","42");
  free(e_fme);
  e_fme = e;

  // Constants end
  // User-defined variables

  struct hc_ventry *var_tmp = hc_var_first;
  if (var_tmp->name == NULL)
    var_tmp = NULL;
  while (var_tmp)
  {
    if (var_tmp->type == HC_USR_VAR)
    {
      e = strreplace(e,var_tmp->name,var_tmp->value);
      free(e_fme);
      e_fme = e;
    } else if (var_tmp->type == HC_USR_FUNC)
    {
      char e_tmp[MAX_EXPR];
      int itmp = 0;
      for (; itmp<MAX_EXPR; itmp++)
	e_tmp[itmp]=0;
      char *tmp1 = hc_plusminus(e);
      strcpy(e_tmp,tmp1);
      free(tmp1);
      e = malloc(MAX_EXPR);
      free(e_fme);
      e_fme = e;
      if (!e)
	mem_error();
      strcpy(e,e_tmp);
      char *ffound = strstr(e,var_tmp->name);
      if (ffound && ffound!=e)
      {
	if (isalpha((char)(ffound - sizeof(char))[0]))
	  ffound = NULL;
      }
      if (ffound)
      {
	if (isalpha((char)(ffound + strlen(var_tmp->name))[0]))
	  ffound = NULL;
      }
      if (ffound)
      {
	unsigned int i=0;
	while (isalpha(ffound[i]))
	  i++;
	if (ffound[i]!='(')
	{
	  syntax_error2();
	  free(e);
	  hc_nested--;
	  return NULL;
	} else {
	  i--;
	  char *args = malloc((strlen(ffound)+1-i)*sizeof(char));
	  strcpy(args,ffound+(sizeof(char)*i+sizeof(char)));
	  unsigned int par=1,j=1;
	  while (par!=0)
	  {
	    if (args[j]==')')
	      par--;
	    if (args[j]=='(')
	      par++;
	    if (args[j]==0)
	    {
	      syntax_error2();
	      free(e);
	      free(args);
	      hc_nested--;
	      return NULL;
	    }
	    j++;
	  }
	  unsigned int f_expr_e = (ffound - e) + i + j + 1;
	  args[j]=0;
	  char *custom_f_expr = malloc((strlen(var_tmp->value)+1)*sizeof(char));
	  if (!custom_f_expr)
	    mem_error();
	  strcpy(custom_f_expr,var_tmp->value);
	  unsigned int k = 1;
	  while (k!=-1)
	  {
	    char *t1 = hc_get_arg(args,k);
	    if (t1)
	    {
	      char *t1fme = t1;
	      t1 = hc_result_(t1);
	      free(t1fme);
	    }
	    char *t2 = hc_get_arg(var_tmp->args,k);
	    if (t1 && t2)
	    {
	      char *fme = custom_f_expr;
	      custom_f_expr = strreplace(custom_f_expr,t2,t1);
	      free(t1);
	      free(t2);
	      free(fme);
	      k++;
	    } else {
	      if (t1 || t2)
	      {
		arg_error_custom();
		if (t1)
		  free(t1);
		else
		  free(t2);
		free(e);
		free(args);
		hc_nested--;
		return NULL;
	      } else {
		char *fme_cfe = custom_f_expr;
		custom_f_expr = hc_result_(custom_f_expr);
		free(fme_cfe);
		if (!custom_f_expr)
		{
		  free(e);
		  free(args);
		  hc_nested--;
		  return NULL;
		}
		unsigned int ffoundmine = ffound - e;
		free(e_fme);
		e = malloc(MAX_EXPR);
		strncpy(e,e_tmp,ffoundmine);
		e[ffoundmine]=0; // strncpy does NOT null-terminate automatically
#ifdef DBG
		printf("{1} %s\n",e);
#endif
		if (strlen(e_tmp)+strlen(custom_f_expr)+strlen(((char *)&e_tmp)+f_expr_e+1)+1>MAX_EXPR)
		  overflow_error();
		strcat(e,custom_f_expr);
#ifdef DBG
		printf("{2} %s\n",e);
#endif
		free(custom_f_expr);
		strcat(e,((char *)&e_tmp)+f_expr_e); // was +1
#ifdef DBG
		printf("{3} %s\n",e);
#endif
		free(args);
		char *rme = hc_result_(e);
		free(e);
		hc_nested--;
		return rme;
	      }
	    }
	  }
	}
      }
    }
    if (var_tmp->next && var_tmp->next->name)
      var_tmp = var_tmp->next;
    else
      var_tmp = NULL;
  }

  // User-defined variables end

  e = malloc(MAX_EXPR);
  if (!e)
    mem_error();
  strcpy(e,e_fme);
  free(e_fme);
  e_fme = e;

  if (strcmp(e,HC_INF)==0)
  {
    hc_nested--;
    return e;
  }

  if (!hc_check(e))
  {
    syntax_error2();
    free(e);
    hc_nested--;
    return NULL;
  }

  //e = strreplace(e,"c","299792458");
  //free(e_fme);
  //e_fme = e;

  if (strcmp(e,"help")==0)
  {
    printf("Welcome to HC!\nTo calculate an expression, simply enter it on the command-line in infix notation and press enter.\nTo use a constant, simply type its name with english characters - for example pi. For more info type \"constants\".\nYou can also use a few functions; the list of them is below:\n - ans() - Will be replaced by the result of the last entered expression\n - abs(x) - absolute value of x\n - acos(x) - arc cosine of x\n - asin(x) arc sine of x\n - atan(x) - arc tangent of x\n - cbrt(x) - returns the cube root of x (you could also use x^(1/3))\n - cmtoinch(x) - converts x, which has to be in cm, to inches\n - cos(x) - cosine of x\n - cosh(x) - hyperbolic cosine of x\n - ctof(x) - converts x, which has to be in degrees Celsius, to Fahrenheit\n - ctok(x) - converts x, which has to be in degrees Celsius, to Kelvin\n - exp(x) - exponential function\n - factorial(x) - factorial of x (you can also use the '!' notation)\n - fibo(x) - returns the xth term of the Fibonacci sequence\n - floztoml(x) - converts x, which has to be in US fluid ounces, to ml\n - ftoc(x) - converts x, which has to be in Fahrenheit, to degrees Celsius\n - ftok(x) - converts x, which has to be in Fahrenheit, to Kelvin\n - fttom(x) - converts x, which has to be in feet, to meters\n - gcd(x,y) - finds the greatest common divisor of x and y\n - inchtocm(x) - converts x, which has to be in inches, to cm\n - kmtomi(x) - converts x, which has to be in km, to miles\n - ktoc(x) - converts x, which has to be in Kelvin, to degrees Celsius\n - ktof(x) - converts x, which has to be in Kelvin, to Fahrenheit\n - lcm(x,y) - finds the least common multiple of x and y\n - ln(x) - natural logarithm\n - log10(x) - common logarithm\n - mitokm(x) - converts x, which has to be in miles, to km\n - mltofloz(x) - converts x, which has to be in ml, to US fluid ounces\n - mod(x,y) - modulus (you can also use the C-style '%%' notation)\n - mtoft(x) - converts x, which has to be in m, to feet\n - nCr(n,k) - binomial coefficient (combinatorics)\n - nPr(n,k) - number of permutations (combinatorics)\n - product(expr,low,high) - returns the product of expr with x from low to high inclusive (WARNING : As of now, other variable names will be ignored)\n - sin(x) - sine of x\n - sinh(x) - hyperbolic sine of x\n - sqrt(x) - returns the square root of x\n - sum(expr,low,high) - same as product, except the result is summation, not multiplication\n - tan(x) - tangent of x\n - tanh(x) - hyperbolic tangent of x\n - totient(x) - Euler's totient function\nYou can also use these directions/configuration commands:\n - \\p PRECISION - change current precision to PRECISION decimal places\n - \\rpn - change modes (RPN / infix)\n - \\sci - switch between \"normal\" and scientific notation for results\n - \\deg, \\rad, \\grad - switch between DEG, RAD, and GRAD modes for angles\n - \\clear - clear all user-defined variables and functions\nTo get a list of only the converter functions, type \"conversions\".\nTo read the license and credits, type \"credits\".\nTo exit The HoubySoft Calculator, just type \"exit\" in the prompt (you can also use CTRL+C on systems that support it, but configuration will not be saved if you do that).\n");
    free(e);
    hc_nested--;
    return NULL;
  }

  if (strcmp(e,"conversions")==0)
  {
    printf("List of conversion functions:\n - cmtoinch(x) - converts x, which has to be in cm, to inches\n - ctof(x) - converts x, which has to be in degrees Celsius, to Fahrenheit\n - ctok(x) - converts x, which has to be in degrees Celsius, to Kelvin\n - floztoml(x) - converts x, which has to be in US fluid ounces, to ml\n - ftoc(x) - converts x, which has to be in Fahrenheit, to degrees Celsius\n - ftok(x) - converts x, which has to be in Fahrenheit, to Kelvin\n - fttom(x) - converts x, which has to be in feet, to meters\n - inchtocm(x) - converts x, which has to be in inches, to cm\n - kmtomi(x) - converts x, which has to be in km, to miles\n - ktoc(x) - converts x, which has to be in Kelvin, to degrees Celsius\n - ktof(x) - converts x, which has to be in Kelvin, to Fahrenheit\n - mitokm(x) - converts x, which has to be in miles, to km\n - mltofloz(x) - converts x, which has to be in ml, to US fluid ounces\n - mtoft(x) - converts x, which has to be in m, to feet\n");
    free(e);
    hc_nested--;
    return NULL;
  }

  if (strcmp(e,"constants")==0)
  {
    printf("To use constants, just type their name with english characters. Here is a list of available constants:\n - answer - The Answer\n - c - speed of light\n - e - Euler's number\n - g - standard gravitational acceleration on Earth\n - phi - Golden ratio\n - pi\n");
    free(e);
    hc_nested--;
    return NULL;
  }

  if (strcmp(e,"credits")==0)
  {
    printf("HC was developed by Jan Dlabal (visit http://houbysoft.com/en/about.php for contact information) and is (C) 2009.\nThe homepage of this program is http://hc.houbysoft.com/.\nThis program was released under the GNU GPL v3 License. Visit gnu.org for more information.\n");
    free(e);
    hc_nested--;
    return NULL;
  }

  char f_name[MAX_F_NAME];
  char f_expr[MAX_F_TMP];
  char e_tmp[MAX_EXPR];
  char *tmp1 = hc_plusminus(e);
  strcpy(e_tmp,tmp1);
  free(tmp1);
  strcpy(e,e_tmp);
  int i,j,k;
  int left_par=0,right_par=0,f_expr_s=0,f_expr_e=0;

  // Find & replace functions with their results
  for (i=0;i<MAX_EXPR;i++)
  {
    if (e[i]==0)
    {
      i = MAX_EXPR;
      break;
    }
    if (isalpha(e[i]) && (isalpha(e[i+1]) || e[i+1]=='(' || e[i+1]==0))
      break;
    if (isalpha(e[i]) && (isdigit(e[i+1])))
      break;
  }

  

  if (i >= MAX_EXPR)
  {
    // all functions were replaced, go go go power rangers (&xkcd)
    char *fme;
#ifdef DBG
    printf("[1] %s\n",e);
#endif
    e = fme = hc_i2p(e);
#ifdef DBG
    printf("[2] %s\n",e);
#endif
    e = hc_postfix_result(e);
#ifdef DBG
    printf("[3] %s\n",e);
#endif
    free(fme);
    free(e_fme);
    hc_nested--;
    return e;
  } else {
    M_APM f_result = m_apm_init();
    // we need to replace a function with its result
    // e[i] points to the start of the function
    j = i;
    k = 0;
    while (e[j]!='(')
    {
      f_name[k++] = e[j];
      if (k >= MAX_F_NAME)
      {
	m_apm_free(f_result);
	free(e);
	syntax_error2();
	hc_nested--;
	return NULL;
      }
      j++;
    }
    f_name[k] = '\0';
    // f_name contains the name of the function

    // get the expression to be processed by the function, e[j] points to first '('
    f_expr_s = j+1;
    left_par = 0;
    right_par = 0;
    f_expr_e = 0;
    j-=1;
    while ((left_par != right_par) || (left_par==0))
    {
      j++;
      if (e[j]=='(')
      {
	left_par++;
      } else if (e[j]==')')
      {
	right_par++;
      }
    }
    f_expr_e = j;
    strncpy(f_expr,e + sizeof(char)*f_expr_s,f_expr_e - f_expr_s);
    f_expr[f_expr_e - f_expr_s] = 0;
    
    char *fme;
    M_APM tmp_num = m_apm_init();

    switch (simple_hash(f_name))
    {
    case HASH_ABS:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      m_apm_set_string(tmp_num,fme);
      free(fme);
      m_apm_absolute_value(f_result,tmp_num);
      break;

    case HASH_ANS:
      m_apm_copy(f_result,hc_lans_mapm);
      break;

    case HASH_ACOS:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      m_apm_set_string(tmp_num,fme);
      free(fme);
      if ((m_apm_compare(tmp_num,MM_One)==1) || (m_apm_compare(tmp_num,MM_MOne)==-1))
	arg_error("acos() : NaN");
      m_apm_acos(f_result,HC_DEC_PLACES,tmp_num);
      hc_from_rad(f_result);
      break;
      
    case HASH_ASIN:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      m_apm_set_string(tmp_num,fme);
      free(fme);
      if ((m_apm_compare(tmp_num,MM_One)==1) || (m_apm_compare(tmp_num,MM_MOne)==-1))
	arg_error("asin() : NaN");
      m_apm_asin(f_result,HC_DEC_PLACES,tmp_num);
      hc_from_rad(f_result);
      break;

    case HASH_ATAN:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      m_apm_set_string(tmp_num,fme);
      free(fme);
      m_apm_atan(f_result,HC_DEC_PLACES,tmp_num);
      hc_from_rad(f_result);
      break;

    case HASH_COS:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      m_apm_set_string(tmp_num,fme);
      free(fme);
      hc_to_rad(tmp_num);
      m_apm_cos(f_result,HC_DEC_PLACES,tmp_num);
      break;

    case HASH_EXP:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      m_apm_set_string(tmp_num,fme);
      free(fme);
      m_apm_exp(f_result,HC_DEC_PLACES,tmp_num);
      break;

    case HASH_FACTORIAL:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      m_apm_set_string(tmp_num,fme);
      free(fme);
      if (!m_apm_is_integer(tmp_num))
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); arg_error("factorial() : an integer is required."); hc_nested--; return NULL;}
      m_apm_factorial(f_result,tmp_num);
      break;

    case HASH_GCD:
      if (hc_gcd(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_LCM:
      if (hc_lcm(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_LN:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      m_apm_set_string(tmp_num,fme);
      free(fme);
      m_apm_log(f_result,HC_DEC_PLACES,tmp_num);
      break;

    case HASH_LOG10:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      m_apm_set_string(tmp_num,fme);
      free(fme);
      m_apm_log10(f_result,HC_DEC_PLACES,tmp_num);
      break;

    case HASH_SIN:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      m_apm_set_string(tmp_num,fme);
      free(fme);
      hc_to_rad(tmp_num);
      m_apm_sin(f_result,HC_DEC_PLACES,tmp_num);
      break;

    case HASH_TAN:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      m_apm_set_string(tmp_num,fme);
      free(fme);
      hc_to_rad(tmp_num);
      m_apm_tan(f_result,HC_DEC_PLACES,tmp_num);
      break;

    case HASH_SUM:
      if (hc_sum(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_PRODUCT:
      if (hc_product(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_COSH:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      m_apm_set_string(tmp_num,fme);
      free(fme);
      m_apm_cosh(f_result,HC_DEC_PLACES,tmp_num);
      break;

    case HASH_SINH:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      m_apm_set_string(tmp_num,fme);
      free(fme);
      m_apm_sinh(f_result,HC_DEC_PLACES,tmp_num);
      break;

    case HASH_TANH:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      m_apm_set_string(tmp_num,fme);
      free(fme);
      m_apm_tanh(f_result,HC_DEC_PLACES,tmp_num);
      break;

    case HASH_SQRT:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      m_apm_set_string(tmp_num,fme);
      free(fme);
      if (m_apm_compare(tmp_num,MM_Zero)==-1)
	arg_error("sqrt() : non-real result.");
      m_apm_sqrt(f_result,HC_DEC_PLACES,tmp_num);
      break;

    case HASH_CBRT:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      m_apm_set_string(tmp_num,fme);
      free(fme);
      m_apm_cbrt(f_result,HC_DEC_PLACES,tmp_num);
      break;

    case HASH_MOD:
      if (hc_modulus(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_FIBO:
      if (hc_fibo(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_TOTIENT:
      if (hc_totient(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_CTOF:
      if (hc_c2f(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_FTOC:
      if (hc_f2c(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_CTOK:
      if (hc_c2k(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_KTOC:
      if (hc_k2c(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_FTOK:
      if (hc_f2k(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_KTOF:
      if (hc_k2f(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_MITOKM:
      if (hc_mi2km(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_KMTOMI:
      if (hc_km2mi(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_MLTOFLOZ:
      if (hc_ml2floz(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_FLOZTOML:
      if (hc_floz2ml(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_INCHTOCM:
      if (hc_inch2cm(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_CMTOINCH:
      if (hc_cm2inch(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_FTTOM:
      if (hc_ft2m(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_MTOFT:
      if (hc_m2ft(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_NCR:
      if (hc_binomc(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_NPR:
      if (hc_permutations(f_result,f_expr) == FAIL)
      {m_apm_free(tmp_num); m_apm_free(f_result); free(e); hc_nested--; return NULL;}
      break;

    case HASH_GRAPH:
      hc_graph(f_expr);
      announce_errors = FALSE;
      m_apm_free(tmp_num);
      m_apm_free(f_result);
      free(e);
      hc_nested--;
      return NULL;
      break;

    default:
      m_apm_free(tmp_num);
      m_apm_free(f_result);
      free(e);
      undef_error();
      hc_nested--;
      return NULL;
    }

    
    strncpy(e,e_tmp,i);
    e[i]=0;
    //char f_result_tmp[MAX_DOUBLE_STRING];
    char *f_result_tmp = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,f_result,'.',0,0);
    if (strlen(e_tmp)+strlen(f_result_tmp)+strlen(((char *)&e_tmp)+f_expr_e+1)+1>MAX_EXPR)
      overflow_error();
    //m_apm_to_string(f_result_tmp,HC_DEC_PLACES,f_result);
    strcat(e,f_result_tmp);
    free(f_result_tmp);
    strcat(e,((char *)&e_tmp)+f_expr_e+1);
    char *rme = hc_result_(e);
    free(e_fme);
    m_apm_free(f_result);
    m_apm_free(tmp_num);
    hc_nested--;
    return rme;
  }
}


char *hc_i2p(char *f)
{
  char *e = malloc(MAX_EXPR*sizeof(char));
  if (!e)
    mem_error();
  strcpy(e,f);
  if (hc.rpn)
    return e;
  e[strlen(e)+1]=0;
  e[strlen(e)]='=';
  char tmp[MAX_EXPR];
  char stack[MAX_OP_STACK];
  int sp=0;
  strcpy(tmp,e);
  memset(e,0,MAX_EXPR);
  int i=0,j=0;

  stack[sp++] = '=';

  while (tmp[i]!=0)
  {
    if (!isspace(tmp[i]))
    {
      if (isoperator(tmp[i]))
      {
	if (sp>=MAX_OP_STACK)
	  overflow_error();
	switch (tmp[i])
	{
	case '=': // terminating character
	  while (stack[sp-1]!='=')
	    e[j++] = stack[--sp];
	  return e;
	case '(':
	  stack[sp++] = '(';
	  break;
	case ')':
	  sp--;
	  while (stack[sp]!='(')
	    e[j++] = stack[sp--];
	  break;
	case '^':
	case '!':
	  if ((stack[sp-1]=='+')||(stack[sp-1]=='-')||(stack[sp-1]=='=')||(stack[sp-1]=='(')||(stack[sp-1]=='*')||(stack[sp-1]=='/')||(stack[sp-1]==PW_SIGN)||(stack[sp-1]=='%'))
	  {
	    stack[sp++] = tmp[i];
	  } else {
	    e[j++] = stack[--sp];
	    i--;
	  }
	  break;
	case '*':
	case '/':
	case '%':
	  if ((stack[sp-1]=='+')||(stack[sp-1]=='-')||(stack[sp-1]=='=')||(stack[sp-1]=='('))
	  {
	    stack[sp++] = tmp[i];
	  } else {
	    e[j++] = stack[--sp];
	    i--;
	  }
	  break;
	case '+':
	case '-':
	    if ((stack[sp-1]!='(')&&(stack[sp-1]!='='))
	    {
	      e[j++] = stack[--sp];
	      i--;
	    } else {
	      stack[sp++] = tmp[i];
	    }
	  break;
	}

      } else {
	while ((!isspace(tmp[i]))&&((!isoperator(tmp[i]) || tolower(tmp[i-1])=='e')))
	  e[j++] = tmp[i++];
	e[j++] = ' ';
	i--;
      }
    }
    i++;
  }

  return e;
}


char *hc_postfix_result(char *e)
{
#ifdef DBG
  printf("hc_postfix_result received %s\n",e);
#endif
  //char *result = malloc(MAX_EXPR);
  //if (!result)
  //mem_error();
     struct hc_stack_element *first;
     struct hc_stack_element *curr;
     first = malloc(sizeof(struct hc_stack_element));
     first->v = m_apm_init();
     first->p = NULL;
     first->n = malloc(sizeof(struct hc_stack_element));
     first->n->p = first;
     first->n->v = m_apm_init();
     first->n->n = NULL;

     curr = first;
     int sp = 0;

     //M_APM stack[MAX_DOUBLE_STACK];
     M_APM op1,op2;

     char tmp_num[MAX_DOUBLE_STRING];
     int i=0,j=0;

     //for (;i<MAX_DOUBLE_STACK;i++)
     //  stack[i] = m_apm_init();
     op1 = m_apm_init();
     op2 = m_apm_init();

     i = 0;

     while (e[i]!=0)
     {
       if (!isspace(e[i]))
       switch (e[i])
       {
       case '*':
	 if (sp < 2)
	 {
	   m_apm_free(op1);m_apm_free(op2);
	   while (first->n)
	   {
	     m_apm_free(first->v);first = first->n;free(first->p);
	   }
	   m_apm_free(first->v);free(first);
	   syntax_error2();
	   return NULL;
	 }
	 curr = curr->p; // [--sp]
	 m_apm_copy(op2,curr->v);
	 curr = curr->p; // [--sp]
	 m_apm_copy(op1,curr->v);
	 m_apm_multiply(curr->v,op1,op2);
	 curr = curr->n; // [sp++]
	 sp -= 1;
         break;
       case '/':
	 if (sp < 2)
	 {
	   m_apm_free(op1);m_apm_free(op2);
	   while (first->n)
	   {
	     m_apm_free(first->v);first = first->n;free(first->p);
	   }
	   m_apm_free(first->v);free(first);
	   syntax_error2();
	   return NULL;
	 }
	 curr = curr->p; // [--sp]
	 m_apm_copy(op2,curr->v);
	 curr = curr->p; // [--sp]
	 m_apm_copy(op1,curr->v);
	 if (m_apm_sign(op2)==0)
	 {
	   m_apm_free(op1);m_apm_free(op2);
	   while (first->n)
	   {
	     m_apm_free(first->v);first = first->n;free(first->p);
	   }
	   m_apm_free(first->v);free(first);
	   d0_error();
	   return NULL;
	 }
	 m_apm_divide(curr->v,HC_DEC_PLACES,op1,op2);
	 curr = curr->n; // [sp++]
	 sp -= 1;
         break;
       case '%': // Modulo divison; for example 5 % 3 = 2 = mod(5,3)
	 if (sp < 2)
	 {
	   m_apm_free(op1);m_apm_free(op2);
	   while (first->n)
	   {
	     m_apm_free(first->v);first = first->n;free(first->p);
	   }
	   m_apm_free(first->v);free(first);
	   syntax_error2();
	   return NULL;
	 }
	 curr = curr->p; // [--sp]
	 m_apm_copy(op2,curr->v);
	 curr = curr->p; // [--sp]
	 m_apm_copy(op1,curr->v);
	 M_APM tmp = m_apm_init();
	 m_apm_integer_div_rem(tmp,curr->v,op1,op2);
	 m_apm_free(tmp);
	 curr = curr->n; // [sp++]
	 sp -= 1;
         break;
       case '+':
	 if (sp < 2)
	 {
	   m_apm_free(op1);m_apm_free(op2);
	   while (first->n)
	   {
	     m_apm_free(first->v);first = first->n;free(first->p);
	   }
	   m_apm_free(first->v);free(first);
	   syntax_error2();
	   return NULL;
	 }
	 curr = curr->p; // [--sp]
	 m_apm_copy(op2,curr->v);
	 curr = curr->p; // [--sp]
	 m_apm_copy(op1,curr->v);
	 m_apm_add(curr->v,op1,op2);
	 curr = curr->n; // [sp++]
	 sp -= 1;
         break;
       case '-':
	 if (sp < 2)
	 {
	   curr = curr->p; // [--sp]
	   m_apm_copy(op1,curr->v);
	   m_apm_subtract(curr->v,MM_Zero,op1);
	   curr = curr->n; // [sp++]
	   // no need to modify sp here
	 } else {
	   curr = curr->p; // [--sp]
	   m_apm_copy(op2,curr->v);
	   curr = curr->p; // [--sp]
	   m_apm_copy(op1,curr->v);
	   m_apm_subtract(curr->v,op1,op2);
	   curr = curr->n; // [sp++]
	   sp -= 1;
	 }
         break;
       case PW_SIGN:
	 if (sp < 2)
	 {
	   m_apm_free(op1);m_apm_free(op2);
	   while (first->n)
	   {
	     m_apm_free(first->v);first = first->n;free(first->p);
	   }
	   m_apm_free(first->v);free(first);
	   syntax_error2();
	   return NULL;
	 }
	 curr = curr->p; // [--sp]
	 m_apm_copy(op2,curr->v);
	 curr = curr->p; // [--sp]
	 m_apm_copy(op1,curr->v);
	 m_apm_pow(curr->v,HC_DEC_PLACES,op1,op2);
	 curr = curr->n; // [sp++]
	 sp -= 1;
	 break;
       case '!':
	 if (sp < 1)
	 {
	   m_apm_free(op1);m_apm_free(op2);
	   while (first->n)
	   {
	     m_apm_free(first->v);first = first->n;free(first->p);
	   }
	   m_apm_free(first->v);free(first);
	   syntax_error2();
	   return NULL;
	 }
	 curr = curr->p; // [--sp]
	 if (!m_apm_is_integer(curr->v))
	 {
	   m_apm_free(op1);m_apm_free(op2);
	   while (first->n)
	   {
	     m_apm_free(first->v);first = first->n;free(first->p);
	   }
	   m_apm_free(first->v);free(first);
	   arg_error("! : an integer is required.");
	   return NULL;
	 }
	 m_apm_copy(op1,curr->v);
	 m_apm_factorial(curr->v,op1);
	 curr = curr->n; // [sp++]
	 break;
       default:
         j = 0;
         while (!isspace(e[i]) && (!isoperator(e[i]) || tolower(e[i-1])=='e') && e[i])
         {
           tmp_num[j++] = e[i++];
         }
         i--;
         tmp_num[j]=0;
	 m_apm_set_string(curr->v,tmp_num);
	 if (curr->n == NULL)
	 {
	   curr->n = malloc(sizeof(struct hc_stack_element));
	   curr->n->v = m_apm_init();
	   curr->n->n = NULL;
	 }
	 curr->n->p = curr;
	 curr = curr->n;
	 sp++; // only used for syntax checking
         break;
       }
       i++;
     }

     if (sp!=1)
     {
       m_apm_free(op1);m_apm_free(op2);
       while (first->n)
       {
	 m_apm_free(first->v);first = first->n;free(first->p);
       }
       m_apm_free(first->v);free(first);
       syntax_error2();
       return NULL;
     }
     
     curr = curr->p;

     char *result;
     if (hc.sci)
     {
       result = malloc(sizeof(char)*(hc.precision+2+1+hc_need_space_int(m_apm_exponent(curr->v))+1)); // x.hc.precisionEexponent + null char
#ifdef DBG
       printf("hc_postfix_result allocated ==> %i\n",hc.precision+2+1+hc_need_space_int(m_apm_exponent(curr->v))+1);
#endif
       m_apm_to_string(result,hc.precision,curr->v);
     } else {
       result = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,curr->v,'.',0,0);
     }
     m_apm_free(op1);
     m_apm_free(op2);

     while (first->n)
     {
       m_apm_free(first->v);
       first = first->n;
       free(first->p);
     }

     m_apm_free(first->v);
     free(first);

     return result;
}


// return 0 if e contains a (detected) syntax error
char hc_check(char *e)
{
  /*int i = 0;
  int par = 0;
  while (e[i])
  {
    if (e[i]=='(')
      par++;
    if (e[i]==')')
      par--;
    if ((e[i]=='.') && (i!=0) && (!isoperator(e[i-1])) && (!isdigit(e[i-1])))
      return 0;
    i++;
  }

  if (par)
    return 0;
  */
  if (hc.rpn)
  {
    char *tmp = strchr(e,'(');
    while (tmp)
    {
      if (!isalpha((int)*(tmp-sizeof(char))))
	return 0;
      tmp = strchr(tmp+sizeof(char),'(');
    }
    return 1;
  } else {
    int i;
    char last_was_op=0;
    char last_was_char=0;
    char last_was_int=0;
    char first=0;
    int left_par=0,right_par=0;
    for (i=0;i<strlen(e);i++)
    {
      if (!isspace(e[i]) && (first==0))
      {
	if ((isoperator_np(e[i])) && (e[i]!='-'))
	{
	  return 0;
	} else {
	  first = 1;
	}
      }
      if ((!isalnum(e[i])) && (!isoperator(e[i])) && (e[i]!='^') && (e[i]!='.') && !isspace(e[i]) && (e[i]!=',') && (e[i]!='x'))
      {
	return 0;
      }
      if (isoperator_np(e[i]))
      {
	if ((last_was_op) && (last_was_op!='!') && ((last_was_op!='+')&&(e[i]!='-')))
	{
	  return 0;
	}
	if ((last_was_op=='+') && (e[i]=='+'))
	{
	  return 0;
	}
	last_was_op = e[i];
      } else {
	if (!isspace(e[i]))
	{
	  last_was_op = 0;
	  if (first)
	    first = 2;
          if (last_was_int && isalpha(e[i]) && tolower(e[i])!='e')
	  {
	    return 0;
	  }
	}
      }
      if (e[i]=='(')
	left_par++;
      if (e[i]==')')
	right_par++;

      if (isalpha(e[i]))
	last_was_char = 1;
      else
	last_was_char = 0;
      if (isdigit(e[i]) || e[i]=='.')
	last_was_int = 1;
      else
	last_was_int = 0;
    }
    
    if (left_par != right_par)
      return 0;
    
    if (first == 1)
      return 0;
    
    return 1;
  }
}


char *hc_plusminus(char *f)
{
#ifdef DBG
  printf("hc_plusminus start %s\n",f);
#endif
  // Rules:
  //  1) If minus occurs first on input, prepend a 0
  //  2) If + & - appear next to each other, figure out the resulting number
  char *e = malloc(MAX_EXPR);
  if (!e)
    mem_error();
  if (strlen(f)>MAX_EXPR-1)
    overflow_error();
  strcpy(e,f);

  int i=0,j=0;
  int count=0;
  char new[MAX_EXPR];
  memset(&new,0,MAX_EXPR);
  char last_was_op=0;

  while (e[i]!=0)
  {
    if (!isspace(e[i]))
    {
      if (count == 0)
      {
	if (e[i] == '-')
	{
	  if (!hc.rpn)
	  {
	    strcpy(new,"(0-");
	    j = strlen(new);
	  }
	  i++;
	  if (isdigit(e[i]) || e[i]=='.')
	  {
	    // number
	    while (isdigit(e[i]) || e[i]=='.' || tolower(e[i])=='e' ||
		   (isoperator(e[i]) && (tolower(e[i-1])=='e')))
	      new[j++] = e[i++];
	  } else {
	    // function / variable
	    char end=0;
	    char left_par=0,right_par=0;
	    if (!isalpha(e[i+1]))
	    {
	      // variable
	      new[j++] = e[i++];
	    } else {
	      // function
	      while (!end)
	      {
		if (e[i]=='(')
		  left_par++;
		if (e[i]==')')
		  right_par++;
		new[j++] = e[i++];
		if ((right_par == left_par) && (right_par))
		  end = 1;
	      }
	    }
	  }
	  if (!hc.rpn)
	  {
	    new[j++] = ')';
	  } else {
	    new[j++] = '-';
	  }
	  new[j] = 0;
	  strcat(new,e + sizeof(char) * (i));
	  free(e);
	  return hc_plusminus(new);
	}
      } else {
	if (e[i]=='-')
	{
	  if (last_was_op)
	  {
	    strncpy(new,e,i);
	    if (!hc.rpn)
	    {
	      strcat(new,"(0-");
	      j = strlen(new);
	    } else {
	      strcat(new,"0 ");
	      j = strlen(new);
	    }
	    i++;
	    if (isdigit(e[i]) || e[i]=='.')
	    {
	      // number
	      while (isdigit(e[i]) || e[i]=='.' || tolower(e[i])=='e' ||
		     (isoperator(e[i]) && (tolower(e[i-1])=='e')))
		new[j++] = e[i++];
	    } else {
	      // function
	      char end=0;
	      char left_par=0,right_par=0;
	      if (!isalpha(e[i+1]))
	      {
		// variable
		new[j++] = e[i++];
	      } else {
		while (!end)
		{
		  if (e[i]=='(')
		    left_par++;
		  if (e[i]==')')
		    right_par++;
		  new[j++] = e[i++];
		  if ((right_par == left_par) && (right_par))
		    end = 1;
		}
	      }
	    }
	    if (!hc.rpn)
	    {
	      new[j++] = ')';
	    } else {
	      new[j++] = '-';
	    }
	    new[j] = 0;
	    strcat(new,e + sizeof(char) * (i));
	    free(e);
	    return hc_plusminus(new);
	  } else {
	    last_was_op = 1;
	  }
	} else {
	  if (isoperator_np(e[i]) || e[i]=='(')
	    last_was_op = 1;
	  else
	    last_was_op = 0;
	}
      }
      count++;
    }
    i++;
  }

  return e;
}


void hc_process_direction(char *d)
{
  int i=0;
  while ((d[i]!=0) && (d[i]!=' '))
    i++;
  d[i]=0;

  struct hc_ventry *hc_var_tmp;

  switch (simple_hash(d))
  {
  case HASH_RPN:
    hc.rpn = hc.rpn==TRUE ? FALSE : TRUE;
    if (hc.rpn == TRUE)
      notify("RPN mode is now on.")
    else
      notify("RPN mode is now off.");
    break;
  case HASH_SCI:
    hc.sci = hc.sci==TRUE ? FALSE : TRUE;
    if (hc.sci == TRUE)
      notify("SCI mode is now on.")
    else
      notify("SCI mode is now off.");
    break;
  case HASH_BPN:
    hc.bypass_nested = hc.bypass_nested==TRUE ? FALSE : TRUE;
    if (hc.bypass_nested == TRUE)
      notify("Bypass is now on.")
    else
      notify("Bypass is now off.");
    break;
  case HASH_PRECISION:
  case HASH_PRECISION_SHORT:
    if (atoi(&d[i+1]) < 0)
    {
      error_nq("Precision needs to be zero or a positive number.");
      break;
    }
    hc.precision = atoi(&d[i+1]);
    /*if (hc.precision>HC_MAX_PRECISION)
    {
      printf("Precision is limited to %i, setting to this value.",HC_MAX_PRECISION);
      hc.precision = HC_MAX_PRECISION;
    } else {*/
      printf("Precision is now set to %i decimal places.",hc.precision);
      //}
    break;
  case HASH_DEG:
    hc.angle = 'd';
    printf("Angle format is now set to DEG.");
    break;
  case HASH_RAD:
    hc.angle = 'r';
    printf("Angle format is now set to RAD.");
    break;
  case HASH_GRAD:
    hc.angle = 'g';
    printf("Angle format is now set to GRAD.");
    break;
  case HASH_CLEAR:
    hc_var_tmp = hc_var_first;
    while (hc_var_tmp->next)
    {
      hc_var_first = hc_var_tmp->next;
      if (hc_var_tmp->name)
	free(hc_var_tmp->name);
      if (hc_var_tmp->value)
	free(hc_var_tmp->value);
      if (hc_var_tmp->args)
	free(hc_var_tmp->args);
      free(hc_var_tmp);
      hc_var_tmp = hc_var_first;
    }
    free(hc_var_tmp);
    hc_var_first = malloc(sizeof (struct hc_ventry));
    if (!hc_var_first)
      mem_error();
    hc_var_first->next = NULL;
    hc_var_first->type = 0;
    hc_var_first->value = NULL;
    hc_var_first->name = NULL;
    hc_var_first->args = NULL;
    printf("All user-defined variables were cleared.");
    break;
  default:
    error_nq("Unknown command.");
    break;
  }
}


char *hc_cfg_get_fn()
{
  static char *fn = NULL;
  if (!fn)
  {
#ifndef WIN32
    fn = malloc(sizeof(char) * (strlen(getenv("HOME"))+strlen(HC_CFG_RELATIVE)+1)); // /home/me/.hc\0
    if (!fn)
      mem_error();
    strcpy(fn,getenv("HOME"));
    strcat(fn,HC_CFG_RELATIVE);
#else
    fn = malloc(sizeof(char) * (strlen(getenv("APPDATA"))+strlen(HC_CFG_RELATIVE_WIN32)+1));
    if (!fn)
      mem_error();
    strcpy(fn,getenv("APPDATA"));
    strcat(fn,HC_CFG_RELATIVE_WIN32);
#endif
  }
  return fn;
}


void hc_load_cfg()
{
  FILE *fr = fopen(HC_CFG_FILE,"r");
  hc.angle = 'r';
  if (!fr)
  {
    hc.rpn = FALSE;
    hc.precision = 16;
    hc.sci = FALSE;
    hc.keys = TRUE;
    hc.plplot_dev_override = NULL;
  } else {
    char *buffer = malloc(sizeof(char) * HC_CFG_BUF_SIZE);
    char tmp[16];
    if (!buffer)
      mem_error();
    while (fgets(buffer,HC_CFG_BUF_SIZE,fr))
    {
      if (buffer[strlen(buffer)-1]=='\n')
	buffer[strlen(buffer)-1]=0;
      strncpy(tmp,buffer,16);
      int i=0;
      while (tmp[i]!='=' && tmp[i]!=' ' && i<16)
	i++;
      tmp[i]=0;

      switch (simple_hash(tmp))
      {
      case HASH_KEYS:
	if (buffer[i+1]=='1')
	  hc.keys = TRUE;
	else
	  hc.keys = FALSE;
	break;
      case HASH_RPN:
	if (buffer[i+1]=='1')
	  hc.rpn = TRUE;
	else
	  hc.rpn = FALSE;
	break;
      case HASH_SCI:
	if (buffer[i+1]=='1')
	  hc.sci = TRUE;
	else
	  hc.sci = FALSE;
	break;
      case HASH_PRECISION:
	hc.precision = atoi(&buffer[i+1]);
	if (hc.precision>HC_MAX_PRECISION)
	  hc.precision = HC_MAX_PRECISION;
	break;
      case HASH_ANGLE_FORMAT:
	hc.angle = buffer[i+1];
	if (hc.angle!='d' && hc.angle!='r' && hc.angle!='g')
	{
	  warning("configuration file has incorrect angle specification. Setting angle format to RAD.");
	  hc.angle = 'r';
	}
	break;
      case HASH_PLDEV:
	if (buffer[i+1]!='$')
	{
	  hc.plplot_dev_override = malloc(strlen((char *)&buffer[i+1])+1);
	  if (!hc.plplot_dev_override)
	    mem_error();
	  strcpy(hc.plplot_dev_override,(char *)&buffer[i+1]);
	} else {
	  hc.plplot_dev_override = NULL;
	}
	break;
      case HASH_BPN:
	if (buffer[i+1]=='1')
	  hc.bypass_nested = TRUE;
	else
	  hc.bypass_nested = FALSE;
	break;
      }
    }
    free(buffer);
    fclose(fr);
  }

  static char announce = TRUE;
  if (announce == TRUE)
  {
    printf("Configuration :\n Angle format = %s | RPN mode = %s | Precision = %i | SCI mode = %s\n",hc.angle=='r' ? "RAD" : hc.angle=='d' ? "DEG" : "GRAD",hc.rpn==0 ? "off" : "on",hc.precision,hc.sci==0 ? "off" : "on");
    announce = FALSE;
  }
}


void hc_save_cfg()
{
  FILE *fw = fopen(HC_CFG_FILE,"w");
  fprintf(fw,"rpn=%i\n",hc.rpn==0 ? FALSE : TRUE);
  fprintf(fw,"precision=%i\n",hc.precision);
  fprintf(fw,"sci=%i\n",hc.sci==0 ? FALSE : TRUE);
  fprintf(fw,"keys=%i\n",hc.keys==0 ? FALSE : TRUE);
  fprintf(fw,"bpn=%i\n",hc.bypass_nested==0 ? FALSE : TRUE);
  if (hc.plplot_dev_override)
    fprintf(fw,"pldev=%s\n",hc.plplot_dev_override);
  else
    fprintf(fw,"pldev=$\n");
  fprintf(fw,"angle=%c\n",hc.angle);
  fclose(fw);
}
