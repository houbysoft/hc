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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hc.h"
#include "hc_functions.h"
#include "hc_chemistry.h"
#include "hc_utils.h"


double hc_get_element_info(char *el, int info)
{
  switch (info)
  {
  case ATOMIC_WEIGHT:
    switch (simple_hash(el))
    {
    case EL_H:
      return 1.00794;

    case EL_HE:
      return 4.002602;

    case EL_LI:
      return 6.941;

    case EL_BE:
      return 9.012182;

    case EL_B:
      return 10.811;

    case EL_C:
      return 12.0107;

    case EL_N:
      return 14.00674;

    case EL_O:
      return 15.9994;

    case EL_F:
      return 18.9984032;

    case EL_NE:
      return 20.1797;

    case EL_NA:
      return 22.98977;

    case EL_MG:
      return 24.3050;

    case EL_AL:
      return 26.981538;

    case EL_SI:
      return 28.0855;

    case EL_P:
      return 30.973761;

    case EL_S:
      return 32.066;

    case EL_CL:
      return 35.453;

    case EL_AR:
      return 39.948;

    case EL_K:
      return 39.0983;

    case EL_CA:
      return 40.078;

    case EL_SC:
      return 44.95591;

    case EL_TI:
      return 47.867;

    case EL_V:
      return 50.9415;

    case EL_CR:
      return 51.9961;

    case EL_MN:
      return 54.938049;

    case EL_FE:
      return 55.8457;

    case EL_CO:
      return 58.9332;

    case EL_NI:
      return 58.6934;

    case EL_CU:
      return 63.546;

    case EL_ZN:
      return 65.409;

    case EL_GA:
      return 69.723;

    case EL_GE:
      return 72.64;

    case EL_AS:
      return 74.9216;

    case EL_SE:
      return 78.96;

    case EL_BR:
      return 79.904;

    case EL_KR:
      return 83.798;

    case EL_RB:
      return 85.4678;

    case EL_SR:
      return 87.62;

    case EL_Y:
      return 88.90585;

    case EL_ZR:
      return 91.224;

    case EL_NB:
      return 92.90638;

    case EL_MO:
      return 95.94;

    case EL_TC:
      return 98;

    case EL_RU:
      return 101.07;

    case EL_RH:
      return 102.9055;

    case EL_PD:
      return 106.42;

    case EL_AG:
      return 107.8682;

    case EL_CD:
      return 112.411;

    case EL_IN:
      return 114.818;

    case EL_SN:
      return 118.71;

    case EL_SB:
      return 121.76;

    case EL_TE:
      return 127.6;

    case EL_I:
      return 126.90447;

    case EL_XE:
      return 131.293;

    case EL_CS:
      return 132.90545;

    case EL_BA:
      return 137.327;

    case EL_LA:
      return 138.9055;

    case EL_CE:
      return 140.116;

    case EL_PR:
      return 140.90765;

    case EL_ND:
      return 144.24;

    case EL_PM:
      return 145;

    case EL_SM:
      return 150.36;

    case EL_EU:
      return 151.964;

    case EL_GD:
      return 157.25;

    case EL_TB:
      return 158.92534;

    case EL_DY:
      return 162.5;

    case EL_HO:
      return 164.93032;

    case EL_ER:
      return 167.259;

    case EL_TM:
      return 168.93421;

    case EL_YB:
      return 173.04;

    case EL_LU:
      return 174.967;

    case EL_HF:
      return 178.49;

    case EL_TA:
      return 180.9479;

    case EL_W:
      return 183.84;

    case EL_RE:
      return 186.207;

    case EL_OS:
      return 190.23;

    case EL_IR:
      return 192.217;

    case EL_PT:
      return 195.078;

    case EL_AU:
      return 196.96655;

    case EL_HG:
      return 200.59;

    case EL_TL:
      return 204.3833;

    case EL_PB:
      return 207.2;

    case EL_BI:
      return 208.98038;

    case EL_PO:
      return 209;

    case EL_AT:
      return 210;

    case EL_RN:
      return 222;

    case EL_FR:
      return 223;

    case EL_RA:
      return 226;

    case EL_AC:
      return 227;

    case EL_TH:
      return 232.0381;

    case EL_PA:
      return 231.03588;

    case EL_U:
      return 238.02891;

    case EL_NP:
      return 237;

    case EL_PU:
      return 244;

    case EL_AM:
      return 243;

    case EL_CM:
      return 247;

    case EL_BK:
      return 247;

    case EL_CF:
      return 251;

    case EL_ES:
      return 252;

    case EL_FM:
      return 257;

    case EL_MD:
      return 258;

    case EL_NO:
      return 259;

    case EL_LR:
      return 262;

    case EL_RF:
      return 261;

    case EL_DB:
      return 262;

    case EL_SG:
      return 266;

    case EL_BH:
      return 264;

    case EL_HS:
      return 269;

    case EL_MT:
      return 268;

    case EL_DS:
      return 271;

    case EL_RG:
      return 272;

    default:
      error_nq("Error : invalid element specified.");
      return 0;
      break;
    }
    break;

  case ATOMIC_NUMBER:
    switch (simple_hash(el))
    {
    case EL_H:
      return 1;
    case EL_HE:
      return 2;
    case EL_LI:
      return 3;
    case EL_BE:
      return 4;
    case EL_B:
      return 5;
    case EL_C:
      return 6;
    case EL_N:
      return 7;
    case EL_O:
      return 8;
    case EL_F:
      return 9;
    case EL_NE:
      return 10;
    case EL_NA:
      return 11;
    case EL_MG:
      return 12;
    case EL_AL:
      return 13;
    case EL_SI:
      return 14;
    case EL_P:
      return 15;
    case EL_S:
      return 16;
    case EL_CL:
      return 17;
    case EL_AR:
      return 18;
    case EL_K:
      return 19;
    case EL_CA:
      return 20;
    case EL_SC:
      return 21;
    case EL_TI:
      return 22;
    case EL_V:
      return 23;
    case EL_CR:
      return 24;
    case EL_MN:
      return 25;
    case EL_FE:
      return 26;
    case EL_CO:
      return 27;
    case EL_NI:
      return 28;
    case EL_CU:
      return 29;
    case EL_ZN:
      return 30;
    case EL_GA:
      return 31;
    case EL_GE:
      return 32;
    case EL_AS:
      return 33;
    case EL_SE:
      return 34;
    case EL_BR:
      return 35;
    case EL_KR:
      return 36;
    case EL_RB:
      return 37;
    case EL_SR:
      return 38;
    case EL_Y:
      return 39;
    case EL_ZR:
      return 40;
    case EL_NB:
      return 41;
    case EL_MO:
      return 42;
    case EL_TC:
      return 43;
    case EL_RU:
      return 44;
    case EL_RH:
      return 45;
    case EL_PD:
      return 46;
    case EL_AG:
      return 47;
    case EL_CD:
      return 48;
    case EL_IN:
      return 49;
    case EL_SN:
      return 50;
    case EL_SB:
      return 51;
    case EL_TE:
      return 52;
    case EL_I:
      return 53;
    case EL_XE:
      return 54;
    case EL_CS:
      return 55;
    case EL_BA:
      return 56;
    case EL_LA:
      return 57;
    case EL_CE:
      return 58;
    case EL_PR:
      return 59;
    case EL_ND:
      return 60;
    case EL_PM:
      return 61;
    case EL_SM:
      return 62;
    case EL_EU:
      return 63;
    case EL_GD:
      return 64;
    case EL_TB:
      return 65;
    case EL_DY:
      return 66;
    case EL_HO:
      return 67;
    case EL_ER:
      return 68;
    case EL_TM:
      return 69;
    case EL_YB:
      return 70;
    case EL_LU:
      return 71;
    case EL_HF:
      return 72;
    case EL_TA:
      return 73;
    case EL_W:
      return 74;
    case EL_RE:
      return 75;
    case EL_OS:
      return 76;
    case EL_IR:
      return 77;
    case EL_PT:
      return 78;
    case EL_AU:
      return 79;
    case EL_HG:
      return 80;
    case EL_TL:
      return 81;
    case EL_PB:
      return 82;
    case EL_BI:
      return 83;
    case EL_PO:
      return 84;
    case EL_AT:
      return 85;
    case EL_RN:
      return 86;
    case EL_FR:
      return 87;
    case EL_RA:
      return 88;
    case EL_AC:
      return 89;
    case EL_TH:
      return 90;
    case EL_PA:
      return 91;
    case EL_U:
      return 92;
    case EL_NP:
      return 93;
    case EL_PU:
      return 94;
    case EL_AM:
      return 95;
    case EL_CM:
      return 96;
    case EL_BK:
      return 97;
    case EL_CF:
      return 98;
    case EL_ES:
      return 99;
    case EL_FM:
      return 100;
    case EL_MD:
      return 101;
    case EL_NO:
      return 102;
    case EL_LR:
      return 103;
    case EL_RF:
      return 104;
    case EL_DB:
      return 105;
    case EL_SG:
      return 106;
    case EL_BH:
      return 107;
    case EL_HS:
      return 108;
    case EL_MT:
      return 109;
    case EL_DS:
      return 110;
    case EL_RG:
      return 111;
    default:
      error_nq("Error : invalid element specified.");
      return 0;
      break;
    }

  default:
    error_nq("Error : invalid 'info' argument passed to hc_get_element_info.");
    return 0;
    break;
  }
}


double hc_mmass_el(char *e, unsigned int *i)
{
  char element[3];
  double element_aw=0;
  char count[32];
  element[0] = e[*i];
  if (islower(e[*i+1]))
  {
    element[1] = e[++*i];
    element[2] = '\0';
  } else {
    element[1] = '\0';
  }
  element_aw = hc_get_element_info(element,ATOMIC_WEIGHT);
  if (!element_aw)
  {
    return 0;
  }
  int tmp=0;
  count[0]=0;
  while (e[*i] && e[*i+1] && isdigit(e[*i+1]) && tmp < 32)
    count[tmp++] = e[++*i];
  if (tmp>=32)
  {
    arg_error("mmass() : integer too big, please use a lower value.");
  }
  count[tmp]=0;
  if (strlen(count))
    tmp = atoi(count);
  else
    tmp = 1;
  element_aw *= tmp;
  return element_aw;
}


double hc_mmass_par(char *e, unsigned int *i)
{
  int par = 1;
  unsigned int j = *i + 1;
  char count[32];
  double par_aw=0;
  ++*i;
  while (par)
  {
    if (e[*i]=='(')
      par++;
    if (e[*i]==')')
      par--;
    ++*i;
  }
  --*i;
  int tmp=0;
  count[0]=0;
  while (e[*i] && e[*i+1] && isdigit(e[*i+1]) && tmp < 32)
    count[tmp++] = e[++*i];
  if (tmp>=32)
  {
    arg_error("mmass() : integer too big, please use a lower value.");
  }
  count[tmp]=0;
  if (strlen(count))
    tmp = atoi(count);
  else
    tmp = 1;
  for (; j <= *i - strlen(count); j++)
  {
    if (isupper(e[j]))
    {
      par_aw += hc_mmass_el(e, &j);      
    }
    if (e[j]=='(')
    {
      par_aw += hc_mmass_par(e, &j);
    }
  }
  ++*i;
  par_aw *= tmp;
  return par_aw;
}
