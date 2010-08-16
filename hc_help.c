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


#include <stdlib.h>
#include "hc.h"
#include "hc_functions.h"
#include "hash.h"


inline char *allocline(char *line)
{
  char *r = malloc(strlen(line)+2);
  if (!r)
    mem_error();
  sprintf(r,"%s\n",line);
  return r;
}


int hc_help(char *e)
{
  e = hc_result_(e);
  if (!e || !is_string(e))
  {
    free(e);
    arg_error("help() : a string argument containing (ONLY, without parentheses etc.) the name of the function/variable you want to learn about is required.");
  }
  char *search = get_string(e);
  free(e);
  unsigned int hash = simple_hash(search);
  char *print = NULL;

  switch (hash)
  {
  case 0:
    print = allocline("Welcome to HC! To evaluate an expression, simply type it and press enter. To get a list of all operators you can use, type help(\"operators\"). To get a list of functions you can use, press TAB twice, and to get more details about a function, call help(\"nameoffunction\"). To get more information about conversion functions, type help(\"conversions\"). To get more information about logic and programming, type help(\"logic\"). For information about complex numbers in HC, type help(\"complex\"). There are also a few configuration directives you can use to switch between modes etc.; to learn more, type help(\"directives\").\n\nTo get credits information, simply type credits. Thanks for using HC, and please do not hesitate to suggest new features or report bugs at bugs.houbysoft.com.");
	break;
/**** FUNCTIONS ****/

  case HASH_ANS:
    print = allocline("- ans() - returns the result of the last expression");
    break;

  case HASH_ABS:
    print = allocline("- abs(x) - absolute value of x");
    break;

  case HASH_ACOS:
  case HASH_ARCCOS:
    print = allocline("- acos(x) - arc cosine of x");
    break;

  case HASH_ASIN:
  case HASH_ARCSIN:
    print = allocline("- asin(x) - arc sine of x");
    break;

  case HASH_ATAN:
  case HASH_ARCTAN:
    print = allocline("- atan(x) - arc tangent of x");
    break;

  case HASH_BOXPLOT:
    print = allocline("- boxplot(v1,v2,...,vn) - draw a boxplot of values v1...vn");
    break;

  case HASH_BOXPLOTF:
    print = allocline("- boxplotf(v1,freq1,v2,freq2,...,...,vn,freqn) - same as boxplot, but after each value, you have to append its frequency");
    break;

  case HASH_CBRT:
    print = allocline("- cbrt(x) - returns the cube root of x (you can also use x^(1/3))");
    break;

  case HASH_CEIL:
    print = allocline("- ceil(x) - returns x rounded to the nearest integer upwards");
    break;

  case HASH_CHARTOCODE:
    print = allocline("- chartocode(x) - converts x, a single-character string, to its ASCII code");
    break;

  case HASH_CLEAR:
	print = allocline("- clear(x) - clears (undefines) x. You can also use \\clear to clear (undefine) all user-defined variables and functions.");
	break;

  case HASH_CMTOINCH:
    print = allocline("- cmtoinch(x) - converts x, which has to be in cm, to inches");
    break;

  case HASH_CODETOCHAR:
    print = allocline("- codetochar(x) - converts x, an ASCII code, to the corresponding character");
    break;

  case HASH_COS:
    print = allocline("- cos(x) - cosine of x");
    break;

  case HASH_COSH:
    print = allocline("- cosh(x) - hyperbolic cosine of x");
    break;

  case HASH_CROSSP:
    print = allocline("- crossp(v1,v2) - return the vector cross product of v1 X v2");
    break;

  case HASH_CTOF:
    print = allocline("- ctof(x) - converts x, which has to be in degrees Celsius, to Fahrenheit");
    break;

  case HASH_CTOK:
    print = allocline("- ctok(x) - converts x, which has to be in degrees Celsius, to Kelvin");
    break;

  case HASH_DOTP:
    print = allocline("- dotp(v1,v2) - return the vector dot product of v1 . v2");
    break;

  case HASH_EXP:
    print = allocline("- exp(x) - exponential function");
    break;

  case HASH_FACTORIAL:
    print = allocline("- factorial(x) - factorial of x (you could also use the '!' notation");
    break;

  case HASH_FIBO:
    print = allocline("- fibo(x) - return the xth term of the Fibonacci sequence");
    break;

  case HASH_FLOOR:
    print = allocline("- floor(x) - returns x rounded to the nearest integer downwards");
    break;

  case HASH_FLOZTOML:
    print = allocline("- floztoml(x) - converts x, which has to be in US fluid ounces, to ml");
    break;

  case HASH_FOR:
    print = allocline("- for(init, condition, iter, body) - executes init, and then executes body while condition holds, executing iter at the end of each iteration.\n    Example:\n     * this example shows how to print numbers from 1 to 5 with for:\n       for(j=1, j<=5, j+=1, print(j)) would print out:\n1\n2\n3\n4\n5");
    break;

  case HASH_FTOC:
    print = allocline("- ftoc(x) - converts x, which has to be in Fahrenheit, to degrees Celsius");
    break;

  case HASH_FTOK:
    print = allocline("- ftok(x) - converts x, which has to be in Fahrenheit, to Kelvin");
    break;

  case HASH_FTTOM:
    print = allocline("- fttom(x) - converts x, which has to be in feet, to meters");
    break;

  case HASH_GCD:
    print = allocline("- gcd(x,y) - finds the greatest common divisor of x and y");
    break;

  case HASH_GMUL:
    print = allocline("- [DEPRECATED, use graph([expr_1, expr_2, ..., expr_n]) instead] gmul(expr_1, expr_2, ..., expr_n) - similar to graph, but can draw an arbitrary number of functions on one graph (note : to set xmin, etc., make a call to graph() first)");
    break;

  case HASH_GRAPH:
    print = allocline("- graph(expr,xmin,xmax,ymin,ymax) - draw a graph (generate a PNG file if in command-line), example syntax:\n    graph(x^2,-10,10,-100,100)\n    Note:\n     * the xmin, xmax, ymin and ymax arguments are optional; if not provided, the last will be used (ie after a call of the example, graph(something) would use -10, 10, -100 and 100\n     * to draw multiple functions on one graph, pass them as a list for expr. For example, if you want to draw x, x^2, and x^3 on the same graph, you can use something like:\n       graph([x,x^2,x^3],-10,10,-100,100)");
    break;

  case HASH_GRAPH3:
    print = allocline("- graph3(expr,xmin,xmax,ymin,ymax,zmin,zmax) - draw a 3D graph (generate a PNG file if in command-line), example syntax:\n    graph3(x+y,-10,10,-10,10,-20,20)\n    Note:\n     * as with graph(), the xmin, xmax, ymin, ymax, zmin and zmax are optional");
    break;

  case HASH_GRAPHPEQ:
    print = allocline("- graphpeq(expr_x,expr_y,tmin,tmax,xmin,xmax,ymin,ymax) - draw the parametric equation modeled by expr_x and expr_y (use t as the variable)\n    Note:\n     * the tmin, tmax, xmin, xmax, ymin and ymax arguments are optional");
    break;

  case HASH_IF:
    print = allocline("- if(condition, truebranch, falsebranch) - evaluates condition, and if it is true, returns truebranch, otherwise returns falsebranch.\n    Note:\n     * falsebranch is optional\n\n    Example:\n     * if(1==2,\"hello\",\"hi\") would return \"hi\", since 1 is not equal to 2. If the falsebranch would have been ommited, in this case nothing would be returned:\n       if(1==2,\"hello\") returns nothing.");
    break;

  case HASH_IM:
  case HASH_IMAG:
    print = allocline("- im(x)/imag(x) - return the imaginary part of x");
    break;

  case HASH_INCHTOCM:
    print = allocline("- inchtocm(x) - converts x, which has to be in inches, to cm");
    break;

  case HASH_INPUT:
    print = allocline("- input(str) - shows an inut dialog using either a standard prompt or str, if specified");
    break;

  case HASH_JOIN:
    print = allocline("- join(a,b) - join a and b, two vectors, together");
    break;

  case HASH_KMTOMI:
    print = allocline("- kmtomi(x) - converts x, which has to be in km, to miles");
    break;

  case HASH_KTOC:
    print = allocline("- ktoc(x) - converts x, which has to be in Kelvin, to degrees Celsius");
    break;

  case HASH_KTOF:
    print = allocline("- ktof(x) - converts x, which has to be in Kelvin, to Fahrenheit");
    break;

  case HASH_LCM:
    print = allocline("- lcm(x,y) - finds the least common multiple of x and y");
    break;

  case HASH_LENGTH:
    print = allocline("- length(x) - return the length of x");
    break;

  case HASH_LN:
    print = allocline("- ln(x) - return the natural logarithm of x");
    break;

  case HASH_LOG10:
    print = allocline("- log10(x) - return the common logarithm of x");
    break;

  case HASH_MITOKM:
    print = allocline("- mitokm(x) - converts x, which has to be in miles, to km");
    break;

  case HASH_MLTOFLOZ:
    print = allocline("- mltofloz(x) - converts x, which has to be in ml, to US fluid ounces");
    break;

  case HASH_MMASS:
    print = allocline("- mmass(molecule) - returns the molar mass of the molecule specified (use standard notation without using subscripts).\n    Examples:\n      * H20\n      * Ca(OH)2");
    break;

  case HASH_MOD:
    print = allocline("- mod(x,y) - modulus (you could also use the C-style '%' notation");
    break;

  case HASH_MTOFT:
    print = allocline("- mtoft(x) - converts x, which has to be in meters, to feet");
    break;

  case HASH_NCR:
    print = allocline("- nCr(n,k) - binomial coefficient (combinatorics)");
    break;

  case HASH_NPR:
    print = allocline("- nPr(n,k) - number of permutations (combinatorics)");
    break;

  case HASH_NUM:
    print = allocline("- num(str) - returns a number representation of the number in string str");
    break;

  case HASH_PRINT:
    print = allocline("- print(expr_1, expr_2, ..., expr_n) - prints its number or string arguments (if more than one argument is passed, the outputs will be separated by a space) and a newline character (useful for printing variables in loops)");
    break;

  case HASH_PRODUCT:
    print = allocline("- product(expr,low,high) - 'capital Pi product'. expr can contain 'x', which will be incremented from low to high inclusive.\n    Example:\n     * product(x^2,1,5) == 14400");
    break;

  case HASH_RAND:
    print = allocline("- rand(int) - if int is specified, return a random integer in the range [1 ; int], otherwise return a real number in the range [0;1[");
    break;

  case HASH_RE:
  case HASH_REAL:
    print = allocline("- re(x)/real(x) - return the real part of x");
    break;

  case HASH_ROUND:
    print = allocline("- round(x) - round x to the nearest integer (for .5 cases, away from zero)");
    break;

  case HASH_SIN:
    print = allocline("- sin(x) - sine of x");
    break;

  case HASH_SINH:
    print = allocline("- sinh(x) - hyperbolic sine of x");
    break;

  case HASH_SLPFLD:
    print = allocline("- slpfld(expr,xmin,xmax,ymin,ymax) - draw a slope field of dy/dx = expr. The xmin, xmax, ymin and ymax arguments are optional. If not specified, the last or default ones will be used.");
    break;

  case HASH_SQRT:
    print = allocline("- sqrt(x) - returns the square root of x");
    break;

  case HASH_STATS:
    print = allocline("- stats(v1,v2,...,vn) - show statistics inforomation of v1...vn\n    Example:\n     * stats(4,2,2,7) :\n      n = 4\n      Average = 3.75\n      Q1 = 2\n      Median = 3\n      Q3 = 5.5\n      Min = 2\n      Max = 7\n      sum(x) = 15\n      sum(x^2) = 73");
    break;

  case HASH_STATSF:
    print = allocline("- statsf(v1,freq1,v2,freq2,...,...,vn,freqn) - same as stats (run help(\"stats\") to learn more), but after each value, you have to append its frequency");
    break;

  case HASH_STR:
    print = allocline("- str(expr) - return a string representation of expr");
    break;

  case HASH_SUM:
    print = allocline("- sum(expr,low,high) - 'capital Sigma sum'. expr can contain 'x', which will be incremented from low to high inclusive.\n    Example:\n     * sum(x^2,1,5) == 55");
    break;

  case HASH_TAN:
    print = allocline("- tan(x) - tangent of x");
    break;

  case HASH_TANH:
    print = allocline("- tanh(x) - hyperbolic tangent of x");
    break;

  case HASH_TOTIENT:
    print = allocline("- totient(x) - Euler's totient function");
    break;

  case HASH_WHILE:
    print = allocline("- while(condition, body) - executes body while condition holds.\n    Example:\n     * for a prime number x, totient(x)==x-1. We can use this property to find the first prime number starting from 1001:\n       n = 1001; while(totient(n)!=n-1, n+=2); n\n       The result would be 1009.");
    break;

  case HASH_WRITE:
    print = allocline("- write(fname, expr_1, expr_2, ..., expr_n) - behaves like the print function (run help(\"print\") to learn more), but writes to the file specified with fname.");
    break;

/**** CONSTANTS ****/

  case HASH_ANSWER:
    print = allocline("- answer - The Answer. A constant that needs no explanation.");
    break;

  case HASH_C:
    print = allocline("- c - the speed of light in vacuum");
    break;

  case HASH_E:
    print = allocline("- e - Euler's number");
    break;

  case HASH_G:
    print = allocline("- g - standard gravitational acceleration on Earth");
    break;

  case HASH_PHI:
    print = allocline("- phi - the Golden Ratio");
    break;

  case HASH_PI:
    print = allocline("- pi - 3.141592...");
    break;

/**** MISC HELP TOPICS ****/

  case HASH_CONVERSIONS:
    print = allocline("List of conversion functions (call help(\"nameoffunction\") to get more details about any of them):\n - chartocode(x)\n - cmtoinch(x)\n - codetochar(x)\n - ctof(x)\n - ctok(x)\n - floztoml(x)\n - ftoc(x)\n - ftok(x)\n - fttom(x)\n - inchtocm(x)\n - kmtomi(x)\n - ktoc(x)\n - ktof(x)\n - mitokm(x)\n - mltofloz(x)\n - mtoft(x)");
    break;

  case HASH_COMPLEX:
    print = allocline("Complex numbers are inputted and outputted with this syntax:\nreal_part i imaginary_part (without any spaces, they are added here for clarity)\n\nFor example, 5 + 3i would be written as\n5i3\nin HC. This format is inspired by the J programming language.\nAs another example, you can try to type\nsqrt(-1)\nin HC. The result given will be:\n0i1\n\nOther notes\n* Trigonometry functions will refuse to work with complex numbers unless you are in RAD mode (switch to it by typing \\rad).\n* You cannot type a non-number as the real part or imaginary part. Therefore, you cannot write something like 0ipi, if you wanted to have a number pi*i. To express this number you would have to use something like:\n0i1*pi");
    break;

  case HASH_LOGIC:
    print = allocline("This version of HC supports the following logic/boolean operators : < > <= >= == != &[&] (AND) |[|] (OR). For AND and OR, both the C-style && / || and single & / | can be used.\nThese operators return 1 (true) or 0 (false). In general in HC, 0 means false, everything else means true.\n\nThis version of HC supports the following control structures : if(), while() and for(), which behave like ordinary functions -- type help(\"if\"), help(\"while\") and help(\"for\") to get more information about using them. For even more information and examples about logic in HC, visit http://houbysoft.com/hc/logic.php.");
    break;

  case HASH_DIRECTIVES:
    print = allocline("You can use the following directives in HC:\n - \\p PLACES - change precision to PLACES decimal places (counted in SCI notation)\n - \\rpn - switch to RPN mode and back\n - \\sci, \\eng, \\normal - switch to scientific, engineering and normal formats\n - \\deg, \\rad, \\grad - switch between DEG, RAD, and GRAD angle modes\n - \\clear - clear all user-defined variables and functions (use clear(name) to clear a single variable/function called name)\n - \\3dpoints SIDE - setup the 3D graphing function to compute a square of side SIDE\n - \\load FILE - load and execute FILE (you can use this for example to define functions you use often, or create larger programs, etc.)");
    break;

  case HASH_OPERATORS:
    print = allocline("The following operators are available : + - * / % ^ ! < > <= >= == != & && (boolean and) | || (boolean or). For boolean and/or, both the single-character and two-character variants are accepted.");
    break;

/**** END ****/

  default:
    print = malloc(strlen("Sorry, no help was found for \"\". Try hitting TAB twice to get a list of functions.\n")+strlen(search)+1);
    if (!print)
      mem_error();
    sprintf(print,"Sorry, no help was found for \"%s\". Try hitting TAB twice to get a list of functions.\n",search);
    break;
  }

  notify(print);
  free(print);
  free(search);
  return SUCCESS;
}
