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
#include <string.h>
#include <stdlib.h>
#include <plplot/plplot.h>
#include <errno.h>
#include <math.h>
#include "hc.h"
#include "hc_functions.h"
#include "hc_graph.h"
#include "hc_complex.h"
#include "hc_list.h"
#include "hc_utils.h"
#include "hc_info.h"


#define HC_GRAPH_POINTS 300
#define HC_GRAPH_POINTS_3D (hc.graph_points_3d)
#define HC_GRAPH_POINTS_SF 25
#define HC_GRAPH_PEQ_T_STEP (hc.peqstep)


#ifdef MEM_DRIVER
void *driver_memory = NULL;
#endif



const unsigned int pl_colors[][3] = {
  {255, 0,   0  }, // red
  {255, 255, 0  }, // yellow
  {0,   255, 0  }, // green
  {127, 255, 212}, // aquamarine
  {255, 105, 180}, // pink
  {245, 222, 179}, // wheat
  {190, 190, 190}, // gray
  {160, 82,  45 }, // brown
  {0,   0,   255}, // blue
  {138, 43,  226}, // blue violet
  {0,   255, 255}, // cyan
  {64,  224, 208}, // turquoise
  {255, 0,   255}, // magenta
  {250, 128, 114}  // salmon
};


void plfbox(PLFLT x, PLFLT y25, PLFLT y50, PLFLT y75, PLFLT lw, PLFLT uw);
char hc_graph_peq_core(char *func_exprx, char *func_expry, double tmin, double tmax, double xmin, double xmax, double ymin, double ymax, char *graph_top_label, char *graph_x_label, char *graph_y_label, int type, char *exprtmp);


void hc_graph_init()
{
  plspage(0, 0, GRAPH_PIXELS_X, GRAPH_PIXELS_Y, 0, 0);
#if defined(MEM_DRIVER)
  plsdev("mem");
  free(driver_memory);
  driver_memory = malloc(GRAPH_PIXELS_Y * GRAPH_PIXELS_X * 3); // 3 for RGB
  if (!driver_memory) mem_error();
  memset(driver_memory, hc.bgcolor == WHITE ? 255 : 0, GRAPH_PIXELS_Y * GRAPH_PIXELS_X * 3); // set all pixels to background color
  plsmem(GRAPH_PIXELS_X, GRAPH_PIXELS_Y, driver_memory);
#elif defined(HCG) && defined(WIN32)
  plsdev("wingcc");
#elif defined(HCG) && !defined(WIN32)
  plsdev("pngcairo"); plsfnam("tmp-graph.png");
#elif !defined(HCG) && defined(WIN32)
  if (!hc.plplot_dev_override)
    plsdev("wingcc");
#elif !defined(HCG) && !defined(WIN32)
  if (!hc.plplot_dev_override)
    plsdev("pngcairo");
#endif

  if (hc.bgcolor == WHITE)
    plscol0(0,255,255,255);
  else
    plscol0(0,0,0,0);

  unsigned int i = 1;
  for (; i <= PL_COLORS_LEN; i++)
    plscol0(i, pl_colors[i-1][0], pl_colors[i-1][1], pl_colors[i-1][2]);

  if (hc.bgcolor == WHITE)
    plscol0(15,0,0,0);
  else
    plscol0(15,255,255,255);
  plinit();
  plcol0(15);
}


// Taken from example 11, to setup the color palette for 3D graphs
static void cmap1_init()
{
  PLFLT i[2], h[2], l[2], s[2];

  i[0] = 0.0;                /* left boundary */
  i[1] = 1.0;                /* right boundary */

  h[0] = 240; /* blue -> green -> yellow -> */
  h[1] = 0;   /* -> red */

  l[0] = 0.6;
  l[1] = 0.6;

  s[0] = 0.8;
  s[1] = 0.8;

  plscmap1n(256);
  c_plscmap1l(0, 2, i, h, l, s, NULL);
}


void hc_graph_init2d(char *label_top, char *label_x, char *label_y, double xmin, double xmax, double ymin, double ymax)
{
  hc_graph_init();
  plenv(xmin,xmax,ymin,ymax,0,1);
  pllab(label_x,label_y,label_top);
  plcol0(1);
}


void hc_graph_init3d(char *label_top, char *label_x, char *label_y, char *label_z, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
{
  hc_graph_init();
  pladv(0);
  plvpor(0,1.0,-0.2,0.9);
  plwind(-1.0,1.0,-1.0,1.5);
  plw3d(1.0,1.0,1.0,xmin,xmax,ymin,ymax,zmin,zmax,33,24);
  plmtex("t",0.5,0.5,0.5,label_top);
  plbox3("bnstu",label_x,0,0,"bnstu",label_y,0,0,"bcdmnstuv",label_z,0,0);
  plcol0(0);
  cmap1_init();
}


void hc_graph_boxplot(char *label_top, char *label_x, char *label_y, char *args, double min, double max, double q1, double q2, double q3)
{
  hc_graph_init();
  pladv(0);
  plvsta();

  plwind(-2, 5, min - (max - min) * 0.1, max + (max - min) * 0.1);
  plcol0(15);
  plbox("bc", 1.0, 0, "bcgnst", 0, 0);
  pllab(label_x ? label_x : "", label_y ? label_y : "", label_top ? label_top : "HC - Box plot");
  plcol0(9);
  plfbox(1, q1, q2, q3, min, max);

  hc_graph_finish(HCGT_BOXPLOT, args, label_top, label_x, label_y, NULL);
}


void hc_graph_finish(int type, char *args, char *tl, char *xl, char *yl, char *zl)
{
  plend();
#if defined(HCG) && !defined(WIN32) && !defined(MEM_DRIVER)
  hcg_disp_graph("tmp-graph.png");
  remove("tmp-graph.png");
#elif defined(MEM_DRIVER)
  hcg_disp_rgb(GRAPH_PIXELS_X, GRAPH_PIXELS_Y, driver_memory, type, args, tl, xl, yl, zl);
  free(driver_memory);
  driver_memory = NULL;
#endif
}


void plfbox(PLFLT x, PLFLT y25, PLFLT y50, PLFLT y75, PLFLT lw, PLFLT uw)
{
    PLFLT px[5], py[5], mx[2], my[2], wx[2], wy[2], barx[2], bary[2];
    PLFLT spacing;
    PLFLT xmin, xmax;
    PLFLT xmid, xwidth;

    spacing = .4; /* in x axis units */

    xmin = x + spacing / 2.;
    xmax = x + 1. - spacing / 2.;

    /* box */
    
    px[0] = xmin;
    py[0] = y25;
    px[1] = xmin;
    py[1] = y75;
    px[2] = xmax;
    py[2] = y75;
    px[3] = xmax;
    py[3] = y25;
    px[4] = xmin;
    py[4] = y25;

    plpsty(0);
    plfill(4, px, py);
    plcol0(1);
    pllsty(1);
    plline(5, px, py);


    /* median */

    mx[0] = xmin;
    my[0] = y50;
    mx[1] = xmax;
    my[1] = y50;

    pllsty(1);
    plline(2, mx, my);
    
    /* lower whisker */

    xmid = (xmin + xmax) / 2.;
    xwidth = xmax - xmin;
    wx[0] = xmid;
    wy[0] = lw;
    wx[1] = xmid;
    wy[1] = y25;
   
    pllsty(2); /* short dashes and gaps */
    plline(2, wx, wy);
    
    barx[0] = xmid - xwidth / 4.;
    bary[0] = lw;
    barx[1] = xmid + xwidth / 4.;
    bary[1] = lw;

    pllsty(1);
    plline(2, barx, bary);

    /* upper whisker */
    
    xmid = (xmin + xmax) / 2.;
    xwidth = xmax - xmin;
    wy[0] = y75;
    wy[1] = uw;
   
    pllsty(2); /* short dashes and gaps */
    plline(2, wx, wy);
    
    bary[0] = uw;
    bary[1] = uw;

    pllsty(1);
    plline(2, barx, bary);
}


#define hc_graph_line2d(x1,y1,x2,y2) pljoin(x1,y1,x2,y2)
#define hc_graph_mesh(x,y,z,nx,ny) plmesh(x,y,z,nx,ny,DRAW_LINEXY | MAG_COLOR)
#define hc_graph_point(x,y) plpoin(1,x,y,2) // last argument is a code for the character to use
#define hc_graph_set_color(color) plcol0(color)


// Draw a defined function
// Arguments : func_name, xmin, xmax, ymin, ymax
char hc_graph(char *e)
{
  char *func_expr,*arg_xmin,*arg_xmax,*arg_ymin,*arg_ymax,*arg_label_top,*arg_label_x,*arg_label_y;
  func_expr = hc_get_arg(e,1);
  arg_xmin = hc_get_arg_r(e,2);
  arg_xmax = hc_get_arg_r(e,3);
  arg_ymin = hc_get_arg_r(e,4);
  arg_ymax = hc_get_arg_r(e,5);
  arg_label_top = hc_get_arg_r(e,6);
  arg_label_x = hc_get_arg_r(e,7);
  arg_label_y = hc_get_arg_r(e,8);

  double xmin,xmax,ymin,ymax;

  if (!func_expr || !strlen(func_expr))
  {
    free(func_expr); free(arg_xmin); free(arg_xmax); free(arg_ymin); free(arg_ymax); free(arg_label_top); free(arg_label_x); free(arg_label_y);
    arg_error("graph() needs at least one argument (expr).");
  }

  if (!arg_xmin || !arg_xmax || !arg_ymin || !arg_ymax)
  {
    if (arg_xmin || arg_xmax || arg_ymin || arg_ymax)
      notify("You haven't provided all of xmin, xmax, ymin and ymax correctly. Using defaults.\n");
    xmin = hc.xmin2d[hcgt_get_idx(HCGT_2D)];
    xmax = hc.xmax2d[hcgt_get_idx(HCGT_2D)];
    ymin = hc.ymin2d[hcgt_get_idx(HCGT_2D)];
    ymax = hc.ymax2d[hcgt_get_idx(HCGT_2D)];
  } else {
    hc.xmin2d[hcgt_get_idx(HCGT_2D)] = xmin = strtod(arg_xmin,NULL);
    hc.xmax2d[hcgt_get_idx(HCGT_2D)] = xmax = strtod(arg_xmax,NULL);
    hc.ymin2d[hcgt_get_idx(HCGT_2D)] = ymin = strtod(arg_ymin,NULL);
    hc.ymax2d[hcgt_get_idx(HCGT_2D)] = ymax = strtod(arg_ymax,NULL);
  }

  if (xmin == xmax)
  {
    free(func_expr); free(arg_xmin); free(arg_xmax); free(arg_ymin); free(arg_ymax); free(arg_label_top); free(arg_label_x); free(arg_label_y);
    arg_error("graph() : xmin and xmax are set to the same value");
  }

  PROCESS_LABELS_2D();

  if (is_vector(func_expr))
  {
    hc_graph_n(list_clean(func_expr), arg_label_top, arg_label_x, arg_label_y);
  } else {
    hc_graph_n(func_expr, arg_label_top, arg_label_x, arg_label_y);
  }
  free(func_expr);
  free(arg_xmin);
  free(arg_xmax);
  free(arg_ymin);
  free(arg_ymax);
  free(arg_label_top);
  free(arg_label_x);
  free(arg_label_y);
  return SUCCESS;
}


// Calculate minimum precision required to draw the graph correctly.
// This uses GRAPH_PIXELS_Y to determine the lowest possible precision
// that will still be undistinguishable from higher precision due to the
// number of available pixels. If the computed precision is higher than the
// current precision, the current precision is kept.
unsigned int hc_graph_precision2d(double ymin, double ymax) {
  int ymax_exponent = (int)floor(log10(fabs(ymax)));
  int res_exponent = (int)floor(log10(fabs(ymax - ymin) / (double)GRAPH_PIXELS_Y));

  unsigned int new_precision;
  if (ymax_exponent > 0)
    new_precision = abs(res_exponent) + abs(ymax_exponent);
  else
    new_precision = abs(res_exponent);

  if (new_precision > hc.precision)
    return hc.precision;
  else if (new_precision <= 1)
    return 2;
  else
    return new_precision;
}


// 2D graphs of more than one function
// Arguments : func_expr_1, func_expr_2, ..., func_expr_n
// Labels can either be NULL to use defaults, or a string
char hc_graph_n(char *e, char *label_top, char *label_x, char *label_y)
{
  char **func_expr = malloc(sizeof(char *)*HC_GRAPH_N_MAX);
  if (!func_expr)
    mem_error();
  unsigned int j=0, arg_idx=0;
  while ((func_expr[j] = hc_get_arg(e,arg_idx+1))!=NULL)
  {
    j++;
    arg_idx++;
    if (hc.graph_show_imag) {
      func_expr[j] = malloc(strlen("im()") + strlen(func_expr[j-1]) + 1);
      if (!func_expr[j]) mem_error();
      sprintf(func_expr[j], "im(%s)", func_expr[j-1]);
      func_expr[j-1] = realloc(func_expr[j-1], strlen("re()") + strlen(func_expr[j-1]) + 1);
      char *tmp = malloc(strlen("re()") + strlen(func_expr[j-1]) + 1);
      if (!func_expr[j-1]) mem_error(); if (!tmp) mem_error();
      sprintf(tmp, "re(%s)", func_expr[j-1]);
      strcpy(func_expr[j-1], tmp);
      free(tmp);
      j++;
    }
    // HC_GRAPH_N_MAX has to be pair, so it's ok to check only here
    if (j >= HC_GRAPH_N_MAX) {
      unsigned int k=0;
      for (; k < j; k++)
	free(func_expr[k]);
      free(func_expr);
      hc_error(ARG, "hc_graph_n() : error : cannot graph more than %i functions on one graph, please reduce the number of functions.", HC_GRAPH_N_MAX);
      return FAIL;
    }
  }

  if (!*func_expr[0])
    arg_error("gmul() and graph() need at least one argument (expr_1).");

  double xmin = hc.xmin2d[hcgt_get_idx(HCGT_2D)];
  double xmax = hc.xmax2d[hcgt_get_idx(HCGT_2D)];
  double ymin = hc.ymin2d[hcgt_get_idx(HCGT_2D)];
  double ymax = hc.ymax2d[hcgt_get_idx(HCGT_2D)];

  double step = (xmax-xmin) / HC_GRAPH_POINTS;
  double curx = xmin;

  graphing_ignore_errors = TRUE;
  unsigned int old_precision = hc.precision;
  hc.precision = hc_graph_precision2d(ymin, ymax);

  unsigned int k=j;
  double x1=0,y1=0,x2=0,y2=0;
  char discont = 1;

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
  hc_graph_init2d(label_top == NULL ? lbl : label_top, label_x == NULL ? "x" : label_x, label_y == NULL ? "y" : label_y, xmin, xmax, ymin, ymax);
  int color=1;

  for (j=0; j<k; j++)
  {
    curx = xmin;
    discont = 1;
    hc_graph_set_color(color);
    color++;
    if (color==15) // 15 is white, 0 is black, 1 is red, details on page 131/153 of plplot-5.9.5.pdf (get it on plplot.sf.net)
      color = 1;
    for (; curx<=xmax; curx+=step)
    {
      char tmp_curx[256];
      sprintf(tmp_curx,"%f",curx);
      char *tmp_expr = strreplace(func_expr[j],"x",tmp_curx);
      char *tmp_2 = hc_result_(tmp_expr);
      char *tmp_3 = NULL;
      if (tmp_2)
        tmp_3 = hc_imag_part(tmp_2);
      if (!tmp_3 && tmp_2 && strlen(tmp_2))
      {
        if (discont)
        {
          x1 = curx;
          y1 = strtod(tmp_2,NULL);
          discont = 0;
        } else {
          x2 = curx;
          y2 = strtod(tmp_2,NULL);
          hc_graph_line2d(x1,y1,x2,y2);
          x1 = x2;
          y1 = y2;
        }
      } else {
        discont = 1;
      }
      free(tmp_expr);
      if (tmp_2)
        free(tmp_2);
      if (tmp_3)
        free(tmp_3);
    }
  }
  graphing_ignore_errors = FALSE;
  hc.precision = old_precision;

  for (j=0; j<k; j++)
  {
    free(func_expr[j]);
  }
  free(func_expr);

  hc_graph_finish(HCGT_2D, e, label_top, label_x, label_y, NULL);

  free(lbl);
  return SUCCESS;
}



char hc_graph3d(char *e)
{
  char *func_expr,*arg_xmin,*arg_xmax,*arg_ymin,*arg_ymax,*arg_zmin,*arg_zmax,*arg_label_top,*arg_label_x,*arg_label_y,*arg_label_z;
  func_expr = hc_get_arg(e,1);
  arg_xmin = hc_get_arg_r(e,2);
  arg_xmax = hc_get_arg_r(e,3);
  arg_ymin = hc_get_arg_r(e,4);
  arg_ymax = hc_get_arg_r(e,5);
  arg_zmin = hc_get_arg_r(e,6);
  arg_zmax = hc_get_arg_r(e,7);
  arg_label_top = hc_get_arg_r(e,8);
  arg_label_x = hc_get_arg_r(e,9);
  arg_label_y = hc_get_arg_r(e,10);
  arg_label_z = hc_get_arg_r(e,11);
  double xmin,xmax,ymin,ymax,zmin,zmax;

  if (!func_expr)
  {
    free(arg_xmin); free(arg_xmax); free(arg_ymin); free(arg_ymax); free(arg_zmin); free(arg_zmax); free(arg_label_top); free(arg_label_x); free(arg_label_y); free(arg_label_z);
    arg_error("graph3() needs at least one argument (expr).");
  }

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
  if (xmin == xmax || ymin == ymax)
  {
    free(arg_xmin); free(arg_xmax); free(arg_ymin); free(arg_ymax); free(arg_zmin); free(arg_zmax); free(arg_label_top); free(arg_label_x); free(arg_label_y); free(arg_label_z);
    arg_error("graph3() : xmin and xmax or ymin and ymax are set to the same value");
  }

  unsigned int i = 0, j = 0;
  double **a= malloc(sizeof(double *)*HC_GRAPH_POINTS_3D);
  char **a_hasval = malloc(sizeof(char *)*HC_GRAPH_POINTS_3D);
  for (i=0; i<HC_GRAPH_POINTS_3D; i++)
  {
    a[i] = malloc(sizeof(double)*HC_GRAPH_POINTS_3D);
    a_hasval[i] = malloc(sizeof(char)*HC_GRAPH_POINTS_3D);
  }
  double *a_x = malloc(sizeof(double)*HC_GRAPH_POINTS_3D);
  double *a_y = malloc(sizeof(double)*HC_GRAPH_POINTS_3D);
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
      char *tmp_3 = NULL;
      if (tmp_2)
        tmp_3 = hc_imag_part(tmp_2);
      if (!tmp_3 && tmp_2)
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

  PROCESS_LABELS_3D();

  char *graph_top_label = malloc(strlen("HoubySoft Calculator - Graph - ")+strlen(func_expr)+1);
  if (!graph_top_label)
    mem_error();
  strcpy(graph_top_label,"HoubySoft Calculator - Graph - ");
  strcat(graph_top_label,func_expr);
  hc_graph_init3d(arg_label_top ? arg_label_top : graph_top_label, arg_label_x ? arg_label_x : "x", arg_label_y ? arg_label_y : "y", arg_label_z ? arg_label_z : "z", xmin, xmax, ymin, ymax, zmin, zmax);
  if (!discont)
    hc_graph_mesh(a_x,a_y,a,HC_GRAPH_POINTS_3D,HC_GRAPH_POINTS_3D);
  else
  {
    // Discontinuity present
    double **a1 = malloc(sizeof(double *)*2);
    a1[0] = malloc(sizeof(double)*2);
    a1[1] = malloc(sizeof(double)*2);
    double *a_x1 = malloc(sizeof(double)*2);
    double *a_y1 = malloc(sizeof(double)*2);
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
          hc_graph_mesh(a_x1, a_y1, a1, 2, 2);
        }
      }
    }
    free(a_x1);
    free(a_y1);
    free(a1[0]);
    free(a1[1]);
    free(a1);
  }
  free(arg_xmin);
  free(arg_ymin);
  free(arg_zmin);
  free(arg_xmax);
  free(arg_ymax);
  free(arg_zmax);
  free(a_x);
  free(a_y);
  for (i=0; i<HC_GRAPH_POINTS_3D; i++)
  {
    free(a[i]);
    free(a_hasval[i]);
  }
  free(a);

  hc_graph_finish(HCGT_3D, func_expr, arg_label_top, arg_label_x, arg_label_y, arg_label_z);
  free(arg_label_top); free(arg_label_x); free(arg_label_y); free(arg_label_z);
  free(graph_top_label);
  free(func_expr);

  return SUCCESS; 
}


char hc_graph_slpfld(char *e)
{
  char *func_expr,*arg_xmin,*arg_xmax,*arg_ymin,*arg_ymax,*arg_label_top,*arg_label_x,*arg_label_y;
  func_expr = hc_get_arg(e,1);
  arg_xmin = hc_get_arg_r(e,2);
  arg_xmax = hc_get_arg_r(e,3);
  arg_ymin = hc_get_arg_r(e,4);
  arg_ymax = hc_get_arg_r(e,5);
  arg_label_top = hc_get_arg_r(e,6);
  arg_label_x = hc_get_arg_r(e,7);
  arg_label_y = hc_get_arg_r(e,8);
  double xmin,xmax,ymin,ymax;

  if (!func_expr)
    arg_error("slpfld() needs at least one argument (expr).");

  if (!arg_xmin || !arg_xmax || !arg_ymin || !arg_ymax)
  {
    if (arg_xmin || arg_xmax || arg_ymin || arg_ymax)
      notify("You haven't provided all of xmin, xmax, ymin and ymax correctly. Using defaults.\n");
    xmin = hc.xmin2d[hcgt_get_idx(HCGT_SLPFLD)];
    xmax = hc.xmax2d[hcgt_get_idx(HCGT_SLPFLD)];
    ymin = hc.ymin2d[hcgt_get_idx(HCGT_SLPFLD)];
    ymax = hc.ymax2d[hcgt_get_idx(HCGT_SLPFLD)];
  } else {
    hc.xmin2d[hcgt_get_idx(HCGT_SLPFLD)] = xmin = strtod(arg_xmin,NULL);
    hc.xmax2d[hcgt_get_idx(HCGT_SLPFLD)] = xmax = strtod(arg_xmax,NULL);
    hc.ymin2d[hcgt_get_idx(HCGT_SLPFLD)] = ymin = strtod(arg_ymin,NULL);
    hc.ymax2d[hcgt_get_idx(HCGT_SLPFLD)] = ymax = strtod(arg_ymax,NULL);
  }

  unsigned int i = 0;
  double **a= malloc(sizeof(double *)*HC_GRAPH_POINTS_SF);
  char **a_hasval = malloc(sizeof(char *)*HC_GRAPH_POINTS_SF);
  for (i=0; i<HC_GRAPH_POINTS_SF; i++)
  {
    a[i] = malloc(sizeof(double)*HC_GRAPH_POINTS_SF);
    a_hasval[i] = malloc(sizeof(char)*HC_GRAPH_POINTS_SF);
  }
  double *a_x = malloc(sizeof(double)*HC_GRAPH_POINTS_SF);
  double *a_y = malloc(sizeof(double)*HC_GRAPH_POINTS_SF);
  double r_x = ((xmax-xmin) / (0.5*(HC_GRAPH_POINTS_SF+1) + 2*(HC_GRAPH_POINTS_SF)));
  double r_y = ((ymax-ymin) / (0.5*(HC_GRAPH_POINTS_SF+1) + 2*(HC_GRAPH_POINTS_SF)));
  double stepx = (5*r_x) / 2;
  double stepy = (5*r_y) / 2;
  double curx = xmin + (3*r_x)/2;
  double cury = ymin + (3*r_y)/2;

  graphing_ignore_errors = TRUE;
  unsigned int ii = 0;
  for (i=0; i<HC_GRAPH_POINTS_SF; i++,curx+=stepx)
  {
    cury = ymin + (3*r_y)/2;
    for (ii=0; ii<HC_GRAPH_POINTS_SF; ii++,cury+=stepy)
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
      char *tmp_3 = NULL;
      if (tmp_2)
        tmp_3 = hc_imag_part(tmp_2);
      if (!tmp_3 && tmp_2)
      {
        a[i][ii] = strtod(tmp_2,NULL);
        if (errno==ERANGE)
        {
          a_hasval[i][ii] = 'n';
        } else {
          a_hasval[i][ii] = 'y';
        }
      } else {
        a_hasval[i][ii] = 'n';
      }
      free(tmp_expr);
      if (tmp_2)
        free(tmp_2);
      if (tmp_3)
        free(tmp_3);
    }
  }
  graphing_ignore_errors = FALSE;

  PROCESS_LABELS_2D();

  char *graph_top_label = malloc(strlen("HoubySoft Calculator - Slope Field - dy/dx = ")+strlen(func_expr)+1);
  if (!graph_top_label)
    mem_error();
  strcpy(graph_top_label,"HoubySoft Calculator - Slope Field - dy/dx = ");
  strcat(graph_top_label,func_expr);
  hc_graph_init2d(arg_label_top ? arg_label_top : graph_top_label, arg_label_x ? arg_label_x : "x", arg_label_y ? arg_label_y : "y", xmin, xmax, ymin, ymax);

  for (i=0; i<HC_GRAPH_POINTS_SF; i++)
  {
    for (ii=0; ii<HC_GRAPH_POINTS_SF; ii++)
    {
      if (a_hasval[i][ii]=='y')
        hc_graph_line2d(a_x[i] + sqrt((pow(r_x,2))/(1 + (pow(a[i][ii],2)))),a_y[ii] + a[i][ii] * sqrt(pow(r_y,2)/(1 + pow(a[i][ii],2))),a_x[i] - sqrt(pow(r_x,2)/(1 + pow(a[i][ii],2))),a_y[ii] - a[i][ii] * sqrt(pow(r_y,2)/(1 + pow(a[i][ii],2))));
    }
  }

  free(arg_xmin);
  free(arg_ymin);
  free(arg_xmax);
  free(arg_ymax);
  free(a_x);
  free(a_y);
  for (i=0; i<HC_GRAPH_POINTS_SF; i++)
  {
    free(a[i]);
    free(a_hasval[i]);
  }
  free(a);

  hc_graph_finish(HCGT_SLPFLD, func_expr, arg_label_top, arg_label_x, arg_label_y, NULL);
  free(graph_top_label);
  free(func_expr);
  free(arg_label_top); free(arg_label_x); free(arg_label_y);

  return SUCCESS; 
}


char hc_graph_polar(char *e)
{
  char *func_expr,*arg_xmin,*arg_xmax,*arg_ymin,*arg_ymax,*arg_tmin,*arg_tmax,*arg_label_top,*arg_label_x,*arg_label_y;
  func_expr = hc_get_arg(e,1);
  arg_tmin = hc_get_arg_r(e,2);
  arg_tmax = hc_get_arg_r(e,3);
  arg_xmin = hc_get_arg_r(e,4);
  arg_xmax = hc_get_arg_r(e,5);
  arg_ymin = hc_get_arg_r(e,6);
  arg_ymax = hc_get_arg_r(e,7);
  arg_label_top = hc_get_arg_r(e,8);
  arg_label_x = hc_get_arg_r(e,9);
  arg_label_y = hc_get_arg_r(e,10);
  double tmin,tmax,xmin,xmax,ymin,ymax;

  if (!func_expr)
  {
    free(func_expr);
    free(arg_tmin); free(arg_tmax); free(arg_xmin); free(arg_xmax); free(arg_ymin); free(arg_ymax); free(arg_label_top); free(arg_label_x); free(arg_label_y);
    arg_error("graphpolar() needs at least one argument (expr_r).");
  }
  if (!arg_tmin || !arg_tmax || !arg_xmin || !arg_xmax || !arg_ymin || !arg_ymax)
  {
    if (arg_xmin || arg_xmax || arg_ymin || arg_ymax || arg_tmin || arg_tmax)
      notify("You haven't provided all of xmin, xmax, ymin and ymax correctly. Using defaults.\n");
    xmin = hc.xmin2d[hcgt_get_idx(HCGT_POLAR)];
    xmax = hc.xmax2d[hcgt_get_idx(HCGT_POLAR)];
    ymin = hc.ymin2d[hcgt_get_idx(HCGT_POLAR)];
    ymax = hc.ymax2d[hcgt_get_idx(HCGT_POLAR)];
    tmin = hc.tminpolar;
    tmax = hc.tmaxpolar;
  } else {
    hc.xmin2d[hcgt_get_idx(HCGT_POLAR)] = xmin = strtod(arg_xmin,NULL);
    hc.xmax2d[hcgt_get_idx(HCGT_POLAR)] = xmax = strtod(arg_xmax,NULL);
    hc.ymin2d[hcgt_get_idx(HCGT_POLAR)] = ymin = strtod(arg_ymin,NULL);
    hc.ymax2d[hcgt_get_idx(HCGT_POLAR)] = ymax = strtod(arg_ymax,NULL);
    hc.tminpolar = tmin = strtod(arg_tmin,NULL);
    hc.tmaxpolar = tmax = strtod(arg_tmax,NULL);
  }

  PROCESS_LABELS_2D();

  char *graph_top_label = malloc(strlen("HoubySoft Calculator - Polar Function - r(t) = ")+strlen(func_expr)+1);
  if (!graph_top_label)
    mem_error();
  strcpy(graph_top_label,"HoubySoft Calculator - Polar Function - r(t) = ");
  strcat(graph_top_label,func_expr);
  char *func_exprx = malloc(strlen(func_expr) + 3 + 6 + 1); if (!func_exprx) mem_error();
  char *func_expry = malloc(strlen(func_expr) + 3 + 6 + 1); if (!func_expry) mem_error();
  sprintf(func_exprx, "(%s)*cos(t)", func_expr);
  sprintf(func_expry, "(%s)*sin(t)", func_expr);
  char angle_setting = hc.angle;
  hc.angle = 'r';
  char r = hc_graph_peq_core(func_exprx, func_expry, tmin, tmax, xmin, xmax, ymin, ymax, arg_label_top ? arg_label_top : graph_top_label, arg_label_x, arg_label_y, HCGT_POLAR, func_expr);
  hc.angle = angle_setting;
  free(graph_top_label);
  free(arg_xmin);
  free(arg_ymin);
  free(arg_xmax);
  free(arg_ymax);
  free(arg_tmin);
  free(arg_tmax);
  free(arg_label_top); free(arg_label_x); free(arg_label_y);
  free(func_exprx); free(func_expry);
  return r;
}


char hc_graph_peq(char *e)
{
  char *func_exprx,*func_expry,*arg_xmin,*arg_xmax,*arg_ymin,*arg_ymax,*arg_tmin,*arg_tmax,*arg_label_top,*arg_label_x,*arg_label_y;
  func_exprx = hc_get_arg(e,1);
  func_expry = hc_get_arg(e,2);
  arg_tmin = hc_get_arg_r(e,3);
  arg_tmax = hc_get_arg_r(e,4);
  arg_xmin = hc_get_arg_r(e,5);
  arg_xmax = hc_get_arg_r(e,6);
  arg_ymin = hc_get_arg_r(e,7);
  arg_ymax = hc_get_arg_r(e,8);
  arg_label_top = hc_get_arg_r(e,9);
  arg_label_x = hc_get_arg_r(e,10);
  arg_label_y = hc_get_arg_r(e,11);
  double tmin,tmax,xmin,xmax,ymin,ymax;

  if (!func_exprx || !func_expry)
  {
    free(func_exprx); free(func_expry);
    free(arg_tmin); free(arg_tmax); free(arg_xmin); free(arg_xmax); free(arg_ymin); free(arg_ymax); free(arg_label_top); free(arg_label_x); free(arg_label_y);
    arg_error("graphpeq() needs at least two arguments (expr_x,expr_y).");
  }
  if (!arg_tmin || !arg_tmax || !arg_xmin || !arg_xmax || !arg_ymin || !arg_ymax)
  {
    if (arg_xmin || arg_xmax || arg_ymin || arg_ymax || arg_tmin || arg_tmax)
      notify("You haven't provided all of xmin, xmax, ymin and ymax correctly. Using defaults.\n");
    xmin = hc.xmin2d[hcgt_get_idx(HCGT_PARAMETRIC)];
    xmax = hc.xmax2d[hcgt_get_idx(HCGT_PARAMETRIC)];
    ymin = hc.ymin2d[hcgt_get_idx(HCGT_PARAMETRIC)];
    ymax = hc.ymax2d[hcgt_get_idx(HCGT_PARAMETRIC)];
    tmin = hc.tminpeq;
    tmax = hc.tmaxpeq;
  } else {
    hc.xmin2d[hcgt_get_idx(HCGT_PARAMETRIC)] = xmin = strtod(arg_xmin,NULL);
    hc.xmax2d[hcgt_get_idx(HCGT_PARAMETRIC)] = xmax = strtod(arg_xmax,NULL);
    hc.ymin2d[hcgt_get_idx(HCGT_PARAMETRIC)] = ymin = strtod(arg_ymin,NULL);
    hc.ymax2d[hcgt_get_idx(HCGT_PARAMETRIC)] = ymax = strtod(arg_ymax,NULL);
    hc.tminpeq = tmin = strtod(arg_tmin,NULL);
    hc.tmaxpeq = tmax = strtod(arg_tmax,NULL);
  }

  PROCESS_LABELS_2D();

  char *graph_top_label = malloc(strlen("HoubySoft Calculator - Parametric Function - x = , y = ")+strlen(func_exprx)+strlen(func_expry)+1);
  if (!graph_top_label)
    mem_error();
  strcpy(graph_top_label,"HoubySoft Calculator - Parametric Function - x = ");
  strcat(graph_top_label,func_exprx);
  strcat(graph_top_label,", y = ");
  strcat(graph_top_label,func_expry);
  char *exprtmp = malloc(strlen(func_exprx) + 1 + strlen(func_expry) + 1);
  if (!exprtmp) mem_error();
  sprintf(exprtmp, "%s,%s", func_exprx, func_expry);

  char r = hc_graph_peq_core(func_exprx, func_expry, tmin, tmax, xmin, xmax, ymin, ymax, arg_label_top ? arg_label_top : graph_top_label, arg_label_x, arg_label_y, HCGT_PARAMETRIC, exprtmp);
  free(exprtmp);
  free(graph_top_label);
  free(arg_xmin);
  free(arg_ymin);
  free(arg_xmax);
  free(arg_ymax);
  free(arg_tmin);
  free(arg_tmax);
  free(func_exprx); free(func_expry);
  free(arg_label_top); free(arg_label_x); free(arg_label_y);
  return r;
}


char hc_graph_peq_core(char *func_exprx, char *func_expry, double tmin, double tmax, double xmin, double xmax, double ymin, double ymax, char *graph_top_label, char *graph_x_label, char *graph_y_label, int type, char *expr)
{
  hc_graph_init2d(graph_top_label, graph_x_label ? graph_x_label : "x", graph_y_label ? graph_y_label : "y", xmin, xmax, ymin, ymax);

  double stept = HC_GRAPH_PEQ_T_STEP;
  double curt = tmin;
  graphing_ignore_errors = TRUE;
  double x1=0,x2=0,y1=0,y2=0;
  char discont = 1;
  for (; curt <= tmax; curt+=stept)
  {
    char tmp_curt[256];
    sprintf(tmp_curt,"%f",curt);
    char *tmp_exprx = strreplace(func_exprx,"t",tmp_curt);
    char *tmp_expry = strreplace(func_expry,"t",tmp_curt);
    char *tmp_x = hc_result_(tmp_exprx);
    char *tmp_y = hc_result_(tmp_expry);
    char *tmp_xi = hc_imag_part(tmp_x);
    char *tmp_yi = hc_imag_part(tmp_y);
    if (tmp_xi || tmp_yi || !tmp_x || !tmp_y)
    {
      free(tmp_xi); free(tmp_yi);
      discont = 1;
    } else {
      if (discont)
      {
        x1 = strtod(tmp_x,NULL);
        y1 = strtod(tmp_y,NULL);
        discont = 0;
      } else {
        x2 = strtod(tmp_x,NULL);
        y2 = strtod(tmp_y,NULL);
        hc_graph_line2d(x1,y1,x2,y2);
        x1 = x2;
        y1 = y2;
      }
    }
    free(tmp_exprx); free(tmp_expry);
    free(tmp_x); free(tmp_y);
  }

  graphing_ignore_errors = FALSE;

  hc_graph_finish(type, expr, graph_x_label && graph_y_label ? graph_top_label : NULL, graph_x_label, graph_y_label, NULL);
  return SUCCESS;
}


// Draw a graph using the values specified. If draw_lines is 0, the points are not connected with lines; if it is 1, they are connected with lines.
char hc_graph_values(char *e, char draw_lines)
{
  char *pts = hc_get_arg(e,1); // pts are used later to pass to hc_graph_finish
  char *points_orig = hc_result_(pts);
  if (!points_orig || !is_list(points_orig))
  {
    free(pts); free(points_orig);
    arg_error("graphvalues() : argument needs to be a list of point coordinates");
  }
  char *points = list_clean(points_orig);
  double xmin=0,xmax=0,ymin=0,ymax=0;
  double lastpointx=0;
  double x1=0,x2=0,y1=0,y2=0;
  char mode = 0;

  unsigned int idx = 0;
  char *cur = NULL;

  // Check points and determine xmin, xmax, ymin, ymax
  while ((cur = hc_get_arg(points,++idx)) != NULL)
  {
    if (is_list(cur) && mode==0)
    {
      mode = 2;
    }
    else if ((is_list(cur) && mode==1) || (!is_list(cur) && mode==2))
    {
      free(cur); free(pts); free(points_orig);
      arg_error("graphvalues() : inconsistent input");
    } else if (mode==0) {
      mode = 1;
    }

    if (mode == 1)
    {
      if (idx == 1)
      {
        xmin = xmax = 0;
        ymin = ymax = strtod(cur,NULL);
      } else {
        xmax = idx - 1;
        if (strtod(cur,NULL) < ymin)
          ymin = strtod(cur,NULL);
        if (strtod(cur,NULL) > ymax)
          ymax = strtod(cur,NULL);
      }
    } else { // mode == 2
      char *curtmp = list_clean(cur);
      char *curx = hc_get_arg(curtmp,1);
      char *cury = hc_get_arg(curtmp,2);
      if (!curx || !is_num(curx) || !cury || !is_num(cury)) {
	free(curx); free(cury); free(cur); free(pts); free(points_orig);
	arg_error("graphvalues() : invalid point list supplied.");
      }
      if (idx == 1)
      {
        xmin = xmax = lastpointx = strtod(curx,NULL);
        ymin = ymax = strtod(cury,NULL);
      } else {
        if (strtod(curx,NULL) <= lastpointx)
        {
          free(curx); free(cury); free(cur); free(pts); free(points_orig);
          arg_error("graphvalues() : invalid point list. x values are not in ascending order.");
        } else {
          lastpointx = strtod(curx,NULL);
          if (lastpointx > xmax)
            xmax = lastpointx;
          if (strtod(cury,NULL) < ymin)
            ymin = strtod(cury,NULL);
          if (strtod(cury,NULL) > ymax)
            ymax = strtod(cury,NULL);
        }
      }
      free(curx); free(cury);
    }

    free(cur);
  }

  if (xmin == xmax) {
    free(points_orig); free(pts);
    arg_error("graphvalues() : invalid point list; make sure to specify at least two points to graph.");
  }

  if (ymin == ymax) {
    ymin -= 1.0;
    ymax += 1.0;
  }

  char *arg_label_top,*arg_label_x,*arg_label_y;
  arg_label_top = hc_get_arg_r(e,2);
  arg_label_x = hc_get_arg_r(e,3);
  arg_label_y = hc_get_arg_r(e,4);

  PROCESS_LABELS_2D();

  hc_graph_init2d(arg_label_top ? arg_label_top : "HoubySoft Calculator - Graph", arg_label_x ? arg_label_x : "x", arg_label_y ? arg_label_y : "y", xmin, xmax, ymin, ymax);

  graphing_ignore_errors = TRUE;
  cur = NULL;
  idx = 0;

  // Do the actual graphing. If we get here, points are correct, so no checking is necessary.
  double cx = -1,cy;
  while ((cur = hc_get_arg(points,++idx)) != NULL)
  {
    if (is_list(cur))
    {
      char *ctmp = list_clean(cur);
      char *ctmpx = hc_get_arg(ctmp,1);
      char *ctmpy = hc_get_arg(ctmp,2);
      cx = strtod(ctmpx,NULL); cy = strtod(ctmpy,NULL);
      free(ctmpx); free(ctmpy);
    } else {
      cx++;
      cy = strtod(cur,NULL);
    }

    if (idx == 1)
    {
      x1 = cx;
      y1 = cy;
    } else {
      x2 = cx;
      y2 = cy;
      if (draw_lines)
        hc_graph_line2d(x1,y1,x2,y2);
      else
      {
        hc_graph_point(&x1,&y1);
        hc_graph_point(&x2,&y2);
      }
      x1 = x2;
      y1 = y2;
    }

    free(cur);
  }

  graphing_ignore_errors = FALSE;

  free(points_orig);

  hc_graph_finish(draw_lines ? HCGT_VALUESLINE : HCGT_VALUESPOINTS, pts, arg_label_top, arg_label_x, arg_label_y, NULL);
  free(pts);
  free(arg_label_top); free(arg_label_x); free(arg_label_y);

  return SUCCESS;
}
