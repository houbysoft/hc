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

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifdef WIN32
#include <hul.h>
#endif
#include "hc.h"
#undef notify

#define gtk_box_pack_start_defaults(x,y) gtk_box_pack_start(x,y,FALSE,FALSE,0)


GtkWidget *window;
GtkWidget *main_box;
GtkWidget *main_menu_bar;

// Scrollback
GtkWidget *scroll_window;
GtkWidget *scroll_text_view;
GtkTextBuffer *scroll_buf;

// Expression entry
GtkWidget *expr_entry;
GtkEntryCompletion *expr_com;
GtkListStore *expr_com_ls;
GtkTreeIter expr_com_iter;

// "Clickable" keys
GtkWidget *keys_table;

void hcg_quit();
void hcg_about();
void hcg_update();
void hcg_help();
void hcg_load();
void hcg_help_cplx();
void hcg_help_logic();
void hcg_process_expr(GtkWidget *widget, gpointer trash);
void scrollprint(char *format, ...);
void hcg_cfg_change_precision(GtkWidget *widget, gpointer trash);
void hcg_cfg_change_rpn(GtkWidget *widget, gpointer trash);
void hcg_cfg_change_angle(GtkWidget *widget, gpointer trash);
void hcg_cfg_change_exp(GtkWidget *widget, gpointer trash);
void hcg_cfg_change_keys(GtkWidget *widget, gpointer trash);
void hcg_C(GtkWidget *widget, gpointer trash);
gboolean hcg_match_select(GtkEntryCompletion *widget, GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data);
void entry_append(GtkWidget *widget, char *what);
void hcg_help_usrdf(GtkWidget *widget, gpointer trash);
void hcg_help_graph(GtkWidget *widget, gpointer trash);
void hcg_clear_usrdf(GtkWidget *widget, gpointer trash);
void hcg_disp_graph(char *fname);


int main(int argc, char *argv[])
{
  hc_load_cfg();

  gtk_init(&argc,&argv);
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_resizable(GTK_WINDOW (window), TRUE);
  g_signal_connect(G_OBJECT (window), "delete-event", G_CALLBACK (hcg_quit), NULL);
  g_signal_connect(G_OBJECT (window), "destroy", G_CALLBACK (hcg_quit), NULL);
  gtk_window_set_title(GTK_WINDOW (window), "HC");

  main_box = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER (window), main_box);

  main_menu_bar = gtk_menu_bar_new();
  GtkWidget *mit_file = gtk_menu_item_new_with_mnemonic("_File");
  GtkWidget *mit_fns = gtk_menu_item_new_with_mnemonic("F_unctions");
  GtkWidget *mit_conv = gtk_menu_item_new_with_mnemonic("C_onversions");
  GtkWidget *mit_cnst = gtk_menu_item_new_with_mnemonic("_Constants");
  GtkWidget *mit_graph = gtk_menu_item_new_with_mnemonic("_Graphs");
  GtkWidget *mit_stats = gtk_menu_item_new_with_mnemonic("_Stats");
  GtkWidget *mit_usrdf = gtk_menu_item_new_with_mnemonic("User-d_efined vars/funcs");
  GtkWidget *mit_help = gtk_menu_item_new_with_mnemonic("_Help");
  gtk_menu_bar_append(main_menu_bar, mit_file);
  gtk_menu_bar_append(main_menu_bar, mit_fns);
  gtk_menu_bar_append(main_menu_bar, mit_conv);
  gtk_menu_bar_append(main_menu_bar, mit_cnst);
  gtk_menu_bar_append(main_menu_bar, mit_graph);
  gtk_menu_bar_append(main_menu_bar, mit_stats);
  gtk_menu_bar_append(main_menu_bar, mit_usrdf);
  gtk_menu_bar_append(main_menu_bar, mit_help);
  GtkWidget *subm_file = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (mit_file), subm_file);
  GtkWidget *subm_fns = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (mit_fns), subm_fns);
  GtkWidget *subm_conv = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (mit_conv), subm_conv);
  GtkWidget *subm_cnst = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (mit_cnst), subm_cnst);
  GtkWidget *subm_graph = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (mit_graph), subm_graph);
  GtkWidget *subm_stats = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (mit_stats), subm_stats);
  GtkWidget *subm_usrdf = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (mit_usrdf), subm_usrdf);
  GtkWidget *subm_help = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (mit_help), subm_help);

  // File submenu
  GtkWidget *subm_mit_load = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, NULL);
  gtk_menu_item_set_label(GTK_MENU_ITEM (subm_mit_load), "Load");
  gtk_menu_append(GTK_MENU (subm_file), subm_mit_load);
  g_signal_connect(G_OBJECT (subm_mit_load), "activate", G_CALLBACK (hcg_load), NULL);
  GtkWidget *subm_mit_quit = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
  gtk_menu_append(GTK_MENU (subm_file), subm_mit_quit);
  g_signal_connect(G_OBJECT (subm_mit_quit), "activate", G_CALLBACK (hcg_quit), NULL);

  // Functions submenu
  // Functions/Combinatorics submenu
  GtkWidget *subm_combinatorics = gtk_menu_new();
  GtkWidget *subm_mit_combinatorics = gtk_menu_item_new_with_label("Combinatorics");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (subm_mit_combinatorics), subm_combinatorics);
  menu_append(GTK_MENU (subm_combinatorics), "nCr(n,k) - binomial coefficient", G_CALLBACK (entry_append), "nCr(");
  menu_append(GTK_MENU (subm_combinatorics), "nPr(n,k) - number of permutations", G_CALLBACK (entry_append), "nPr(");
  gtk_menu_append(GTK_MENU (subm_fns), subm_mit_combinatorics);

  // Functions/Vectors submenu
  GtkWidget *subm_vect = gtk_menu_new();
  GtkWidget *subm_mit_vect = gtk_menu_item_new_with_label("Vectors");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (subm_mit_vect), subm_vect);
  menu_append(GTK_MENU (subm_vect), "crossP(v1,v2) - return the vector cross product of v1 X v2", G_CALLBACK (entry_append), "crossP(");
  menu_append(GTK_MENU (subm_vect), "dotP(v1,v2) - return the vector dot product of v1 . v2", G_CALLBACK (entry_append), "dotP(");
  gtk_menu_append(GTK_MENU (subm_fns), subm_mit_vect);

  // Functions/Trigonometry submenu
  GtkWidget *subm_trig = gtk_menu_new();
  GtkWidget *subm_mit_trig = gtk_menu_item_new_with_label("Trigonometry");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (subm_mit_trig), subm_trig);
  menu_append(GTK_MENU (subm_trig), "acos(x) - arc cosine of x", G_CALLBACK (entry_append), "acos(");
  menu_append(GTK_MENU (subm_trig), "asin(x) - arc sine of x", G_CALLBACK (entry_append), "asin(");
  menu_append(GTK_MENU (subm_trig), "atan(x) - arc tangent of x", G_CALLBACK (entry_append), "atan(");
  menu_append(GTK_MENU (subm_trig), "cos(x) - cosine of x", G_CALLBACK (entry_append), "cos(");
  menu_append(GTK_MENU (subm_trig), "cosh(x) - hyperbolic cosine of x", G_CALLBACK (entry_append), "cosh(");
  menu_append(GTK_MENU (subm_trig), "sin(x) - sine of x", G_CALLBACK (entry_append), "sin(");
  menu_append(GTK_MENU (subm_trig), "sinh(x) - hyperbolic sine of x", G_CALLBACK (entry_append), "sinh(");
  menu_append(GTK_MENU (subm_trig), "tan(x) - tangent of x", G_CALLBACK (entry_append), "tan(");
  menu_append(GTK_MENU (subm_trig), "tanh(x) - hyperbolic tangent of x", G_CALLBACK (entry_append), "tanh(");
  gtk_menu_append(GTK_MENU (subm_fns), subm_mit_trig);

  // Functions/Math submenu
  GtkWidget *subm_math = gtk_menu_new();
  GtkWidget *subm_mit_math = gtk_menu_item_new_with_label("Math");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (subm_mit_math), subm_math);
  menu_append(GTK_MENU (subm_math), "abs(x) - absolute value of x", G_CALLBACK (entry_append), "abs(");
  menu_append(GTK_MENU (subm_math), "cbrt(x) - returns the cube root of x (you could also use x^(1/3))", G_CALLBACK (entry_append), "cbrt(");
  menu_append(GTK_MENU (subm_math), "ceil(x) - returns x rounded to the nearest integer upwards", G_CALLBACK (entry_append), "ceil(");
  menu_append(GTK_MENU (subm_math), "exp(x) - exponential function", G_CALLBACK (entry_append), "exp(");
  menu_append(GTK_MENU (subm_math), "factorial(x) - factorial of x (you can also use the '!' notation)", G_CALLBACK (entry_append), "factorial(");
  menu_append(GTK_MENU (subm_math), "fibo(x) - returns the xth term of the Fibonacci sequence", G_CALLBACK (entry_append), "fibo(");
  menu_append(GTK_MENU (subm_math), "floor(x) - returns x rounded to the nearest integer downwards", G_CALLBACK (entry_append), "floor(");
  menu_append(GTK_MENU (subm_math), "gcd(x,y) - finds the greatest common divisor of x and y", G_CALLBACK (entry_append), "gcd(");
  menu_append(GTK_MENU (subm_math), "im(x)/imag(x) - return the imaginary part of x", G_CALLBACK (entry_append), "imag(");
  menu_append(GTK_MENU (subm_math), "lcm(x,y) - finds the least common multiple of x and y", G_CALLBACK (entry_append), "lcm(");
  menu_append(GTK_MENU (subm_math), "ln(x) - natural logarithm", G_CALLBACK (entry_append), "ln(");
  menu_append(GTK_MENU (subm_math), "log10(x) - common logarithm", G_CALLBACK (entry_append), "log10(");
  menu_append(GTK_MENU (subm_math), "mod(x,y) - modulus (you can also use the C-style '%' notation)", G_CALLBACK (entry_append), "mod(");
  menu_append(GTK_MENU (subm_math), "product(expr,low,high) - returns the product of expr with x from low to high inclusive", G_CALLBACK (entry_append), "product(");
  menu_append(GTK_MENU (subm_math), "re(x)/real(x) - return the real part of x", G_CALLBACK (entry_append), "real(");
  menu_append(GTK_MENU (subm_math), "round(x) - round x to the nearest integer (for .5 cases, away from zero)", G_CALLBACK (entry_append), "round(");
  menu_append(GTK_MENU (subm_math), "sqrt(x) - returns the square root of x", G_CALLBACK (entry_append), "sqrt(");
  menu_append(GTK_MENU (subm_math), "sum(expr,low,high) - same as product, except the result is summation, not multiplication", G_CALLBACK (entry_append), "sum(");
  menu_append(GTK_MENU (subm_math), "totient(x) - Euler's totient function", G_CALLBACK (entry_append), "totient(");
  gtk_menu_append(GTK_MENU (subm_fns), subm_mit_math);

  // Functions/Chemistry submenu
  GtkWidget *subm_chemistry = gtk_menu_new();
  GtkWidget *subm_mit_chemistry = gtk_menu_item_new_with_label("Chemistry");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (subm_mit_chemistry), subm_chemistry);
  menu_append(GTK_MENU (subm_chemistry), "mmass(molecule) - returns the molar mass of the molecule specified (use standard notation without using subscripts, such as H2O or Ca(OH)2)", G_CALLBACK (entry_append), "mmass(");
  gtk_menu_append(GTK_MENU (subm_fns), subm_mit_chemistry);

  // Functions/Misc submenu
  GtkWidget *subm_misc = gtk_menu_new();
  GtkWidget *subm_mit_misc = gtk_menu_item_new_with_label("Misc");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (subm_mit_misc), subm_misc);
  menu_append(GTK_MENU (subm_misc), "ans() - Will be replaced by the result of the last entered expression", G_CALLBACK (entry_append), "ans()");
  menu_append(GTK_MENU (subm_misc), "help(\"str\") - show help for function/variable/topic called str", G_CALLBACK (entry_append), "help(\"");
  menu_append(GTK_MENU (subm_misc), "input([str]) - shows an inut dialog using either a standard prompt or str, if specified", G_CALLBACK (entry_append), "input(");
  menu_append(GTK_MENU (subm_misc), "join(a,b) - join a and b, two vectors, together", G_CALLBACK (entry_append), "join(");
  menu_append(GTK_MENU (subm_misc), "length(x) - return the length of x", G_CALLBACK (entry_append), "length(");
  menu_append(GTK_MENU (subm_misc), "num(str) - returns a number representation of the number in the string str", G_CALLBACK (entry_append), "num(");
  menu_append(GTK_MENU (subm_misc), "ones(n) - generate a list containing n ones", G_CALLBACK (entry_append), "ones(");
  menu_append(GTK_MENU (subm_misc), "print(expr_1[,expr_2,...,expr_n]) - prints its number or string arguments (if more than one, separated by a space) and a newline character (useful for printing variables in loops)", G_CALLBACK (entry_append), "print(");
  menu_append(GTK_MENU (subm_misc), "rand([int]) - if int is specified, return a random integer in the range [1 ; int], otherwise return a real number in the range [0;1[", G_CALLBACK (entry_append), "rand(");
  menu_append(GTK_MENU (subm_misc), "sort(list[,cmp]) - sort list, optionally using cmp as comparison function", G_CALLBACK (entry_append), "sort(");
  menu_append(GTK_MENU (subm_misc), "str(expr) - returns a string representation of expr", G_CALLBACK (entry_append), "str(");
  menu_append(GTK_MENU (subm_misc), "write(fname,expr_1[expr_2,...,expr_n]) - behaves like the print function, but writes to the file specified with fname", G_CALLBACK (entry_append), "write(");
  menu_append(GTK_MENU (subm_misc), "zeros(n) - generate a list containing n zeros", G_CALLBACK (entry_append), "zeros(");
  gtk_menu_append(GTK_MENU (subm_fns), subm_mit_misc);

  // Functions/* submenu

  // Conversions submenu
  menu_append(GTK_MENU (subm_conv), "chartocode(x) - converts x, a single-character string, to its ASCII code", G_CALLBACK (entry_append), "chartocode(");
  menu_append(GTK_MENU (subm_conv), "cmtoinch(x) - converts x, which has to be in cm, to inches", G_CALLBACK (entry_append), "cmtoinch(");
  menu_append(GTK_MENU (subm_conv), "codetochar(x) - converts x, an ASCII code, to the corresponding character", G_CALLBACK (entry_append), "codetochar(");
  menu_append(GTK_MENU (subm_conv), "ctof(x) - converts x, which has to be in degrees Celsius, to Fahrenheit", G_CALLBACK (entry_append), "ctof(");
  menu_append(GTK_MENU (subm_conv), "ctok(x) - converts x, which has to be in degrees Celsius, to Kelvin", G_CALLBACK (entry_append), "ctok(");
  menu_append(GTK_MENU (subm_conv), "floztoml(x) - converts x, which has to be in US fluid ounces, to ml", G_CALLBACK (entry_append), "floztoml(");
  menu_append(GTK_MENU (subm_conv), "ftoc(x) - converts x, which has to be in Fahrenheit, to degrees Celsius", G_CALLBACK (entry_append), "ftoc(");
  menu_append(GTK_MENU (subm_conv), "ftok(x) - converts x, which has to be in Fahrenheit, to Kelvin", G_CALLBACK (entry_append), "ftok(");
  menu_append(GTK_MENU (subm_conv), "fttom(x) - converts x, which has to be in feet, to meters", G_CALLBACK (entry_append), "fttom(");
  menu_append(GTK_MENU (subm_conv), "inchtocm(x) - converts x, which has to be in inches, to cm", G_CALLBACK (entry_append), "inchtocm(");
  menu_append(GTK_MENU (subm_conv), "kmtomi(x) - converts x, which has to be in km, to miles", G_CALLBACK (entry_append), "kmtomi(");
  menu_append(GTK_MENU (subm_conv), "ktoc(x) - converts x, which has to be in Kelvin, to degrees Celsius", G_CALLBACK (entry_append), "ktoc(");
  menu_append(GTK_MENU (subm_conv), "ktof(x) - converts x, which has to be in Kelvin, to Fahrenheit", G_CALLBACK (entry_append), "ktof(");
  menu_append(GTK_MENU (subm_conv), "mitokm(x) - converts x, which has to be in miles, to km", G_CALLBACK (entry_append), "mitokm(");
  menu_append(GTK_MENU (subm_conv), "mltofloz(x) - converts x, which has to be in ml, to US fluid ounces", G_CALLBACK (entry_append), "mltofloz(");
  menu_append(GTK_MENU (subm_conv), "mtoft(x) - converts x, which has to be in m, to feet", G_CALLBACK (entry_append), "mtoft(");

  // Constants submenu
  menu_append(GTK_MENU (subm_cnst), "answer - The Answer", G_CALLBACK (entry_append), "answer");
  menu_append(GTK_MENU (subm_cnst), "c - speed of light", G_CALLBACK (entry_append), "c");
  menu_append(GTK_MENU (subm_cnst), "e - Euler's number", G_CALLBACK (entry_append), "e");
  menu_append(GTK_MENU (subm_cnst), "g - std gravitational acceleration on Earth", G_CALLBACK (entry_append), "g");
  menu_append(GTK_MENU (subm_cnst), "Na - Avogadro constant", G_CALLBACK (entry_append), "Na");
  menu_append(GTK_MENU (subm_cnst), "phi - golden ratio", G_CALLBACK (entry_append), "phi");
  menu_append(GTK_MENU (subm_cnst), "pi", G_CALLBACK (entry_append), "pi");

  // Graph submenu
  menu_append(GTK_MENU (subm_graph), "Help", G_CALLBACK (hcg_help_graph), NULL);
  menu_append(GTK_MENU (subm_graph), "graph(expr[, xmin, xmax, ymin, ymax]) - draw a graph", G_CALLBACK (entry_append), "graph(");
  menu_append(GTK_MENU (subm_graph), "gmul(expr_1, expr_2, ..., expr_n) - draw multiple functions on one graph", G_CALLBACK (entry_append), "gmul(");
  menu_append(GTK_MENU (subm_graph), "graph3(expr[, xmin, xmax, ymin, ymax, zmin, zmax]) - draw a 3D graph", G_CALLBACK (entry_append), "graph3(");
  menu_append(GTK_MENU (subm_graph), "graphpeq(expr_x,expr_y[,tmin,tmax,xmin,xmax,ymin,ymax]) - draw the parametric equation modeled by expr_x and expr_y (use t as the variable)", G_CALLBACK (entry_append), "graphpeq(");
  menu_append(GTK_MENU (subm_graph), "slpfld(expr[,xmin,xmax,ymin,ymax]) - draw a slope field of dy/dx = expr", G_CALLBACK (entry_append), "slpfld(");

  // Stats submenu
  menu_append(GTK_MENU (subm_stats), "stats(v1,v2,...,vn) - show stats info of v1...vn", G_CALLBACK (entry_append), "stats(");
  menu_append(GTK_MENU (subm_stats), "statsf(v1,freq1,v2,freq2,...,...,vn,freqn) - same as stats, but after each value, you have to append its frequency", G_CALLBACK (entry_append), "statsf(");
  menu_append(GTK_MENU (subm_stats), "boxplot(v1,v2,...,vn) - draw a boxplot of v1...vn", G_CALLBACK (entry_append), "boxplot(");
  menu_append(GTK_MENU (subm_stats), "boxplotf(v1,freq1,v2,freq2,...,...,vn,freqn) - same as boxplot, but after each value, you have to append its frequency", G_CALLBACK (entry_append), "boxplot(");
	
  // User-defined functions and variables submenu
  menu_append(GTK_MENU (subm_usrdf), "Help", G_CALLBACK (hcg_help_usrdf), NULL);
  menu_append(GTK_MENU (subm_usrdf), "Clear all user-defined variables and functions", G_CALLBACK (hcg_clear_usrdf), NULL);

  // Help submenu
  GtkWidget *subm_mit_help = gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP, NULL);
  GtkWidget *subm_mit_cplx_num = gtk_menu_item_new_with_mnemonic("_Complex numbers");
  GtkWidget *subm_mit_logic = gtk_menu_item_new_with_mnemonic("_Logic");
  GtkWidget *subm_mit_update = gtk_menu_item_new_with_mnemonic("Check for _updates");
  GtkWidget *subm_mit_about = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, NULL);
  gtk_menu_append(GTK_MENU (subm_help), subm_mit_help);
  gtk_menu_append(GTK_MENU (subm_help), subm_mit_cplx_num);
  gtk_menu_append(GTK_MENU (subm_help), subm_mit_logic);
  gtk_menu_append(GTK_MENU (subm_help), subm_mit_update);
  gtk_menu_append(GTK_MENU (subm_help), subm_mit_about);
  g_signal_connect(G_OBJECT (subm_mit_help), "activate", G_CALLBACK (hcg_help), NULL);
  g_signal_connect(G_OBJECT (subm_mit_cplx_num), "activate", G_CALLBACK (hcg_help_cplx), NULL);
  g_signal_connect(G_OBJECT (subm_mit_logic), "activate", G_CALLBACK (hcg_help_logic), NULL);
  g_signal_connect(G_OBJECT (subm_mit_update), "activate", G_CALLBACK (hcg_update), NULL);
  g_signal_connect(G_OBJECT (subm_mit_about), "activate", G_CALLBACK (hcg_about), NULL);

  gtk_box_pack_start_defaults(GTK_BOX (main_box), main_menu_bar);

  // Scroll-back
  scroll_window = gtk_scrolled_window_new(NULL,NULL);
  scroll_text_view = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW (scroll_text_view), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW (scroll_text_view), FALSE);
  gtk_container_add(GTK_CONTAINER (scroll_window), scroll_text_view);
  scroll_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(scroll_text_view));
  gtk_box_pack_start(GTK_BOX (main_box), scroll_window, TRUE, TRUE, 0);

  // Expression entry
  expr_entry = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY (expr_entry), MAX_EXPR+1);
  expr_com = gtk_entry_completion_new();
  expr_com_ls = gtk_list_store_new(2,G_TYPE_STRING,G_TYPE_STRING);
  // Not ready yet -- wait for some next version :)
  //gtk_list_store_append(GTK_LIST_STORE (expr_com_ls), &expr_com_iter);
  //gtk_list_store_set(GTK_LIST_STORE (expr_com_ls), &expr_com_iter, 0, "test", 1, "test", -1);
  gtk_entry_set_completion(GTK_ENTRY (expr_entry), expr_com);
  gtk_entry_completion_set_model(expr_com, GTK_TREE_MODEL (expr_com_ls));
  gtk_entry_completion_set_text_column(expr_com, 0);
  g_signal_connect(G_OBJECT (expr_com), "match-selected", G_CALLBACK (hcg_match_select), NULL);
  gtk_box_pack_start_defaults(GTK_BOX (main_box), expr_entry);
  g_signal_connect(G_OBJECT (expr_entry), "activate", G_CALLBACK (hcg_process_expr), NULL);

  // "Clickable" keys
  keys_table = gtk_table_new(5,4,TRUE);
  GtkWidget *k_pl,*k_pr,*k_sqrt,*k_div,*k_mul,*k_min,*k_plus,*k_enter,*k_radix,*k_0,*k_1,*k_2,*k_3,*k_4,*k_5,*k_6,*k_7,*k_8,*k_9,*k_C;
  k_pl = gtk_button_new_with_label("(");
  k_pr = gtk_button_new_with_label(")");
  k_sqrt = gtk_button_new_with_label("√");
  k_div = gtk_button_new_with_label("÷");
  k_mul = gtk_button_new_with_label("×");
  k_min = gtk_button_new_with_label("−");
  k_plus = gtk_button_new_with_label("+");
  k_enter = gtk_button_new_with_label("=");
  k_radix = gtk_button_new_with_label(".");
  k_0 = gtk_button_new_with_label("0");
  k_1 = gtk_button_new_with_label("1");
  k_2 = gtk_button_new_with_label("2");
  k_3 = gtk_button_new_with_label("3");
  k_4 = gtk_button_new_with_label("4");
  k_5 = gtk_button_new_with_label("5");
  k_6 = gtk_button_new_with_label("6");
  k_7 = gtk_button_new_with_label("7");
  k_8 = gtk_button_new_with_label("8");
  k_9 = gtk_button_new_with_label("9");
  k_C = gtk_button_new_with_label("C");
  g_signal_connect(G_OBJECT (k_pl), "clicked", G_CALLBACK (entry_append), "(");
  g_signal_connect(G_OBJECT (k_pr), "clicked", G_CALLBACK (entry_append), ")");
  g_signal_connect(G_OBJECT (k_sqrt), "clicked", G_CALLBACK (entry_append), "sqrt(");
  g_signal_connect(G_OBJECT (k_div), "clicked", G_CALLBACK (entry_append), "/");
  g_signal_connect(G_OBJECT (k_7), "clicked", G_CALLBACK (entry_append), "7");
  g_signal_connect(G_OBJECT (k_8), "clicked", G_CALLBACK (entry_append), "8");
  g_signal_connect(G_OBJECT (k_9), "clicked", G_CALLBACK (entry_append), "9");
  g_signal_connect(G_OBJECT (k_mul), "clicked", G_CALLBACK (entry_append), "*");
  g_signal_connect(G_OBJECT (k_4), "clicked", G_CALLBACK (entry_append), "4");
  g_signal_connect(G_OBJECT (k_5), "clicked", G_CALLBACK (entry_append), "5");
  g_signal_connect(G_OBJECT (k_6), "clicked", G_CALLBACK (entry_append), "6");
  g_signal_connect(G_OBJECT (k_min), "clicked", G_CALLBACK (entry_append), "-");
  g_signal_connect(G_OBJECT (k_1), "clicked", G_CALLBACK (entry_append), "1");
  g_signal_connect(G_OBJECT (k_2), "clicked", G_CALLBACK (entry_append), "2");
  g_signal_connect(G_OBJECT (k_3), "clicked", G_CALLBACK (entry_append), "3");
  g_signal_connect(G_OBJECT (k_plus), "clicked", G_CALLBACK (entry_append), "+");
  g_signal_connect(G_OBJECT (k_0), "clicked", G_CALLBACK (entry_append), "0");
  g_signal_connect(G_OBJECT (k_radix), "clicked", G_CALLBACK (entry_append), ".");
  g_signal_connect(G_OBJECT (k_C), "clicked", G_CALLBACK (hcg_C), NULL);
  g_signal_connect(G_OBJECT (k_enter), "clicked", G_CALLBACK (hcg_process_expr), NULL);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_pl, 0, 1, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_pr, 1, 2, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_sqrt, 2, 3, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_div, 3, 4, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_7, 0, 1, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_8, 1, 2, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_9, 2, 3, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_mul, 3, 4, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_4, 0, 1, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_5, 1, 2, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_6, 2, 3, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_min, 3, 4, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_1, 0, 1, 3, 4);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_2, 1, 2, 3, 4);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_3, 2, 3, 3, 4);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_plus, 3, 4, 3, 4);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_0, 0, 1, 4, 5);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_radix, 1, 2, 4, 5);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_C, 2, 3, 4, 5);
  gtk_table_attach_defaults(GTK_TABLE (keys_table), k_enter, 3, 4, 4, 5);
  gtk_box_pack_start_defaults(GTK_BOX (main_box), keys_table);

  // Configuration box
  GtkWidget *conf_box = gtk_hbox_new(FALSE, 0);
  GtkWidget *spin_precision = gtk_spin_button_new_with_range(1,8192,1);
  gtk_box_pack_start_defaults(GTK_BOX (conf_box), gtk_label_new("Precision: "));
  gtk_spin_button_set_value(GTK_SPIN_BUTTON (spin_precision), (gdouble) hc.precision);
  g_signal_connect(G_OBJECT (spin_precision), "value-changed", G_CALLBACK (hcg_cfg_change_precision), NULL);
  gtk_box_pack_start_defaults(GTK_BOX (conf_box), spin_precision);
  gtk_box_pack_start_defaults(GTK_BOX (conf_box), gtk_label_new("digits  "));
  GtkWidget *radio_angle_deg = gtk_radio_button_new_with_label(NULL, "DEG");
  GtkWidget *radio_angle_rad = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (radio_angle_deg), "RAD");
  GtkWidget *radio_angle_grad = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (radio_angle_deg), "GRAD");
  g_signal_connect(G_OBJECT (radio_angle_deg), "toggled", G_CALLBACK (hcg_cfg_change_angle), (gpointer) 0);
  g_signal_connect(G_OBJECT (radio_angle_rad), "toggled", G_CALLBACK (hcg_cfg_change_angle), (gpointer) 1);
  g_signal_connect(G_OBJECT (radio_angle_grad), "toggled", G_CALLBACK (hcg_cfg_change_angle), (gpointer) 2);
  gtk_box_pack_start_defaults(GTK_BOX (conf_box), radio_angle_deg);
  gtk_box_pack_start_defaults(GTK_BOX (conf_box), radio_angle_rad);
  gtk_box_pack_start_defaults(GTK_BOX (conf_box), radio_angle_grad);
  switch (hc.angle)
  {
  case 'd':
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (radio_angle_deg), TRUE);
    break;
  case 'r':
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (radio_angle_rad), TRUE);
    break;
  case 'g':
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (radio_angle_grad), TRUE);
    break;

  }
  GtkWidget *radio_exp_n = gtk_radio_button_new_with_label(NULL, "NORMAL");
  GtkWidget *radio_exp_sci = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (radio_exp_n), "SCI");
  GtkWidget *radio_exp_eng = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (radio_exp_n), "ENG");
  g_signal_connect(G_OBJECT (radio_exp_n), "toggled", G_CALLBACK (hcg_cfg_change_exp), (gpointer) 0);
  g_signal_connect(G_OBJECT (radio_exp_sci), "toggled", G_CALLBACK (hcg_cfg_change_exp), (gpointer) 1);
  g_signal_connect(G_OBJECT (radio_exp_eng), "toggled", G_CALLBACK (hcg_cfg_change_exp), (gpointer) 2);
  gtk_box_pack_start_defaults(GTK_BOX (conf_box), radio_exp_n);
  gtk_box_pack_start_defaults(GTK_BOX (conf_box), radio_exp_sci);
  gtk_box_pack_start_defaults(GTK_BOX (conf_box), radio_exp_eng);
  switch (hc.exp)
  {
  case 'n':
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (radio_exp_n), TRUE);
    break;
  case 's':
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (radio_exp_sci), TRUE);
    break;
  case 'e':
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (radio_exp_eng), TRUE);
    break;

  }
  GtkWidget *toggle_rpn = gtk_check_button_new_with_label("RPN");
  if (hc.rpn)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (toggle_rpn), TRUE);
  else
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (toggle_rpn), FALSE);
  g_signal_connect(G_OBJECT (toggle_rpn), "toggled", G_CALLBACK (hcg_cfg_change_rpn), NULL);
  gtk_box_pack_start_defaults(GTK_BOX (conf_box), toggle_rpn);
  GtkWidget *toggle_keys = gtk_check_button_new_with_label("KEYS");
  if (hc.keys)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (toggle_keys), TRUE);
  else
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (toggle_keys), FALSE);
  g_signal_connect(G_OBJECT (toggle_keys), "toggled", G_CALLBACK (hcg_cfg_change_keys), NULL);
  gtk_box_pack_start_defaults(GTK_BOX (conf_box), toggle_keys);
  gtk_box_pack_start_defaults(GTK_BOX (main_box), gtk_hseparator_new());
  gtk_box_pack_start_defaults(GTK_BOX (main_box), conf_box);

  scrollprint("Welcome to HC!\nTo evaluate an expression, type it below and press enter.");

  gtk_widget_show_all(window);
  if (!hc.keys)
    gtk_widget_hide_all(keys_table);
  gtk_main();
  return 0;
}


void hcg_quit()
{
  hc_save_cfg();
  gtk_main_quit();
}


void hcg_about()
{
  gtk_show_about_dialog (NULL,
			 "program-name", "HC",
			 "comments", "A high-precision scientific calculator.",
			 "website", "http://houbysoft.com/hc/",
			 "title", "About HC",
			 "copyright", "This program is available under the terms of the GNU GPL v3 License.\n(c) Jan Dlabal, 2010.",
			 NULL);
}


void hcg_update()
{
#ifndef WIN32
  notify("In Linux, please use your package manager to update HC (in ArchLinux, hc and hcg are in AUR).");
#else
  HUL *update = hul_checkupdates(VERSION,STATUS_URL_GUI);
  if (update && update->version)
  {
    // TODO
  } else if (update) {
    notify("You have the latest version.");
  } else {
    notify_error("An error occured while checking for updates.");
  }
#endif
}


void hcg_help()
{
  notify("Welcome to the HoubySoft Calculator!\nUsage is very simple; to evaluate an expression either type it with your keyboard, or use the on-screen keys, and then press enter (or the '=' symbol, if using the onscreen keyboard).\n\nIf you need a constant, function, or conversion function, either type it with your keyboard if you remember it, or find it using the menus, and then click on the menu item to insert it.\n\nTo get help about any function, constant, etc., use the help() function, calling it with the name of the object you want to learn about as a string. For example, to learn about the fibo() function, you would type help(\"fibo\").\n\nBelow the expression entry and the onscreen keys (if enabled), you will notice some configuration options:\n  - precision (calculated in scientific notation) - enter whatever precision you require\n - DEG / RAD / GRAD - the angle mode - degrees, radians and gradians respectively, click on the one you want to use\n  - the SCI checkbox - select it if you want the results to be displayed in scientific notation\n  - the RPN checkbox - select it if you want to use RPN (ie use 2 2 + instead of 2+2)\n  - the KEYS checkbox - select it if you want the onscreen keys to be displayed.\n\n\nIf you have any questions, feel free to email me at dlabaljan@gmail.\nThis program is (C) Jan Dlabal, 2009-2010 and is distributed under the terms of the GNU GPL version 3 License.");
}


void hcg_help_cplx()
{
  free(hc_result("help(\"complex\")"));
}


void hcg_help_logic()
{
  free(hc_result("help(\"logic\")"));
}


void scrollprint(char *format, ...)
{
  va_list args;
  char *line = NULL;

  va_start(args, format);
  g_vasprintf(&line,format,args);
  va_end(args);

  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER (scroll_buf), &iter);
  gtk_text_buffer_insert(GTK_TEXT_BUFFER (scroll_buf), &iter, (gchar *) line, strlen(line));
  gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER (scroll_buf), &iter);
  GtkTextMark *mark = gtk_text_buffer_create_mark(GTK_TEXT_BUFFER (scroll_buf), NULL, &iter, FALSE);
  gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (scroll_text_view), mark, 0.0, FALSE, 0.0, 0.0);
  gtk_widget_grab_focus(expr_entry);
}


void hcg_cfg_change_precision(GtkWidget *widget, gpointer trash)
{
  hc.precision = (int) gtk_spin_button_get_value(GTK_SPIN_BUTTON (widget));
}


void hcg_cfg_change_rpn(GtkWidget *widget, gpointer trash)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widget))==TRUE)
    hc.rpn = TRUE;
  else
    hc.rpn = FALSE;
}


void hcg_cfg_change_angle(GtkWidget *widget, gpointer trash)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widget))==TRUE)
  {
    switch ((int) trash)
    {
    case 0:
      hc.angle = 'd';
      break;
    case 1:
      hc.angle = 'r';
      break;
    case 2:
      hc.angle = 'g';
      break;
    }
  }
}


void hcg_cfg_change_exp(GtkWidget *widget, gpointer trash)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widget))==TRUE)
  {
    switch ((int) trash)
    {
    case 0:
      hc.exp = 'n';
      break;
    case 1:
      hc.exp = 's';
      break;
    case 2:
      hc.exp = 'e';
      break;
    }
  }
}


void hcg_cfg_change_keys(GtkWidget *widget, gpointer trash)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (widget))==TRUE)
  {
    gtk_widget_show_all(keys_table);
    hc.keys = TRUE;
  } else {
    gtk_widget_hide_all(keys_table);
    hc.keys = FALSE;
  }
}


gboolean hcg_match_select(GtkEntryCompletion *widget, GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data)
{
  GValue value = {0, };
  gtk_tree_model_get_value(model, iter, 1, &value);
  printf("%s\n",g_value_get_string(&value));
  g_value_unset(&value);
  return TRUE;
}


void hcg_process_expr(GtkWidget *widget, gpointer trash)
{
  if (gtk_entry_get_text(GTK_ENTRY (expr_entry))[0]=='\\')
  {
    hcg_C(NULL,NULL);
    notify_error("Sorry, you can't use these commands in GUI mode. Please use the configuration widgets under the entry and the keys (if enabled) to configure HC in GUI mode.");
    return;
  }
  static char *tmp1 = NULL;
  if (!tmp1)
  {
    tmp1 = malloc(strlen((char *)gtk_entry_get_text(GTK_ENTRY (expr_entry)))+sizeof(char));
    strcpy(tmp1,(char *)gtk_entry_get_text(GTK_ENTRY (expr_entry)));
  } else {
    tmp1 = realloc(tmp1,strlen(tmp1)+strlen((char *)gtk_entry_get_text(GTK_ENTRY (expr_entry)))+2*sizeof(char));
    strcat(tmp1," ");
    strcat(tmp1,(char *)gtk_entry_get_text(GTK_ENTRY (expr_entry)));
  }
  if (check_completeness(tmp1))
  {
    scrollprint("\n>%s",gtk_entry_get_text(GTK_ENTRY (expr_entry)));
    char *tmp = hc_result(tmp1);
    if (tmp && strlen(tmp))
      scrollprint("\n = %s",tmp);
    else
      gtk_widget_grab_focus(expr_entry);
    if (tmp)
      free(tmp);
    free(tmp1);
    tmp1 = NULL;
  } else {
    scrollprint("\n. %s",gtk_entry_get_text(GTK_ENTRY (expr_entry)));
  }
}


void entry_append(GtkWidget *widget, char *what)
{
  gint s,e;
  if (gtk_editable_get_selection_bounds(GTK_EDITABLE (expr_entry), &s, &e)==TRUE)
  {
    hcg_C(NULL,NULL);
  }
  gint pos = gtk_editable_get_position(GTK_EDITABLE (expr_entry));
  gtk_editable_insert_text(GTK_EDITABLE (expr_entry), what, strlen(what), &pos);
  gtk_editable_set_position(GTK_EDITABLE (expr_entry), pos);
}


void hcg_C(GtkWidget *widget, gpointer trash)
{
  gtk_entry_set_text(GTK_ENTRY (expr_entry), "");
}


void hcg_help_usrdf(GtkWidget *widget, gpointer trash)
{
  notify("USER-DEFINED VARIABLES AND FUNCTIONS\n====================================\nFrom hc version 0.5, there is support for user-defined variables and functions in the calculator.\nThis is a brief description of how to use them and how they work.\n\nNaming\n------\nNames must consist of only numbers and letters. The first character must be a letter.\n\nDefining\n--------\nTo define a variable, use this syntax:\nVAR_NAME = VAR_VALUE\nVAR_NAME is the variable's name (surprisingly), and VAR_VALUE is any valid expression.\nSo, for example, to define a new variable named x and having the value 27, you would type for example this:\nx = 20+7+sqrt(0)\nThen, you can use x in calculations, so after this definition, if you type x^2, you will get this answer:\n> x^2\n 729\n\nTo define a function, use this syntax:\nFUNC_NAME(ARG1, ARG2, ..., ARGN) = EXPRESSION\nFUNC_NAME is again the function's name, ARG1...N are its arguments, and EXPRESSION is the function's expression.\nI'll give you an example again since I think this is the best way to demonstrate how to use this.\nLet's say you want to create a function that will square its first argument, and you want to name it func. To accomplish that, you would type (for example):\nfunc(x) = x^2\nThen you can test your new function by typing for example func(3) in the prompt; you will get 9 as the answer.\n");
}


void hcg_help_graph(GtkWidget *widget, gpointer trash)
{
  notify("From version 0.6, HC has graphing capabilities.\nThe syntax of the function to draw 2D graphs, graph() (surprise), is very simple; it takes 1 required and 4 optional arguments:\n - expr (required) : an expression of the function, for example x^2\n - xmin (optional) : the minimum x value\n - xmax (optional) : the maximum x value\n - ymin (optional) : the minimum y value\n - ymax (optional) : the maximum y value.\n\nExample :\ngraph(x^2,-10,10,-100,100)\nNote: should you want to graph more functions on one graph, use gmul(expr_1, expr_2, ..., expr_n)\n\nThe function to draw 3D graphs - graph3 - (available from version 0.7) is similar, but takes two more optional arguments, zmin and zmax. Example :\ngraph3((x^3*y-y^3*x)/390,-10,10,-10,10,-10,10)\n\nIf nothing is provided for the optional arguments xmin, etc., the last will be used.\n");
}


void hcg_clear_usrdf(GtkWidget *widget, gpointer trash)
{
  char *blah = malloc(strlen("\\clear")+1);
  strcpy(blah,"\\clear");
  free(hc_result(blah));
  free(blah);
  notify("All user-defined variables were cleared.");
}


void hcg_disp_graph(char *fname)
{
  GtkWidget *graph_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW (graph_win), "HC - Graph");
  gtk_window_set_resizable(GTK_WINDOW (graph_win), FALSE);
  gtk_container_add(GTK_CONTAINER (graph_win), gtk_image_new_from_file(fname));
  gtk_widget_show_all(graph_win);
}


void hcg_load()
{
  GtkWidget *dialog = gtk_file_chooser_dialog_new ("Load File",
						   GTK_WINDOW (window),
						   GTK_FILE_CHOOSER_ACTION_OPEN,
						   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						   GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						   NULL);
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
  {
    char *filename;
    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
    hc_load(filename);
    g_free (filename);
  }
  gtk_widget_destroy (dialog);
}


void load_error(unsigned int line, char *expr)
{
  char *str = g_strdup_printf("Error occured at line %i (%s), interrupting execution.\n",line,expr);
  notify_error(str);
  g_free(str);
}


void type_error(char *expr)
{
  char *str = g_strdup_printf("Type error : %s\n",expr);
  notify_error(str);
  g_free(str);
}
