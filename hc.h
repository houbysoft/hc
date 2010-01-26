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


#ifndef HC_H
#define HC_H
#include <math.h>
#include <m_apm.h>
#include "m_apmc.h"
//#define NDEBUG
//#include <assert.h>

#define HC_CFG_RELATIVE "/.hc"
#define HC_CFG_RELATIVE_WIN32 "\\hc.cfg"
#define HC_CFG_FILE hc_cfg_get_fn()
#define HC_CFG_BUF_SIZE 512
#define HC_MAX_PRECISION 128
#define HC_FNAMES 69
#define HC_NESTED_MAX 128

#define HC_GRAPH_N_MAX 128

#define SUCCESS 1
#define FAIL 0

#ifdef DBG
#define syntax_error() {printf("Syntax Error @ %s:%i.\n",__FILE__,__LINE__);exit(2);}
#define syntax_error2() syntax_error()
#else
#define syntax_error() {printf("Syntax Error.\n");exit(2);}
#ifndef HCG_E
#define syntax_error2() {if (announce_errors==TRUE) {printf("Syntax Error.\n");announce_errors=FALSE;}}
#else
#define syntax_error2() {if (announce_errors==TRUE) {notify_error("Syntax Error.\n");announce_errors=FALSE;}}
#endif
#endif
#ifndef HCG_E
#define d0_error() {if (!graphing_ignore_errors){printf("Division by zero error.\n");}}
#else
#define d0_error() {if (!graphing_ignore_errors){notify_error("Division by zero error.\n");}}
#endif
#ifndef HCG_E
#define arg_error(str) {if (!graphing_ignore_errors){printf("Argument Error : %s\n",str);}return FAIL;}
#define arg_error_custom() {printf("User-defined function argument error.\n");}
#else
#define arg_error(str) {if (!graphing_ignore_errors){notify_error("Argument Error.");}return FAIL;}
#define arg_error_custom() {notify_error("User-defined function argument error.\n");}
#endif
#ifndef HCG_E
#define undef_error() {printf("Function name undefined or you used an undefined variable name as an argument.\n");}
#else
#define undef_error() {notify_error("Function name undefined or you used an undefined variable name as an argument.\n");}
#endif
#ifndef HCG_E
#define varname_error() {printf("Invalid variable name.\n");return;}
#define varname_predefined_error() {printf("You can't change a predefined variable.\n");return 0;}
#define recursive_error() {printf("Error : recursive definition.\n");return;}
#define recursion_error() {printf("Error : too much recursion/nestedness. If you know what you are doing, you can bypass this with:\n \\bpn.\n");}
#define boxplot_cplx_error() {printf("Warning : Complex part of number was ignored in the boxplot.\n");}
#else
#define varname_error() {notify_error("Invalid variable name.\n");return;}
#define varname_predefined_error() {notify_error("You can't change a predefined variable.\n");return 0;}
#define recursive_error() {notify_error("Error : recursive definition.\n");return;}
#define recursion_error() {notify_error("Error : too much recursion/nestedness. This can be bypassed in the command-line version if you know what you are doing.\n");}
#define boxplot_cplx_error() {notify_error("Warning : Complex part of number was ignored in the boxplot.\n");}
#endif
#define mem_error() {printf("Cannot allocate enough memory.\n");exit(5);}
//#define range_error(str) {printf("Range error : %s\n",str);exit(6);}
#define overflow_error() {printf("Overflow error in result string (result probably too large).\n");exit(7);}

#define warning(str) {printf("Warning : %s\n",str);}
#define error_nq(str) {printf(str);}
#ifndef HCG
#define notify(str) {printf(str);}
#endif

#define NAME_VERSION "Welcome to hc 0.9.1 (infix / RPN) -- http://hc.houbysoft.com/\n"

#define MAX_EXPR 16384
#define MAX_F_TMP (MAX_EXPR / 2)
#define MAX_F_NAME 16
#define MAX_DOUBLE_STRING MAX_EXPR // To make the routines to check overflows work properly, do not change this (but you can change MAX_EXPR, obviously)
#define MAX_OP_STACK (MAX_EXPR / 2)
//#define MAX_DOUBLE_STACK MAX_EXPR
#define HC_INF "inf"
#define HC_DEC_PLACES (hc.precision*2)
#define HC_GP3D_DEFAULT 25
#define HC_GP3D_FAST 250

#define PW_SIGN '^'

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

#define HC_USR_VAR 1
#define HC_USR_FUNC 2

typedef struct hc_config {
  char rpn;
  int precision;
  char output_format;
  char exp;
  char angle;
  char keys; // GUI only - whether the clickable keys should be displayed
  char bypass_nested; // used to bypass the recursion protection
  char *plplot_dev_override;
  int graph_points;
  int graph_width;
  int graph_height;
  unsigned int graph_points_3d;
  double xmin2d;
  double xmax2d;
  double ymin2d;
  double ymax2d;
  double xmin3d;
  double xmax3d;
  double ymin3d;
  double ymax3d;
  double zmin3d;
  double zmax3d;
} hc_config_;

typedef struct hc_stack_element {
  struct hc_stack_element *p;
  M_APM re; // real part of the number
  M_APM im; // imaginary part of the number
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
  char *value;
  char *args; // functions only
  char type; // function or variable
  struct hc_ventry *next;
} hc_ventry_;

extern struct hc_config hc;
extern struct hc_ventry *hc_var_first;
extern char announce_errors;
extern char graphing_ignore_errors;
extern const char *hc_fnames[][2];

extern void hc_load_cfg();
extern char *hc_result(char *e);
extern char *hc_result_(char *e);
extern char *hc_plusminus(char *);
extern void hc_varassign(char *);
extern char hc_check_varname(char *);
extern unsigned int simple_hash(char *p);

#endif
