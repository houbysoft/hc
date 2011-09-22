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


#ifndef HC_GRAPH_H
#define HC_GRAPH_H

#include "hc_info.h" // for hc_graph_finish graph types


#define PROCESS_LABELS_2D() {\
  if (arg_label_top || arg_label_x || arg_label_y) {\
    if (arg_label_top && arg_label_x && arg_label_y) {\
      char *tmp1, *tmp2, *tmp3;\
      tmp1 = get_string(arg_label_top); tmp2 = get_string(arg_label_x); tmp3 = get_string(arg_label_y);\
      if (!tmp1 || !tmp2 || !tmp3) {\
	free(tmp1); free(tmp2); free(tmp3); free(arg_label_top); free(arg_label_x); free(arg_label_y);\
	arg_label_top = arg_label_x = arg_label_y = NULL;\
	hc_error(WARNING, "You haven't provided all three labels (top, x, y) correctly (as strings). Using defaults.\n");\
      } else {\
	free(arg_label_top); free(arg_label_x); free(arg_label_y);\
	arg_label_top = tmp1; arg_label_x = tmp2; arg_label_y = tmp3;\
      }\
    } else {\
      hc_error(WARNING, "You haven't provided all three labels (top, x, y). Using defaults.\n");\
      free(arg_label_top); free(arg_label_x); free(arg_label_y);\
      arg_label_top = arg_label_x = arg_label_y = NULL;\
    }\
  }\
}


#define PROCESS_LABELS_3D() {\
  if (arg_label_top || arg_label_x || arg_label_y || arg_label_z) {\
    if (arg_label_top && arg_label_x && arg_label_y && arg_label_z) {\
      char *tmp1, *tmp2, *tmp3, *tmp4;					\
      tmp1 = get_string(arg_label_top); tmp2 = get_string(arg_label_x); tmp3 = get_string(arg_label_y); tmp4 = get_string(arg_label_z); \
      if (!tmp1 || !tmp2 || !tmp3 || !tmp4) {\
	free(tmp1); free(tmp2); free(tmp3); free(tmp4); free(arg_label_top); free(arg_label_x); free(arg_label_y); free(arg_label_z); \
	arg_label_top = arg_label_x = arg_label_y = arg_label_z = NULL;\
	hc_error(WARNING, "You haven't provided all four labels (top, x, y, z) correctly (as strings). Using defaults.\n");\
      } else {\
	free(arg_label_top); free(arg_label_x); free(arg_label_y); free(arg_label_z); \
	arg_label_top = tmp1; arg_label_x = tmp2; arg_label_y = tmp3; arg_label_z = tmp4; \
      }\
    } else {\
      hc_error(WARNING, "You haven't provided all four labels (top, x, y, z). Using defaults.\n");\
      free(arg_label_top); free(arg_label_x); free(arg_label_y); free(arg_label_z); \
      arg_label_top = arg_label_x = arg_label_y = arg_label_z = NULL;\
    }\
  }\
}


extern void hc_graph_init();
extern void hc_graph_init2d(char *label_top, char *label_x, char *label_y, double xmin, double xmax, double ymin, double ymax);
extern void hc_graph_boxplot(char *label_top, char *label_x, char *label_y, char *args, double min, double max, double q1, double q2, double q3);
extern void hc_graph_finish(int type, char *args);

extern char hc_graph(char *e);
extern char hc_graph_n(char *e, char *label_top, char *label_x, char *label_y);
extern char hc_graph3d(char *e);
extern char hc_graph_slpfld(char *e);
extern char hc_graph_peq(char *e);
extern char hc_graph_polar(char *e);
extern char hc_graph_values(char *e, char draw_lines);

#if (defined(HCG) || defined(HCG_E)) && !defined(WIN32)
extern void hcg_disp_graph(char *);
#endif

#if defined(MEM_DRIVER)
extern void hcg_disp_rgb(unsigned int x, unsigned int y, void *data, int type, char *args);
#endif

#endif
