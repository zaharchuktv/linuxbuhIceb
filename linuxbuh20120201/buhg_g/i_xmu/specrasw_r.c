/*$Id: specrasw_r.c,v 1.13 2011-02-21 07:35:57 sasa Exp $*/
/*07.08.2010	17.05.2005	Белых А.И. 	specrasw_r.c
Расчёт прогноза реализации 
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "../headers/buhg_g.h"
#include "specrasw.h"

class specrasw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class specrasw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  specrasw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   specrasw_r_key_press(GtkWidget *widget,GdkEventKey *event,class specrasw_r_data *data);
gint specrasw_r1(class specrasw_r_data *data);
void  specrasw_r_v_knopka(GtkWidget *widget,class specrasw_r_data *data);


void rppvo1(int skl1,char *naiskl,
short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,
int *kst,
FILE *ff);

void rasmasop(iceb_u_spisok *spopr,iceb_u_double *mkm,iceb_u_double *mkm1_1r,iceb_u_double *mkm1_1k,
short tip,FILE *ff,class specrasw_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern double	okrg1;  /*Округление 1*/

int specrasw_r(class specrasw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class specrasw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Расчёт количества материалов на изделие"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(specrasw_r_key_press),&data);

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

repl.plus(gettext("Расчёт количества материалов на изделие"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(specrasw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)specrasw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  specrasw_r_v_knopka(GtkWidget *widget,class specrasw_r_data *data)
{
//printf("specrasw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   specrasw_r_key_press(GtkWidget *widget,GdkEventKey *event,class specrasw_r_data *data)
{
// printf("specrasw_r_key_press\n");
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}

/******************************************/
/******************************************/

gint specrasw_r1(class specrasw_r_data *data)
{
iceb_clock sssk(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];



SQL_str         row1;
FILE		*ff;
char		imaf[32];
int		kolndet; /*Количество наименований деталей*/
char		naim[512];
int		sss;
float		kolstr1;
char		ei[32];
int		kodm=0;


if(data->rk->kolih.ravno_atof() == 0.)
 {
  iceb_menu_soob(gettext("Не введено количество !"),data->window);
  sssk.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


class iceb_razuz_data m_uz;
m_uz.kod_izd=data->rk->kod_izdel.ravno_atoi();
kolndet=iceb_razuz_kod(&m_uz,data->window);

SQLCURSOR cur1;
memset(naim,'\0',sizeof(naim));
sprintf(strsql,"select naimat from Material where kodm=%d",data->rk->kod_izdel.ravno_atoi());
if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
 {
  sprintf(strsql,"specrasw_r-%s %d !",gettext("Не найден код материалла"),data->rk->kod_izdel.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  sssk.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  
else
 strncpy(naim,row1[0],sizeof(naim)-1);

sprintf(strsql,"%s %d %s\n\
%s:%.10g\n\
---------------------------------------------------------------------\n",
gettext("Расчет входимости материалов в изделие"),
data->rk->kod_izdel.ravno_atoi(),naim,
gettext("Количество изделий"),
data->rk->kolih.ravno_atof());

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(imaf,"specr%d.lst",data->rk->kod_izdel.ravno_atoi());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sssk.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_startfil(ff);



short ddd=0;
short mmm=0;
short ggg=0;
iceb_u_poltekdat(&ddd,&mmm,&ggg);

sprintf(strsql,"%s %d %s",
gettext("Расчет входимости материаллов в изделие"),
data->rk->kod_izdel.ravno_atoi(),naim);

iceb_u_zagolov(strsql,0,0,0,0,0,0,organ,ff);

fprintf(ff,"%s:%.10g\n",gettext("Количество изделий"),data->rk->kolih.ravno_atof());



fprintf(ff,"\x1B\x4D"); /*12-знаков*/

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Kod |   Наименование   соcтавляющей          |Ед.из|Количество| Цена   |  Сумма   |  Остаток |  Разница |\n"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------\n");

double koliho=0.;
double kolihr=0.;
double cena=0.;
double suma=0.;
double isuma=0.;
double ostatok=0.;

kolstr1=0;
kolndet=m_uz.kod_det_ei.kolih();
for(sss=0; sss < kolndet; sss++)
 {
  iceb_pbar(data->bar,kolndet,++kolstr1);
  
  memset(naim,'\0',sizeof(naim));
  memset(ei,'\0',sizeof(ei));
  iceb_u_polen(m_uz.kod_det_ei.ravno(sss),&kodm,1,'|');
  iceb_u_polen(m_uz.kod_det_ei.ravno(sss),ei,sizeof(ei),2,'|');

  cena=suma=0.;
  if(m_uz.metka_mu.ravno(sss) == 0)
    sprintf(strsql,"select naimat,ei,cenapr from Material where kodm=%d",kodm);
  if(m_uz.metka_mu.ravno(sss) == 1)
    sprintf(strsql,"select naius,ei,cena from Uslugi where kodus=%d",kodm);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код записи"),kodm);
    iceb_menu_soob(strsql,data->window);
   }  
  else
   {
    strncpy(naim,row1[0],sizeof(naim)-1);
    if(row1[2] != NULL)
     cena=atof(row1[2]);
    if(cena != 0.)
     cena=iceb_u_okrug(cena,okrg1);
   }
  suma=koliho=kolihr=0.;
  kolihr=m_uz.kolih_det_ei.ravno(sss)*data->rk->kolih.ravno_atof();

  suma=cena*kolihr;
  if(suma != 0.)
   suma=iceb_u_okrug(suma,okrg1); 
  isuma+=suma;
  ostatok=0.;
  
  if(m_uz.metka_mu.ravno(sss) == 0)
   {
    koliho=ostdok1w(ddd,mmm,ggg,0,kodm);
    ostatok=koliho-kolihr;
   }
    
  sprintf(strsql,"%-4d %-*.*s %10.10g %8.8g %10.2f %10.10g %10.10g\n",
  kodm,
  iceb_u_kolbait(20,naim),iceb_u_kolbait(20,naim),naim,
  kolihr,cena,suma,koliho,ostatok);

  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  fprintf(ff,"%-4d %-*s %*s %10.10g %8.8g %10.2f %10.10g %10.10g\n",
  kodm,
  iceb_u_kolbait(40,naim),naim,
  iceb_u_kolbait(5,ei),ei,
  kolihr,cena,suma,koliho,ostatok);
  
 }
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(71,gettext("Итого")),gettext("Итого"),isuma);
sprintf(strsql,"%*s:%10.2f\n",iceb_u_kolbait(45,gettext("Итого")),gettext("Итого"),isuma);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
int kolnuz=m_uz.kod_uz.kolih();
if(kolnuz != 0)
 {
  sprintf(strsql,"\n%s:\n\
----------------------------------------------------------\n",
  gettext("Входимость узлов"));

  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  fprintf(ff,"\n%s:\n\
-------------------------------------------------------------------------\n",
  gettext("Входимость узлов"));  
  kolstr1=0;
  for(sss=0; sss < kolnuz; sss++)
   {
    iceb_pbar(data->bar,kolnuz,++kolstr1);
    memset(naim,'\0',sizeof(naim));
    memset(ei,'\0',sizeof(ei));
    for(int metka=0; metka < 2; metka++)
     {
      if(metka == 0)
       sprintf(strsql,"select naimat,ei from Material where kodm=%d",m_uz.kod_uz.ravno(sss));
      if(metka == 1)
       sprintf(strsql,"select naius,ei from Usluge where kodus=%d",m_uz.kod_uz.ravno(sss));
      if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
       {
        if(metka == 1)
         {        
          sprintf(strsql,"%s %d !",gettext("Не найден код записи"),m_uz.kod_uz.ravno(sss));
          iceb_menu_soob(strsql,data->window);
         }
       }  
      else
       {
        strncpy(naim,row1[0],sizeof(naim)-1);
        strncpy(ei,row1[1],sizeof(ei)-1);
        break;
       }
     }

    sprintf(strsql,"%-4d %-*s %10.10g\n",
    m_uz.kod_uz.ravno(sss),
    iceb_u_kolbait(30,naim),naim,
    m_uz.kolih_uz.ravno(sss)*data->rk->kolih.ravno_atof());

    fprintf(ff,"%-4d %-*s %*s %10.10g\n",
    m_uz.kod_uz.ravno(sss),
    iceb_u_kolbait(40,naim),naim,
    iceb_u_kolbait(5,ei),ei,
    m_uz.kolih_uz.ravno(sss)*data->rk->kolih.ravno_atof());
    
   }

 }
fprintf(ff,"\x1B\x50"); /*10-знаков*/
iceb_podpis(ff,data->window);
fclose(ff);



data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расчет входимости материаллов в изделие"));


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
