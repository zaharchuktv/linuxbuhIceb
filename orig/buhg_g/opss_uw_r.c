/*$Id: opss_uw_r.c,v 1.16 2011-02-21 07:35:55 sasa Exp $*/
/*23.03.2010	24.12.2005	Белых А.И. 	opss_uw_r.c
Расчёт отчёта по счетам списания услуг
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "opss_uw.h"

class opss_uw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class opss_uw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  opss_uw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   opss_uw_r_key_press(GtkWidget *widget,GdkEventKey *event,class opss_uw_r_data *data);
gint opss_uw_r1(class opss_uw_r_data *data);
void  opss_uw_r_v_knopka(GtkWidget *widget,class opss_uw_r_data *data);



extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern int kol_strok_na_liste;
extern double	okrcn;  /*Округление цены*/
extern double	okrg1;  /*Округление 1*/

int opss_uw_r(class opss_uw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class opss_uw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
if(data.rk->metka == 1)
 sprintf(strsql,"%s %s",name_system,gettext("Распечатать движение услуг по счетам получения"));
if(data.rk->metka == 2)
 sprintf(strsql,"%s %s",name_system,gettext("Распечатать движение услуг по счетам списания"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(opss_uw_r_key_press),&data);

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

if(data.rk->metka == 1)
 repl.plus(gettext("Распечатать движение услуг по счетам получения"));
if(data.rk->metka == 2)
 repl.plus(gettext("Распечатать движение услуг по счетам списания"));

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(opss_uw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)opss_uw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  opss_uw_r_v_knopka(GtkWidget *widget,class opss_uw_r_data *data)
{
//printf("opss_uw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   opss_uw_r_key_press(GtkWidget *widget,GdkEventKey *event,class opss_uw_r_data *data)
{
// printf("opss_uw_r_key_press\n");
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}
/********************************/
/*шапка*/
/**********************************/

void opss_h(int *nomlist,int *kol_str,FILE *ff)
{
*nomlist+=1;
fprintf(ff,"%110s%s N%d\n","",gettext("Лист"),*nomlist);
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"\
  Код   |    Наименование    |Счёт спис.|   Счёт   |   Сумма  |Един. |Количec-|   Цена   |   Дата   |Номер    |Склад|Опера-|Контра-|\n\
материа.|     материалла     |/получения|  учёта   |          |измер.|  тво   |          | документа|документа|     |ция   |гент   |\n");

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------\n");
*kol_str+=5;


}

/*****************************/
/*Выдача итога*/
/*****************************/

void opss_itogo(int metka, //1-счёт учёта 2-счёт списания
const char *kod,double itog,FILE *ff,FILE *ffi)
{
if(itog == 0.)
 return;
 
char bros[312];
if(metka == 1)
  sprintf(bros,"%s %s",gettext("Итого по счёту учёта"),kod);
if(metka == 2)
 {
  sprintf(bros,"%s %s",gettext("Итого по счёту списания"),kod);
  fprintf(ffi,"%-10s %10.2f\n",kod,itog);
 }

if(metka == 3)
 {
  sprintf(bros,"%s",gettext("Общий итог"));
  fprintf(ffi,"%*.*s %10.2f\n",
  iceb_u_kolbait(10,gettext("Итого")),
  iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),itog);
 }
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(51,bros),bros,itog);

}
/************************/
/*Счётчик строк*/
/*************************/

void opps_shet_str(int *nomlist,int *kol_str,FILE *ff)
{
*kol_str+=1;
if(*kol_str <= kol_strok_na_liste)
 return;

fprintf(ff,"\f");
*kol_str=0;
opss_h(nomlist,kol_str,ff);
*kol_str+=1;
}

/**************************************/
/*Распечатываем свод по услугам*/
/******************************/
void opss_u_svod(FILE *ff,class iceb_u_spisok *shet_kodzap,
class iceb_u_double *shet_kodzap_suma,
class iceb_u_double *shet_kodzap_kolih,
GtkWidget *wpredok)
{

fprintf(ff,"\
-------------------------------------------------------------\n");

fprintf(ff,gettext("\
  Код   |    Наименование    |Счёт спис.|   Сумма  |Количec-|\n\
 услуги |      услуги        |/получения|          |  тво   |\n"));

/*
12345678 12345678901234567890 1234567890 1234567890 12345678
*/

fprintf(ff,"\
-------------------------------------------------------------\n");

char kodzap[24];
char shet_spis[24];
double suma;
double kolih;
char naim[512];
SQL_str row1;
SQLCURSOR cur1;
char strsql[512];
double itogo_suma=0.;
double itogo_kolih=0.;
char shet_zap[32];
memset(shet_zap,'\0',sizeof(shet_zap));
double itogo_suma_shet=0.;
double itogo_kolih_shet=0.;
for(int i=0; i < shet_kodzap->kolih(); i++)
 {
  iceb_u_polen(shet_kodzap->ravno(i),shet_spis,sizeof(shet_spis),1,'|');
  iceb_u_polen(shet_kodzap->ravno(i),kodzap,sizeof(kodzap),2,'|');

  if(iceb_u_SRAV(shet_zap,shet_spis,0) != 0)
   {
    if(shet_zap[0] != '\0')
     {
      sprintf(strsql,"%s %s",gettext("Итого по счёту"),shet_zap);
      fprintf(ff,"%40s:%10.2f %8.2f\n",strsql,itogo_suma_shet,itogo_kolih_shet);
     }
    itogo_suma_shet=itogo_kolih_shet=0.;
    strncpy(shet_zap,shet_spis,sizeof(shet_zap)-1);
    
   }

  memset(naim,'\0',sizeof(naim));
  
  //читаем материал
  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naius from Uslugi where kodus=%s",kodzap);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
   {
    strncpy(naim,row1[0],sizeof(naim)-1);
   }
  else
   {
    sprintf(strsql,"select naimat from Material where kodm=%s",kodzap);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
      strncpy(naim,row1[0],sizeof(naim)-1);
   } 
  suma=shet_kodzap_suma->ravno(i);
  kolih=shet_kodzap_kolih->ravno(i);
  
  itogo_suma+=suma;
  itogo_kolih+=kolih;
  
  itogo_suma_shet+=suma;
  itogo_kolih_shet+=kolih;
    
  fprintf(ff,"%-8s %-*.*s %-*s %10.2f %8.2f\n",
  kodzap,
  iceb_u_kolbait(20,naim),iceb_u_kolbait(20,naim),naim,
  iceb_u_kolbait(10,shet_spis),shet_spis,
  suma,kolih);

 }

sprintf(strsql,"%s %s",gettext("Итого по счёту"),shet_spis);
fprintf(ff,"%*s:%10.2f %8.2f\n",
iceb_u_kolbait(40,strsql),strsql,
itogo_suma_shet,itogo_kolih_shet);

fprintf(ff,"\
-------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %8.2f\n",iceb_u_kolbait(40,gettext("Итого")),gettext("Итого"),itogo_suma,itogo_kolih);

iceb_podpis(ff,wpredok);

}

/******************************************/
/******************************************/

gint opss_uw_r1(class opss_uw_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;


short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%s %d.%d.%d %s %d.%d.%d\n",gettext("Период с"),dn,mn,gn,gettext("по"),
dk,mk,gk);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"select datd,podr,nomd,metka,kodzap,kolih,cena,ei,shsp,shetu,nz from Usldokum1 \
where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and tp=%d order by shsp asc",
gn,mn,dn,gk,mk,dk,data->rk->metka);

int kolstr;
SQL_str row;
SQLCURSOR cur;
SQL_str row1;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf_tmp[56];

sprintf(imaf_tmp,"opss%d.tmp",getpid());
FILE *ff_tmp;

if((ff_tmp = fopen(imaf_tmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_tmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char kodop[32];
char kontr[24];
char shet_uheta[56];
char shet_sp[24];
double kolih;
short dd,md,gd;
char naim[512];

float kolstr1=0.;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->shet_sp.ravno(),row[8],1,0) != 0)
    continue;  
  if(iceb_u_proverka(data->rk->shet_uh.ravno(),row[9],1,0) != 0)
    continue;  
  if(iceb_u_proverka(data->rk->podr.ravno(),row[1],0,0) != 0)
    continue;  

  if(iceb_u_proverka(data->rk->kodzap.ravno(),row[4],0,0) != 0)
    continue;  

  //читаем материал
  memset(naim,'\0',sizeof(naim));
  if(row[3][0] == '0')
    sprintf(strsql,"select kodgr,naimat from Material where kodm=%s",row[4]);
  if(row[3][0] == '1')
    sprintf(strsql,"select kodgr,naius from Uslugi where kodus=%s",row[4]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    strncpy(naim,row1[1],sizeof(naim)-1);
    if(iceb_u_proverka(data->rk->kodgr.ravno(),row1[0],0,0) != 0)
      continue;  

   }

  iceb_u_rsdat(&dd,&md,&gd,row[0],2);
  kolih=readkoluw(dd,md,gd,atoi(row[1]),row[2],data->rk->metka,atoi(row[3]),atoi(row[4]),atoi(row[10]),data->window);
  if(kolih == 0.)
   continue;  

  memset(kodop,'\0',sizeof(kodop));
  memset(kontr,'\0',sizeof(kontr));
  
  //читаем шапку документа
  sprintf(strsql,"select kontr,kodop from Usldokum where datd='%s' and podr=%s and \
nomd='%s' and tp=%d",row[0],row[1],row[2],data->rk->metka);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    if(iceb_u_proverka(data->rk->kodop.ravno(),row1[1],0,0) != 0)
      continue;  
    if(iceb_u_proverka(data->rk->kontr.ravno(),row1[0],0,0) != 0)
      continue;  
    strncpy(kodop,row1[1],sizeof(kodop)-1);
    strncpy(kontr,row1[0],sizeof(kontr)-1);
   }

  memset(shet_sp,'\0',sizeof(shet_sp));
  strncpy(shet_sp,row[8],sizeof(shet_sp)-1);
  if(shet_sp[0] == '\0')
   strcpy(shet_sp,"???");

  fprintf(ff_tmp,"%s|%s|%s|%s|%s|%s|%s|%.10g|%s|%s|%s|%s|%s|\n",
  shet_sp,row[9],
  row[0],row[1],row[2],row[3],row[4],kolih,row[6],row[7],
  kodop,kontr,naim);
 }

fclose(ff_tmp);

//sprintf(strsql,"sort -o %s -t\\| -n +0 -2 %s",imaf_tmp,imaf_tmp);
sprintf(strsql,"sort -o %s -t\\| -n -k1,2 %s",imaf_tmp,imaf_tmp);
system(strsql);

if((ff_tmp = fopen(imaf_tmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaf_tmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
unlink(imaf_tmp);
char imaf[56];
FILE *ff;

sprintf(imaf,"opss%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int kol_str=0;

char imafi[56];
FILE *ffi;

sprintf(imafi,"opssi%d.lst",getpid());

if((ffi = fopen(imafi,"w")) == NULL)
 {
  iceb_er_op_fil(imafi,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf_svod[56];
sprintf(imaf_svod,"opsss%d.lst",getpid());
FILE *ff_svod;
if((ff_svod = fopen(imaf_svod,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_svod,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


if(data->rk->metka == 1)
 {
  iceb_u_zagolov(gettext("Движение услуг по счетам получения"),dn,mn,gn,dk,mk,gk,organ,ff);
  iceb_u_zagolov(gettext("Движение услуг по счетам получения"),dn,mn,gn,dk,mk,gk,organ,ffi);
  iceb_u_zagolov(gettext("Движение услуг по счетам получения"),dn,mn,gn,dk,mk,gk,organ,ff_svod);
 }
if(data->rk->metka == 2)
 {
  iceb_u_zagolov(gettext("Движение услуг по счетам списания"),dn,mn,gn,dk,mk,gk,organ,ff);
  iceb_u_zagolov(gettext("Движение услуг по счетам списания"),dn,mn,gn,dk,mk,gk,organ,ffi);
  iceb_u_zagolov(gettext("Движение услуг по счетам списания"),dn,mn,gn,dk,mk,gk,organ,ff_svod);
 }
kol_str=5;

if(data->rk->podr.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код подразделения"),data->rk->podr.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Код подразделения"),data->rk->podr.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Код подразделения"),data->rk->podr.ravno());
  kol_str++;
 }
if(data->rk->shet_sp.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Счёт списания"),data->rk->shet_sp.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Счёт списания"),data->rk->shet_sp.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Счёт списания"),data->rk->shet_sp.ravno());
  kol_str++;
 }
if(data->rk->shet_uh.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Счёт учёта"),data->rk->shet_uh.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Счёт учёта"),data->rk->shet_uh.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Счёт учёта"),data->rk->shet_uh.ravno());
  kol_str++;
 }
if(data->rk->kodgr.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Группа"),data->rk->kodgr.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Группа"),data->rk->kodgr.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Группа"),data->rk->kodgr.ravno());
  kol_str++;
 }
if(data->rk->kodzap.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код услуги"),data->rk->kodzap.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Код услуги"),data->rk->kodzap.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Код услуги"),data->rk->kodzap.ravno());
  kol_str++;
 }
if(data->rk->kodop.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
  kol_str++;
 }
if(data->rk->kontr.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код контрагента"),data->rk->kontr.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Код контрагента"),data->rk->kontr.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Код контрагента"),data->rk->kontr.ravno());
  kol_str++;
 }

int nomlist=0;

//Шапка 

opss_h(&nomlist,&kol_str,ff);

fprintf(ffi,"----------------------\n");
fprintf(ffi,gettext("  Счёт    |   Сумма  |\n"));
fprintf(ffi,"----------------------\n");

double suma;
double cena;
char shet_sp_z[24];
char shet_uheta_z[24];
memset(shet_sp_z,'\0',sizeof(shet_sp_z));
memset(shet_uheta_z,'\0',sizeof(shet_uheta_z));
double itogo_shetu=0.;
double itogo_shets=0.;
double itogo_shets_o=0.;

char stroka[1024];
char datd[24];
//datd,sklad,nomd,nomkar,kodm,kolih,cena,ei
char sklad[24];
char nomd[24];
char kodmat[24];
char ei[32];
class iceb_u_spisok shet_kodzap;
class iceb_u_double shet_kodzap_suma;
class iceb_u_double shet_kodzap_kolih;

int nomer_shet_kodzap=0;

while(fgets(stroka,sizeof(stroka),ff_tmp) != NULL)
 {
//  printw("%s\n",stroka);
  iceb_u_polen(stroka,shet_sp,sizeof(shet_sp),1,'|');
  iceb_u_polen(stroka,shet_uheta,sizeof(shet_uheta),2,'|');

  iceb_u_polen(stroka,datd,sizeof(datd),3,'|');
  iceb_u_rsdat(&dd,&md,&gd,datd,2);

  iceb_u_polen(stroka,sklad,sizeof(sklad),4,'|');
  iceb_u_polen(stroka,nomd,sizeof(nomd),5,'|');

  iceb_u_polen(stroka,kodmat,sizeof(kodmat),7,'|');

  iceb_u_polen(stroka,strsql,sizeof(strsql),8,'|');
  kolih=atof(strsql);

  iceb_u_polen(stroka,strsql,sizeof(strsql),9,'|');
  cena=atof(strsql);

  iceb_u_polen(stroka,ei,sizeof(ei),10,'|');

  iceb_u_polen(stroka,kodop,sizeof(kodop),11,'|');

  iceb_u_polen(stroka,kontr,sizeof(kontr),12,'|');

  iceb_u_polen(stroka,naim,sizeof(naim),13,'|');

  if(iceb_u_SRAV(shet_uheta_z,shet_uheta,0) != 0)
   {
    if(shet_uheta_z[0] != '\0')
     {

      opps_shet_str(&nomlist,&kol_str,ff);
      opss_itogo(1,shet_uheta_z,itogo_shetu,ff,ffi);

      opps_shet_str(&nomlist,&kol_str,ff);
      fprintf(ff,"\n");
     }    
    itogo_shetu=0.;
    strcpy(shet_uheta_z,shet_uheta);
   }

  if(iceb_u_SRAV(shet_sp_z,shet_sp,0) != 0)
   {
    if(shet_sp_z[0] != '\0')
     {

      opps_shet_str(&nomlist,&kol_str,ff);
      opss_itogo(1,shet_uheta_z,itogo_shetu,ff,ffi);
      itogo_shetu=0.;

      opps_shet_str(&nomlist,&kol_str,ff);
      opss_itogo(2,shet_sp_z,itogo_shets,ff,ffi);

      opps_shet_str(&nomlist,&kol_str,ff);
      fprintf(ff,"\n");
     }    
    itogo_shets_o+=itogo_shets;
    itogo_shets=0.;
    strcpy(shet_sp_z,shet_sp);
   }

  
  cena=iceb_u_okrug(cena,okrcn);
  
  suma=cena*kolih;
  suma=iceb_u_okrug(suma,okrg1);
  itogo_shetu+=suma;
  itogo_shets+=suma;  

  sprintf(strsql,"%s|%s",shet_sp,kodmat);
  
  if((nomer_shet_kodzap=shet_kodzap.find(strsql)) < 0)
    shet_kodzap.plus(strsql);
  
  shet_kodzap_kolih.plus(kolih,nomer_shet_kodzap);
  shet_kodzap_suma.plus(suma,nomer_shet_kodzap);

  opps_shet_str(&nomlist,&kol_str,ff);

  fprintf(ff,"%-8s %-*.*s %-*s %-*s %10.2f %-*s %8.8g %10.10g %02d.%02d.%d %-*s %-5s %-*s %-*s\n",
  kodmat,
  iceb_u_kolbait(20,naim),iceb_u_kolbait(20,naim),naim,
  iceb_u_kolbait(10,shet_sp),shet_sp,
  iceb_u_kolbait(10,shet_uheta),shet_uheta,
  suma,
  iceb_u_kolbait(6,ei),ei,
  kolih,cena,dd,md,gd,
  iceb_u_kolbait(9,nomd),nomd,
  sklad,
  iceb_u_kolbait(6,kodop),kodop,
  iceb_u_kolbait(7,kontr),kontr);
 }

fclose(ff_tmp);



itogo_shets_o+=itogo_shets;

opss_itogo(1,shet_uheta_z,itogo_shetu,ff,ffi);
opss_itogo(2,shet_sp_z,itogo_shets,ff,ffi);
opss_itogo(3,"",itogo_shets_o,ff,ffi);

iceb_podpis(ff,data->window);
iceb_podpis(ffi,data->window);

fclose(ff);
fclose(ffi);

opss_u_svod(ff_svod,&shet_kodzap,&shet_kodzap_suma,&shet_kodzap_kolih,data->window);

fclose(ff_svod);


data->rk->imaf.new_plus(imaf);

if(data->rk->metka == 1)
  data->rk->naimf.new_plus(gettext("Движение услуг по счетам получения"));
if(data->rk->metka == 2)
  data->rk->naimf.new_plus(gettext("Движение услуг по счетам списания"));

data->rk->imaf.plus(imafi);
data->rk->naimf.plus(gettext("Общие итоги"));

data->rk->imaf.plus(imaf_svod);
data->rk->naimf.plus(gettext("Свод по кодам услуг и счетам"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
  iceb_ustpeh(data->rk->imaf.ravno(nom),0,data->window);



gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
