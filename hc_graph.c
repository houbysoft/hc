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
#include <string.h>
#include <stdlib.h>
#include <plplot/plplot.h>
#include "hc.h"
#include "hc_functions.h"
#include "hc_graph.h"


#define HC_GRAPH_POINTS 1000


// Draw a defined function
// Arguments : func_name, xmin, xmax, ymin, ymax
char hc_graph(char *e)
{
  char *func_expr,*t1,*t2,*t3,*t4,*arg_xmin,*arg_xmax,*arg_ymin,*arg_ymax;
  func_expr = hc_get_arg(e,1);
  t1 = hc_get_arg(e,2);
  t2 = hc_get_arg(e,3);
  t3 = hc_get_arg(e,4);
  t4 = hc_get_arg(e,5);
  arg_xmin = hc_result_(t1);
  arg_xmax = hc_result_(t2);
  arg_ymin = hc_result_(t3);
  arg_ymax = hc_result_(t4);
  free(t1);
  free(t2);
  double xmin,xmax,ymin,ymax;

  if (!func_expr || !arg_xmin || !arg_xmax || !arg_ymin || !arg_ymax)
    arg_error("graph() needs 3 arguments (expr,xmin,xmax,ymin,ymax).");

  xmin = strtod(arg_xmin,NULL);
  xmax = strtod(arg_xmax,NULL);
  ymin = strtod(arg_ymin,NULL);
  ymax = strtod(arg_ymax,NULL);

  int j=strlen(arg_xmin)-1;
  while (arg_xmin[j]=='0')
    arg_xmin[j--]=0;
  if (arg_xmin[j]=='.')
    arg_xmin[j]=0;
  j = strlen(arg_xmax)-1;
  while (arg_xmax[j]=='0')
    arg_xmax[j--]=0;
  if (arg_xmax[j]=='.')
    arg_xmax[j]=0;

  // freed later (needed for labels)

  unsigned int i = 0;
  PLFLT *a= malloc(sizeof(PLFLT)*HC_GRAPH_POINTS);
  PLFLT *a_x = malloc(sizeof(PLFLT)*HC_GRAPH_POINTS);
  char *a_hasval = malloc(sizeof(char)*HC_GRAPH_POINTS);
  char **t = malloc(sizeof (char *) * HC_GRAPH_POINTS);
  char *t_2 = malloc(sizeof (char) * HC_GRAPH_POINTS);
  for (j=0; j<HC_GRAPH_POINTS; j++)
    t[j] = NULL;
  t[0] = arg_xmin;
  t[HC_GRAPH_POINTS-1] = arg_xmax;
  for (j=0; j<HC_GRAPH_POINTS; j++)
    t_2[j] = FALSE;
  t_2[0] = TRUE;
  t_2[HC_GRAPH_POINTS-1] = TRUE;
  double step = fabs(xmax-xmin) / HC_GRAPH_POINTS;
  double curx = xmin;

  for (; i<HC_GRAPH_POINTS; i++,curx+=step)
  {
    char tmp_curx[256];
    sprintf(tmp_curx,"%f",curx);
    a_x[i] = strtod(tmp_curx,NULL);
    char *tmp_expr = strreplace(func_expr,"x",tmp_curx);
    char *tmp_2 = hc_result_(tmp_expr);
    if (tmp_2)
    {
      a_hasval[i] = 'y';
      a[i] = strtod(tmp_2,NULL);
    } else {
      if (i!=0)
	a[i] = a[i-1];
      else
	a[i] = 0;
      a_hasval[i] = 'n';
    }
    free(tmp_expr);
    if (tmp_2)
      free(tmp_2);
    if (fabs(curx) <= step)
    {
      t[i] = "0";
      t_2[i] = TRUE;
    }
  }

#ifndef HCG
  if (!hc.plplot_dev_override)
    plsdev("pngcairo");
#else
  plsdev("pngcairo");
  plsfnam("tmp-graph.png");
#endif
  plinit();
  plcol0(15);
  plenv(xmin,xmax,ymin,ymax,0,1);
  char *graph_top_label = malloc(strlen("HoubySoft Calculator - Graph - ")+strlen(func_expr)+1);
  if (!graph_top_label)
    mem_error();
  strcpy(graph_top_label,"HoubySoft Calculator - Graph - ");
  strcat(graph_top_label,func_expr);
  pllab("x","y",graph_top_label);
  plcol0(1);
  for (i=0; i<HC_GRAPH_POINTS-1; i++)
  {
    if (a_hasval[i]=='y' && a_hasval[i+1]=='y') // discontinuity check
      pljoin(a_x[i],a[i],a_x[i+1],a[i+1]);
  }
  plend();

  free(graph_top_label);
  free(func_expr);
  free(arg_xmin);
  free(arg_xmax);
  free(t);
  free(t_2);
  free(a_x);
  free(a);
  free(a_hasval);
  
  return SUCCESS;
}
