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


#ifndef HC_H
#define HC_H
#include <m_apm.h>
#include <stdio.h> // for readline
#include "m_apmc.h"
#include "hc_names.h"
#include "hc_info.h"
#include "hc_utils.h"
#ifndef HCG_E
#include <readline/readline.h>
#endif
//#define NDEBUG
//#include <assert.h>

#define HC_CFG_RELATIVE "/.hc"
#define HC_CFG_RELATIVE_WIN32 "\\hc.cfg"
#define HC_CFG_FILE hc_cfg_get_fn()
#define HC_CFG_BUF_SIZE 512
#define HC_NESTED_MAX 128

// has to be pair
#define HC_GRAPH_N_MAX 128

#define SUCCESS 1
#define FAIL 0

#ifndef HCG_E
#define d0_error() {if (!graphing_ignore_errors){printf("Division by zero error.\n");}}
#else
#define d0_error() {if (!graphing_ignore_errors){notify_error("Division by zero error.\n");}}
#endif
#define arg_error(str) {if (!graphing_ignore_errors){hc_error(ARG, str);}return FAIL;}
#ifndef HCG_E
#define arg_error_custom() {printf("User-defined function argument error.\n");}
#else
#define arg_error_custom() {notify_error("User-defined function argument error.\n");}
#endif
#ifndef HCG_E
#define undef_error() {printf("Function name undefined or you used an undefined variable name as an argument.\n");}
#else
#define undef_error() {notify_error("Function name undefined or you used an undefined variable name as an argument.\n");}
#endif
#ifndef HCG_E
#define varname_error() {printf("Invalid variable name (variables must contain only letters and numbers, and may not be 'e' or 'i', which are used for exponents and complex numbers respectively).\n");return;}
#define varname_predefined_error() {printf("You can't change a predefined variable.\n");return 0;}
#define var_nospecial_error() {printf("You cannot use +=, -=, *= or /= with this type of variable.\n");return;}
#define recursion_error() {printf("Error : too much recursion/nestedness. If you know what you are doing, you can bypass this with:\n \\bpn.\n");}
#define boxplot_cplx_error() {printf("Warning : Complex part of number was ignored in the boxplot.\n");}
#define dim_error() printf("Dimension error.\n")
#else
extern void unknown_var_error(char *var, char type);
#define dim_error() notify_error("Dimension error.\n")
#define varname_error() {notify_error("Invalid variable name (variables must contain only letters and numbers, may not be 'e' or 'i', which are used for exponents and complex numbers respectively).\n");return;}
#define varname_predefined_error() {notify_error("You can't change a predefined variable.\n");return 0;}
#define var_nospecial_error() {notify_error("You cannot use +=, -=, *= or /= with this type of variable.\n");return;}
#define recursion_error() {notify_error("Error : too much recursion/nestedness. This can be bypassed in the command-line version if you know what you are doing.\n");}
#define boxplot_cplx_error() {notify_error("Warning : Complex part of number was ignored in the boxplot.\n");}
#endif
#define mem_error() {printf("Cannot allocate enough memory.\n");exit(5);}
//#define range_error(str) {printf("Range error : %s\n",str);exit(6);}
#define overflow_error() {printf("Overflow error in result string (result probably too large).\n");exit(7);}

#define warning(str) {printf("Warning : %s\n",str);}
#define error_nq(str) {printf("%s\n",str);}
#if !defined(HCG) && !defined(HCG_E)
#define notify(str) printf("%s",str)
#define notify_nonmodal(str) notify(str)
#define notify_console(str) notify(str)
#define notify_error(str) notify(str)
#define prompt(str) readline(str)
#else
extern void notify(char *);
extern void notify_nonmodal(char *);
extern void notify_console(char *);
extern void notify_error(char *);
extern char *prompt(char *);
#endif
#define is_string(x) (strip_spaces(x)[0]=='\"' && last_char(x)=='\"')
#define is_vector(x) (strip_spaces(x)[0]=='[' && last_char(x)==']')
#define is_list(x) is_vector(x)
extern char is_num(char *);
extern char is_real_num(char *);
extern char is_int(char *);
extern char is_positive_int(char *); // positive or zero

#define MAX_EXPR 16384
#define MAX_V_NAME 64 // maximum length of variable / function name
#define MAX_OP_STACK (MAX_EXPR / 2)
#define MAX_LAMBDA_ARGS 100
//#define MAX_DOUBLE_STACK MAX_EXPR
#define HC_DEC_PLACES (hc.precision*2)
#define HC_GP3D_DEFAULT 25
#define HC_GP3D_FAST 250
#define HC_PEQSTEP_DEFAULT 0.05

#define PW_SIGN '^'
#define NOT_SIGN '?'

#ifndef MM_PI
#define HC_PI 3.14159265359
#else
#define HC_PI MM_PI
#endif

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

#define TRUE 1
#define FALSE 0

#define REAL 0
#define IMAG 1

#define HC_USR_VAR 1
#define HC_USR_FUNC 2

#define HC_MAPM 1
#define HC_UINT 2

typedef enum {
  HC_VAR_INVALID, HC_VAR_EMPTY, HC_VAR_NUM, HC_VAR_STR, HC_VAR_VEC
} HC_VAR_TYPES;

typedef enum {
  HC_COND_E, HC_COND_NE, HC_COND_GE, HC_COND_SE, HC_COND_S, HC_COND_G
} HC_CONDS;

typedef enum {
  HC_EXEC_IF, HC_EXEC_WHILE, HC_EXEC_FOR
} HC_EXECS;

typedef struct hc_stack_element {
  struct hc_stack_element *p;
  char type; // type of element
  M_APM re; // real part of the number (if element is a number)
  M_APM im; // imaginary part of the number (if element is a number)
  char *str; // string (if element is a string)
  struct hc_stack_element *n;
} hc_stack_element_;


typedef struct hc_stack_element_stats {
  struct hc_stack_element_stats *p;
  M_APM re; // real part of the number
  M_APM im; // imaginary part of the number
  M_APM ef; // effective / how many times that value is present in the set
  struct hc_stack_element_stats *n;
} hc_stack_element_stats_;


typedef struct hc_ventry {
  char *name;
  unsigned int hash; // hash of the name of the variable / function -- this is used in some functions for speed where the string name is not important
  char *value;
  char *args; // functions only
  char type; // function or variable
  struct hc_ventry *next;
} hc_ventry_;

extern struct hc_ventry *hc_var_first;
extern char announce_errors;
extern char graphing_ignore_errors;
extern unsigned int hc_hashes[HC_NAMES];
extern char *_cur_var_name_;
extern M_APM hc_lans_mapm_re;
extern M_APM hc_lans_mapm_im;
extern char *hc_lans_strvec;
extern char hc_lans_type;
extern M_APM MM_MOne;

extern void hc_load_cfg();
extern void hc_load(char *);
extern char *hc_result(char *e);
extern char *hc_result_(char *e);
extern char *hc_plusminus(char *);
extern char *hc_impmul_resolve(char *);
extern void hc_varassign(char *);
extern char *hc_exec_struct(char *);
extern char hc_check_varname(char *);
extern unsigned int simple_hash(char *p);
extern char hc_is_predef(char *var);
extern char *hc_value(char *type, char *v_name, char *f_expr);

#endif
