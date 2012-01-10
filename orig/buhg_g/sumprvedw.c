/*$Id: sumprvedw.c,v 1.7 2011-02-21 07:35:57 sasa Exp $*/
/*17.11.2009	18.02.2003	Белых А.И. 	sumprvedw.c
Вставка суммы прописью в шапку ведомости
*/
#include        <errno.h>
#include        "buhg_g.h"
#include <unistd.h>


void	sumprvedw(iceb_u_double *SUMPODR, //Суммы по ведомостям
iceb_u_int *KOLLIST, //Количество листов в распечатках
char *imaf)
{
char	imaftmp[32];
char	stroka[1024];
char	strokasum[1024];
FILE	*fftmp,*ff;
double  suma=0.;
int	shethik=0;
int	shethikl=0;
short   metkastroki=0;
char    bros[512];

if((ff = fopen(imaf,"r")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,NULL);
    return;
 }

sprintf(imaftmp,"prom%d.tmp",getpid());


if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,NULL);
  return;
 }

memset(strokasum,'\0',sizeof(strokasum));
while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
//  printw("%s",stroka);
/*******************
  if(iceb_u_SRAV(stroka,gettext("\
в сумме______________________    Сумма__________________________"),1) == 0)  
******************/
  if(iceb_u_SRAV(stroka,"в сум",1) == 0)  
   {
    suma=SUMPODR->ravno(shethik++);
    memset(strokasum,'\0',sizeof(strokasum));
    iceb_u_preobr(suma,strokasum,0);
//    printw("%s\n",strokasum);
    iceb_u_bstab(stroka,strokasum,6,40,1);
   }  

  if(metkastroki == 1)
   {
//    printw("%s\n",&strokasum[24]);
    iceb_u_bstab(stroka,&strokasum[32],0,40,1);
    memset(strokasum,'\0',sizeof(strokasum));
    
   }

  if(strokasum[0] != '\0')
    metkastroki++;
  else
    metkastroki=0;

  if(iceb_u_strstrm(stroka,"#") != 0)
   {
    sprintf(bros,"%d",KOLLIST->ravno(shethikl++));
    iceb_u_bstab(stroka,bros,71,80,1);
   }    

  fprintf(fftmp,"%s",stroka);
 }
fclose(ff);
fclose(fftmp);
unlink(imaf);
rename(imaftmp,imaf);
//OSTANOV();
}
