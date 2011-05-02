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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <m_apm.h>
#include <errno.h>
#include "hc.h"
#include "hc_functions.h"
#include "hash.h"
#include "hc_complex.h"
#include "hc_graph.h"
#include "hc_help.h"
#include "hc_list.h"
#include "hc_programming.h"
#include "hc_utils.h"
#include "hc_fnp.h"
#include "hc_conversions.h"


struct hc_ventry *hc_var_first;
char hc_check_funcname(char *e);
char hc_check_varname(char *e);
char hc_check_not_recursive(char *n, char *e);
int hc_clear(char *e);
int hc_get_first_index(char **indexes, unsigned long *index);
char *hc_varassign_replace_index(char *oldvalue, char **indexes, char *value);


void hc_varassign(char *e)
{
  char special = 0;
  unsigned int indexed = 0;
  if (!strchr(e,'='))
    varname_error();
  char *expr = strchr(e,'=') + sizeof(char);
  strchr(e,'=')[0] = '\0';
  char *var = e;

  if (!var || !expr)
    varname_error();

  if (var[strlen(var)-1]=='+' || var[strlen(var)-1]=='-' || var[strlen(var)-1]=='*' || var[strlen(var)-1]=='/' || var[strlen(var)-1]=='%')
  {
    special = var[strlen(var)-1];
    var[strlen(var)-1] = '\0';
  }

  // clean up var name
  unsigned int i = 0;
  int par = 0;
  while (var[i])
  {
    if (isspace(var[i]))
    {
      memmove((void *)(var + i),(void *)(var + i + 1),strlen((char *)(var + i + 1))+1);
    } else {
      if (var[i]=='[')
      {
        if (!indexed)
          indexed = i;
        par++;
      } else if (var[i]==']')
      {
        par--;
      }
      i++;
    }
  }

  if (par)
  {
    hc_error(SYNTAX,"['s and ]'s do not match");
    return;
  }
  if (indexed)
    var[indexed] = '\0';

  if (!hc_check_varname(var))
  {
    if (!hc_check_funcname(var))
    {
      varname_error();
    } else {
      // function
      if (special)
        var_nospecial_error();
      if (indexed)
      {
        hc_error(ERROR, "you can't index a function.");
        return;
      }
      char *name = strtok(var,"(");
      unsigned int name_hash = simple_hash(name);
      char *args = strtok(NULL,"(");
      args[strlen(args)-1]=0;

      if (!hc_check_not_recursive(name,expr))
        hc_error(WARNING,"recursive definition");

#ifdef DBG
      printf("[%s] [%s]\n",name,args);
#endif

      struct hc_ventry *tmp = hc_var_first;
      while ((tmp->next) && (tmp->name!=NULL) && (tmp->hash!=name_hash))
        tmp = tmp->next;
      if ((tmp->name!=NULL) && tmp->hash!=name_hash)
      {
        tmp->next = malloc(sizeof (struct hc_ventry));
        if (!tmp->next)
          mem_error();
        tmp = tmp->next;
        tmp->next = NULL;
        tmp->name = NULL;
      }
      
      tmp->type = HC_USR_FUNC;
      if ((tmp->name==NULL) || (tmp->hash!=name_hash))
      {
        // new function name
        tmp->name = strdup(name);
        if (!tmp->name)
          mem_error();
        tmp->hash = name_hash;
      } else {
        // function name already defined, just changing expression
        free(tmp->value);
        free(tmp->args);
      }
      tmp->args = strdup(args);
      if (!tmp->args)
        mem_error();
      tmp->value = strdup(expr);
      tmp->value = hc_impmul_resolve(tmp->value);
      if (!tmp->value)
        mem_error();
    }

  } else {

    // variable
    unsigned int name_hash = simple_hash(var);
    hc.flags |= PRINTFULL; // return all the digits that are used internally
    char *value = hc_result_(expr);
    hc.flags &= ~PRINTFULL;
    if (!value)
      return;

    if (special) // special means that one of these has been used: += -= *= /= %=
    {
      char *tmp = malloc(strlen(var)+1+(indexed ? 1 + strlen((char *)(var+indexed+1)) : 0) + strlen(value)+1);
      sprintf(tmp,"%s%s%s%c%s",var,indexed ? "[" : "",indexed ? (char *)(var+indexed+1) : "",special,value);
      free(value);
      value = hc_result_(tmp);
      free(tmp);
      if (!value)
        return;
    }

    struct hc_ventry *tmp = hc_var_first;
    while ((tmp->next) && (tmp->name!=NULL) && (tmp->hash!=name_hash))
      tmp = tmp->next;
    if ((tmp->name!=NULL) && (tmp->hash!=name_hash))
    {
      tmp->next = malloc(sizeof (struct hc_ventry));
      if (!tmp->next)
        mem_error();
      tmp = tmp->next;
      tmp->next = NULL;
      tmp->name = NULL;
    }

    tmp->type = HC_USR_VAR;
    tmp->args = NULL;
    if ((tmp->name==NULL) || (tmp->hash!=name_hash))
    {
      // new variable name
      if (indexed)
      {
        tmp->name = NULL;
        free(value);
        hc_error(ERROR, "Variable %s does not exist yet you are trying to index it.",var);
        return;
      }
      tmp->name = malloc((strlen(var)+1) * sizeof(char));
      if (!tmp->name)
        mem_error();
      strcpy(tmp->name,var);
      tmp->hash = name_hash;
      tmp->value = value;
    } else if (!indexed) {
      // variable name already defined, just changing value, but not indexing
      free(tmp->value);
      tmp->value = value;
    } else {
      // variable name already defined, changing value of one of its members (by indexing)
      char *indexes = var + indexed + 1;
      char *newvalue = hc_varassign_replace_index(tmp->value, &indexes, value);
      free(value);
      if (newvalue)
      {
        free(tmp->value);
        tmp->value = newvalue;
      }
    }
  }
}


char *hc_varassign_replace_index(char *oldvalue, char **indexes, char *value)
{
  char *newvalue = NULL;
  unsigned long i = 0;
  unsigned long changeindex;
  char donereplacing = FALSE;
  int done = hc_get_first_index(indexes, &changeindex);
  if (done == -1)
  {
    return NULL;
  }
  unsigned long curindex;
  if (!done)
  {
    if (oldvalue[i] != '\"' && oldvalue[i] != '[')
    {
      hc_error(ERROR, "Only strings and vectors can be indexed");
      return NULL;
    } else if (oldvalue[i] == '\"') {
      i++;
      curindex = 0;
      while (curindex != changeindex)
      {
        if (oldvalue[i+1] == '\"') // since i is ++'d below
        {
          hc_error(ERROR, "Index out of range.");
          return NULL;
        } else {
          i++; curindex++;
        }
      }
      char *tmpval = get_string(value);
      if (!tmpval || tmpval[1]!='\0')
      {
        hc_error(ERROR, "You can only replace a character in a string by another character");
        free(tmpval);
        return NULL;
      }
      newvalue = strdup(oldvalue);
      newvalue[i] = tmpval[0];
      free(tmpval);
      donereplacing = TRUE;

    } else {

      int par = 1;
      char ignore = FALSE;
      i++;
      unsigned long curindex = 0;
      unsigned long starts_at = 0, ends_at = 0;
      while (curindex != changeindex && oldvalue[i])
      {
        if (oldvalue[i] == '\"')
          ignore = ignore == FALSE ? TRUE : FALSE;
        else if (oldvalue[i] == '[' && !ignore)
          par++;
        else if (oldvalue[i] == ']' && !ignore)
          par--;
        else if (oldvalue[i] == ',' && !ignore && par == 1)
          curindex++;
        i++;
      }
      if (curindex != changeindex)
      {
        hc_error(SYNTAX, "malformed index(es)");
        return NULL;
      }
      starts_at = i;
      ignore = FALSE;
      while (!ends_at)
      {
        if (oldvalue[i] == '\"')
          ignore = ignore == FALSE ? TRUE : FALSE;
        else if (oldvalue[i] == '[' && !ignore)
          par++;
        else if (oldvalue[i] == ']' && !ignore)
          par--;

        if ((oldvalue[i] == ',' && !ignore && par == 1) || (oldvalue[i] == ']' && par == 0))
          ends_at = i;

        i++;
      }
      char *newold = malloc(ends_at - starts_at + 1);
      if (!newold) mem_error();
      strncpy(newold, oldvalue + starts_at, ends_at - starts_at);
      newold[ends_at - starts_at] = '\0';
      char *tmp = hc_varassign_replace_index(newold, indexes, value);
      free(newold);
      if (!tmp)
      {
        return NULL;
      }
      newvalue = malloc(starts_at + 1 + strlen(tmp) + strlen(oldvalue) - ends_at);
      if (!newvalue) mem_error();
      strncpy(newvalue, oldvalue, starts_at);
      newvalue[starts_at] = '\0';
      strcat(newvalue, tmp);
      free(tmp);
      strcat(newvalue, oldvalue + ends_at);
    }

    done = hc_get_first_index(indexes, &changeindex);
    if (done == -1)
    {
      return NULL;
    } else if (done == 0 && donereplacing == TRUE)
    {
      hc_error(ERROR,"index too deep");
      return NULL;
    } else {
      return newvalue;
    }
  } else {
    return hc_result_(value);
  }
}


char hc_check_funcname(char *f)
{
  // strtok modifies its argument, this would mess up the rest, so let's copy f to a temporary buffer
  char *e = malloc((strlen(f)+1)*sizeof(char));
  strcpy(e,f);

  if (strchr(e,'(')==NULL || e[strlen(e)-1]!=')')
  {
    free(e);
    return 0;
  }

  char *name = strtok(e,"(");
  char *args = strtok(NULL,"(");
  args[strlen(args)-1]=0; // delete the trailing )

  if (!hc_check_varname(name))
  {
    free(e);
    return 0;
  }

  char *TMP = args;
  char *checkme;
  while ((checkme = strtok(TMP,","))!=NULL)
  {
    if (TMP)
      TMP = NULL;
    if (!hc_check_varname(checkme))
    {
      free(e);
      return 0;
    }
  }

  free(e);
  return 1;
}


char hc_check_varname(char *e)
{
  if (hc_is_predef(e))
  {
    if (!graphing_ignore_errors)
      varname_predefined_error()
    else
      return 0;
  }

  unsigned int i = 0;
  while (e[i]!=0)
  {
    if ((i==0) && ((!isalpha(e[i])) || (tolower(e[i])=='e' && !isalpha(e[i+1])) || (tolower(e[i])=='i' && !isalpha(e[i+1]))))
      return 0;
    else
      if (!isalnum(e[i]))
        return 0;
    i++;
  }

  if (i >= 64)
    return 0;
  else
    return 1;
}


char hc_check_not_recursive(char *n, char *e)
{
  // this functions has to check for definitions which would lead to recursion, such as:
  // f(x) = f(x) + 1
  char *found = e;
  while ((found = strstr(found,n))!=NULL)
  {
    if ((found != e) && ((!isalpha((char)(found-sizeof(char))[0])) && !isalpha((char)(found+strlen(n)*sizeof(char))[0])))
      return 0;
    if ((found == e) && !isalpha((char)(found + strlen(n)*sizeof(char))[0]))
      return 0;
    found++;
  }
  return 1;
}


int hc_clear(char *e)
{
  unsigned int hash = simple_hash(e);
  struct hc_ventry *curr = hc_var_first;
  struct hc_ventry *tmp = NULL;
  char done = FALSE;
  if (curr->hash == hash)
  {
    free(curr->name);
    free(curr->value);
    free(curr->args);
    hc_var_first = curr->next;
    free(curr);
    if (!hc_var_first)
    {
      hc_var_first = malloc(sizeof (struct hc_ventry));
      if (!hc_var_first)
        mem_error();
      hc_var_first->next = NULL;
      hc_var_first->type = 0;
      hc_var_first->value = NULL;
      hc_var_first->name = NULL;
      hc_var_first->args = NULL;
      hc_var_first->hash = 0;
    }
    done = TRUE;
  } else {
    while (curr->next)
    {
      if (hash == curr->next->hash)
      {
        free(curr->next->name);
        free(curr->next->value);
        free(curr->next->args);
        tmp = curr->next->next;
        free(curr->next);
        curr->next = tmp;
        done = TRUE;
        break;
      }
      curr = curr->next;
    }
  }
  if (!done)
  {
    arg_error("clear() : argument is already not defined.");
  } else {
    return SUCCESS;
  }
}


int hc_get_first_index(char **indexes, unsigned long *index)
{
  if (!*indexes[0])
  {
    return 1; // no more indexes available, we are done
  } else {
    int par = 1;
    char *tmp = malloc(strlen(*indexes)+1); // maximum length (strlen()+1-2, where -2 because of the [])
    unsigned int i = 0;
    char ignore = FALSE;
    while (par && *indexes[0])
    {
      tmp[i++] = *indexes[0];
      if (*indexes[0] == '\"')
        ignore = ignore == FALSE ? TRUE : FALSE;
      else if (*indexes[0] == '[' && !ignore)
        par++;
      else if (*indexes[0] == ']' && !ignore)
        par--;
      *indexes += 1;
    }
    if (par != 0)
    {
      free(tmp);
      hc_error(SYNTAX, "['s and ]'s do not match");
      return -1;
    }
    tmp[i-1] = '\0';
    if (*indexes[0] == '[')
      *indexes += 1;
    char *tmpres = hc_result_(tmp);
    free(tmp);
    if (!tmpres || !is_positive_int(tmpres))
    {
      free(tmpres);
      hc_error(ERROR, "indexes must be positive integers (or zero)");
      return -1;
    }
    errno = 0;
    *index = strtol(tmpres, NULL, 10);
    if (errno)
    {
      free(tmpres);
      hc_error(ERROR, "%s", strerror(errno));
      return -1;
    }
    free(tmpres);
    return 0;
  }
}


char *hc_value(char *type, char *v_name, char *f_expr)
{
  unsigned int v_hash = simple_hash(v_name);

  if (!strlen(f_expr)) // variable
  {
    unsigned int i = HC_NAMES_CNST_START;
    for (; i <= HC_NAMES_CNST_STOP; i++) // check built-in constants
    {
      if (hc_hashes[i]==0)
        hc_hashes[i] = simple_hash((char *)hc_names[i][0]);
      if (v_hash == hc_hashes[i])
      {
        *type = HC_VAR_NUM; // all hc constants are numbers
        if (isdigit(hc_names[i][1][6]))
          return strdup((char *)hc_names[i][1] + strlen("cnst:"));
        else {
          return hc_result_((char *)hc_names[i][1] + strlen("cnst:"));
        }
      }
    }

    struct hc_ventry *var_tmp = hc_var_first;
    while (var_tmp && var_tmp->name)
    {
      if (var_tmp->type == HC_USR_VAR && var_tmp->hash == v_hash)
      {
        char *result = strdup(var_tmp->value);
        if (is_num(result))
          *type = HC_VAR_NUM;
        else if (is_string(result))
          *type = HC_VAR_STR;
        else if (is_vector(result))
          *type = HC_VAR_VEC;
        else {
          notify("Invalid variable type. This is most likely a bug, please report it.\n");
          free(result);
          return NULL;
        }
        return result;
      }
      var_tmp = var_tmp->next;
    }

    hc_error(ERROR, "Variable %s is undefined.", v_name);
    return 0;

  } else { // function

    f_expr++; // skip first ')'
    f_expr[strlen(f_expr)-1] = '\0'; // delete last ')'

    M_APM f_result_re = m_apm_init();
    M_APM f_result_im = m_apm_init();
    M_APM tmp_num_re = m_apm_init();
    M_APM tmp_num_im = m_apm_init();
    char *f_result_str = NULL;
    char f_result_is_simplified = FALSE;
    char *fme;
    char *tmp_ri;
    char success = TRUE;
    *type = HC_VAR_NUM; // you are a number until proven otherwise

    switch (v_hash)
    {
    case HASH_ABS:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
        m_apm_set_string(tmp_num_im,tmp_ri);
        free(tmp_ri);
        m_apm_copy(f_result_im,MM_Zero);
        m_apmc_abs(f_result_re,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      } else {
        m_apm_copy(f_result_im,MM_Zero);
        m_apm_absolute_value(f_result_re,tmp_num_re);
      }
      break;

    case HASH_FLOOR:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
        m_apm_set_string(tmp_num_im,tmp_ri);
        free(tmp_ri);
      } else {
        m_apm_copy(f_result_im,MM_Zero);
      }
      m_apmc_floor(f_result_re,f_result_im,tmp_num_re,tmp_num_im);
      break;

    case HASH_CEIL:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
        m_apm_set_string(tmp_num_im,tmp_ri);
        free(tmp_ri);
      } else {
        m_apm_copy(f_result_im,MM_Zero);
      }
      m_apmc_ceil(f_result_re,f_result_im,tmp_num_re,tmp_num_im);
      break;

    case HASH_ROUND:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
        m_apm_set_string(tmp_num_im,tmp_ri);
        free(tmp_ri);
      } else {
        m_apm_copy(f_result_im,MM_Zero);
      }
      m_apmc_round(f_result_re,f_result_im,tmp_num_re,tmp_num_im);
      break;

    case HASH_ANS:
      if (hc_lans_type == HC_VAR_NUM)
      {
        m_apm_copy(f_result_re,hc_lans_mapm_re);
        m_apm_copy(f_result_im,hc_lans_mapm_im);
      } else if (hc_lans_type == HC_VAR_VEC || hc_lans_type == HC_VAR_STR) {
        *type = hc_lans_type;
        f_result_str = strdup(hc_lans_strvec);
      }
      break;

    case HASH_ACOS:
    case HASH_ARCCOS:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
        m_apm_set_string(tmp_num_im,tmp_ri);
        free(tmp_ri);
      } else {
        m_apm_set_string(tmp_num_im,"0");
      }
      if (m_apm_compare(tmp_num_im,MM_Zero)==0 && m_apm_compare(tmp_num_re,MM_One)!=1 && m_apm_compare(tmp_num_re,MM_MOne)!=-1)
        m_apm_acos(f_result_re,HC_DEC_PLACES,tmp_num_re);
      else
        m_apmc_acos(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);

      if (m_apm_compare(f_result_im,MM_Zero)!=0 && hc.angle!='r')
      {
        m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
        m_apm_free(f_result_re); m_apm_free(f_result_im);
        arg_error("acos() : Domain error. Please switch to RAD mode with \\rad for complex results.");
      } else {
        hc_from_rad(f_result_re);
      }
      break;

    case HASH_ASIN:
    case HASH_ARCSIN:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
        m_apm_set_string(tmp_num_im,tmp_ri);
        free(tmp_ri);
      } else {
        m_apm_set_string(tmp_num_im,"0");
      }
      if (m_apm_compare(tmp_num_im,MM_Zero)==0 && m_apm_compare(tmp_num_re,MM_One)!=1 && m_apm_compare(tmp_num_re,MM_MOne)!=-1)
        m_apm_asin(f_result_re,HC_DEC_PLACES,tmp_num_re);
      else
        m_apmc_asin(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);

      if (m_apm_compare(f_result_im,MM_Zero)!=0 && hc.angle!='r')
      {
        m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
        m_apm_free(f_result_re); m_apm_free(f_result_im);
        arg_error("asin() : Domain error. Please switch to RAD mode with \\rad for complex results.");
      } else {
        hc_from_rad(f_result_re);
      }
      break;

    case HASH_ATAN:
    case HASH_ARCTAN:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
        m_apm_set_string(tmp_num_im,tmp_ri);
        free(tmp_ri);
      } else {
        m_apm_set_string(tmp_num_im,"0");
      }
      if (m_apm_compare(tmp_num_im,MM_Zero)==0)
        m_apm_atan(f_result_re,HC_DEC_PLACES,tmp_num_re);
      else
        m_apmc_atan(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      if (m_apm_compare(f_result_im,MM_Zero)!=0 && hc.angle!='r')
      {
        m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
        m_apm_free(f_result_re); m_apm_free(f_result_im);
        arg_error("atan() : Domain error. Please switch to RAD mode with \\rad for complex results.");
      } else {
        hc_from_rad(f_result_re);
      }
      break;

    case HASH_COS:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
        m_apm_set_string(tmp_num_im,tmp_ri);
        free(tmp_ri);
      } else {
        m_apm_set_string(tmp_num_im,"0");
      }
      if (m_apm_compare(tmp_num_im,MM_Zero)!=0 && hc.angle!='r')
      {
        m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
        m_apm_free(f_result_re); m_apm_free(f_result_im);
        arg_error("cos() : Domain error. Please switch to RAD mode with \\rad for complex results.");
      } else {
        hc_to_rad(tmp_num_re);
        m_apmc_cos(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      }
      break;

    case HASH_EXP:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
        m_apm_set_string(tmp_num_im,tmp_ri);
        free(tmp_ri);
      } else {
        m_apm_set_string(tmp_num_im,"0");
      }
      m_apmc_exp(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      break;

    case HASH_FACTORIAL:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
        m_apm_set_string(tmp_num_im,tmp_ri);
        free(tmp_ri);
      } else {
        m_apm_set_string(tmp_num_im,"0");
      }
      if (!m_apm_is_integer(tmp_num_re) || m_apm_compare(tmp_num_im,MM_Zero)!=0)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); arg_error("factorial() : an integer is required."); return 0;}
      m_apm_factorial(f_result_re,tmp_num_re);
      m_apm_copy(f_result_im,MM_Zero);
      break;

    case HASH_GCD:
      if (hc_gcd(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_LCM:
      if (hc_lcm(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_LN:
      fme = hc_result_(f_expr);
      if (!fme) {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
        m_apm_set_string(tmp_num_im,tmp_ri);
        free(tmp_ri);
      } else {
        m_apm_set_string(tmp_num_im,"0");
      }
      if (m_apm_compare(tmp_num_re,MM_Zero) == 0 && m_apm_compare(tmp_num_im,MM_Zero) == 0)
      {
        hc_error(ARG, "ln(0) - negative infinity result encountered");
        {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      }
      m_apmc_log(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      break;

    case HASH_LOG:
      if (hc_log(f_result_re, f_result_im, f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_LOG10:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
        m_apm_set_string(tmp_num_im,tmp_ri);
        free(tmp_ri);
      } else {
        m_apm_set_string(tmp_num_im,"0");
      }
      if (m_apm_compare(tmp_num_re,MM_Zero) == 0 && m_apm_compare(tmp_num_im,MM_Zero) == 0)
      {
        hc_error(ARG, "ln(0) - negative infinity result encountered");
        {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      }
      m_apmc_log10(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      break;

    case HASH_SIN:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
        m_apm_set_string(tmp_num_im,tmp_ri);
        free(tmp_ri);
      } else {
        m_apm_set_string(tmp_num_im,"0");
      }
      if (m_apm_compare(tmp_num_im,MM_Zero)!=0 && hc.angle!='r')
      {
        m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
        m_apm_free(f_result_re); m_apm_free(f_result_im);
        arg_error("sin() : Domain error. Please switch to RAD mode with \\rad for complex results.");
      } else {
        hc_to_rad(tmp_num_re);
        m_apmc_sin(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      }
      break;

    case HASH_TAN:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
        m_apm_set_string(tmp_num_im,tmp_ri);
        free(tmp_ri);
      } else {
        m_apm_set_string(tmp_num_im,"0");
      }
      if (m_apm_compare(tmp_num_im,MM_Zero)!=0 && hc.angle!='r')
      {
        m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
        m_apm_free(f_result_re); m_apm_free(f_result_im);
        arg_error("tan() : Domain error. Please switch to RAD mode with \\rad for complex results.");
      } else {
        hc_to_rad(tmp_num_re);
        m_apmc_tan(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      }
      break;

    case HASH_SUM:
      if (hc_sum(f_result_re,f_result_im,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_PRODUCT:
      if (hc_product(f_result_re,f_result_im,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_COSH:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
        m_apm_set_string(tmp_num_im,tmp_ri);
        free(tmp_ri);
      } else {
        m_apm_set_string(tmp_num_im,"0");
      }
      if (m_apm_compare(tmp_num_im,MM_Zero)!=0 && hc.angle!='r')
      {
        m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
        m_apm_free(f_result_re); m_apm_free(f_result_im);
        arg_error("cosh() : Domain error. Please switch to RAD mode with \\rad for complex results.");
      } else {
        m_apmc_cosh(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      }
      break;

    case HASH_SINH:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
        m_apm_set_string(tmp_num_im,tmp_ri);
        free(tmp_ri);
      } else {
        m_apm_set_string(tmp_num_im,"0");
      }
      if (m_apm_compare(tmp_num_im,MM_Zero)!=0 && hc.angle!='r')
      {
        m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
        m_apm_free(f_result_re); m_apm_free(f_result_im);
        arg_error("sinh() : Domain error. Please switch to RAD mode with \\rad for complex results.");
      } else {
        m_apmc_sinh(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      }
      break;

    case HASH_TANH:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
        m_apm_set_string(tmp_num_im,tmp_ri);
        free(tmp_ri);
      } else {
        m_apm_set_string(tmp_num_im,"0");
      }
      if (m_apm_compare(tmp_num_im,MM_Zero)!=0 && hc.angle!='r')
      {
        m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
        m_apm_free(f_result_re); m_apm_free(f_result_im);
        arg_error("tanh() : Domain error. Please switch to RAD mode with \\rad for complex results.");
      } else {
        m_apmc_tanh(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      }
      break;

    case HASH_SQRT:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
        m_apm_set_string(tmp_num_im,tmp_ri);
        free(tmp_ri);
      } else {
        m_apm_set_string(tmp_num_im,"0");
      }
      m_apmc_sqrt(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im);
      break;

    case HASH_CBRT:
      fme = hc_result_(f_expr);
      if (!fme) { m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
            tmp_ri = hc_real_part(fme);
      m_apm_set_string(tmp_num_re,tmp_ri);
      free(tmp_ri);
      tmp_ri = hc_imag_part(fme);
      free(fme);
      if (tmp_ri)
      {
        m_apm_set_string(tmp_num_im,tmp_ri);
        free(tmp_ri);
      } else {
        m_apm_set_string(tmp_num_im,"0");
      }
      m_apmc_root(f_result_re,f_result_im,HC_DEC_PLACES,tmp_num_re,tmp_num_im,3,3);
      break;

    case HASH_DOTP: // dot/scalar product
      if (hc_dotp(f_result_re,f_result_im,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_CROSSP: // cross product
      if ((f_result_str = hc_crossp(f_expr)) == NULL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      *type = HC_VAR_VEC;
      break;

    case HASH_STR:
      if ((f_result_str = hc_2str(f_expr)) == NULL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      *type = HC_VAR_STR;
      break;

    case HASH_NUM:
      if (hc_2num(f_result_re,f_result_im,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_RE:
    case HASH_REAL:
      if (hc_reim(REAL,f_result_re,f_result_im,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_IM:
    case HASH_IMAG:
      if (hc_reim(IMAG,f_result_re,f_result_im,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_MOD:
      if (hc_modulus(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_FIBO:
      if (hc_fibo(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_TOTIENT:
      if (hc_totient(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_GAMMA:
      if (hc_gamma(f_result_re,f_result_im,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_IF:
      if (hc_if(f_expr,f_result_re,f_result_im,&f_result_str,type) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_FOR:
      if (hc_for(f_expr,f_result_re,f_result_im,&f_result_str,type) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_WHILE:
      if (hc_while(f_expr,f_result_re,f_result_im,&f_result_str,type) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_CONVERT:
      if (hc_convert(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_CHARTOCODE:
      if (hc_char2code(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_CODETOCHAR:
      *type = HC_VAR_STR;
      if ((f_result_str = hc_code2char(f_expr)) == NULL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_NCR:
      if (hc_binomc(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_NPR:
      if (hc_permutations(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_ONES:
      *type = HC_VAR_VEC;
      f_result_is_simplified = TRUE;
      if ((f_result_str = hc_constantlist(f_expr,"1")) == NULL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_ZEROS:
      *type = HC_VAR_VEC;
      f_result_is_simplified = TRUE;
      if ((f_result_str = hc_constantlist(f_expr,"0")) == NULL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_RAND:
      if (hc_rand(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_SORT:
      *type = HC_VAR_VEC;
      f_result_is_simplified = TRUE;
      if ((f_result_str = hc_sort(f_expr)) == NULL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_HELP:
      hc_help(f_expr);
      *type = HC_VAR_EMPTY;
      break;

    case HASH_PRINT:
      hc_output(PRINT, f_expr);
      *type = HC_VAR_EMPTY;
      break;

    case HASH_WRITE:
      hc_output(WRITE, f_expr);
      *type = HC_VAR_EMPTY;
      break;

    case HASH_INPUT:
      if (hc_input(f_result_re,f_result_im,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_JOIN:
      if ((f_result_str = hc_join(f_expr)) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      *type = HC_VAR_VEC;
      f_result_is_simplified = TRUE;
      break;

    case HASH_LENGTH:
      if (hc_length(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_COUNT:
      if (hc_count(f_result_re,f_expr) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_MAP:
      if ((f_result_str = hc_map(f_expr)) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      *type = HC_VAR_VEC;
      f_result_is_simplified = TRUE;
      break;

    case HASH_RANGE:
      if ((f_result_str = hc_range(f_expr)) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      *type = HC_VAR_VEC;
      f_result_is_simplified = TRUE;
      break;

    case HASH_LINREG:
      if ((f_result_str = hc_linreg(f_expr)) == FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      *type = HC_VAR_VEC;
      f_result_is_simplified = TRUE;
      break;

    case HASH_GRAPH:
      hc_graph(f_expr);
      *type = HC_VAR_EMPTY;
      break;

    case HASH_GMUL:
      hc_graph_n(f_expr);
      *type = HC_VAR_EMPTY;
      break;

    case HASH_GRAPH3:
      hc_graph3d(f_expr);
      *type = HC_VAR_EMPTY;
      break;

    case HASH_GRAPHV:
    case HASH_GRAPHVALUES:
      hc_graph_values(f_expr,1); // 1 is for lines
      *type = HC_VAR_EMPTY;
      break;

    case HASH_GRAPHP:
    case HASH_GRAPHPOINTS:
      hc_graph_values(f_expr,0); // 0 is for points
      *type = HC_VAR_EMPTY;
      break;

    case HASH_SLPFLD:
      hc_graph_slpfld(f_expr);
      *type = HC_VAR_EMPTY;
      break;

    case HASH_GRAPHPEQ:
      hc_graph_peq(f_expr);
      *type = HC_VAR_EMPTY;
      break;

    case HASH_GRAPHPOLAR:
      hc_graph_polar(f_expr);
      *type = HC_VAR_EMPTY;
      break;

    case HASH_STATS:
      hc_stats(f_expr, FALSE);
      *type = HC_VAR_EMPTY;
      break;

    case HASH_BOXPLOT:
      hc_stats(f_expr, TRUE);
      *type = HC_VAR_EMPTY;
      break;

    case HASH_CLEAR:
      if (hc_clear(f_expr)==FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      *type = HC_VAR_EMPTY;
      break;

    case HASH_MMASS:
      if (hc_mmass(f_result_re,f_expr)==FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    case HASH_ATOMICNUMBER:
      if (hc_atomicnumber(f_result_re,f_expr)==FAIL)
      {m_apm_free(tmp_num_re); m_apm_free(tmp_num_im); m_apm_free(f_result_re); m_apm_free(f_result_im); return 0;}
      break;

    default:
      success = FALSE;
      break;
    }

    char *result = NULL;
    if (success)
    {
      if (*type == HC_VAR_NUM)
      {
        char *f_result_tmp_re = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,f_result_re,'.',0,0);
        if (f_result_tmp_re[0]=='-')
          f_result_tmp_re[0] = '_';
        char *f_result_tmp_im;
        if (m_apm_compare(f_result_im,MM_Zero)!=0)
        {
          f_result_tmp_im = m_apm_to_fixpt_stringexp(HC_DEC_PLACES,f_result_im,'.',0,0);
          if (f_result_tmp_im[0]=='-')
            f_result_tmp_im[0] = '_';
          // re i im \0
          result = malloc(strlen(f_result_tmp_re) + 1 + strlen(f_result_tmp_im) + 1);
          if (!result) mem_error();
          sprintf(result,"%si%s", f_result_tmp_re, f_result_tmp_im);
          free(f_result_tmp_re); free(f_result_tmp_im);
        } else {
          result = f_result_tmp_re;
        }
      } else if (*type == HC_VAR_STR)
      {
        result = f_result_str;
      } else if (*type == HC_VAR_VEC) {
        if (!f_result_is_simplified)
        {
          char *newlist = list_simplify(f_result_str);
          if (newlist)
          {
            result = newlist;
            free(f_result_str);
          } else {
            free(f_result_str); free(newlist);
            m_apm_free(f_result_re); m_apm_free(f_result_im); m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
            return NULL;
          }
        } else {
          result = f_result_str;
        }
      } else if (*type == HC_VAR_EMPTY)
      {
        result = strdup("");
      }
      m_apm_free(f_result_re); m_apm_free(f_result_im); m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);
      return result;
    } else { // check if not a user-defined function
      m_apm_free(f_result_re); m_apm_free(f_result_im); m_apm_free(tmp_num_re); m_apm_free(tmp_num_im);

      struct hc_ventry *var_tmp = hc_var_first;
      while (var_tmp && var_tmp->name)
      {
        if (var_tmp->type == HC_USR_FUNC && var_tmp->hash == v_hash)
        {
          // f_expr
          char *t1, *t2;
          unsigned int k=1;
          t1 = hc_get_arg_r(f_expr,k);
          t2 = hc_get_arg(var_tmp->args,k);
          char *res_expr = strdup(var_tmp->value);
          if (!res_expr)
            mem_error();
          while (t1 && t2)
          {
            char *fme = res_expr;
            res_expr = strreplace(res_expr,t2,t1);
            free(fme);
            free(t1); free(t2);
            k++;
            t1 = hc_get_arg(f_expr,k);
            t2 = hc_get_arg(var_tmp->args,k);
          }
          if (t1 || t2)
          {
            arg_error_custom();
            free(t1); free(t2); free(res_expr);
            return NULL;
          } else {
            char *res_of_expr = hc_result_(res_expr);
            free(res_expr);
            if (!res_of_expr)
              return NULL;
            else {
              result = res_of_expr;
              if (is_num(result))
                *type = HC_VAR_NUM;
              else if (is_string(result))
                *type = HC_VAR_STR;
              else if (is_vector(result))
                *type = HC_VAR_VEC;
              else if (!strlen(result))
        *type = HC_VAR_EMPTY;
          else {
                notify("Invalid variable type. This is most likely a bug, please report it.\n");
                free(result);
                return NULL;
              }
              return result;
            }
          }
        }
        var_tmp = var_tmp->next;
      }

      hc_error(ERROR, "Function %s is undefined.", v_name);
      return NULL;
    }
  }
}
