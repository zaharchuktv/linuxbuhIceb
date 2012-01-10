/*$Id: rasrnn_pw.c,v 1.28 2011-08-29 07:13:44 sasa Exp $*/
/*17.02.2011	30.08.2005	Белых А.И.	rasrnn_p.c
Распечатка реестра полученных налоговых накладных 
*/
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <errno.h>
#include "buhg_g.h"
#include <unistd.h>
#include "dbfhead.h"
#include "xrnn_poiw.h"
#include "rnn_d5w.h"
#define RAZMER_MAS_STR 9

void rasrnn_zvdbf(int nom_zap,short dp,short mp,short gp,char *nnn,char *naim,char *inn,double *suma,short dv,short mv,short gv,int,FILE *ff,int tipdok,const char *viddok,GtkWidget *wpredok);

void rasrnn_p_str_xml(int nomer_str,short d,short m,short g,short dv,short mv,short gv,char *nomer_dok,char *naim_kontr,char *inn,double *suma,const char *viddok,FILE *ff_xml);
void rasrnn_p_xml_itog(double *itog,double *itog_u,FILE *ff_xml);
int xml_nn_ukr(int *pnd,short dd,short md,short gd,char *nomdok,char *nom_nal_nak,short mn,short gn,short mk,int);
void rasrnn_p_svod(short dn,short mn,short gn,char *nomdok,char *naimkontr,double *mas_sum,int*,FILE *ff);
void rasrnn_p_rm(double proc_dot,double *itogo_o,double *itogo_o1,double *itogo_o2);
void rasrnn_p_creatheadbf(char *imaf,long kolz,GtkWidget*);
void rasrnn_p_dbf(const char *imaf_dbf_tmp,char *imaf_dbf,int nomer_xml_str,GtkWidget *wpredok);
void rpvnn_piw(short dn,short mn,short gn,char *vsego,double *itogo,double *itogo_u,int *kol_strok,FILE *ff);
void rasrnn_p_vvrozkor(int *nomer_str,short dnn,short mnn,short gnn,short dpn,short mpn,short gpn,char *nomer_dok,char *naim_kontr,char *inn,double suma_bez_nds,double suma_nds,const char *koment,FILE *ff_xml_rozkor);

extern char *organ;
extern int kol_strok_na_liste;
extern SQL_baza bd;

int rasrnn_pw(class xrnn_poiw *data,
int kol_strok,
int period_type,
FILE *ff,FILE *ff_prot,FILE *ff_xml,
FILE *ff_dot,FILE *ff_xml_dot,
FILE *ff_xml_rozkor,
FILE *ff_lst_rozkor,
class rnn_d5w *dod5,
char *imaf_dbf, /*имя файла для реестра полученных накладных в dbf формате*/
char *imaf_dbf_d, /*имя файла для для дополнительного реестра полученных накладных в dbf формате*/
GtkWidget *bar,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str imafil_xml;
//printw("\nРеестр полученных налоговых накладных %d.%d.%d => %d.%d.%d\n",dn,mn,gn,dk,mk,gk);
short dn,mn,gn;
short dk,mk,gk;
//iceb_rsdatp(&dn,&mn,&gn,data->datan.ravno(),&dk,&mk,&gk,data->datak.ravno(),wpredok);
iceb_u_rsdat(&dn,&mn,&gn,data->datan.ravno(),1);
iceb_u_rsdat(&dk,&mk,&gk,data->datak.ravno(),1);

sprintf(strsql,"select datd,nnn,sumd1,snds1,sumd2,snds2,sumd3,snds3,sumd4,snds4,\
inn,nkontr,mi,datdi,nomdi,gr,koment,ko,dvd,vd,nz from Reenn1 order by datd asc,nnn asc");

if(dn != 0 && dk == 0)
   sprintf(strsql,"select datd,nnn,sumd1,snds1,sumd2,snds2,sumd3,snds3,sumd4,snds4,\
inn,nkontr,mi,datdi,nomdi,gr,koment,ko,dvd,vd,nz from Reenn1 where \
datd >= '%04d-%02d-%02d'  order by datd asc,nnn asc",gn,mn,dn);

if(dn != 0 && dk != 0)
   sprintf(strsql,"select datd,nnn,sumd1,snds1,sumd2,snds2,sumd3,snds3,sumd4,snds4,\
inn,nkontr,mi,datdi,nomdi,gr,koment,ko,dvd,vd,nz from Reenn1 where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' order by datd asc,nnn asc",gn,mn,dn,gk,mk,dk);


SQLCURSOR cur,cur1;
SQL_str row,row1;
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

double proc_dot=data->proc_dot.ravno_atof();

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

if(proc_dot != 0.)
 {
  if((ff_dbf_d_tmp=fopen(imaf_dbf_d_tmp,"w")) == NULL)
   {
    iceb_er_op_fil(imaf_dbf_d_tmp,"",errno,wpredok);
    return(1);
   }
 }
int ibros=0;
/*Определяем какую шапку нужно печатать*/
class iceb_u_str imafalx("");
if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) < 0)
 imafalx.new_plus("rpvnn1.alx");

if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0  && iceb_u_sravmydat(dn,mn,gn,1,1,2011) < 0)
 imafalx.new_plus("rpvnn1_1.alx");

if(iceb_u_sravmydat(dn,mn,gn,1,1,2011) >= 0)
 imafalx.new_plus("rpvnn1_2.alx");
 
//Вставляем шапку 
kol_strok+=iceb_insfil(imafalx.ravno(),ff,wpredok);
if(ff_dot != NULL)
   ibros=iceb_insfil(imafalx.ravno(),ff_dot,wpredok);

iceb_u_zagolov(gettext("Реестр полученных налоговых накладных"),dn,mn,gn,dk,mk,gk,organ,ff_prot);
if(data->kodgr.getdlinna() > 1)
 fprintf(ff_prot,"%s:%s\n",gettext("Группа"),data->kodgr.ravno());

rpvnn_ps_pw(ff_prot);

fprintf(ff_lst_rozkor,"\
-----------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_lst_rozkor,"\
   |                               |Податкова накладна,  |Розрахунок коригуван.|Підстава  | Обсяг    | Сума     |\n\
   |   Постачальник(продавець)     |за якою пров.коригув.|кількісних і вартісн.|для кориг.|поставки  |  ПДВ     |\n\
   |                               |                     |показників до под.нак|податкових|(без ПДВ) | (+чи-)   |\n\
   |---------------------------------------------------------------------------|зобов'яза.| (+чи-)   |          |\n\
 N |   Найменування     |Індивідуал|  Дата    |  Номер   | Номер    |Дата отрим|з податку |          |          |\n\
   |(П.І.Б. для фізичних|  ьний    |          |          |          |мання отри|на додану |          |          |\n\
   |      осіб)         |подаковий |          |          |          |мувачем   |вартість  |          |          |\n\
   |                    | номер    |          |          |          |(покупцем)|          |          |          |\n\
   |                    |          |          |          |          |розрахунку|          |          |          |\n");
fprintf(ff_lst_rozkor,"\
-----------------------------------------------------------------------------------------------------------------\n");

double sumd[4];
double snds[4];
double sumdok=0.;
double sumnds=0.;

double itogo_mes[RAZMER_MAS_STR];
memset(itogo_mes,'\0',sizeof(itogo_mes));

double itogo_o[RAZMER_MAS_STR];
memset(itogo_o,'\0',sizeof(itogo_o));
double itogo_ou[RAZMER_MAS_STR];
memset(itogo_ou,'\0',sizeof(itogo_ou));
int nomer_str=0;

int npz=data->nom_p_zp.ravno_atoi();

if(npz != 0)
 nomer_str=npz-1;
 
short mz=0;
short d,m,g;
short dd,md,gd;
float kolstr1=0;
int metka_pods;
char podsis[10];
double sumd_pr[4];
double sumnds_pr[4];
memset(sumd_pr,'\0',sizeof(sumd_pr));
memset(sumnds_pr,'\0',sizeof(sumnds_pr));
double stroka_xml[RAZMER_MAS_STR];
double stroka_xml_dot[RAZMER_MAS_STR];
char nomdok[32];
char naim_kontr[512];
int nomer_xml_str=0;
int por_nom_dok=0;
int nomer_innn=0;
short dv,mv,gv;
double sumdok1=0.;
double itogo_o1[RAZMER_MAS_STR];
double itogo_o2[RAZMER_MAS_STR];
int korrekt=0;
int gtype=0;
int nomer_str_rozkor=0;
double itog_rozkor1=0.;
double itog_rozkor2=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(bar,kolstr,++kolstr1);    

  iceb_u_rsdat(&dv,&mv,&gv,row[18],2);  

  if(data->datavnn.getdlinna() > 1)
   if(iceb_u_sravmydat(iceb_u_datzap(row[18]),data->datavnn.ravno())  < 0)
    continue;
  if(data->datavnk.getdlinna() > 1)
   if(iceb_u_sravmydat(iceb_u_datzap(row[18]),data->datavnk.ravno())  < 0)
    continue;

  if(iceb_u_proverka(data->kodgr.ravno(),row[15],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->viddok.ravno(),row[19],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->kodop.ravno(),row[17],0,0) != 0)
   continue;

  if(data->naim_kontr.getdlinna() > 1 )
    if(iceb_u_strstrm(row[11],data->naim_kontr.ravno()) == 0)
      continue;
   
  if(data->podsistema.getdlinna() > 1)
   {
    char metka_pod[20];
    memset(metka_pod,'\0',sizeof(metka_pod));

    if(row[12][0] == '1')
     strcpy(metka_pod,gettext("МУ"));
    if(row[12][0] == '2')
     strcpy(metka_pod,gettext("УСЛ"));
    if(row[12][0] == '3')
     strcpy(metka_pod,gettext("УОС"));
    if(row[12][0] == '4')
     strcpy(metka_pod,gettext("УКР"));
    if(row[12][0] == '5')
     strcpy(metka_pod,gettext("ГК"));

    if(iceb_u_proverka(data->podsistema.ravno(),metka_pod,0,0) != 0)
     continue; 
   }
   
  if(iceb_u_proverka(data->nnn.ravno(),row[1],0,0) != 0)
     continue; 

  if(iceb_u_proverka(data->nomdok.ravno(),row[14],0,0) != 0)
     continue; 

  if(data->inn.getdlinna() > 1 )
    if(iceb_u_strstrm(row[10],data->inn.ravno()) == 0)
     continue; 

  if(data->koment.getdlinna() > 1 )
    if(iceb_u_strstrm(row[16],data->koment.ravno()) == 0)
     continue; 

  iceb_u_rsdat(&d,&m,&g,row[0],2);
  iceb_u_rsdat(&dd,&md,&gd,row[13],2);

  if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0)
  if((gtype=rnnvdw(1,row[19])) <= 0 )
    {
     sprintf(strsql,"Неизвестный тип документа %d %s !\n%d.%d.%d %s",
     gtype,row[19],d,m,g,row[1]);
     iceb_menu_soob(strsql,wpredok);
    }

  korrekt=0;  
  if(iceb_u_strstrm(row[19],"У") == 1 || iceb_u_SRAV(row[19],"РК",0) == 0)/*Если в виде документа присутствует буква У - то это коректирующий документ*/
    korrekt=1;

  if(mz != m)
   {
    if(mz != 0)
     {
      rasrnn_p_rm(proc_dot,itogo_mes,itogo_o1,itogo_o2);

      rpvnn_piw(dn,mn,gn,gettext("Всего за месяц"),itogo_o1,itogo_ou,&kol_strok,ff);
      if(ff_dot != NULL)
        rpvnn_piw(dn,mn,gn,gettext("Всего за месяц"),itogo_o2,itogo_ou,&ibros,ff_dot);
      
     }
    itogo_o[0]+=itogo_mes[0];
    itogo_o[1]+=itogo_mes[1];
    itogo_o[2]+=itogo_mes[2];
    itogo_o[3]+=itogo_mes[3];
    itogo_o[4]+=itogo_mes[4];
    itogo_o[5]+=itogo_mes[5];
    itogo_o[6]+=itogo_mes[6];
    itogo_o[7]+=itogo_mes[7];
    itogo_o[8]+=itogo_mes[8];

    sumd_pr[0]+=itogo_mes[1];
    sumd_pr[1]+=itogo_mes[3];
    sumd_pr[2]+=itogo_mes[5];
    sumd_pr[3]+=itogo_mes[7];
  
    sumnds_pr[0]=itogo_mes[2];
    sumnds_pr[1]=itogo_mes[4];
    sumnds_pr[2]=itogo_mes[6];
    sumnds_pr[3]=itogo_mes[8];
  
    memset(itogo_mes,'\0',sizeof(itogo_mes));
    mz=m;
   }
  sumd[0]=atof(row[2]);
  sumd[0]=iceb_u_okrug(sumd[0],0.01);
  snds[0]=atof(row[3]);
  snds[0]=iceb_u_okrug(snds[0],0.01);

  sumd[1]=atof(row[4]);
  sumd[1]=iceb_u_okrug(sumd[1],0.01);
  snds[1]=atof(row[5]);
  snds[1]=iceb_u_okrug(snds[1],0.01);


  sumd[2]=atof(row[6]);
  sumd[2]=iceb_u_okrug(sumd[2],0.01);
  snds[2]=atof(row[7]);
  snds[2]=iceb_u_okrug(snds[2],0.01);

  sumd[3]=atof(row[8]);
  sumd[3]=iceb_u_okrug(sumd[3],0.01);
  snds[3]=atof(row[9]);
  snds[3]=iceb_u_okrug(snds[3],0.01);
  
  if(korrekt == 1)
   {
    itogo_ou[0]+=sumd[0];
    itogo_ou[1]+=sumd[0]-snds[0];
    itogo_ou[2]+=snds[0];
    itogo_ou[3]+=sumd[1]-snds[1];
    itogo_ou[4]+=snds[1];
    itogo_ou[5]+=sumd[2]-snds[2];
    itogo_ou[6]+=snds[2];
    itogo_ou[7]+=sumd[3]-snds[3];
    itogo_ou[8]+=snds[3];
   }  
  
  metka_pods=atoi(row[12]);

  memset(nomdok,'\0',sizeof(nomdok));
  strncpy(nomdok,row[1],sizeof(nomdok)-1);

  memset(naim_kontr,'\0',sizeof(naim_kontr));
  strncpy(naim_kontr,row[11],sizeof(naim_kontr)-1);

  sumdok1=sumdok=sumd[0]+sumd[1]+sumd[2]+sumd[3];
  sumnds=snds[0]+snds[1]+snds[2]+snds[3];

  if(sumnds != 0.)
   if(row[10][0] == '\0' || isdigit(row[10][0]) == 0) /*значит символ не число*/
    {
     class iceb_u_str repl;
     sprintf(strsql,"%s:%d.%d.%d",gettext("Дата получения налоговой накладной"),d,m,g);
     repl.plus(strsql);
     
     sprintf(strsql,"%s:%s",gettext("Номер налоговой накладной"),row[1]);
     repl.ps_plus(strsql);

     sprintf(strsql,"%s:%s",gettext("Контрагент"),row[11]);
     repl.ps_plus(strsql);

     repl.ps_plus(gettext("Не ввели индивидуальный налоговый номер!"));
     iceb_menu_soob(&repl,wpredok);
    }  

  if(row[18][0] != '0')
   {
    sprintf(strsql,"select datd from Reenn1 where dvd='%s' and inn='%s' and nnn='%s' and nz != %s",
    row[18],row[10],row[1],row[20]);

    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) > 0)
     {
      sprintf(strsql,"Номер прибуткової податкової накладної %s дата виписки %s\nповторно введен в документ %s IНН %s",
      row[1],iceb_u_datzap(row[18]),iceb_u_datzap(row1[0]),row[10]);
      iceb_menu_soob(strsql,wpredok);
     }
   }

  if(proc_dot != 0.)
   {
    sumdok1=sumdok1*(100.-proc_dot)/100.;
    sumdok1=iceb_u_okrug(sumdok1,0.01);
   }
  
  if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0)
   {
    fprintf(ff,"|%-5d|%02d.%02d.%04d|%02d.%02d.%04d|%-*.*s|%-*.*s|%-*.*s|%-14s|%10.2f|",
    ++nomer_str,
    d,m,g,
    dv,mv,gv,
    iceb_u_kolbait(9,nomdok),iceb_u_kolbait(9,nomdok),nomdok,
    iceb_u_kolbait(5,row[19]),iceb_u_kolbait(5,row[19]),row[19],
    iceb_u_kolbait(20,naim_kontr),iceb_u_kolbait(20,naim_kontr),naim_kontr,
    row[10],
    sumdok1);

    if(ff_dot != NULL)
     fprintf(ff_dot,"|%-5d|%02d.%02d.%04d|%02d.%02d.%04d|%-*.*s|%-*.*s|%-*.*s|%-14s|%10.2f|",
     nomer_str,d,m,g,
     dv,mv,gv,
     iceb_u_kolbait(9,nomdok),iceb_u_kolbait(9,nomdok),nomdok,
     iceb_u_kolbait(5,row[19]),iceb_u_kolbait(5,row[19]),row[19],
     iceb_u_kolbait(20,naim_kontr),iceb_u_kolbait(20,naim_kontr),naim_kontr,
     row[10],sumdok-sumdok1);
   }      
  else
   {
    fprintf(ff,"|%-5d|%02d.%02d.%04d|%-*.*s|%-*.*s|%-14s|%10.2f|",
    ++nomer_str,d,m,g,
    iceb_u_kolbait(9,nomdok),iceb_u_kolbait(9,nomdok),nomdok,
    iceb_u_kolbait(20,naim_kontr),iceb_u_kolbait(20,naim_kontr),naim_kontr,
    row[10],
    sumdok1);

    if(ff_dot != NULL)
     fprintf(ff_dot,"|%-5d|%02d.%02d.%04d|%-*.*s|%-*.*s|%-14s|%10.2f|",
     nomer_str,d,m,g,
     iceb_u_kolbait(9,nomdok),iceb_u_kolbait(9,nomdok),nomdok,
     iceb_u_kolbait(20,naim_kontr),iceb_u_kolbait(20,naim_kontr),naim_kontr,
     row[10],sumdok-sumdok1);
   }     
  itogo_mes[0]+=sumdok;

  itogo_mes[1]+=sumd[0]-snds[0];
  itogo_mes[2]+=snds[0];    
  itogo_mes[3]+=sumd[1]-snds[1];
  itogo_mes[4]+=snds[1];    
  itogo_mes[5]+=sumd[2]-snds[2];
  itogo_mes[6]+=snds[2];    
  itogo_mes[7]+=sumd[3]-snds[3];
  itogo_mes[8]+=snds[3];    

  memset(stroka_xml,'\0',sizeof(stroka_xml));
  memset(stroka_xml_dot,'\0',sizeof(stroka_xml_dot));
  
  stroka_xml[0]=sumdok;
  stroka_xml[1]=sumd[0]-snds[0];
  stroka_xml[2]=snds[0];    
  stroka_xml[3]=sumd[1]-snds[1];
  stroka_xml[4]=snds[1];    
  stroka_xml[5]=sumd[2]-snds[2];
  stroka_xml[6]=snds[2];    
  stroka_xml[7]=sumd[3]-snds[3];
  stroka_xml[8]=snds[3];    

  if(proc_dot != 0.)
   {
    stroka_xml[0]=stroka_xml[0]*(100.0-proc_dot)/100.;
    stroka_xml[1]=stroka_xml[1]*(100.0-proc_dot)/100.;
    stroka_xml[2]=stroka_xml[2]*(100.0-proc_dot)/100.;
    stroka_xml[3]=stroka_xml[3]*(100.0-proc_dot)/100.;
    stroka_xml[4]=stroka_xml[4]*(100.0-proc_dot)/100.;
    stroka_xml[5]=stroka_xml[5]*(100.0-proc_dot)/100.;
    stroka_xml[6]=stroka_xml[6]*(100.0-proc_dot)/100.;
    stroka_xml[7]=stroka_xml[7]*(100.0-proc_dot)/100.;
    stroka_xml[8]=stroka_xml[8]*(100.0-proc_dot)/100.;
    
    stroka_xml[0]=iceb_u_okrug(stroka_xml[0],0.01);
    stroka_xml[1]=iceb_u_okrug(stroka_xml[1],0.01);
    stroka_xml[2]=iceb_u_okrug(stroka_xml[2],0.01);
    stroka_xml[3]=iceb_u_okrug(stroka_xml[3],0.01);
    stroka_xml[4]=iceb_u_okrug(stroka_xml[4],0.01);
    stroka_xml[5]=iceb_u_okrug(stroka_xml[5],0.01);
    stroka_xml[6]=iceb_u_okrug(stroka_xml[6],0.01);
    stroka_xml[7]=iceb_u_okrug(stroka_xml[7],0.01);
    stroka_xml[8]=iceb_u_okrug(stroka_xml[8],0.01);

   }
  if(korrekt == 1)/*Если в виде документа присутствует буква У - то это коректирующий документ*/
   {
    class iceb_u_str komentout(row[16]);
    if(komentout.getdlinna() <= 1)
      komentout.new_plus("Несвоєчасне отримання податкової накладної");
    
    rasrnn_p_vvrozkor(&nomer_str_rozkor,d,m,g,dv,mv,gv,nomdok,naim_kontr,row[10],(sumdok1-sumnds),sumnds,komentout.ravno(),ff_xml_rozkor);
    
    fprintf(ff_lst_rozkor,"%3d %-*.*s %-*s %02d.%02d.%04d %-*s %-10s %02d.%02d.%04d %-*.*s %10.2f %10.2f\n",
    nomer_str_rozkor,
    iceb_u_kolbait(20,naim_kontr),
    iceb_u_kolbait(20,naim_kontr),
    naim_kontr,
    iceb_u_kolbait(10,row[10]),row[10],
    dv,mv,gv,
    iceb_u_kolbait(10,nomdok),nomdok,
    "1",
    d,m,g,
    iceb_u_kolbait(10,komentout.ravno()),
    iceb_u_kolbait(10,komentout.ravno()),
    komentout.ravno(),
    sumdok1-sumnds,
    sumnds);
    
    itog_rozkor1+=sumdok1-sumnds;
    itog_rozkor2+=sumnds;
   }  

  rasrnn_p_str_xml(++nomer_xml_str,d,m,g,dv,mv,gv,nomdok,naim_kontr,row[10],stroka_xml,row[19],ff_xml);
  
  rasrnn_zvdbf(nomer_xml_str,d,m,g,nomdok,naim_kontr,row[10],stroka_xml,dv,mv,gv,korrekt,ff_dbf_tmp,gtype,row[19],wpredok);

  if(ff_xml_dot != NULL)
   {
    stroka_xml_dot[0]=sumdok-stroka_xml[0];
    stroka_xml_dot[1]=sumd[0]-snds[0]-stroka_xml[1];
    stroka_xml_dot[2]=snds[0]-stroka_xml[2];    
    stroka_xml_dot[3]=sumd[1]-snds[1]-stroka_xml[3];
    stroka_xml_dot[4]=snds[1]-stroka_xml[4];    
    stroka_xml_dot[5]=sumd[2]-snds[2]-stroka_xml[5];
    stroka_xml_dot[6]=snds[2]-stroka_xml[6];    
    stroka_xml_dot[7]=sumd[3]-snds[3]-stroka_xml[7];
    stroka_xml_dot[8]=snds[3]-stroka_xml[8];    
    
    rasrnn_p_str_xml(nomer_xml_str,d,m,g,dv,mv,gv,nomdok,naim_kontr,row[10],stroka_xml_dot,row[19],ff_xml_dot);

    rasrnn_zvdbf(nomer_xml_str,d,m,g,nomdok,naim_kontr,row[10],stroka_xml_dot,dv,mv,gv,korrekt,ff_dbf_d_tmp,gtype,row[19],wpredok);
   }  
  
  rasrnn_p_svod(dn,mn,gn,nomdok,naim_kontr,stroka_xml,&kol_strok,ff);
  if(ff_dot != NULL)
    rasrnn_p_svod(dn,mn,gn,nomdok,naim_kontr,stroka_xml_dot,&ibros,ff_dot);
   

  memset(podsis,'\0',sizeof(podsis));  
  if(metka_pods == 1)
   {
    strcpy(podsis,gettext("МУ"));
    //Запись накладной в xml формате
    xml_nn_matw(&por_nom_dok,1,dd,md,gd,row[14],row[1],mn,gn,mk,period_type,&imafil_xml,wpredok);
   }
  if(metka_pods == 2)
   {
    strcpy(podsis,gettext("УCЛ"));
    //Запись накладной в xml формате
    xml_nn_uslw(&por_nom_dok,1,dd,md,gd,row[14],row[1],mn,gn,mk,period_type,&imafil_xml,wpredok);
   }

  if(metka_pods == 3)
   {
    strcpy(podsis,gettext("УОC"));
    //Запись накладной в xml формате
    xml_nn_uosw(&por_nom_dok,1,dd,md,gd,row[14],row[1],mn,gn,mk,period_type,&imafil_xml,wpredok);
   }

  if(metka_pods == 4)
   {
    strcpy(podsis,gettext("УКР"));
    //Запись накладной в xml формате
    xml_nn_ukrw(&por_nom_dok,dd,md,gd,row[14],row[1],mn,gn,mk,period_type,&imafil_xml,wpredok);
   }
   

  fprintf(ff_prot,"%5d %02d.%02d.%04d %-10s %10s %8s %d %-5s %12s %-5s %02d.%02d.%04d %-10s %s\n",
  nomer_str,d,m,g,row[1],row[2],row[3],0,row[15],row[10],podsis,dd,md,gd,row[14],naim_kontr);
  fprintf(ff_prot,"%28s %9s %8s\n","",row[4],row[5]);
  fprintf(ff_prot,"%28s %9s %8s\n","",row[6],row[7]);
  fprintf(ff_prot,"%28s %9s %8s\n","",row[8],row[9]);

  if(korrekt == 0)
   {
    if(snds[0] != 0.)
     {
      if(dod5->innn1.find(row[10]) < 0)
       dod5->innn1.plus(row[10]);    
      
      sprintf(strsql,"%s|%02d.%04d",row[10],mv,gv);
      
      if((nomer_innn=dod5->innn_per1.find(strsql)) < 0)
       {
        dod5->innn_per1.plus(strsql);    
        dod5->naim_kontr1.plus(row[11]);
       }  

      dod5->suma_bez_nds1.plus(sumd[0]-snds[0],nomer_innn);
      dod5->suma_nds1.plus(snds[0],nomer_innn);
      if(metka_pods == 3) /*Учёт основных средств*/
       {
        dod5->os_sr1[0]+=sumd[0]-snds[0];
        dod5->os_sr1[1]+=snds[0];
       }
     }
    
    if(snds[1] != 0. || snds[2] != 0. || snds[3] != 0. )
     {
      if(dod5->innn1d.find(row[10]) < 0)
       dod5->innn1d.plus(row[10]);    

      sprintf(strsql,"%s|%02d.%04d",row[10],mv,gv);
      
      if((nomer_innn=dod5->innn_per1d.find(strsql)) < 0)
       {
        dod5->innn_per1d.plus(strsql);    
        dod5->naim_kontr1d.plus(row[11]);
        
        dod5->suma_bez_nds1d.plus(0.,-1);
        dod5->suma_nds1d.plus(0.,-1);
        nomer_innn=dod5->innn_per1d.find(strsql);
       }  

      if(snds[1] != 0.)
       {
        dod5->suma_bez_nds1d.plus(sumd[1]-snds[1],nomer_innn);
        dod5->suma_nds1d.plus(snds[1],nomer_innn);
       }   
      if(snds[2] != 0.)
       {
        dod5->suma_bez_nds1d.plus(sumd[2]-snds[2],nomer_innn);
        dod5->suma_nds1d.plus(snds[2],nomer_innn);
       }   
      if(snds[3] != 0.)
       {
        dod5->suma_bez_nds1d.plus(sumd[3]-snds[3],nomer_innn);
        dod5->suma_nds1d.plus(snds[3],nomer_innn);
       }   
     }
  }
 }

fclose(ff_dbf_tmp);
if(ff_dbf_d_tmp != NULL)
  fclose(ff_dbf_d_tmp);

sumd_pr[0]+=itogo_mes[1];
sumd_pr[1]+=itogo_mes[3];
sumd_pr[2]+=itogo_mes[5];
sumd_pr[3]+=itogo_mes[7];
  
sumnds_pr[0]=itogo_mes[2];
sumnds_pr[1]=itogo_mes[4];
sumnds_pr[2]=itogo_mes[6];
sumnds_pr[3]=itogo_mes[8];

fprintf(ff_prot,"%*s:%10.2f %8.2f\n",iceb_u_kolbait(27,gettext("Итого")),gettext("Итого"),sumd_pr[0],sumnds_pr[0]);
fprintf(ff_prot,"%27s %10.2f %8.2f\n","",sumd_pr[1],sumnds_pr[1]);
fprintf(ff_prot,"%27s %10.2f %8.2f\n","",sumd_pr[2],sumnds_pr[2]);
fprintf(ff_prot,"%27s %10.2f %8.2f\n","",sumd_pr[3],sumnds_pr[3]);

rasrnn_p_rm(proc_dot,itogo_mes,itogo_o1,itogo_o2);

rpvnn_piw(dn,mn,gn,gettext("Всего за месяц"),itogo_o1,itogo_ou,&kol_strok,ff);
if(ff_dot != NULL)
  rpvnn_piw(dn,mn,gn,gettext("Всего за месяц"),itogo_o2,itogo_ou,&ibros,ff_dot);


itogo_o[0]+=itogo_mes[0];
itogo_o[1]+=itogo_mes[1];
itogo_o[2]+=itogo_mes[2];
itogo_o[3]+=itogo_mes[3];
itogo_o[4]+=itogo_mes[4];
itogo_o[5]+=itogo_mes[5];
itogo_o[6]+=itogo_mes[6];
itogo_o[7]+=itogo_mes[7];
itogo_o[8]+=itogo_mes[8];

rasrnn_p_rm(proc_dot,itogo_o,itogo_o1,itogo_o2);



if(mn != mk)
 {
  rpvnn_piw(dn,mn,gn,gettext("Общий итог"),itogo_o1,itogo_ou,&kol_strok,ff);
  if(ff_dot != NULL)
    rpvnn_piw(dn,mn,gn,gettext("Общий итог"),itogo_o2,itogo_ou,&ibros,ff_dot);
 }

rasrnn_p_xml_itog(itogo_o1,itogo_ou,ff_xml);
if(ff_xml_dot != NULL)
  rasrnn_p_xml_itog(itogo_o2,itogo_ou,ff_xml_dot);

sprintf(imaf_dbf,"rnn_p_dbf_%02d%d.dbf",mn,gn);
rasrnn_p_dbf(imaf_dbf_tmp,imaf_dbf,nomer_xml_str,wpredok);

if(ff_dbf_d_tmp != NULL)
 {
  sprintf(imaf_dbf_d,"rnn_p_dbf_d_%02d%d.dbf",mn,gn);
  rasrnn_p_dbf(imaf_dbf_d_tmp,imaf_dbf_d,nomer_xml_str,wpredok);
 }

if(nomer_str_rozkor > 0)
 {
  fprintf(ff_xml_rozkor,"   <R02G9>%.2f</R02G9>\n",itog_rozkor1);
  fprintf(ff_xml_rozkor,"   <R02G10>%.2f</R02G10>\n",itog_rozkor2);

  fprintf(ff_lst_rozkor,"%*s:%10.2f %10.2f\n",
  iceb_u_kolbait(96,gettext("Итого")),gettext("Итого"),
  itog_rozkor1,itog_rozkor2);  

 }
return(0);
}
/********************************/
/*Вывод строки в xml файл*/
/**************************/
void rasrnn_p_str_xml(int nomer_str,
short d,short m,short g,
short dv,short mv,short gv,
char *nomer_dok,
char *naim_kontr,
char *inn,
double *suma,
const char *viddok,
FILE *ff_xml)
{

/*************************
fprintf(ff_xml,"   <T1RXXXXG1 ROWNUM=\"%d\">%d</T1RXXXXG1>\n",nomer_str,nomer_str); //1
fprintf(ff_xml,"   <T1RXXXXG2D ROWNUM=\"%d\">%02d%02d%d</T1RXXXXG2D>\n",nomer_str,d,m,g); //1
fprintf(ff_xml,"   <T1RXXXXG3D ROWNUM=\"%d\">%02d%02d%d</T1RXXXXG3D>\n",nomer_str,dv,mv,gv); //2

if(nomer_dok[0] != '\0')
 fprintf(ff_xml,"   <T1RXXXXG4S ROWNUM=\"%d\">%s</T1RXXXXG4S>\n",nomer_str,iceb_u_filtr_xml(nomer_dok)); //3
else
 fprintf(ff_xml,"   <T1RXXXXG4S xsi:nil=\"true\" ROWNUM=\"%d\"></T1RXXXXG4S>\n",nomer_str); //3

if(viddok[0] != '\0')
 fprintf(ff_xml,"   <T1RXXXXG5S ROWNUM=\"%d\">%s</T1RXXXXG5S>\n",nomer_str,iceb_u_filtr_xml(viddok)); //3
else
 fprintf(ff_xml,"   <T1RXXXXG5S xsi:nil=\"true\" ROWNUM=\"%d\"></T1RXXXXG5S>\n",nomer_str); //3

if(naim_kontr[0] != '\0')
 fprintf(ff_xml,"   <T1RXXXXG6S ROWNUM=\"%d\">%s</T1RXXXXG6S>\n",nomer_str,iceb_u_filtr_xml(naim_kontr)); //4
else
 fprintf(ff_xml,"   <T1RXXXXG6S xsi:nil=\"true\" ROWNUM=\"%d\"></T1RXXXXG6S>\n",nomer_str); //4

if(inn[0] == '\0' || isdigit(inn[0]) == 0)  //Значит символ не число
  fprintf(ff_xml,"   <T1RXXXXG7 xsi:nil=\"true\" ROWNUM=\"%d\"/>\n",nomer_str);
else
  fprintf(ff_xml,"   <T1RXXXXG7 ROWNUM=\"%d\">%s</T1RXXXXG7>\n",nomer_str,inn); //5
  
fprintf(ff_xml,"   <T1RXXXXG8 ROWNUM=\"%d\">%.2f</T1RXXXXG8>\n",nomer_str,suma[0]); //6
fprintf(ff_xml,"   <T1RXXXXG9 ROWNUM=\"%d\">%.2f</T1RXXXXG9>\n",nomer_str,suma[1]); //7
fprintf(ff_xml,"   <T1RXXXXG10 ROWNUM=\"%d\">%.2f</T1RXXXXG10>\n",nomer_str,suma[2]); //8
fprintf(ff_xml,"   <T1RXXXXG11 ROWNUM=\"%d\">%.2f</T1RXXXXG11>\n",nomer_str,suma[3]); //9
fprintf(ff_xml,"   <T1RXXXXG12 ROWNUM=\"%d\">%.2f</T1RXXXXG12>\n",nomer_str,suma[4]); //10
fprintf(ff_xml,"   <T1RXXXXG13 ROWNUM=\"%d\">%.2f</T1RXXXXG13>\n",nomer_str,suma[5]); //11
fprintf(ff_xml,"   <T1RXXXXG14 ROWNUM=\"%d\">%.2f</T1RXXXXG14>\n",nomer_str,suma[6]); //12
fprintf(ff_xml,"   <T1RXXXXG15 ROWNUM=\"%d\">%.2f</T1RXXXXG15>\n",nomer_str,suma[7]); //13
fprintf(ff_xml,"   <T1RXXXXG16 ROWNUM=\"%d\">%.2f</T1RXXXXG16>\n",nomer_str,suma[8]); //14
****************************/

fprintf(ff_xml,"   <T2RXXXXG1 ROWNUM=\"%d\">%d</T2RXXXXG1>\n",nomer_str,nomer_str); //1
fprintf(ff_xml,"   <T2RXXXXG2D ROWNUM=\"%d\">%02d%02d%d</T2RXXXXG2D>\n",nomer_str,d,m,g); //1
fprintf(ff_xml,"   <T2RXXXXG3D ROWNUM=\"%d\">%02d%02d%d</T2RXXXXG3D>\n",nomer_str,dv,mv,gv); //2

if(nomer_dok[0] != '\0')
 fprintf(ff_xml,"   <T2RXXXXG4S ROWNUM=\"%d\">%s</T2RXXXXG4S>\n",nomer_str,iceb_u_filtr_xml(nomer_dok)); //3
else
 fprintf(ff_xml,"   <T2RXXXXG4S xsi:nil=\"true\" ROWNUM=\"%d\"></T2RXXXXG4S>\n",nomer_str); //3

if(viddok[0] != '\0')
 fprintf(ff_xml,"   <T2RXXXXG5S ROWNUM=\"%d\">%s</T2RXXXXG5S>\n",nomer_str,iceb_u_filtr_xml(viddok)); //3
else
 fprintf(ff_xml,"   <T2RXXXXG5S xsi:nil=\"true\" ROWNUM=\"%d\"></T2RXXXXG5S>\n",nomer_str); //3

if(naim_kontr[0] != '\0')
 fprintf(ff_xml,"   <T2RXXXXG6S ROWNUM=\"%d\">%s</T2RXXXXG6S>\n",nomer_str,iceb_u_filtr_xml(naim_kontr)); //4
else
 fprintf(ff_xml,"   <T2RXXXXG6S xsi:nil=\"true\" ROWNUM=\"%d\"></T2RXXXXG6S>\n",nomer_str); //4

if(inn[0] == '\0' || isdigit(inn[0]) == 0)  //Значит символ не число
  fprintf(ff_xml,"   <T2RXXXXG7 xsi:nil=\"true\" ROWNUM=\"%d\"/>\n",nomer_str);
else
  fprintf(ff_xml,"   <T2RXXXXG7 ROWNUM=\"%d\">%s</T2RXXXXG7>\n",nomer_str,inn); //5
  
fprintf(ff_xml,"   <T2RXXXXG8 ROWNUM=\"%d\">%.2f</T2RXXXXG8>\n",nomer_str,suma[0]); //6
fprintf(ff_xml,"   <T2RXXXXG9 ROWNUM=\"%d\">%.2f</T2RXXXXG9>\n",nomer_str,suma[1]); //7
fprintf(ff_xml,"   <T2RXXXXG10 ROWNUM=\"%d\">%.2f</T2RXXXXG10>\n",nomer_str,suma[2]); //8
fprintf(ff_xml,"   <T2RXXXXG11 ROWNUM=\"%d\">%.2f</T2RXXXXG11>\n",nomer_str,suma[3]); //9
fprintf(ff_xml,"   <T2RXXXXG12 ROWNUM=\"%d\">%.2f</T2RXXXXG12>\n",nomer_str,suma[4]); //10
fprintf(ff_xml,"   <T2RXXXXG13 ROWNUM=\"%d\">%.2f</T2RXXXXG13>\n",nomer_str,suma[5]); //11
fprintf(ff_xml,"   <T2RXXXXG14 ROWNUM=\"%d\">%.2f</T2RXXXXG14>\n",nomer_str,suma[6]); //12
fprintf(ff_xml,"   <T2RXXXXG15 ROWNUM=\"%d\">%.2f</T2RXXXXG15>\n",nomer_str,suma[7]); //13
fprintf(ff_xml,"   <T2RXXXXG16 ROWNUM=\"%d\">%.2f</T2RXXXXG16>\n",nomer_str,suma[8]); //14

}
/*****************************/
/*Вывод в xml файл расчёта корректировки*/
/*******************************/
void rasrnn_p_vvrozkor(int *nomer_str,
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
fprintf(ff_xml_rozkor,"   <T2RXXXXG2S ROWNUM=\"%d\">%s</T2RXXXXG2S>\n",*nomer_str,iceb_u_filtr_xml(naim_kontr));
fprintf(ff_xml_rozkor,"   <T2RXXXXG3 ROWNUM=\"%d\">%s</T2RXXXXG3>\n",*nomer_str,inn);
fprintf(ff_xml_rozkor,"   <T2RXXXXG4D ROWNUM=\"%d\">%02d%02d%d</T2RXXXXG4D>\n",*nomer_str,dpn,mpn,gpn);
fprintf(ff_xml_rozkor,"   <T2RXXXXG5S ROWNUM=\"%d\">%s</T2RXXXXG5S>\n",*nomer_str,iceb_u_filtr_xml(nomer_dok));
fprintf(ff_xml_rozkor,"   <T2RXXXXG6S ROWNUM=\"%d\">1</T2RXXXXG6S>\n",*nomer_str); /*номер расчёта корректировки*/
fprintf(ff_xml_rozkor,"   <T2RXXXXG7D ROWNUM=\"%d\">%02d%02d%d</T2RXXXXG7D>\n",*nomer_str,dnn,mnn,gnn);
fprintf(ff_xml_rozkor,"   <T2RXXXXG8S ROWNUM=\"%d\">%s</T2RXXXXG8S>\n",*nomer_str,iceb_u_filtr_xml(koment));
fprintf(ff_xml_rozkor,"   <T2RXXXXG9 ROWNUM=\"%d\">%.2f</T2RXXXXG9>\n",*nomer_str,suma_bez_nds);
fprintf(ff_xml_rozkor,"   <T2RXXXXG10 ROWNUM=\"%d\">%.2f</T2RXXXXG10>\n",*nomer_str,suma_nds);

}
/********************************/
/*Распечатать итог*/
/*****************************/
void rasrnn_p_xml_itog(double *itog,double *itog_u,FILE *ff_xml)
{
/***********************
fprintf(ff_xml,"   <T1R01G8>%.2f</T1R01G8>\n",itog[0]);
fprintf(ff_xml,"   <T1R01G9>%.2f</T1R01G9>\n",itog[1]);
fprintf(ff_xml,"   <T1R01G10>%.2f</T1R01G10>\n",itog[2]);
fprintf(ff_xml,"   <T1R01G11>%.2f</T1R01G11>\n",itog[3]);
fprintf(ff_xml,"   <T1R01G12>%.2f</T1R01G12>\n",itog[4]);
fprintf(ff_xml,"   <T1R01G13>%.2f</T1R01G13>\n",itog[5]);
fprintf(ff_xml,"   <T1R01G14>%.2f</T1R01G14>\n",itog[6]);
fprintf(ff_xml,"   <T1R01G15>%.2f</T1R01G15>\n",itog[7]);
fprintf(ff_xml,"   <T1R01G16>%.2f</T1R01G16>\n",itog[8]);

fprintf(ff_xml,"   <T1R02G8>%.2f</T1R02G8>\n",itog_u[0]);
fprintf(ff_xml,"   <T1R02G9>%.2f</T1R02G9>\n",itog_u[1]);
fprintf(ff_xml,"   <T1R02G10>%.2f</T1R02G10>\n",itog_u[2]);
fprintf(ff_xml,"   <T1R02G11>%.2f</T1R02G11>\n",itog_u[3]);
fprintf(ff_xml,"   <T1R02G12>%.2f</T1R02G12>\n",itog_u[4]);
fprintf(ff_xml,"   <T1R02G13>%.2f</T1R02G13>\n",itog_u[5]);
fprintf(ff_xml,"   <T1R02G14>%.2f</T1R02G14>\n",itog_u[6]);
fprintf(ff_xml,"   <T1R02G15>%.2f</T1R02G15>\n",itog_u[7]);
fprintf(ff_xml,"   <T1R02G16>%.2f</T1R02G16>\n",itog_u[8]);
***********************/
fprintf(ff_xml,"   <R021G8>%.2f</R021G8>\n",itog[0]);
fprintf(ff_xml,"   <R021G9>%.2f</R021G9>\n",itog[1]);
fprintf(ff_xml,"   <R021G10>%.2f</R021G10>\n",itog[2]);
fprintf(ff_xml,"   <R021G11>%.2f</R021G11>\n",itog[3]);
fprintf(ff_xml,"   <R021G12>%.2f</R021G12>\n",itog[4]);
fprintf(ff_xml,"   <R021G13>%.2f</R021G13>\n",itog[5]);
fprintf(ff_xml,"   <R021G14>%.2f</R021G14>\n",itog[6]);
fprintf(ff_xml,"   <R021G15>%.2f</R021G15>\n",itog[7]);
fprintf(ff_xml,"   <R021G16>%.2f</R021G16>\n",itog[8]);

fprintf(ff_xml,"   <R022G8>%.2f</R022G8>\n",itog_u[0]);
fprintf(ff_xml,"   <R022G9>%.2f</R022G9>\n",itog_u[1]);
fprintf(ff_xml,"   <R022G10>%.2f</R022G10>\n",itog_u[2]);
fprintf(ff_xml,"   <R022G11>%.2f</R022G11>\n",itog_u[3]);
fprintf(ff_xml,"   <R022G12>%.2f</R022G12>\n",itog_u[4]);
fprintf(ff_xml,"   <R022G13>%.2f</R022G13>\n",itog_u[5]);
fprintf(ff_xml,"   <R022G14>%.2f</R022G14>\n",itog_u[6]);
fprintf(ff_xml,"   <R022G15>%.2f</R022G15>\n",itog_u[7]);
fprintf(ff_xml,"   <R022G16>%.2f</R022G16>\n",itog_u[8]);

}
/****************************/
/*Вывод концовки строки в распечатку*/
/****************************/
void rasrnn_p_svod(short dn,short mn,short gn,char *nomdok,char *naim_kontr,double *mas_sum,int *kol_strok,FILE *ff)
{
if(mas_sum[1] != 0. || mas_sum[2] != 0.)
 {
  if(mas_sum[2] != 0.)
    fprintf(ff,"%9.2f|%8.2f|",mas_sum[1],mas_sum[2]);
  else
    fprintf(ff,"%9.2f|XXXXXXXX|",mas_sum[1]);
 }
else
 fprintf(ff,"%9s|%8s|","","");

if(mas_sum[3] != 0. || mas_sum[4] != 0.)
 {
  if(mas_sum[4] != 0.)
    fprintf(ff,"%9.2f|%8.2f|",mas_sum[3],mas_sum[4]);
  else
    fprintf(ff,"%9.2f|XXXXXXXX|",mas_sum[3]);
 }
else
 fprintf(ff,"%9s|%8s|","","");

if(mas_sum[5] != 0. || mas_sum[6] != 0.)
 {
  if(mas_sum[6] != 0.)
    fprintf(ff,"%9.2f|%8.2f|",mas_sum[5],mas_sum[6]);
  else
    fprintf(ff,"%9.2f|XXXXXXXX|",mas_sum[5]);
 }
else
 fprintf(ff,"%9s|%8s|","","");

if(mas_sum[7] != 0. || mas_sum[8] != 0.)
 {
  if(mas_sum[8] != 0.)
    fprintf(ff,"%9.2f|%8.2f|",mas_sum[7],mas_sum[8]);
  else
    fprintf(ff,"%9.2f|XXXXXXXX|",mas_sum[7]);
 }
else
 fprintf(ff,"%9s|%8s|","","");

fprintf(ff,"\n");

if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0)
 {
  if(iceb_u_strlen(naim_kontr) > 20 || iceb_u_strlen(nomdok) > 9)
   {
    fprintf(ff,"|%5s|%10s|%10s|%-*.*s|%5s|%-*.*s|%14s|%10s|%9s|%8s|%9s|%8s|%9s|%8s|%9s|%8s|\n",
    "","","",
    iceb_u_kolbait(9,iceb_u_adrsimv(9,nomdok)),
    iceb_u_kolbait(9,iceb_u_adrsimv(9,nomdok)),
    iceb_u_adrsimv(9,nomdok),
    "",
    iceb_u_kolbait(20,iceb_u_adrsimv(20,naim_kontr)),
    iceb_u_kolbait(20,iceb_u_adrsimv(20,naim_kontr)),
    iceb_u_adrsimv(20,naim_kontr),
    "","","","","","","","","","");
   }
  if(iceb_u_strlen(naim_kontr) > 40 || iceb_u_strlen(nomdok) > 18)
   {
    fprintf(ff,"|%5s|%10s|%10s|%-*.*s|%5s|%-*.*s|%14s|%10s|%9s|%8s|%9s|%8s|%9s|%8s|%9s|%8s|\n",
    "","","",
    iceb_u_kolbait(9,iceb_u_adrsimv(18,nomdok)),
    iceb_u_kolbait(9,iceb_u_adrsimv(18,nomdok)),
    iceb_u_adrsimv(18,nomdok),
    "",
    iceb_u_kolbait(20,iceb_u_adrsimv(40,naim_kontr)),
    iceb_u_kolbait(20,iceb_u_adrsimv(40,naim_kontr)),
    iceb_u_adrsimv(40,naim_kontr),
    "","","","","","","","","","");
   }
  for(int nom=60; nom < iceb_u_strlen(naim_kontr); nom+=20)
   {
    fprintf(ff,"|%5s|%10s|%10s|%9s|%5s|%-*.*s|%14s|%10s|%9s|%8s|%9s|%8s|%9s|%8s|%9s|%8s|\n",
    "","","","","",
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim_kontr)),
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim_kontr)),
    iceb_u_adrsimv(nom,naim_kontr),
    "","","","","","","","","","");
   }
 }
else
 {
  if(iceb_u_strlen(naim_kontr) > 20 || iceb_u_strlen(nomdok) > 9)
   {
    fprintf(ff,"|%5s|%10s|%-*.*s|%-*.*s|%14s|%10s|%9s|%8s|%9s|%8s|%9s|%8s|%9s|%8s|\n",
    "","",
    iceb_u_kolbait(9,iceb_u_adrsimv(9,nomdok)),
    iceb_u_kolbait(9,iceb_u_adrsimv(9,nomdok)),
    iceb_u_adrsimv(9,nomdok),
    iceb_u_kolbait(20,iceb_u_adrsimv(20,naim_kontr)),
    iceb_u_kolbait(20,iceb_u_adrsimv(20,naim_kontr)),
    iceb_u_adrsimv(20,naim_kontr),
    "","","","","","","","","","");
   }
  if(iceb_u_strlen(naim_kontr) > 40 || iceb_u_strlen(nomdok) > 18)
   {
    fprintf(ff,"|%5s|%10s|%-*.*s|%-*.*s|%14s|%10s|%9s|%8s|%9s|%8s|%9s|%8s|%9s|%8s|\n",
    "","",
    iceb_u_kolbait(9,iceb_u_adrsimv(18,nomdok)),
    iceb_u_kolbait(9,iceb_u_adrsimv(18,nomdok)),
    iceb_u_adrsimv(18,nomdok),
    iceb_u_kolbait(20,iceb_u_adrsimv(40,naim_kontr)),
    iceb_u_kolbait(20,iceb_u_adrsimv(40,naim_kontr)),
    iceb_u_adrsimv(40,naim_kontr),
    "","","","","","","","","","");
   }
  for(int nom=60; nom < iceb_u_strlen(naim_kontr); nom+=20)
   {
    fprintf(ff,"|%5s|%10s|%9s|%-*.*s|%14s|%10s|%9s|%8s|%9s|%8s|%9s|%8s|%9s|%8s|\n",
    "","","",
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim_kontr)),
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim_kontr)),
    iceb_u_adrsimv(nom,naim_kontr),
    "","","","","","","","","","");
   }
 }
}
/************************/
/*разбивка массива по проценту на два массива*/
/*******************************/
void rasrnn_p_rm(double proc_dot,double *itogo_o,double *itogo_o1,double *itogo_o2)
{
memset(itogo_o1,'\0',RAZMER_MAS_STR);
memset(itogo_o2,'\0',RAZMER_MAS_STR);

itogo_o1[0]=itogo_o[0];
itogo_o1[1]=itogo_o[1];
itogo_o1[2]=itogo_o[2];
itogo_o1[3]=itogo_o[3];
itogo_o1[4]=itogo_o[4];
itogo_o1[5]=itogo_o[5];
itogo_o1[6]=itogo_o[6];
itogo_o1[7]=itogo_o[7];
itogo_o1[8]=itogo_o[8];

if(proc_dot != 0.)
 {

  itogo_o1[0]=itogo_o[0]*(100.-proc_dot)/100.;
  itogo_o1[1]=itogo_o[1]*(100.-proc_dot)/100.;
  itogo_o1[2]=itogo_o[2]*(100.-proc_dot)/100.;
  itogo_o1[3]=itogo_o[3]*(100.-proc_dot)/100.;
  itogo_o1[4]=itogo_o[4]*(100.-proc_dot)/100.;
  itogo_o1[5]=itogo_o[5]*(100.-proc_dot)/100.;
  itogo_o1[6]=itogo_o[6]*(100.-proc_dot)/100.;
  itogo_o1[7]=itogo_o[7]*(100.-proc_dot)/100.;
  itogo_o1[8]=itogo_o[8]*(100.-proc_dot)/100.;

  itogo_o1[0]=iceb_u_okrug(itogo_o1[0],0.01);
  itogo_o1[1]=iceb_u_okrug(itogo_o1[1],0.01);
  itogo_o1[2]=iceb_u_okrug(itogo_o1[2],0.01);
  itogo_o1[3]=iceb_u_okrug(itogo_o1[3],0.01);
  itogo_o1[4]=iceb_u_okrug(itogo_o1[4],0.01);
  itogo_o1[5]=iceb_u_okrug(itogo_o1[5],0.01);
  itogo_o1[6]=iceb_u_okrug(itogo_o1[6],0.01);
  itogo_o1[7]=iceb_u_okrug(itogo_o1[7],0.01);
  itogo_o1[8]=iceb_u_okrug(itogo_o1[8],0.01);


  itogo_o2[0]=itogo_o[0]-itogo_o1[0];
  itogo_o2[1]=itogo_o[1]-itogo_o1[1];
  itogo_o2[2]=itogo_o[2]-itogo_o1[2];
  itogo_o2[3]=itogo_o[3]-itogo_o1[3];
  itogo_o2[4]=itogo_o[4]-itogo_o1[4];
  itogo_o2[5]=itogo_o[5]-itogo_o1[5];
  itogo_o2[6]=itogo_o[6]-itogo_o1[6];
  itogo_o2[7]=itogo_o[7]-itogo_o1[7];
  itogo_o2[8]=itogo_o[8]-itogo_o1[8];


 }

}
/*********************/
/*Запись в dbf файл*/
/*******************/
void rasrnn_zvdbf(int nom_zap, /*Номер записи*/
short dp,short mp,short gp,  /*Дата получения*/
char *nnn, /*Номер налоговой накладной*/
char *naim, /*Наименование контрагента*/
char *inn,  /*индивидуальный налоговый номер*/
double *suma, /*массив с суммами*/
short dv,short mv,short gv, /*дата выписки*/
int korrekt, /*признак корректировки*/
FILE *ff,
int tipdok,
const char *viddok,
GtkWidget *wpredok)
{
int pkred=0; /*Податковий кредит*/
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
%16.2f%16.2f%16.2f%16.2f%16.2f%16.2f%16.2f%16.2f%16.2f%3d\
%2d%2d%2d%-*.*s\
%04d%02d%02d",
nom_zap,
gp,mp,dp,
iceb_u_kolbait(50,nnn),iceb_u_kolbait(50,nnn),nnn,
iceb_u_kolbait(200,naim),iceb_u_kolbait(200,naim),naim,
inn,
suma[0],
suma[1],
suma[2],
suma[3],
suma[4],
suma[5],
suma[6],
suma[7],
suma[8],
pkred,
rez,
korrekt,
tipdok,
iceb_u_kolbait(4,viddok),iceb_u_kolbait(4,viddok),viddok,
gv,mv,dv);

}
/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void rasrnn_p_create_hdbf(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
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
void rasrnn_p_creatheadbf(char *imaf,long kolz,GtkWidget *wpredok)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
#define		kolpol 20 //Количество колонок (полей) в dbf файле
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
rasrnn_p_create_hdbf(&f[shetshik++],"NPP", 'C', 7, 0,&header_len,&rec_len);
rasrnn_p_create_hdbf(&f[shetshik++],"DATEV", 'D', 8, 0,&header_len,&rec_len);
rasrnn_p_create_hdbf(&f[shetshik++],"NUM", 'C', 50, 0,&header_len,&rec_len);
rasrnn_p_create_hdbf(&f[shetshik++],"NAZP", 'C', 200, 0,&header_len,&rec_len);
rasrnn_p_create_hdbf(&f[shetshik++],"IPN", 'C', 20, 0,&header_len,&rec_len);

rasrnn_p_create_hdbf(&f[shetshik++],"ZAGSUM", 'N', 16, 2,&header_len,&rec_len);
rasrnn_p_create_hdbf(&f[shetshik++],"VART7", 'N', 16, 2,&header_len,&rec_len);
rasrnn_p_create_hdbf(&f[shetshik++],"SUM8", 'N', 16, 2,&header_len,&rec_len);
rasrnn_p_create_hdbf(&f[shetshik++],"VART9", 'N', 16, 2,&header_len,&rec_len);
rasrnn_p_create_hdbf(&f[shetshik++],"SUM10", 'N', 16, 2,&header_len,&rec_len);
rasrnn_p_create_hdbf(&f[shetshik++],"VART11", 'N', 16, 2,&header_len,&rec_len);
rasrnn_p_create_hdbf(&f[shetshik++],"SUM12", 'N', 16, 2,&header_len,&rec_len);
rasrnn_p_create_hdbf(&f[shetshik++],"VART13", 'N', 16, 2,&header_len,&rec_len);
rasrnn_p_create_hdbf(&f[shetshik++],"SUM14", 'N', 16, 2,&header_len,&rec_len);

rasrnn_p_create_hdbf(&f[shetshik++],"PKRED", 'N', 3, 0,&header_len,&rec_len);
rasrnn_p_create_hdbf(&f[shetshik++],"NREZ", 'N', 2, 0,&header_len,&rec_len);
rasrnn_p_create_hdbf(&f[shetshik++],"KOR", 'N', 2, 0,&header_len,&rec_len);
rasrnn_p_create_hdbf(&f[shetshik++],"WMDTYPE", 'N', 2, 0,&header_len,&rec_len);
rasrnn_p_create_hdbf(&f[shetshik++],"WMDTYPESTR", 'C', 4, 0,&header_len,&rec_len);

rasrnn_p_create_hdbf(&f[shetshik++],"DTVP", 'D', 8, 0,&header_len,&rec_len);


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

void rasrnn_p_dbf(const char *imaf_dbf_tmp,char *imaf_dbf,int nomer_xml_str,GtkWidget *wpredok)
{
iceb_perecod(1,imaf_dbf_tmp,wpredok);
/*Создаём заголовок dbf файла*/ 
rasrnn_p_creatheadbf(imaf_dbf,nomer_xml_str,wpredok);

/*Сливаем два файла*/
iceb_cat(imaf_dbf,imaf_dbf_tmp,wpredok);  


unlink(imaf_dbf_tmp);

}
