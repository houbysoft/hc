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
#include <hul.h>
#include "../hc.h"


void printstatus(int status)
{
  static int lastp = -1;
  if (status <= 100 && status >= 0)
  {
    if (status != lastp)
    {
      printf("\r%i%% downloaded",status);
      lastp = status;
    }
  } else if (status == HUL_STATUS_CONNECTING) {
    printf("\nConnecting to update server...\n\n");
  } else if (status == HUL_STATUS_DOWNLOAD_IN_PROGRESS) {
    printf("\nDownloading update...\n");
  } else if (status == HUL_STATUS_DOWNLOAD_FINISHED) {
    printf("\r               \nDownload finished.\n");
  }
}


int main()
{
  printf("Welcome to the HC updater!\nChecking for updates...\n");
  HUL *update = hul_checkupdates(VERSION,STATUS_URL_GUI);
  if (update && update->version)
  {
    printf("Update found (newest version is %s). Please wait while the update is downloaded and installed.\n",update->version);
    if (!hul_applyupdate(update,printstatus))
    {
      printf("\r               \n\nAn error occured while downloading or veryfing the update. Please try again later.\n");
    } else {
      return;
    }
  } else if (update) {
    printf("\nYou have the latest version.\n");
  } else {
    printf("\nAn error occured while checking for updates.\n");
  }
  system("pause");
  return 0;
}
