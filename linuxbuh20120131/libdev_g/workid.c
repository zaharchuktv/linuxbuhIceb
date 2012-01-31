/*$Id: workid.c,v 1.3 2011-01-13 13:50:01 sasa Exp $*/

#include <stdio.h>
#include <sys/types.h>
#include <utmpx.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define DEV_DIR_WITH_TRAILING_SLASH "/dev/"
#define DEV_DIR_LEN (sizeof (DEV_DIR_WITH_TRAILING_SLASH) - 1)

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 255
#endif

/*
 * Функция workid возвращает указатель на строку, идентифицирующую
 * рабочее место, на котором запущена программа, в формате:
 *
 * хост|удаленный_хост|терминал
 *   |         |           |
 *   |         |       устройство терминальной линии (если не в X)
 *   |         |            
 *   |         +--- адрес удаленного терминала
 *   |
 *   +--- на котором запущена программа
 *
 * либо NULL, если идентификация невозможна.
 *
 */

char *workid() 
{
   struct utmpx *u;
   char *ttyname_b = NULL;
   char host[HOST_NAME_MAX];
   char *host_remote, *term_line;
//   int n = 0;
   int found = 0;
   
   if ((ttyname_b = getenv("WORKID")) == NULL) {
     
     host[0] = '\0';
     gethostname(host, HOST_NAME_MAX-1);
     host_remote = getenv("DISPLAY");
     if (host_remote == NULL) {
       /* run in text mode */
       ttyname_b = ttyname (0);
       if (ttyname_b) {
         /* program has controlled terminal */
         if (strncmp (ttyname_b, DEV_DIR_WITH_TRAILING_SLASH, DEV_DIR_LEN) == 0)
            ttyname_b += DEV_DIR_LEN;	/* Discard /dev/ prefix.  */

         while ((u=getutxent()) != NULL)
          if (u->ut_type == USER_PROCESS ) 
               if (strcmp(u->ut_line, ttyname_b) == 0) {
                  found++;
                  break;
               }
         if (found) {
              host_remote = u->ut_host;
              term_line=u->ut_line;
           }
       }
     } else {
       /* run in X mode */
       found++;
       term_line = "";
     }
     
     if (found) {
        found=strlen(host)+1+strlen(host_remote)+1+strlen(term_line)+1;
        ttyname_b = (char*) malloc(found);
        sprintf(ttyname_b, "%s|%s|%s", host, host_remote, term_line);
     }
     else ttyname_b = NULL;
   }
   return(ttyname_b);
}
