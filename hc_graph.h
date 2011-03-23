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

extern void hc_graph_init();
extern void hc_graph_init2d(char *label_top, char *label_x, char *label_y, double xmin, double xmax, double ymin, double ymax);
extern void hc_graph_boxplot(char *label_top, char *label_x, char *label_y, double min, double max, double q1, double q2, double q3);
extern void hc_graph_finish();

extern char hc_graph(char *e);
extern char hc_graph_n(char *e);
extern char hc_graph3d(char *e);
extern char hc_graph_slpfld(char *e);
extern char hc_graph_peq(char *e);
extern char hc_graph_values(char *e, char draw_lines);

#if (defined(HCG) || defined(HCG_E)) && !defined(WIN32)
extern void hcg_disp_graph(char *);
#endif

#if defined(MEM_DRIVER)
extern void hcg_disp_rgb(unsigned int x, unsigned int y, void *data, char *args);
#endif

#endif
