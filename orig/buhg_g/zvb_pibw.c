/*$Id: zvb_pibw.c,v 1.16 2011-02-21 07:36:00 sasa Exp $*/
/*29.10.2009	19.07.2005	Белых А.И.	zvb_pibw.c
*/
#include <stdlib.h>
#include        <errno.h>
#include    "buhg_g.h"
#include <unistd.h>
#include "l_nahud.h"

extern char *organ;
extern int      kol_strok_na_liste;
extern SQL_baza bd;

void	zvb_pibw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);
SQL_str		row,row1;
char		fio[512];
char            famil[512];
char            imq[512];
char            otch[512];
double		sumai=0.,sum=0.;;
char            imafkartr[32];
char		imafkartf[32];
FILE            *ffkartr=NULL; //Распечатка выгрузки на картсчета
FILE		*ffkartf=NULL; //Файл выгрузки на дискету
int		kolstrkartr=0;
double          sumalist=0.;
char		strsql[512];
short		d,m,g;
int		kolstr=0;
char		strok[1024];
char		bros[512];
int		kolz=0;
char		bankshet[32];
double          itogo=0.,itogoo=0.;
int		nomer=0;
char		inn[32];
int		kolstrok=0;
char		rukov[512];
char		glavbuh[512];

SQLCURSOR curr;
memset(rukov,'\0',sizeof(rukov));
memset(glavbuh,'\0',sizeof(glavbuh));

memset(bros,'\0',sizeof(bros));
iceb_poldan("Табельный номер руководителя",bros,"zarnas.alx",wpredok);
if(bros[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros);
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    strncpy(rukov,row1[0],sizeof(rukov)-1);
 } 

memset(bros,'\0',sizeof(bros));
iceb_poldan("Табельный номер бухгалтера",bros,"zarnas.alx",wpredok);

if(bros[0] != '\0')
 {
  sprintf(strsql,"select fio,inn,telef from Kartb where tabn=%s",bros);
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    strncpy(glavbuh,row1[0],sizeof(glavbuh)-1);
 }


//printw("%s\n",strsql);
//OSTANOV();

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),zapros,wpredok);
  return;
 }


sprintf(imafkartr,"kartr%d.lst",getpid());
sprintf(imafkartf,"kartf%d.txt",getpid());



if((ffkartr = fopen(imafkartr,"w")) == NULL)
 {
  iceb_er_op_fil(imafkartr,"",errno,wpredok);
  return;
 }

hrvnks(ffkartr,&kolstrkartr);

if((ffkartf = fopen(imafkartf,"w")) == NULL)
 {
  iceb_er_op_fil(imafkartf,"",errno,wpredok);
  return;
 }



sumai=0.;
float kolstr1=0.;
memset(bankshet,'\0',sizeof(bankshet));
int kol_na_liste=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  memset(fio,'\0',sizeof(fio));
  if(l_nahud_prov_str(row,poisk,wpredok) != 0)
   continue;

  strncpy(fio,row[15],sizeof(fio)-1);  

   
  iceb_u_rsdat(&d,&m,&g,row[0],2);

  sum=atof(row[4]);
  if(sum == 0.)
    continue;
  if(prn == 2)
    sum*=-1;
  
  iceb_u_polen(fio,famil,sizeof(famil),1,' ');
  iceb_u_polen(fio,imq,sizeof(imq),2,' ');
  iceb_u_polen(fio,otch,sizeof(otch),3,' ');
  sprintf(strsql,"%s %1.1s. %1.1s.",famil,imq,otch);
  

  memset(inn,'\0',sizeof(inn));

  sprintf(strsql,"select fio,bankshet,inn from Kartb where tabn=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден табельный номер"),row[1]);
    iceb_menu_soob(strsql,wpredok);
    continue;
   }
  strncpy(inn,row1[2],sizeof(inn)-1);
  strncpy(bankshet,row1[1],sizeof(bankshet)-1);
  if(bankshet[0] == '\0')
   {
    class iceb_u_str repl;
    repl.plus(gettext("Не введён карт-счёт !"));

    sprintf(strsql,"%s:%s %s",gettext("Табельный номер"),row[1],fio);
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);
    continue;
   }



  iceb_u_polen(fio,famil,sizeof(famil),1,' ');
  iceb_u_polen(fio,imq,sizeof(imq),2,' ');
  iceb_u_polen(fio,otch,sizeof(otch),3,' ');
  iceb_u_polen(fio,bros,sizeof(bros),4,' ');
  if(bros[0] == '\0')
   {
    strcat(otch," ");
    strcat(otch,bros);
   }


  iceb_u_polen(fio,famil,sizeof(famil),1,' ');
  iceb_u_polen(fio,imq,sizeof(imq),2,' ');
  iceb_u_polen(fio,otch,sizeof(otch),3,' ');
  iceb_u_polen(fio,bros,sizeof(bros),4,' ');
  if(bros[0] != '\0')
   {
    strcat(otch," ");
    strcat(otch,bros);
   }        
  
  kolz++;
 
  pib_itlw(&kolstrkartr,&sumalist,&kol_na_liste,rukov,glavbuh,ffkartr);

  sumalist+=sum;
  pib_strw(kolz,bankshet,sum,fio,inn,ffkartr);
  
  kol_na_liste++;    
  fprintf(ffkartf,"%-16s %8.2f %s\n",bankshet,sum,fio);

  iceb_u_prnb(sum,strok);
  kolstrok+=2;
  sumai+=sum;
  itogo+=sum; 
  itogoo+=sum; 
  nomer++;
 }

pib_end_dokw(kol_na_liste,sumalist,itogoo,nomer,rukov,glavbuh,ffkartr);

fclose(ffkartr);

fclose(ffkartf);

//Записываем шапку и концовку
hdisk(nomer,itogoo,imafkartf);
  
class iceb_u_spisok imafr;
class iceb_u_spisok naimf;
imafr.plus(imafkartr);
imafr.plus(imafkartf);
naimf.plus(gettext("Ведомость для зачисления на карт-счёта."));
naimf.plus(gettext("Файл для передачи в банк."));

gdite.close();

iceb_ustpeh(imafr.ravno(0),1,wpredok);

iceb_rabfil(&imafr,&naimf,"",0,wpredok);

}
