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


#define HC_GRAPH_POINTS 100
#define HC_GRAPH_POINTS_3D 30


// Taken from example 11, to setup the color palette for 3D graphs
static void cmap1_init()
{
  PLFLT i[2], h[2], l[2], s[2];

  i[0] = 0.0;		/* left boundary */
  i[1] = 1.0;		/* right boundary */

  h[0] = 240; /* blue -> green -> yellow -> */
  h[1] = 0;   /* -> red */

  l[0] = 0.6;
  l[1] = 0.6;

  s[0] = 0.8;
  s[1] = 0.8;

  plscmap1n(256);
  c_plscmap1l(0, 2, i, h, l, s, NULL);
}


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
    arg_error("graph() needs 5 arguments (expr,xmin,xmax,ymin,ymax).");

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
  double step = fabs(xmax-xmin) / (HC_GRAPH_POINTS - 1);
  double curx = xmin;

  graphing_ignore_errors = TRUE;
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
  }
  graphing_ignore_errors = FALSE;

#ifndef HCG
  if (!hc.plplot_dev_override)
#ifndef WIN32
    plsdev("pngcairo");
#else
    plsdev("wingcc");
#endif
#else
#ifndef WIN32
  plsdev("pngcairo");
  plsfnam("tmp-graph.png");
#else
  plsdev("wingcc");
#endif
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
  free(a_x);
  free(a);
  free(a_hasval);

#ifdef HCG
#ifndef WIN32
  hcg_disp_graph("tmp-graph.png");
  remove("tmp-graph.png");
#endif
#endif
  
  return SUCCESS;
}



char hc_graph3d(char *e)
{
  char *func_expr,*t1,*t2,*t3,*t4,*t5,*t6,*arg_xmin,*arg_xmax,*arg_ymin,*arg_ymax,*arg_zmin,*arg_zmax;
  func_expr = hc_get_arg(e,1);
  t1 = hc_get_arg(e,2);
  t2 = hc_get_arg(e,3);
  t3 = hc_get_arg(e,4);
  t4 = hc_get_arg(e,5);
  t5 = hc_get_arg(e,6);
  t6 = hc_get_arg(e,7);
  arg_xmin = hc_result_(t1);
  arg_xmax = hc_result_(t2);
  arg_ymin = hc_result_(t3);
  arg_ymax = hc_result_(t4);
  arg_zmin = hc_result_(t5);
  arg_zmax = hc_result_(t6);
  // free those
  double xmin,xmax,ymin,ymax,zmin,zmax;

  if (!func_expr || !arg_xmin || !arg_xmax || !arg_ymin || !arg_ymax || !arg_zmin || !arg_zmax)
    arg_error("graph3d() needs 7 arguments (expr,xmin,xmax,ymin,ymax,zmin,zmax).");

  xmin = strtod(arg_xmin,NULL);
  xmax = strtod(arg_xmax,NULL);
  ymin = strtod(arg_ymin,NULL);
  ymax = strtod(arg_ymax,NULL);
  zmin = strtod(arg_zmin,NULL);
  zmax = strtod(arg_zmax,NULL);

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
  PLFLT **a= malloc(sizeof(PLFLT *)*HC_GRAPH_POINTS_3D);
  for (i=0; i<HC_GRAPH_POINTS_3D; i++)
    a[i] = malloc(sizeof(PLFLT)*HC_GRAPH_POINTS_3D);
  PLFLT *a_x = malloc(sizeof(PLFLT)*HC_GRAPH_POINTS_3D);
  PLFLT *a_y = malloc(sizeof(PLFLT)*HC_GRAPH_POINTS_3D);
  char *a_hasval = malloc(sizeof(char)*HC_GRAPH_POINTS_3D*HC_GRAPH_POINTS_3D);
  double stepx = fabs(xmax-xmin) / (HC_GRAPH_POINTS_3D - 1);
  double stepy = fabs(ymax-ymin) / (HC_GRAPH_POINTS_3D - 1);
  double curx = xmin;
  double cury = ymin;

  graphing_ignore_errors = TRUE;
  unsigned int ii = 0;
  for (i=0; i<HC_GRAPH_POINTS_3D; i++,curx+=stepx)
  {
    cury = ymin;
    for (ii=0; ii<HC_GRAPH_POINTS_3D; ii++,cury+=stepy)
    {
      char tmp_curx[256],tmp_cury[256];
      sprintf(tmp_curx,"%f",curx);
      sprintf(tmp_cury,"%f",cury);
      a_x[i] = strtod(tmp_curx,NULL);
      a_y[ii] = strtod(tmp_cury,NULL);
      char *tmp_expr = strreplace(func_expr,"x",tmp_curx);
      char *tmp_expr2 = strreplace(tmp_expr,"y",tmp_cury);
      free(tmp_expr);
      tmp_expr = tmp_expr2;
      char *tmp_2 = hc_result_(tmp_expr);
      if (tmp_2)
      {
	a_hasval[i] = 'y';
	a[i][ii] = strtod(tmp_2,NULL);
      } else {
	printf("FIX FIX FIX (setting to 0)\n");
	a[i][ii] = 0;
      }
      free(tmp_expr);
      if (tmp_2)
	free(tmp_2);
    }
  }
  graphing_ignore_errors = FALSE;

#ifndef HCG
  if (!hc.plplot_dev_override)
#ifndef WIN32
    plsdev("pngcairo");
#else
    plsdev("wingcc");
#endif
#else
#ifndef WIN32
  plsdev("pngcairo");
  plsfnam("tmp-graph.png");
#else
  plsdev("wingcc");
#endif
#endif
  plinit();
  plcol0(15);
  pladv(0);
  plvpor(0.0,1.0,0.0,0.9);
  plwind(-1.0,1.0,-1.0,1.5);
  plw3d(1.0,1.0,1.0,xmin,xmax,ymin,ymax,zmin,zmax,33,24);
  char *graph_top_label = malloc(strlen("HoubySoft Calculator - Graph - ")+strlen(func_expr)+1);
  if (!graph_top_label)
    mem_error();
  strcpy(graph_top_label,"HoubySoft Calculator - Graph - ");
  strcat(graph_top_label,func_expr);
  plmtex("t",0.5,0.5,0.5,graph_top_label);
  plbox3("bnstu","x",0,0,"bnstu","y",0,0,"bcdmnstuv","z",0,0);
  plcol0(0);
  cmap1_init();
  plmesh(a_x,a_y,a,HC_GRAPH_POINTS_3D,HC_GRAPH_POINTS_3D,DRAW_LINEXY | MAG_COLOR);
  plend();

  free(graph_top_label);
  free(func_expr);
  free(arg_xmin);
  free(arg_xmax);
  free(a_x);
  free(a);
  free(a_hasval);

#ifdef HCG
#ifndef WIN32
  hcg_disp_graph("tmp-graph.png");
  remove("tmp-graph.png");
#endif
#endif
  
  return SUCCESS; 
}
