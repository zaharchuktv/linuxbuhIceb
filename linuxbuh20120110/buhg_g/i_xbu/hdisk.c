/*$Id: hdisk.c,v 1.5 2011-02-21 07:35:51 sasa Exp $*/
/*14.01.2010	28.08.2003	Белых А.И.	Белых А.И.
Добавление заголовка и концовки в файл который записывается на дискету для передачи в банк
*/
#include	<stdio.h>
#include	<errno.h>
#include	<time.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include <unistd.h>


void hdisk(int nomer,double itogoo,const char *imafkartf)
{
struct tm *bf;
char imaftmp[40];
FILE *fftmp;
FILE *ffkartf;
char strsql[512];
time_t  vrem;

time(&vrem);
bf=localtime(&vrem);


sprintf(imaftmp,"pib%d.tmp",getpid());

if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,NULL);
  return;
 }

if((ffkartf = fopen(imafkartf,"r")) == NULL)
 {
  iceb_er_op_fil(imafkartf,"",errno,NULL);
  return;
 }

fprintf(fftmp,"\
CODE: 048\n\
FILE: UOBLUAAM.F\n\
DATE: %02d/%02d/%04d\n\
CURR: 980\n\
CARD: %d\n\
AMNT: %.2f\n\
TEXT:\n\
START:\n",
bf->tm_mday,
bf->tm_mon+1,
bf->tm_year+1900,
nomer,itogoo);

while(fgets(strsql,sizeof(strsql),ffkartf) != NULL)
   fprintf(fftmp,"%s",strsql);
  
fprintf(fftmp,"END:\n");
fclose(fftmp);
fclose(ffkartf);
rename(imaftmp,imafkartf);
}
