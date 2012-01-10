/* $Id: rasf1df.c,v 1.21 2011-08-29 07:13:43 sasa Exp $ */
/*14.11.2009	13.07.2000	Белых А.И.	rasf1df.c
Вигрузка в *.dbf или *.lst файл формы 8ДР
*/
#include <stdlib.h>
#include        <errno.h>
#include        <ctype.h>
#include        "dbfhead.h"
#include        <math.h>
#include        <time.h>
#include        "buhg_g.h"
#include <unistd.h>

int  rasf1df_m(class iceb_u_str *kolih_v_htate,class iceb_u_str *kolih_po_sovm, GtkWidget *wpredok);


#define         KSNL 63 /*Количество строк на листе*/
#define		SH1L 34 /*Шапка первого листа*/
#define		SHPL 6 /*Шапка последующих листов*/

extern char     *organ;/*Наименование организации*/
extern float ppn;
extern SQL_baza bd;


int rasf1df(const char *nomd,
int metka_oth, //0-звітний 1-новий звітний 2-уточнюючий
class iceb_u_spisok *imafils,
class iceb_u_spisok *naimfils,
GtkWidget *wpredok)
{

int		kolstr;
SQL_str         row,row1;
char		strsql[512];
char		inn[24];
double		sumad,suman,isumad,isuman;
double		sumadn,sumann,isumadn,isumann;
char		priz[4];
short		lgota=0;
char		lgotach[56];
char		data1[11];
char		data2[11];
char		data11[11]; /*Дата для dbf файла*/
char		data21[11]; /*Дата для dbf файла*/
char		data12[11]; /*дата для xml файла*/
char		data22[11]; /*дата для xml файла*/
int		npp1;
char		imaf[32],imafdbf[32],imaffam[32];
FILE		*ff1=NULL,*ff4=NULL,*fffam=NULL;
short		d,m,g;
int		kolr=0; /*Количество физических лиц*/
int		klst=0; /*Количество строк*/
int		klls=1; /*Количество листов*/
short		god;
int		kvrt;
int		vidd;
char		bros[512];
char kod[32];
short		TYP; /*Метка 0-юридическая особа 1-физическая особа*/
char            priznak[16];
SQLCURSOR curr;


/*Читаем шапку документа*/

sprintf(strsql,"select * from F8dr where nomd='%s'",nomd);
if(sql_readkey(&bd,strsql,&row,&curr) < 1)
 {
  iceb_menu_soob(gettext("Не найден документ !"),wpredok);
  return(1);
 } 
god=atoi(row[0]);
kvrt=atoi(row[1]);
vidd=atoi(row[3]);
iceb_refresh();

sprintf(imafdbf,"da0000%02d.%d",1,kvrt);
if(iceb_poldan("Имя DBF файла",bros,"zarsdf1df.alx",wpredok) == 0)
  sprintf(imafdbf,"%s%02d.%d",bros,1,kvrt);



sprintf(strsql,"select inn,sumad,suman,priz,datap,datau,lgota,fio,sumadn,sumann,pr \
from F8dr1 where nomd='%s' order by inn asc,pr desc",nomd);

class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),wpredok);
  return(1);
 }
iceb_refresh();
char naim_kontr[512];
char telef_kontr[512];
memset(naim_kontr,'\0',sizeof(naim_kontr));
memset(telef_kontr,'\0',sizeof(telef_kontr));

memset(kod,'\0',sizeof(kod));
TYP=0;
sprintf(bros,"select naimkod,kod,telef from Kontragent where kodkon='00'");
if(sql_readkey(&bd,bros,&row,&curr) == 1)
 {
  strncpy(naim_kontr,row[0],sizeof(naim_kontr)-1);
  strncpy(kod,row[1],sizeof(kod)-1);
  strncpy(telef_kontr,row[2],sizeof(telef_kontr)-1);
 }

if(iceb_poldan("Физическое лицо",bros,"zarsdf1df.alx",wpredok) == 0)
 if(iceb_u_SRAV(bros,"Вкл",1) == 0)
  {
   TYP=1;
  }

class iceb_u_str kolih_v_htate("");
class iceb_u_str kolih_po_sovm("");

if(rasf1df_m(&kolih_v_htate,&kolih_po_sovm,wpredok) != 0)
 return(1);

sprintf(imaf,"dov%d_%d.lst",kvrt,getpid());
if((ff1 = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }

sprintf(imaffam,"dovf%d_%d.lst",kvrt,getpid());
if((fffam = fopen(imaffam,"w")) == NULL)
 {
  iceb_er_op_fil(imaffam,"",errno,wpredok);
  return(1);
 }
//Заголовок распечаток

zagf1df(kolih_v_htate.ravno_atoi(),kolih_po_sovm.ravno_atoi(),kvrt,kod,TYP,&klst,vidd,0,0,god,0,0,0,metka_oth,ff1,fffam,wpredok);

spkvd(&klst,ff1);



creatheaddbf(imafdbf,kolstr+3,wpredok);
if((ff4 = fopen(imafdbf,"a")) == NULL)
 {
  iceb_er_op_fil(imafdbf,"",errno,wpredok);
  return(1);
 }


char imaf_xml[50];
memset(imaf_xml,'\0',sizeof(imaf_xml));
FILE *ff_xml=NULL;
f1df_xml_zagw(kvrt,god,kod,TYP,metka_oth,kolih_v_htate.ravno_atoi(),kolih_po_sovm.ravno_atoi(),naim_kontr,imaf_xml,&ff_xml,wpredok);

class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);

isumad=isuman=0.;
isumadn=isumann=0.;
npp1=0;
float kolstr1=0.;
class iceb_u_spisok fiz_lico;
int nomer_xml_str=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);

  if(fiz_lico.find(row[1]) < 0)
   fiz_lico.plus(row[1]);
  
  memset(inn,'\0',sizeof(inn));
  strncpy(inn,row[0],sizeof(inn)-1);

  sumad=atof(row[1]);
  sumad=iceb_u_okrug(sumad,0.01);

  suman=atof(row[2]);
  suman=iceb_u_okrug(suman,0.01);

  sumadn=atof(row[8]);
  sumadn=iceb_u_okrug(sumadn,0.01);

  sumann=atof(row[9]);
  sumann=iceb_u_okrug(sumann,0.01);

  isumad+=sumad;
  isuman+=suman;

  isumadn+=sumadn;
  isumann+=sumann;

  memset(priz,'\0',sizeof(priz));
  strcpy(priz,row[3]);

  d=m=g=0;
  iceb_u_rsdat(&d,&m,&g,row[4],2);
  memset(data1,'\0',sizeof(data1));
  memset(data11,'\0',sizeof(data11));
  memset(data12,'\0',sizeof(data12));
  if(d != 0)
   {
    sprintf(data1,"%04d%02d%02d",g,m,d);
    sprintf(data11,"%02d.%02d.%4d",d,m,g);
    sprintf(data12,"%02d%02d%4d",d,m,g);
   }
  d=m=g=0;
  iceb_u_rsdat(&d,&m,&g,row[5],2);
  memset(data2,'\0',sizeof(data2));
  memset(data21,'\0',sizeof(data21));
  memset(data22,'\0',sizeof(data22));
  if(d != 0)
   {
    sprintf(data2,"%04d%02d%02d",g,m,d);
    sprintf(data21,"%02d.%02d.%04d",d,m,g);
    sprintf(data22,"%02d%02d%04d",d,m,g);
   }

  lgota=atoi(row[6]);
  memset(lgotach,'\0',sizeof(lgotach));
  if(lgota != 0)
   sprintf(lgotach,"%02d",lgota);  

  memset(priznak,'\0',sizeof(priznak));
  if(vidd != 0)
    strcpy(priznak,row[10]);

  //Счетчик строк
  ssf8dr(&klst,&klls,ff1);
  npp1++;    

  
  fprintf(ff1,"%-5d|%-10s|%10.2f|%10.2f|%10.2f|%10.2f|  %-4s|%-10s|%-10s|%-10s|%6s|\n",
  npp1,inn,sumadn,sumad,sumann,suman,priz,data11,data21,lgotach,priznak);

  //Узнаем инвентарный номер
  char fam[512];    
  char tabn[20];
  memset(fam,'\0',sizeof(fam));
  memset(tabn,'\0',sizeof(tabn));
  if(row[7][0] == '\0')
   {
    sprintf(strsql,"select tabn,fio from Kartb where inn='%s'",row[0]);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
     {
      strncpy(tabn,row1[0],sizeof(tabn)-1);
      strncpy(fam,row1[1],sizeof(fam)-1);
     }
   }
  else
   {
    strncpy(fam,row[7],sizeof(fam)-1);      
   }

  fprintf(fffam,"%-5d %-10s %10.2f %10.2f %10.2f %10.2f %-6s %10s %10s %-10s %6s %-5s %s\n",
  npp1,inn,sumadn,sumad,sumann,suman,priz,data11,data21,lgotach,priznak,tabn,fam);   

  zapf8dr(npp1,kvrt,god,kod,TYP,inn,sumadn,sumad,sumann,suman,priz,data1,data2,lgota,atoi(row[10]),ff4);

  f1df_xml_strw(&nomer_xml_str,inn,sumadn,sumad,sumann,suman,priz,data12,data22,lgotach,row[10],ff_xml);
  
 }
kolr=fiz_lico.kolih();

gdite.close(); //Закрываем окно Ждите
iceb_refresh();


koncf8dr(kvrt,god,kod,TYP,isumad,isumadn,isuman,isumann,isumad,isumadn,isuman,isumann,kolr,klls,&npp1,2,ff1,fffam,ff4,wpredok);
fclose(ff1);
fclose(fffam);

//koncf8dr(kvrt,god,kod,TYP,isumad,isumadn,isuman,isumann,isumad,isumadn,isuman,isumann,kolr,klls,&npp1,1,ff1,fffam,ff4,wpredok);
fputc(26, ff4);
fclose(ff4);


f1df_xml_konw(isumadn,isumad,isumann,isuman,nomer_xml_str,kolr,klls,naim_kontr,telef_kontr,TYP,ff_xml,wpredok);

imafils->plus(imaf);
naimfils->plus(gettext("Распечатка формы 1ДФ (для сдачи в налоговую)"));

imafils->plus(imaffam);
naimfils->plus(gettext("Распечатка формы 1ДФ (c фамилиями)"));

for(int nom=0; nom < imafils->kolih(); nom++)
 iceb_ustpeh(imafils->ravno(nom),3,wpredok);

imafils->plus(imafdbf);
naimfils->plus(gettext("Форма 1ДФ в dbf формате"));


imafils->plus(imaf_xml);
naimfils->plus(gettext("Форма 1ДФ в XML формате"));


return(0);
}

