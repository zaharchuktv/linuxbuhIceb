/*$Id: zar_alim_r.c,v 1.14 2011-02-21 07:35:59 sasa Exp $*/
/*19.11.2009	08.12.2006	Белых А.И.	zar_alim_r.c
Расчёт реестра почтовых переводов алиментов
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"

class zar_alim_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  const char *data_r;
  class iceb_u_spisok *imaf;
  class iceb_u_spisok *naimf;
    
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_alim_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_alim_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_alim_r_data *data);
gint zar_alim_r1(class zar_alim_r_data *data);
void  zar_alim_r_v_knopka(GtkWidget *widget,class zar_alim_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern int kol_strok_na_liste;


int zar_alim_r(const char *data_r,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_alim_r_data data;

data.data_r=data_r;
data.imaf=imaf;
data.naimf=naimf;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать реестр почтовых переводов алиментов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_alim_r_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

repl.plus(gettext("Распечатать реестр почтовых переводов алиментов"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),imabaz,organ);
repl.ps_plus(strsql);

GtkWidget *label=gtk_label_new(repl.ravno_toutf());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),450,300);

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста
//PangoFontDescription *font_pango=pango_font_description_from_string("Nimbus Mono L, Bold 12");
PangoFontDescription *font_pango=pango_font_description_from_string(shrift_ravnohir.ravno());
gtk_widget_modify_font(GTK_WIDGET(data.view),font_pango);
pango_font_description_free(font_pango);

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
//gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_ALWAYS,GTK_POLICY_ALWAYS);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);


data.bar=gtk_progress_bar_new();

gtk_progress_bar_set_bar_style(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_CONTINUOUS);

gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_LEFT_TO_RIGHT);

gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
GtkTooltips *tooltops=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltops,data.knopka,gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_alim_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_alim_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_alim_r_v_knopka(GtkWidget *widget,class zar_alim_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_alim_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_alim_r_data *data)
{
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}
/*************************************/
/*Шапка*/
/**************************************/
void reestal_sap(int *kolstr,FILE *ff)
{

*kolstr+=5;

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n\
 N |          Куди                |   Кому (найменування    |  Сумма  |Плата за|      |Відмітка|        |        |\n\
   |  (повна адреса одержувача)   |   підприємства або      |переказу|пере-ння| ПДВ  |   про  |   N    |Примітка|\n\
   |                              |   прізвище одержувача)  | (грн)  |без ПДВ |(грн) |  інші  |переказу|        |\n\
   |                              |                         |        | (грн)  |      |послуги |        |        |\n\
-----------------------------------------------------------------------------------------------------------------\n");
}

/*******/
/*Шапка*/
/*******/
void            restsp(int *kolstrok,FILE *ff,GtkWidget *wpredok)
{
*kolstrok=0;

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"zar_pp_start.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return;
 }

SQL_str row;
SQLCURSOR cur;
char kod[54];
char naim_org[512];
char adres_org[512];
memset(naim_org,'\0',sizeof(naim_org));
memset(kod,'\0',sizeof(kod));
memset(adres_org,'\0',sizeof(adres_org));
char stroka[1024];

sprintf(stroka,"select naikon,adres,kod from Kontragent where kodkon='00'");
if(iceb_sql_readkey(stroka,&row,&cur,wpredok) == 1)
 {
  strncpy(naim_org,row[0],sizeof(naim_org)-1);
  strncpy(adres_org,row[1],sizeof(adres_org)-1);
  strncpy(kod,row[2],sizeof(kod)-1);
 }

memset(stroka,'\0',sizeof(stroka));
int nomer_str=0;
char stroka1[1024];
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomer_str++;
  memset(stroka,'\0',sizeof(stroka));
  strncpy(stroka,row_alx[0],sizeof(stroka)-1);
  switch(nomer_str)
   {
    case 1:
      sprintf(stroka1,"%s %s",naim_org,adres_org);
      iceb_u_bstab(stroka,naim_org,0,67,1);
      break;

    case 4:
      iceb_u_bstab(stroka,kod,21,43,2);
      break;

    case 10:
      sprintf(stroka1,"%s %s",naim_org,adres_org);
      iceb_u_bstab(stroka,stroka1,11,120,1);
      break;
   }
  *kolstrok+=1;   
  fprintf(ff,"%s",stroka);
 }

reestal_sap(kolstrok,ff);

}

/*******/
/*Конец*/
/*******/
void konre(double razom,double pdv,double vsego,double p_zbir,
FILE *ff,GtkWidget *wpredok)
{

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%7.2f %8.2f %6.2f\n",
iceb_u_kolbait(60,gettext("Итого")),gettext("Итого"),razom,p_zbir,pdv);

char stroka[1024];
char stroka1[1024];

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"zar_pp_end.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return;
 }

SQL_str row;
SQLCURSOR cur;

char fio_ruk[112];
memset(fio_ruk,'\0',sizeof(fio_ruk));

iceb_poldan("Табельный номер руководителя",stroka1,"zarnast.alx",wpredok);

if(stroka1[0] != '\0')
 {
  sprintf(stroka,"select fio from Kartb where tabn=%s",stroka1);
  if(iceb_sql_readkey(stroka,&row,&cur,wpredok) == 1)
    strncpy(fio_ruk,row[0],sizeof(fio_ruk)-1);
 } 

char fio_buh[112];
memset(fio_buh,'\0',sizeof(fio_buh));

iceb_poldan("Табельный номер бухгалтера",stroka1,"zarnast.alx",wpredok);

if(stroka1[0] != '\0')
 {
  sprintf(stroka,"select fio from Kartb where tabn=%s",stroka1);
  if(iceb_sql_readkey(stroka,&row,&cur,wpredok) == 1)
    strncpy(fio_buh,row[0],sizeof(fio_buh)-1);
 } 


memset(stroka,'\0',sizeof(stroka));
int nomer_str=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomer_str++;
  memset(stroka,'\0',sizeof(stroka));
  strncpy(stroka,row_alx[0],sizeof(stroka)-1);
  switch(nomer_str)
   {
    case 1:
      memset(stroka1,'\0',sizeof(stroka1));
      iceb_u_preobr(razom,stroka1,0);
      iceb_u_bstab(stroka,stroka1,25,78,1);
      break;

    case 2:
      memset(stroka1,'\0',sizeof(stroka1));
      iceb_u_preobr(pdv,stroka1,0);
      iceb_u_bstab(stroka,stroka1,25,78,1);
      break;

    case 3:
      memset(stroka1,'\0',sizeof(stroka1));
      iceb_u_preobr(vsego,stroka1,0);
      iceb_u_bstab(stroka,stroka1,25,86,1);
      break;

    case 7:
      iceb_u_bstab(stroka,fio_ruk,20,86,1);
      break;

    case 10:
      iceb_u_bstab(stroka,fio_buh,20,86,1);
      break;
   }
  fprintf(ff,"%s",stroka);
 }


}
/*************************************/
/*счетчик строк*/
void reestal_ss(int *kolstrok,FILE *ff)
{
*kolstrok+=1;
if(*kolstrok <= kol_strok_na_liste)
 return;
fprintf(ff,"\f");
*kolstrok=0; 

reestal_sap(kolstrok,ff);
*kolstrok+=1;
}

/******************************************/
/******************************************/

gint zar_alim_r1(class zar_alim_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);


short mr,gr;
iceb_u_rsdat1(&mr,&gr,data->data_r);





double nds1=iceb_pnds(data->window);

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
const char *imaf_alx={"zaralim.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,data->window);
  return(1);
 }


char imaf[56];
sprintf(imaf,"reestr%d.lst",mr);
FILE *f1;
if((f1 = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf1[54];

sprintf(imaf1,"vua%d.lst",mr);
FILE *f2;
if((f2 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

fprintf(f2,"%s\n\n%s\n",
gettext("Ведомость удержания алиментов"),organ);

int kolstrok=0;
restsp(&kolstrok,f1,data->window);

double prsb=0.;
int tabn1=0;
double sal=0,sps=0,osym=0,spr=0,ssb=0,proc=0.;
int tabn=0;
int pnom=0;
char mesqc[40];

iceb_mesc(mr,0,mesqc);
double razom=0.,pdv=0.,vsego=0.;
double nds=0.;
double sps_b_nds=0.; //Сумма почтового сбора без НДС
double isps_b_nds=0.; //Итоговая сумма почтового сбора без НДС
double sumps=0;
double sumalim=0;
char br[512];
int kodalim=0;
int kodps=0;
SQL_str row;
class SQLCURSOR curr;
int mpr=0;
char fam[512];
char adr[512];
char famr[512];
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  proc=prsb=sumps=sumalim=0.;

  if(iceb_u_SRAV(row_alx[0],"***",1) == 0)  //Метка нового документа
   {
    konre(razom,pdv,vsego,isps_b_nds,f1,data->window);
    pnom=0;
    osym=spr=ssb=proc=0.;
    razom=pdv=vsego=0.;
    isps_b_nds=0.;

    fprintf(f1,"\f");
    restsp(&kolstrok,f1,data->window);

    continue;
   }

  memset(br,'\0',sizeof(br));
  if(iceb_u_polen(row_alx[0],br,sizeof(br),1,'|') != 0)
    continue;

  if(iceb_u_SRAV("Код удержания алиментов",br,0) == 0)
   {
    iceb_u_polen(row_alx[0],br,sizeof(br),2,'|');
    kodalim=(short)atoi(br);
    continue;
   }

  if(iceb_u_SRAV("Код удержания почтового сбора",br,0) == 0)
   {
    iceb_u_polen(row_alx[0],br,sizeof(br),2,'|');
    kodps=(short)atoi(br);
    continue;
   }

  tabn=atol(br);
  if(tabn != 0)
   {
    //Проверяем табельный номер
    sprintf(strsql,"select tabn from Kartb where tabn=%d",tabn);
    if(iceb_sql_readkey(strsql,&row,&curr,data->window) != 1)
     {
      sprintf(strsql,"%s %d !\n",gettext("Не найден табельный номер"),tabn);
      iceb_menu_soob(strsql,data->window);
      continue;
     }     
   }
  if(tabn != 0 && tabn != tabn1)
   {
    tabn1=tabn;
   }

  if(tabn != 0)
    mpr=0;

  memset(br,'\0',sizeof(br));
  iceb_u_polen(row_alx[0],br,sizeof(br),2,'|');
  proc=iceb_u_atof(br);

  memset(br,'\0',sizeof(br));
  iceb_u_polen(row_alx[0],br,sizeof(br),3,'|');
  prsb=iceb_u_atof(br);

  /*Получает на месте*/
  if(tabn != 0 && prsb == 0.)
    mpr=1;

  if(mpr == 1)
    continue;
  memset(fam,'\0',sizeof(fam));
  iceb_u_polen(row_alx[0],fam,sizeof(fam),4,'|');

  memset(adr,'\0',sizeof(adr));
  iceb_u_polen(row_alx[0],adr,sizeof(adr),5,'|');
  if(tabn != 0)
   {
    
    if(kodalim == 0)
     {
      iceb_menu_soob(gettext("В настоечном файле не найден код алиментов !"),data->window);
      break;      
     }
    sprintf(strsql,"select fio from Kartb where tabn=%d",tabn);
    if(iceb_sql_readkey(strsql,&row,&curr,data->window) != 1)
     {
      sprintf(strsql,"%s %d !\n",gettext("Не найден табельный номер"),tabn);
      iceb_menu_soob(strsql,data->window);
       continue;
     }
    
    strncpy(famr,row[0],sizeof(famr)-1);
    
    sprintf(strsql,"%5d %-*.*s\n",
    tabn,
    iceb_u_kolbait(20,fam),iceb_u_kolbait(20,fam),fam);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    fprintf(f2,"\
----------------------------------------------------------------\n\
%5d %s -> %s  \n",tabn,famr,fam);

    
    /*Читаем алименты*/
    sumalim=0.;
    sprintf(strsql,"select SUM(suma) from Zarp where datz >= '%d-%d-01' \
and datz <= '%d-%d-31' and tabn=%d and prn='2' \
and knah=%d",gr,mr,gr,mr,tabn,kodalim); 
    if(iceb_sql_readkey(strsql,&row,&curr,data->window) >= 1)
      if(row[0] != NULL)
       sumalim=atof(row[0])*-1;      

    if(sumalim == 0.)
     {
      tabn=tabn1=0;
      continue;
     }      

    sumps=0.;
    sps_b_nds=0.;
    /*Читаем почтовый сбор*/
    sprintf(strsql,"select SUM(suma) from Zarp where datz >= '%d-%d-01' \
and datz <= '%d-%d-31' and tabn=%d and prn='2'\
and knah=%d",gr,mr,gr,mr,tabn,kodps); 
    if(iceb_sql_readkey(strsql,&row,&curr,data->window) >= 1)
     {
      if(row[0] != NULL)
       {
        sumps=atof(row[0])*-1;      
        if(sumps != 0.)
         {
          sps_b_nds=sumps-sumps*nds1/(100.+nds1);
          sps_b_nds=iceb_u_okrug(sps_b_nds,0.01);
         }
       }
     }
//    if(sumps == 0.)
//     continue;
     

    nds=sumps-sps_b_nds;
    nds=iceb_u_okrug(nds,0.01);

    pdv+=nds;
    razom+=sumalim;
    vsego+=sumalim+sumps;
    
    reestal_ss(&kolstrok,f1);

    fprintf(f1,"%3d %-*.*s %-*.*s %7.2f %8.2f %6.2f\n",
    ++pnom,
    iceb_u_kolbait(30,adr),iceb_u_kolbait(30,adr),adr,
    iceb_u_kolbait(25,fam),iceb_u_kolbait(25,fam),fam,
    sumalim,sps_b_nds,nds);

    fprintf(f2,"\n\
%*s:%20s (%.2f%%)\n",
    iceb_u_kolbait(30,gettext("Алименты")),gettext("Алименты"),
    iceb_u_prnbr(sumalim),proc);

    fprintf(f2,"\
%*s:%20s\n",
    iceb_u_kolbait(30,gettext("Почтовый сбор")),gettext("Почтовый сбор"),
    iceb_u_prnbr(sumps));

    osym+=sumalim;
    spr+=sumalim;
    sps+=sumps;
    ssb+=sumps;
    sal+=sumalim;
    isps_b_nds+=sps_b_nds;
   }
  else
   if(tabn1 != 0)
    {
     reestal_ss(&kolstrok,f1);
     fprintf(f1,"%3s %-*.*s %-*.*s\n",
     " ",
     iceb_u_kolbait(30,adr),iceb_u_kolbait(30,adr),adr,
     iceb_u_kolbait(25,fam),iceb_u_kolbait(25,fam),fam);
    }
 }
sprintf(strsql,"\n-------------------------------------------------------\n\
%-*s:%20s\n",
iceb_u_kolbait(20,gettext("Итого алиментов")),gettext("Итого алиментов"),
iceb_u_prnbr(sal));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%-*s:%20s\n\n",
iceb_u_kolbait(20,gettext("Итого почтовый сбор")),gettext("Итого почтовый сбор"),iceb_u_prnbr(sps));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

fprintf(f2,"-----------------------------------------------------------------\n\
%-*s:%20s\n",
iceb_u_kolbait(20,gettext("Итого алиментов")),gettext("Итого алиментов"),iceb_u_prnbr(sal));

fprintf(f2,"\
%-*s:%20s\n",
iceb_u_kolbait(20,gettext("Итого почтовый сбор")),gettext("Итого почтовый сбор"),iceb_u_prnbr(sps));

konre(razom,pdv,vsego,isps_b_nds,f1,data->window);


iceb_podpis(f1,data->window);
iceb_podpis(f2,data->window);

fclose(f1);
fclose(f2);

data->imaf->plus(imaf);
data->imaf->plus(imaf1);
data->naimf->plus(gettext("Реестр почтовых переводов алиментов"));
data->naimf->plus(gettext("Ведомость удержания алиментов"));

for(int nom=0; nom < data->imaf->kolih(); nom++)
 iceb_ustpeh(data->imaf->ravno(nom),0,data->window);


data->kon_ras=0;
data->voz=0;
gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

return(FALSE);

}
