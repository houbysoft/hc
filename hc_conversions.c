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


#include <m_apm.h>
#include <stdlib.h>
#include <string.h>
#include "hc.h"
#include "hc_conversions.h"
#include "hc_functions.h"


/* hc_conversions fields
 * ---------------------
 * [x][0] : base unit type
 * [x][1] : factor (ie. 1 base_unit = factor output units), except for temperature
 * [x][2] : output unit name 
 * [x][3 to MAX_CONVERSION_FIELDS-1] : synonyms for the output unit name. Last entry is always NULL.
 */
const char *hc_conversions[HC_CONVERSIONS][MAX_CONVERSION_FIELDS] = {
  {"length", "(1/(12/39.37))", "feet", "ft", NULL},
  {"length", "1e24", "yoctometers", "ym", NULL},
  {"length", "1e21", "zeptometers", "zm", NULL},
  {"length", "1e15", "femtometers", "fermis", "fermi", NULL},
  {"length", "1e12", "picometers", "pm", NULL},
  {"length", "1e9", "nanometers", "nm", NULL},
  {"length", "1e6", "micrometers", "microns", NULL},
  {"length", "1000", "milimeters", "mm", NULL},
  {"length", "100", "centimeters", "cm", NULL},
  {"length", "10", "decimeters", "dm", NULL},
  {"length", "0.1", "decameters", "dam", NULL},
  {"length", "0.01", "hectometers", "hm", NULL},
  {"length", "0.001", "kilometers", "km", NULL},
  {"length", "1e-6", "megameters", "Mm", NULL},
  {"length", "1e-9", "gigameters", "Gm", NULL},
  {"length", "1e-15", "petameters", "Pm", NULL},
  {"length", "1e-18", "exameters", "Em", NULL},
  {"length", "1e-21", "zettameters", "Zm", NULL},
  {"length", "1e-24", "yottameters", "Ym", NULL},
  {"length", "0.000621371192", "miles", "mi", NULL},
  {"length", "(1/0.0254)", "inches", "inch", NULL},
  {"length", "(1/0.9144)", "yards", "yd", NULL},
  {"length", "1e10", "angstroms", NULL},
  {"length", "(1/149597870691)", "astronomical units", "AU", "ua", NULL},
  {"length", "(1/5.291772108e-11)", "atomic units", "atomic unit", NULL},
  {"length", "10e18", "attometers", "am", NULL},
  {"length", "(1/0.008466667)", "Barley corn", NULL},
  {"length", "(1/219.456)", "cable length (US)", NULL},
  {"length", "(1/158.2)", "cable length (International)", NULL},
  {"length", "(1/185.3184)", "cable length (Imperial)", NULL},
  {"length", "(1/(66*12/39.37))", "chains (Gunter)", "chains", "chain", NULL},
  {"length", "(1/30.48)", "chains (Ramsden's)", "engineer chains", NULL},
  {"length", "(1/0.4572)", "cubits", NULL},
  {"length", "(1/2.2225e-2)", "fingers", NULL},
  {"weight", "2.20462262185", "pounds", "lbs", NULL},
  {"weight", "0.157473044", "stones", NULL},
  {"volume", "1000", "liters", "L", NULL},
  {"volume", "1000000", "mililiters", "mL", "ml", NULL},
  {"volume", "(1000000*(1/29.5735296875))", "US fluid ounces", "fl oz", "floz", "fluid ounces", NULL},
  {"temperature", "", "C", "degree Celsius", "Celsius", NULL},
  {"temperature", "", "F", "degree Fahrenheit", "Fahrenheit", NULL},
  {"temperature", "", "K", "degree Kelvin", "Kelvin", NULL},
  {"area", "10000", "square centimeters", "cm^2", NULL},
  {"area", "(1/4046.8564224)", "acres", NULL},
  {"area", "1000000", "square milimeters", "mm^2", NULL},
  {"area", "1.19599005", "square yards", "yards^2", "yd^2", NULL},
  {"area", "3.86102159e-7", "square miles", "mi^2", NULL},
  {"area", "10.7639104", "square feet", "feet^2", NULL},
  {"area", "1550.0031", "square inches", "inches^2", NULL},
  {"area", "1e-6", "square kilometers", "km^2", NULL},
  {"area", "1e-4", "hectares", "ha", NULL},
  {"area", "1e-2", "ares", NULL},
  {"speed", "1e2", "centimeters/second", "cm/s", NULL},
  {"speed", "(1/3.048e-1)", "feet/second", NULL},
  {"speed", "(1/5.08e-3)", "feet/minute", "feet/min", NULL},
  {"speed", "(1/0.44704)", "miles/hour", "mph", NULL},
  {"speed", "3.6", "kilometers/hour", "km/h", "kph", NULL},
  {"speed", "(1/340.3)", "Mach", "M", "Ma", "machs", "mach", NULL},
  {"speed", "(1/1.609344e3)", "Miles/second", "mps", "mile/s", "miles/s", NULL},
  {"speed", "(1/26.8224)", "Miles/minute", "mpm", "mile/min", "miles/minute", NULL},
  {"speed", "(15e5/127)", "Feet/hour", "fph", NULL},
  {"speed", "(30e4/127)", "inches/min", "ipm", "in/m", NULL}
};

/* hc_conversions_basenames fields
 * -------------------------------
 * [x][0] : base unit type
 * [x][1 to MAX_CONVERSION_FIELDS-1] : synonyms for the base unit name. Last entry is always NULL.
 */
const char *hc_conversions_basenames[HC_CONVERSIONS_BASENAMES][MAX_CONVERSION_FIELDS] = {
  {"length", "meters", "m", NULL},
  {"weight", "kilograms", "kg", NULL},
  {"volume", "cubic meters", "m^3", NULL},
  {"area", "square meters", "m^2", NULL},
  {"speed", "meters/second", "m/s", NULL}
};


void hc_convert_showunits();


int hc_convert(M_APM result, char *e)
{
  char *qty = hc_get_arg_r(e,1);
  char *in_unit1 = hc_get_arg_r(e,2);
  char *out_unit1 = hc_get_arg_r(e,3);
  if ((!qty || !strlen(qty))&& !in_unit1 && !out_unit1)
  {
    hc_convert_showunits();
    return SUCCESS;
  }
  if (!qty || !is_num(qty) || !in_unit1 || !is_string(in_unit1) || !out_unit1 || !is_string(out_unit1))
  {
    free(qty); free(in_unit1); free(out_unit1);
    arg_error("convert() : three arguments are required : x, in_unit, and out_unit. Or, just call convert() to get a list of supported units. See help(\"convert\") for details.");
  }
  char *in_unit = get_string(strip_spaces(in_unit1));
  char *out_unit = get_string(strip_spaces(out_unit1));
  free(in_unit1); free(out_unit1);

  unsigned int i = 0, j = 0;
  char found_in_unit_idx, found_out_unit_idx;
  found_in_unit_idx = found_out_unit_idx = FALSE;
  unsigned int in_unit_idx=0, out_unit_idx=0;
  for (i = 0; (!found_in_unit_idx || !found_out_unit_idx) && i < HC_CONVERSIONS; i++)
  {
    if (!found_in_unit_idx)
    {
      j = HC_CONVERSIONS_NAME_START_IDX;
      while (!found_in_unit_idx && hc_conversions[i][j]) {
        if (strcmp(in_unit, hc_conversions[i][j]) == 0)
        {
          found_in_unit_idx = HC_CONVERSIONS_NORMAL_UNIT;
          in_unit_idx = i;
          break;
        }
        j++;
      }
    }
    if (!found_out_unit_idx)
    {
      j = HC_CONVERSIONS_NAME_START_IDX;
      while (!found_out_unit_idx && hc_conversions[i][j]) {
        if (strcmp(out_unit, hc_conversions[i][j]) == 0)
        {
          found_out_unit_idx = HC_CONVERSIONS_NORMAL_UNIT;
          out_unit_idx = i;
          break;
        }
        j++;
      }
    }
  }
  for (i = 0; (!found_in_unit_idx || !found_out_unit_idx) && i < HC_CONVERSIONS_BASENAMES; i++)
  {
    if (!found_in_unit_idx)
    {
      j = 1;
      while (!found_in_unit_idx && hc_conversions_basenames[i][j]) {
        if (strcmp(in_unit, hc_conversions_basenames[i][j]) == 0)
        {
          found_in_unit_idx = HC_CONVERSIONS_BASE_UNIT;
          in_unit_idx = i;
          break;
        }
        j++;
      }
    }
    if (!found_out_unit_idx)
    {
      j = 1;
      while (!found_out_unit_idx && hc_conversions_basenames[i][j]) {
        if (strcmp(out_unit, hc_conversions_basenames[i][j]) == 0)
        {
          found_out_unit_idx = HC_CONVERSIONS_BASE_UNIT;
          out_unit_idx = i;
          break;
        }
        j++;
      }
    }
  }
  if (!found_out_unit_idx || !found_in_unit_idx)
  {
    hc_error(ARG,"convert() : unknown or invalid unit specified : \"%s\"", found_out_unit_idx ? in_unit : out_unit);
    free(in_unit); free(out_unit); free(qty);
    return FAIL;
  }

  if (found_out_unit_idx == found_in_unit_idx && out_unit_idx == in_unit_idx)
  {
    m_apm_set_string(result, qty);
  } else if (found_out_unit_idx == HC_CONVERSIONS_BASE_UNIT && found_in_unit_idx == HC_CONVERSIONS_BASE_UNIT) {
    free(qty); free(in_unit); free(out_unit);
    arg_error("convert() : incompatible units");
  } else if (found_out_unit_idx != found_in_unit_idx) {
    const char *base_out, *base_in;
    base_out = found_out_unit_idx == HC_CONVERSIONS_BASE_UNIT ? hc_conversions_basenames[out_unit_idx][0] : hc_conversions[out_unit_idx][0];
    base_in = found_in_unit_idx == HC_CONVERSIONS_BASE_UNIT ? hc_conversions_basenames[in_unit_idx][0] : hc_conversions[in_unit_idx][0];
    if (strcmp(base_out, base_in) != 0)
    {
      free(qty); free(in_unit); free(out_unit);
      arg_error("convert() : incompatible units");
    } else {
      char *tmp = malloc(strlen(qty)+1+strlen(found_out_unit_idx == HC_CONVERSIONS_BASE_UNIT ? hc_conversions[in_unit_idx][1] : hc_conversions[out_unit_idx][1])+1); if (!tmp) mem_error();
      sprintf(tmp, "%s%c%s", qty, found_out_unit_idx == HC_CONVERSIONS_BASE_UNIT ? '/' : '*', found_out_unit_idx == HC_CONVERSIONS_BASE_UNIT ? hc_conversions[in_unit_idx][1] : hc_conversions[out_unit_idx][1]);
      char *res = hc_result_(tmp); free(tmp);
      if (!res) { free(qty); free(in_unit); free(out_unit); return FAIL; }
      m_apm_set_string(result, res); free(res);
    }
  } else if (found_out_unit_idx == HC_CONVERSIONS_NORMAL_UNIT && found_in_unit_idx == HC_CONVERSIONS_NORMAL_UNIT) {
    if (strcmp(hc_conversions[out_unit_idx][0],hc_conversions[in_unit_idx][0]) != 0) {
      free(qty); free(in_unit); free(out_unit);
      arg_error("convert() : incompatible units");
    } else if (strcmp(hc_conversions[out_unit_idx][0], "temperature") == 0) {
      int ret = FAIL;
      if (strcmp(hc_conversions[in_unit_idx][2], "C") == 0) {
        if (strcmp(hc_conversions[out_unit_idx][2], "F") == 0) {
          ret = hc_c2f(result, qty);
        } else if (strcmp(hc_conversions[out_unit_idx][2], "K") == 0) {
          ret = hc_c2k(result, qty);
        }
      } else if (strcmp(hc_conversions[in_unit_idx][2], "F") == 0) {
        if (strcmp(hc_conversions[out_unit_idx][2], "C") == 0) {
          ret = hc_f2c(result, qty);
        } else if (strcmp(hc_conversions[out_unit_idx][2], "K") == 0) {
          ret = hc_f2k(result, qty);
        }
      } else if (strcmp(hc_conversions[in_unit_idx][2], "K") == 0) {
        if (strcmp(hc_conversions[out_unit_idx][2], "C") == 0) {
          ret = hc_k2c(result, qty);
        } else if (strcmp(hc_conversions[out_unit_idx][2], "F") == 0) {
          ret = hc_k2f(result, qty);
        }
      }
      if (ret == FAIL) { free(qty); free(in_unit); free(out_unit); return FAIL; }
    } else {
      char *tmp = malloc(1 + strlen(qty) + 1 + strlen(hc_conversions[in_unit_idx][1]) + 2 + strlen(hc_conversions[out_unit_idx][1]) + 1); if (!tmp) mem_error();
      sprintf(tmp, "(%s/%s)*%s", qty, hc_conversions[in_unit_idx][1], hc_conversions[out_unit_idx][1]);
      char *res = hc_result_(tmp); free(tmp);
      if (!res) { free(qty); free(in_unit); free(out_unit); return FAIL; }
      m_apm_set_string(result, res); free(res);
    }
  }

  free(qty); free(in_unit); free(out_unit);
  return SUCCESS;
}


void hc_convert_showunits()
{
  unsigned int i=0;
  notify_console("Supported units (synonyms also listed): \n");
  for (; i < HC_CONVERSIONS; i++)
  {
    unsigned int j = HC_CONVERSIONS_NAME_START_IDX;
    while (hc_conversions[i][j])
    {
      notify_console((char *)hc_conversions[i][j]);
      notify_console(" ");
      j++;
    }
  }
  for (; i < HC_CONVERSIONS_BASENAMES; i++)
  {
    unsigned int j = 1;
    while (hc_conversions_basenames[i][j])
    {
      notify_console((char *)hc_conversions_basenames[i][j]);
      notify_console(" ");
      j++;
    }
  }
}
