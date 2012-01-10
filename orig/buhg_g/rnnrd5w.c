/*$Id: rnnrd5w.c,v 1.18 2011-08-29 07:13:44 sasa Exp $*/
/*23.07.2011	03.04.2008	Белых А.И.	rnnrd5w.c
Распечатка додатка 5
*/
#include <ctype.h>
#include <errno.h>
#include "buhg_g.h"
#include <unistd.h>
#include "rnn_d5w.h"

void rnnrd5_start(int god,char *kvrt,char *mes,int,FILE *ff,GtkWidget*);
void rnnrd5_end(const char *fio_ruk,const char *fio_buh,FILE *ff,GtkWidget*);
void rnnrd5_sap(FILE *ff_svod);
void rnn_rd5_sap2(double suma_bez_nds2,double suma_nds2,double os_sr21,double os_sr22,double inh1,double inh2,FILE *ff_svod,FILE*);
void rnnrd5_endras(class rnn_d5w *dod5,double,int,FILE *ff_svod,FILE *ff_xml,GtkWidget *wpredok);

extern class iceb_rnfxml_data rek_zag_nn;
extern char *organ;
extern SQL_baza bd;

int rnnrd5w(short dn,short mn,short gn,short dk,short mk,short gk,
char *imaf_xml,char *imaf_svod,char *imaf_svod_naim,
char *imaf_xml_dot,char *imaf_svod_dot, /*Дополнительные отчёты*/
double proc_dot, /*Процент дотации*/
class rnn_d5w *dod5,
GtkWidget *wpredok)
{
int period_type;
FILE *ff_xml=NULL;
FILE *ff_xml_dot=NULL;

if(iceb_openxml(dn,mn,gn,dk,mk,gk,imaf_xml,"J02","151","02",1,&period_type,&rek_zag_nn,&ff_xml,wpredok) != 0)
 return(1);

rnn_sap_xmlw(mn,gn,mk,"J02","151",2,1,"J0215102.xsd",period_type,&rek_zag_nn,ff_xml);


fprintf(ff_xml," <DECLARBODY>\n");
fprintf(ff_xml,"   <HZ>1</HZ>\n"); /*1-звітний */
fprintf(ff_xml,"   <HZY>%d</HZY>\n",gk);
fprintf(ff_xml,"   <HZM>%d</HZM>\n",mk);
fprintf(ff_xml,"   <HZYP xsi:nil=\"true\"/>\n");
fprintf(ff_xml,"   <HNAME>%s</HNAME>\n",iceb_u_filtr_xml(rek_zag_nn.naim_kontr));
//fprintf(ff_xml,"   <HLOC>%s</HLOC>\n",iceb_u_filtr_xml(rek_zag_nn.adres));
fprintf(ff_xml,"   <HTINJ>%s</HTINJ>\n",rek_zag_nn.kod_edrpuo);
fprintf(ff_xml,"   <HDDGVSD xsi:nil=\"true\"/>\n");
fprintf(ff_xml,"   <HNDGVSD xsi:nil=\"true\"/>\n");
fprintf(ff_xml,"   <HNPDV>%s</HNPDV>\n",rek_zag_nn.innn);
fprintf(ff_xml,"   <HNSPDV>%s</HNSPDV>\n",rek_zag_nn.nspnds);

if(proc_dot != 0.)
 {
  if(iceb_openxml(dn,mn,gn,dk,mk,gk,imaf_xml_dot,"J02","851","02",1,&period_type,&rek_zag_nn,&ff_xml_dot,wpredok) != 0)
   return(1);

  rnn_sap_xmlw(mn,gn,mk,"J02","851",2,1,"J0285102.xsd",period_type,&rek_zag_nn,ff_xml_dot);


  fprintf(ff_xml_dot," <DECLARBODY>\n");
  fprintf(ff_xml_dot,"   <HZ>1</HZ>\n"); /*1-звітний */
  fprintf(ff_xml_dot,"   <HZY>%d</HZY>\n",gk);
  fprintf(ff_xml_dot,"   <HZM>%d</HZM>\n",mk);
  fprintf(ff_xml_dot,"   <HZYP xsi:nil=\"true\"/>\n");
  fprintf(ff_xml_dot,"   <HNAME>%s</HNAME>\n",iceb_u_filtr_xml(rek_zag_nn.naim_kontr));
  //fprintf(ff_xml_dot,"   <HLOC>%s</HLOC>\n",iceb_u_filtr_xml(rek_zag_nn.adres));
  fprintf(ff_xml_dot,"   <HTINJ>%s</HTINJ>\n",rek_zag_nn.kod_edrpuo);
  fprintf(ff_xml_dot,"   <HDDGVSD xsi:nil=\"true\"/>\n");
  fprintf(ff_xml_dot,"   <HNDGVSD xsi:nil=\"true\"/>\n");
  fprintf(ff_xml_dot,"   <HNPDV>%s</HNPDV>\n",rek_zag_nn.innn);
  fprintf(ff_xml_dot,"   <HNSPDV>%s</HNSPDV>\n",rek_zag_nn.nspnds);
 
 }


FILE *ff_svod;
FILE *ff_svod_dot=NULL;

sprintf(imaf_svod,"rnns%d.lst",getpid());

if((ff_svod=fopen(imaf_svod,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_svod,"",errno,wpredok);
  return(1);
 }
iceb_u_startfil(ff_svod);
fprintf(ff_svod,"\x1b\x6C%c",10); /*Установка левого поля*/
//fprintf(ff_svod,"\x1B\x4D"); /*12-знаков*/
fprintf(ff_svod,"\x0F");  /*Ужатый режим*/
//fprintf(ff_svod,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/

if(proc_dot != 0.)
 {
  sprintf(imaf_svod_dot,"rnnsd%d.lst",getpid());

  if((ff_svod_dot=fopen(imaf_svod_dot,"w")) == NULL)
   {
    iceb_er_op_fil(imaf_svod_dot,"",errno,wpredok);
    return(1);
   }
  iceb_u_startfil(ff_svod_dot);
  fprintf(ff_svod_dot,"\x1b\x6C%c",10); /*Установка левого поля*/
  fprintf(ff_svod_dot,"\x0F");  /*Ужатый режим*/
 }

FILE *ff_svod_naim;

sprintf(imaf_svod_naim,"rnnsn%d.lst",getpid());

if((ff_svod_naim=fopen(imaf_svod_naim,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_svod_naim,"",errno,wpredok);
  return(1);
 }
iceb_u_startfil(ff_svod_naim);
//fprintf(ff_svod_naim,"\x1B\x4D"); /*12-знаков*/
fprintf(ff_svod_naim,"\x0F");  /*Ужатый режим*/

iceb_u_zagolov("Додаток 5",dn,mn,gn,dk,mk,gk,organ,ff_svod_naim);



char kvrt[5];
char mes[5];
memset(kvrt,'\0',sizeof(kvrt));
memset(mes,'\0',sizeof(mes));
if(mn == mk && gn == gk)
  sprintf(mes,"%d",mk);
else
 {
  int kvt=0;
  if(mk <= 3)
   kvt=1;
  if( mk <= 6)
   kvt=2;
  if( mk <= 9)
   kvt=3;
  if( mk > 9)
   kvt=4;
   
  sprintf(kvrt,"%d",kvt);
 }
rnnrd5_start(gk,kvrt,mes,0,ff_svod,wpredok);
if(ff_svod_dot != NULL)
  rnnrd5_start(gk,kvrt,mes,1,ff_svod_dot,wpredok);

rnnrd5_sap(ff_svod);
if(ff_svod_dot != NULL)
  rnnrd5_sap(ff_svod_dot);

double inh1=0.,inh2=0.;


fprintf(ff_svod_naim,"Розділ І. Податкові зобов'язання\n");
fprintf(ff_svod_naim,"\
-------------------------------------------------------------------\n");

fprintf(ff_svod_naim,"\
  N  |Платник податку - покупець       |Обсяги поставки|Сума ПДВ  |\n\
     |(індивідуальний податковий номер)| (без ПДВ)     |          |\n");
fprintf(ff_svod_naim,"\
-------------------------------------------------------------------\n");



fprintf(ff_svod,"\x1B\x2D\x31"); /*Включить подчеркивание*/
fprintf(ff_svod_naim,"\x1B\x2D\x31"); /*Включить подчеркивание*/
int nomer_str=0;
double suma_bez_nds=0.;
double suma_nds=0.;
double i_suma_bez_nds=0.;
double i_suma_nds=0.;

for(int ii=0; ii < dod5->innn2.kolih(); ii++)
 {
  if(dod5->innn2.ravno(ii)[0] == '\0' ||  isdigit(dod5->innn2.ravno(ii)[0]) == 0)  /*Значит символ не число*/
   {
    inh1+=dod5->suma_bez_nds2.ravno(ii);
    inh2+=dod5->suma_nds2.ravno(ii);
    continue;
   }
  if(dod5->suma_bez_nds2.ravno(ii) == 0. && dod5->suma_nds2.ravno(ii) == 0.)
   continue;
  
  suma_bez_nds=dod5->suma_bez_nds2.ravno(ii);
  suma_nds=dod5->suma_nds2.ravno(ii);
  if(proc_dot != 0.)
   {
    suma_bez_nds=suma_bez_nds*(100.0-proc_dot)/100.;
    suma_bez_nds=iceb_u_okrug(suma_bez_nds,0.01);

    suma_nds=suma_nds*(100.0-proc_dot)/100.;
    suma_nds=iceb_u_okrug(suma_nds,0.01);
    
   }
  
  i_suma_bez_nds+=suma_bez_nds;
  i_suma_nds+=suma_nds;
  
  fprintf(ff_svod,"%5d|%-*s|%15.2f|%10.2f|\n",
  ++nomer_str,
  iceb_u_kolbait(33,dod5->innn2.ravno(ii)),dod5->innn2.ravno(ii),
  suma_bez_nds,suma_nds);
  
  if(ff_svod_dot != NULL)
   {
    fprintf(ff_svod_dot,"%5d|%-*s|%15.2f|%10.2f|\n",
    nomer_str,
    iceb_u_kolbait(33,dod5->innn2.ravno(ii)),dod5->innn2.ravno(ii),
    dod5->suma_bez_nds2.ravno(ii)-suma_bez_nds,
    dod5->suma_nds2.ravno(ii)-suma_nds);
   }

  fprintf(ff_svod_naim,"%5d|%-*s|%15.2f|%10.2f|%s\n",
  nomer_str,
  iceb_u_kolbait(33,dod5->innn2.ravno(ii)),dod5->innn2.ravno(ii),
  dod5->suma_bez_nds2.ravno(ii),dod5->suma_nds2.ravno(ii),dod5->naim_kontr2.ravno(ii));

  fprintf(ff_xml,"   <T1RXXXXG2 ROWNUM=\"%d\">%s</T1RXXXXG2>\n",nomer_str,dod5->innn2.ravno(ii));
  fprintf(ff_xml,"   <T1RXXXXG3 ROWNUM=\"%d\">%.2f</T1RXXXXG3>\n",nomer_str,suma_bez_nds);
  fprintf(ff_xml,"   <T1RXXXXG4 ROWNUM=\"%d\">%.2f</T1RXXXXG4>\n",nomer_str,suma_nds);
  
  if(ff_xml_dot != NULL)
   {
    fprintf(ff_xml_dot,"   <T1RXXXXG2 ROWNUM=\"%d\">%s</T1RXXXXG2>\n",nomer_str,dod5->innn2.ravno(ii));
    fprintf(ff_xml_dot,"   <T1RXXXXG3 ROWNUM=\"%d\">%.2f</T1RXXXXG3>\n",nomer_str,dod5->suma_bez_nds2.ravno(ii)-suma_bez_nds);
    fprintf(ff_xml_dot,"   <T1RXXXXG4 ROWNUM=\"%d\">%.2f</T1RXXXXG4>\n",nomer_str,dod5->suma_nds2.ravno(ii)-suma_nds);
   } 
 }

fprintf(ff_svod_naim,"\x1B\x2D\x30"); /*Выключить подчеркивание*/

double os1=dod5->os_sr2[0];
double os2=dod5->os_sr2[1];
double inhi1=inh1;
double inhi2=inh2;
if(proc_dot != 0)
 {
  os1=os1*(100.0-proc_dot)/100.;
  os2=os2*(100.0-proc_dot)/100.;
  os1=iceb_u_okrug(os1,0.01);  
  os2=iceb_u_okrug(os2,0.01);  

  inhi1=inhi1*(100.0-proc_dot)/100.;
  inhi1=iceb_u_okrug(inhi1,0.01);
  inhi2=inhi2*(100.0-proc_dot)/100.;
  inhi2=iceb_u_okrug(inhi2,0.01);
 }
rnn_rd5_sap2(i_suma_bez_nds+inh1,i_suma_nds+inh2,os1,os2,inhi1,inhi2,ff_svod,ff_xml);

if(ff_svod_dot != NULL)
  rnn_rd5_sap2(dod5->suma_bez_nds2.suma()-i_suma_bez_nds+inh1-inhi1,
  dod5->suma_nds2.suma()-i_suma_nds+inh2-inhi2,\
  dod5->os_sr2[0]-os1,dod5->os_sr2[1]-os2,inh1-inhi1,inh2-inhi2,ff_svod_dot,ff_xml_dot);


if(inh1 == 0. && inh2 == 0.)
  fprintf(ff_svod_naim,"\
Інші                                   |               |          |\n");
else
  fprintf(ff_svod_naim,"\
Інші                                   |%15.2f|%10.2f|\n",inh1,inh2);

fprintf(ff_svod_naim,"\
Разом                                  |%15.2f|%10.2f|\n",
dod5->suma_bez_nds2.suma(),
dod5->suma_nds2.suma());
fprintf(ff_svod_naim,"\
-------------------------------------------------------------------\n");

fprintf(ff_svod_naim,"\nРозділ ІІ. Податковий кредит\n");
fprintf(ff_svod_naim,"\
-------------------------------------------------------------------\n");

fprintf(ff_svod_naim,"\
  N  |Платник податку -   |Період* виписки  |  Обсяги  | Сума ПДВ |\n\
     |покупець (індивіду- |податкових накла-| поставки |          |\n\
     |альний податковий   |дних, інших доку-|(без ПДВ) |          |\n\
     | номер)             |ментів, що дають |          |          |\n\
     |                    |право на податко-|          |          |\n\
     |                    |вий кредит       |          |          |\n");

/********
12345 12345678901234567890 12345678901234567 1234567890 1234567890
***********/
fprintf(ff_svod_naim,"\
-------------------------------------------------------------------\n");
fprintf(ff_svod_naim,"\
Операції з придбання з ПДВ, які надають право формування податкового\n\
                  кредиту\n");
fprintf(ff_svod_naim,"\
-------------------------------------------------------------------\n");
                  
char period[10];
char in1[20];
nomer_str=0;
short m,g;
fprintf(ff_svod,"\x1B\x2D\x31"); /*Включить подчеркивание*/
fprintf(ff_svod_naim,"\x1B\x2D\x31"); /*Включить подчеркивание*/
i_suma_bez_nds=i_suma_nds=0.;
for(int kkk=0; kkk < dod5->innn1.kolih(); kkk++)
 {
  for(int ii=0; ii < dod5->innn_per1.kolih(); ii++)
   {
    iceb_u_polen(dod5->innn_per1.ravno(ii),in1,sizeof(in1),1,'|');  
    if(iceb_u_SRAV(dod5->innn1.ravno(kkk),in1,0) != 0)
     continue;
    iceb_u_polen(dod5->innn_per1.ravno(ii),period,sizeof(period),2,'|');  

    if(dod5->suma_bez_nds1.ravno(ii) == 0. && dod5->suma_nds1.ravno(ii) == 0.)   
     continue;

    iceb_u_rsdat1(&m,&g,period);
    
    suma_bez_nds=dod5->suma_bez_nds1.ravno(ii);
    suma_nds=dod5->suma_nds1.ravno(ii);

    if(proc_dot != 0.)
     {
      suma_bez_nds=suma_bez_nds*(100.0-proc_dot)/100.;
      suma_bez_nds=iceb_u_okrug(suma_bez_nds,0.01);

      suma_nds=suma_nds*(100.0-proc_dot)/100.;
      suma_nds=iceb_u_okrug(suma_nds,0.01);
      
     }
    
    i_suma_bez_nds+=suma_bez_nds;
    i_suma_nds+=suma_nds;
    
    fprintf(ff_svod,"%5d|%-20s|%-17s|%10.2f|%10.2f|\n",
    ++nomer_str,in1,period,suma_bez_nds,suma_nds);
    if(ff_svod_dot != NULL)
      fprintf(ff_svod_dot,"%5d|%-20s|%-17s|%10.2f|%10.2f|\n",
      nomer_str,in1,period,dod5->suma_bez_nds1.ravno(ii)-suma_bez_nds,dod5->suma_nds1.ravno(ii)-suma_nds);

    fprintf(ff_svod_naim,"%5d|%-20s|%-17s|%10.2f|%10.2f|%s\n",
    nomer_str,in1,period,dod5->suma_bez_nds1.ravno(ii),dod5->suma_nds1.ravno(ii),dod5->naim_kontr1.ravno(ii));
    

    fprintf(ff_xml,"   <T2RXXXXG2 ROWNUM=\"%d\">%s</T2RXXXXG2>\n",nomer_str,in1);
    fprintf(ff_xml,"   <T2RXXXXG3A ROWNUM=\"%d\">%d</T2RXXXXG3A>\n",nomer_str,m);
    fprintf(ff_xml,"   <T2RXXXXG3B ROWNUM=\"%d\">%d</T2RXXXXG3B>\n",nomer_str,g);
    fprintf(ff_xml,"   <T2RXXXXG4 ROWNUM=\"%d\">%.2f</T2RXXXXG4>\n",nomer_str,suma_bez_nds);
    fprintf(ff_xml,"   <T2RXXXXG5 ROWNUM=\"%d\">%.2f</T2RXXXXG5>\n",nomer_str,suma_nds);
    
    if(ff_xml_dot != NULL)
     {
      fprintf(ff_xml_dot,"   <T2RXXXXG2 ROWNUM=\"%d\">%s</T2RXXXXG2>\n",nomer_str,in1);
      fprintf(ff_xml_dot,"   <T2RXXXXG3A ROWNUM=\"%d\">%d</T2RXXXXG3A>\n",nomer_str,m);
      fprintf(ff_xml_dot,"   <T2RXXXXG3B ROWNUM=\"%d\">%d</T2RXXXXG3B>\n",nomer_str,g);
      fprintf(ff_xml_dot,"   <T2RXXXXG4 ROWNUM=\"%d\">%.2f</T2RXXXXG4>\n",nomer_str,dod5->suma_bez_nds1.ravno(ii)-suma_bez_nds);
      fprintf(ff_xml_dot,"   <T2RXXXXG5 ROWNUM=\"%d\">%.2f</T2RXXXXG5>\n",nomer_str,dod5->suma_nds1.ravno(ii)-suma_nds);
     }    
   }
 }
fprintf(ff_svod_naim,"\x1B\x2D\x30"); /*Выключить подчеркивание*/

fprintf(ff_svod_naim,"\
-------------------------------------------------------------------\n");
fprintf(ff_svod_naim,"\
Разом                                       |%10.2f|%10.2f|\n",
dod5->suma_bez_nds1.suma(),dod5->suma_nds1.suma());

os1=dod5->os_sr1[0];
os2=dod5->os_sr1[1];
if(proc_dot != 0.)
 {
  os1=os1*(100.0-proc_dot)/100.;
  os2=os2*(100.0-proc_dot)/100.;
  os1=iceb_u_okrug(os1,0.01);  
  os2=iceb_u_okrug(os2,0.01);  
 
 }
rnnrd5_endras(dod5,proc_dot,0,ff_svod,ff_xml,wpredok);

if(ff_svod_dot != NULL)
  rnnrd5_endras(dod5,proc_dot,1,ff_svod_dot,ff_xml_dot,wpredok);

iceb_podpis(ff_svod_naim,wpredok);
fclose(ff_svod_naim);

return(0);

}
/*******************************/
/*Распечатка шапки документа*/
/****************************/

void rnnrd5_start(int god,char *kvrt,char *mes,int metka,
FILE *ff,
GtkWidget *wpredok)
{

class iceb_u_str imaf_sap;
if(metka == 0)
 imaf_sap.plus("rnnd5_start.alx");
if(metka == 1)
 imaf_sap.plus("rnnd2_start.alx");

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_sap.ravno());
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_sap.ravno());
  iceb_menu_soob(strsql,wpredok);
  return;
 }

char stroka[1024];
int nomer_str=0;
char bros[512];

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;


  nomer_str++; 
  memset(stroka,'\0',sizeof(stroka));
  strncpy(stroka,row_alx[0],sizeof(stroka)-1);
  switch (nomer_str)
   {
    case 14:
     sprintf(bros,"%d",god);
     iceb_u_bstab(stroka,bros,65,70,1);     
     iceb_u_bstab(stroka,kvrt,74,80,1);     
     iceb_u_bstab(stroka,mes,85,90,1);     
     break;   

    case 22:
     iceb_u_bstab(stroka,rek_zag_nn.naim_kontr,13,97,1);     
     break;   

    case 27:
     iceb_u_bstab(stroka,rek_zag_nn.kod_edrpuo,35,56,1);     
     break;   

    case 40:
     iceb_u_bstab(stroka,rek_zag_nn.innn,41,62,1);     
     iceb_u_bstab(stroka,rek_zag_nn.nspnds,64,85,1);     
     break;   

   } 

  fprintf(ff,"%s",stroka);
 }



}
/***********************/
/*Распечатка концовки*/
/**********************/
void rnnrd5_end(const char *fio_ruk,
const char *fio_buh,
FILE *ff,GtkWidget *wpredok)
{

class iceb_u_str imaf_sap;
imaf_sap.plus("rnnd5_end.alx");

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_sap.ravno());
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_sap.ravno());
  iceb_menu_soob(strsql,wpredok);
  return;
 }

char stroka[1024];
int nomer_str=0;
char bros[512];
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomer_str++; 
  memset(stroka,'\0',sizeof(stroka));
  strncpy(stroka,row_alx[0],sizeof(stroka)-1);
  switch (nomer_str)
   {
    case 2:
     sprintf(bros,"%02d.%02d.%d",rek_zag_nn.dt,rek_zag_nn.mt,rek_zag_nn.gt);
     iceb_u_bstab(stroka,bros,24,35,1);     
     break;   

    case 4:
     iceb_u_bstab(stroka,fio_ruk,37,63,1);     
     break;   

    case 7:
     iceb_u_bstab(stroka,fio_buh,37,63,1);     
     break;   
   }
   
  fprintf(ff,"%s",stroka);
 }

}
/**********************************/
/*шапка додатка 5 и 2*/
/**********************************/

void rnnrd5_sap(FILE *ff_svod)
{

fprintf(ff_svod,"\x1B\x50"); /*10-знаков*/
fprintf(ff_svod,"\x12");  /*Нормальный режим печати*/

/*печатаем свод*/

fprintf(ff_svod,"Розділ І. Податкові зобов'язання\n");
fprintf(ff_svod,"\
-------------------------------------------------------------------\n");

fprintf(ff_svod,"\
  N  |Платник податку - покупець       |Обсяги поставки|Сума ПДВ  |\n\
     |(індивідуальний податковий номер)| (без ПДВ)     |          |\n");
/*****
12345 123456789012345678901234567890123 123456789012345 1234567890
*******/

fprintf(ff_svod,"\
-------------------------------------------------------------------\n");

fprintf(ff_svod,"\x1B\x2D\x31"); /*Включить подчеркивание*/

}
/***************************************/
/*Концовка раздела 1 и начало раздела 2*/
/***************************************/
void rnn_rd5_sap2(double suma_bez_nds2,
double suma_nds2,
double os_sr21,
double os_sr22,
double inh1,double inh2,FILE *ff_svod,FILE *ff_xml)
{

if(inh1 == 0. && inh2 == 0.)
 {
  fprintf(ff_xml,"   <R001G3 xsi:nil=\"true\"/>\n");
  fprintf(ff_xml,"   <R001G4 xsi:nil=\"true\"/>\n");
 }
else
 {
  fprintf(ff_xml,"   <R001G3>%.2f</R001G3>\n",inh1);
  fprintf(ff_xml,"   <R001G4>%.2f</R001G4>\n",inh2);
 } 

fprintf(ff_xml,"   <R010G3>%.2f</R010G3>\n",suma_bez_nds2);
fprintf(ff_xml,"   <R010G4>%.2f</R010G4>\n",suma_nds2);
/***************
if(os_sr21 == 0.)
  fprintf(ff_xml,"   <R03G3 xsi:nil=\"true\"/>\n");
else
  fprintf(ff_xml,"   <R03G3>%.2f</R03G3>\n",os_sr21);

if(os_sr22 == 0.)
  fprintf(ff_xml,"   <R03G4 xsi:nil=\"true\"/>\n");
else
  fprintf(ff_xml,"   <R03G4>%.2f</R03G4>\n",os_sr22);
***************/
fprintf(ff_xml,"   <R011G3 xsi:nil=\"true\"/>\n");
fprintf(ff_xml,"   <R011G4 xsi:nil=\"true\"/>\n");

fprintf(ff_svod,"\x1B\x2D\x30"); /*Выключить подчеркивание*/

fprintf(ff_svod,"\
-------------------------------------------------------------------\n");
if(inh1 == 0. && inh2 == 0.)
  fprintf(ff_svod,"\
Інші                                   |               |          |\n");
else
  fprintf(ff_svod,"\
Інші                                   |%15.2f|%10.2f|\n",inh1,inh2);

fprintf(ff_svod,"\
-------------------------------------------------------------------\n");

fprintf(ff_svod,"\
Усього за місяць (квартал)(рядок 1 дек-|               |          |\n\
ларації),в тому числі:                 |%15.2f|%10.2f|\n",
suma_bez_nds2,suma_nds2);


fprintf(ff_svod,"\
-------------------------------------------------------------------\n");

fprintf(ff_svod,"\
поставка основних фондів               |%15.2f|%10.2f|\n",os_sr21,os_sr22);

fprintf(ff_svod,"\
передача товарів (послуг) комісіонером/|               |          |\n\
повіреним на умовах, передбаченних     |               |          |\n\
п 4.7 ст. 4 Закону (без суми комісійної|               |          |\n\
винагороди)                            |               |          |\n");
fprintf(ff_svod,"\
-------------------------------------------------------------------\n");


fprintf(ff_svod,"\nРозділ ІІ. Податковий кредит\n");
fprintf(ff_svod,"\
-------------------------------------------------------------------\n");

fprintf(ff_svod,"\
  N  |Платник податку -   |Період* виписки  |  Обсяги  | Сума ПДВ |\n\
     |покупець (індивіду- |податкових накла-| поставки |          |\n\
     |альний податковий   |дних, інших доку-|(без ПДВ) |          |\n\
     | номер)             |ментів, що дають |          |          |\n\
     |                    |право на податко-|          |          |\n\
     |                    |вий кредит       |          |          |\n");

/********
12345 12345678901234567890 12345678901234567 1234567890 1234567890
***********/
fprintf(ff_svod,"\
-------------------------------------------------------------------\n");
fprintf(ff_svod,"\
Операції з придбання з ПДВ, які надають право формування податкового\n\
                  кредиту\n");
fprintf(ff_svod,"\
-------------------------------------------------------------------\n");


}
/******************/
/*концовка*/
/******************/
void rnnrd5_endras(class rnn_d5w *dod5,
double proc_dot,
int metka_v,    /*0-основной 1-дополнительный*/
FILE *ff_svod,FILE *ff_xml,
GtkWidget *wpredok)
{

double suma_bez_nds=dod5->suma_bez_nds1.suma();
double suma_nds=dod5->suma_nds1.suma();

double os1=dod5->os_sr1[0];
double os2=dod5->os_sr1[1];

if(proc_dot != 0.)
 {
  suma_bez_nds=suma_bez_nds*(100.0-proc_dot)/100.;
  suma_bez_nds=iceb_u_okrug(suma_bez_nds,0.01);

  suma_nds=suma_nds*(100.0-proc_dot)/100.;
  suma_nds=iceb_u_okrug(suma_nds,0.01);
  
  os1=os1*(100.0-proc_dot)/100.;
  os2=os2*(100.0-proc_dot)/100.;
  os1=iceb_u_okrug(os1,0.01);  
  os2=iceb_u_okrug(os2,0.01);  

  if(metka_v == 1)
   {
    suma_bez_nds=dod5->suma_bez_nds1.suma()-suma_bez_nds;
    suma_nds=dod5->suma_nds1.suma()-suma_nds;
    os1=dod5->os_sr1[0]-os1;
    os2=dod5->os_sr1[1]-os2;
   } 
  
 }





fprintf(ff_svod,"\x1B\x2D\x30"); /*Выключить подчеркивание*/

fprintf(ff_svod,"\
-------------------------------------------------------------------\n");
fprintf(ff_svod,"\
Усього за місяць (квартал)(рядок 10.1 декла-|          |          |\n\
рації), в тому числі:                       |%10.2f|%10.2f|\n",
suma_bez_nds,suma_nds);


fprintf(ff_svod,"\
-------------------------------------------------------------------\n");
fprintf(ff_svod,"\
придбання (будівництво, спорудження)        |%10.2f|%10.2f|\n\
основних фондів                             |          |          |\n",os1,os2);

fprintf(ff_svod,"\
-------------------------------------------------------------------\n");
fprintf(ff_svod,"\
проведення розрахунків комісіонером/повірен-|          |          |\n\
ним за товари (послуги) на умовах передба-  |          |          |\n\
чених п.4.7 ст. 4 Закону                    |          |          |\n");

fprintf(ff_svod,"\
-------------------------------------------------------------------\n");



class iceb_fioruk_rk rekruk;
class iceb_fioruk_rk rekbuh;

iceb_fioruk(1,&rekruk,wpredok);
iceb_fioruk(2,&rekbuh,wpredok);


fprintf(ff_xml,"   <R020G4>%.2f</R020G4>\n",suma_bez_nds);
fprintf(ff_xml,"   <R020G5>%.2f</R020G5>\n",suma_nds);

if(os1 == 0.)
  fprintf(ff_xml,"   <R021G4 xsi:nil=\"true\"/>\n");
else
  fprintf(ff_xml,"   <R021G4>%.2f</R021G4>\n",os1);

if(os2 == 0.)
  fprintf(ff_xml,"   <R021G5 xsi:nil=\"true\"/>\n");
else
  fprintf(ff_xml,"   <R021G5>%.2f</R021G5>\n",os2);
/************
fprintf(ff_xml,"   <R07G4 xsi:nil=\"true\"/>\n");
fprintf(ff_xml,"   <R07G5 xsi:nil=\"true\"/>\n");
**************/
/********
fprintf(ff_xml,"   <T3RXXXXG2 xsi:nil=\"true\" ROWNUM=\"1\"/>\n");
fprintf(ff_xml,"   <T3RXXXXG3A xsi:nil=\"true\" ROWNUM=\"1\"/>\n");
fprintf(ff_xml,"   <T3RXXXXG3B xsi:nil=\"true\" ROWNUM=\"1\"/>\n");
fprintf(ff_xml,"   <T3RXXXXG4 xsi:nil=\"true\" ROWNUM=\"1\"/>\n");
fprintf(ff_xml,"   <T3RXXXXG5 xsi:nil=\"true\" ROWNUM=\"1\"/>\n");
***************/
double i_suma_bez_nds=0.;
double i_suma_nds=0.;
char period[20];
short m=0,g=0;
char in1[30];

fprintf(ff_svod,"\
Операції з придбання з ПДВ, які не надають права формування\n\
            податкового кредиту\n");


fprintf(ff_svod,"\
-------------------------------------------------------------------\n");



for(int kkk=0; kkk< dod5->innn1d.kolih(); kkk++)
 for(int nomer=0; nomer < dod5->innn_per1d.kolih(); nomer++)
  {
   iceb_u_polen(dod5->innn_per1.ravno(nomer),in1,sizeof(in1),1,'|');  
   if(iceb_u_SRAV(dod5->innn1.ravno(kkk),in1,0) != 0)
     continue;
   iceb_u_polen(dod5->innn_per1d.ravno(nomer),period,sizeof(period),2,'|');  
   iceb_u_rsdat1(&m,&g,period);

   suma_bez_nds=dod5->suma_bez_nds1d.ravno(nomer);
   suma_nds=dod5->suma_nds1d.ravno(nomer);
   if(proc_dot != 0.)
    {
     suma_bez_nds=suma_bez_nds*(100.0-proc_dot)/100.;
     suma_bez_nds=iceb_u_okrug(suma_bez_nds,0.01);

      suma_nds=suma_nds*(100.0-proc_dot)/100.;
      suma_nds=iceb_u_okrug(suma_nds,0.01);
      if(metka_v == 1)
       {
        suma_bez_nds=dod5->suma_bez_nds1.suma()-suma_bez_nds;
        suma_nds=dod5->suma_nds1.suma()-suma_nds;
       }
     }
    i_suma_bez_nds+=suma_bez_nds;
    i_suma_nds+=suma_nds;

    fprintf(ff_svod,"%5d|%-20s|%02d.%04d          |%10.2f|%10.2f|\n",nomer+1,dod5->innn1d.ravno(nomer),m,g,suma_bez_nds,suma_nds);

    if(dod5->innn1d.ravno(nomer)[0] == '\0' ||  isdigit(dod5->innn1d.ravno(nomer)[0]) == 0)  /*Значит символ не число*/
     fprintf(ff_xml,"   <T3RXXXXG2 xsi:nil=\"true\" ROWNUM=\"%d\"/>\n",nomer+1);
    else  
      fprintf(ff_xml,"   <T3RXXXXG2 ROWNUM=\"%d\">%s</T3RXXXXG2>\n",nomer+1,dod5->innn1d.ravno(nomer));
    fprintf(ff_xml,"   <T3RXXXXG3A ROWNUM=\"%d\">%d</T3RXXXXG3A>\n",nomer+1,m);
    fprintf(ff_xml,"   <T3RXXXXG3B ROWNUM=\"%d\">%d</T3RXXXXG3B>\n",nomer+1,g);
    fprintf(ff_xml,"   <T3RXXXXG4 ROWNUM=\"%d\">%.2f</T3RXXXXG4>\n",nomer+1,suma_bez_nds);
    fprintf(ff_xml,"   <T3RXXXXG5  ROWNUM=\"%d\">%.2f</T3RXXXXG5>\n",nomer+1,suma_nds);
  }
if(dod5->innn1d.kolih() == 0)
 {
  fprintf(ff_svod,"\
     |                    |                 |          |          |\n");
 }
fprintf(ff_svod,"\
-------------------------------------------------------------------\n");

fprintf(ff_svod,"\
Усього за місяць (квартал) рядки 10.2+14    |%10.2f|%10.2f|\n\
   декларації):                             |          |          |\n",i_suma_bez_nds,i_suma_nds);
fprintf(ff_svod,"\
-------------------------------------------------------------------\n");

fprintf(ff_svod,"\x1B\x4D"); /*12-знаков*/

fprintf(ff_svod,"\
* Будь-які податкові періоди в межах установлених Законом строків давності.\n\n");

rnnrd5_end(rekruk.fio.ravno(),rekbuh.fio.ravno(),ff_svod,wpredok);

if(i_suma_bez_nds == 0.)
  fprintf(ff_xml,"   <R08G4 xsi:nil=\"true\"/>\n");
else
  fprintf(ff_xml,"   <R08G4>%.2f</R08G4>\n",i_suma_bez_nds);

if(i_suma_nds == 0.)
  fprintf(ff_xml,"   <R08G5 xsi:nil=\"true\"/>\n");
else
  fprintf(ff_xml,"   <R08G5>%.2f</R08G5>\n",i_suma_nds);

fprintf(ff_xml,"   <HFILL>%02d%02d%04d</HFILL>\n",rek_zag_nn.dt,rek_zag_nn.mt,rek_zag_nn.gt);



fprintf(ff_xml,"   <HBOS>%s</HBOS>\n",rekruk.fio.ravno());
fprintf(ff_xml,"   <HBUH>%s</HBUH>\n",rekbuh.fio.ravno());
fprintf(ff_xml,"   <HFO xsi:nil=\"true\"/>\n");

fprintf(ff_xml," </DECLARBODY>\n");
fprintf(ff_xml,"</DECLAR>\n");

iceb_podpis(ff_svod,wpredok);
fclose(ff_svod);

fclose(ff_xml);

}
