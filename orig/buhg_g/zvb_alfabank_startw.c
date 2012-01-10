/*$Id: zvb_alfabank_startw.c,v 1.4 2011-02-21 07:36:00 sasa Exp $*/
/*19.10.2009	15.10.2009	Белых А.И.	zvb_alfabank_startw.c
Меню для начала 
*/
#include <stdlib.h>
#include        <errno.h>
#include    "buhg_g.h"
#include <unistd.h>
#include        "dbfhead.h"

int zvb_alfabank_v(class iceb_u_str *nomerb,class iceb_u_str *pornom,GtkWidget *wpredok);

extern char *organ;


int zvb_alfabank_startw(char *imafr,//имя файла распечатки
char *imaf_dbf, 
char *imaf_dbf_tmp, //имя файла в электронном виде
FILE **ff,  //Файл в электонном виде
FILE **ffr, //Файл распечатки
GtkWidget *wpredok)
{
class iceb_u_str nomerb("");
class iceb_u_str pornom("");

if(zvb_alfabank_v(&nomerb,&pornom,wpredok)!= 0)
 return(1);

time_t vrem;
struct tm *bf;
time(&vrem);
bf=localtime(&vrem);

sprintf(imaf_dbf,"ZP%5.5s%02d%02d%02d%02d.dbf",nomerb.ravno(),bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900-2000,pornom.ravno_atoi());
sprintf(imaf_dbf_tmp,"alfabankdbf%d.tmp",getpid());

if((*ff = fopen(imaf_dbf_tmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_dbf_tmp,"",errno,wpredok);
  return(1);
 }
sprintf(imafr,"alfabank%d.txt",getpid());
if((*ffr = fopen(imafr,"w")) == NULL)
 {
  iceb_er_op_fil(imafr,"",errno,wpredok);
  return(1);
 }

fprintf(*ffr,"%s\n\n",organ);
fprintf(*ffr,"Відомість для зарахування на карт-рахунки\n");

fprintf(*ffr,"\
------------------------------------------------------------------------------------------------\n");

fprintf(*ffr,"\
N п/п |      Призвище Ім'я Побатькові          |N картрахунку |  Сумма    |Код ІНН   |Код ЕДРПОУ|\n");
/*********
123456 1234567890123456789012345678901234567890 12345678901234 1234567890 1234567890 1234567890
*********/
fprintf(*ffr,"\
------------------------------------------------------------------------------------------------\n");





return(0);
}

