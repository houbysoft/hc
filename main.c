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
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#ifndef I_HATE_TIPS
#include <time.h>
#endif
#include <string.h>
#include "hc.h"


#ifndef I_HATE_TIPS
char *tips[] = {
  "You can use \"pi\" to get the value of pi.",
  "To enter a negative number, simply prepend it with a '-' (same as the minus operator).",
  "We may be in the matrix right now.",
  "If you have an idea or wish for a function in HC, you can contact me at http://houbysoft.com/en/about.php.",
  "If you don't like these tips, you can recompile HC with the -DI_HATE_TIPS flag.",
  "To get the factorial of n, type factorial(n) or use the '!' notation.",
  "To recall the last result, you can use ans().",
  "You can use \\rpn to switch between RPN-mode and infix-mode.",
  "You can use \\deg, \\rad, and \\grad to switch the angle mode.",
  "HC is also a good converter. For example, to convert 1 US Fl Oz to mL, type floztoml(1). Type conversions to get a list of other built-in conversions!",
  "You can type a constant's name with english characters to use it in a calculation.",
  "This calculator can provide you The Answer.",
  "From version 0.4.1, you can use the '!' notation for factorial and the '%' notation for modulus."
};
#define TIP_COUNT 13
#endif


#ifdef WIN32
void exit_fn() {printf("Press Enter to close this window.");getchar();}
#endif


int main(int argc, char *argv[])
{
#ifdef WIN32
  atexit(exit_fn);
#endif
  char *fme;

#ifndef WIN32
  using_history();
#endif

  printf("%s(type help for help, and exit to exit)\n",NAME_VERSION);

#ifndef I_HATE_TIPS
  srand(time(NULL));
  printf("Did you know? %s\n",tips[rand()%TIP_COUNT]);
#endif

  hc_load_cfg();

#ifndef WIN32
  char *expr;
#else
  char *expr = malloc(sizeof(char) * MAX_EXPR + sizeof(char));
  memset(expr,0,MAX_EXPR+1);
#endif

  while (42)
  {
#ifndef WIN32
    expr = readline("> ");
#else
    printf("> ");
    fgets(expr,MAX_EXPR+1,stdin);
#endif
    if (strlen(expr)>=MAX_EXPR)
      overflow_error();
#ifdef WIN32
    expr[strlen(expr)-1]=0;
#endif
    if (strcmp(expr,"exit")==0)
      exit(0);
    fme = hc_result(expr);
    if (fme)
    {
      printf(" %s\n",fme);
      free(fme);
    }
#ifndef WIN32
    add_history(expr);
    free(expr);
#endif
  }

  return 0;
}
