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


#include <stdarg.h>
#ifndef WIN32
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <m_apm.h>
#include "hc.h"
#include "hc_utils.h"
#include "hc_complex.h"


void hc_error(char type, char *xformat, ...)
{
  char *format = NULL;
  switch (type)
  {
  case SYNTAX:
    format = malloc(strlen("Syntax Error : ")+strlen(xformat)+1);
    if (!format) mem_error();
    sprintf(format,"Syntax Error : %s",xformat);
    break;

  case NOT_ENOUGH_OPERANDS:
    format = malloc(strlen("Syntax Error : not enough operands supplied for ")+strlen(xformat)+1);
    if (!format) mem_error();
    sprintf(format,"Syntax Error : not enough operands supplied for %s",xformat);
    break;

  case TYPE:
    format = malloc(strlen("Type Error : ")+strlen(xformat)+1);
    if (!format) mem_error();
    sprintf(format,"Type Error : %s",xformat);
    break;

  case LOAD:
    format = malloc(strlen("Load Error : ")+strlen(xformat)+1);
    if (!format) mem_error();
    sprintf(format,"Load Error : %s",xformat);
    break;

  case ARG:
    format = malloc(strlen("Argument Error : ")+strlen(xformat)+1);
    if (!format) mem_error();
    sprintf(format,"Argument Error : %s",xformat);
    break;

  case WARNING:
    format = malloc(strlen("Warning : ")+strlen(xformat)+1);
    if (!format) mem_error();
    sprintf(format,"Warning : %s",xformat);
    break;

  case ERROR:
    format = malloc(strlen("Error : ")+strlen(xformat)+1);
    if (!format) mem_error();
    sprintf(format,"Error : %s",xformat);
    break;
  }

  va_list args;
#ifndef WIN32
  char *line = NULL;
#else
  char *line = malloc(8192);
  if (!line)
    mem_error();
#endif

  va_start(args, format);
#ifndef WIN32
  vasprintf(&line,format,args);
#else
  _vsnprintf(line,8192,format,args);
#endif
  va_end(args);

  line = realloc(line, strlen(line)+2);
  line[strlen(line)+1] = '\0';
  line[strlen(line)] = '\n';
  if (!graphing_ignore_errors)
    notify_error(line);
  free(line); free(format);
}


void hc_set_from_string(M_APM re, M_APM im, char *num)
{
  char *rp = hc_real_part(num);
  char *ip = hc_imag_part(num);
  if (ip)
    m_apm_set_string(im, ip);
  else
    m_apm_copy(im, MM_Zero);
  m_apm_set_string(re, rp);
  free(rp); free(ip);
}


char *hc_enlarge_buffer(char *buffer, unsigned int *sz)
{
  *sz += MAX_EXPR;
  buffer = realloc(buffer, *sz);
  if (!buffer) mem_error();
  return buffer;
}
