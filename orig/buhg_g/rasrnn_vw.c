/*$Id: rasrnn_vw.c,v 1.29 2011-08-29 07:13:44 sasa Exp $*/
/*18.10.2010	30.08.2005	Белых А.И.	rasrnn_v.c
Распечатка реестра выданных налоговых накладных 
*/
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "buhg_g.h"
#include <unistd.h>
#include "dbfhead.h"
#include "xrnn_poiw.h"
#include "rnn_d5w.h"
#define RAZMER_MAS 6
void rasrnn_v_xml_itog(double *itog,double *itog_u,FILE *ff_xml);
void rasrnn_v_str_xml(int nomer_str,short d,short m,short g,char *nomer_dok,char *naim_kontr,char *inn,double *suma,const char *viddok,FILE *ff_xml);
void rasrnn_v_rm(double proc_dot,double *itogo_o,double *itogo_o1,double *itogo_o2);
void rasrnn_v_str(short dn,short mn,short gn,char *nomdok,char *naim_kontr,int metka_dok,double sumd,double snds,int *kol_strok,double *itogo_mes,double *stroka_xml,FILE *ff);

void rasrnn_v_dbf(char *imaf_dbf_tmp,char *imaf_dbf,int nomer_xml_str,GtkWidget*);
void rasrnn_v_creatheadbf(char *imaf,long kolz,GtkWidget*);
void rasrnn_v_zvdbf(int nom_zap,short dp,short mp,short gp,char *nnn,char *naim,char *inn,double *suma,int,int tipdok,const char *viddok,FILE *ff,GtkWidget*);
void rpvnn_viw(short dn,short mn,short gn,char *vsego,double *itogo,double *itogo_u,int *kol_strok,FILE *ff);
void rasrnn_v_vvrozkor(int *nomer_str,short dnn,short mnn,short gnn,short dpn,short mpn,short gpn,char *nomer_dok,char *naim_kontr,char *inn,double suma_bez_nds,double suma_nds,const char *koment,FILE *ff_xml_rozkor);
int rnn_vsw();

extern int kol_strok_na_liste;
extern SQL_baza bd;
extern char *organ;

int rasrnn_vw(class xrnn_poiw *data,
int kol_strok,
int period_type,
FILE *ff,FILE *ff_prot,FILE *ff_xml,
FILE *ff_dot,FILE *ff_xml_dot,
FILE *ff_xml_rozkor,
FILE *ff_lst_rozkor,
class rnn_d5w *dod5,
char *imaf_dbf,
char *imaf_dbf_d,
GtkWidget *bar,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok)
{
char strsql[512];
short dn,mn,gn;
short dk,mk,gk;
class iceb_u_str imafil_xml;
//iceb_rsdatp(&dn,&mn,&gn,data->datan.ravno(),&dk,&mk,&gk,data->datak.ravno(),wpredok);
if((data->metka_sort=rnn_vsw()) < 0)
 return(1);

iceb_u_rsdat(&dn,&mn,&gn,data->datan.ravno(),1);
iceb_u_rsdat(&dk,&mk,&gk,data->datak.ravno(),1);
class iceb_u_str sort("");
if(data->metka_sort == 0)
 sort.new_plus(" order by datd asc,nnni asc");
else
 sort.new_plus(" order by datd asc,dnz asc");
  
//printw("Реестр выданных накладных %d.%d.%d => %d.%d.%d\n",dn,mn,gn,dk,mk,gk);
sprintf(strsql,"select datd,nnn,sumd,snds,mt,inn,nkontr,mi,datdi,nomdi,gr,koment,ko,vd,datv from Reenn");

if(dn != 0 && dk == 0)
  sprintf(strsql,"select datd,nnn,sumd,snds,mt,inn,nkontr,mi,datdi,nomdi,gr,koment,ko,vd,datv from Reenn where \
datd >= '%04d-%02d-%02d'",gn,mn,dn);

if(dn != 0 && dk != 0)
  sprintf(strsql,"select datd,nnn,sumd,snds,mt,inn,nkontr,mi,datdi,nomdi,gr,koment,ko,vd,datv from Reenn where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d'",gn,mn,dn,gk,mk,dk);

strcat(strsql,sort.ravno());

SQLCURSOR cur;
SQL_str row;
int kolstr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),wpredok);
  return(1);
 }

char imaf_dbf_tmp[50];
sprintf(imaf_dbf_tmp,"rnn%d.tmp",getpid());
char imaf_dbf_d_tmp[50];
sprintf(imaf_dbf_d_tmp,"rnnd%d.tmp",getpid());

FILE *ff_dbf_tmp;
FILE *ff_dbf_d_tmp=NULL;
if((ff_dbf_tmp=fopen(imaf_dbf_tmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_dbf_tmp,"",errno,wpredok);
  return(1);
 }
double proc_dot=data->proc_dot.ravno_atof();

if(proc_dot != 0.)
 {
  if((ff_dbf_d_tmp=fopen(imaf_dbf_d_tmp,"w")) == NULL)
   {
    iceb_er_op_fil(imaf_dbf_d_tmp,"",errno,wpredok);
    return(1);
   }
 }

int ibros=0;
class iceb_u_str imafalx("");
//Вставляем шапку 
if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) < 0)
 imafalx.new_plus("rpvnn2.alx");

if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0 && iceb_u_sravmydat(dn,mn,gn,1,1,2011) < 0)
 imafalx.new_plus("rpvnn2_1.alx");

if(iceb_u_sravmydat(dn,mn,gn,1,1,2011) >= 0)
 imafalx.new_plus("rpvnn2_2.alx");

kol_strok+=iceb_insfil(imafalx.ravno(),ff,wpredok);
if(ff_dot != NULL)
   ibros=iceb_insfil(imafalx.ravno(),ff_dot,wpredok);
 
iceb_u_zagolov(gettext("Реестр выданных налоговых накладных"),dn,mn,gn,dk,mk,gk,organ,ff_prot);
if(data->kodgr1.getdlinna() > 1)
 fprintf(ff_prot,"%s:%s\n",gettext("Группа"),data->kodgr1.ravno());

rpvnn_ps_pw(ff_prot);
fprintf(ff_lst_rozkor,"\
-----------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_lst_rozkor,"\
   |                               |Податкова накладна,  |Розрахунок коригуван.|Підстава  | Обсяг    | Сума     |\n\
   |    Отримувач (покупець)       |за якою пров.коригув.|кількісних і вартісн.|для кориг.|поставки  |  ПДВ     |\n\
   |                               |                     |показників до под.нак|податкових|(без ПДВ) | (+чи-)   |\n\
   |---------------------------------------------------------------------------|зобов'яза.| (+чи-)   |          |\n\
 N |   Найменування     |Індивідуал|  Дата    |  Номер   | Номер    |Дата отрим|з податку |          |          |\n\
   |(П.І.Б. для фізичних|  ьний    |          |          |          |мання отри|на додану |          |          |\n\
   |      осіб)         |подаковий |          |          |          |мувачем   |вартість  |          |          |\n\
   |                    | номер    |          |          |          |(покупцем)|          |          |          |\n\
   |                    |          |          |          |          |розрахунку|          |          |          |\n");
fprintf(ff_lst_rozkor,"\
-----------------------------------------------------------------------------------------------------------------\n");

double sumd=0.;
double sumd1=0.;
double snds=0.;
double snds1=0.;

double itogo_mes[RAZMER_MAS];
memset(itogo_mes,'\0',sizeof(itogo_mes));

double itogo_o[RAZMER_MAS];
memset(itogo_o,'\0',sizeof(itogo_o));
double itogo_ou[RAZMER_MAS];
memset(itogo_ou,'\0',sizeof(itogo_o));
int nomer_str=0;
int npz=data->nom_p_zm.ravno_atoi();
if(npz != 0)
 nomer_str=npz-1;

short mz=0;
short d,m,g;
short dd,md,gd;
float kolstr1=0;
int metka_dok;
int metka_pods;
char podsis[10];
double sumd_pr=0.;
double sumnds_pr=0.;
char nomdok[32];
char naim_kontr[512];
int nomer_str_xml=0;
double stroka_xml[RAZMER_MAS];
double stroka_xml_dot[RAZMER_MAS];
double itogo_o1[RAZMER_MAS];
double itogo_o2[RAZMER_MAS];
int por_nom_dok=0;
int nomer_innn=0;
int korrekt=0;
int gtype=0;
int nomer_str_rozkor=0;
double itogo_rozkor1=0.;
double itogo_rozkor2=0.;
short dv,mv,gv;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->kodgr1.ravno(),row[10],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->kodop.ravno(),row[12],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->viddok.ravno(),row[13],0,0) != 0)
   continue;

  if(data->naim_kontr.getdlinna() > 1 )
    if(iceb_u_strstrm(row[6],data->naim_kontr.ravno()) == 0)
      continue;
   
  if(data->podsistema.getdlinna() > 1)
   {
    char metka_pod[20];
    memset(metka_pod,'\0',sizeof(metka_pod));

    if(row[7][0] == '1')
     strcpy(metka_pod,gettext("МУ"));
    if(row[7][0] == '2')
     strcpy(metka_pod,gettext("УСЛ"));
    if(row[7][0] == '3')
     strcpy(metka_pod,gettext("УОС"));
    if(row[7][0] == '4')
     strcpy(metka_pod,gettext("УКР"));
    if(row[7][0] == '5')
     strcpy(metka_pod,gettext("ГК"));
    if(iceb_u_proverka(data->podsistema.ravno(),metka_pod,0,0) != 0)
    continue;
   }
   
  if(iceb_u_proverka(data->nnn.ravno(),row[1],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->nomdok.ravno(),row[9],0,0) != 0)
    continue;

  if(data->inn.getdlinna() > 1 )
    if(iceb_u_strstrm(row[5],data->inn.ravno()) == 0)
     continue;

  if(data->koment.getdlinna() > 1)
    if(iceb_u_strstrm(row[11],data->koment.ravno()) == 0)
     continue;




  iceb_u_rsdat(&d,&m,&g,row[0],2);
  iceb_u_rsdat(&dd,&md,&gd,row[8],2);

  if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0)
  if((gtype=rnnvdw(2,row[13])) <= 0) /*ТИП документа*/
   {
    sprintf(strsql,"Неизвестный тип документа %d %s !\n%d.%d.%d %s",
    gtype,row[13],d,m,g,row[1]);
    iceb_menu_soob(strsql,wpredok);
   }
  korrekt=0;  
  if(iceb_u_strstrm(row[13],"У") == 1 || iceb_u_SRAV(row[13],"РК",0) == 0) /*Если в виде документа присутствует буква У - то это коректирующий документ*/
    korrekt=1;
  if(mz != m)
   {
    if(mz != 0)
     {
      rasrnn_v_rm(proc_dot,itogo_mes,itogo_o1,itogo_o2);
      rpvnn_viw(dn,mn,gn,gettext("Всего за месяц"),itogo_o1,itogo_ou,&kol_strok,ff);
      if(ff_dot != NULL)
        rpvnn_viw(dn,mn,gn,gettext("Всего за месяц"),itogo_o2,itogo_ou,&ibros,ff_dot);
     }
    itogo_o[0]+=itogo_mes[0];
    itogo_o[1]+=itogo_mes[1];
    itogo_o[2]+=itogo_mes[2];
    itogo_o[3]+=itogo_mes[3];
    itogo_o[4]+=itogo_mes[4];
    itogo_o[5]+=itogo_mes[5];
    
    memset(itogo_mes,'\0',sizeof(itogo_mes));
    mz=m;
   }
  sumd=atof(row[2]);
  sumd1=sumd=iceb_u_okrug(sumd,0.01);
  snds=atof(row[3]);
  snds1=snds=iceb_u_okrug(snds,0.01);

  if(proc_dot != 0.)
   {
    sumd1=sumd1*(100.0-proc_dot)/100.;
    sumd1=iceb_u_okrug(sumd1,0.01);
    snds1=snds1*(100.0-proc_dot)/100.;
    snds1=iceb_u_okrug(snds1,0.01);
   }

  metka_dok=atoi(row[4]);

  if(korrekt == 1)
   {
    itogo_ou[0]+=sumd;
    if(metka_dok == 0)
     {
      itogo_ou[1]+=sumd-snds;
      itogo_ou[2]+=snds;
     }
    if(metka_dok == 1)
     itogo_ou[3]+=sumd;
    if(metka_dok == 2)
     itogo_ou[4]+=sumd;
    if(metka_dok == 3)
     itogo_ou[5]+=sumd;
   }  
  
  sumd_pr+=sumd;
  sumnds_pr+=snds;
  
  metka_pods=atoi(row[7]);
  memset(nomdok,'\0',sizeof(nomdok));
  strncpy(nomdok,row[1],sizeof(nomdok)-1);

  memset(naim_kontr,'\0',sizeof(naim_kontr));
  strncpy(naim_kontr,row[6],sizeof(naim_kontr)-1);

  if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0)
   {
    fprintf(ff,"|%-5d|%02d.%02d.%04d|%-*.*s|%-*.*s|%-*.*s|%-14s|%10.2f|",
    ++nomer_str,d,m,g,
    iceb_u_kolbait(8,nomdok),iceb_u_kolbait(8,nomdok),nomdok,
    iceb_u_kolbait(5,row[13]),iceb_u_kolbait(5,row[13]),row[13],
    iceb_u_kolbait(20,naim_kontr),iceb_u_kolbait(20,naim_kontr),naim_kontr,
    row[5],
    sumd1);
     
    if(ff_dot != NULL)
      fprintf(ff_dot,"|%-5d|%02d.%02d.%04d|%-*.*s|%-*.*s|%-*.*s|%-14s|%10.2f|",
      nomer_str,d,m,g,
      iceb_u_kolbait(8,nomdok),iceb_u_kolbait(8,nomdok),nomdok,
      iceb_u_kolbait(5,row[13]),iceb_u_kolbait(5,row[13]),row[13],
      iceb_u_kolbait(20,naim_kontr),iceb_u_kolbait(20,naim_kontr),naim_kontr,
      row[5],sumd-sumd1);
   }
  else
   {  
    fprintf(ff,"|%-5d|%02d.%02d.%04d|%-*.*s|%-*.*s|%-14s|%10.2f|",
    ++nomer_str,d,m,g,
    iceb_u_kolbait(8,nomdok),iceb_u_kolbait(8,nomdok),nomdok,
    iceb_u_kolbait(20,naim_kontr),iceb_u_kolbait(20,naim_kontr),naim_kontr,
    row[5],
    sumd1);
     
    if(ff_dot != NULL)
      fprintf(ff_dot,"|%-5d|%02d.%02d.%04d|%-*.*s|%-*.*s|%-14s|%10.2f|",
      nomer_str,d,m,g,
      iceb_u_kolbait(8,nomdok),iceb_u_kolbait(8,nomdok),nomdok,
      iceb_u_kolbait(20,naim_kontr),iceb_u_kolbait(20,naim_kontr),naim_kontr,
      row[5],sumd-sumd1);
   }  
  memset(stroka_xml,'\0',sizeof(stroka_xml));
  memset(stroka_xml_dot,'\0',sizeof(stroka_xml_dot));
  
  itogo_mes[0]+=sumd;
  
  stroka_xml[0]=sumd1;
  stroka_xml_dot[0]=sumd-sumd1;
  
  rasrnn_v_str(dn,mn,gn,nomdok,naim_kontr,metka_dok,sumd1,snds1,&kol_strok,itogo_mes,stroka_xml,ff);
  if(ff_dot != NULL)
    rasrnn_v_str(dn,mn,gn,nomdok,naim_kontr,metka_dok,sumd-sumd1,snds-snds1,&kol_strok,itogo_mes,stroka_xml_dot,ff_dot);

  memset(podsis,'\0',sizeof(podsis));  

  if(metka_pods == 1)
   {
    strcpy(podsis,gettext("МУ"));
    //Запись накладной в xml формате
    xml_nn_matw(&por_nom_dok,2,dd,md,gd,row[9],row[1],mn,gn,mk,period_type,&imafil_xml,wpredok);
   }

  if(metka_pods == 2)
   {
    strcpy(podsis,gettext("УCЛ"));
    //Запись накладной в xml формате
    xml_nn_uslw(&por_nom_dok,2,dd,md,gd,row[9],row[1],mn,gn,mk,period_type,&imafil_xml,wpredok);
   }
  if(metka_pods == 3)
   {
    strcpy(podsis,gettext("УОC"));
    //Запись накладной в xml формате
    xml_nn_uosw(&por_nom_dok,2,dd,md,gd,row[9],row[1],mn,gn,mk,period_type,&imafil_xml,wpredok);
   }
   

  fprintf(ff_prot,"%5d %02d.%02d.%04d %-10s %10s %8s %d %-5s %12s %-5s %02d.%02d.%04d %-10s %s\n",
  nomer_str,d,m,g,row[1],row[2],row[3],metka_dok,row[10],row[5],podsis,dd,md,gd,row[9],naim_kontr);

  if(korrekt == 1) /*Если в виде документа присутствует буква У - то это коректирующий документ*/
   {
    class iceb_u_str komentout(row[11]);
    iceb_u_rsdat(&dv,&mv,&gv,row[14],2);
    if(dv == 0)
     {
      dv=d;
      mv=m;
      gv=g;
     }

    if(komentout.getdlinna() <= 1)
      komentout.new_plus("Несвоєчасна видача податкової накладної");

    rasrnn_v_vvrozkor(&nomer_str_rozkor,d,m,g,dv,mv,gv,nomdok,naim_kontr,row[5],(sumd-snds),snds,komentout.ravno(),ff_xml_rozkor);

    fprintf(ff_lst_rozkor,"%3d %-*.*s %*s %02d.%02d.%04d %-*s %-10s %02d.%02d.%04d %-*.*s %10.2f %10.2f\n",
    nomer_str_rozkor,
    iceb_u_kolbait(20,naim_kontr),
    iceb_u_kolbait(20,naim_kontr),
    naim_kontr,
    iceb_u_kolbait(10,row[5]),row[5],
    dv,mv,gv,
    iceb_u_kolbait(10,nomdok),nomdok,
    "1",
    d,m,g,
    iceb_u_kolbait(10,komentout.ravno()),
    iceb_u_kolbait(10,komentout.ravno()),
    komentout.ravno(),
    sumd-snds,
    snds);

    itogo_rozkor1+=sumd-snds;
    itogo_rozkor2+=snds;
   }

  rasrnn_v_str_xml(++nomer_str_xml,d,m,g,nomdok,naim_kontr,row[5],stroka_xml,row[13],ff_xml);
  
   
  rasrnn_v_zvdbf(nomer_str_xml,d,m,g,nomdok,naim_kontr,row[5],stroka_xml,korrekt,gtype,row[13],ff_dbf_tmp,wpredok);

  if(ff_xml_dot != NULL)
   {
    rasrnn_v_str_xml(nomer_str_xml,d,m,g,nomdok,naim_kontr,row[5],stroka_xml_dot,row[13],ff_xml_dot);
    rasrnn_v_zvdbf(nomer_str_xml,d,m,g,nomdok,naim_kontr,row[5],stroka_xml_dot,korrekt,gtype,row[13],ff_dbf_d_tmp,wpredok);
   }  

  if(korrekt == 0)
  if(snds != 0.)
   {
    if((nomer_innn=dod5->innn2.find(row[5])) < 0)
     {
      dod5->innn2.plus(row[5]);    
      dod5->naim_kontr2.plus(row[6]);
     }
    dod5->suma_bez_nds2.plus(sumd-snds,nomer_innn);
    dod5->suma_nds2.plus(snds,nomer_innn);
    if(metka_pods == 3) /*Учёт основных средств*/
     {
      dod5->os_sr2[0]+=sumd-snds;
      dod5->os_sr2[1]+=snds;
     }
   }
 }

fclose(ff_dbf_tmp);
if(ff_dbf_d_tmp != NULL)
  fclose(ff_dbf_d_tmp);

fprintf(ff_prot,"%*s:%10.2f %8.2f\n",iceb_u_kolbait(27,gettext("Итого")),gettext("Итого"),sumd_pr,sumnds_pr);

rasrnn_v_rm(proc_dot,itogo_mes,itogo_o1,itogo_o2);

rpvnn_viw(dn,mn,gn,gettext("Всего за месяц"),itogo_o1,itogo_ou,&kol_strok,ff);
if(ff_dot != NULL)
  rpvnn_viw(dn,mn,gn,gettext("Всего за месяц"),itogo_o2,itogo_ou,&ibros,ff_dot);

itogo_o[0]+=itogo_mes[0];
itogo_o[1]+=itogo_mes[1];
itogo_o[2]+=itogo_mes[2];
itogo_o[3]+=itogo_mes[3];
itogo_o[4]+=itogo_mes[4];
itogo_o[5]+=itogo_mes[5];

rasrnn_v_rm(proc_dot,itogo_o,itogo_o1,itogo_o2);

if(mn != mk)
 {
  rpvnn_viw(dn,mn,gn,gettext("Общий итог"),itogo_o1,itogo_ou,&kol_strok,ff);
  if(ff_dot != NULL)
    rpvnn_viw(dn,mn,gn,gettext("Общий итог"),itogo_o2,itogo_ou,&ibros,ff_dot);
  
 }
rasrnn_v_xml_itog(itogo_o1,itogo_ou,ff_xml);
if(ff_xml_dot != NULL)
  rasrnn_v_xml_itog(itogo_o2,itogo_ou,ff_xml_dot);

sprintf(imaf_dbf,"rnn_v_dbf_%02d%d.dbf",mn,gn);
rasrnn_v_dbf(imaf_dbf_tmp,imaf_dbf,nomer_str_xml,wpredok);

if(ff_dbf_d_tmp != NULL)
 {
  sprintf(imaf_dbf_d,"rnn_v_dbf_d_%02d%d.dbf",mn,gn);
  rasrnn_v_dbf(imaf_dbf_d_tmp,imaf_dbf_d,nomer_str_xml,wpredok);
 }

if(nomer_str_rozkor > 0)
 {
  fprintf(ff_xml_rozkor,"   <R01G9>%.2f</R01G9>\n",itogo_rozkor1);
  fprintf(ff_xml_rozkor,"   <R01G10>%.2f</R01G10>\n",itogo_rozkor2);
  fprintf(ff_lst_rozkor,"%*s:%10.2f %10.2f\n",
  iceb_u_kolbait(96,gettext("Итого")),gettext("Итого"),
  itogo_rozkor1,itogo_rozkor2);  
 }
return(0);
}
/**********************************/
/*Запись строки в xml файл*/
/**************************/

void rasrnn_v_str_xml(int nomer_str,
short d,short m,short g,
char *nomer_dok,
char *naim_kontr,
char *inn,
double *suma,
const char *viddok,
FILE *ff_xml)
{
/*******************
fprintf(ff_xml,"   <T2RXXXXG1 ROWNUM=\"%d\">%d</T2RXXXXG1>\n",nomer_str,nomer_str); //1
fprintf(ff_xml,"   <T2RXXXXG2D ROWNUM=\"%d\">%02d%02d%d</T2RXXXXG2D>\n",nomer_str,d,m,g); //1

if(nomer_dok[0] != '\0')
 fprintf(ff_xml,"   <T2RXXXXG3S ROWNUM=\"%d\">%s</T2RXXXXG3S>\n",nomer_str,iceb_u_filtr_xml(nomer_dok)); //2
else
 fprintf(ff_xml,"   <T2RXXXXG3S xsi:nil=\"true\" ROWNUM=\"%d\"></T2RXXXXG3S>\n",nomer_str); //2

if(viddok[0] != '\0')
 fprintf(ff_xml,"   <T2RXXXXG4S ROWNUM=\"%d\">%s</T2RXXXXG4S>\n",nomer_str,iceb_u_filtr_xml(viddok)); //2
else
 fprintf(ff_xml,"   <T2RXXXXG4S xsi:nil=\"true\" ROWNUM=\"%d\"></T2RXXXXG4S>\n",nomer_str); //2

if(naim_kontr[0] != '\0')
 fprintf(ff_xml,"   <T2RXXXXG5S ROWNUM=\"%d\">%s</T2RXXXXG5S>\n",nomer_str,iceb_u_filtr_xml(naim_kontr)); //3
else
 fprintf(ff_xml,"   <T2RXXXXG5S xsi:nil=\"true\" ROWNUM=\"%d\"></T2RXXXXG5S>\n",nomer_str); //3

if(inn[0] == '\0' || isdigit(inn[0]) == 0) //Значит символ не число
  fprintf(ff_xml,"   <T2RXXXXG6 xsi:nil=\"true\" ROWNUM=\"%d\"/>\n",nomer_str); //4
else
  fprintf(ff_xml,"   <T2RXXXXG6 ROWNUM=\"%d\">%s</T2RXXXXG6>\n",nomer_str,inn); //4

fprintf(ff_xml,"   <T2RXXXXG7 ROWNUM=\"%d\">%.2f</T2RXXXXG7>\n",nomer_str,suma[0]); //5
fprintf(ff_xml,"   <T2RXXXXG8 ROWNUM=\"%d\">%.2f</T2RXXXXG8>\n",nomer_str,suma[1]); //6
fprintf(ff_xml,"   <T2RXXXXG9 ROWNUM=\"%d\">%.2f</T2RXXXXG9>\n",nomer_str,suma[2]); //7
fprintf(ff_xml,"   <T2RXXXXG10 ROWNUM=\"%d\">%.2f</T2RXXXXG10>\n",nomer_str,suma[3]); //8
fprintf(ff_xml,"   <T2RXXXXG11 ROWNUM=\"%d\">%.2f</T2RXXXXG11>\n",nomer_str,suma[4]); //9
fprintf(ff_xml,"   <T2RXXXXG12 ROWNUM=\"%d\">%.2f</T2RXXXXG12>\n",nomer_str,suma[5]); //10

***************************/
fprintf(ff_xml,"   <T1RXXXXG1 ROWNUM=\"%d\">%d</T1RXXXXG1>\n",nomer_str,nomer_str); //1
fprintf(ff_xml,"   <T1RXXXXG2D ROWNUM=\"%d\">%02d%02d%d</T1RXXXXG2D>\n",nomer_str,d,m,g); //1

if(nomer_dok[0] != '\0')
 fprintf(ff_xml,"   <T1RXXXXG3S ROWNUM=\"%d\">%s</T1RXXXXG3S>\n",nomer_str,iceb_u_filtr_xml(nomer_dok)); //2
else
 fprintf(ff_xml,"   <T1RXXXXG3S xsi:nil=\"true\" ROWNUM=\"%d\"></T1RXXXXG3S>\n",nomer_str); //2

if(viddok[0] != '\0')
 fprintf(ff_xml,"   <T1RXXXXG4S ROWNUM=\"%d\">%s</T1RXXXXG4S>\n",nomer_str,iceb_u_filtr_xml(viddok)); //2
else
 fprintf(ff_xml,"   <T1RXXXXG4S xsi:nil=\"true\" ROWNUM=\"%d\"></T1RXXXXG4S>\n",nomer_str); //2

if(naim_kontr[0] != '\0')
 fprintf(ff_xml,"   <T1RXXXXG5S ROWNUM=\"%d\">%s</T1RXXXXG5S>\n",nomer_str,iceb_u_filtr_xml(naim_kontr)); //3
else
 fprintf(ff_xml,"   <T1RXXXXG5S xsi:nil=\"true\" ROWNUM=\"%d\"></T1RXXXXG5S>\n",nomer_str); //3

if(inn[0] == '\0' || isdigit(inn[0]) == 0) //*Значит символ не число
  fprintf(ff_xml,"   <T1RXXXXG6 xsi:nil=\"true\" ROWNUM=\"%d\"/>\n",nomer_str); //4
else
  fprintf(ff_xml,"   <T1RXXXXG6 ROWNUM=\"%d\">%s</T1RXXXXG6>\n",nomer_str,inn); //4

fprintf(ff_xml,"   <T1RXXXXG7 ROWNUM=\"%d\">%.2f</T1RXXXXG7>\n",nomer_str,suma[0]); //5
fprintf(ff_xml,"   <T1RXXXXG8 ROWNUM=\"%d\">%.2f</T1RXXXXG8>\n",nomer_str,suma[1]); //6
fprintf(ff_xml,"   <T1RXXXXG9 ROWNUM=\"%d\">%.2f</T1RXXXXG9>\n",nomer_str,suma[2]); //7
fprintf(ff_xml,"   <T1RXXXXG10 ROWNUM=\"%d\">%.2f</T1RXXXXG10>\n",nomer_str,suma[3]); //8
fprintf(ff_xml,"   <T1RXXXXG11 ROWNUM=\"%d\">%.2f</T1RXXXXG11>\n",nomer_str,suma[4]); //9
fprintf(ff_xml,"   <T1RXXXXG12 ROWNUM=\"%d\">%.2f</T1RXXXXG12>\n",nomer_str,suma[5]); //10

}
/*****************************/
/*Вывод в xml файл расчёта корректировки*/
/*******************************/
void rasrnn_v_vvrozkor(int *nomer_str,
short dnn,short mnn,short gnn, //Дата выписки налоговой накладной
short dpn,short mpn,short gpn, //Дата получения налоговой накладной
char *nomer_dok,
char *naim_kontr,
char *inn,
double suma_bez_nds,
double suma_nds,
const char *koment,
FILE *ff_xml_rozkor)
{
*nomer_str+=1;
 
fprintf(ff_xml_rozkor,"   <T1RXXXXG2S ROWNUM=\"%d\">%s</T1RXXXXG2S>\n",*nomer_str,iceb_u_filtr_xml(naim_kontr));
fprintf(ff_xml_rozkor,"   <T1RXXXXG3 ROWNUM=\"%d\">%s</T1RXXXXG3>\n",*nomer_str,inn);
fprintf(ff_xml_rozkor,"   <T1RXXXXG4D ROWNUM=\"%d\">%02d%02d%04d</T1RXXXXG4D>\n",*nomer_str,dpn,mpn,gpn);
fprintf(ff_xml_rozkor,"   <T1RXXXXG5S ROWNUM=\"%d\">%s</T1RXXXXG5S>\n",*nomer_str,iceb_u_filtr_xml(nomer_dok));
fprintf(ff_xml_rozkor,"   <T1RXXXXG6S ROWNUM=\"%d\">1</T1RXXXXG6S>\n",*nomer_str); /*номер расчёта корректировки*/
fprintf(ff_xml_rozkor,"   <T1RXXXXG7D ROWNUM=\"%d\">%02d%02d%04d</T1RXXXXG7D>\n",*nomer_str,dnn,mnn,gnn);
fprintf(ff_xml_rozkor,"   <T1RXXXXG8S ROWNUM=\"%d\">%s</T1RXXXXG8S>\n",*nomer_str,iceb_u_filtr_xml(koment));
fprintf(ff_xml_rozkor,"   <T1RXXXXG9 ROWNUM=\"%d\">%.2f</T1RXXXXG9>\n",*nomer_str,suma_bez_nds);
fprintf(ff_xml_rozkor,"   <T1RXXXXG10 ROWNUM=\"%d\">%.2f</T1RXXXXG10>\n",*nomer_str,suma_nds);

}
/********************************/
/*Распечатать итог*/
/*****************************/
void rasrnn_v_xml_itog(double *itog,double *itog_u,FILE *ff_xml)
{
/*********************
fprintf(ff_xml,"   <T2R01G6>%.2f</T2R01G6>\n",itog[0]);
fprintf(ff_xml,"   <T2R01G7>%.2f</T2R01G7>\n",itog[1]);
fprintf(ff_xml,"   <T2R01G8>%.2f</T2R01G8>\n",itog[2]);
fprintf(ff_xml,"   <T2R01G9>%.2f</T2R01G9>\n",itog[3]);
fprintf(ff_xml,"   <T2R01G10>%.2f</T2R01G10>\n",itog[4]);
fprintf(ff_xml,"   <T2R01G11>%.2f</T2R01G11>\n",itog[5]);

fprintf(ff_xml,"   <T2R02G7>%.2f</T2R02G7>\n",itog_u[0]);
fprintf(ff_xml,"   <T2R02G8>%.2f</T2R02G8>\n",itog_u[1]);
fprintf(ff_xml,"   <T2R02G9>%.2f</T2R02G9>\n",itog_u[2]);
fprintf(ff_xml,"   <T2R02G10>%.2f</T2R02G10>\n",itog_u[3]);
fprintf(ff_xml,"   <T2R02G11>%.2f</T2R02G11>\n",itog_u[4]);
fprintf(ff_xml,"   <T2R02G12>%.2f</T2R02G12>\n",itog_u[5]);
***************************/
fprintf(ff_xml,"   <R011G6>%.2f</R011G6>\n",itog[0]);
fprintf(ff_xml,"   <R011G7>%.2f</R011G7>\n",itog[1]);
fprintf(ff_xml,"   <R011G8>%.2f</R011G8>\n",itog[2]);
fprintf(ff_xml,"   <R011G9>%.2f</R011G9>\n",itog[3]);
fprintf(ff_xml,"   <R011G10>%.2f</R011G10>\n",itog[4]);
fprintf(ff_xml,"   <R011G11>%.2f</R011G11>\n",itog[5]);

fprintf(ff_xml,"   <R012G7>%.2f</R012G7>\n",itog_u[0]);
fprintf(ff_xml,"   <R012G8>%.2f</R012G8>\n",itog_u[1]);
fprintf(ff_xml,"   <R012G9>%.2f</R012G9>\n",itog_u[2]);
fprintf(ff_xml,"   <R012G10>%.2f</R012G10>\n",itog_u[3]);
fprintf(ff_xml,"   <R012G11>%.2f</R012G11>\n",itog_u[4]);
fprintf(ff_xml,"   <R012G12>%.2f</R012G12>\n",itog_u[5]);

}
/************************/
/*разбивка массива по проценту на два массива*/
/*******************************/
void rasrnn_v_rm(double proc_dot,double *itogo_o,double *itogo_o1,double *itogo_o2)
{
memset(itogo_o1,'\0',RAZMER_MAS);
memset(itogo_o2,'\0',RAZMER_MAS);

itogo_o1[0]=itogo_o[0];
itogo_o1[1]=itogo_o[1];
itogo_o1[2]=itogo_o[2];
itogo_o1[3]=itogo_o[3];
itogo_o1[4]=itogo_o[4];
itogo_o1[5]=itogo_o[5];

if(proc_dot != 0.)
 {

  itogo_o1[0]=itogo_o[0]*(100.-proc_dot)/100.;
  itogo_o1[1]=itogo_o[1]*(100.-proc_dot)/100.;
  itogo_o1[2]=itogo_o[2]*(100.-proc_dot)/100.;
  itogo_o1[3]=itogo_o[3]*(100.-proc_dot)/100.;
  itogo_o1[4]=itogo_o[4]*(100.-proc_dot)/100.;
  itogo_o1[5]=itogo_o[5]*(100.-proc_dot)/100.;

  itogo_o1[0]=iceb_u_okrug(itogo_o1[0],0.01);
  itogo_o1[1]=iceb_u_okrug(itogo_o1[1],0.01);
  itogo_o1[2]=iceb_u_okrug(itogo_o1[2],0.01);
  itogo_o1[3]=iceb_u_okrug(itogo_o1[3],0.01);
  itogo_o1[4]=iceb_u_okrug(itogo_o1[4],0.01);
  itogo_o1[5]=iceb_u_okrug(itogo_o1[5],0.01);

  itogo_o2[0]=itogo_o[0]-itogo_o1[0];
  itogo_o2[1]=itogo_o[1]-itogo_o1[1];
  itogo_o2[2]=itogo_o[2]-itogo_o1[2];
  itogo_o2[3]=itogo_o[3]-itogo_o1[3];
  itogo_o2[4]=itogo_o[4]-itogo_o1[4];
  itogo_o2[5]=itogo_o[5]-itogo_o1[5];


 }

}
/***********************/
/*вывод строки продолжения*/
/**************************/
void rasrnn_v_str(short dn,short mn,short gn,char *nomdok,char *naim_kontr,int metka_dok,double sumd,double snds,int *kol_strok,
double *itogo_mes,
double *stroka_xml,
FILE *ff)
{
if(metka_dok == 0)
 {
  if(snds != 0.)
    fprintf(ff,"%9.2f|%8.2f|",sumd-snds,snds);
  else
    fprintf(ff,"%9.2f|XXXXXXXX|",sumd);
  itogo_mes[1]+=sumd-snds;
  itogo_mes[2]+=snds;    
  stroka_xml[1]=sumd-snds;
  stroka_xml[2]=snds;    
 }
else
 fprintf(ff,"%9s|%8s|","","");

if(metka_dok == 1)
 {
  fprintf(ff,"%9.2f|",sumd);
  itogo_mes[3]+=sumd;
  stroka_xml[3]=sumd;
 }
else
 fprintf(ff,"%9s|","");

if(metka_dok == 2)
 {
  fprintf(ff,"%9.2f|",sumd);
  itogo_mes[4]+=sumd;
  stroka_xml[4]=sumd;
 }
else
 fprintf(ff,"%9s|","");

if(metka_dok == 3)
 {
  fprintf(ff,"%9.2f|",sumd);
  itogo_mes[5]+=sumd;
  stroka_xml[5]=sumd;
 }
else
 fprintf(ff,"%9s|","");

fprintf(ff,"\n");


if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0)
 {
  if(iceb_u_strlen(naim_kontr) > 20 || iceb_u_strlen(nomdok) > 8)
   {
    fprintf(ff,"|%5s|%10s|%-*.*s|%5s|%-*.*s|%14s|%10s|%9s|%8s|%9s|%9s|%9s|\n",
    "","",
    iceb_u_kolbait(8,iceb_u_adrsimv(8,nomdok)),
    iceb_u_kolbait(8,iceb_u_adrsimv(8,nomdok)),
    iceb_u_adrsimv(8,nomdok),
    "",
    iceb_u_kolbait(20,iceb_u_adrsimv(20,naim_kontr)),
    iceb_u_kolbait(20,iceb_u_adrsimv(20,naim_kontr)),
    iceb_u_adrsimv(20,naim_kontr),
    "","","","","","","");
   }
  for(int nom=40; nom < iceb_u_strlen(naim_kontr); nom+= 20)
   {
    fprintf(ff,"|%5s|%10s|%8s|%5s|%-*.*s|%14s|%10s|%9s|%8s|%9s|%9s|%9s|\n",
    "","","","",
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim_kontr)),
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim_kontr)),
    iceb_u_adrsimv(nom,naim_kontr),
    "","","","","","","");
   }
 }
else
 {
  if(iceb_u_strlen(naim_kontr) > 20 || iceb_u_strlen(nomdok) > 8)
   {
    fprintf(ff,"|%5s|%10s|%-*.*s|%-*.*s|%14s|%10s|%9s|%8s|%9s|%9s|%9s|\n",
    "","",
    iceb_u_kolbait(8,iceb_u_adrsimv(8,nomdok)),
    iceb_u_kolbait(8,iceb_u_adrsimv(8,nomdok)),
    iceb_u_adrsimv(8,nomdok),
    iceb_u_kolbait(20,iceb_u_adrsimv(20,naim_kontr)),
    iceb_u_kolbait(20,iceb_u_adrsimv(20,naim_kontr)),
    iceb_u_adrsimv(20,naim_kontr),
    "","","","","","","");
   }
  for(int nom=40; nom < iceb_u_strlen(naim_kontr); nom+= 20)
   {
    fprintf(ff,"|%5s|%10s|%8s|%-*.*s|%14s|%10s|%9s|%8s|%9s|%9s|%9s|\n",
    "","","",
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim_kontr)),
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim_kontr)),
    iceb_u_adrsimv(nom,naim_kontr),
    "","","","","","","");
   }
 }
}
/*********************/
/*Запись в dbf файл*/
/*******************/
void rasrnn_v_zvdbf(int nom_zap, /*Номер записи*/
short dp,short mp,short gp,  /*Дата получения*/
char *nnn, /*Номер налоговой накладной*/
char *naim, /*Наименование контрагента*/
char *inn,  /*индивидуальный налоговый номер*/
double *suma, /*массив с суммами*/
int korrekt, /*признак корректировки*/
int tipdok,
const char *viddok,
FILE *ff,
GtkWidget *wpredok)
{
int pzob=0; /*податкові зобов'язання*/
int rez=0;    /*признак резидента*/
if(inn[0] == '\0' || isdigit(inn[0]) == 0)  /*Значит символ не число*/
 {
  rez=1;
  SQL_str row;
  class SQLCURSOR cur;
  char strsql[1024];
  class iceb_u_str naim_cop(naim);
  sprintf(strsql,"select kod from Kontragent where naikon='%s'",naim_cop.ravno_filtr());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    if(row[0] != '\0')
     rez=0; 
   }
 }

fprintf(ff," %7d%04d%02d%02d%-*.*s%-*.*s%-20.20s\
%16.2f%16.2f%16.2f%16.2f%16.2f%16.2f%3d\
%2d%2d%2d%-*.*s",\
nom_zap,
gp,mp,dp,
iceb_u_kolbait(50,nnn),iceb_u_kolbait(50,nnn), nnn,
iceb_u_kolbait(200,naim),iceb_u_kolbait(200,naim),naim,
inn,
suma[0],
suma[1],
suma[2],
suma[3],
suma[4],
suma[5],
pzob,
rez,
korrekt,
tipdok,
iceb_u_kolbait(4,viddok),iceb_u_kolbait(4,viddok),viddok);

}
/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void rasrnn_v_create_hdbf(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
int *header_len,int *rec_len)
{

strncpy(f->name, fn, 11);
f->type = ft;
f->length = l1;
f->dec_point = l2;
*header_len=*header_len+sizeof(DBASE_FIELD);
*rec_len=*rec_len+l1;
}
/***********************************************/
void rasrnn_v_creatheadbf(char *imaf,long kolz,GtkWidget *wpredok)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
#define		kolpol 16 //Количество колонок (полей) в dbf файле
DBASE_FIELD f[kolpol];
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

memset(&h,'\0',sizeof(h));
memset(&f, '\0', sizeof(f));

h.version = 3;

time(&tmm);
bf=localtime(&tmm);

h.l_update[0] = bf->tm_year;       /* yymmdd for last update*/
h.l_update[1] = bf->tm_mon+1;       /* yymmdd for last update*/
h.l_update[2] = bf->tm_mday;       /* yymmdd for last update*/

h.count = kolz;              /* number of records in file*/

header_len = sizeof(h);
rec_len = 0;
int shetshik=0;
rasrnn_v_create_hdbf(&f[shetshik++],"NPP", 'C',7, 0,&header_len,&rec_len);
rasrnn_v_create_hdbf(&f[shetshik++],"DATEV", 'D', 8, 0,&header_len,&rec_len);
rasrnn_v_create_hdbf(&f[shetshik++],"NUM", 'C', 50, 0,&header_len,&rec_len);
rasrnn_v_create_hdbf(&f[shetshik++],"NAZP", 'C', 200, 0,&header_len,&rec_len);
rasrnn_v_create_hdbf(&f[shetshik++],"IPN", 'C', 20, 0,&header_len,&rec_len);

rasrnn_v_create_hdbf(&f[shetshik++],"ZAGSUM", 'N', 16, 2,&header_len,&rec_len);
rasrnn_v_create_hdbf(&f[shetshik++],"BAZOP20", 'N', 16, 2,&header_len,&rec_len);
rasrnn_v_create_hdbf(&f[shetshik++],"SUMPDV", 'N', 16, 2,&header_len,&rec_len);
rasrnn_v_create_hdbf(&f[shetshik++],"BAZOP0", 'N', 16, 2,&header_len,&rec_len);
rasrnn_v_create_hdbf(&f[shetshik++],"ZVILN", 'N', 16, 2,&header_len,&rec_len);
rasrnn_v_create_hdbf(&f[shetshik++],"EXPORT", 'N', 16, 2,&header_len,&rec_len);

rasrnn_v_create_hdbf(&f[shetshik++],"PZOB", 'N', 3, 0,&header_len,&rec_len);
rasrnn_v_create_hdbf(&f[shetshik++],"NREZ", 'N', 2, 0,&header_len,&rec_len);
rasrnn_v_create_hdbf(&f[shetshik++],"KOR", 'N', 2, 0,&header_len,&rec_len);
rasrnn_v_create_hdbf(&f[shetshik++],"WMDTYPE", 'N', 2, 0,&header_len,&rec_len);
rasrnn_v_create_hdbf(&f[shetshik++],"WMDTYPESTR", 'C', 4, 0,&header_len,&rec_len);



h.header = header_len + 1;/* length of the header
                           * includes the \r at end
                           */
h.lrecl= rec_len + 1;     /* length of a record
                           * includes the delete
                           * byte
                          */
/*
 printw("h.header=%d h.lrecl=%d\n",h.header,h.lrecl);
*/


fd = fileno(ff);

write(fd, &h, sizeof(h));
for(i=0; i < kolpol; i++) 
 {
  write(fd, &f[i], sizeof(DBASE_FIELD));
 }
fputc('\r', ff);

fclose(ff);

}
/*************************/
/*Создание dbf файла*/
/**********************/

void rasrnn_v_dbf(char *imaf_dbf_tmp,char *imaf_dbf,int nomer_xml_str,GtkWidget *wpredok)
{
/*перекодируем в DOS кодировку*/
iceb_perecod(1,imaf_dbf_tmp,wpredok);
/*Создаём заголовок dbf файла*/ 
rasrnn_v_creatheadbf(imaf_dbf,nomer_xml_str,wpredok);

/*Сливаем два файла*/
iceb_cat(imaf_dbf,imaf_dbf_tmp,wpredok);  


unlink(imaf_dbf_tmp);

}
