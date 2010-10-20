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
#include <locale.h>
#ifndef WIN32
#include <wordexp.h>
#endif
#include "m_apmc.h"
#include "hc.h"
#include "hc_functions.h"
#include "hash.h"
#include "hc_graph.h"
#include "hc_complex.h"
#include "hc_list.h"
#include "hc_utils.h"
#include "hc_fnp.h"
//#define NHASH 29989 // use a large prime number
#define NHASH 139969
#define MULT 31


const char *hc_names[][2] = {
  {"ans","func"},
  {"abs","func"},
  {"acos","func"},
  {"asin","func"},
  {"atan","func"},
  {"boxplot","func"},
  {"boxplotf","func"},
  {"cat","func"},
  {"cbrt","func"},
  {"ceil","func"},
  {"chartocode","func"},
  {"clear","func"},
  {"cmtoinch","func"},
  {"codetochar","func"},
  {"cos","func"},
  {"cosh","func"},
  {"crossp","func"},
  {"ctof","func"},
  {"ctok","func"},
  {"dotp","func"},
  {"exp","func"},
  {"factorial","func"},
  {"fibo","func"},
  {"floor","func"},
  {"floztoml","func"},
  {"for","func"},
  {"ftoc","func"},
  {"ftok","func"},
  {"fttom","func"},
  {"gcd","func"},
  {"graph","func"},
  {"graphpeq","func"},
  {"gmul","func"},
  {"graph3","func"},
  {"help","func"},
  {"if","func"},
  {"im","func"},
  {"imag","func"},
  {"inchtocm","func"},
  {"input","func"},
  {"join","func"},
  {"kmtomi","func"},
  {"ktoc","func"},
  {"ktof","func"},
  {"lcm","func"},
  {"length","func"},
  {"ln","func"},
  {"log10","func"},
  {"map","func"},
  {"mitokm","func"},
  {"mltofloz","func"},
  {"mmass","func"},
  {"mod","func"},
  {"mtoft","func"},
  {"nCr","func"},
  {"nPr","func"},
  {"num","func"},
  {"ones","func"},
  {"print","func"},
  {"product","func"},
  {"rand","func"},
  {"range","func"},
  {"re","func"},
  {"real","func"},
  {"round","func"},
  {"sin","func"},
  {"sinh","func"},
  {"slpfld","func"},
  {"sort","func"},
  {"sqrt","func"},
  {"stats","func"},
  {"statsf","func"},
  {"str","func"},
  {"sum","func"},
  {"tan","func"},
  {"tanh","func"},
  {"totient","func"},
  {"while","func"},
  {"write","func"},
  {"zeros","func"},
  // CONSTANTS
  {"pi","cnst:3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384461"},
  {"c","cnst:299792458"},
  {"e","cnst:2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274274663919320030599218174136"},
  {"phi","cnst:1.61803398874989484820458683436563811772030917980576286213544862270526046281890244970720720418939113748475408807538689175212663386"},
  {"g","cnst:9.80655"},
  {"answer","cnst:42"},
  {"na","cnst:6.0221415e23"},
  // OTHERS
  {"help","cmd"},
  {"credits","cmd"},
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
  {"\\tstep","dir"},
  {"\\3dpoints","dir"}
};


unsigned int hc_hashes[HC_NAMES];

unsigned int simple_hash(char *p)
{
  unsigned int h = 0;
  for(; *p; p++)
    h = MULT * h + tolower(*p);
  return h % NHASH;
}

// all operators
#define isoperator(c) (isoperator_np(c) || (c=='(') || (c==')'))

// all operators without parenthesis
#define isoperator_np(c) (isoperator_allowfirst(c) || (c=='-') || (c=='$') || (c==',') || (c=='!') || (c=='_') || (c=='<') || (c=='>') || (c=='=') || (c=='&') || (c=='|') || (c==NOT_SIGN))

// all operators that are allowed for "reusing" an old result. For example if a user types 2+3, the result, 5, will be stored internally. If the next input received is then for example *3, the 5 will be substituted back and therefore the result of *3 would be 5*3 = 15.
// Note that c=='-' is not allowed since it is ambiguous (this might change in a later version).
#define isoperator_allowfirst(c) ((c=='*') || (c=='/') || (c=='+') || (c==PW_SIGN) || (c=='%'))

#define isdirection(x) (x[0]=='\\')
#define isvarassign(x) (strchr_outofblock(x,'=')!=NULL && strchr_outofblock(x,'=')[1]!='=' && strchr(x,'=')!=x && (strchr_outofblock(x,'=')-1)[0]!='<' && (strchr_outofblock(x,'=')-1)[0]!='>' && (strchr_outofblock(x,'=')-1)[0]!='!')
#define iscondition(x) (strstr(x,"==")!=NULL || strstr(x,"!=")!=NULL || strstr(x,">=")!=NULL || strstr(x,"<=")!=NULL || strstr(x,"<")!=NULL || strstr(x,">")!=NULL)
#define iscontrolstruct(x) ((strstr(x,"if ")==x) || (strstr(x,"while")==x) || (strstr(x,"for")==x))
#define isdigitb(c,b) ((b==16 && isxdigit(c)) || (b==2 && (c=='0' || c=='1')) || (b==10 && isdigit(c)))

#define hc_postfix_result_cleanup() {m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str); while (first->n) {m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);} m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);}


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
void hc_load(char *);


M_APM hc_lans_mapm_re;
M_APM hc_lans_mapm_im;
char *hc_lans_strvec;
char *hc_lans_strform;
char hc_lans_type;

M_APM MM_MOne; // Minus one
struct hc_config hc;
char announce_errors=TRUE;
char graphing_ignore_errors=FALSE;
int hc_nested;


char *hc_result(char *e)
{
  if (!e)
    return NULL;
  announce_errors = TRUE; // This variable is used so that syntax errors etc. are not announced multiple times if multiple errors are detected
  hc_nested = 0; // This is set to prevent infinite recursion

  static char init=0;
  if (!init)
  {
    setlocale(LC_NUMERIC, "POSIX");
    atexit(hc_cleanup);
    hc_load_cfg();
    hc_lans_mapm_re = m_apm_init();
    hc_lans_mapm_im = m_apm_init();
    hc_lans_strvec = NULL;
    hc_lans_strform = NULL;
    hc_lans_type = HC_VAR_NUM;
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
    hc.flags = 0;
    m_apmc_init();
    int i = 0;
    for (; i < HC_NAMES; i++)
      hc_hashes[i] = simple_hash((char *)hc_names[i][0]);
    init = 1;
  }

  m_apm_trim_mem_usage();

  if (strcmp(e,"help")==0)
  {
    return hc_result("help(\"\")");
  }

  if (strcmp(e,"credits")==0)
  {
    printf("HC was developed by Jan Dlabal (visit http://houbysoft.com/en/about.php for contact information) and is (C) 2009-2010.\nThe homepage of this program is http://hc.houbysoft.com/.\nThis program was released under the GNU GPL v3 License. Visit gnu.org for more information.\n");
    return NULL;
  }

  char *r = hc_result_(e);
  if (!r || !strlen(r))
  {
    return r;
  } else if (is_num(r))
  {
    char *r_re = hc_real_part(r);
    char *r_im = hc_imag_part(r);
    free(r);
    free(hc_lans_strvec);
    hc_lans_strvec = NULL;
    hc_lans_type = HC_VAR_NUM;
    m_apm_set_string(hc_lans_mapm_re,r_re);
    if (r_im)
    {
      m_apm_set_string(hc_lans_mapm_im,r_im);
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
  } else if (is_vector(r)) {
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
      }
      r_new = realloc(r_new, strlen(r_new)+1+strlen(curarg)+1);
      strcat(r_new,curarg);
      strcat(r_new,",");
      free(curarg);
      curarg = hc_get_arg((char *)(r + sizeof(char)),++idx);
    }
    r_new[strlen(r_new)-1] = ']';
    free(r);
    r = r_new;
    free(hc_lans_strvec);
    hc_lans_type = HC_VAR_VEC;
    hc_lans_strvec = strdup(r);
  } else if (is_string(r)) {
    free(hc_lans_strvec);
    hc_lans_type = HC_VAR_STR;
    hc_lans_strvec = strdup(r);
  } else {
    free(r);
    syntax_error2();
    return NULL;
  }
  free(hc_lans_strform);
  hc_lans_strform = strdup(r);
  return r;
}


char *hc_result_(char *e)
{
  char *r=NULL;
  if (!e)
    return NULL;
  e = strip_spaces(e);
  if (!e[0] || e[0]=='#') // empty string or comment
  {
    r = malloc(1);
    if (!r) mem_error();
    r[0]='\0';
    return r;
  }
  char buf_isvarassign = isvarassign(e);
  
  if (strchr_outofblock(e,';') && !(buf_isvarassign))
  {
    return hc_result_mul(e);
  } else {
    if (buf_isvarassign)
    {
      if (strchr_outofblock(e,';'))
	return hc_result_mul(e);
      else
      {
	char *f = strdup(e);
	if (!f)
	  mem_error();
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
	char *f = strdup(e);
	if (!f)
	  mem_error();
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

#ifdef DDBG
  printf("hc_nested = %i\n",hc_nested);
#endif

  char *e = strdup(f);
  if (!e) mem_error();

  char *e_fme = hc_impmul_resolve(e);
  e = hc_plusminus(e_fme);
  free(e_fme);
  e_fme = e;

  if (!e)
  {
    hc_nested--;
    return NULL;
  }

  char firstchar = strip_spaces(e)[0];
  if (hc_lans_strform && isoperator_allowfirst(firstchar))
  {
    e = malloc(strlen(hc_lans_strform)+strlen(e_fme)+1);
    if (!e) mem_error();
    sprintf(e,"%s%s",hc_lans_strform,e_fme);
    free(e_fme);
    e_fme = e;
  }

  char *fme;

#ifdef DBG
  printf("[1] %s\n",e);
#endif
  e = fme = hc_i2p(e);
  if (!e)
  {
    free(e_fme);
    hc_nested--;
    return NULL;
  }
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
}


char *hc_i2p(char *f)
{
  if (strlen(f) >= MAX_EXPR)
  {
    overflow_error_nq();
    return NULL;
  }
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
  char last_was_operator[2]={TRUE,TRUE}; // used to distinguish when ! is being used as factorial and when as NOT

  stack[sp][0] = '$';
  stack[sp++][1] = 0;

  while (tmp[i]!=0 && j < MAX_EXPR)
  {
    if (!isspace(tmp[i]))
    {
      if (isoperator(tmp[i]) && (tmp[i]!='_' || !isdigit(tmp[i+1])))
      {
	last_was_operator[1] = TRUE;
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
	  last_was_operator[1] = FALSE;
	  stack[sp][1] = 0;
	  stack[sp++][0] = '(';
	  break;

	case ')':
	  last_was_operator[1] = FALSE;
	  sp--;
	  if (sp < 0)
	  {
	    hc_error(SYNTAX,"('s and )'s do not match");
	    free(e);
	    return NULL;
	  }
	  while (stack[sp][0]!='(')
	  {
	    e[j++] = stack[sp][0];
	    if (stack[sp][1])
	      e[j++] = stack[sp][1];
	    sp--;
	    if (sp < 0)
	    {
	      hc_error(SYNTAX,"('s and )'s do not match");
	      free(e);
	      return NULL;
	    }
	  }
	  if (neg)
	  {
	    e[j++] = '_';
	    e[j++] = ' '; // otherwise following number is treated as a negative
	    neg=0;
	  }
	  break;

	case '^':
	  if ((stack[sp-1][0]=='+')||(stack[sp-1][0]=='-')||(stack[sp-1][0]=='_')||(stack[sp-1][0]=='$')||(stack[sp-1][0]=='(')||(stack[sp-1][0]=='*')||(stack[sp-1][0]=='/')||(stack[sp-1][0]==PW_SIGN)||(stack[sp-1][0]=='%')||(stack[sp-1][0]=='<')||(stack[sp-1][0]=='>')||(stack[sp-1][0]=='=')||(stack[sp-1][0]=='!' && stack[sp-1][1]=='=')||(stack[sp-1][0]=='&')||(stack[sp-1][0]=='|'))
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
	  } else if (last_was_operator[0] == FALSE) {
	    // interpret as factorial
	    if ((stack[sp-1][0]=='+')||(stack[sp-1][0]=='-')||(stack[sp-1][0]=='_')||(stack[sp-1][0]=='$')||(stack[sp-1][0]=='(')||(stack[sp-1][0]=='*')||(stack[sp-1][0]=='/')||(stack[sp-1][0]==PW_SIGN)||(stack[sp-1][0]=='%')||(stack[sp-1][0]=='<')||(stack[sp-1][0]=='>')||(stack[sp-1][0]=='=')||(stack[sp-1][0]=='!' && stack[sp-1][1]=='=')||(stack[sp-1][0]=='&')||(stack[sp-1][0]=='|'))
	    {
	      stack[sp][0] = tmp[i];
	      stack[sp++][1] = 0;
	    } else {
	      e[j++] = stack[--sp][0];
	    if (stack[sp][1])
	      e[j++] = stack[sp][1];
	    i--;
	    }
	  } else {
	    // interpret as logical NOT and replace the ! with a NOT_SIGN, so that hc_postfix_result interprets this correctly
	    if ((stack[sp-1][0]=='+')||(stack[sp-1][0]=='-')||(stack[sp-1][0]=='_')||(stack[sp-1][0]=='$')||(stack[sp-1][0]=='(')||(stack[sp-1][0]=='*')||(stack[sp-1][0]=='/')||(stack[sp-1][0]==PW_SIGN)||(stack[sp-1][0]=='%')||(stack[sp-1][0]=='<')||(stack[sp-1][0]=='>')||(stack[sp-1][0]=='=')||(stack[sp-1][0]=='!' && stack[sp-1][1]=='=')||(stack[sp-1][0]=='&')||(stack[sp-1][0]=='|'))
	    {
	      stack[sp][0] = NOT_SIGN;
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
	  if ((stack[sp-1][0]=='+')||(stack[sp-1][0]=='-')||(stack[sp-1][0]=='_')||(stack[sp-1][0]=='$')||(stack[sp-1][0]=='(')||(stack[sp-1][0]=='<')||(stack[sp-1][0]=='>')||(stack[sp-1][0]=='=')||(stack[sp-1][0]=='!' && stack[sp-1][1]=='=')||(stack[sp-1][0]=='&')||(stack[sp-1][0]=='|'))
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
	  if ((stack[sp-1][0]=='(')||(stack[sp-1][0]=='$')||(stack[sp-1][0]=='<')||(stack[sp-1][0]=='>')||(stack[sp-1][0]=='=')||(stack[sp-1][0]=='!' && stack[sp-1][1]=='=')||(stack[sp-1][0]=='&')||(stack[sp-1][0]=='|'))
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
	  if ((stack[sp-1][0]=='(')||(stack[sp-1][0]=='$')||(stack[sp-1][0]=='|'))
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
	last_was_operator[0] = last_was_operator[1];

      } else {
	last_was_operator[0] = FALSE;
	if (isdigit(tmp[i]) || tmp[i]=='.' || tmp[i]=='_')
	{
	  char tmpsts = 0;
	  int tmpe = 1;
	  int tmpi = 1;
	  char base;
	  if (tmp[i]=='0' && tolower(tmp[i+1])=='x')
	    base = 16;
	  else if (tmp[i]=='0' && tolower(tmp[i+1])=='b')
	    base = 2;
	  else
	    base = 10;
	  /*
	    Base 10:

	    tmpsts holds various states in reading the number. These states are used for checking the syntax of the number -- ie. the allowed characters to occur at a particular state. tmpe and tmpi hold the number of allowed 'e's and 'i's respectively.

	    States
	    ------
	    0 : at the beginning of the scan, or after an 'e', or after an 'i'
	    1 : after a number, allowing for a decimal point
	    2 : after a number or a decimal point, not allowing for a decimal point
	    3 : after a negative sign
	   */
	  if (base == 10)
	  {
	    while ((!isspace(tmp[i]))&&(!isoperator(tmp[i]) || (i!=0 && tolower(tmp[i-1])=='e') || tmp[i]=='_'))
	    {
	      if (((tmpsts == 0) && (!isdigit(tmp[i]) && tmp[i]!='.' && tmp[i]!='_' && tmp[i]!='-')) || ((tmpsts == 1) && (!isdigit(tmp[i]) && tmp[i]!='.' && tolower(tmp[i])!='e' && tolower(tmp[i])!='i')) || ((tmpsts == 2) && (!isdigit(tmp[i]) && tolower(tmp[i])!='e' && tolower(tmp[i])!='i')) || ((tmpsts == 3) && (!isdigit(tmp[i]) && tmp[i]!='.')))
	      {
		hc_error(SYNTAX,"invalid number encountered");
		free(e);
		return NULL;
	      }
	      if (tolower(tmp[i]) == 'e')
	      {
		tmpsts = 0;
		tmpe--;
	      } else if (tolower(tmp[i]) == 'i')
	      {
		tmpsts = 0;
		tmpi--;
		tmpe = 1;
	      } else if (isdigit(tmp[i]))
	      {
		if (tmpsts == 0 || tmpsts == 3)
		  tmpsts = 1;
		// nothing needs to be done otherwise
	      } else if (tmp[i]=='.')
	      {
		tmpsts = 2;
	      } else if (tmp[i]=='_' || tmp[i]=='-')
	      {
		tmpsts = 3;
	      }
	      if (tmpe < 0 || tmpi < 0)
	      {
		hc_error(SYNTAX,"too many e's or i's encountered in number");
		free(e);
		return NULL;
	      }
	      e[j++] = tmp[i++];
	    }
	  } else { // base == 2 or base == 16
	    /* Base 2 or Base 16:

	       tmpsts contains either TRUE, to allow a decimal point, or FALSE, to allow only digits.
	       No more complexity is required as the exponents and complex numbers are not used, available, and for the case of the 'e' in hex, impossible to implement.
	    */
	    tmpsts = TRUE;
	    e[j++] = tmp[i++]; // write down the '0'
	    e[j++] = tmp[i++]; // write down the either 'x' (base 16) or 'b' (base 2)
	    while (!isspace(tmp[i]) && (!isoperator(tmp[i]) || tmp[i]=='_'))
	    {
	      if (!isdigitb(tmp[i],base) && (tmp[i]!='.' || tmpsts == FALSE))
	      {
		hc_error(SYNTAX, "invalid binary or hex number encountered");
		free(e);
		return NULL;
	      } else {
		if (tmp[i]=='.')
		  tmpsts = FALSE;
		e[j++] = tmp[i++];
	      }
	    }
	  }
	} else if (tmp[i]=='\"') {
	  e[j++] = tmp[i++];
	  while (tmp[i]!='\"' && tmp[i]!='$')
	    e[j++] = tmp[i++];
	  if (tmp[i] != '\"')
	  {
	    hc_error(SYNTAX,"missing end quotes");
	    free(e);
	    return NULL;
	  }
	  e[j++] = tmp[i++];
	} else if (tmp[i]=='[') {
	  while (tmp[i]=='[')
	  {
	    unsigned int pct = 1;
	    char ignore = FALSE;
	    e[j++] = tmp[i++];
	    while (pct!=0 && tmp[i])
	    {
	      if (tmp[i]=='\"')
	      {
		ignore = ignore == FALSE ? TRUE : FALSE;
	      }
	      if (tmp[i]=='[' && !ignore)
		pct++;
	      if (tmp[i]==']' && !ignore)
		pct--;
	      e[j++] = tmp[i++];
	    }
	    if (pct!=0)
	    {
	      hc_error(SYNTAX, "['s and ]'s do not match");
	      free(e);
	      return NULL;
	    }
	  }
	} else if (tmp[i]=='\'') { // lambda expression
	  char *endoffunc = strchr_outofblock(tmp + i + 1,'\'');
	  if (!endoffunc)
	  {
	    hc_error(SYNTAX,"single quotes do not match");
	    free(e);
	    return NULL;
	  }
	  memcpy((char *)(e + j),(char *)(tmp + i),endoffunc - tmp - i + 1);
	  j += endoffunc - tmp - i + 1;
	  i = endoffunc - tmp + 1;
	  if (tmp[i]=='(') // called
	  {
	    e[j++] = tmp[i];
	    int par = 1;
	    char ignore = FALSE;
	    while (par!=0 && tmp[i]!='$' && tmp[i])
	    {
	      e[j++] = tmp[++i];
	      if (tmp[i]=='\"')
	      {
		ignore = ignore == FALSE ? TRUE : FALSE;
		continue;
	      }
	      if (ignore)
		continue;
	      if (tmp[i]=='(')
		par++;
	      else if (tmp[i]==')')
		par--;
	    }
	    if (par != 0)
	    {
	      hc_error(SYNTAX,"('s and )'s do not match");
	      free(e);
	      return NULL;
	    } else {
	      i++; // skip the last ')'
	    }
	  }
	} else {
	  if (!isalpha(tmp[i]))
	  {
	    hc_error(SYNTAX, "at character %i : %c", i+1, tmp[i]);
	    free(e);
	    return NULL;
	  }
	  while (isalpha(tmp[i]) || isdigit(tmp[i])) // the first to be checked can't be a digit since that would be caught above
	  {
	    e[j++] = tmp[i++];
	  }
	  if (tmp[i]=='(') // function
	  {
	    e[j++] = tmp[i];
	    int par = 1;
	    char ignore = FALSE;
	    while (par!=0 && tmp[i]!='$' && tmp[i])
	    {
	      e[j++] = tmp[++i];
	      if (tmp[i]=='\"')
	      {
		ignore = ignore == FALSE ? TRUE : FALSE;
		continue;
	      }
	      if (ignore)
		continue;
	      if (tmp[i]=='(')
		par++;
	      else if (tmp[i]==')')
		par--;
	    }
	    if (par != 0)
	    {
	      hc_error(SYNTAX,"('s and )'s do not match");
	      free(e);
	      return NULL;
	    } else {
	      i++; // skip the last ')'
	    }
	  }
	}

	while (tmp[i]=='[')
	{
	  unsigned int pct = 1;
	  char ignore = FALSE;
	  e[j++] = tmp[i++];
	  while (pct!=0 && tmp[i])
	  {
	    if (tmp[i]=='\"')
	    {
	      ignore = ignore == FALSE ? TRUE : FALSE;
	    }
	    if (tmp[i]=='[' && !ignore)
	      pct++;
	    if (tmp[i]==']' && !ignore)
	      pct--;
	    e[j++] = tmp[i++];
	  }
	  if (pct!=0)
	  {
	    hc_error(SYNTAX,"['s and ]'s do not match");
	    free(e);
	    return NULL;
	  }
	}

	e[j++] = ' ';
	i--;
      }
    }
    i++;
  }
  if (j >= MAX_EXPR)
  {
    overflow_error_nq();
    free(e);
    return NULL;
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
  
  char tmp_num[MAX_DOUBLE_STRING]; // this is used below, do not change
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
	  hc_postfix_result_cleanup();
	  hc_error(NOT_ENOUGH_OPERANDS,"*");
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
	} else if (op2_type == HC_VAR_VEC)
	{
	  op2_str = strdup(curr->str);
	}
	curr = curr->p; // [--sp]
	op1_type = curr->type;
	if (op1_type == HC_VAR_NUM)
	{
	  m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	} else if (op1_type == HC_VAR_STR)
	{
	  op1_str = get_string(curr->str);
	} else if (op1_type == HC_VAR_VEC)
	{
	  op1_str = strdup(curr->str);
	}
	if (op1_type == HC_VAR_NUM && op2_type == HC_VAR_NUM)
	{
	  m_apmc_multiply(curr->re,curr->im,op1_r,op1_i,op2_r,op2_i);
	} else if ((op1_type == HC_VAR_NUM && m_apm_compare(op1_i,MM_Zero)==0 && m_apm_is_integer(op1_r) && op2_type == HC_VAR_STR) || (op1_type == HC_VAR_STR && op2_type == HC_VAR_NUM && m_apm_compare(op2_i,MM_Zero)==0 && m_apm_is_integer(op2_r)))
	{
	  curr->type = HC_VAR_STR;
	  free(curr->str); free(curr->n->str);
	  curr->n->str = NULL;
	  curr->str = str_multiply(op1_type == HC_VAR_STR ? op1_str : op2_str, op1_type == HC_VAR_NUM ? op1_r : op2_r);
	  free(op1_str); op1_str = NULL;
	  free(op2_str); op2_str = NULL;
	} else if ((op1_type == HC_VAR_NUM && op2_type == HC_VAR_VEC) || (op2_type == HC_VAR_NUM && op1_type == HC_VAR_VEC) || (op1_type == HC_VAR_STR && op2_type == HC_VAR_VEC) || (op2_type == HC_VAR_STR && op1_type == HC_VAR_VEC)) {
	  curr->type = HC_VAR_VEC;
	  free(curr->str); free(curr->n->str);
	  curr->n->str = NULL;
	  curr->str = list_mul_div(op1_type == HC_VAR_VEC ? op1_str : op2_str, op1_type == HC_VAR_NUM ? op1_r : op2_type == HC_VAR_NUM ? op2_r : NULL, op1_type == HC_VAR_NUM ? op1_i : op2_type == HC_VAR_NUM ? op2_i : NULL, op1_type == HC_VAR_STR ? op1_str : op2_type == HC_VAR_STR ? op2_str : NULL, '*');
	  if (!curr->str)
	  {
	    hc_postfix_result_cleanup();
	    return NULL;
	  }
	  free(op1_str); op1_str = NULL;
	  free(op2_str); op2_str = NULL;
	} else {
	  hc_postfix_result_cleanup();
	  type_error("* accepts either numbers, an integer and a string, a vector and a number, or an all-integer vector and a string");
	  return NULL;	  
	}
	curr = curr->n; // [sp++]
	sp -= 1;
	break;
	
      case '/':
	if (sp < 2)
	{
	  hc_postfix_result_cleanup();
	  hc_error(NOT_ENOUGH_OPERANDS,"/");
	  return NULL;
	}
	curr = curr->p; // [--sp]
	op2_type = curr->type;
	if (curr->type == HC_VAR_NUM)
	{
	  m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
	} else if (curr->type == HC_VAR_VEC)
	{
	  op2_str = strdup(curr->str);
	}
	curr = curr->p; // [--sp]
	op1_type = curr->type;
	if (curr->type == HC_VAR_NUM)
	{
	  m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	} else if (curr->type == HC_VAR_VEC)
	{
	  op1_str = strdup(curr->str);
	}
	if (op1_type == HC_VAR_NUM && op2_type == HC_VAR_NUM)
	{
	  if (m_apm_sign(op2_r)==0 && m_apm_sign(op2_i)==0)
	  {
	    hc_postfix_result_cleanup();
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
	} else if (op1_type == HC_VAR_VEC && op2_type == HC_VAR_NUM )
	{
	  curr->type = HC_VAR_VEC;
	  free(curr->str); free(curr->n->str);
	  curr->n->str = NULL;
	  curr->str = list_mul_div(op1_type == HC_VAR_VEC ? op1_str : op2_str, op1_type == HC_VAR_NUM ? op1_r : op2_r, op1_type == HC_VAR_NUM ? op1_i : op2_i, NULL, '/');
	  if (!curr->str)
	  {
	    hc_postfix_result_cleanup();
	    return NULL;
	  }
	  free(op1_str); op1_str = NULL;
	  free(op2_str); op2_str = NULL;
	} else {
	  hc_postfix_result_cleanup();
	  type_error("/ accepts only two numbers or a vector and a number (in that order)");
	  return NULL;
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
	    hc_postfix_result_cleanup();
	    type_error("% accepts only numbers");
	    return NULL;
	  }
	  m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
	  curr = curr->p; // [--sp]
	  if (curr->type != HC_VAR_NUM)
	  {
	    hc_postfix_result_cleanup();
	    type_error("% accepts only numbers");
	    return NULL;
	  }
	  m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	} else {
	  m_apm_set_string(op1_i,"1"); m_apm_set_string(op2_i,"1");
	}
	if (sp < 2 || m_apm_compare(op1_i,MM_Zero)!=0 || m_apm_compare(op2_i,MM_Zero)!=0)
	{
	  hc_postfix_result_cleanup();
	  if (sp < 2)
	  {
	    hc_error(NOT_ENOUGH_OPERANDS,"%%");
	  } else {
	    arg_error("% : real arguments are required.");
	  }
	  return NULL;
	}
	M_APM tmp = m_apm_init();
	if (curr->type != HC_VAR_NUM)
	{
	  hc_postfix_result_cleanup();
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
	  hc_postfix_result_cleanup();
	  hc_error(NOT_ENOUGH_OPERANDS,"+");
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
	} else if (op2_type == HC_VAR_VEC)
	{
	  op2_str = strdup(curr->str);
	}
	curr = curr->p; // [--sp]
	op1_type = curr->type;
	if (op1_type == HC_VAR_NUM)
	{
	  m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	} else if (op1_type == HC_VAR_STR) {
	  op1_str = get_string(curr->str);
	} else if (op1_type == HC_VAR_VEC) {
	  op1_str = strdup(curr->str);
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
	} else if (op1_type == HC_VAR_VEC && op2_type == HC_VAR_VEC) {
	  free(curr->str);
	  free(curr->n->str);
	  curr->n->str = NULL;
	  if (!(curr->str = list_add_sub(op1_str, op2_str, '+')))
	  {
	    hc_postfix_result_cleanup();
	    return NULL;
	  }
	  free(op1_str); op1_str = NULL;
	  free(op2_str); op2_str = NULL;
	} else {
	  hc_postfix_result_cleanup();
	  type_error("+ accepts either two numbers, two strings or two vectors");
	  return NULL;
	}
	curr = curr->n; // [sp++]
	sp -= 1;
	break;
	
      case '_':
	if (e[i+1] && !isdigit(e[i+1]))
	{
	  curr = curr->p;
	  if (curr->type == HC_VAR_NUM)
	  {
	    m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	    m_apmc_subtract(curr->re,curr->im,MM_Zero,MM_Zero,op1_r,op1_i);
	  } else if (curr->type == HC_VAR_VEC) {
	    char *tmp = curr->str;
	    curr->str = list_neg(curr->str);
	    free(tmp);
	    if (!curr->str)
	    {
	      hc_postfix_result_cleanup();
	      return NULL;
	    }
	  } else {
	    hc_postfix_result_cleanup();
	    type_error("- accepts only numbers or vectors");
	    return NULL;
	  }
	  curr = curr->n;
	  break;
	} else {
	  if (!e[i+1])
	  {
	    hc_postfix_result_cleanup();
	    syntax_error2();
	    return NULL;
	  }
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
	    hc_postfix_result_cleanup();
	    type_error("- accepts only two numbers or two vectors");
	    return NULL;
	  }
	  m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	  m_apmc_subtract(curr->re,curr->im,MM_Zero,MM_Zero,op1_r,op1_i);
	  curr = curr->n; // [sp++]
	  // no need to modify sp here
	} else {
	  curr = curr->p; // [--sp]
	  op2_type = curr->type;
	  if (curr->type == HC_VAR_NUM)
	  {
	    m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
	  } else if (curr->type == HC_VAR_VEC) {
	    op2_str = strdup(curr->str);
	  }
	  curr = curr->p; // [--sp]
	  op1_type = curr->type;
	  if (curr->type == HC_VAR_NUM) {
	    m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	  } else if (curr->type == HC_VAR_VEC) {
	    op1_str = strdup(curr->str);
	  }
	  if (op1_type == HC_VAR_NUM && op2_type == HC_VAR_NUM)
	  {
	    m_apmc_subtract(curr->re,curr->im,op1_r,op1_i,op2_r,op2_i);
	  } else if (op1_type == HC_VAR_VEC && op2_type == HC_VAR_VEC) {
	    free(curr->str);
	    free(curr->n->str);
	    curr->n->str = NULL;
	    if (!(curr->str = list_add_sub(op1_str, op2_str, '-')))
	    {
	      hc_postfix_result_cleanup();
	      return NULL;
	    }
	    free(op1_str); op1_str = NULL;
	    free(op2_str); op2_str = NULL;
	  } else {
	    hc_postfix_result_cleanup();
	    type_error("- accepts only two numbers or two vectors");
	    return NULL;
	  }
	  curr = curr->n; // [sp++]
	  sp -= 1;
	}
	break;
	
      case PW_SIGN:
	if (sp < 2)
	{
	  hc_postfix_result_cleanup();
	  hc_error(NOT_ENOUGH_OPERANDS,"%c",PW_SIGN);
	  return NULL;
	}
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  hc_postfix_result_cleanup();
	  type_error("^ accepts only numbers");
	  return NULL;
	}
	m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  hc_postfix_result_cleanup();
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

      case NOT_SIGN:
	if (sp < 1)
	{
	  hc_postfix_result_cleanup();
	  hc_error(NOT_ENOUGH_OPERANDS,"! (used as logical NOT)");
	  return NULL;
	}
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  hc_postfix_result_cleanup();
	  type_error("! (used as logical NOT) accepts only numbers");
	  return NULL;
	}
	m_apm_copy(curr->im,MM_Zero);
	if (m_apm_compare(curr->re,MM_Zero)==0)
	{
	  m_apm_copy(curr->re,MM_One);
	} else {
	  m_apm_copy(curr->re,MM_Zero);
	}
	curr = curr->n; // [sp++]
	break;

      case '!':
	switch (e[i+1])
	{
	case '=':
	  // interpret as condition
	  i++;
	  if (sp < 2)
	  {
	    hc_postfix_result_cleanup();
	    hc_error(NOT_ENOUGH_OPERANDS,"!=");
	    return NULL;
	  }
	  curr = curr->p; // [--sp]
	  op2_type = curr->type;
	  if (op2_type == HC_VAR_NUM)
	  {
	    // Avoid errors due to calculating internally with higher precision (HC_DEC_PLACES) and then truncating back to hc.precision
	    hc_round(op2_r,hc.precision,curr->re);
	    hc_round(op2_i,hc.precision,curr->im);
	  } else if (op2_type == HC_VAR_STR)
	  {
	    op2_str = get_string(curr->str);
	  } else if (op2_type == HC_VAR_VEC)
	  {
	    op2_str = strdup(curr->str);
	  }
	  curr = curr->p; // [--sp]
	  op1_type = curr->type;
	  if (op1_type == HC_VAR_NUM)
	  {
	    // Avoid errors due to calculating internally with higher precision (HC_DEC_PLACES) and then truncating back to hc.precision
	    hc_round(op1_r,hc.precision,curr->re);
	    hc_round(op1_i,hc.precision,curr->im);
	  } else if (op1_type == HC_VAR_STR) {
	    op1_str = get_string(curr->str);
	  } else if (op1_type == HC_VAR_VEC) {
	    op1_str = strdup(curr->str);
	  }
	  m_apm_copy(curr->im,MM_Zero);
	  curr->type = HC_VAR_NUM;
	  if (op1_type == op2_type)
	  {
	    if (op1_type == HC_VAR_NUM)
	    {
	      if (m_apmc_eq(op1_r,op1_i,op2_r,op2_i))
		m_apm_copy(curr->re,MM_Zero);
	      else
		m_apm_copy(curr->re,MM_One);
	    } else if (op1_type == HC_VAR_STR)
	    {
	      if (strcmp(op1_str,op2_str)==0)
		m_apm_copy(curr->re,MM_Zero);
	      else
		m_apm_copy(curr->re,MM_One);
	    } else if (op1_type == HC_VAR_VEC)
	    {
	      if (list_compare(op1_str,op2_str)==0)
		m_apm_copy(curr->re,MM_Zero);
	      else
		m_apm_copy(curr->re,MM_One);
	    }
	  } else {
	    m_apm_copy(curr->re,MM_One);
	  }
	  free(op1_str); free(op2_str);
	  op1_str = op2_str = NULL;
	  curr = curr->n;
	  sp -= 1;
	  break;
	default:
	  // interpret as factorial
	  if (sp < 1)
	  {
	    hc_postfix_result_cleanup();
	    hc_error(NOT_ENOUGH_OPERANDS,"! (used as factorial)");
	    return NULL;
	  }
	  curr = curr->p; // [--sp]
	  if (curr->type != HC_VAR_NUM)
	  {
	    hc_postfix_result_cleanup();
	    type_error("! (used as factorial) accepts only numbers");
	    return NULL;
	  }
	  if (!m_apm_is_integer(curr->re) || m_apm_compare(curr->im,MM_Zero)!=0)
	  {
	    hc_postfix_result_cleanup();
	    arg_error("! : an integer is required.");
	    return NULL;
	  }
	  m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
	  m_apm_factorial(curr->re,op1_r);
	  m_apm_copy(curr->im,MM_Zero);
	  curr = curr->n; // [sp++]
	  break;
	}
	break;
	
      case '<':
	if (sp < 2)
	{
	  hc_postfix_result_cleanup();
	  hc_error(NOT_ENOUGH_OPERANDS,"< / <=");
	  return NULL;
	}
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  hc_postfix_result_cleanup();
	  type_error("</<= accepts only numbers");
	  return NULL;
	}
	// Avoid errors due to calculating internally with higher precision (HC_DEC_PLACES) and then truncating back to hc.precision
	hc_round(op2_r,hc.precision,curr->re);
	hc_round(op2_i,hc.precision,curr->im);
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  hc_postfix_result_cleanup();
	  type_error("</<= accepts only numbers");
	  return NULL;
	}
	// Avoid errors due to calculating internally with higher precision (HC_DEC_PLACES) and then truncating back to hc.precision
	hc_round(op1_r,hc.precision,curr->re);
	hc_round(op1_i,hc.precision,curr->im);
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
	   hc_postfix_result_cleanup();
	   hc_error(NOT_ENOUGH_OPERANDS,"> / >=");
	   return NULL;
	}
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  hc_postfix_result_cleanup();
	  type_error(">/>= accepts only numbers");
	  return NULL;
	}
	// Avoid errors due to calculating internally with higher precision (HC_DEC_PLACES) and then truncating back to hc.precision
	hc_round(op2_r,hc.precision,curr->re);
	hc_round(op2_i,hc.precision,curr->im);
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  hc_postfix_result_cleanup();
	  type_error(">/>= accepts only numbers");
	  return NULL;
	}
	// Avoid errors due to calculating internally with higher precision (HC_DEC_PLACES) and then truncating back to hc.precision
	hc_round(op1_r,hc.precision,curr->re);
	hc_round(op1_i,hc.precision,curr->im);
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
	  hc_postfix_result_cleanup();
	  hc_error(NOT_ENOUGH_OPERANDS,"==");
	  return NULL;
	}
	curr = curr->p; // [--sp]
	op2_type = curr->type;
	if (op2_type == HC_VAR_NUM)
	{
	  // Avoid errors due to calculating internally with higher precision (HC_DEC_PLACES) and then truncating back to hc.precision
	  hc_round(op2_r,hc.precision,curr->re);
	  hc_round(op2_i,hc.precision,curr->im);
	} else if (op2_type == HC_VAR_STR)
	{
	  op2_str = get_string(curr->str);
	} else if (op2_type == HC_VAR_VEC)
	{
	  op2_str = strdup(curr->str);
	}
	curr = curr->p; // [--sp]
	op1_type = curr->type;
	if (op1_type == HC_VAR_NUM)
	{
	  // Avoid errors due to calculating internally with higher precision (HC_DEC_PLACES) and then truncating back to hc.precision
	  hc_round(op1_r,hc.precision,curr->re);
	  hc_round(op1_i,hc.precision,curr->im);
	} else if (op1_type == HC_VAR_STR) {
	  op1_str = get_string(curr->str);
	} else if (op1_type == HC_VAR_VEC) {
	  op1_str = strdup(curr->str);
	}
	m_apm_copy(curr->im,MM_Zero);
	switch(e[++i])
	{
	case '=':
	  curr->type = HC_VAR_NUM;
	  if (op1_type == op2_type)
	  {
	    if (op1_type == HC_VAR_NUM)
	    {
	      if (m_apmc_eq(op1_r,op1_i,op2_r,op2_i))
		m_apm_copy(curr->re,MM_One);
	      else
		m_apm_copy(curr->re,MM_Zero);
	    } else if (op1_type == HC_VAR_STR)
	    {
	      if (strcmp(op1_str,op2_str)==0)
		m_apm_copy(curr->re,MM_One);
	      else
		m_apm_copy(curr->re,MM_Zero);
	    } else if (op1_type == HC_VAR_VEC)
	    {
	      if (list_compare(op1_str,op2_str)==0)
		m_apm_copy(curr->re,MM_One);
	      else
		m_apm_copy(curr->re,MM_Zero);
	    }
	  } else {
	    m_apm_copy(curr->re,MM_Zero);
	  }
	  free(op1_str); free(op2_str);
	  op1_str = op2_str = NULL;
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
	  hc_postfix_result_cleanup();
	  hc_error(NOT_ENOUGH_OPERANDS,"||");
	  return NULL;
	}
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  hc_postfix_result_cleanup();
	  type_error("|| accepts only numbers");
	  return NULL;
	}
	m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  hc_postfix_result_cleanup();
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
	  hc_postfix_result_cleanup();
	  hc_error(NOT_ENOUGH_OPERANDS,"&&");
	  return NULL;
	}
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  hc_postfix_result_cleanup();
	  type_error("&& accepts only numbers");
	  return NULL;
	}
	m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
	curr = curr->p; // [--sp]
	if (curr->type != HC_VAR_NUM)
	{
	  hc_postfix_result_cleanup();
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
	if (isdigit(e[i]) || e[i]=='.' || e[i]=='_')
	{
	  type = HC_VAR_NUM;
	  char tmpsts = 0;
	  int tmpe = 1;
	  int tmpi = 1;
	  char base;
	  if (e[i]=='0' && tolower(e[i+1])=='x')
	    base = 16;
	  else if (e[i]=='0' && tolower(e[i+1])=='b')
	    base = 2;
	  else
	    base = 10;
	  /*
	    Base 10:

	    tmpsts holds various states in reading the number. These states are used for checking the syntax of the number -- ie. the allowed characters to occur at a particular state. tmpe and tmpi hold the number of allowed 'e's and 'i's respectively.

	    States
	    ------
	    0 : at the beginning of the scan, or after an 'e', or after an 'i'
	    1 : after a number, allowing for a decimal point
	    2 : after a number or a decimal point, not allowing for a decimal point
	    3 : after a negative sign
	   */
	  if (base == 10)
	  {
	    while ((!isspace(e[i]))&&(!isoperator(e[i]) || (i!=0 && tolower(e[i-1])=='e') || e[i]=='_')&& e[i])
	    {
	      if (hc.rpn) // otherwise, this has already been checked by hc_i2p()
	      {
		if (((tmpsts == 0) && (!isdigit(e[i]) && e[i]!='.' && e[i]!='_' && e[i]!='-')) || ((tmpsts == 1) && (!isdigit(e[i]) && e[i]!='.' && tolower(e[i])!='e' && tolower(e[i])!='i')) || ((tmpsts == 2) && (!isdigit(e[i]) && tolower(e[i])!='e' && tolower(e[i])!='i')) || ((tmpsts == 3) && (!isdigit(e[i]) && e[i]!='.')))
		{
		  hc_postfix_result_cleanup();
		  return NULL;
		}
		if (tolower(e[i]) == 'e')
		{
		  tmpsts = 0;
		  tmpe--;
		} else if (tolower(e[i]) == 'i')
		{
		  tmpsts = 0;
		  tmpi--;
		  tmpe = 1;
		} else if (isdigit(e[i]))
		{
		  if (tmpsts == 0 || tmpsts == 3)
		    tmpsts = 1;
		  // nothing needs to be done otherwise
		} else if (e[i]=='.')
		{
		  tmpsts = 2;
		} else if (e[i]=='_' || e[i]=='-')
		{
		  tmpsts = 3;
		}
		if (tmpe < 0 || tmpi < 0)
		{
		  hc_postfix_result_cleanup();
		  return NULL;
		}
	      }
	      tmp_num[j++] = e[i++];
	    }
	  } else { // base == 2 or base == 16
	    /* Base 2 or Base 16:

	       tmpsts contains either TRUE, to allow a decimal point, or FALSE, to allow only digits.
	       No more complexity is required as the exponents and complex numbers are not used, available, and for the case of the 'e' in hex, impossible to implement.
	    */
	    tmpsts = TRUE;
	    i+=2; // skip the initial '0x' or '0b'
	    while (!isspace(e[i]) && (!isoperator(e[i]) || e[i]=='_'))
	    {
	      if (hc.rpn && (!isdigitb(e[i],base) && (e[i]!='.' || tmpsts == FALSE))) // hc.rpn : if not true, this has already been checked by hc_i2p() so we can skip this
	      {
		hc_postfix_result_cleanup();
		return NULL;
	      } else {
		if (e[i]=='.')
		  tmpsts = FALSE;
		tmp_num[j++] = e[i++];
	      }
	    }
	    tmp_num[j] = '\0';
	    if (!hc_2dec(base,(char *)&tmp_num,MAX_DOUBLE_STRING))
	    {
	      hc_postfix_result_cleanup();
	      return NULL;
	    }
	    j = strlen(tmp_num);
	  }
	} else if (e[i]=='\"') {
	  type = HC_VAR_STR;
	  tmp_num[j++] = e[i++];
	  while (e[i]!='\"' && e[i])
	    tmp_num[j++] = e[i++];
	  if (e[i] != '\"')
	  {
	    hc_postfix_result_cleanup();
	    hc_error(SYNTAX,"missing end quotes");
	    return NULL;
	  }
	  tmp_num[j++] = e[i++];
	  while (e[i]=='[') // index
	  {
	    tmp_num[j] = 0;
	    if (!hc_get_by_index((char *)&tmp_num,&type,e,&i))
	    {
	      hc_postfix_result_cleanup();
	      return NULL;
	    }
	    j = strlen(tmp_num);
	  }
	} else if (e[i]=='[') {
	  type = HC_VAR_VEC;
	  unsigned int pct = 1;
	  char ignore = FALSE;
	  tmp_num[j++] = e[i++];
	  while (pct!=0 && e[i])
	  {
	    if (e[i]=='\"')
	    {
	      ignore = ignore == FALSE ? TRUE : FALSE;
	    }
	    if (e[i]=='[' && !ignore)
	      pct++;
	    if (e[i]==']' && !ignore)
	      pct--;
	    tmp_num[j++] = e[i++];
	  }
	  if (pct!=0)
	  {
	    hc_postfix_result_cleanup();
	    return NULL;
	  }
	  tmp_num[j]=0;
	  char *newlist = list_simplify((char *)&tmp_num);
	  if (!newlist)
	  {
	    hc_postfix_result_cleanup();
	    return NULL;
	  } else if (strlen(newlist) < MAX_DOUBLE_STRING)
	  {
	    strcpy(tmp_num,newlist);
	    free(newlist);
	  } else {
	    hc_postfix_result_cleanup();
	    overflow_error();
	  }
	  j = strlen(tmp_num);
	  while (e[i]=='[') // index
	  {
	    tmp_num[j]=0;
	    if (!hc_get_by_index((char *)&tmp_num,&type,e,&i))
	    {
	      hc_postfix_result_cleanup();
	      return NULL;
	    }
	    j = strlen(tmp_num);
	  }
	} else if (e[i]=='\'') { // lambda expression
	  char *lambda_expr = malloc(MAX_EXPR);
	  if (!lambda_expr) mem_error();
	  char *lambda_end = strchr_outofblock((char *)(e + i + 1),'\'');
	  if (!lambda_end)
	  {
	    hc_error(SYNTAX,"single quotes do not match");
	    hc_postfix_result_cleanup();
	    free(lambda_expr);
	    return NULL;
	  }
	  if (((char *)(lambda_end + 1))[0] != '(')
	  {
	    hc_error(SYNTAX,"( expected after lambda expression");
	    hc_postfix_result_cleanup();
	    free(lambda_expr);
	    return NULL;
	  }
	  memcpy(lambda_expr,(char *)(e + i + 1),lambda_end - e - i - 1);
	  lambda_expr[lambda_end - e - i - 1] = '\0';
	  i = lambda_end - e + 1;
	  char *args_end = strchr_outofblock((char *)(e + i),')');
	  if (!args_end)
	  {
	    hc_error(SYNTAX,"('s and )'s do not match");
	    hc_postfix_result_cleanup();
	    free(lambda_expr);
	    return NULL;
	  }
	  char *args = malloc(MAX_EXPR); if (!args) mem_error();
	  memcpy(args,(char *)(e + i + 1),args_end - e - i - 1);
	  args[args_end - e - i - 1] = '\0';
	  i = args_end - e + 1;
	  if (!hc_eval_lambda((char *)&tmp_num, MAX_DOUBLE_STRING, &type, lambda_expr, args))
	  {
	    free(lambda_expr); free(args);
	    hc_postfix_result_cleanup();
	    return NULL;
	  }
	  free(lambda_expr); free(args);
	  j = strlen(tmp_num);
	  while (e[i]=='[')
	  {
	    tmp_num[j]=0;
	    if (!hc_get_by_index((char *)&tmp_num,&type,e,&i))
	    {
	      hc_postfix_result_cleanup();
	      return NULL;
	    }
	    j = strlen(tmp_num);
	  }
	  j = strlen(tmp_num);
	} else {
	  if (!isalpha(e[i]))
	  {
	    hc_postfix_result_cleanup();
	    syntax_error2();
	    return NULL;
	  }
	  char *v_name = malloc(MAX_V_NAME * sizeof(char));
	  char *f_expr = malloc(MAX_F_TMP * sizeof(char));
	  if (!v_name || !f_expr)
	    mem_error();
	  f_expr[0] = '\0';
	  int ti = 0;
	  while ((isalpha(e[i]) || isdigit(e[i])) && (ti < MAX_V_NAME - 1)) // the first to be checked can't be a digit since that would be caught above
	  {
	    v_name[ti++] = e[i++];
	  }
	  v_name[ti] = '\0';
	  if (isalpha(e[i]))
	  {
	    hc_error(SYNTAX,"name of variable or function too long (limit is %i characters)",MAX_V_NAME);
	    free(v_name); free(f_expr);
	    hc_postfix_result_cleanup();
	    return NULL;
	  } else if (e[i] == '(') { // function
	    f_expr[0] = e[i];
	    ti = 1;
	    char ignore = FALSE;
	    int par = 1;
	    while (par!=0 && e[i])
	    {
	      f_expr[ti++] = e[++i];
	      if (e[i]=='\"')
	      {
		ignore = ignore == FALSE ? TRUE : FALSE;
		continue;
	      }
	      if (ignore)
		continue;
	      if (e[i]=='(')
		par++;
	      else if (e[i]==')')
		par--;
	    }
	    if (par == 0)
	    {
	      f_expr[ti] = '\0';
	      i++; // skip the last ')'
	    } else {
	      hc_error(SYNTAX,"('s and )'s do not match");
	      free(v_name); free(f_expr);
	      hc_postfix_result_cleanup();
	      return NULL;
	    }
	  }
	  if (!hc_value((char *)&tmp_num, MAX_DOUBLE_STRING, &type, v_name, f_expr))
	  {
	    free(v_name); free(f_expr);
	    hc_postfix_result_cleanup();
	    return NULL;
	  }
	  free(v_name); free(f_expr);
	  j = strlen(tmp_num);
	  while (e[i]=='[')
	  {
	    tmp_num[j]=0;
	    if (!hc_get_by_index((char *)&tmp_num,&type,e,&i))
	    {
	      hc_postfix_result_cleanup();
	      return NULL;
	    }
	    j = strlen(tmp_num);
	  }
	  j = strlen(tmp_num);
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
	} else if (curr->type == HC_VAR_VEC)
	{
	  curr->str = strdup(tmp_num);
	} else if (curr->type == HC_VAR_EMPTY)
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
    hc_postfix_result_cleanup();
    syntax_error2();
    return NULL;
  }
  
  curr = curr->p;

  char *result = hc_strrepr(curr);
  
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

  return result;
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
  if (hc.rpn)
  {
    char *f2 = f;
    f = strreplace_(f," -"," _");
    free(f2);
  }
  if (strlen(f)>=MAX_EXPR)
  {
    free(f);
    overflow_error_nq();
    return NULL;
  }
  char *e = malloc(MAX_EXPR);
  if (!e)
    mem_error();
  strcpy(e,f);
  free(f);

  int i=0;
  int count=0;
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
  case HASH_P:
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
  case HASH_TSTEP:
    if (strtod(&d[i+1],NULL) <= 0.001)
    {
      printf("Please specify a real number larger than 0.001.\n");
    } else {
      hc.peqstep = strtod(&d[i+1],NULL);
      printf("t-step for parametric equations is now set to %f.\n",hc.peqstep);
    }
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
      free(hc_var_tmp->name);
      free(hc_var_tmp->value);
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
    hc_var_first->hash = 0;
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
      if (((isalpha(e[i]) || e[i]=='(') && tolower(e[i])!='i' && tolower(e[i])!='e') && last_was_num && (last_was_num!='0' || (tolower(e[i])!='x' && tolower(e[i])!='b')))
      {
	e = realloc(e, strlen(e)+2);
	memmove(e+sizeof(char)*i+sizeof(char), e+sizeof(char)*i, strlen(e)-i+1);
	e[i] = '*';
	last_was_num = 0;
      }
      if ((isdigit(e[i]) || e[i]=='.') && could_be_num)
      {
	last_was_num = e[i];
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
  hc.peqstep = HC_PEQSTEP_DEFAULT;
  hc.xmin2d = hc.ymin2d = hc.xmin3d = hc.ymin3d = hc.zmin3d = hc.xminsf = hc.yminsf = hc.xminpeq = hc.yminpeq = -10;
  hc.xmax2d = hc.ymax2d = hc.xmax3d = hc.ymax3d = hc.zmax3d = hc.xmaxsf = hc.ymaxsf = hc.xmaxpeq = hc.ymaxpeq = 10;
  hc.tminpeq = 0;
  hc.tmaxpeq = 2 * 3.1415926535897932384626433832795028841971693993751058209749445923;
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
      case HASH_EXPF:
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
      case HASH_ANGLE:
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
      case HASH_XMINSF:
	hc.xminsf = strtod(&buffer[i+1],NULL);
	break;
      case HASH_XMAXSF:
	hc.xmaxsf = strtod(&buffer[i+1],NULL);
	break;
      case HASH_YMINSF:
	hc.yminsf = strtod(&buffer[i+1],NULL);
	break;
      case HASH_YMAXSF:
	hc.ymaxsf = strtod(&buffer[i+1],NULL);
	break;
      case HASH_TMINPEQ:
	hc.tminpeq = strtod(&buffer[i+1],NULL);
	break;
      case HASH_TMAXPEQ:
	hc.tmaxpeq = strtod(&buffer[i+1],NULL);
	break;
      case HASH_XMINPEQ:
	hc.xminpeq = strtod(&buffer[i+1],NULL);
	break;
      case HASH_XMAXPEQ:
	hc.xmaxpeq = strtod(&buffer[i+1],NULL);
	break;
      case HASH_YMINPEQ:
	hc.yminpeq = strtod(&buffer[i+1],NULL);
	break;
      case HASH_YMAXPEQ:
	hc.ymaxpeq = strtod(&buffer[i+1],NULL);
	break;
      case HASH_PEQSTEP:
	hc.peqstep = strtod(&buffer[i+1],NULL);
	break;
      }
    }
    free(buffer);
    fclose(fr);
  }

  if (hc.announce == TRUE)
  {
    printf("Configuration :\n Angle format = %s | RPN mode = %s | Precision = %i | EXP format = %s\n",hc.angle=='r' ? "RAD" : hc.angle=='d' ? "DEG" : "GRAD",hc.rpn==0 ? "off" : "on",hc.precision,hc.exp=='s' ? "SCI" : hc.exp=='e' ? "ENG" : "NORMAL");
    hc.announce = FALSE;
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
  fprintf(fw,"peqstep=%f\n",hc.peqstep);
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
  fprintf(fw,"xminsf=%f\n",hc.xminsf);
  fprintf(fw,"xmaxsf=%f\n",hc.xmaxsf);
  fprintf(fw,"yminsf=%f\n",hc.yminsf);
  fprintf(fw,"ymaxsf=%f\n",hc.ymaxsf);
  fprintf(fw,"tminpeq=%f\n",hc.tminpeq);
  fprintf(fw,"tmaxpeq=%f\n",hc.tmaxpeq);
  fprintf(fw,"xminpeq=%f\n",hc.xminpeq);
  fprintf(fw,"xmaxpeq=%f\n",hc.xmaxpeq);
  fprintf(fw,"yminpeq=%f\n",hc.yminpeq);
  fprintf(fw,"ymaxpeq=%f\n",hc.ymaxpeq);
  fclose(fw);
}


void hc_cleanup()
{
  hc_output(PRINT,""); // make it close file descriptors (if any)
  is_zero_free(); // free is_zero()'s M_APM if allocated
  hc_save_cfg();
}


char hc_is_predef(char *var)
{
  unsigned int i = 0;
  unsigned int name_hash = simple_hash(var);
  for (; i < HC_NAMES; i++)
  {
    if (name_hash == hc_hashes[i])
    {
      return 1;
    }
  }
  return 0;
}


void hc_load(char *fname_)
{
#ifndef WIN32
  wordexp_t expanded_fname;
  FILE *fr = NULL;
  if (!wordexp(fname_, &expanded_fname, 0))
  {
    fr = fopen(expanded_fname.we_wordv[0],"r");
  }
  wordfree(&expanded_fname);
#else
  FILE *fr = fopen(fname_,"r");
#endif

  if (!fr)
  {
    perror("Error");
    error_nq("Error: Cannot open file.");
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
      continue;
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
