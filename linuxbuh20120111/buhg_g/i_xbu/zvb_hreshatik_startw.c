/*$Id: zvb_hreshatik_startw.c,v 1.5 2011-02-21 07:36:00 sasa Exp $*/
/*16.10.2009	29.01.2006	Белых А.И. 	zvb_hreshatik_start.c
Начало формирования данных для зачисления на карт-счета банка Крещатик
Если вернули 0-шапка сформирована
             1-нет
*/

#include        <errno.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include <unistd.h>
#include "zvb_hreshatik.h"

int zvb_hreshatik_start_v(class zvb_hreshatik_rek *rek,GtkWidget *wpredok);

extern char *organ;

int zvb_hreshatik_startw(char *imafr,//имя файла распечатки
char *imaf, //имя файла в электронном виде
double suma, //Сумма
short *d_pp,short *m_pp,short *g_pp, //Дата платёжки
FILE **ff,  //Файл в электонном виде
FILE **ffr, //Файл распечатки
GtkWidget *wpredok)
{
class zvb_hreshatik_rek rek;
short mz=0,gz=0;

if(zvb_hreshatik_start_v(&rek,wpredok) != 0)
 return(1);

iceb_u_rsdat(d_pp,m_pp,g_pp,rek.datpp.ravno(),1);
iceb_u_rsdat1(&mz,&gz,rek.meszp.ravno());

time_t vrem;
struct tm *bf;
time(&vrem);
bf=localtime(&vrem);


//int suma_kop=(int)(suma*100.);
sprintf(imaf,"PA030_%.f_POLTEH_048.%d",suma*100.,bf->tm_yday);
if((*ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }
sprintf(imafr,"hreshatik%d.txt",getpid());
if((*ffr = fopen(imafr,"w")) == NULL)
 {
  iceb_er_op_fil(imafr,"",errno,wpredok);
  return(1);
 }

fprintf(*ffr,"%s\n\n",organ);



fprintf(*ffr,"\
          ЗВЕДЕНА ВІДОМІСТЬ,\n\
      працівників %s \n\
      по заробітній платі за %02d місяць %d р.,\n\
      перерахований за платіжним дорученням N%s\n\
      від %02d.%02d.%d р.\n",organ,mz,gz,rek.nomerpp.ravno(),*d_pp,*m_pp,*g_pp);

fprintf(*ffr,"\
-------------------------------------------------------------------------\n");
fprintf(*ffr,"\
 Н/п | Призвище,і'мя, по батькові   |   ІНН    |Номер рахунку|  Сумма    |\n");
/*
12345 123456789012345678901234567890 1234567890 1234567890123 1234567890
*/
fprintf(*ffr,"\
-------------------------------------------------------------------------\n");


fprintf(*ff,"00\
%04d%02d%02d\
%02d%02d%02d\
%1s\
%12s\
%-8s\
%04d%02d%02d\
%012.2f\
00\n",
bf->tm_year+1900,bf->tm_mon+1,bf->tm_mday,
bf->tm_hour,bf->tm_min,bf->tm_sec,
" ",
" ",
rek.nomerpp.ravno(),
*g_pp,*m_pp,*d_pp,
suma);

return(0);
}
