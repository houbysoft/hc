/*    (c) 2009 Jan Dlabal <dlabaljan@gmail.com>                          */
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

#include <gtk/gtk.h>
#include <string.h> // for strdup
#include "gui_main.h"
#include "gui_utils.h"


void notify(char *string)
{
  GtkWidget *dialog1 = gtk_message_dialog_new(
			NULL,
			GTK_DIALOG_MODAL,
                        GTK_MESSAGE_INFO,
                        GTK_BUTTONS_OK,
                        "%s",
			string);

  gtk_dialog_run (GTK_DIALOG (dialog1));
  gtk_widget_destroy (dialog1);
}

void notify_error(char *string)
{
  GtkWidget *dialog1 = gtk_message_dialog_new(
			NULL,
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_ERROR,
                        GTK_BUTTONS_OK,
                        "%s",
			string);

  gtk_dialog_run (GTK_DIALOG (dialog1));
  gtk_widget_destroy (dialog1);
}

int notify_ask(char *string)
{
  GtkWidget *dialog1 = gtk_message_dialog_new(
			NULL,
			GTK_DIALOG_MODAL,
                        GTK_MESSAGE_QUESTION,
                        GTK_BUTTONS_YES_NO,
                        string);

  int ret = gtk_dialog_run (GTK_DIALOG (dialog1));
  gtk_widget_destroy (dialog1);
  return ret; // either GTK_RESPONSE_NO or GTK_RESPONSE_YES
}

void prompt_respond(GtkWidget *entry, GtkDialog *dialog)
{
  gtk_dialog_response(GTK_DIALOG (dialog), GTK_RESPONSE_OK);
}

char *prompt(char *string)
{
  GtkWidget *dialog1 = gtk_message_dialog_new(NULL,
					      GTK_DIALOG_MODAL,
					      GTK_MESSAGE_QUESTION,
					      GTK_BUTTONS_OK,
					      "%s",
					      string);

  GtkWidget *entry = gtk_entry_new();
  g_signal_connect(G_OBJECT (entry), "activate", G_CALLBACK (prompt_respond), dialog1);
  gtk_box_pack_end_defaults(GTK_BOX (gtk_dialog_get_content_area(GTK_DIALOG (dialog1))), entry);
  gtk_widget_show(entry);
  gtk_dialog_run (GTK_DIALOG (dialog1));
  char *r = strdup((char *) gtk_entry_get_text(GTK_ENTRY (entry)));
  gtk_widget_destroy(entry);
  gtk_widget_destroy(dialog1);
  return r;
}

void menu_append(GtkMenu *menu, char *visible, GCallback callback, char *insert)
{
    GtkWidget *subm_mit = gtk_menu_item_new_with_label(visible);
    gtk_menu_append(menu, subm_mit);
    g_signal_connect(G_OBJECT (subm_mit), "activate", callback , insert);
}
