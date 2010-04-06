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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <m_apm.h>
#include "m_apmc.h"
#include "hc.h"
#include "hc_functions.h"
#include "hash.h"
#include "hc_graph.h"
#include "hc_complex.h"
#define NHASH 29989 // use a large prime number
#define MULT 31


const char *hc_fnames[][2] = {
  {"ans","func"},
  {"abs","func"},
  {"acos","func"},
  {"asin","func"},
  {"atan","func"},
  {"boxplot","func"},
  {"boxplotf","func"},
  {"cbrt","func"},
  {"ceil","func"},
  {"cmtoinch","func"},
  {"cos","func"},
  {"cosh","func"},
  {"ctof","func"},
  {"ctok","func"},
  {"exp","func"},
  {"factorial","func"},
  {"fibo","func"},
  {"floor","func"},
  {"floztoml","func"},
  {"ftoc","func"},
  {"ftok","func"},
  {"fttom","func"},
  {"gcd","func"},
  {"graph","func"},
  {"gmul","func"},
  {"graph3","func"},
  {"inchtocm","func"},
  {"input","func"},
  {"kmtomi","func"},
  {"ktoc","func"},
  {"ktof","func"},
  {"lcm","func"},
  {"ln","func"},
  {"log10","func"},
  {"mitokm","func"},
  {"mltofloz","func"},
  {"mmass","func"},
  {"mod","func"},
  {"mtoft","func"},
  {"nCr","func"},
  {"nPr","func"},
  {"print","func"},
  {"product","func"},
  {"rand","func"},
  {"round","func"},
  {"sin","func"},
  {"sinh","func"},
  {"sqrt","func"},
  {"stats","func"},
  {"statsf","func"},
  {"sum","func"},
  {"tan","func"},
  {"tanh","func"},
  {"totient","func"},
  {"write","func"},
  // CONSTANTS
  {"pi","cnst"},
  {"c","cnst"},
  {"e","cnst"},
  {"phi","cnst"},
  {"g","cnst"},
  {"answer","cnst"},
  // OTHERS
  {"help","cmd"},
  {"credits","cmd"},
  {"constants","cmd"},
  {"conversions","cmd"},
  {"logic","cmd"},
  {"i","sys"},
  // DIRECTIONS/CONFIG (for tab-complete)
  {"\\p","dir"},
  {"\\rpn","dir"},
  {"\\sci","dir"},
  {"\\eng","dir"},
  {"\\normal","dir"},
  {"\\deg","dir"},
  {"\\rad","dir"},
  {"\\grad","dir"},
  {"\\clear","dir"},
  {"\\load","dir"},
  {"\\3dpoints","dir"}
};


unsigned int hc_hashes[HC_FNAMES];


unsigned int simple_hash(char *p)
{
  unsigned int h = 0;
  for(; *p; p++)
    h = MULT * h + tolower(*p);
  return h % NHASH;
}


#define isoperator(c) ((c=='*') || (c=='/') || (c=='+') || (c=='-') || (c=='(') || (c==')') || (c==PW_SIGN) || (c=='$') || (c==',') || (c=='!') || (c=='%') || (c=='_') || (c=='<') || (c=='>') || (c=='=') || (c=='&') || (c=='|'))
#define isoperator_np(c) ((c=='*') || (c=='/') || (c=='+') || (c=='-') || (c=='$') || (c==PW_SIGN) || (c==',') || (c=='!') || (c=='%') || (c=='_') || (c=='<') || (c=='>') || (c=='=') || (c=='&') || (c=='|'))
#define isdirection(x) (x[0]=='\\')
#define isvarassign(x) (strchr(x,'=')!=NULL && strchr(x,'=')[1]!='=' && strchr(x,'=')!=x && (strchr(x,'=')-1)[0]!='<' && (strchr(x,'=')-1)[0]!='>' && (strchr(x,'=')-1)[0]!='!')
#define iscondition(x) (strstr(x,"==")!=NULL || strstr(x,"!=")!=NULL || strstr(x,">=")!=NULL || strstr(x,"<=")!=NULL || strstr(x,"<")!=NULL || strstr(x,">")!=NULL)
#define iscontrolstruct(x) ((strstr(x,"if ")==x) || (strstr(x,"while")==x) || (strstr(x,"for")==x))


char *hc_i2p(char *f);
char *hc_postfix_result(char *e);
char hc_check(char *e);
char hc_is_predef(char *var);
char *hc_plusminus(char *e);
char *hc_impmul_resolve(char *e);
char *hc_result_normal(char *f);
void hc_process_direction(char *d);
void hc_load_cfg();
void hc_save_cfg();
void hc_cleanup();
void hc_load(char *fname);


M_APM hc_lans_mapm_re;
M_APM hc_lans_mapm_im;
M_APM MM_MOne; // Minus one
struct hc_config hc;
char announce_errors=TRUE;
char graphing_ignore_errors=FALSE;
int hc_nested;


char *hc_result(char *e)
{
  announce_errors = TRUE; // This variable is used so that syntax errors etc. are not announced multiple times if multiple errors are detected
  hc_nested = 0; // This is set to prevent infinite recursion

  static char init=0;
  if (!init)
  {
    atexit(hc_cleanup);
    hc_load_cfg();
    hc_lans_mapm_re = m_apm_init();
    hc_lans_mapm_im = m_apm_init();
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
    m_apmc_init();
    memset(hc_hashes,0,HC_FNAMES * sizeof(unsigned int));
    init = 1;
  }

  m_apm_trim_mem_usage();

  if (strcmp(e,"help")==0)
  {
    printf("Welcome to HC!\nTo calculate an expression, simply enter it on the command-line in infix notation (RPN is also available, to enable it, type \\rpn) and press enter (available operators : + - * / % ^ ! < > <= >= == != &[&] (boolean and) |[|] (boolean or)). To get more information about boolean/logic operators and control structures (programming), type \"logic\".\nTo use a constant, simply type its name with english characters - for example pi. For more info type \"constants\".\nHC also supports complex numbers, type \"complex\" for more info.\nTo draw a graph, use the graph() function (gmul() for multiple functions on one graph) or the graph3() function; see below for more info.\nYou can also use a few functions; the list of them is below:\n - ans() - Will be replaced by the result of the last entered expression\n - abs(x) - absolute value of x\n - acos(x) - arc cosine of x\n - asin(x) arc sine of x\n - atan(x) - arc tangent of x\n - boxplot(v1,v2,...,vn) - draw a boxplot of v1...vn\n - boxplotf(v1,freq1,v2,freq2,...,...,vn,freqn) - same as boxplot, but after each value, you have to append its frequency\n - cbrt(x) - returns the cube root of x (you could also use x^(1/3))\n - ceil(x) - returns x rounded to the nearest integer upwards\n - cmtoinch(x) - converts x, which has to be in cm, to inches\n - cos(x) - cosine of x\n - cosh(x) - hyperbolic cosine of x\n - ctof(x) - converts x, which has to be in degrees Celsius, to Fahrenheit\n - ctok(x) - converts x, which has to be in degrees Celsius, to Kelvin\n - exp(x) - exponential function\n - factorial(x) - factorial of x (you can also use the '!' notation)\n - fibo(x) - returns the xth term of the Fibonacci sequence\n - floor(x) - returns x rounded to the nearest integer downwards\n - floztoml(x) - converts x, which has to be in US fluid ounces, to ml\n - ftoc(x) - converts x, which has to be in Fahrenheit, to degrees Celsius\n - ftok(x) - converts x, which has to be in Fahrenheit, to Kelvin\n - fttom(x) - converts x, which has to be in feet, to meters\n - gcd(x,y) - finds the greatest common divisor of x and y\n - gmul(expr_1, expr_2, ..., expr_n) - similar to graph, but can draw an arbitrary number of functions on one graph (note : to set xmin, etc., make a call to graph() first)\n - graph(expr[,xmin,xmax,ymin,ymax]) - draw a graph (generate a PNG file if in command-line), example syntax:\n     graph(x^2,-10,10,-100,100)\n    Note: the xmin, xmax, ymin and ymax arguments are optional; if not provided, the last will be used (ie after a call of the example, graph(something) would use -10, 10, -100 and 100\n - graph3(expr[,xmin,xmax,ymin,ymax,zmin,zmax]) - draw a 3D graph (generate a PNG file if in command-line), example syntax:\n     graph3(x+y,-10,10,-10,10,-20,20)\n    Note: as with graph(), the xmin, xmax, ymin, ymax, zmin and zmax are optional\n - inchtocm(x) - converts x, which has to be in inches, to cm\n - input([str]) - shows an inut dialog using either a standard prompt or str, if specified\n - kmtomi(x) - converts x, which has to be in km, to miles\n - ktoc(x) - converts x, which has to be in Kelvin, to degrees Celsius\n - ktof(x) - converts x, which has to be in Kelvin, to Fahrenheit\n - lcm(x,y) - finds the least common multiple of x and y\n - ln(x) - natural logarithm\n - log10(x) - common logarithm\n - mitokm(x) - converts x, which has to be in miles, to km\n - mltofloz(x) - converts x, which has to be in ml, to US fluid ounces\n - mmass(molecule) - returns the molar mass of the molecule specified (use standard notation without using subscripts, such as H2O or Ca(OH)2)\n - mod(x,y) - modulus (you can also use the C-style '%%' notation)\n - mtoft(x) - converts x, which has to be in m, to feet\n - nCr(n,k) - binomial coefficient (combinatorics)\n - nPr(n,k) - number of permutations (combinatorics)\n - print(expr_1[,expr_2,...,expr_n]) - prints its number or string arguments (if more than one, separated by a space) and a newline character (useful for printing variables in loops)\n - product(expr,low,high) - returns the product of expr with x from low to high inclusive (WARNING : As of now, other variable names will be ignored)\n - rand([int]) - if int is specified, return a random integer in the range [1 ; int], otherwise return a real number in the range [0;1[\n - round(x) - round x to the nearest integer (for .5 cases, away from zero)\n - sin(x) - sine of x\n - sinh(x) - hyperbolic sine of x\n - sqrt(x) - returns the square root of x\n - stats(v1,v2,...,vn) - show stats info of v1...vn\n - statsf(v1,freq1,v2,freq2,...,...,vn,freqn) - same as stats, but after each value, you have to append its frequency\n - sum(expr,low,high) - same as product, except the result is summation, not multiplication\n - tan(x) - tangent of x\n - tanh(x) - hyperbolic tangent of x\n - totient(x) - Euler's totient function\n - write(fname,expr_1[expr_2,...,expr_n]) - behaves like the print function, but writes to the file specified with fname\nYou can also use these directions/configuration commands:\n - \\p PRECISION - change current precision to PRECISION decimal places\n - \\rpn - change modes (RPN / infix)\n - \\sci, \\eng, \\normal - switch between scientific, engineering, and normal exponentials formats for output\n - \\deg, \\rad, \\grad - switch between DEG, RAD, and GRAD modes for angles\n - \\clear - clear all user-defined variables and functions\n - \\3dpoints SIDE - setup the 3D graphing function to compute a square of SIDE^2 points\n - \\load FILE - load and execute lines in FILE. You can use this for example to define functions you use often etc. (Note : you do not need to use this to load other configuration - it is saved automatically)\nTo get a list of only the converter functions, type \"conversions\".\nTo read the license and credits, type \"credits\".\nTo exit The HoubySoft Calculator, just type \"exit\" in the prompt (you can also use CTRL+C on systems that support it, but configuration will not be saved if you do that).\n");
    return NULL;
  }

  if (strcmp(e,"conversions")==0)
  {
    printf("List of conversion functions:\n - cmtoinch(x) - converts x, which has to be in cm, to inches\n - ctof(x) - converts x, which has to be in degrees Celsius, to Fahrenheit\n - ctok(x) - converts x, which has to be in degrees Celsius, to Kelvin\n - floztoml(x) - converts x, which has to be in US fluid ounces, to ml\n - ftoc(x) - converts x, which has to be in Fahrenheit, to degrees Celsius\n - ftok(x) - converts x, which has to be in Fahrenheit, to Kelvin\n - fttom(x) - converts x, which has to be in feet, to meters\n - inchtocm(x) - converts x, which has to be in inches, to cm\n - kmtomi(x) - converts x, which has to be in km, to miles\n - ktoc(x) - converts x, which has to be in Kelvin, to degrees Celsius\n - ktof(x) - converts x, which has to be in Kelvin, to Fahrenheit\n - mitokm(x) - converts x, which has to be in miles, to km\n - mltofloz(x) - converts x, which has to be in ml, to US fluid ounces\n - mtoft(x) - converts x, which has to be in m, to feet\n");
    return NULL;
  }

  if (strcmp(e,"constants")==0)
  {
    printf("To use constants, just type their name with english characters. Here is a list of available constants:\n - answer - The Answer\n - c - speed of light\n - e - Euler's number\n - g - standard gravitational acceleration on Earth\n - phi - Golden ratio\n - pi\n");
    return NULL;
  }

  if (strcmp(e,"credits")==0)
  {
    printf("HC was developed by Jan Dlabal (visit http://houbysoft.com/en/about.php for contact information) and is (C) 2009-2010.\nThe homepage of this program is http://hc.houbysoft.com/.\nThis program was released under the GNU GPL v3 License. Visit gnu.org for more information.\n");
    return NULL;
  }

  if (strcmp(e,"complex")==0)
  {
    printf("Complex numbers are inputted and outputted with this syntax:\nreal_part i imaginary_part (without any spaces, they are added here for clarity)\n\nFor example, 5 + 3i would be written as\n5i3\nin HC. This format is inspired by the J programming language.\nAs another example, you can try to type\nsqrt(-1)\nin HC. The result given will be:\n0i1\n\nOther notes\n* Trigonometry functions will refuse to work with complex numbers unless you are in RAD mode (switch to it by typing \\rad).\n* You cannot type a non-number as the real part or imaginary part. Therefore, you cannot write something like 0ipi, if you wanted to have a number pi*i. To express this number you would have to use something like:\n0i1*pi\n");
    return NULL;
  }

  if (strcmp(e,"logic")==0)
  {
    printf("This version of HC supports the following logic/boolean operators : < > <= >= == != &[&] (AND) |[|] (OR). For AND and OR, both the C-style && / || and single & / | can be used.\nThese operators return 1 (true) or 0 (false). In general in HC, 0 means false, everything else means true.\n\nThis version of HC supports the following control structures : if while for. The syntax is somewhat similar to C:\nif (condition) { cmd_1; cmd_2; ...; cmd_n; } else { else_cmd_1; else_cmd_2; ...; else_cmd_n; }\nwhile (condition) { cmd_1; cmd_2; ...; cmd_n; }\nfor (start_cmd; condition; iter_cmd) { cmd_1; cmd_2; ...; cmd_n; }\nFor the \"if\" structure, the \"else\" clause is optional. The { } part of each structure can have an arbitrary number of commands or nested structures.\nFor more information and examples about logic in HC, visit http://houbysoft.com/hc/logic.php .\n");
    return NULL;
  }

  char *r = hc_result_(e);
  if (r && strlen(r) && !is_string(e))
  {
    char *tmp_num = hc_real_part(r);
    m_apm_set_string(hc_lans_mapm_re,tmp_num);
    free(tmp_num);
    tmp_num = hc_imag_part(r);
    if (tmp_num)
    {
      m_apm_set_string(hc_lans_mapm_im,tmp_num);
      free(tmp_num);
    }
    char *r_re,*r_im;
    r_re = hc_real_part(r); r_im = hc_imag_part(r);
    free(r);
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
  }
  return r;
}

char *hc_result_(char *e)
{
  if (!e)
    return NULL;
  e = strip_spaces(e);
  if (!e[0]) // empty string
    return NULL;
  char *r=NULL;
  
  if (iscontrolstruct(e) || (strchr(e,';') && !(isvarassign(e))))
  {
    if (iscontrolstruct(e))
    {
      r = hc_exec_struct(e);
      if (!r)
      {
	r = malloc(1);
	r[0]=0;
      }
      return r;
    } else {
      return hc_result_mul(e);
    }
  } else {
    if (isvarassign(e))
    {
      if (strchr_outofblock(e,';'))
	return hc_result_mul(e);
      else
      {
	char *f = malloc(strlen(e)+1);
	if (!f)
	  mem_error();
	strcpy(f,e);
	hc_varassign(f);
	free(f);
	r = malloc(1);
	if (!r)
	  mem_error();
	r[0]=0;
      }
    } else {
      if (isdirection(e))
      {
	char *f = malloc(strlen(e)+1);
	if (!f)
	  mem_error();
	strcpy(f,e);
	hc_process_direction((char *)f+1);
	free(f);
	r = malloc(1);
	*r = 0;
	if (!r)
	  mem_error();
      } else {
	r = hc_result_normal(e);
      }
    }
  }
  
  return r;
}


// do NOT call this if you want to use the hc core in your own open source project, call hc_result() instead
char *hc_result_normal(char *f)
{
  if (!f)
    return NULL;
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

  char *e = strdup(f);
  if (!e)
    mem_error();

  char *e_fme = e = hc_impmul_resolve(e);

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

  int pos_cur=0,pos_beg;
  char couldbevar=0;
  char ignore = FALSE;
  while (e[pos_cur])
  {
    if (e[pos_cur]=='\"')
    {
      ignore = ignore == FALSE ? TRUE : FALSE;
      pos_cur++;
      continue;
    }
    if (ignore)
    {
      pos_cur++;
      continue;
    }
    if (((isalpha(e[pos_cur]) && (tolower(e[pos_cur])!='e' || (isalpha(e[pos_cur+1]))) && (tolower(e[pos_cur]!='i') || (isalpha(e[pos_cur+1])))) || (isalnum(e[pos_cur]) && (couldbevar==1))) && e[pos_cur+1]!='\0')
    {
      if (!couldbevar)
      {
	couldbevar = 1;
	pos_beg = pos_cur;
      }
    } else {
      if (((isalpha(e[pos_cur]) && tolower(e[pos_cur])!='e' && tolower(e[pos_cur]!='i')) || (isalnum(e[pos_cur])&& (couldbevar==1)))&& e[pos_cur+1]=='\0')
      {
	if (!couldbevar)
	{
	  couldbevar = 1;
	  pos_beg = pos_cur;
	}
	pos_cur++;
      }
      if (couldbevar == 1)
      {
	char type = e[pos_cur]=='(' ? HC_USR_FUNC : HC_USR_VAR;
	char *tmp = malloc(sizeof(char)*(pos_cur-pos_beg+1));
	strncpy(tmp,e + sizeof(char)*pos_beg,pos_cur-pos_beg);
	tmp[pos_cur-pos_beg]=0;
	if (!hc_is_predef(tmp))
	{
	  char done = 0;
	  struct hc_ventry *var_tmp = hc_var_first;
	  if (var_tmp->name == NULL)
	    var_tmp = NULL;
	  while (var_tmp && done==0)
	  {
	    if (var_tmp->type == type && strcmp(var_tmp->name,tmp)==0)
	    {
	      done = 1;
	      pos_cur = -1; // Need to restart scanning (the string will change after replacement)
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
#ifdef DBG
		  printf("User-defined function found\n");
#endif
		  unsigned int i=0;
		  while (isalnum(ffound[i]))
		    i++;
		  if (ffound[i]!='(')
		  {
		    syntax_error2();
		    free(e);
		    free(tmp);
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
			free(tmp);
			free(args);
			hc_nested--;
			return NULL;
		      }
		      j++;
		    }
		    unsigned int f_expr_e = (ffound - e) + i + j + 1;
		    args[j]=0;
#ifdef DBG
		    printf("args - %s\n",args);
#endif
		    char *custom_f_expr = malloc((strlen(var_tmp->value)+1)*sizeof(char));
		    if (!custom_f_expr)
		      mem_error();
		    strcpy(custom_f_expr,var_tmp->value);
		    unsigned int k = 1;
		    char xybypass = 0; // bypass for variables x and y, which can be part of the expression in certain cases (ie sum(), product(), graphs...)
		    while (k!=-1)
		    {
		      char *tmp_args = malloc(strlen(args));
		      if (!tmp_args)
			mem_error();
		      strcpy(tmp_args,(char *)(args+sizeof(char)));
		      tmp_args[strlen(tmp_args)-1]=0;
		      char *t1 = hc_get_arg(tmp_args,k);
		      free(tmp_args);
#ifdef DBG
		      printf("t1 = %s\n",t1);
#endif
		      if (t1)
		      {
			char *t1fme = t1;
			char a_tmp = announce_errors;
			announce_errors = FALSE;
			if (strcmp(t1,"x")==0 || strcmp(t1,"y")==0)
			{
			  t1 = malloc(2);
			  strcpy(t1,t1fme);
			  xybypass = 1;
			} else {
			  t1 = hc_result_(t1);
			  xybypass = 0;
			}
			announce_errors = a_tmp;
			if (t1)
			{
#ifdef DBG
			  printf("t1 is now %s\n",t1);
#endif
			  free(t1fme);
			} else {
			  free(tmp);
			  free(e);
			  free(custom_f_expr);
			  free(args);
			  hc_nested--;
			  free(t1fme);
			  return NULL;
			}
		      }
		      char *t2 = hc_get_arg(var_tmp->args,k);
		      if (t1 && t2)
		      {
			char *fme = custom_f_expr;
			if (strcmp(t1,t2)!=0)
			{
			  custom_f_expr = strreplace(custom_f_expr,t2,t1);
			  free(fme);
			}
			free(t1);
			free(t2);
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
			  free(tmp);
			  hc_nested--;
			  return NULL;
			} else {
			  char *fme_cfe = custom_f_expr;
			  if (!xybypass)
			  {
			    custom_f_expr = hc_result_(custom_f_expr);
			    free(fme_cfe);
			  }
			  if (!custom_f_expr)
			  {
			    free(e);
			    free(args);
			    free(tmp);
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
			  free(tmp);
			  hc_nested--;
			  return rme;
			}
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
	  if (!done && strcmp(tmp,"x")!=0 && strcmp(tmp,"y")!=0)
	  {
	    unknown_var_error(tmp,type);
	    free(tmp);
	    free(e);
	    hc_nested--;
	    return NULL;
	  }
	}
	free(tmp);
      }
      couldbevar = 0;
    }
    pos_cur++;
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

  char f_name[MAX_F_NAME];
  char f_expr[MAX_F_TMP];
  char e_tmp[MAX_EXPR];
  char *tmp1 = hc_plusminus(e);
  strcpy(e_tmp,tmp1);
  free(tmp1);
  strcpy(e,e_tmp);
  int i,j,k;
  int left_par=0,right_par=0,f_expr_s=0,f_expr_e=0;
  ignore = FALSE;

  // Find & replace functions with their results
  for (i=0;i<MAX_EXPR;i++)
  {
    if (e[i]=='\"')
    {
      ignore = ignore == FALSE ? TRUE : FALSE;
      continue;
    }
    if (ignore)
      continue;
    if (e[i]==0)
    {
      i = MAX_EXPR;
      break;
    }
    if (isalpha(e[i]) && (isalpha(e[i+1]) || e[i+1]=='(' || e[i+1]==0))
      break;
    if (isalpha(e[i]) && (tolower(e[i])!='i' || (tolower(e[i])=='i' && isalpha(e[i+1]))) && (tolower(e[i])!='e' || (tolower(e[i])=='i' && isalpha(e[i+1]))) && (isdigit(e[i+1])))
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
    M_APM f_result_re = m_apm_init();
    M_APM f_result_im = m_apm_init();
    // we need to replace a function with its result
    // e[i] points to the start of the function
    j = i;
    k = 0;
    while (e[j]!='(')
    {
      f_name[k++] = e[j];
      if (k >= MAX_F_NAME)
      {
	m_apm_free(f_result_re);m_apm_free(f_result_im);
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
    ignore = FALSE;
    while ((left_par != right_par) || (left_par==0))
    {
      j++;
      if (e[j]=='\"')
      {
	ignore = ignore == FALSE ? TRUE : FALSE;
	continue;
      }
      if (ignore)
	continue;
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
    M_APM tmp_num_re = m_apm_init();
    M_APM tmp_num_im = m_apm_init();
    char *tmp_ri; // tmp for real/imaginary parts of the number

    switch (simple_hash(f_name))
    {
    case HASH_ABS:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
	m_apm_set_string(tmp_num_im,tmp_ri);
	free(tmp_ri);
	m_apm_set_string(f_result_im,"0");
	m_apmc_abs(f_result_re,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      } else {
	m_apm_set_string(f_result_im,"0");
	m_apm_absolute_value(f_result_re,tmp_num_re);
      }
      break;

    case HASH_FLOOR:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
	m_apm_set_string(tmp_num_im,tmp_ri);
	free(tmp_ri);
      } else {
	m_apm_set_string(f_result_im,"0");
      }
      m_apmc_floor(f_result_re,f_result_im,tmp_num_re,tmp_num_im);
      break;

    case HASH_CEIL:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
	m_apm_set_string(tmp_num_im,tmp_ri);
	free(tmp_ri);
      } else {
	m_apm_set_string(f_result_im,"0");
      }
      m_apmc_ceil(f_result_re,f_result_im,tmp_num_re,tmp_num_im);
      break;

    case HASH_ROUND:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
	m_apm_set_string(tmp_num_im,tmp_ri);
	free(tmp_ri);
      } else {
	m_apm_set_string(f_result_im,"0");
      }
      m_apmc_round(f_result_re,f_result_im,tmp_num_re,tmp_num_im);
      break;

    case HASH_ANS:
      m_apm_copy(f_result_re,hc_lans_mapm_re);
      m_apm_copy(f_result_im,hc_lans_mapm_im);
      break;

    case HASH_ACOS:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
	m_apm_set_string(tmp_num_im,tmp_ri);
	free(tmp_ri);
      } else {
	m_apm_set_string(tmp_num_im,"0");
      }
      m_apmc_acos(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      if (m_apm_compare(f_result_im,MM_Zero)!=0 && hc.angle!='r')
      {
	m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
	m_apm_free(f_result_re); m_apm_free(f_result_im);
	free(e);
	hc_nested--;
	arg_error("acos() : Domain error. Please switch to RAD mode with \\rad for complex results.");
      } else {
	hc_from_rad(f_result_re);
      }
      break;
      
    case HASH_ASIN:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
	m_apm_set_string(tmp_num_im,tmp_ri);
	free(tmp_ri);
      } else {
	m_apm_set_string(tmp_num_im,"0");
      }
      m_apmc_asin(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      if (m_apm_compare(f_result_im,MM_Zero)!=0 && hc.angle!='r')
      {
	m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
	m_apm_free(f_result_re); m_apm_free(f_result_im);
	free(e);
	hc_nested--;
	arg_error("asin() : Domain error. Please switch to RAD mode with \\rad for complex results.");
      } else {
	hc_from_rad(f_result_re);
      }
      break;

    case HASH_ATAN:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
	m_apm_set_string(tmp_num_im,tmp_ri);
	free(tmp_ri);
      } else {
	m_apm_set_string(tmp_num_im,"0");
      }
      m_apmc_atan(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      if (m_apm_compare(f_result_im,MM_Zero)!=0 && hc.angle!='r')
      {
	m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
	m_apm_free(f_result_re); m_apm_free(f_result_im);
	free(e);
	hc_nested--;
	arg_error("atan() : Domain error. Please switch to RAD mode with \\rad for complex results.");
      } else {
	hc_from_rad(f_result_re);
      }
      break;

    case HASH_COS:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
	m_apm_set_string(tmp_num_im,tmp_ri);
	free(tmp_ri);
      } else {
	m_apm_set_string(tmp_num_im,"0");
      }
      if (m_apm_compare(tmp_num_im,MM_Zero)!=0 && hc.angle!='r')
      {
	m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
	m_apm_free(f_result_re); m_apm_free(f_result_im);
	free(e);
	hc_nested--;
	arg_error("cos() : Domain error. Please switch to RAD mode with \\rad for complex results.");
      } else {
	hc_to_rad(tmp_num_re);
        m_apmc_cos(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      }
      break;

    case HASH_EXP:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
	m_apm_set_string(tmp_num_im,tmp_ri);
	free(tmp_ri);
      } else {
	m_apm_set_string(tmp_num_im,"0");
      }
      m_apmc_exp(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      break;

    case HASH_FACTORIAL:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
	m_apm_set_string(tmp_num_im,tmp_ri);
	free(tmp_ri);
      } else {
	m_apm_set_string(tmp_num_im,"0");
      }
      if (!m_apm_is_integer(tmp_num_re) || m_apm_compare(tmp_num_im,MM_Zero)!=0)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); arg_error("factorial() : an integer is required."); hc_nested--; return NULL;}
      m_apm_factorial(f_result_re,tmp_num_re);
      m_apm_set_string(f_result_im,"0");
      break;

    case HASH_GCD:
      if (hc_gcd(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_LCM:
      if (hc_lcm(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_LN:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
	m_apm_set_string(tmp_num_im,tmp_ri);
	free(tmp_ri);
      } else {
	m_apm_set_string(tmp_num_im,"0");
      }
      m_apmc_log(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      break;

    case HASH_LOG10:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
	m_apm_set_string(tmp_num_im,tmp_ri);
	free(tmp_ri);
      } else {
	m_apm_set_string(tmp_num_im,"0");
      }
      m_apmc_log10(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      break;

    case HASH_SIN:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
	m_apm_set_string(tmp_num_im,tmp_ri);
	free(tmp_ri);
      } else {
	m_apm_set_string(tmp_num_im,"0");
      }
      if (m_apm_compare(tmp_num_im,MM_Zero)!=0 && hc.angle!='r')
      {
	m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
	m_apm_free(f_result_re); m_apm_free(f_result_im);
	free(e);
	hc_nested--;
	arg_error("sin() : Domain error. Please switch to RAD mode with \\rad for complex results.");
      } else {
	hc_to_rad(tmp_num_re);
        m_apmc_sin(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      }
      break;

    case HASH_TAN:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
	m_apm_set_string(tmp_num_im,tmp_ri);
	free(tmp_ri);
      } else {
	m_apm_set_string(tmp_num_im,"0");
      }
      if (m_apm_compare(tmp_num_im,MM_Zero)!=0 && hc.angle!='r')
      {
	m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
	m_apm_free(f_result_re); m_apm_free(f_result_im);
	free(e);
	hc_nested--;
	arg_error("tan() : Domain error. Please switch to RAD mode with \\rad for complex results.");
      } else {
	hc_to_rad(tmp_num_re);
        m_apmc_tan(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      }
      break;

    case HASH_SUM:
      if (hc_sum(f_result_re,f_result_im,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_PRODUCT:
      if (hc_product(f_result_re,f_result_im,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_COSH:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
	m_apm_set_string(tmp_num_im,tmp_ri);
	free(tmp_ri);
      } else {
	m_apm_set_string(tmp_num_im,"0");
      }
      if (m_apm_compare(tmp_num_im,MM_Zero)!=0 && hc.angle!='r')
      {
	m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
	m_apm_free(f_result_re); m_apm_free(f_result_im);
	free(e);
	hc_nested--;
	arg_error("cosh() : Domain error. Please switch to RAD mode with \\rad for complex results.");
      } else {
        m_apmc_cosh(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      }
      break;

    case HASH_SINH:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
	m_apm_set_string(tmp_num_im,tmp_ri);
	free(tmp_ri);
      } else {
	m_apm_set_string(tmp_num_im,"0");
      }
      if (m_apm_compare(tmp_num_im,MM_Zero)!=0 && hc.angle!='r')
      {
	m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
	m_apm_free(f_result_re); m_apm_free(f_result_im);
	free(e);
	hc_nested--;
	arg_error("sinh() : Domain error. Please switch to RAD mode with \\rad for complex results.");
      } else {
        m_apmc_sinh(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      }
      break;

    case HASH_TANH:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
	m_apm_set_string(tmp_num_im,tmp_ri);
	free(tmp_ri);
      } else {
	m_apm_set_string(tmp_num_im,"0");
      }
      if (m_apm_compare(tmp_num_im,MM_Zero)!=0 && hc.angle!='r')
      {
	m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
	m_apm_free(f_result_re); m_apm_free(f_result_im);
	free(e);
	hc_nested--;
	arg_error("tanh() : Domain error. Please switch to RAD mode with \\rad for complex results.");
      } else {
        m_apmc_tanh(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      }
      break;

    case HASH_SQRT:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
	m_apm_set_string(tmp_num_im,tmp_ri);
	free(tmp_ri);
      } else {
	m_apm_set_string(tmp_num_im,"0");
      }
      m_apmc_sqrt(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      break;

    case HASH_CBRT:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
	m_apm_set_string(tmp_num_im,tmp_ri);
	free(tmp_ri);
      } else {
	m_apm_set_string(tmp_num_im,"0");
      }
      m_apmc_root(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im,3,3);
      break;

    case HASH_MOD:
      if (hc_modulus(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_FIBO:
      if (hc_fibo(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_TOTIENT:
      if (hc_totient(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_CTOF:
      if (hc_c2f(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_FTOC:
      if (hc_f2c(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_CTOK:
      if (hc_c2k(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_KTOC:
      if (hc_k2c(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_FTOK:
      if (hc_f2k(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_KTOF:
      if (hc_k2f(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_MITOKM:
      if (hc_mi2km(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_KMTOMI:
      if (hc_km2mi(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_MLTOFLOZ:
      if (hc_ml2floz(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_FLOZTOML:
      if (hc_floz2ml(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_INCHTOCM:
      if (hc_inch2cm(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_CMTOINCH:
      if (hc_cm2inch(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_FTTOM:
      if (hc_ft2m(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_MTOFT:
      if (hc_m2ft(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_NCR:
      if (hc_binomc(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_NPR:
      if (hc_permutations(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_RAND:
      if (hc_rand(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_PRINT:
      hc_output(PRINT, f_expr);
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--;
	char *tmp = malloc(1); tmp[0]=0; return tmp;}
      break;

    case HASH_WRITE:
      hc_output(WRITE, f_expr);
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--;
	char *tmp = malloc(1); tmp[0]=0; return tmp;}
      break;

    case HASH_INPUT:
      if (hc_input(f_result_re,f_result_im,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    case HASH_GRAPH:
      hc_graph(f_expr);
      announce_errors = FALSE;
      m_apm_free(tmp_num_re);
      m_apm_free(tmp_num_im);
      m_apm_free(f_result_re);
      m_apm_free(f_result_im);
      free(e);
      hc_nested--;
      e = malloc(1); if (!e) mem_error(); strcpy(e,"");
      return e;
      break;

    case HASH_GMUL:
      hc_graph_n(f_expr);
      announce_errors = FALSE;
      m_apm_free(tmp_num_re);
      m_apm_free(tmp_num_im);
      m_apm_free(f_result_re);
      m_apm_free(f_result_im);
      free(e);
      hc_nested--;
      e = malloc(1); if (!e) mem_error(); strcpy(e,"");
      return e;
      break;

    case HASH_GRAPH3D:
      hc_graph3d(f_expr);
      announce_errors = FALSE;
      m_apm_free(tmp_num_re);
      m_apm_free(tmp_num_im);
      m_apm_free(f_result_re);
      m_apm_free(f_result_im);
      free(e);
      hc_nested--;
      e = malloc(1); if (!e) mem_error(); strcpy(e,"");
      return e;
      break;

    case HASH_STATS:
      hc_stats(f_expr, FALSE, FALSE);
      m_apm_free(tmp_num_re);
      m_apm_free(tmp_num_im);
      m_apm_free(f_result_im);
      m_apm_free(f_result_re);
      free(e);
      hc_nested--;
      e = malloc(1); if (!e) mem_error(); strcpy(e,"");
      return e;
      break;

   case HASH_STATS_EFF:
      hc_stats(f_expr, FALSE, TRUE);
      m_apm_free(tmp_num_re);
      m_apm_free(tmp_num_im);
      m_apm_free(f_result_im);
      m_apm_free(f_result_re);
      free(e);
      hc_nested--;
      e = malloc(1); if (!e) mem_error(); strcpy(e,"");
      return e;
      break;

    case HASH_BOXPLOT:
      hc_stats(f_expr, TRUE, FALSE);
      m_apm_free(tmp_num_re);
      m_apm_free(tmp_num_im);
      m_apm_free(f_result_im);
      m_apm_free(f_result_re);
      free(e);
      hc_nested--;
      e = malloc(1); if (!e) mem_error(); strcpy(e,"");
      return e;
      break;

    case HASH_BOXPLOT_EFF:
      hc_stats(f_expr, TRUE, TRUE);
      m_apm_free(tmp_num_re);
      m_apm_free(tmp_num_im);
      m_apm_free(f_result_im);
      m_apm_free(f_result_re);
      free(e);
      hc_nested--;
      e = malloc(1); if (!e) mem_error(); strcpy(e,"");
      return e;
      break;

    case HASH_MMASS:
      if (hc_mmass(f_result_re,f_expr)==FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); free(e); hc_nested--; return NULL;}
      break;

    default:
      m_apm_free(tmp_num_re);
      m_apm_free(tmp_num_im);
      m_apm_free(f_result_re);
      m_apm_free(f_result_im);
      free(e);
      undef_error();
      hc_nested--;
      return NULL;
    }

    
    strncpy(e,e_tmp,i);
    e[i]=0;
    char *f_result_tmp_re = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,f_result_re,'.',0,0);
    if (f_result_tmp_re[0]=='-')
      f_result_tmp_re[0] = '_';
    char *f_result_tmp_im,*f_result_tmp;
    if (m_apm_compare(f_result_im,MM_Zero)!=0)
    {
      f_result_tmp_im = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,f_result_im,'.',0,0);
      if (f_result_tmp_im[0]=='-')
	f_result_tmp_im[0] = '_';
      f_result_tmp = malloc(strlen(f_result_tmp_re)+1+strlen(f_result_tmp_im)+1);
                     // re i im \0
      strcpy(f_result_tmp,f_result_tmp_re);
      strcat(f_result_tmp,"i");
      strcat(f_result_tmp,f_result_tmp_im);
      free(f_result_tmp_re); free(f_result_tmp_im);
    } else {
      f_result_tmp = f_result_tmp_re;
    }
    if (strlen(e_tmp)+strlen(f_result_tmp)+strlen(((char *)&e_tmp)+f_expr_e+1)+1>MAX_EXPR)
      overflow_error();
    //m_apm_to_string(f_result_tmp,HC_DEC_PLACES,f_result);
    strcat(e,f_result_tmp);
    free(f_result_tmp);
    strcat(e,((char *)&e_tmp)+f_expr_e+1);
    char *rme = hc_result_(e);
    free(e_fme);
    m_apm_free(f_result_re); m_apm_free(f_result_im);
    m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
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
  e[strlen(e)]='$';
  char tmp[MAX_EXPR];
  char stack[MAX_OP_STACK][2];
  int sp=0;
  strcpy(tmp,e);
  memset(e,0,MAX_EXPR);
  int i=0,j=0;

  char neg=0;

  stack[sp][0] = '$';
  stack[sp++][1] = 0;

  while (tmp[i]!=0)
  {
    if (!isspace(tmp[i]))
    {
      if (isoperator(tmp[i]) && (tmp[i]!='_' || !isdigit(tmp[i+1])))
      {
	if (sp>=MAX_OP_STACK)
	  overflow_error();
	if (tmp[i]=='_' && tmp[i+1]=='(')
	{
	  neg = 1;
	  i++;
	}
	switch (tmp[i])
	{
	case '$': // terminating character
	  while (stack[sp-1][0]!='$')
	  {
	    e[j++] = stack[--sp][0];
	    if (stack[sp][1])
	      e[j++] = stack[sp][1];
	  }
	  return e;

	case '(':
	  stack[sp][1] = 0;
	  stack[sp++][0] = '(';
	  break;

	case ')':
	  sp--;
	  while (stack[sp][0]!='(')
	  {
	    e[j++] = stack[sp][0];
	    if (stack[sp][1])
	      e[j++] = stack[sp][1];
	    sp--;
	  }
	  if (neg)
	  {
	    e[j++] = '_';
	    e[j++] = ' '; // otherwise following number is treated as a negative
	    neg=0;
	  }
	  break;

	case '^':
	  if ((stack[sp-1][0]=='+')||(stack[sp-1][0]=='-')||(stack[sp-1][0]=='_')||(stack[sp-1][0]=='$')||(stack[sp-1][0]=='(')||(stack[sp-1][0]=='*')||(stack[sp-1][0]=='/')||(stack[sp-1][0]==PW_SIGN)||(stack[sp-1][0]=='%')||(stack[sp-1][0]=='<')||(stack[sp-1][0]=='>')||(stack[sp-1][0]=='=')||(stack[sp-1][0]=='&')||(stack[sp-1][0]=='|'))
	  {
	    stack[sp][0] = tmp[i];
	    stack[sp++][1] = 0;
	  } else {
	    e[j++] = stack[--sp][0];
	    if (stack[sp][1])
	      e[j++] = stack[sp][1];
	    i--;
	  }
	  break;

	case '!':
	  if (tmp[i+1]=='=')
	  {
	    // interpret as condition
	    if ((stack[sp-1][0]=='(')||(stack[sp-1][0]=='$')||(stack[sp-1][0]=='&')||(stack[sp-1][0]=='|'))
	    {
	      stack[sp][0] = '!';
	      stack[sp++][1] = '=';
	      i++;
	    } else {
	      e[j++] = stack[--sp][0];
	      if (stack[sp][1])
		e[j++] = stack[sp][1];
	      i--;
	    }
	  } else {
	    // interpret as factorial
	    if ((stack[sp-1][0]=='+')||(stack[sp-1][0]=='-')||(stack[sp-1][0]=='_')||(stack[sp-1][0]=='$')||(stack[sp-1][0]=='(')||(stack[sp-1][0]=='*')||(stack[sp-1][0]=='/')||(stack[sp-1][0]==PW_SIGN)||(stack[sp-1][0]=='%')||(stack[sp-1][0]=='<')||(stack[sp-1][0]=='>')||(stack[sp-1][0]=='=')||(stack[sp-1][0]=='&')||(stack[sp-1][0]=='|'))
	    {
	      stack[sp][0] = tmp[i];
	      stack[sp++][1] = 0;
	    } else {
	      e[j++] = stack[--sp][0];
	    if (stack[sp][1])
	      e[j++] = stack[sp][1];
	    i--;
	    }
	  }
	  break;

	case '*':
	case '/':
	case '%':
	  if ((stack[sp-1][0]=='+')||(stack[sp-1][0]=='-')||(stack[sp-1][0]=='_')||(stack[sp-1][0]=='$')||(stack[sp-1][0]=='(')||(stack[sp-1][0]=='<')||(stack[sp-1][0]=='>')||(stack[sp-1][0]=='=')||(stack[sp-1][0]=='&')||(stack[sp-1][0]=='|'))
	  {
	    stack[sp][0] = tmp[i];
	    stack[sp++][1] = 0;
	  } else {
	    e[j++] = stack[--sp][0];
	    if (stack[sp][1])
	      e[j++] = stack[sp][1];
	    i--;
	  }
	  break;

	case '_':
	case '+':
	case '-':
	  if ((stack[sp-1][0]=='(')||(stack[sp-1][0]=='$')||(stack[sp-1][0]=='<')||(stack[sp-1][0]=='>')||(stack[sp-1][0]=='=')||(stack[sp-1][0]=='&')||(stack[sp-1][0]=='|'))
	  {
	    stack[sp][0] = tmp[i];
	    stack[sp++][1] = 0;
	  } else {
	    e[j++] = stack[--sp][0];
	    if (stack[sp][1])
	      e[j++] = stack[sp][1];
	    i--;
	  }
	  break;

	case '>':
	case '<':
	case '=':
	  if ((stack[sp-1][0]=='(')||(stack[sp-1][0]=='$')||(stack[sp-1][0]=='&')||(stack[sp-1][0]=='|'))
	  {
	    stack[sp][0] = tmp[i];
	    if (tmp[i+1]=='=')
	    {
	      stack[sp++][1] = '=';
	      i++;
	    }
	    else
	      stack[sp++][1] = 0;
	  } else {
	    e[j++] = stack[--sp][0];
	    if (stack[sp][1])
	      e[j++] = stack[sp][1];
	    i--;
	  }
	  break;
	  
	case '&':
	case '|':
	  if ((stack[sp-1][0]=='(')||(stack[sp-1][0]=='$'))
	  {
	    stack[sp][0] = tmp[i];
	    if (tmp[i+1]==tmp[i])
	    {
	      stack[sp++][1] = tmp[i+1];
	      i++;
	    } else
	      stack[sp++][1] = 0;
	  } else {
	    e[j++] = stack[--sp][0];
	    if (stack[sp][1])
	      e[j++] = stack[sp][1];
	    i--;
	  }
	  break;
	}

      } else {
	if (tmp[i]!='\"')
	{
	  while ((!isspace(tmp[i]))&&(!isoperator(tmp[i]) || (i!=0 && tolower(tmp[i-1])=='e') || tmp[i]=='_'))
	    e[j++] = tmp[i++];
	} else {
	  e[j++] = tmp[i++];
	  while (tmp[i]!='\"')
	    e[j++] = tmp[i++];
	  e[j++] = tmp[i++];
	}
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
  struct hc_stack_element *first;
  struct hc_stack_element *curr;
  first = malloc(sizeof(struct hc_stack_element));
  first->re = m_apm_init();
  first->im = m_apm_init();
  first->str = NULL;
  first->p = NULL;
  first->n = malloc(sizeof(struct hc_stack_element));
  first->n->p = first;
  first->n->re = m_apm_init();
  first->n->im = m_apm_init();
  first->n->str = NULL;
  first->n->n = NULL;
  
  curr = first;
  int sp = 0;
  
  M_APM op1_r,op1_i,op2_r,op2_i;
  char *op1_str=NULL,*op2_str=NULL;

  char op1_type,op2_type;
  
  char tmp_num[MAX_DOUBLE_STRING];
  int i=0,j=0;
  
  op1_r = m_apm_init();
  op1_i = m_apm_init();
  op2_r = m_apm_init();
  op2_i = m_apm_init();

  i = 0;
  
  while (e[i]!=0)
  {
    if (!isspace(e[i]))
      switch (e[i])
      {
      case '*':
	if (sp < 2)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  syntax_error2();
	  return NULL;
	}
	curr = curr->p; // [--sp]
	m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
	curr = curr->p; // [--sp]
	m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	m_apmc_multiply(curr->re,curr->im,op1_r,op1_i,op2_r,op2_i);
	curr = curr->n; // [sp++]
	sp -= 1;
	break;
	
      case '/':
	if (sp < 2)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  syntax_error2();
	  return NULL;
	}
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  type_error("/ accepts only numbers");
	  return NULL;
	}
	m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  type_error("/ accepts only numbers");
	  return NULL;
	}
	m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	if (m_apm_sign(op2_r)==0 && m_apm_sign(op2_i)==0)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  d0_error();
	  return NULL;
	}
	if (m_apm_compare(op1_i,MM_Zero)==0 && m_apm_compare(op2_i,MM_Zero)==0)
	{
	  m_apm_set_string(curr->im,"0");
	  m_apm_divide(curr->re,HC_DEC_PLACES,op1_r,op2_r);
	} else {
	  m_apmc_divide(curr->re,curr->im,HC_DEC_PLACES,op1_r,op1_i,op2_r,op2_i);
	}
	curr = curr->n; // [sp++]
	sp -= 1;
	break;
	
      case '%': // Modulo divison; for example 5 % 3 = 2 = mod(5,3)
	if (sp >= 2)
	{
	  curr = curr->p; // [--sp]
	  if (curr->type != HC_VAR_NUM)
	  {
	    m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	    while (first->n)
	    {
	      m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	    }
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	    type_error("% accepts only numbers");
	    return NULL;
	  }
	  m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
	  curr = curr->p; // [--sp]
	  if (curr->type != HC_VAR_NUM)
	  {
	    m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	    while (first->n)
	    {
	      m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	    }
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	    type_error("% accepts only numbers");
	    return NULL;
	  }
	  m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	} else {
	  m_apm_set_string(op1_i,"1"); m_apm_set_string(op2_i,"1");
	}
	if (sp < 2 || m_apm_compare(op1_i,MM_Zero)!=0 || m_apm_compare(op2_i,MM_Zero)!=0)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  if (sp < 2)
	  {
	    syntax_error2();
	  } else {
	    arg_error("% : real arguments are required.");
	  }
	  return NULL;
	}
	M_APM tmp = m_apm_init();
	if (curr->type != HC_VAR_NUM)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  type_error("% accepts only numbers");
	  return NULL;
	}
	m_apm_integer_div_rem(tmp,curr->re,op1_r,op2_r);
	m_apm_set_string(curr->im,"0");
	m_apm_free(tmp);
	curr = curr->n; // [sp++]
	sp -= 1;
	break;
	
      case '+':
	if (sp < 2)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  syntax_error2();
	  return NULL;
	}
	curr = curr->p; // [--sp]
	op2_type = curr->type;
	if (op2_type == HC_VAR_NUM)
	{
	  m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
	} else if (op2_type == HC_VAR_STR)
	{
	  op2_str = get_string(curr->str);
	}
	curr = curr->p; // [--sp]
	op1_type = curr->type;
	if (op1_type == HC_VAR_NUM)
	{
	  m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	} else {
	  op1_str = get_string(curr->str);
	}
	if (op1_type == HC_VAR_NUM && op2_type == HC_VAR_NUM)
	{
	  curr->type = HC_VAR_NUM;
	  m_apmc_add(curr->re,curr->im,op1_r,op1_i,op2_r,op2_i);
	} else if (op1_type == HC_VAR_STR && op2_type == HC_VAR_STR)
	{
	  curr->type = HC_VAR_STR;
	  free(curr->str);
	  free(curr->n->str);
	  curr->n->str = NULL;
	  curr->str = malloc(1+strlen(op1_str)+strlen(op2_str)+2);
	  if (!curr->str)
	    mem_error();
	  sprintf(curr->str,"\"%s%s\"",op1_str,op2_str);
	  free(op1_str); op1_str = NULL;
	  free(op2_str); op2_str = NULL;
	} else {
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  type_error("+ accepts either two numbers or two strings");
	  return NULL;
	}
	curr = curr->n; // [sp++]
	sp -= 1;
	break;
	
      case '_':
	if (!isdigit(e[i+1]))
	{
	  curr = curr->p;
	  if (curr->type != HC_VAR_NUM)
	  {
	    m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	    while (first->n)
	    {
	      m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	    }
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	    type_error("- accepts only numbers");
	    return NULL;
	  }
	  m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	  m_apmc_subtract(curr->re,curr->im,MM_Zero,MM_Zero,op1_r,op1_i);
	  curr = curr->n;
	  break;
	} else {
	  e[i] = '@';
	  i--;
	  break;
	}
      case '-':
	if (sp < 2)
	{
	  curr = curr->p; // [--sp]
	  if (curr->type != HC_VAR_NUM)
	  {
	    m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	    while (first->n)
	    {
	      m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	    }
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	    type_error("- accepts only numbers");
	    return NULL;
	  }
	  m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	  m_apmc_subtract(curr->re,curr->im,MM_Zero,MM_Zero,op1_r,op1_i);
	  curr = curr->n; // [sp++]
	  // no need to modify sp here
	} else {
	  curr = curr->p; // [--sp]
	  if (curr->type != HC_VAR_NUM)
	  {
	    m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	    while (first->n)
	    {
	      m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	    }
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	    type_error("- accepts only numbers");
	    return NULL;
	  }
	  m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
	  curr = curr->p; // [--sp]
	  if (curr->type != HC_VAR_NUM)
	  {
	    m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	    while (first->n)
	    {
	      m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	    }
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	    type_error("- accepts only numbers");
	    return NULL;
	  }
	  m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	  m_apmc_subtract(curr->re,curr->im,op1_r,op1_i,op2_r,op2_i);
	  curr = curr->n; // [sp++]
	  sp -= 1;
	}
	break;
	
      case PW_SIGN:
	if (sp < 2)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  syntax_error2();
	  return NULL;
	}
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  type_error("^ accepts only numbers");
	  return NULL;
	}
	m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  type_error("^ accepts only numbers");
	  return NULL;
	}
	m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	if (m_apm_compare(op2_i,MM_Zero)==0 && m_apm_compare(op2_r,MM_Two)==0)
	{
	  m_apmc_sqr(curr->re,curr->im,op1_r,op1_i);
	} else {
	  if (m_apm_compare(op2_i,MM_Zero)==0 && m_apm_compare(op1_i,MM_Zero)==0)
	  {
	    m_apm_set_string(curr->im,"0");
	    m_apm_pow(curr->re,HC_DEC_PLACES,op1_r,op2_r);
	  } else {
	    m_apmc_pow(curr->re,curr->im,HC_DEC_PLACES,op1_r,op1_i,op2_r,op2_i);
	  }
	}
	curr = curr->n; // [sp++]
	sp -= 1;
	break;
	
      case '!':
	switch (e[i+1])
	{
	case '=':
	  // interpret as condition
	  i++;
	  if (sp < 2)
	  {
	    m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	    while (first->n)
	    {
	      m_apm_free(first->re);m_apm_free(first->im);first = first->n;free(first->p);
	    }
	    m_apm_free(first->re);m_apm_free(first->im);free(first);
	    syntax_error2();
	    return NULL;
	  }
	  curr = curr->p; // [--sp]
	  m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
	  curr = curr->p; // [--sp]
	  m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	  m_apm_set_string(curr->im,"0");
	  if (m_apmc_eq(op1_r,op1_i,op2_r,op2_i))
	    m_apm_set_string(curr->re,"0");
	  else
	    m_apm_set_string(curr->re,"1");
	  curr = curr->n;
	  sp -= 1;
	  break;
	default:
	  // interpret as factorial
	  if (sp < 1)
	  {
	    m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	    while (first->n)
	    {
	      m_apm_free(first->re);m_apm_free(first->im);first = first->n;free(first->p);
	    }
	    m_apm_free(first->re);m_apm_free(first->im);free(first);
	    syntax_error2();
	    return NULL;
	  }
	  curr = curr->p; // [--sp]
	  if (curr->type != HC_VAR_NUM)
	  {
	    m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	    while (first->n)
	    {
	      m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	    }
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	    type_error("! accepts only numbers");
	    return NULL;
	  }
	  if (!m_apm_is_integer(curr->re) || m_apm_compare(curr->im,MM_Zero)!=0)
	  {
	    m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	    while (first->n)
	    {
	      m_apm_free(first->re);m_apm_free(first->im);first = first->n;free(first->p);
	    }
	    m_apm_free(first->re);m_apm_free(first->im);free(first);
	    arg_error("! : an integer is required.");
	    return NULL;
	  }
	  m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	  m_apm_factorial(curr->re,op1_r);
	  m_apm_set_string(curr->im,"0");
	  curr = curr->n; // [sp++]
	  break;
	}
	break;
	
      case '<':
	if (sp < 2)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first);
	  syntax_error2();
	  return NULL;
	}
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  type_error("</<= accepts only numbers");
	  return NULL;
	}
	m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  type_error("</<= accepts only numbers");
	  return NULL;
	}
	m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	m_apm_set_string(curr->im,"0");
	switch(e[++i])
	{
	case '=':
	  if (m_apmc_le(op1_r,op1_i,op2_r,op2_i))
	    m_apm_set_string(curr->re,"1");
	  else
	    m_apm_set_string(curr->re,"0");
	  break;
	default:
	  i--;
	  if (m_apmc_lt(op1_r,op1_i,op2_r,op2_i))
	    m_apm_set_string(curr->re,"1");
	  else
	    m_apm_set_string(curr->re,"0");
	  break;
	}
	curr = curr->n;
	sp -= 1;
	break;
	
      case '>':
	if (sp < 2)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first);
           syntax_error2();
           return NULL;
	}
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	   type_error(">/>= accepts only numbers");
	   return NULL;
	}
	m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  type_error(">/>= accepts only numbers");
	  return NULL;
	}
	m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	m_apm_set_string(curr->im,"0");
	switch(e[++i])
	{
	case '=':
	  if (m_apmc_ge(op1_r,op1_i,op2_r,op2_i))
	    m_apm_set_string(curr->re,"1");
	  else
	    m_apm_set_string(curr->re,"0");
	  break;
	default:
	  i--;
	  if (m_apmc_gt(op1_r,op1_i,op2_r,op2_i))
	    m_apm_set_string(curr->re,"1");
	  else
	    m_apm_set_string(curr->re,"0");
	  break;
	}
	curr = curr->n;
	sp -= 1;
	break;
	
      case '=':
	if (sp < 2)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  syntax_error2();
	  return NULL;
	}
	curr = curr->p; // [--sp]
	m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
	curr = curr->p; // [--sp]
	m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	m_apm_set_string(curr->im,"0");
	switch(e[++i])
	{
	case '=':
	  if (m_apmc_eq(op1_r,op1_i,op2_r,op2_i))
	    m_apm_set_string(curr->re,"1");
	  else
	    m_apm_set_string(curr->re,"0");
	  break;
	default:
	  break; // TODO : if this is reached something bad happened (tm)
	}
	curr = curr->n;
	sp -= 1;
	break;
	
      case '|':
	if (e[i+1]=='|') // both single | and double || are accepted
	  i++;
	if (sp < 2)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  syntax_error2();
	  return NULL;
	}
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  type_error("|| accepts only numbers");
	  return NULL;
	}
	m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  type_error("|| accepts only numbers");
	  return NULL;
	}
	m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	m_apm_set_string(curr->im,"0");
	if (e[i+1]==e[i])
	  i++;
	if (m_apmc_eq(op1_r,op1_i,MM_Zero,MM_Zero)==0 || m_apmc_eq(op2_r,op2_i,MM_Zero,MM_Zero)==0)
	  m_apm_set_string(curr->re,"1");
	else
	  m_apm_set_string(curr->re,"0");
	curr = curr->n;
	sp -= 1;
	break;
	
      case '&':
	if (e[i+1]=='&') // both single & and double && are accepted
	  i++;
	if (sp < 2)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  syntax_error2();
	  return NULL;
	}
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  type_error("&& accepts only numbers");
	  return NULL;
	}
	m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
	  while (first->n)
	  {
	    m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);
	  }
	  m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);
	  type_error("&& accepts only numbers");
	  return NULL;
	}
	m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	m_apm_set_string(curr->im,"0");
	if (e[i+1]==e[i])
	  i++;
	if (m_apmc_eq(op1_r,op1_i,MM_Zero,MM_Zero)==0 && m_apmc_eq(op2_r,op2_i,MM_Zero,MM_Zero)==0)
	  m_apm_set_string(curr->re,"1");
	else
	  m_apm_set_string(curr->re,"0");
	curr = curr->n;
	sp -= 1;
	break;
	
      default:
	if (e[i]=='@')
	  e[i] = '_';
	j = 0;
	char type;
	if (e[i]!='\"')
	{
	  type = HC_VAR_NUM;
	  while (!isspace(e[i]) && (!isoperator(e[i]) || (i!=0 && tolower(e[i-1])=='e') || e[i]=='_') && e[i])
	  {
	    tmp_num[j++] = e[i++];
	  }
	} else {
	  type = HC_VAR_STR;
	  tmp_num[j++] = e[i++];
	  while (e[i]!='\"')
	    tmp_num[j++] = e[i++];
	  tmp_num[j++] = e[i++];
	}
	i--;
	tmp_num[j]=0;
	curr->type = type;
	if (curr->type == HC_VAR_NUM)
	{
	  char *tmp_num2;
	  tmp_num2 = hc_real_part(tmp_num);
	  if (tmp_num2[0]=='_')
	    tmp_num2[0] = '-';
	  m_apm_set_string(curr->re,tmp_num2); // set real part
	  free(tmp_num2);
	  tmp_num2 = hc_imag_part(tmp_num);
	  if (tmp_num2)
	  {
	    if (tmp_num2[0]=='_')
	      tmp_num2[0] = '-';
	    m_apm_set_string(curr->im,tmp_num2); // set imaginary part
	    free(tmp_num2);
	  } else {
	    m_apm_set_string(curr->im,"0"); // set null imaginary part
	  }
	} else if (curr->type == HC_VAR_STR)
	{
	  curr->str = strdup(tmp_num);
	}
	if (curr->n == NULL)
	{
	  curr->n = malloc(sizeof(struct hc_stack_element));
	  curr->n->re = m_apm_init();
	  curr->n->im = m_apm_init();
	  curr->n->str = NULL;
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
    m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str);
    while (first->n)
    {
      m_apm_free(first->re);m_apm_free(first->im);first = first->n;free(first->p);
    }
    m_apm_free(first->re);m_apm_free(first->im);free(first);
    syntax_error2();
    return NULL;
  }
  
  curr = curr->p;
  
  char *result_std=NULL,*result_im=NULL;
  
  if (curr->type == HC_VAR_NUM)
  {
    result_std = m_apm_to_fixpt_stringexp(hc.precision,curr->re,'.',0,0);
    if (m_apm_compare(curr->im,MM_Zero)!=0)
      result_im = m_apm_to_fixpt_stringexp(hc.precision,curr->im,'.',0,0);
  } else {
    result_std = strdup(curr->str);
  }
  
  m_apm_free(op1_r);m_apm_free(op1_i);
  m_apm_free(op2_r);m_apm_free(op2_i);

  while (first->n)
  {
    m_apm_free(first->re);m_apm_free(first->im);
    free(first->str);
    first = first->n;
    free(first->p);
  }
  
  m_apm_free(first->re);m_apm_free(first->im);
  free(first->str);
  free(first);
  
  if (!result_im)
    return result_std;
  else
  {
    char *result_cplx = malloc(strlen(result_std)+1+strlen(result_im)+1);
    strcpy(result_cplx,result_std);
    strcat(result_cplx,"i");
    strcat(result_cplx,result_im);
    free(result_std);
    free(result_im);
    return result_cplx;
  }
}


// return 0 if e contains a (detected) syntax error
char hc_check(char *e)
{
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
    char ignore=FALSE;
    for (i=0;i<strlen(e);i++)
    {
      if (e[i]=='\"')
      {
	ignore = ignore == FALSE ? TRUE : FALSE;
	if (!first)
	  first = 1;
	else if (first==1)
	  first = 2;
	continue;
      }

      if (ignore)
      {
	last_was_op = '\"';
	continue;
      }
      
      if (!isspace(e[i]) && (first==0))
      {
	if ((isoperator_np(e[i])) && (e[i]!='-') && (e[i]!='_'))
	{
	  return 0;
	} else {
	  first = 1;
	}
      }
      if ((!isalnum(e[i])) && (!isoperator(e[i])) && (e[i]!='^') && (e[i]!='.') && !isspace(e[i]) && (e[i]!=',') && (e[i]!='x') && (e[i]!='_'))
      {
	return 0;
      }
      if (isoperator_np(e[i]))
      {
	if ((last_was_op) && (last_was_op!='!') && ((last_was_op!='+')&&(e[i]!='-')&&(e[i]!='_')) && (last_was_op!='<' && e[i]!='=') && (last_was_op!='>' && e[i]!='=') && (last_was_op!='=' && e[i]!='=') && (last_was_op!='&' && e[i]!='&') && (last_was_op!='|' && e[i]!='|') && (last_was_op!='\"' && e[i]!=','))
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
	  if (last_was_int && isalpha(e[i]) && tolower(e[i])!='e' && tolower(e[i])!='i')
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
    {
      return 0;
    }
    
    if (first == 1)
    {
      return 0;
    }
    
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
  f = strreplace_(f,"i-","i_");
  char *e = malloc(MAX_EXPR);
  if (!e)
    mem_error();
  if (strlen(f)>MAX_EXPR-1)
    overflow_error();
  strcpy(e,f);
  free(f);

  int i=0;
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
	  e[i]='_';
	  count++;
	}
      } else {
	if (e[i]=='-')
	{
	  if (last_was_op)
	  {
	    e[i]='_';
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
      notify("RPN mode is now on.\n");
    else
      notify("RPN mode is now off.\n");
    break;
  case HASH_SCI:
    hc.exp = 's';
    notify("Exponential format is now SCI.\n");
    break;
  case HASH_ENG:
    hc.exp = 'e';
    notify("Exponential format is now ENG.\n");
    break;
  case HASH_NORMAL:
    hc.exp = 'n';
    notify("Exponential format is now normal.\n");
    break;
  case HASH_BPN:
    hc.bypass_nested = hc.bypass_nested==TRUE ? FALSE : TRUE;
    if (hc.bypass_nested == TRUE)
      notify("Bypass is now on.\n");
    else
      notify("Bypass is now off.\n");
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
      printf("Precision is now set to %i decimal places.\n",hc.precision);
      //}
    break;
  case HASH_3DPOINTS:
    if (atoi(&d[i+1]) < 2)
    {
      error_nq("Amount of points needs to be a positive number greater than 1.");
      break;
    }
    hc.graph_points_3d = atoi(&d[i+1]);
    printf("3D graphs will now be computed using %ix%i points.\n",hc.graph_points_3d,hc.graph_points_3d);
    if (hc.graph_points_3d > HC_GP3D_FAST)
      printf("Warning : your setting will probably cause the graph to render slowly. Consider lowering to the default, %ix%i points.\n",HC_GP3D_DEFAULT,HC_GP3D_DEFAULT);
    break;
  case HASH_DEG:
    hc.angle = 'd';
    printf("Angle format is now set to DEG.\n");
    break;
  case HASH_RAD:
    hc.angle = 'r';
    printf("Angle format is now set to RAD.\n");
    break;
  case HASH_GRAD:
    hc.angle = 'g';
    printf("Angle format is now set to GRAD.\n");
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
    printf("All user-defined variables were cleared.\n");
    break;
  case HASH_LOAD:
    hc_load(&d[i+1]);
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


char *hc_impmul_resolve(char *e)
{
  unsigned int i=0;
  char last_was_num = 0;
  char could_be_num = 1;
  for (; i<strlen(e); i++)
  {
    if (!isspace(e[i]))
    {
      if (((isalpha(e[i]) || e[i]=='(') && tolower(e[i])!='i' && tolower(e[i])!='e') && last_was_num)
      {
	e = realloc(e, strlen(e)+2);
	memmove(e+sizeof(char)*i+sizeof(char), e+sizeof(char)*i, strlen(e)-i+1);
	e[i] = '*';
	last_was_num = 0;
      }
      if ((isdigit(e[i]) || e[i]=='.') && could_be_num)
      {
	last_was_num = 1;
      } else {
	last_was_num = 0;
	could_be_num = 0;
      }
      if (isoperator(e[i]))
	could_be_num = 1;
    }
  }

  return e;
}


void hc_load_cfg()
{
  FILE *fr = fopen(HC_CFG_FILE,"r");
  hc.angle = 'r';
  hc.graph_points_3d = HC_GP3D_DEFAULT;
  hc.xmin2d = hc.ymin2d = hc.xmin3d = hc.ymin3d = hc.zmin3d = -10;
  hc.xmax2d = hc.ymax2d = hc.xmax3d = hc.ymax3d = hc.zmax3d = 10;
  if (!fr)
  {
    hc.rpn = FALSE;
    hc.precision = 16;
    hc.exp = 'n';
    hc.keys = TRUE;
    hc.plplot_dev_override = NULL;
  } else {
    char *buffer = malloc(sizeof(char) * HC_CFG_BUF_SIZE);
    char tmp[HC_CFG_BUF_SIZE];
    if (!buffer)
      mem_error();
    while (fgets(buffer,HC_CFG_BUF_SIZE,fr))
    {
      if (buffer[strlen(buffer)-1]=='\n')
	buffer[strlen(buffer)-1]=0;
      strncpy(tmp,buffer,HC_CFG_BUF_SIZE);
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
      case HASH_EXP_FORMAT:
	hc.exp = buffer[i+1];
	if (hc.exp!='s' && hc.exp!='e' && hc.exp!='n')
	{
	  warning("configuration file has incorrect exponential format specification. Setting to normal.");
	  hc.exp = 'n';
	}
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
      case HASH_3DPOINTS:
	if (atoi(&buffer[i+1]) < 2)
	{
	  hc.graph_points_3d = HC_GP3D_DEFAULT;
	} else {
	  hc.graph_points_3d = atoi(&buffer[i+1]);
	}
	break;
      case HASH_XMIN2D:
	hc.xmin2d = strtod(&buffer[i+1],NULL);
	break;
      case HASH_XMAX2D:
	hc.xmax2d = strtod(&buffer[i+1],NULL);
	break;
      case HASH_YMIN2D:
	hc.ymin2d = strtod(&buffer[i+1],NULL);
	break;
      case HASH_YMAX2D:
	hc.ymax2d = strtod(&buffer[i+1],NULL);
	break;
      case HASH_XMIN3D:
	hc.xmin3d = strtod(&buffer[i+1],NULL);
	break;
      case HASH_XMAX3D:
	hc.xmax3d = strtod(&buffer[i+1],NULL);
	break;
      case HASH_YMIN3D:
	hc.ymin3d = strtod(&buffer[i+1],NULL);
	break;
      case HASH_YMAX3D:
	hc.ymax3d = strtod(&buffer[i+1],NULL);
	break;
      case HASH_ZMIN3D:
	hc.zmin3d = strtod(&buffer[i+1],NULL);
	break;
      case HASH_ZMAX3D:
	hc.zmax3d = strtod(&buffer[i+1],NULL);
	break;
      }
    }
    free(buffer);
    fclose(fr);
  }

  static char announce = TRUE;
  if (announce == TRUE)
  {
    printf("Configuration :\n Angle format = %s | RPN mode = %s | Precision = %i | EXP format = %s\n",hc.angle=='r' ? "RAD" : hc.angle=='d' ? "DEG" : "GRAD",hc.rpn==0 ? "off" : "on",hc.precision,hc.exp=='s' ? "SCI" : hc.exp=='e' ? "ENG" : "NORMAL");
    announce = FALSE;
  }
}


void hc_save_cfg()
{
  FILE *fw = fopen(HC_CFG_FILE,"w");
  fprintf(fw,"rpn=%i\n",hc.rpn==0 ? FALSE : TRUE);
  fprintf(fw,"precision=%i\n",hc.precision);
  fprintf(fw,"keys=%i\n",hc.keys==0 ? FALSE : TRUE);
  fprintf(fw,"bpn=%i\n",hc.bypass_nested==0 ? FALSE : TRUE);
  if (hc.plplot_dev_override)
    fprintf(fw,"pldev=%s\n",hc.plplot_dev_override);
  else
    fprintf(fw,"pldev=$\n");
  fprintf(fw,"angle=%c\n",hc.angle);
  fprintf(fw,"expf=%c\n",hc.exp);
  fprintf(fw,"3dpoints=%i\n",hc.graph_points_3d);
  fprintf(fw,"xmin2d=%f\n",hc.xmin2d);
  fprintf(fw,"xmax2d=%f\n",hc.xmax2d);
  fprintf(fw,"ymin2d=%f\n",hc.ymin2d);
  fprintf(fw,"ymax2d=%f\n",hc.ymax2d);
  fprintf(fw,"xmin3d=%f\n",hc.xmin3d);
  fprintf(fw,"xmax3d=%f\n",hc.xmax3d);
  fprintf(fw,"ymin3d=%f\n",hc.ymin3d);
  fprintf(fw,"ymax3d=%f\n",hc.ymax3d);
  fprintf(fw,"zmin3d=%f\n",hc.zmin3d);
  fprintf(fw,"zmax3d=%f\n",hc.zmax3d);
  fclose(fw);
}


void hc_cleanup()
{
  hc_output(PRINT,""); // make it close file descriptors (if any)
  hc_save_cfg();
}


char hc_is_predef(char *var)
{
  unsigned int i = 0;
  unsigned int name_hash = simple_hash(var);
  for (; i < HC_FNAMES; i++)
  {
    if (hc_hashes[i]==0)
      hc_hashes[i] = simple_hash((char *)hc_fnames[i][0]);
    if (name_hash == hc_hashes[i])
    {
      return 1;
    }
  }
  return 0;
}


void hc_load(char *fname)
{
  FILE *fr = fopen(fname,"r");
  if (!fr)
  {
    perror("Error");
    error_nq("Error: Cannot open file.\n");
    return;
  }
  char *expr = malloc(sizeof(char) * MAX_EXPR + sizeof(char));
  if (!expr)
    mem_error();
  memset(expr,0,MAX_EXPR+1);
  unsigned int line = 1;
  while (fgets(expr,MAX_EXPR+1,fr))
  {
    if (strcmp(expr,"\n")==0)
      break;
    if (expr[strlen(expr)-1]=='\n')
      expr[strlen(expr)-1]=0;
    if (strlen(expr)>=MAX_EXPR)
    {
      overflow_error_nq();
      break;
    }
    while (!check_completeness(expr))
    {
      if (!fgets(expr+strlen(expr)*sizeof(char),MAX_EXPR + 1 - strlen(expr),fr))
	break;
      line++;
    }
    char *fme = hc_result(expr);
    if (!fme)
    {
      load_error(line,expr);
      break;
    }
    free(fme);
    line++;
  }
  fclose(fr);
  free(expr);
}

