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

#define VERSION "4.0.1"
#define STATUS_URL_GUI "http://houbysoft.com/status/hcg_w32"

// hc_config's hc.flags
#define PRINTFULL 0x1

typedef struct hc_config {
  char announce; // used when command line options are supplied to supress configuration notice
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
  double peqstep;
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
  double xminsf;
  double xmaxsf;
  double yminsf;
  double ymaxsf;
  double tminpeq;
  double tmaxpeq;
  double xminpeq;
  double xmaxpeq;
  double yminpeq;
  double ymaxpeq;
  char flags;
} hc_config_;

extern struct hc_config hc;

#ifdef __cplusplus
}
#endif

#endif
