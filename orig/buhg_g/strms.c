/*$Id: strms.c,v 1.6 2011-02-21 07:35:57 sasa Exp $*/
/*17.11.2009	03.01.2008	Белых А.И.	strms.c
Выдача строки месяцев в ведомостях амортизационных отчислений
*/

#include "buhg_g.h"


void            strms(short mt, //0-с номером месяца 1 - без
short mn,short mk,FILE *ff)
{
short           i;
char		bros[512];

memset(bros,'\0',sizeof(bros));
if(mt == 0)
 {
  for(i=mn-1; i< mk ; i++)
    fprintf(ff,"    %-6d|",i+1);

  if(mn != mk)
   fprintf(ff,"%-*s|",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));
 }
if(mt == 1)
 {
  for(i=mn-1; i< mk ; i++)
   fprintf(ff,"%10s|"," ");

  if(mn != mk)
    fprintf(ff,"%10s|"," ");
 }
fprintf(ff,"\n");
}
