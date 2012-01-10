/*$Id: strpod.c,v 1.3 2011-01-13 13:49:54 sasa Exp $*/
/*03.01.2008	03.01.2008	Белых А.И.	strpod.c
подчёркивание в ведомостях амортизационных отчислений
*/
#include <stdio.h>
#include <string.h>

void strpod(short mn,short mk,short dlu,FILE *ff)
{
short           i;
char		bros[512];

memset(bros,'\0',sizeof(bros));
for(i=0; i< dlu ; i++)
 strcat(bros,"-");
for(i=mn-1; i<mk ;i++)
  fprintf(ff,"%s",bros);
if(mn != mk)
  fprintf(ff,"%s",bros);
fprintf(ff,"\n");
}
