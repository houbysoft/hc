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
#include <stdlib.h>
#include <string.h>
#include "hc_complex.h"


char *hc_real_part(char *num)
{
  char *ipos = strchr(num,'i');
  char *ret;
  if (ipos)
  {
    ret = malloc(strlen(num)-strlen(ipos)+1);
    strncpy(ret,num,strlen(num)-strlen(ipos));
    ret[strlen(num)-strlen(ipos)]='\0';
  } else {
    ret = strdup(num);
  }
  return ret;
}


char *hc_imag_part(char *num)
{
  char *ipos = strchr(num,'i');
  if (ipos)
  {
    char *ret = malloc(strlen(ipos)); // no need for +1, since ipos contains 'i' which we won't store
    strcpy(ret,(char *)(ipos+sizeof(char)));
    return ret;
  } else {
    return NULL;
  }
}
