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
#include "hc.h"
#include "hc_conversions.h"
#include "hc_functions.h"


/* hc_conversions fields
 * ---------------------
 * [x][0] : base unit type
 * [x][1] : factor (ie. 1 base_unit = factor output units)
 * [x][2] : output unit name 
 * [x][3 to MAX_CONVERSION_FIELDS-1] : synonyms for the output unit name. Last entry is always NULL.
 */
#define HC_CONVERSIONS 8
const char *hc_conversions[][MAX_CONVERSION_FIELDS] = {
  {"length", "(1/0.3048)", "feet", "ft", NULL},
  {"length", "1000", "milimeters", "mm", NULL},
  {"length", "100", "centimeters", "cm", NULL},
  {"length", "10", "decimeters", "dm", NULL},
  {"length", "0.001", "kilometers", "km", NULL},
  {"length", "0.000621371192", "miles", "mi", NULL},
  {"length", "(1/0.0254)", "inches", "inch", NULL},
  {"weight", "2.20462262185", "pounds", "lbs", NULL},
};

/* hc_conversions_basenames fields
 * -------------------------------
 * [x][0] : base unit type
 * [x][1 to MAX_CONVERSION_FIELDS-1] : synonyms for the base unit name. Last entry is always NULL.
 */
#define HC_CONVERSIONS_BASENAMES 2
const char *hc_conversions_basenames[][MAX_CONVERSION_FIELDS] = {
  {"length", "meters", "m", NULL},
  {"weight", "kilograms", "kg", NULL}
};


int hc_convert(M_APM result, char *e)
{
  char *qty = hc_get_arg_r(e,1);
  char *in_unit1 = hc_get_arg_r(e,2);
  char *out_unit1 = hc_get_arg_r(e,3);
  if (!qty || !is_num(qty) || !in_unit1 || !is_string(in_unit1) || !out_unit1 || !is_string(out_unit1))
  {
    free(qty); free(in_unit1); free(out_unit1);
    arg_error("convert() : three arguments are required : x, in_unit, and out_unit. See help(\"convert\") for details.");
  }
  char *in_unit = get_string(strip_spaces(in_unit1));
  char *out_unit = get_string(strip_spaces(out_unit1));
  free(in_unit1); free(out_unit1);

  unsigned int i = 0, j = 0;
  char found_in_unit_idx, found_out_unit_idx;
  found_in_unit_idx = found_out_unit_idx = FALSE;
  unsigned int in_unit_idx, out_unit_idx;
  for (i = 0; (!found_in_unit_idx || !found_out_unit_idx) && i < HC_CONVERSIONS; i++)
  {
    if (!found_in_unit_idx)
    {
      j = 2;
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
      j = 2;
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
