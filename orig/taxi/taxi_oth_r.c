/*$Id: taxi_oth_r.c,v 1.34 2011-02-21 07:36:21 sasa Exp $*/
/*21.05.2005	09.12.2003	Белых А.И.	taxi_oth_r.c
Формирование отчета по водителям и операторам
*/
#include <stdlib.h>
#include <unistd.h>
#include        <pwd.h>
#include <time.h>
#include <errno.h>
#include "taxi.h"
#include "taxi_oth.h"

gint  taxi_oth_r1(class taxi_oth_data *data);
void sort_vod(iceb_u_spisok *vod,GtkWidget *wpredok);

extern SQL_baza	bd;
extern char *name_system;

int  taxi_oth_r(class taxi_oth_data *data)
{
char strsql[300];
short dn,mn,gn;
short dk,mk,gk;
short hasn=0,minn=0,sekn=0;
short hask=0,mink=0,sekk=0;

iceb_u_rsdat(&dn,&mn,&gn,data->datn.ravno(),1);
iceb_u_rsdat(&dk,&mk,&gk,data->datk.ravno(),1);
iceb_u_rstime(&hasn,&minn,&sekn,data->vremn.ravno());
if(data->vremk.getdlinna() <= 1)
 {
  hask=24; mink=0; sekk=0;
 }
else
  iceb_u_rstime(&hask,&mink,&sekk,data->vremk.ravno());

if(data->metkarr == 1) //По дате заказа
   sprintf(strsql,"select kz,kv,suma,ktoi,kodk from Taxizak where \
(datz='%d-%d-%d' and vremz >= '%02d:%02d:%02d') or \
(datz='%d-%d-%d' and vremz <= '%02d:%02d:%02d') or \
(datz > '%d-%d-%d' and datz < '%d-%d-%d') order by kz asc",
gn,mn,dn,hasn,minn,sekn,
gk,mk,dk,hask,mink,sekk,
gn,mn,dn,gk,mk,dk);

if(data->metkarr == 0) //По дате записи заказа
 {
  struct  tm      bf;
  
  bf.tm_mday=(int)dn;
  bf.tm_mon=(int)(mn-1);
  bf.tm_year=(int)(gn-1900);

  bf.tm_hour=(int)hasn;
  bf.tm_min=(int)minn;
  bf.tm_sec=sekn;

  time_t vremn=mktime(&bf);

  bf.tm_mday=(int)dk;
  bf.tm_mon=(int)(mk-1);
  bf.tm_year=(int)(gk-1900);

  bf.tm_hour=(int)hask;
  bf.tm_min=(int)mink;
  bf.tm_sec=sekk;

  time_t vremk=mktime(&bf);

  sprintf(strsql,"select kz,kv,suma,ktoi,kodk from Taxizak where \
vrem >= %ld and vrem <= %ld",vremn,vremk);

 }
 
printf("taxi_oth_r-%s\n",strsql);

if((data->kolstr=data->cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

data->window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data->window),ICEB_POS_CENTER);

sprintf(strsql,"%s %s",NAME_SYSTEM,iceb_u_toutf("Расчет отчетов."));
gtk_window_set_title(GTK_WINDOW(data->window),strsql);

//gtk_signal_connect(GTK_OBJECT(data->window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data->window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data->window), vbox);

GtkWidget *label=gtk_label_new(gettext("Ждите !!!"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

label=gtk_label_new(iceb_u_toutf("Расчет отчетов по водителям и операторам.."));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);


//GtkObject *adjustment=gtk_adjustment_new(0.,1.,data->kolstr,0.,0.,0.);
//data->bar=gtk_progress_bar_new_with_adjustment(GTK_ADJUSTMENT(adjustment));
data->bar=gtk_progress_bar_new();

gtk_progress_bar_set_bar_style(GTK_PROGRESS_BAR(data->bar),GTK_PROGRESS_CONTINUOUS);

gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(data->bar),GTK_PROGRESS_LEFT_TO_RIGHT);

gtk_box_pack_start (GTK_BOX (vbox), data->bar, FALSE, FALSE, 2);

gtk_idle_add((GtkFunction)taxi_oth_r1,data);
gtk_widget_show_all(data->window);

gtk_main();

return(0);

}
/*********************************/
/*Расчет форм                    */
/*********************************/


gint  taxi_oth_r1(class taxi_oth_data *data)
{
SQL_str row;
int kodop=0;
int kodzav=0;
int kolvod=0;
char kodkl[30];        
int kolklient=0;

printf("Создаём массивы.\n");
//Определяем количество операторов, водителей и кодов завершения
while(data->cur.read_cursor(&row) != 0)
 {
 //    printf("%s %s\n",row[0],row[1]);
  if(iceb_u_proverka(data->kodop.ravno(),row[3],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->kodvod.ravno(),row[1],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->kodzav.ravno(),row[0],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->kodklienta.ravno(),row[4],0,0) != 0)
    continue;
    
  kodzav=atoi(row[0]);
  kodop=atoi(row[3]);
  
  if(data->vod.find(row[1]) == -1)
     data->vod.plus(row[1]);
  if(data->zav.find(kodzav) == -1)
    data->zav.plus(kodzav,-1);
  if(data->op.find(kodop) == -1)
    data->op.plus(kodop,-1);

  memset(kodkl,'\0',sizeof(kodkl));
  if(row[4][0] == '\0')
    strcpy(kodkl,"000");
  else
    strncpy(kodkl,row[4],sizeof(kodkl)-1);

 //    printf("kolkl=%s\n",kodkl);
  
  if(data->klient.find(kodkl) == -1)
    data->klient.plus(kodkl);
 }

sort_vod(&data->vod,data->window);

data->cur.poz_cursor(0);

data->vod_zav.make_class(data->zav.kolih()*data->vod.kolih());
data->op_zav.make_class(data->zav.kolih()*data->op.kolih());

kolvod=data->vod.kolih();
data->vod_bp.make_class(kolvod);
data->vod_pp.make_class(kolvod);
data->vod_sum_pp.make_class(kolvod);
data->vod_sum_bp.make_class(kolvod);

kolklient=data->klient.kolih();
printf("Количество клиентов=%d\n",kolklient);
data->kli_kp_o.make_class(kolklient);
data->kli_kp_b.make_class(kolklient);
data->sum_op.make_class(kolklient);
data->sum_bp.make_class(kolklient);

sprintf(data->imafprot,"vodprot%d.lst",getpid());

if((data->ffprot = fopen(data->imafprot,"w")) == NULL)
 {
  iceb_er_op_fil(data->imafprot,"",errno,data->window);
  return(FALSE);
 }
iceb_u_startfil(data->ffprot);

//printf("Просматриваем записи.\n");

int nomer_vod=0;
int nomer_kli=0;
int nomer_zav=0;
int nomer_op=0;
int nomer=0;
int kolzav=data->zav.kolih();
double suma;
while(data->cur.read_cursor(&row) != 0)
 {

  iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

  if(iceb_u_proverka(data->kodop.ravno(),row[3],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->kodvod.ravno(),row[1],0,0) != 0)
    continue;
   if(iceb_u_proverka(data->kodzav.ravno(),row[0],0,0) != 0)
      continue;

  if(iceb_u_proverka(data->kodklienta.ravno(),row[4],0,0) != 0)
    continue;
  
  kodzav=atoi(row[0]);
  kodop=atoi(row[3]);
  
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  fprintf(data->ffprot,"%s %s %s %s\n",row[0],row[1],row[2],row[3]);
    
  nomer_op=data->op.find(kodop);
  nomer_zav=data->zav.find(kodzav);
  nomer_vod=data->vod.find(row[1]);

  memset(kodkl,'\0',sizeof(kodkl));
  if(row[4][0] == '\0')
    strcpy(kodkl,"000");
  else
    strncpy(kodkl,row[4],sizeof(kodkl)-1);

  nomer_kli=data->klient.find(kodkl);

  fprintf(data->ffprot,"nomer_op=%d nomer_zav=%d nomer_vod=%d\n",
  nomer_op,nomer_zav, nomer_vod);

  nomer=(nomer_vod*kolzav)+nomer_zav;
  data->vod_zav.plus(1,nomer);

  fprintf(data->ffprot,"nomer=%d\n",nomer);
    
  nomer=(nomer_op*kolzav)+nomer_zav;
  data->op_zav.plus(1,nomer);
  fprintf(data->ffprot,"nomer=%d\n",nomer);
  suma=atof(row[2]);

  if(kodzav == 1 && row[4][0] != '\0')
   {   
    if(suma < 0.)
     {
      data->vod_bp.plus(1,nomer_vod);
      data->vod_sum_bp.plus(suma,nomer_vod);
      if(nomer_kli >= 0)
       {
        data->kli_kp_b.plus(1,nomer_kli);
        data->sum_bp.plus(suma,nomer_kli);
       }       
     }
    else
     {
      data->vod_pp.plus(1,nomer_vod);
      data->vod_sum_pp.plus(suma,nomer_vod);
      if(nomer_kli >= 0)
       {
        data->kli_kp_o.plus(1,nomer_kli);
        data->sum_op.plus(suma,nomer_kli);
       }
     }
    
   }  
  
 }

//printf("Распечатываем.\n");

time_t vrem;
struct tm *bf;
time(&vrem);
bf=localtime(&vrem);

kolvod=data->vod.kolih();
char naim[100];
char kodvod[20];
char gosnomer[20];
char strsql[300];
short dlinna=37+(4+1)*(kolzav+1)+1;
char stroka[dlinna];
int kolz=0;
int itogo=0;
FILE *ff;

short hasn,minn,sekn;
short hask,mink,sekk;

iceb_u_rstime(&hasn,&minn,&sekn,data->vremn.ravno());
if(data->vremk.getdlinna() <= 1)
 {
  hask=24; mink=0; sekk=0;
 }
else
  iceb_u_rstime(&hask,&mink,&sekk,data->vremk.ravno());

//распечатка по водителям

char imaf[40];

sprintf(imaf,"vod%d.lst",getpid());
printf("Имя файла:%s\n",imaf);

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return(FALSE);
 }

iceb_u_startfil(ff);
fprintf(ff,"\x1B\x4D"); /*12-знаков*/

fprintf(ff,"\nОтчет по водителям:\n");


fprintf(ff,"%s %s%s %02d:%02d:%02d %s %s%s %02d:%02d:%02d\n",
gettext("Период с"),
data->datn.ravno(),
gettext("г."),
hasn,minn,sekn,
gettext("по"),
data->datk.ravno(),
gettext("г."),
hask,mink,sekk);

fprintf(ff,"\
%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);
SQLCURSOR cur;
if(data->metkarr == 0)
  fprintf(ff,"Отчёт по времени записи.\n");
if(data->metkarr == 1)
  fprintf(ff,"Отчёт по времени заказа.\n");
  

//Распечатываем список кодов завершения
fprintf(ff,"\nСписок кодов завершения:\n");
for(nomer_zav=0; nomer_zav < kolzav; nomer_zav++)
 {
  kodzav=data->zav.ravno(nomer_zav);
  if(kodzav == 0)
   continue;
   
  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naik from Taxikzz where kod=%d",kodzav);
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   strncpy(naim,row[0],sizeof(naim)-1);  
  fprintf(ff,"%d %s\n",kodzav,naim);
 }
fprintf(ff,"\n");

if(data->kodvod.getdlinna() > 1)
 fprintf(ff,"Код водителя:%s\n",data->kodvod.ravno());
if(data->kodop.getdlinna() > 1)
 fprintf(ff,"Код оператора:%s\n",data->kodop.ravno());
if(data->kodzav.getdlinna() > 1)
 fprintf(ff,"Код завершения:%s\n",data->kodzav.ravno());

if(data->kodklienta.getdlinna() > 1)
 fprintf(ff,"Код клиента:%s\n",data->kodklienta.ravno());
 
memset(stroka,'-',dlinna);
stroka[dlinna-1]='\0';

fprintf(ff,"%s\n",stroka);

fprintf(ff,"\
Код |    Фамилия         | Номер    |");

for(nomer_zav=0; nomer_zav < kolzav; nomer_zav++)
  fprintf(ff,"%4d|",data->zav.ravno(nomer_zav));
fprintf(ff,"Итог|\n");

fprintf(ff,"%s\n",stroka);

for(nomer_vod=0 ; nomer_vod < kolvod; nomer_vod++)
 {
  strncpy(kodvod,data->vod.ravno(nomer_vod),sizeof(kodvod)-1);

  memset(naim,'\0',sizeof(naim));
  memset(gosnomer,'\0',sizeof(gosnomer));

  sprintf(strsql,"select fio,gosn from Taxivod where kod='%s'",kodvod);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    strncpy(naim,row[0],sizeof(naim)-1);    
    strncpy(gosnomer,row[1],sizeof(gosnomer)-1);    

   }
  else
    strncpy(naim,"Без водителя",sizeof(naim)-1);    
  
//  printf("%4s %-20.20s %-10.10s\n",kodvod,naim,gosnomer);

  fprintf(ff,"%4s %-20.20s %-10.10s",
  kodvod,naim,gosnomer);
  itogo=0;
  kolz=0;
  for(nomer_zav=0; nomer_zav < kolzav; nomer_zav++)
   {
    kolz=data->vod_zav.ravno((nomer_vod*kolzav)+nomer_zav);
    itogo+=kolz;
    fprintf(ff," %4d",kolz);
   }
  fprintf(ff," %4d\n",itogo);

 }

fprintf(ff,"%s\n",stroka);
fprintf(ff,"%36s","Итого");
itogo=0;
int itogozav=0;
for(nomer_zav=0; nomer_zav < kolzav; nomer_zav++)
 {
  itogozav=0;
  for(nomer_vod=0 ; nomer_vod < kolvod; nomer_vod++)
    itogozav+=data->vod_zav.ravno((nomer_vod*kolzav)+nomer_zav);
  fprintf(ff," %4d",itogozav);
  itogo+=itogozav;  
 }
fprintf(ff," %4d\n",itogo);
fprintf(ff,"%s\n",stroka);

iceb_podpis(ff,data->window);
fprintf(ff,"\x1B\x50"); /*10-знаков*/
fclose(ff);

char imafvp[30];
sprintf(imafvp,"vodvp%d.lst",getpid());
printf("Имя файла:%s\n",imafvp);

if((ff = fopen(imafvp,"w")) == NULL)
 {
  iceb_er_op_fil(imafvp,"",errno,data->window);
  return(FALSE);
 }

iceb_u_startfil(ff);
fprintf(ff,"\x1B\x4D"); /*12-знаков*/

fprintf(ff,"\nОтчёт по выполненым заказам:\n");
fprintf(ff,"%s %s%s %02d:%02d:%02d %s %s%s %02d:%02d:%02d\n",
gettext("Период с"),
data->datn.ravno(),
gettext("г."),
hasn,minn,sekn,
gettext("по"),
data->datk.ravno(),
gettext("г."),
hask,mink,sekk);

fprintf(ff,"\
%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);
if(data->metkarr == 0)
  fprintf(ff,"Отчёт по времени записи.\n");
if(data->metkarr == 1)
  fprintf(ff,"Отчёт по времени заказа.\n");

if(data->kodvod.getdlinna() > 1)
 fprintf(ff,"Код водителя:%s\n",data->kodvod.ravno());
if(data->kodop.getdlinna() > 1)
 fprintf(ff,"Код оператора:%s\n",data->kodop.ravno());
if(data->kodzav.getdlinna() > 1)
 fprintf(ff,"Код завершения:%s\n",data->kodzav.ravno());

if(data->kodklienta.getdlinna() > 1)
 fprintf(ff,"Код клиента:%s\n",data->kodklienta.ravno());

fprintf(ff,"\
-------------------------------------------------------------------------------------------\n\
Код |    Фамилия         | Номер    |Количество| Сумма    |Количество|   Сумма  |К оплате\n\
    |                    |          |платных   | платных  |бесплатных|бесплатных|\n\
-------------------------------------------------------------------------------------------\n");
itogo=0;
int kolz1;
double sum,sum1;
int itogo1=0;
double itsum=0.;
double itsum1=0.;
double k_oplate;
double itogo_k_oplate=0.;

for(nomer_vod=0 ; nomer_vod < kolvod; nomer_vod++)
 {
  strncpy(kodvod,data->vod.ravno(nomer_vod),sizeof(kodvod)-1);
  if(kodvod[0] == '\0')
   continue;
  memset(naim,'\0',sizeof(naim));
  memset(gosnomer,'\0',sizeof(gosnomer));

  sprintf(strsql,"select fio,gosn from Taxivod where kod='%s'",kodvod);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    strncpy(naim,row[0],sizeof(naim)-1);    
    strncpy(gosnomer,row[1],sizeof(gosnomer)-1);    

   }
  else
    strncpy(naim,"Без водителя",sizeof(naim)-1);    

  kolz=data->vod_pp.ravno(nomer_vod);
  kolz1=data->vod_bp.ravno(nomer_vod);
  sum=data->vod_sum_pp.ravno(nomer_vod);
  sum1=data->vod_sum_bp.ravno(nomer_vod);
  itogo+=kolz;  
  itogo1+=kolz1;  
  itsum+=sum;
  itsum1+=sum1;
//  k_oplate=sum+sum1+kolz1;  
  k_oplate=sum+sum1;  
  itogo_k_oplate+=k_oplate;
  fprintf(ff,"%4s %-20.20s %-10.10s %10d %10.2f %10d %10.2f %10.2f\n",
  kodvod,naim,gosnomer,kolz,sum,kolz1,sum1,k_oplate);

 }
fprintf(ff,"\
-------------------------------------------------------------------------------------------\n");
fprintf(ff,"%36s %10d %10.2f %10d %10.2f %10.2f\n","Итого",itogo,itsum,itogo1,itsum1,itogo_k_oplate);
 
iceb_podpis(ff,data->window);

fprintf(ff,"\x1B\x50"); /*10-знаков*/

fclose(ff);

char imafkl[30];
sprintf(imafkl,"vodkl%d.lst",getpid());
printf("Имя файла:%s\n",imafkl);

if((ff = fopen(imafkl,"w")) == NULL)
 {
  iceb_er_op_fil(imafkl,"",errno,data->window);
  return(FALSE);
 }

iceb_u_startfil(ff);
fprintf(ff,"\x1B\x4D"); /*12-знаков*/

fprintf(ff,"\nОтчёт по клиентам:\n");
fprintf(ff,"%s %s%s %02d:%02d:%02d %s %s%s %02d:%02d:%02d\n",
gettext("Период с"),
data->datn.ravno(),
gettext("г."),
hasn,minn,sekn,
gettext("по"),
data->datk.ravno(),
gettext("г."),
hask,mink,sekk);

fprintf(ff,"\
%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

if(data->metkarr == 0)
  fprintf(ff,"Отчёт по времени записи.\n");
if(data->metkarr == 1)
  fprintf(ff,"Отчёт по времени заказа.\n");

if(data->kodvod.getdlinna() > 1)
 fprintf(ff,"Код водителя:%s\n",data->kodvod.ravno());
if(data->kodop.getdlinna() > 1)
 fprintf(ff,"Код оператора:%s\n",data->kodop.ravno());
if(data->kodzav.getdlinna() > 1)
 fprintf(ff,"Код завершения:%s\n",data->kodzav.ravno());

if(data->kodklienta.getdlinna() > 1)
 fprintf(ff,"Код клиента:%s\n",data->kodklienta.ravno());

fprintf(ff,"\
----------------------------------------------------------------------------\n\
   Код    |    Фамилия         |Количество| Сумма    |Количество|   Сумма  |\n\
          |                    |платных   | платных  |бесплатных|бесплатных|\n\
----------------------------------------------------------------------------\n");
itogo=0;
itogo1=0;
itsum=0.;
itsum1=0.;
kolklient=data->klient.kolih();
for(nomer_vod=0 ; nomer_vod < kolklient; nomer_vod++)
 {
  strncpy(kodvod,data->klient.ravno(nomer_vod),sizeof(kodvod)-1);
  if(kodvod[0] == '\0')
   continue;
  memset(naim,'\0',sizeof(naim));

  sprintf(strsql,"select fio from Taxiklient where kod='%s'",kodvod);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    strncpy(naim,row[0],sizeof(naim)-1);    
   }
  else
    strncpy(naim,"Не клиент",sizeof(naim)-1);    

  kolz=data->kli_kp_o.ravno(nomer_vod);
  kolz1=data->kli_kp_b.ravno(nomer_vod);
  sum=data->sum_op.ravno(nomer_vod);
  sum1=data->sum_bp.ravno(nomer_vod);
  itogo+=kolz;  
  itogo1+=kolz1;  
  itsum+=sum;
  itsum1+=sum1;
  
  fprintf(ff,"%10s %-20.20s %10d %10.2f %10d %10.2f\n",
  kodvod,naim,kolz,sum,kolz1,sum1);

 }
fprintf(ff,"\
----------------------------------------------------------------------------\n");
fprintf(ff,"%31s %10d %10.2f %10d %10.2f\n","Итого",itogo,itsum,itogo1,itsum1);
 
iceb_podpis(ff,data->window);

fprintf(ff,"\x1B\x50"); /*10-знаков*/

fclose(ff);



//распечатка по операторам

char imafop[40];

sprintf(imafop,"oper%d.lst",getpid());

if((ff = fopen(imafop,"w")) == NULL)
 {
  iceb_er_op_fil(imafop,"",errno,data->window);
  return(FALSE);
 }
iceb_u_startfil(ff);
fprintf(ff,"\x1B\x4D"); /*12-знаков*/
fprintf(ff,"\nОтчет по операторам:\n");


fprintf(ff,"%s %s%s %02d:%02d:%02d %s %s%s %02d:%02d:%02d\n",
gettext("Период с"),
data->datn.ravno(),
gettext("г."),
hasn,minn,sekn,
gettext("до"),
data->datk.ravno(),
gettext("г."),
hask,mink,sekk);

fprintf(ff,"\
%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

if(data->metkarr == 0)
  fprintf(ff,"Отчёт по времени записи.\n");
if(data->metkarr == 1)
  fprintf(ff,"Отчёт по времени заказа.\n");

//Распечатываем список кодов завершения
fprintf(ff,"\nСписок кодов завершения:\n");
for(nomer_zav=0; nomer_zav < kolzav; nomer_zav++)
 {
  kodzav=data->zav.ravno(nomer_zav);
  if(kodzav == 0)
   continue;
   
  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naik from Taxikzz where kod=%d",kodzav);
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   strncpy(naim,row[0],sizeof(naim)-1);  
  fprintf(ff,"%d %s\n",kodzav,naim);
 }
fprintf(ff,"\n");

if(data->kodvod.getdlinna() > 1)
 fprintf(ff,"Код водителя:%s\n",data->kodvod.ravno());
if(data->kodop.getdlinna() > 1)
 fprintf(ff,"Код оператора:%s\n",data->kodop.ravno());
if(data->kodzav.getdlinna() > 1)
 fprintf(ff,"Код завершения:%s\n",data->kodzav.ravno());
if(data->kodklienta.getdlinna() > 1)
 fprintf(ff,"Код клиента:%s\n",data->kodklienta.ravno());

dlinna=26+(4+1)*(kolzav+1)+1;
memset(stroka,'-',dlinna);
stroka[dlinna-1]='\0';

fprintf(ff,"%s\n",stroka);

fprintf(ff,"\
Код |    Фамилия         |");

for(nomer_zav=0; nomer_zav < kolzav; nomer_zav++)
  fprintf(ff,"%4d|",data->zav.ravno(nomer_zav));
fprintf(ff,"Итог|\n");

fprintf(ff,"%s\n",stroka);

struct  passwd  *ktoz; /*Кто записал*/
kolvod=data->op.kolih();
for(nomer_vod=0 ; nomer_vod < kolvod; nomer_vod++)
 {
  kodop=data->op.ravno(nomer_vod);
  memset(naim,'\0',sizeof(naim));

  if((ktoz=getpwuid(kodop)) != NULL)
    strncpy(naim,ktoz->pw_gecos,sizeof(naim)-1);
  else
    strncpy(naim,gettext("Неизвестный логин"),sizeof(naim)-1);


  
//  printf("%4d %-20.20s\n",kodop,naim);

  fprintf(ff,"%4d %-20.20s",
  kodop,naim);

  itogo=0;
  kolz=0;
  for(nomer_zav=0; nomer_zav < kolzav; nomer_zav++)
   {
    kolz=data->op_zav.ravno((nomer_vod*kolzav)+nomer_zav);
    itogo+=kolz;
    fprintf(ff," %4d",kolz);
   }
  fprintf(ff," %4d\n",itogo);

 }

fprintf(ff,"%s\n",stroka);
fprintf(ff,"%25s","Итого");
itogo=0;
itogozav=0;
for(nomer_zav=0; nomer_zav < kolzav; nomer_zav++)
 {
  itogozav=0;
  for(nomer_vod=0 ; nomer_vod < kolvod; nomer_vod++)
    itogozav+=data->op_zav.ravno((nomer_vod*kolzav)+nomer_zav);
  fprintf(ff," %4d",itogozav);
  itogo+=itogozav;  
 }
fprintf(ff," %4d\n",itogo);
fprintf(ff,"%s\n",stroka);
iceb_podpis(ff,data->window);


fprintf(ff,"\x1B\x50"); /*10-знаков*/
fclose(ff);


fclose(data->ffprot);

data->imafil.plus(imaf);
data->imafil.plus(imafvp);
data->imafil.plus(imafkl);
data->imafil.plus(imafop);
data->imafil.plus(data->imafprot);
data->nameoth.plus("Отчет по водителям.");
data->nameoth.plus("Отчет по водителям по выполненным заказам.");
data->nameoth.plus("Отчет по клиентам.");
data->nameoth.plus("Отчет по операторам.");
data->nameoth.plus("Протокол работы программы.");

gtk_widget_destroy(data->window);
return(FALSE);


}
/********************************/
/*Сортировка списка водителей по номерам автомобилей*/
/**********************************/

void sort_vod(iceb_u_spisok *vod,GtkWidget *wpredok)
{
FILE *ff;
char imaf[30];

sprintf(imaf,"sortvod%d.tmp",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
char strsql[300];
SQL_str row;
SQLCURSOR cur;
int kolvod=vod->kolih();
for(int i=0; i < kolvod; i++)
 {
  sprintf(strsql,"select gosn from Taxivod where kod='%s'",vod->ravno(i));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1) 
     fprintf(ff,"%s|%s|\n",row[0],vod->ravno(i));
  else
     fprintf(ff,"%s|%s|\n","**",vod->ravno(i));
 }
fclose(ff);

//сортируем файл по номерам

sprintf(strsql,"sort -o %s -t\\|  +0 -1 %s",imaf,imaf);
system(strsql);

if((ff = fopen(imaf,"r")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
char stroka[500];
vod->free_class();
while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  iceb_u_polen(stroka,strsql,sizeof(strsql),2,'|');
//  printf("%s / %s\n",stroka,strsql);
  vod->plus(strsql);  
 }
fclose(ff);
unlink(imaf); 

}
