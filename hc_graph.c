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
#include <string.h>
#include <stdlib.h>
#include <plplot/plplot.h>
#include <errno.h>
#include "hc.h"
#include "hc_functions.h"
#include "hc_graph.h"
#include "hc_complex.h"


#define HC_GRAPH_POINTS 100
#define HC_GRAPH_POINTS_3D (hc.graph_points_3d)


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
  free(t1);free(t3);
  free(t2);free(t4);
  double xmin,xmax,ymin,ymax;

  if (!func_expr)
    arg_error("graph() needs at least one argument (expr).");

  if (!arg_xmin || !arg_xmax || !arg_ymin || !arg_ymax)
  {
    if (arg_xmin || arg_xmax || arg_ymin || arg_ymax)
      notify("You haven't provided all of xmin, xmax, ymin and ymax correctly. Using defaults.\n");
    xmin = hc.xmin2d;
    xmax = hc.xmax2d;
    ymin = hc.ymin2d;
    ymax = hc.ymax2d;
  } else {
    hc.xmin2d = xmin = strtod(arg_xmin,NULL);
    hc.xmax2d = xmax = strtod(arg_xmax,NULL);
    hc.ymin2d = ymin = strtod(arg_ymin,NULL);
    hc.ymax2d = ymax = strtod(arg_ymax,NULL);
  }

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
    char *tmp_3 = hc_imag_part(tmp_2);
    if (!tmp_3)
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
    if (tmp_3)
      free(tmp_3);
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
  if (arg_xmin)
    free(arg_xmin);
  if (arg_xmax)
    free(arg_xmax);
  if (arg_ymin)
    free(arg_ymin);
  if (arg_ymax)
    free(arg_ymax);
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



// 2D graphs of more than one function
// Arguments : func_expr_1, func_expr_2, ..., func_expr_n
char hc_graph_n(char *e)
{
  char **func_expr = malloc(sizeof(char *)*HC_GRAPH_N_MAX);
  if (!func_expr)
    mem_error();
  unsigned int j=0;
  while ((func_expr[j] = hc_get_arg(e,j+1))!=NULL)
  {
    j++;
  }

  if (!*func_expr[0])
    arg_error("gmul() needs at least one argument (expr_1).");

  double xmin = hc.xmin2d;
  double xmax = hc.xmax2d;
  double ymin = hc.ymin2d;
  double ymax = hc.ymax2d;

  unsigned int i = 0;
  PLFLT **a= malloc(sizeof(PLFLT *)*j);
  PLFLT **a_x = malloc(sizeof(PLFLT *)*j);
  char **a_hasval = malloc(sizeof(char *)*j);
  for (i=0; i<j; i++)
  {
    a[i] = malloc(sizeof(PLFLT)*HC_GRAPH_POINTS);
    a_x[i] = malloc(sizeof(PLFLT)*HC_GRAPH_POINTS);
    a_hasval[i] = malloc(sizeof(char)*HC_GRAPH_POINTS);
  }
  double step = fabs(xmax-xmin) / (HC_GRAPH_POINTS - 1);
  double curx = xmin;

  graphing_ignore_errors = TRUE;

  unsigned int k=j;

  for (j=0; j<k; j++)
  {
    curx = xmin;
    for (i=0; i<HC_GRAPH_POINTS; i++,curx+=step)
    {
      char tmp_curx[256];
      sprintf(tmp_curx,"%f",curx);
      a_x[j][i] = strtod(tmp_curx,NULL);
      char *tmp_expr = strreplace(func_expr[j],"x",tmp_curx);
      char *tmp_2 = hc_result_(tmp_expr);
      char *tmp_3 = hc_imag_part(tmp_2);
      if (!tmp_3)
      {
	a_hasval[j][i] = 'y';
	a[j][i] = strtod(tmp_2,NULL);
      } else {
	if (i!=0)
	  a[j][i] = a[j][i-1];
	else
	  a[j][i] = 0;
	a_hasval[j][i] = 'n';
      }
      free(tmp_expr);
      if (tmp_2)
	free(tmp_2);
      if (tmp_3)
	free(tmp_3);
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
  plenv(xmin,xmax,ymin,ymax,0,1);
  int mallocme = strlen("HC - ")+1;
  for (j=0; j<k; j++)
  {
    mallocme += strlen(func_expr[j]);
    if (j<k-1)
      mallocme += strlen("; ");
  }
  char *lbl = malloc(mallocme);
  if (!lbl)
    mem_error();
  strcpy(lbl,"HC - ");
  for (j=0; j<k; j++)
  {
    strcat(lbl,func_expr[j]);
    if (j<k-1)
      strcat(lbl,"; ");
  }
  pllab("x","y",lbl);
  free(lbl);
  int color=1;
  for (j=0; j<k; j++)
  {
    plcol0(color);
    color++;
    if (color==15) // 15 is white, 0 is black, 1 is red, details on page 131/153 of plplot-5.9.5.pdf (get it on plplot.sf.net)
      color = 1;
    for (i=0; i<HC_GRAPH_POINTS-1; i++)
    {
      if (a_hasval[j][i]=='y' && a_hasval[j][i+1]=='y') // discontinuity check
	pljoin(a_x[j][i],a[j][i],a_x[j][i+1],a[j][i+1]);
    }
  }
  plend();

  for (j=0; j<k; j++)
  {
    free(a_x[j]);
    free(a[j]);
    free(a_hasval[j]);
    free(func_expr[j]);
  }
  free(a_x);
  free(a);
  free(a_hasval);
  free(func_expr);
    
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
  free(t1); free(t2); free(t3);
  free(t4); free(t5); free(t6);
  double xmin,xmax,ymin,ymax,zmin,zmax;

  if (!func_expr)
    arg_error("graph3() needs at least one argument (expr).");

  if (!arg_xmin || !arg_xmax || !arg_ymin || !arg_ymax || !arg_zmin || !arg_zmax)
  {
    if (arg_xmin || arg_xmax || arg_ymin || arg_ymax || arg_zmin || arg_zmax)
      notify("You haven't provided all of xmin, xmax, ymin and ymax correctly. Using defaults.\n");
    xmin = hc.xmin3d;
    xmax = hc.xmax3d;
    ymin = hc.ymin3d;
    ymax = hc.ymax3d;
    zmin = hc.zmin3d;
    zmax = hc.zmax3d;
  } else {
    hc.xmin3d = xmin = strtod(arg_xmin,NULL);
    hc.xmax3d = xmax = strtod(arg_xmax,NULL);
    hc.ymin3d = ymin = strtod(arg_ymin,NULL);
    hc.ymax3d = ymax = strtod(arg_ymax,NULL);
    hc.zmin3d = zmin = strtod(arg_zmin,NULL);
    hc.zmax3d = zmax = strtod(arg_zmax,NULL);
  }

  unsigned int i = 0, j = 0;
  PLFLT **a= malloc(sizeof(PLFLT *)*HC_GRAPH_POINTS_3D);
  char **a_hasval = malloc(sizeof(char *)*HC_GRAPH_POINTS_3D);
  for (i=0; i<HC_GRAPH_POINTS_3D; i++)
  {
    a[i] = malloc(sizeof(PLFLT)*HC_GRAPH_POINTS_3D);
    a_hasval[i] = malloc(sizeof(char)*HC_GRAPH_POINTS_3D);
  }
  PLFLT *a_x = malloc(sizeof(PLFLT)*HC_GRAPH_POINTS_3D);
  PLFLT *a_y = malloc(sizeof(PLFLT)*HC_GRAPH_POINTS_3D);
  double stepx = fabs(xmax-xmin) / (HC_GRAPH_POINTS_3D - 1);
  double stepy = fabs(ymax-ymin) / (HC_GRAPH_POINTS_3D - 1);
  double curx = xmin;
  double cury = ymin;
  char discont = FALSE;

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
      char *tmp_3 = hc_imag_part(tmp_2);
      if (!tmp_3)
      {
	a[i][ii] = strtod(tmp_2,NULL);
	if (a[i][ii]<zmin || a[i][ii]>zmax || errno==ERANGE)
	{
	  a_hasval[i][ii] = 'n';
	  discont = TRUE;
	} else {
	  a_hasval[i][ii] = 'y';
	}
      } else {
	a_hasval[i][ii] = 'n';
	discont = TRUE; // we have at least one discontinuity/absence of value - this will switch to the slow-but-safe plotting mode
      }
      free(tmp_expr);
      if (tmp_2)
	free(tmp_2);
      if (tmp_3)
	free(tmp_3);
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
  plvpor(0,1.0,-0.2,0.9);
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
  if (!discont)
    plmesh(a_x,a_y,a,HC_GRAPH_POINTS_3D,HC_GRAPH_POINTS_3D,DRAW_LINEXY | MAG_COLOR);
  else
  {
    // Discontinuity present
    PLFLT **a1 = malloc(sizeof(PLFLT *)*2);
    a1[0] = malloc(sizeof(PLFLT)*2);
    a1[1] = malloc(sizeof(PLFLT)*2);
    PLFLT *a_x1 = malloc(sizeof(PLFLT)*2);
    PLFLT *a_y1 = malloc(sizeof(PLFLT)*2);
    // Check 2x2 squares, if all have a value, plot them, otherwise skip
    for (i=0; i<HC_GRAPH_POINTS_3D-1; i++)
    {
      for (j=0; j<HC_GRAPH_POINTS_3D-1; j++)
      {
	if (a_hasval[i][j]=='y' && a_hasval[i+1][j]=='y' && a_hasval[i+1][j+1]=='y' && a_hasval[i][j+1]=='y')
	{
	  // All points in the square have a value, plot them!
	  a_x1[0] = a_x[i];
	  a_x1[1] = a_x[i+1];
	  a_y1[0] = a_y[j];
	  a_y1[1] = a_y[j+1];
	  a1[0][0] = a[i][j];
	  a1[0][1] = a[i][j+1];
	  a1[1][0] = a[i+1][j];
	  a1[1][1] = a[i+1][j+1];
	  plmesh(a_x1,a_y1,a1,2,2,DRAW_LINEXY | MAG_COLOR);
	}
      }
    }
    free(a_x1);
    free(a_y1);
    free(a1[0]);
    free(a1[1]);
    free(a1);
  }
  plend();

  free(graph_top_label);
  free(func_expr);
  if (arg_xmin)
    free(arg_xmin);
  if (arg_ymin)
    free(arg_ymin);
  if (arg_zmin)
    free(arg_zmin);
  if (arg_xmax)
    free(arg_xmax);
  if (arg_ymax)
    free(arg_ymax);
  if (arg_zmax)
    free(arg_zmax);
  free(a_x);
  free(a_y);
  for (i=0; i<HC_GRAPH_POINTS_3D; i++)
  {
    free(a[i]);
    free(a_hasval[i]);
  }
  free(a);

#ifdef HCG
#ifndef WIN32
  hcg_disp_graph("tmp-graph.png");
  remove("tmp-graph.png");
#endif
#endif
  
  return SUCCESS; 
}
