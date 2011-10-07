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


#ifndef HC_INFO_H
#define HC_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

#define VERSION "6.0"
#define STATUS_URL_GUI "http://houbysoft.com/status/hcg_w32"

// hc_config's hc.flags
#define PRINTFULL 0x1

// hc_config's hc.bgcolor
#define WHITE '0'
#define BLACK '1'

// Graph types -- do not change order; the GUI relies on these values
enum HCGT { HCGT_NULL, HCGT_2D, HCGT_PARAMETRIC, HCGT_VALUESPOINTS, HCGT_VALUESLINE, HCGT_3D, HCGT_SLPFLD, HCGT_BOXPLOT, HCGT_POLAR, HCGT_COUNT };
enum HCGT_IDX { HCGT_IDX_2D, HCGT_IDX_PARAMETRIC, HCGT_IDX_SLPFLD, HCGT_IDX_POLAR, HCGT_IDX_COUNT };

#define GRAPH_PIXELS_X 640 // do not change this, the Qt GUI relies on these values (see question http://stackoverflow.com/questions/5450611 for description of the problem)
#define GRAPH_PIXELS_Y 480

#define hcgt_get_idx(type) (type == HCGT_2D ? HCGT_IDX_2D : type == HCGT_PARAMETRIC ? HCGT_IDX_PARAMETRIC : type == HCGT_SLPFLD ? HCGT_IDX_SLPFLD : type == HCGT_POLAR ? HCGT_IDX_POLAR : 0)

typedef struct hc_config {
  char announce; // used when command line options are supplied to supress configuration notice
  char rpn;
  char autoupdate;
  unsigned int precision;
  char output_format;
  char exp;
  char angle;
  char keys; // GUI only - whether the clickable keys should be displayed
  char bypass_nested; // used to bypass the recursion protection
  char *plplot_dev_override;
  char graph_show_imag;
  int graph_points;
  int graph_width;
  int graph_height;
  unsigned int graph_points_3d;
  double peqstep;
  double xmin2d[HCGT_IDX_COUNT];
  double xmax2d[HCGT_IDX_COUNT];
  double ymin2d[HCGT_IDX_COUNT];
  double ymax2d[HCGT_IDX_COUNT];
  double xmin3d;
  double xmax3d;
  double ymin3d;
  double ymax3d;
  double zmin3d;
  double zmax3d;
  double tminpeq;
  double tmaxpeq;
  double tminpolar;
  double tmaxpolar;
  char flags;
  char bgcolor; // background color for graphs -- either WHITE or BLACK
} hc_config_;

extern struct hc_config hc;

#define PL_COLORS_LEN 14
extern const unsigned int pl_colors[][3];

#ifdef __cplusplus
}
#endif

#endif
