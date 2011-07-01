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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <m_apm.h>
#include <locale.h>
#if !defined(WIN32) && !defined(NO_WORDEXP)
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


/* hc_names fields
 * ---------------
 * [x][0] : name of element that can be used to use/call/... it
 * [x][1] : type and other info
 * [x][2] : menu path for GUIs etc.
 * [x][3] : short description
 * [x][4] : full help text (if there is any, and if it is different from the short description)
 */
const char *hc_names[][5] = {
  {"ans","func:","Functions/Misc","last result","returns the result of the last expression\n    Note that there is also an easier way to reuse the last result; simply start the new line with an operator. Example:\n      > 2 + 1\n      3\n      > + 4\n       7\n    Not all operators can however be used in this way, since for some ambiguity would result; in those cases, use ans() which is always unambiguous."},
  {"abs","func:x","Functions/Math","absolute value of x",""},
  {"acos","func:x","Functions/Trigonometry","arc cosine of x",""},
  {"asin","func:x","Functions/Trigonometry","arc sine of x",""},
  {"atan","func:x","Functions/Trigonometry","arc tangent of x",""},
  {"atomicnumber","func:str","Functions/Chemistry","atomic number (Z) of element in str","returns the atomic number (Z) of element in string str. For example, to get the atomic number of Oxygen, type atomicnumber(\"O\")"},
  {"boxplot","func:[v_1,v_2,...,v_n]","Stats","draws a boxplot of values v_1...v_n","draw a boxplot of values v1...vn Each of these values can either be a single number, or a list containing two elements : the number and its frequency. For examples, view help for \"stats\" which has the same argument format."},
  {"cbrt","func:x","Functions/Math","cube root of x (=x^(1/3))","returns the cube root of x (you can also use x^(1/3))"},
  {"ceil","func:x","Functions/Math","rounds x up to nearest integer",""},
  {"chartocode","func:x","Conversions","converts a character to its ASCII code","converts x, a single-character string, to its ASCII code. For example, to get the ascii value of an uppercase A, type chartocode(\"A\")"},
  {"clear","func:var","Functions/Misc","clears variable named var","clears (undefines) var. You can also use \\clear to clear (undefine) all user-defined variables and functions."},
  {"codetochar","func:x","Conversions","converts an ASCII code to a character","converts x, an ASCII code, to the corresponding character. For example, codetochar(65) will return \"A\"."},
  {"convert","func:x,in_unit,out_unit","Conversions","converts x in_units to out_units","Converts x in_units to out_units. You can also just type convert() without arguments, and it will print the supported units on the console. Example:\n    - say you want to convert 20 centimeters into feet; type convert(20, \"centimeters\", \"feet\"). Note that synonyms like \"cm\" instead of \"centimeters\" can also be used."},
  {"cos","func:x","Functions/Trigonometry","cosine of x",""},
  {"cosh","func:x","Functions/Trigonometry","hyperbolic cosine of x",""},
  {"count","func:list,x","Functions/Misc", "number of occurences of x in list",""},
  {"crossp","func:a,b","Functions/Vectors","vector cross product - a X b","Returns the vector cross product of a X b. For example, if a is the vector [1,2,3] and b is the vector [-1,-2,-3], the result would is [0,0,0] -- crossp([1,2,3],[-1,-2,-3]) = [0,0,0]"},
  {"dotp","func:a,b","Functions/Vectors","vector dot product - a . b","return the vector dot product of a . b; it is also sometimes called scalar product, as the result is a scalar. For example, dotp([1,2,3],[4,5,6]) = 32"},
  {"exp","func:x","Functions/Math","exponential function",""},
  {"factorial","func:x","Functions/Math","factorial of x (=x!)","factorial of x (you could also use the '!' notation)"},
  {"fibo","func:n","Functions/Math","gives the nth Fibonacci number",""},
  {"floor","func:x","Functions/Math","rounds x down to the nearest integer",""},
  {"for","func:init,condition,iter,body","Programming","evaluates init, then body while condition is true, evaluating iter each iteration","executes init, and then executes body while condition holds, executing iter at the end of each iteration.\n    Example:\n     * this example shows how to print numbers from 1 to 5 with for:\n       for(j=1, j<=5, j+=1, print(j)) would print out:\n1\n2\n3\n4\n5"},
  {"gamma","func:z","Functions/Math","Euler Gamma function",""},
  {"gcd","func:x,y","Functions/Math","greatest common denominator of x and y","finds the greatest common divisor / denominator of x and y"},
  {"graph","func:expr,xmin,xmax,ymin,ymax","Graphs","draw a 2D graph. xmin,...,ymax are optional","draw a graph (generate a PNG file if in command-line), example syntax:\n    graph(x^2,-10,10,-100,100)\n    Note:\n     * the xmin, xmax, ymin and ymax arguments are optional; if not provided, the last will be used (ie after a call of the example, graph(something) would use -10, 10, -100 and 100\n     * to draw multiple functions on one graph, pass them as a list for expr. For example, if you want to draw x, x^2, and x^3 on the same graph, you can use something like:\n       graph([x,x^2,x^3],-10,10,-100,100)"},
  {"graphpeq","func:expr_x,expr_y,tmin,tmax,xmin,xmax,ymin,ymax","Graphs","draw a parametric graph. tmin,...,ymax are optional","draw the parametric equation modeled by expr_x and expr_y (use t as the variable)\n    Note:\n     * the tmin, tmax, xmin, xmax, ymin and ymax arguments are optional"},
  {"graphp","func:[v_1,v_2,...,v_n]","#","","see help for graphpoints()"},
  {"graphpoints","func:[v_1,v_2,...,v_n]","Graphs","draw a data-point plot of values v_1,...,v_n","same as graphvalues(), but only draws points (ie. it does not connect them with lines)"},
  {"graphpolar","func:expr_r,tmin,tmax,xmin,xmax,ymin,ymax","Graphs","draw a polar graph, tmin,...,ymax are optional","draw the polar equation modeled by expr_r, where t is the variable.\n    Note:\n     * the tmin, tmax, xmin, xmax, ymin and ymax arguments are optional"},
  {"graphv","func:[v_1,v_2,...,v_n]","#","","see help for graphvalues()"},
  {"graphvalues","func:[v_1,v_2,...,v_n]","Graphs","draw a xy-line plot of values v_1,...,v_n","synonym : graphv() - Use this to graph a table of values as a xy line. The values must be in a list. There are two formats which you can use; either simply list the y coordinates, and the x coordinates will be automatically added, starting from 0 and going +1 for each value, or the values can themselves be lists containing two elements: the x and the y coordinate.\n    Examples:\n     * Let's say you want to draw a line connecting three points: (0,0), (1,3), and (2,6). Since the x values are all just +1 increments, you can use : graphvalues([0,3,6]) or graphv([0,3,6])\n     * Now let's say your points are (0,0), (5,3) and (8,6). You can't use the same as above since HC would think your points' x coordinates are 0,1 and 2. For this data, you would need to use: graphvalues([[0,0],[5,3],[8,6]]) or graphv([[0,0],[5,3],[8,6]])"},
  {"gmul","func:expr_1,expr_2,...,expr_n","Graphs","deprecated - use graph([expr_1,expr_2,...,expr_n]) instead","- [DEPRECATED, use graph([expr_1, expr_2, ..., expr_n]) instead] gmul(expr_1, expr_2, ..., expr_n) - similar to graph, but can draw an arbitrary number of functions on one graph (note : to set xmin, etc., make a call to graph() first)"},
  {"graph3","func:expr,xmin,xmax,ymin,ymax,zmin,zmax","Graphs","draw a 3D graph. xmin,...,zmax are optional","draw a 3D graph (generate a PNG file if in command-line), example syntax:\n    graph3(x+y,-10,10,-10,10,-20,20)\n    Note:\n     * as with graph(), the xmin, xmax, ymin, ymax, zmin and zmax are optional"},
  {"help","func:topic","#","provide help about topic - try help(\"graph\")",""},
  {"if","func:condition,truebranch,falsebranch","Programming","if condition is true, evaluates truebranch, otherwise falsebranch","evaluates condition, and if it is true, returns truebranch, otherwise returns falsebranch.\n    Note:\n     * falsebranch is optional\n\n    Example:\n     * if(1==2,\"hello\",\"hi\") would return \"hi\", since 1 is not equal to 2. If the falsebranch would have been ommited, in this case nothing would be returned:\n       if(1==2,\"hello\") returns nothing."},
  {"im","func:x","#","","see help for imag()"},
  {"imag","func:x","Functions/Math","imaginary part of x","returns the imaginary part of x. For example, imag(3i2) would return 2."},
  {"input","func:str","Functions/Misc","ask the user for a value, using str as prompt","shows an inut dialog using either a standard prompt or str, if specified. For example, to ask the user a number and save it in x, use x = input(\"Enter a number please\")"},
  {"join","func:a,b","Functions/Misc","join (concatenate) a and b","join a and b, two vectors, together. For example, join([1,2],[3,4]) returns [1,2,3,4]"},
  {"lcm","func:x,y","Functions/Math","least common multiple of x and y","finds the least common multiple of x and y"},
  {"length","func:x","Functions/Misc","length of x","return the length of x. For example, length(\"hello\") = 5. Another example, length([42,27,69]) = 3"},
  {"linreg","func:[v_1,v_2,...,v_n]","Stats","returns slope and y-intercept of best-fit line through v_1,...,v_n","returns a list of two values; the first one being the slope and the second the intercept of the \"best fit\" line through the data points\n    Example:\n     * linreg([[1,3],[2,6],[3,8.95]]) would return [2.975,0.0333333333333333] which means that the best fit line would have the equation 2.975x + 0.0333333333333333"},
  {"ln","func:x","Functions/Math","natural logarithm of x",""},
  {"log","func:b,x","Functions/Math","base b logarithm of x","gives the base b logarithm of x. For example, to get the base-5 logarithm of 4, use log(5,4)"},
  {"log10","func:x","Functions/Math","common logarithm of x",""},
  {"map","func:f,list","Programming","maps f to each element in list","map f to each element in list; ie. return a list of the form [f(list[0]),f(list[1]),...]\n    Examples:\n     * create a function that will show the n first fibonacci numbers:\n       > showfibo(n) = map(fibo,range(1,n+1))\n       > showfibo(10)\n        [1,1,2,3,5,8,13,21,34,55]"},
  {"mmass","func:str","Functions/Chemistry","molar mass of molecules - see help(\"mmass\") for more info","returns the molar mass of the molecule specified (use standard notation without using subscripts).\n    Examples:\n      * H20\n      * Ca(OH)2"},
  {"mod","func:x,y","Functions/Math","modulo division (=x%y)","modulus (you could also use the C-style '%' notation"},
  {"nCr","func:n,k","Functions/Combinatorics","binomial coefficient","binomial coefficient (combinatorics)"},
  {"nPr","func:n,k","Functions/Combinatorics","number of permutations","number of permutations (combinatorics)"},
  {"num","func:str","Functions/Misc","number representation of string str","returns a number representation of the number in string str"},
  {"ones","func:n","Functions/Misc","returns an array containing n ones","generate a list containing n ones. See also zeros().\n    Example:\n     * Generate a list containing eight fives:\n      ones(8) * 5"},
  {"print","func:element_1,element_2,...,element_n","Functions/Misc","prints out its arguments","prints its number or string arguments (if more than one argument is passed, the outputs will be separated by a space) and a newline character (useful for printing variables in loops)"},
  {"product","func:expr,low,high","Functions/Math","\"big Pi\" product with x as the variable","'capital Pi product'. expr can contain 'x', which will be incremented from low to high inclusive.\n    Example:\n     * product(x^2,1,5) == 14400"},
  {"rand","func:n","Functions/Misc","if n is specified, returns a random integer in [1;n]; otherwise returns a real in [0;1[","if int is specified, return a random integer in the range [1 ; int], otherwise return a real number in the range [0;1["},
  {"range","func:a,b","Functions/Misc","if a is specified, list with integers from a (included) to b (not included); if not, from 0 (included) to b (not included)","return a list containing numbers from a (included) to b (not included). If only one argument is provided, return a list containing numbers from 0 (included) to the argument (not included).\n    Examples:\n     * range(5) will return [0,1,2,3,4]\n     * range(2,4) will return [2,3]\n     * range(3,3) will return []"},
  {"re","func:x","#","see help for real()"},
  {"real","func:x","Functions/Misc","real part of x","returns the real part of x. For example, real(3i2) = 3."},
  {"round","func:x","Functions/Math","round x to nearest integer (away from zero for .5 cases)","round x to the nearest integer (for .5 cases, away from zero)"},
  {"sin","func:x","Functions/Trigonometry","sine of x",""},
  {"sinh","func:x","Functions/Trigonometry","hyperbolic sine of x",""},
  {"slpfld","func:expr,xmin,xmax,ymin,ymax","Graphs","draw a slope field of dy/dx = expr","draw a slope field of dy/dx = expr. The xmin, xmax, ymin and ymax arguments are optional. If not specified, the last or default ones will be used."},
  {"sort","func:list,cmp","Functions/Misc","sort list, cmp is optional - see help(\"sort\")","returns a sorted version of list. The cmp argument is an optional name of a custom sorting function which should return -1 if its first argument is smaller than the second, 0 if the two are equal (optional, it can return anything for equal elements), and 1 if the second is larger than the first.\n    Example:\n     * sort([-2,0,1,-1,2]) will return [-2,-1,0,1,2]\n     * Example of using the optional cmp argument to \"reverse sort\" a list:\n       mycmp(a,b) = if(a<b,1,-1)\n       sort([-2,0,1,-1,2],mycmp) will return [2,1,0,-1,-2]"},
  {"sqrt","func:x","Functions/Math","square root of x",""},
  {"stats","func:[v_1,v_2,...,v_n]","Stats","stats info for v_1,...,v_n","show statistics inforomation of v1...vn Each of these values can either be a single number, or a list containing two elements : the number and its frequency\n    Examples:\n     * stats([4,2,2,7]) :\n      n = 4\n      Average = 3.75\n      Q1 = 2\n      Median = 3\n      Q3 = 5.5\n      Min = 2\n      Max = 7\n      sum(x) = 15\n      sum(x^2) = 73\n     * stats([[1,5],[2,4],3]) --  [1,5] means 1 repeats 5 times in the data, [2,4] means 2 repeats 4 times in the data, and the 3 is just present once. Hence, this represents the data : [1,1,1,1,1,2,2,2,2,3]. The result will be:\n      n = 10\n      Average = 1.6\n      Q1 = 1\n      Median = 1.5\n      Q3 = 2\n      Min = 1\n      Max = 3\n      sum(x) = 16\n      sum(x^2) = 30"},
  {"str","func:num","Functions/Misc","string representation of num","return a string representation of expr"},
  {"sum","func:expr,low,high","Functions/Math","\"sigma sum\" with x as the variable","'capital Sigma sum'. expr can contain 'x', which will be incremented from low to high inclusive.\n    Example:\n     * sum(x^2,1,5) == 55"},
  {"tan","func:x","Functions/Trigonometry","tangent of x",""},
  {"tanh","func:x","Functions/Trigonometry","hyperbolic tangent of x",""},
  {"totient","func:x","Functions/Math","Euler's totient of x","Euler's totient function. An interesting application is primality testing, for example. If n is prime, totient(n) = n-1. For example, totient(422753)=422752."},
  {"while","func:condition,body","Programming","evaluates body while condition is true","executes body while condition holds.\n    Example:\n     * for a prime number x, totient(x)==x-1. We can use this property to find the first prime number starting from 1001:\n       n = 1001; while(totient(n)!=n-1, n+=2); n\n       The result would be 1009."},
  {"write","func:filename,element_1,element_2,...,element_n","Functions/Misc","same as print, but writes to file specified with filename","behaves like the print function (run help(\"print\") to learn more), but writes to the file specified with filename."},
  {"zeros","func:n","Functions/Misc","returns an array containing n zeros","generate a list containing n zeros. See also ones()"},
  // CONSTANTS; use raw number or (expr) in parentheses
  {"pi","cnst:(ln(-1)/0i1)","Constants","3.14159...",""},
  {"c","cnst:299792458","Constants","speed of light in vacuum",""},
  {"e","cnst:2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274274663919320030599218174136","Constants","Euler's number",""},
  {"phi","cnst:((1+sqrt(5))/2)","Constants","golden ratio",""},
  {"g","cnst:9.80655","Constants","std gravitational acceleration on Earth",""},
  {"answer","cnst:42","Constants","H2G2","The Answer to Life, the Universe, and Everything. A constant that needs no explanation."},
  {"na","cnst:6.0221415e23","Constants","Avogadro constant",""},
  // OTHERS
  {"help","cmd","#","",""},
  {"credits","cmd","#","",""},
  {"i","sys","#","",""},
  // DIRECTIONS/CONFIG (for tab-complete)
  {"\\p","dir","#","",""},
  {"\\rpn","dir","#","",""},
  {"\\sci","dir","#","",""},
  {"\\eng","dir","#","",""},
  {"\\normal","dir","#","",""},
  {"\\deg","dir","#","",""},
  {"\\rad","dir","#","",""},
  {"\\grad","dir","#","",""},
  {"\\clear","dir","#","",""},
  {"\\load","dir","#","",""},
  {"\\tstep","dir","#","",""},
  {"\\3dpoints","dir","#","",""},
  {"\\bgcolor","dir","#","","Switches graph background color between white and black."},
  // HELP TOPICS
  {"conversions","#","#","","HC is a powerful converter. See the \"Conversions\" menu in the GUI version for a list of all conversions functions"},
  {"complex","#","#","","Complex numbers are inputted and outputted with this syntax:\nreal_part i imaginary_part (without any spaces, they are added here for clarity)\n\nFor example, 5 + 3i would be written as\n5i3\nin HC. This format is inspired by the J programming language.\nAs another example, you can try to type\nsqrt(-1)\nin HC. The result given will be:\n0i1\n\nOther notes\n* Trigonometry functions will refuse to work with complex numbers unless you are in RAD mode (switch to it by typing \\rad).\n* You cannot type a non-number as the real part or imaginary part. Therefore, you cannot write something like 0ipi, if you wanted to have a number pi*i. To express this number you would have to use something like:\n0i1*pi"},
  {"logic","#","#","","This version of HC supports the following logic/boolean operators : < > <= >= == != &[&] (AND) |[|] (OR). For AND and OR, both the C-style && / || and single & / | can be used.\nThese operators return 1 (true) or 0 (false). In general in HC, 0 means false, everything else means true.\n\nThis version of HC supports the following control structures : if(), while() and for(), which behave like ordinary functions -- type help(\"if\"), help(\"while\") and help(\"for\") to get more information about using them. For even more information and examples about logic in HC, visit http://houbysoft.com/hc/logic.php."},
  {"directives","#","#","","You can use the following directives in HC:\n - \\p PLACES - change precision to PLACES decimal places (counted in SCI notation)\n - \\rpn - switch to RPN mode and back\n - \\sci, \\eng, \\normal - switch to scientific, engineering and normal formats\n - \\deg, \\rad, \\grad - switch between DEG, RAD, and GRAD angle modes\n - \\clear - clear all user-defined variables and functions (use clear(name) to clear a single variable/function called name)\n - \\3dpoints SIDE - setup the 3D graphing function to compute a square of side SIDE\n - \\load FILE - load and execute FILE (you can use this for example to define functions you use often, or create larger programs, etc.)"},
  {"operators","#","#","","The following operators are available : + - * / % ^ ! < > <= >= == != & && (boolean and) | || (boolean or). For boolean and/or, both the single-character and two-character variants are accepted."},
  {"lambda","#","#","","HC supports lambda expressions (also called anonymous functions). The syntax is very simple; to create an anonymous function, enclose its expression in single quotes. To use the arguments in your lambda expression, use ~1 or ~ for the first argument, ~2 for the second, ~3 for the third, etc. The lambda expression can be then used as any other function.\n    Examples:\n     * Use map(), range() and a lambda expression to quickly generate the list of pair numbers from 0 to 50:\n       map('~ * 2',range(26))\n     * Example of using a lambda expression as a normal function:\n       '~^2 + ~2 + 1'(2,4) will return 9"}
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

#define hc_postfix_result_cleanup() {m_apm_free(op1_r);m_apm_free(op1_i);m_apm_free(op2_r);m_apm_free(op2_i);free(op1_str);free(op2_str); while (first->n) {m_apm_free(first->re);m_apm_free(first->im);free(first->str);first = first->n;free(first->p);} m_apm_free(first->re);m_apm_free(first->im);free(first->str);free(first);free(tmp_num);}


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
    free(hc_lans_strvec);
    hc_lans_strvec = NULL;
    hc_lans_type = HC_VAR_NUM;

    r = hc_format_num(r, TRUE); // TRUE means to save r as last result
  } else if (is_vector(r)) {
    free(hc_lans_strvec);
    hc_lans_type = HC_VAR_VEC;

    r = hc_format_vec(r);

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
  char *tmp = malloc(strlen(f)+2);
  if (!tmp)
    mem_error();
  strcpy(tmp,f);
  if (hc.rpn)
    return tmp;
  tmp[strlen(tmp)+1]=0;
  tmp[strlen(tmp)]='$';
  char stack[MAX_OP_STACK][2];
  int sp=0;
  unsigned int e_alloc = MAX_EXPR;
  char *e = malloc(e_alloc);
  if (!e)
    mem_error();
  unsigned int i=0,j=0;

  char neg=0;
  char last_was_operator[2]={TRUE,TRUE}; // used to distinguish when ! is being used as factorial and when as NOT

  stack[sp][0] = '$';
  stack[sp++][1] = 0;

  while (tmp[i]!=0)
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
            if (j+1 >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
            e[j++] = stack[--sp][0];
            if (stack[sp][1])
              e[j++] = stack[sp][1];
          }
          if (j >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
          e[j] = 0;
          free(tmp);
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
            free(e); free(tmp);
            return NULL;
          }
          while (stack[sp][0]!='(')
          {
            if (j+1 >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
            e[j++] = stack[sp][0];
            if (stack[sp][1])
              e[j++] = stack[sp][1];
            sp--;
            if (sp < 0)
            {
              hc_error(SYNTAX,"('s and )'s do not match");
              free(e); free(tmp);
              return NULL;
            }
          }
          if (neg)
          {
            if (j + 1 >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
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
            if (j+1 >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
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
              if (j+1 >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
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
              if (j+1 >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
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
              if (j+1 >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
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
            if (j+1 >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
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
            if (j+1 >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
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
            if (j+1 >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
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
            if (j+1 >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
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
            if (j+1 >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
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
            0 : at the beginning of the scan, or after an 'i'
            1 : after a number, allowing for a decimal point
            2 : after a number or a decimal point, not allowing for a decimal point
            3 : after a negative sign
            4 : after an 'e'
           */
          if (base == 10)
          {
            while ((!isspace(tmp[i]))&&(!isoperator(tmp[i]) || (i!=0 && tolower(tmp[i-1])=='e') || tmp[i]=='_'))
            {
              if (((tmpsts == 0) && (!isdigit(tmp[i]) && tmp[i]!='.' && tmp[i]!='_' && tmp[i]!='-')) || ((tmpsts == 1) && (!isdigit(tmp[i]) && tmp[i]!='.' && tolower(tmp[i])!='e' && tolower(tmp[i])!='i')) || ((tmpsts == 2) && (!isdigit(tmp[i]) && tolower(tmp[i])!='e' && tolower(tmp[i])!='i')) || ((tmpsts == 3) && (!isdigit(tmp[i]) && tmp[i]!='.')) || ((tmpsts == 4) && (!isdigit(tmp[i]) && tmp[i]!='-' && tmp[i]!='+')))
              {
                hc_error(SYNTAX,"invalid number encountered");
                free(e); free(tmp);
                return NULL;
              }
              if (tolower(tmp[i]) == 'e')
              {
                tmpsts = 4;
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
                free(e); free(tmp);
                return NULL;
              }
              if (j >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
              e[j++] = tmp[i++];
            }
          } else { // base == 2 or base == 16
            /* Base 2 or Base 16:

               tmpsts contains either TRUE, to allow a decimal point, or FALSE, to allow only digits.
               No more complexity is required as the exponents and complex numbers are not used, available, and for the case of the 'e' in hex, impossible to implement.
            */
            tmpsts = TRUE;
            if (j+1 >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
            e[j++] = tmp[i++]; // write down the '0'
            e[j++] = tmp[i++]; // write down the either 'x' (base 16) or 'b' (base 2)
            while (!isspace(tmp[i]) && (!isoperator(tmp[i]) || tmp[i]=='_'))
            {
              if (!isdigitb(tmp[i],base) && (tmp[i]!='.' || tmpsts == FALSE))
              {
                hc_error(SYNTAX, "invalid binary or hex number encountered");
                free(e); free(tmp);
                return NULL;
              } else {
                if (tmp[i]=='.')
                  tmpsts = FALSE;
                if (j >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
                e[j++] = tmp[i++];
              }
            }
          }
        } else if (tmp[i]=='\"') {
          if (j >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
          e[j++] = tmp[i++];
          while (tmp[i]!='\"' && tmp[i]!='$')
          {
            if (j >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
            e[j++] = tmp[i++];
          }
          if (tmp[i] != '\"')
          {
            hc_error(SYNTAX,"missing end quotes");
            free(e); free(tmp);
            return NULL;
          }
          if (j >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
          e[j++] = tmp[i++];
        } else if (tmp[i]=='[') {
          while (tmp[i]=='[')
          {
            unsigned int pct = 1;
            char ignore = FALSE;
            if (j >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
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
              if (j >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
              e[j++] = tmp[i++];
            }
            if (pct!=0)
            {
              hc_error(SYNTAX, "['s and ]'s do not match");
              free(e); free(tmp);
              return NULL;
            }
          }
        } else if (tmp[i]=='\'') { // lambda expression
          char *endoffunc = strchr_outofblock(tmp + i + 1,'\'');
          if (!endoffunc)
          {
            hc_error(SYNTAX,"single quotes do not match");
            free(e); free(tmp);
            return NULL;
          }
          while (j+endoffunc-tmp-i+1 >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
          memcpy((char *)(e + j),(char *)(tmp + i),endoffunc - tmp - i + 1);
          j += endoffunc - tmp - i + 1;
          i = endoffunc - tmp + 1;
          if (tmp[i]=='(') // called
          {
            if (j >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
            e[j++] = tmp[i];
            int par = 1;
            char ignore = FALSE;
            while (par!=0 && tmp[i]!='$' && tmp[i])
            {
              if (j >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
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
              free(e); free(tmp);
              return NULL;
            } else {
              i++; // skip the last ')'
            }
          }
        } else {
          if (!isalpha(tmp[i]))
          {
            hc_error(SYNTAX, "at character %i : %c", i+1, tmp[i]);
            free(e); free(tmp);
            return NULL;
          }
          while (isalpha(tmp[i]) || isdigit(tmp[i])) // the first to be checked can't be a digit since that would be caught above
          {
            if (j >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
            e[j++] = tmp[i++];
          }
          if (tmp[i]=='(') // function
          {
            if (j >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
            e[j++] = tmp[i];
            int par = 1;
            char ignore = FALSE;
            while (par!=0 && tmp[i]!='$' && tmp[i])
            {
              if (j >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
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
              free(e); free(tmp);
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
          if (j >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
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
            if (j >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
            e[j++] = tmp[i++];
          }
          if (pct!=0)
          {
            hc_error(SYNTAX,"['s and ]'s do not match");
            free(e); free(tmp);
            return NULL;
          }
        }

        if (j >= e_alloc) { e = hc_enlarge_buffer(e, &e_alloc); }
        e[j++] = ' ';
        i--;
      }
    }
    i++;
  }

  free(tmp);
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
  
  char *tmp_num = NULL;
  unsigned int tmp_num_alloc = 0;
  unsigned int i=0,j=0;
  
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
        } else if ((op1_type == HC_VAR_NUM && m_apm_compare(op1_i,MM_Zero)==0 && m_apm_is_integer(op1_r) && m_apm_compare(op1_r,MM_Zero)!=-1 && op2_type == HC_VAR_STR) || (op1_type == HC_VAR_STR && op2_type == HC_VAR_NUM && m_apm_compare(op2_i,MM_Zero)==0 && m_apm_is_integer(op2_r) && m_apm_compare(op1_r,MM_Zero)!=-1))
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
        } else if (op1_type == HC_VAR_VEC && op2_type == HC_VAR_VEC) {
          // matrix multiplication
          curr->type = HC_VAR_VEC;
          free(curr->str); free(curr->n->str);
          curr->n->str = NULL;
          curr->str = list_matrix_mul(op1_str, op2_str);
          if (!curr->str)
          {
            hc_postfix_result_cleanup();
            return NULL;
          }
          free(op1_str); op1_str = NULL;
          free(op2_str); op2_str = NULL;
        } else {
          hc_postfix_result_cleanup();
          hc_error(TYPE,"* accepts either numbers, matrices, an integer and a string, a vector and a number, or an all-integer vector and a string");
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
          hc_error(TYPE,"/ accepts only two numbers or a vector and a number (in that order)");
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
            hc_error(TYPE,"%% accepts only numbers");
            return NULL;
          }
          m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
          curr = curr->p; // [--sp]
          if (curr->type != HC_VAR_NUM)
          {
            hc_postfix_result_cleanup();
            hc_error(TYPE,"%% accepts only numbers");
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
          hc_error(TYPE,"%% accepts only numbers");
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
          hc_error(TYPE,"+ accepts either two numbers, two strings or two vectors");
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
            hc_error(TYPE,"- accepts only numbers or vectors");
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
            hc_error(TYPE,"- accepts only two numbers or two vectors");
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
            hc_error(TYPE,"- accepts only two numbers or two vectors");
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
          hc_error(TYPE,"^ accepts only numbers");
          return NULL;
        }
        m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
        curr = curr->p; // [--sp]
        if (curr->type != HC_VAR_NUM)
        {
          hc_postfix_result_cleanup();
          hc_error(TYPE,"^ accepts only numbers");
          return NULL;
        }
        m_apm_copy(op1_r,curr->re);m_apm_copy(op1_i,curr->im);
        if (m_apm_compare(op2_i,MM_Zero)==0 && m_apm_compare(op2_r,MM_Two)==0)
        {
          m_apmc_sqr(curr->re,curr->im,op1_r,op1_i);
        } else {
          if (m_apm_compare(op2_i,MM_Zero)==0 && m_apm_compare(op1_i,MM_Zero)==0 && (m_apm_compare(op1_r,MM_Zero)!=-1 || m_apm_is_integer(op2_r) == TRUE))
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
          hc_error(TYPE,"! (used as logical NOT) accepts only numbers");
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
            hc_error(TYPE,"! (used as factorial) accepts only numbers");
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
          hc_error(TYPE,"</<= accepts only numbers");
          return NULL;
        }
        // Avoid errors due to calculating internally with higher precision (HC_DEC_PLACES) and then truncating back to hc.precision
        hc_round(op2_r,hc.precision,curr->re);
        hc_round(op2_i,hc.precision,curr->im);
        curr = curr->p; // [--sp]
        if (curr->type != HC_VAR_NUM)
        {
          hc_postfix_result_cleanup();
          hc_error(TYPE,"</<= accepts only numbers");
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
          hc_error(TYPE,">/>= accepts only numbers");
          return NULL;
        }
        // Avoid errors due to calculating internally with higher precision (HC_DEC_PLACES) and then truncating back to hc.precision
        hc_round(op2_r,hc.precision,curr->re);
        hc_round(op2_i,hc.precision,curr->im);
        curr = curr->p; // [--sp]
        if (curr->type != HC_VAR_NUM)
        {
          hc_postfix_result_cleanup();
          hc_error(TYPE,">/>= accepts only numbers");
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
          hc_error(TYPE,"|| accepts only numbers");
          return NULL;
        }
        m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
        curr = curr->p; // [--sp]
        if (curr->type != HC_VAR_NUM)
        {
          hc_postfix_result_cleanup();
          hc_error(TYPE,"|| accepts only numbers");
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
          hc_error(TYPE,"&& accepts only numbers");
          return NULL;
        }
        m_apm_copy(op2_r,curr->re);m_apm_copy(op2_i,curr->im);
        curr = curr->p; // [--sp]
        if (curr->type != HC_VAR_NUM)
        {
          hc_postfix_result_cleanup();
          hc_error(TYPE,"&& accepts only numbers");
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
          tmp_num_alloc = MAX_EXPR;
          tmp_num = malloc(tmp_num_alloc);
          if (!tmp_num) mem_error();
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
            0 : at the beginning of the scan, or after an 'i'
            1 : after a number, allowing for a decimal point
            2 : after a number or a decimal point, not allowing for a decimal point
            3 : after a negative sign
            4 : after an 'e'
           */
          if (base == 10)
          {
            while ((!isspace(e[i]))&&(!isoperator(e[i]) || (i!=0 && tolower(e[i-1])=='e') || e[i]=='_')&& e[i])
            {
              if (hc.rpn) // otherwise, this has already been checked by hc_i2p()
              {
                if (((tmpsts == 0) && (!isdigit(e[i]) && e[i]!='.' && e[i]!='_' && e[i]!='-')) || ((tmpsts == 1) && (!isdigit(e[i]) && e[i]!='.' && tolower(e[i])!='e' && tolower(e[i])!='i')) || ((tmpsts == 2) && (!isdigit(e[i]) && tolower(e[i])!='e' && tolower(e[i])!='i')) || ((tmpsts == 3) && (!isdigit(e[i]) && e[i]!='.')) || ((tmpsts == 4) && (!isdigit(e[i]) && e[i]!='-' && e[i]!='+')))
                {
                  hc_postfix_result_cleanup();
                  return NULL;
                }
                if (tolower(e[i]) == 'e')
                {
                  tmpsts = 4;
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
              if (j >= tmp_num_alloc) { tmp_num = hc_enlarge_buffer(tmp_num, &tmp_num_alloc); }
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
                if (j >= tmp_num_alloc) { tmp_num = hc_enlarge_buffer(tmp_num, &tmp_num_alloc); }
                tmp_num[j++] = e[i++];
              }
            }
            if (j >= tmp_num_alloc) { tmp_num = hc_enlarge_buffer(tmp_num, &tmp_num_alloc); }
            tmp_num[j] = '\0';
            char *tmp_num2;
            if (!(tmp_num2 = hc_2dec(base,tmp_num)))
            {
              hc_postfix_result_cleanup();
              return NULL;
            }
            free(tmp_num);
            tmp_num = tmp_num2;
            j = strlen(tmp_num);
          }
        } else if (e[i]=='\"') {
          type = HC_VAR_STR;
          tmp_num = malloc(MAX_EXPR);
          if (!tmp_num) mem_error();
          tmp_num_alloc = MAX_EXPR;
          tmp_num[j++] = e[i++];
          while (e[i]!='\"' && e[i])
          {
            if (j >= tmp_num_alloc) { tmp_num = hc_enlarge_buffer(tmp_num, &tmp_num_alloc); }
            tmp_num[j++] = e[i++];
          }
          if (e[i] != '\"')
          {
            hc_postfix_result_cleanup();
            hc_error(SYNTAX,"missing end quotes");
            return NULL;
          }
          if (j >= tmp_num_alloc) { tmp_num = hc_enlarge_buffer(tmp_num, &tmp_num_alloc); }
          tmp_num[j++] = e[i++];
          while (e[i]=='[') // index
          {
            if (j >= tmp_num_alloc) { tmp_num = hc_enlarge_buffer(tmp_num, &tmp_num_alloc); }
            tmp_num[j] = 0;
            if (!hc_get_by_index(tmp_num,&type,e,&i))
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
          tmp_num_alloc = MAX_EXPR;
          tmp_num = malloc(tmp_num_alloc);
          if (!tmp_num) mem_error();
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
            if (j >= tmp_num_alloc) { tmp_num = hc_enlarge_buffer(tmp_num, &tmp_num_alloc); }
            tmp_num[j++] = e[i++];
          }
          if (pct!=0)
          {
            hc_postfix_result_cleanup();
            return NULL;
          }
          if (j >= tmp_num_alloc) { tmp_num = hc_enlarge_buffer(tmp_num, &tmp_num_alloc); }
          tmp_num[j]=0;
          char *newlist = list_simplify(tmp_num);
          if (!newlist)
          {
            hc_postfix_result_cleanup();
            return NULL;
          } else {
            free(tmp_num);
            tmp_num = newlist;
            tmp_num_alloc = strlen(tmp_num) + 1;
          }
          j = strlen(tmp_num);
          while (e[i]=='[') // index
          {
            if (j >= tmp_num_alloc) { tmp_num = hc_enlarge_buffer(tmp_num, &tmp_num_alloc); }
            tmp_num[j]=0;
            if (!hc_get_by_index(tmp_num,&type,e,&i))
            {
              hc_postfix_result_cleanup();
              return NULL;
            }
            j = strlen(tmp_num);
          }
        } else if (e[i]=='\'') { // lambda expression
          char *lambda_end = strchr_outofblock((char *)(e + i + 1),'\'');
          if (!lambda_end)
          {
            hc_error(SYNTAX,"single quotes do not match");
            hc_postfix_result_cleanup();
            return NULL;
          }
          if (((char *)(lambda_end + 1))[0] != '(')
          {
            hc_error(SYNTAX,"( expected after lambda expression");
            hc_postfix_result_cleanup();
            return NULL;
          }
          char *lambda_expr = malloc(lambda_end - e - i + 1);
          if (!lambda_expr) mem_error();
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
          char *args = malloc(args_end - e - i + 1); if (!args) mem_error();
          memcpy(args,(char *)(e + i + 1),args_end - e - i - 1);
          args[args_end - e - i - 1] = '\0';
          i = args_end - e + 1;
          if (!(tmp_num = hc_eval_lambda(&type, lambda_expr, args)))
          {
            free(lambda_expr); free(args);
            hc_postfix_result_cleanup();
            return NULL;
          }
          free(lambda_expr); free(args);
          j = strlen(tmp_num);
          tmp_num_alloc = j + 1;
          while (e[i]=='[')
          {
            if (j >= tmp_num_alloc) { tmp_num = hc_enlarge_buffer(tmp_num, &tmp_num_alloc); }
            tmp_num[j]=0;
            if (!hc_get_by_index(tmp_num,&type,e,&i))
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
          char *f_expr = malloc(MAX_EXPR * sizeof(char));
          unsigned int f_expr_alloc = MAX_EXPR;
          if (!v_name || !f_expr)
            mem_error();
          f_expr[0] = '\0';
          unsigned int ti = 0;
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
              if (ti >= f_expr_alloc) {
                f_expr = hc_enlarge_buffer(f_expr, &f_expr_alloc);
              }
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
              if (ti >= f_expr_alloc) {
                f_expr = hc_enlarge_buffer(f_expr, &f_expr_alloc);
              }
              f_expr[ti] = '\0';
              i++; // skip the last ')'
            } else {
              hc_error(SYNTAX,"('s and )'s do not match");
              free(v_name); free(f_expr);
              hc_postfix_result_cleanup();
              return NULL;
            }
          }
          if (!(tmp_num = hc_value(&type, v_name, f_expr)))
          {
            free(v_name); free(f_expr);
            hc_postfix_result_cleanup();
            return NULL;
          }
          free(v_name); free(f_expr);
          j = strlen(tmp_num);
          tmp_num_alloc = j + 1;
          while (e[i]=='[')
          {
            if (j >= tmp_num_alloc) { tmp_num = hc_enlarge_buffer(tmp_num, &tmp_num_alloc); }
            tmp_num[j]=0;
            if (!hc_get_by_index(tmp_num,&type,e,&i))
            {
              hc_postfix_result_cleanup();
              return NULL;
            }
            j = strlen(tmp_num);
          }
          j = strlen(tmp_num);
        }
        i--;
        if (j >= tmp_num_alloc) { tmp_num = hc_enlarge_buffer(tmp_num, &tmp_num_alloc); }
        tmp_num[j]=0; // continue here
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
          free(tmp_num);
        } else if (curr->type == HC_VAR_STR)
        {
          curr->str = tmp_num;
        } else if (curr->type == HC_VAR_VEC)
        {
          curr->str = tmp_num;
        } else if (curr->type == HC_VAR_EMPTY)
        {
          curr->str = tmp_num;
        }
        tmp_num = NULL;
        tmp_num_alloc = 0;
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
  free(tmp_num);

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
  char *e = f;
  if (!e)
    mem_error();

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
    if (atoi(&d[i+1]) <= 0)
    {
      error_nq("Precision needs to be a positive number.");
      break;
    }
    hc.precision = atoi(&d[i+1]);
    printf("Precision is now set to %i decimal places.\n",hc.precision);
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
  case HASH_BGCOLOR:
    hc.bgcolor = hc.bgcolor==WHITE ? BLACK : WHITE;
    if (hc.bgcolor == WHITE)
      notify("Graph background is now set to white.\n");
    else
      notify("Graph background is now set to black.\n");
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
      if (isoperator(e[i]) || e[i] == '[' || e[i] == ']')
        could_be_num = 1;
    }
  }

  return e;
}


void hc_load_cfg()
{
  FILE *fr = fopen(HC_CFG_FILE,"r");
  hc.angle = 'r';
  hc.autoupdate = TRUE;
  hc.graph_points_3d = HC_GP3D_DEFAULT;
  hc.peqstep = HC_PEQSTEP_DEFAULT;
  hc.xmin2d[hcgt_get_idx(HCGT_2D)] = hc.ymin2d[hcgt_get_idx(HCGT_2D)] = hc.xmin3d = hc.ymin3d = hc.zmin3d = hc.xmin2d[hcgt_get_idx(HCGT_SLPFLD)] = hc.ymin2d[hcgt_get_idx(HCGT_SLPFLD)] = hc.xmin2d[hcgt_get_idx(HCGT_PARAMETRIC)] = hc.ymin2d[hcgt_get_idx(HCGT_PARAMETRIC)] = hc.xmin2d[hcgt_get_idx(HCGT_POLAR)] = hc.ymin2d[hcgt_get_idx(HCGT_POLAR)] = -10;
  hc.xmax2d[hcgt_get_idx(HCGT_2D)] = hc.ymax2d[hcgt_get_idx(HCGT_2D)] = hc.xmax3d = hc.ymax3d = hc.zmax3d = hc.xmax2d[hcgt_get_idx(HCGT_SLPFLD)] = hc.ymax2d[hcgt_get_idx(HCGT_SLPFLD)] = hc.xmax2d[hcgt_get_idx(HCGT_PARAMETRIC)] = hc.ymax2d[hcgt_get_idx(HCGT_PARAMETRIC)] = hc.xmax2d[hcgt_get_idx(HCGT_POLAR)] = hc.ymax2d[hcgt_get_idx(HCGT_POLAR)] = 10;
  hc.tminpolar = hc.tminpeq = 0;
  hc.tmaxpolar = hc.tmaxpeq = 2 * 3.1415926535897932384626433832795028841971693993751058209749445923;
  hc.bgcolor = BLACK;
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
      case HASH_BGCOLOR:
        if (buffer[i+1]==WHITE)
          hc.bgcolor = WHITE;
        else
          hc.bgcolor = BLACK;
      case HASH_AUTOUPDATE:
        if (buffer[i+1]=='1')
              hc.autoupdate = TRUE;
            else
              hc.autoupdate = FALSE;
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
        hc.xmin2d[hcgt_get_idx(HCGT_2D)] = strtod(&buffer[i+1],NULL);
        break;
      case HASH_XMAX2D:
        hc.xmax2d[hcgt_get_idx(HCGT_2D)] = strtod(&buffer[i+1],NULL);
        break;
      case HASH_YMIN2D:
        hc.ymin2d[hcgt_get_idx(HCGT_2D)] = strtod(&buffer[i+1],NULL);
        break;
      case HASH_YMAX2D:
        hc.ymax2d[hcgt_get_idx(HCGT_2D)] = strtod(&buffer[i+1],NULL);
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
        hc.xmin2d[hcgt_get_idx(HCGT_SLPFLD)] = strtod(&buffer[i+1],NULL);
        break;
      case HASH_XMAXSF:
        hc.xmax2d[hcgt_get_idx(HCGT_SLPFLD)] = strtod(&buffer[i+1],NULL);
        break;
      case HASH_YMINSF:
        hc.ymin2d[hcgt_get_idx(HCGT_SLPFLD)] = strtod(&buffer[i+1],NULL);
        break;
      case HASH_YMAXSF:
        hc.ymax2d[hcgt_get_idx(HCGT_SLPFLD)] = strtod(&buffer[i+1],NULL);
        break;
      case HASH_TMINPEQ:
        hc.tminpeq = strtod(&buffer[i+1],NULL);
        break;
      case HASH_TMAXPEQ:
        hc.tmaxpeq = strtod(&buffer[i+1],NULL);
        break;
      case HASH_TMINPOLAR:
        hc.tminpolar = strtod(&buffer[i+1],NULL);
        break;
      case HASH_TMAXPOLAR:
        hc.tmaxpolar = strtod(&buffer[i+1],NULL);
        break;
      case HASH_XMINPEQ:
        hc.xmin2d[hcgt_get_idx(HCGT_PARAMETRIC)] = strtod(&buffer[i+1],NULL);
        break;
      case HASH_XMAXPEQ:
        hc.xmax2d[hcgt_get_idx(HCGT_PARAMETRIC)] = strtod(&buffer[i+1],NULL);
        break;
      case HASH_YMINPEQ:
        hc.ymin2d[hcgt_get_idx(HCGT_PARAMETRIC)] = strtod(&buffer[i+1],NULL);
        break;
      case HASH_YMAXPEQ:
        hc.ymax2d[hcgt_get_idx(HCGT_PARAMETRIC)] = strtod(&buffer[i+1],NULL);
        break;
      case HASH_XMINPO:
        hc.xmin2d[hcgt_get_idx(HCGT_POLAR)] = strtod(&buffer[i+1],NULL);
        break;
      case HASH_XMAXPO:
        hc.xmax2d[hcgt_get_idx(HCGT_POLAR)] = strtod(&buffer[i+1],NULL);
        break;
      case HASH_YMINPO:
        hc.ymin2d[hcgt_get_idx(HCGT_POLAR)] = strtod(&buffer[i+1],NULL);
        break;
      case HASH_YMAXPO:
        hc.ymax2d[hcgt_get_idx(HCGT_POLAR)] = strtod(&buffer[i+1],NULL);
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
  fprintf(fw,"autoupdate=%i\n",hc.autoupdate==0 ? FALSE : TRUE);
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
  fprintf(fw,"xmin2d=%f\n",hc.xmin2d[hcgt_get_idx(HCGT_2D)]);
  fprintf(fw,"xmax2d=%f\n",hc.xmax2d[hcgt_get_idx(HCGT_2D)]);
  fprintf(fw,"ymin2d=%f\n",hc.ymin2d[hcgt_get_idx(HCGT_2D)]);
  fprintf(fw,"ymax2d=%f\n",hc.ymax2d[hcgt_get_idx(HCGT_2D)]);
  fprintf(fw,"xmin3d=%f\n",hc.xmin3d);
  fprintf(fw,"xmax3d=%f\n",hc.xmax3d);
  fprintf(fw,"ymin3d=%f\n",hc.ymin3d);
  fprintf(fw,"ymax3d=%f\n",hc.ymax3d);
  fprintf(fw,"zmin3d=%f\n",hc.zmin3d);
  fprintf(fw,"zmax3d=%f\n",hc.zmax3d);
  fprintf(fw,"xminsf=%f\n",hc.xmin2d[hcgt_get_idx(HCGT_SLPFLD)]);
  fprintf(fw,"xmaxsf=%f\n",hc.xmax2d[hcgt_get_idx(HCGT_SLPFLD)]);
  fprintf(fw,"yminsf=%f\n",hc.ymin2d[hcgt_get_idx(HCGT_SLPFLD)]);
  fprintf(fw,"ymaxsf=%f\n",hc.ymax2d[hcgt_get_idx(HCGT_SLPFLD)]);
  fprintf(fw,"tminpeq=%f\n",hc.tminpeq);
  fprintf(fw,"tmaxpeq=%f\n",hc.tmaxpeq);
  fprintf(fw,"tminpolar=%f\n",hc.tminpolar);
  fprintf(fw,"tmaxpolar=%f\n",hc.tmaxpolar);
  fprintf(fw,"xminpeq=%f\n",hc.xmin2d[hcgt_get_idx(HCGT_PARAMETRIC)]);
  fprintf(fw,"xmaxpeq=%f\n",hc.xmax2d[hcgt_get_idx(HCGT_PARAMETRIC)]);
  fprintf(fw,"yminpeq=%f\n",hc.ymin2d[hcgt_get_idx(HCGT_PARAMETRIC)]);
  fprintf(fw,"ymaxpeq=%f\n",hc.ymax2d[hcgt_get_idx(HCGT_PARAMETRIC)]);
  fprintf(fw,"xminpo=%f\n",hc.xmin2d[hcgt_get_idx(HCGT_POLAR)]);
  fprintf(fw,"xmaxpo=%f\n",hc.xmax2d[hcgt_get_idx(HCGT_POLAR)]);
  fprintf(fw,"yminpo=%f\n",hc.ymin2d[hcgt_get_idx(HCGT_POLAR)]);
  fprintf(fw,"ymaxpo=%f\n",hc.ymax2d[hcgt_get_idx(HCGT_POLAR)]);
  fprintf(fw,"bgcolor=%c\n",hc.bgcolor);
  fclose(fw);
}


void hc_cleanup()
{
  hc_output(PRINT,""); // make it close file descriptors (if any)
  is_zero_free(); // free is_zero()'s M_APM if allocated
  free(gamma_coefficients);
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
#if !defined(WIN32) && !defined(NO_WORDEXP)
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
  unsigned int expr_alloc = MAX_EXPR + 1;
  char *expr = malloc(expr_alloc);
  if (!expr)
    mem_error();
  unsigned int line = 1;
  char end_of_line = FALSE;
  while (fgets(expr,expr_alloc,fr))
  {
    end_of_line = FALSE;
    if (strcmp(expr,"\n")==0)
      continue;
    if (expr[strlen(expr)-1]=='\n')
    {
      expr[strlen(expr)-1]=0;
      end_of_line = TRUE;
    }
    while (!end_of_line || !check_completeness(expr))
    {
      end_of_line = FALSE;
      while (expr_alloc - strlen(expr) < MAX_EXPR)
      {
        expr_alloc += MAX_EXPR;
        expr = realloc(expr, expr_alloc);
      }
      if (!fgets(expr+strlen(expr),expr_alloc - strlen(expr),fr))
        break;
      if (expr[strlen(expr)-1]=='\n')
      {
        expr[strlen(expr)-1]=0;
        end_of_line = TRUE;
      }
      if (end_of_line)
        line++;
      if (feof(fr))
        end_of_line = TRUE;
    }
    char *fme = hc_result(expr);
    if (!fme)
    {
      hc_error(LOAD,"occured at line %i (%s), interrupting execution.",line,expr);
      break;
    }
    free(fme);
    line++;
  }
  fclose(fr);
  free(expr);
}
