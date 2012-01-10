/*$Id: opssw_r.c,v 1.16 2011-02-21 07:35:55 sasa Exp $*/
/*09.03.2010	09.05.2005	Белых А.И. 	opssw_r.c
Расчёт отчёта по счетам списания материалов
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "opssw.h"

class opss_svt
 {
  public:
   class iceb_u_int kod_mat;
   class iceb_u_spisok sheta_spis;
   class iceb_u_double sum_svt;
  
 };

class opss_kodm_ei
 {
  public:
   class iceb_u_spisok kodm_ei;
   class iceb_u_spisok sheta_spis;
   class iceb_u_double suma;
   class iceb_u_double kolih;
   class iceb_u_double gori_kolih;  
   class iceb_u_double gori_suma;  
 };
class opss_svt_shet
 {
  public:
   class iceb_u_spisok kod_gr_shet;
   class iceb_u_spisok sheta_spis;
   class iceb_u_double sum_svt;
  
 };

class opssw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class opssw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  opssw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   opssw_r_key_press(GtkWidget *widget,GdkEventKey *event,class opssw_r_data *data);
gint opssw_r1(class opssw_r_data *data);
void  opssw_r_v_knopka(GtkWidget *widget,class opssw_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern int kol_strok_na_liste;
extern double	okrcn;  /*Округление цены*/
extern double	okrg1;  /*Округление 1*/

int opssw_r(class opssw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class opssw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать движение товарно-материальных ценностей по счетам списания"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(opssw_r_key_press),&data);

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

repl.plus(gettext("Распечатать движение товарно-материальных ценностей по счетам списания"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(opssw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)opssw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  opssw_r_v_knopka(GtkWidget *widget,class opssw_r_data *data)
{
//printf("opssw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   opssw_r_key_press(GtkWidget *widget,GdkEventKey *event,class opssw_r_data *data)
{
// printf("opssw_r_key_press\n");
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}
/***********************/
/*Распечатка реквизитов поиска*/
/*****************************/
void opss_rrp(int *kol_str,
class opssw_rr *rp,
FILE *ff)
{

if(rp->sklad.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Склад"),rp->sklad.ravno());
  *kol_str+=1;
 }
if(rp->shet_sp.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Счёт списания"),rp->shet_sp.ravno());
  *kol_str+=1;
 }
if(rp->shet_uh.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Счёт учёта"),rp->shet_uh.ravno());
  *kol_str+=1;
 }
if(rp->kodgrmat.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Группа"),rp->kodgrmat.ravno());
  *kol_str+=1;
 }
if(rp->kodmat.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код материалла"),rp->kodmat.ravno());
  *kol_str+=1;
 }
if(rp->kodop.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код операции"),rp->kodop.ravno());
  *kol_str+=1;
 }
if(rp->kontr.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Контрагент"),rp->kontr.ravno());
  *kol_str+=1;
 }
}

/********************************/
/*Распечатка сводной таблицы*/
/******************************/
void opss_rsvt(int metka_oth, //0-по кодам материалов 1-по кодам групп материалов
class opss_svt *svt,
class opssw_rr *rp,
GtkWidget *wpredok)
{
char imaf[56];
FILE *ff;
if(metka_oth == 0)
  sprintf(imaf,"opss_sv%d.lst",getpid());
if(metka_oth == 1)
 sprintf(imaf,"opss_svg%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
rp->imaf.plus(imaf);
if(metka_oth == 0 )
  rp->naimf.plus(gettext("Общие итоги"));
if(metka_oth == 1 )
  rp->naimf.plus(gettext("Общие итоги по группам материалов"));

iceb_u_zagolov(gettext("Движение товарно-материальных ценностей по счетам списания"),rp->datan.ravno(),rp->datak.ravno(),organ,ff);

int kol_str=0;

//Распечатка реквизитов поиска
opss_rrp(&kol_str,rp,ff);

int kolih_shet=svt->sheta_spis.kolih();
int kolih_kod_mat=svt->kod_mat.kolih();

class iceb_u_str liniq;
liniq.plus("-------------------------------------");
//          1234567890123456789012345678901234567

for(int ii=0; ii <= kolih_shet; ii++)
  liniq.plus("-----------");
//            12345678901

fprintf(ff,"%s\n",liniq.ravno());

if(metka_oth == 0)
 fprintf(ff,"%-*s|%-*.*s|",
 iceb_u_kolbait(5,gettext("Код")),gettext("Код"),
 iceb_u_kolbait(30,gettext("Наименование материалла")),
 iceb_u_kolbait(30,gettext("Наименование материалла")),
 gettext("Наименование материалла"));

if(metka_oth == 1)
 fprintf(ff,"%-*s|%-*.*s|",
 iceb_u_kolbait(5,gettext("Код")),gettext("Код"),
 iceb_u_kolbait(30,gettext("Наименование группы материалла")),
 iceb_u_kolbait(30,gettext("Наименование группы материалла")),
 gettext("Наименование группы материалла"));

for(int ii=0; ii < kolih_shet; ii++)
 fprintf(ff,"%-*s|",iceb_u_kolbait(10,svt->sheta_spis.ravno(ii)),svt->sheta_spis.ravno(ii));

fprintf(ff,"%*s|\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));

fprintf(ff,"%s\n",liniq.ravno());

char naim_kod_mat[112];
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
double itogo_gor=0.;

for(int ii=0; ii < kolih_kod_mat; ii++)
 {
  memset(naim_kod_mat,'\0',sizeof(naim_kod_mat));
  if(metka_oth == 0)  
   sprintf(strsql,"select naimat from Material where kodm=%d",svt->kod_mat.ravno(ii));

  if(metka_oth == 1)  
   sprintf(strsql,"select naik from Grup where kod=%d",svt->kod_mat.ravno(ii));
  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strncpy(naim_kod_mat,row[0],sizeof(naim_kod_mat)-1);  
  
  fprintf(ff,"%5d %-*.*s",svt->kod_mat.ravno(ii),
  iceb_u_kolbait(30,naim_kod_mat),iceb_u_kolbait(30,naim_kod_mat),naim_kod_mat);
  itogo_gor=0.;
    
  for(int kk=0; kk < kolih_shet; kk++)
   {
    fprintf(ff," %10.2f",svt->sum_svt.ravno(ii*kolih_shet+kk)); 
    itogo_gor+=svt->sum_svt.ravno(ii*kolih_shet+kk);
   }
  fprintf(ff," %10.2f\n",itogo_gor);
 }
fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff,"%*s",iceb_u_kolbait(36,gettext("Итого")),gettext("Итого"));
itogo_gor=0;
double itogo_kol=0;
for(int ii=0; ii < kolih_shet; ii++)
 {
  itogo_kol=0.;  
  //считаем колонку по вертикали
  for(int kk=0; kk < kolih_kod_mat; kk++)
    itogo_kol+=svt->sum_svt.ravno(kk*kolih_shet+ii);    
  fprintf(ff," %10.2f",itogo_kol);
  itogo_gor+=itogo_kol;
 }
fprintf(ff," %10.2f\n",itogo_gor);


iceb_podpis(ff,wpredok);
fclose(ff);






}

/********************************/
/*Распечатка сводной таблицы по группам и счетам учёта*/
/******************************/
void opss_rsvt_shet(class opss_svt_shet *svt,
class opssw_rr *rp,
GtkWidget *wpredok)
{
char imaf[56];
FILE *ff;
sprintf(imaf,"opss_svg_sh%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
rp->imaf.plus(imaf);
rp->naimf.plus(gettext("Общие итоги по группам и счетам"));

iceb_u_zagolov(gettext("Движение товарно-материальных ценностей по счетам списания"),rp->datan.ravno(),rp->datak.ravno(),organ,ff);

int kol_str=0;

//Распечатка реквизитов поиска
opss_rrp(&kol_str,rp,ff);

int kolih_shet=svt->sheta_spis.kolih();
int kolih_kod_mat=svt->kod_gr_shet.kolih();

class iceb_u_str liniq;
liniq.plus("----------------------------------------------");
//          1234567890123456789012345678901234567

for(int ii=0; ii <= kolih_shet; ii++)
  liniq.plus("-----------");
//            12345678901

fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff,"%-*s|%-*.*s|%-*.*s|",
iceb_u_kolbait(5,gettext("Код")),gettext("Код"),
iceb_u_kolbait(30,gettext("Наименование группы материалла")),
iceb_u_kolbait(30,gettext("Наименование группы материалла")),
gettext("Наименование группы материалла"),
iceb_u_kolbait(8,gettext("Счёт")),
iceb_u_kolbait(8,gettext("Счёт")),
gettext("Счёт"));

for(int ii=0; ii < kolih_shet; ii++)
 fprintf(ff,"%-*s|",iceb_u_kolbait(10,svt->sheta_spis.ravno(ii)),svt->sheta_spis.ravno(ii));

fprintf(ff,"%*s|\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));

fprintf(ff,"%s\n",liniq.ravno());

char naim_kod_mat[112];
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
double itogo_gor=0.;
char kod_gr[20];
char shet_uheta[20];

for(int ii=0; ii < kolih_kod_mat; ii++)
 {
  memset(naim_kod_mat,'\0',sizeof(naim_kod_mat));
  iceb_u_polen(svt->kod_gr_shet.ravno(ii),kod_gr,sizeof(kod_gr),1,'|');
  iceb_u_polen(svt->kod_gr_shet.ravno(ii),shet_uheta,sizeof(shet_uheta),2,'|');
    

  sprintf(strsql,"select naik from Grup where kod=%s",kod_gr);
  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strncpy(naim_kod_mat,row[0],sizeof(naim_kod_mat)-1);  
  
  fprintf(ff,"%5s %-*.*s %-*.*s",
  kod_gr,
  iceb_u_kolbait(30,naim_kod_mat),iceb_u_kolbait(30,naim_kod_mat),naim_kod_mat,
  iceb_u_kolbait(8,shet_uheta),iceb_u_kolbait(8,shet_uheta),shet_uheta);
  
  itogo_gor=0.;
    
  for(int kk=0; kk < kolih_shet; kk++)
   {
    fprintf(ff," %10.2f",svt->sum_svt.ravno(ii*kolih_shet+kk)); 
    itogo_gor+=svt->sum_svt.ravno(ii*kolih_shet+kk);
   }
  fprintf(ff," %10.2f\n",itogo_gor);
 }
fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff,"%*s",iceb_u_kolbait(45,gettext("Итого")),gettext("Итого"));
itogo_gor=0;
double itogo_kol=0;
for(int ii=0; ii < kolih_shet; ii++)
 {
  itogo_kol=0.;  
  //считаем колонку по вертикали
  for(int kk=0; kk < kolih_kod_mat; kk++)
    itogo_kol+=svt->sum_svt.ravno(kk*kolih_shet+ii);    
  fprintf(ff," %10.2f",itogo_kol);
  itogo_gor+=itogo_kol;
 }
fprintf(ff," %10.2f\n",itogo_gor);


iceb_podpis(ff,wpredok);
fclose(ff);

}
/*********************************/
/*Распечатка сводной таблицы по кодам материалов-единицам измерения с количеством и суммами*/
/**********************************/
void opss_ei_kolih(class opss_kodm_ei *svt,
class opssw_rr *rp,
GtkWidget *wpredok)
{
char imaf[56];
FILE *ff;
sprintf(imaf,"opss_eik%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
rp->imaf.plus(imaf);
rp->naimf.plus(gettext("Общие итоги по счетам с количеством"));

iceb_u_zagolov(gettext("Движение товарно-материальных ценностей по счетам списания"),rp->datan.ravno(),rp->datak.ravno(),organ,ff);

int kol_str=0;

//Распечатка реквизитов поиска
opss_rrp(&kol_str,rp,ff);

int kolih_shet=svt->sheta_spis.kolih();
int kolih_km_ei=svt->kodm_ei.kolih();

class iceb_u_str liniq;
liniq.plus("--------------------------------------------");
//          1234567890123456789012345678901234567

for(int ii=0; ii <= kolih_shet; ii++)
  liniq.plus("----------------------");
//            12345678901

fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff,"%-*s|%-*.*s|%-*.*s|",
iceb_u_kolbait(5,gettext("Код")),gettext("Код"),
iceb_u_kolbait(30,gettext("Наименование материалла")),
iceb_u_kolbait(30,gettext("Наименование материалла")),
gettext("Наименование материалла"),
iceb_u_kolbait(6,gettext("Единица")),
iceb_u_kolbait(6,gettext("Единица")),
gettext("Единица"));


for(int ii=0; ii < kolih_shet; ii++)
 fprintf(ff,"%-*s|",iceb_u_kolbait(21,svt->sheta_spis.ravno(ii)),svt->sheta_spis.ravno(ii));

fprintf(ff,"%-*s|\n",iceb_u_kolbait(21,gettext("Итого")),gettext("Итого"));



fprintf(ff,"%5s|%30s|%*.*s|","","",
iceb_u_kolbait(6,gettext("измерения")),
iceb_u_kolbait(6,gettext("измерения")),
gettext("измерения"));

for(int ii=0; ii < kolih_shet+1; ii++)
 fprintf(ff,"%-*.*s|%-*.*s|",
 iceb_u_kolbait(10,gettext("Количество")),
 iceb_u_kolbait(10,gettext("Количество")),
 gettext("Количество"),
 iceb_u_kolbait(10,gettext("Сумма")),
 iceb_u_kolbait(10,gettext("Сумма")),
 gettext("Сумма"));

fprintf(ff,"\n");


fprintf(ff,"%s\n",liniq.ravno());
char ei[32];
int kodmat=0;
char naim_kod_mat[112];
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
double itogo_gor_kolih=0.;
double itogo_gor_suma=0.;
double kolih=0.;
double suma=0.;
for(int nom_km_ei=0; nom_km_ei < kolih_km_ei; nom_km_ei++)
 {
  memset(naim_kod_mat,'\0',sizeof(naim_kod_mat));
  iceb_u_polen(svt->kodm_ei.ravno(nom_km_ei),&kodmat,1,'|');
  iceb_u_polen(svt->kodm_ei.ravno(nom_km_ei),ei,sizeof(ei),2,'|');
    

  sprintf(strsql,"select naimat from Material where kodm=%d",kodmat);
  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strncpy(naim_kod_mat,row[0],sizeof(naim_kod_mat)-1);  
  
  fprintf(ff,"%5d|%-*.*s|%-*.*s|",
  kodmat,
  iceb_u_kolbait(30,naim_kod_mat),iceb_u_kolbait(30,naim_kod_mat),naim_kod_mat,
  iceb_u_kolbait(6,ei),iceb_u_kolbait(6,ei),ei);
  
  itogo_gor_kolih=itogo_gor_suma=0.;
    
  for(int nom_shet=0; nom_shet < kolih_shet; nom_shet++)
   {
    suma=svt->suma.ravno(nom_km_ei*kolih_shet+nom_shet);
    kolih=svt->kolih.ravno(nom_km_ei*kolih_shet+nom_shet);
    if(kolih != 0. || suma != 0.)
     {    
      fprintf(ff,"%10.2f|%10.2f|",kolih,suma);
      itogo_gor_kolih+=kolih;
      itogo_gor_suma+=suma;
     }
    else
     fprintf(ff,"%10s|%10s|","","");
   }
  fprintf(ff,"%10.2f|%10.2f|\n",itogo_gor_kolih,itogo_gor_suma);
 }
fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff,"%*s|",iceb_u_kolbait(43,gettext("Итого")),gettext("Итого"));

for(int nom_shet=0; nom_shet < kolih_shet; nom_shet++)
 fprintf(ff,"%10.2f|%10.2f|",svt->gori_kolih.ravno(nom_shet),svt->gori_suma.ravno(nom_shet));
 
fprintf(ff,"%10.2f|%10.2f|\n",svt->gori_kolih.suma(),svt->gori_suma.suma());

iceb_podpis(ff,wpredok);
fclose(ff);

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

fprintf(ff,gettext("\
  Код   |    Наименование    |  Счёт    |   Счёт   |   Сумма  |Един. |Количec-|   Цена   |   Дата   |Номер    |Склад|Опера-|Контра-|\n"));
fprintf(ff,gettext("\
материа.|     материалла     |списания  |  учёта   |          |измер.|  тво   |          | документа|документа|     |ция   |гент   |\n"));

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------\n");
*kol_str+=5;


}

/*****************************/
/*Выдача итога*/
/*****************************/

void opss_itogo(int metka, //1-счёт учёта 2-счёт списания
char *kod,double itog,FILE *ff,
class opssw_r_data *data)
{
if(itog == 0.)
 return;
 
char bros[312];
if(metka == 1)
  sprintf(bros,"%s %s",gettext("Итого по счёту учёта"),kod);
if(metka == 2)
  sprintf(bros,"%s %s",gettext("Итого по счёту списания"),kod);

if(metka == 3) //По коду группы
 {
  SQL_str row;
  class SQLCURSOR cur;
  char strsql[512];
  char naim[512];
  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naik from Grup where kod=%s",kod);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
    strncpy(naim,row[0],sizeof(naim)-1);
  sprintf(bros,"%s %s %s",gettext("Итого по группе"),kod,naim);

 }

fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(51,bros),bros,itog);
char strsql[100];
sprintf(strsql,"%*s:%10.2f\n",iceb_u_kolbait(30,bros),bros,itog);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

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


/******************************************/
/******************************************/

gint opssw_r1(class opssw_r_data *data)
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

class opss_svt svt; //по кодам материалов
class opss_svt svtg; //по группам материалов
class opss_kodm_ei svt_ei; /*по кодам материалов-единиц измерения */

class opss_svt_shet svtg_shets; //по группам материалов и счетам учёта


sprintf(strsql,"%d.%d.%d => %d.%d.%d\n",dn,mn,gn,dk,mk,gk);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"select datd,sklad,nomd,nomkar,kodm,kolih,cena,ei,shet from Dokummat1 \
where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and \
nomkar <> 0 and tipz=2 order by shet asc",
gn,mn,dn,gk,mk,dk);

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
char imaf_tmp[50];

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
float kolstr1=0;
char kod_grup[24];

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);
  
  if(iceb_u_proverka(data->rk->shet_sp.ravno(),row[8],0,0) != 0)
    continue;  
  if(iceb_u_proverka(data->rk->sklad.ravno(),row[1],0,0) != 0)
    continue;  

  if(iceb_u_proverka(data->rk->kodmat.ravno(),row[4],0,0) != 0)
    continue;  

  //читаем материал
  memset(naim,'\0',sizeof(naim));
  memset(kod_grup,'\0',sizeof(kod_grup));
  sprintf(strsql,"select kodgr,naimat from Material where kodm=%s",row[4]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    strncpy(naim,row1[1],sizeof(naim)-1);
    if(iceb_u_proverka(data->rk->kodgrmat.ravno(),row1[0],0,0) != 0)
      continue;  
    strncpy(kod_grup,row1[0],sizeof(kod_grup)-1);

   }
  memset(kodop,'\0',sizeof(kodop));
  memset(kontr,'\0',sizeof(kontr));
  
  //читаем шапку документа
  sprintf(strsql,"select kontr,kodop from Dokummat where datd='%s' and sklad=%s and \
nomd='%s'",row[0],row[1],row[2]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    if(iceb_u_proverka(data->rk->kodop.ravno(),row1[1],0,0) != 0)
      continue;  
    if(iceb_u_proverka(data->rk->kontr.ravno(),row1[0],0,0) != 0)
      continue;  
    strncpy(kodop,row1[1],sizeof(kodop)-1);
    strncpy(kontr,row1[0],sizeof(kontr)-1);
   }

  iceb_u_rsdat(&dd,&md,&gd,row[0],2);
  kolih=readkolkw(atoi(row[1]),atoi(row[3]),dd,md,gd,row[2],data->window);  
  if(kolih == 0.)
   continue;
   
  memset(shet_uheta,'\0',sizeof(shet_uheta));

  sprintf(strsql,"select shetu from Kart where sklad=%s and nomk=%s",row[1],row[3]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
     if(iceb_u_proverka(data->rk->shet_uh.ravno(),row1[0],0,0) != 0)
      continue;  
    strncpy(shet_uheta,row1[0],sizeof(shet_uheta)-1);    
   }
  else
   {
    iceb_menu_soob(gettext("Не найдена карточка"),data->window);
   }

  memset(shet_sp,'\0',sizeof(shet_sp));
  strncpy(shet_sp,row[8],sizeof(shet_sp)-1);
  if(shet_sp[0] == '\0')
   strcpy(shet_sp,"???");

  fprintf(ff_tmp,"%s|%s|%s|%s|%s|%s|%s|%s|%.10g|%s|%s|%s|%s|%s|\n",
  shet_sp,kod_grup,shet_uheta,
  row[0],row[1],row[2],row[3],row[4],kolih,row[6],row[7],
  kodop,kontr,naim);

  if(svt.kod_mat.find(row[4]) < 0)
   svt.kod_mat.plus(row[4]);
  if(svt.sheta_spis.find(shet_sp) < 0)
   svt.sheta_spis.plus(shet_sp);

  if(svtg.kod_mat.find(kod_grup) < 0)
   svtg.kod_mat.plus(kod_grup);
  if(svtg.sheta_spis.find(shet_sp) < 0)
   svtg.sheta_spis.plus(shet_sp);
   
  sprintf(strsql,"%s|%s",kod_grup,shet_uheta);
  if(svtg_shets.kod_gr_shet.find(strsql) < 0)
   svtg_shets.kod_gr_shet.plus(strsql);
  if(svtg_shets.sheta_spis.find(shet_sp) < 0)
   svtg_shets.sheta_spis.plus(shet_sp);

  sprintf(strsql,"%s|%s",row[4],row[7]);
  if(svt_ei.kodm_ei.find(strsql) < 0)
   svt_ei.kodm_ei.plus(strsql);   
  if(svt_ei.sheta_spis.find(shet_sp) < 0)
   svt_ei.sheta_spis.plus(shet_sp);   

 }

fclose(ff_tmp);

svt.sum_svt.make_class(svt.kod_mat.kolih()*svt.sheta_spis.kolih());
svtg.sum_svt.make_class(svtg.kod_mat.kolih()*svtg.sheta_spis.kolih());
svtg_shets.sum_svt.make_class(svtg_shets.kod_gr_shet.kolih()*svtg_shets.sheta_spis.kolih());

svt_ei.kolih.make_class(svt_ei.kodm_ei.kolih()*svt_ei.sheta_spis.kolih());
svt_ei.suma.make_class(svt_ei.kodm_ei.kolih()*svt_ei.sheta_spis.kolih());
svt_ei.gori_kolih.make_class(svt_ei.sheta_spis.kolih());
svt_ei.gori_suma.make_class(svt_ei.sheta_spis.kolih());
int nomer_kod_mat=0;
int nomer_sheta_spis=0;
//sprintf(strsql,"sort -o %s -t\\| -n +0 -3 %s",imaf_tmp,imaf_tmp);
sprintf(strsql,"sort -o %s -t\\| -n -k1,4 %s",imaf_tmp,imaf_tmp);
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

data->rk->imaf.new_plus(imaf);
data->rk->naimf.new_plus(gettext("Движение товарно-материальных ценностей по счетам списания"));

iceb_u_zagolov(gettext("Движение товарно-материальных ценностей по счетам списания"),dn,mn,gn,dk,mk,gk,organ,ff);
kol_str=5;

//Распечатка реквизитов поиска
opss_rrp(&kol_str,data->rk,ff);

int nomlist=0;

//Шапка 

opss_h(&nomlist,&kol_str,ff);


double suma;
double cena;
char shet_sp_z[24];
char shet_uheta_z[24];
memset(shet_sp_z,'\0',sizeof(shet_sp_z));
memset(shet_uheta_z,'\0',sizeof(shet_uheta_z));
double itogo_shetu=0.;
double itogo_shets=0.;
double itogo_kod_grup=0.;

char stroka[1024];
char datd[24];
char sklad[24];
char nomd[24];
char kodmat[24];
char ei[32];
char kod_grup_z[24];
memset(kod_grup_z,'\0',sizeof(kod_grup_z));

while(fgets(stroka,sizeof(stroka),ff_tmp) != NULL)
 {
//  printw("%s\n",stroka);
  iceb_u_polen(stroka,shet_sp,sizeof(shet_sp),1,'|');
  iceb_u_polen(stroka,kod_grup,sizeof(kod_grup),2,'|');
  iceb_u_polen(stroka,shet_uheta,sizeof(shet_uheta),3,'|');

  iceb_u_polen(stroka,datd,sizeof(datd),4,'|');
  iceb_u_rsdat(&dd,&md,&gd,datd,2);

  iceb_u_polen(stroka,sklad,sizeof(sklad),5,'|');
  iceb_u_polen(stroka,nomd,sizeof(nomd),6,'|');

  iceb_u_polen(stroka,kodmat,sizeof(kodmat),8,'|');

  iceb_u_polen(stroka,strsql,sizeof(strsql),9,'|');
  kolih=atof(strsql);

  iceb_u_polen(stroka,strsql,sizeof(strsql),10,'|');
  cena=atof(strsql);

  iceb_u_polen(stroka,ei,sizeof(ei),11,'|');

  iceb_u_polen(stroka,kodop,sizeof(kodop),12,'|');

  iceb_u_polen(stroka,kontr,sizeof(kontr),13,'|');

  iceb_u_polen(stroka,naim,sizeof(naim),14,'|');

  if(iceb_u_SRAV(kod_grup_z,kod_grup,0) != 0)
   {
    if(kod_grup_z[0] != '\0')
     {

      opps_shet_str(&nomlist,&kol_str,ff);
      opss_itogo(3,kod_grup_z,itogo_kod_grup,ff,data);
      itogo_kod_grup=0.;

     }    
    strcpy(kod_grup_z,kod_grup);
   }

  if(iceb_u_SRAV(shet_uheta_z,shet_uheta,0) != 0)
   {
    if(shet_uheta_z[0] != '\0')
     {

      opps_shet_str(&nomlist,&kol_str,ff);
      opss_itogo(1,shet_uheta_z,itogo_shetu,ff,data);
      itogo_shetu=0.;

      opps_shet_str(&nomlist,&kol_str,ff);
      fprintf(ff,"\n");
     }    
    strcpy(shet_uheta_z,shet_uheta);
   }

  if(iceb_u_SRAV(shet_sp_z,shet_sp,0) != 0)
   {
    if(shet_sp_z[0] != '\0')
     {

      opps_shet_str(&nomlist,&kol_str,ff);
      opss_itogo(1,shet_uheta_z,itogo_shetu,ff,data);
      itogo_shetu=0.;

      opps_shet_str(&nomlist,&kol_str,ff);
      opss_itogo(3,kod_grup_z,itogo_kod_grup,ff,data);
      itogo_kod_grup=0.;
      memset(kod_grup_z,'\0',sizeof(kod_grup_z));

      opps_shet_str(&nomlist,&kol_str,ff);
      opss_itogo(2,shet_sp_z,itogo_shets,ff,data);

      opps_shet_str(&nomlist,&kol_str,ff);
      itogo_shets=0.;
      fprintf(ff,"\n");
     }    
    strcpy(shet_sp_z,shet_sp);
   }

  
  cena=iceb_u_okrug(cena,okrcn);
  
  suma=cena*kolih;
  suma=iceb_u_okrug(suma,okrg1);
  itogo_shetu+=suma;
  itogo_shets+=suma;  
  itogo_kod_grup+=suma;

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


  nomer_kod_mat=svt.kod_mat.find(kodmat);
  nomer_sheta_spis=svt.sheta_spis.find(shet_sp);
  svt.sum_svt.plus(suma,nomer_kod_mat*svt.sheta_spis.kolih()+nomer_sheta_spis);

  nomer_kod_mat=svtg.kod_mat.find(kod_grup);
  nomer_sheta_spis=svtg.sheta_spis.find(shet_sp);
  svtg.sum_svt.plus(suma,nomer_kod_mat*svtg.sheta_spis.kolih()+nomer_sheta_spis);
  
  sprintf(strsql,"%s|%s",kod_grup,shet_uheta);
  nomer_kod_mat=svtg_shets.kod_gr_shet.find(strsql);
  nomer_sheta_spis=svtg_shets.sheta_spis.find(shet_sp);
  svtg_shets.sum_svt.plus(suma,nomer_kod_mat*svtg_shets.sheta_spis.kolih()+nomer_sheta_spis);

  sprintf(strsql,"%s|%s",kodmat,ei);
  nomer_kod_mat=svt_ei.kodm_ei.find(strsql);
  nomer_sheta_spis=svt_ei.sheta_spis.find(shet_sp);
  svt_ei.suma.plus(suma,nomer_kod_mat*svt_ei.sheta_spis.kolih()+nomer_sheta_spis);
  svt_ei.kolih.plus(kolih,nomer_kod_mat*svt_ei.sheta_spis.kolih()+nomer_sheta_spis);
  svt_ei.gori_kolih.plus(kolih,nomer_sheta_spis);    
  svt_ei.gori_suma.plus(suma,nomer_sheta_spis);    


 }

fclose(ff_tmp);




opss_itogo(3,kod_grup_z,itogo_kod_grup,ff,data);
opss_itogo(1,shet_uheta_z,itogo_shetu,ff,data);
opss_itogo(2,shet_sp_z,itogo_shets,ff,data);

iceb_podpis(ff,data->window);

fclose(ff);


opss_rsvt(0,&svt,data->rk,data->window);

opss_rsvt(1,&svtg,data->rk,data->window);

opss_rsvt_shet(&svtg_shets,data->rk,data->window);

opss_ei_kolih(&svt_ei,data->rk,data->window);

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
