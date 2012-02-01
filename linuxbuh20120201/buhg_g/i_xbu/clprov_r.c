/*$Id: clprov_r.c,v 1.11 2011-02-21 07:35:51 sasa Exp $*/
/*28.04.2004	13.04.2004	Белых А.И.	clprov_r.c
Удаление проводок за период
*/
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "go.h"

class clprov_r_data
 {
  public:
  
  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  gfloat kolstr1; //Количество прочитанных строк
  int    kolstr;  //Количество строк в курсоре
  short     prohod;
  time_t vremn;

  short m,g;
  int   metka_r;    
  int   kolud;
  int   koluds;
  short metkazap;
      
  //Конструктор  
  clprov_r_data()
   {
    kolstr1=0.;
    kolstr=6;
    prohod=0;
  
    metkazap=koluds=kolud=0;
   }

 };

gboolean   clprov_r_key_press(GtkWidget *widget,GdkEventKey *event,class clprov_r_data *data);
gint clprov_r1(class clprov_r_data *data);
void  clprov_r_v_knopka(GtkWidget *widget,class clprov_r_data *data);
 
extern SQL_baza bd;
extern char *organ;
extern char *name_system;
extern short vplsh; /*0-двух порядковый план счетов 1-многпорядковый*/
extern short	startgodb; /*Стартовый год*/
extern iceb_u_str shrift_ravnohir;

void clprov_r(int metka_r,short mu,short gu)
{
char strsql[512];
iceb_u_str soob;
SQLCURSOR cur;
SQL_str   row;

clprov_r_data data;
data.metka_r=metka_r;
data.m=mu;
data.g=gu;

strcpy(strsql,"select VERSION()");
sql_readkey(&bd,strsql,&row,&cur);
if(iceb_u_SRAV(row[0],"3.22",1) == 0)
  data.metkazap=1;  

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Журнал ордер"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(clprov_r_key_press),&data);

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

if(metka_r == 0)
  sprintf(strsql,"%s",gettext("Удаление проводок за месяц"));
if(metka_r == 1)
  sprintf(strsql,"%s",gettext("Удаление проводок за год"));

soob.new_plus(strsql);

if(metka_r == 0)
  sprintf(strsql,"%s %d.%d",gettext("Дата"),data.m,data.g);
if(metka_r == 1)
  sprintf(strsql,"%s %d",gettext("Дата"),data.g);

soob.ps_plus(strsql);

sprintf(strsql,"VERSION:%s",row[0]);
soob.ps_plus(strsql);

GtkWidget *label=gtk_label_new(soob.ravno_toutf());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);


data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),400,300);

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста
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
gtk_widget_show_all(data.window);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
GtkTooltips *tooltops=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltops,data.knopka,gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(clprov_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)clprov_r1,&data);

gtk_main();


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  clprov_r_v_knopka(GtkWidget *widget,class clprov_r_data *data)
{
 printf("clprov_r_v_knopka\n");
 gtk_widget_destroy(data->window);
 return;
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   clprov_r_key_press(GtkWidget *widget,GdkEventKey *event,class clprov_r_data *data)
{
 printf("clprov_r_key_press\n");

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

gint clprov_r1(class clprov_r_data *data)
{
char strsql[512];
iceb_u_str soob;

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
//printf("clprov_r1 data->prohod=%d %d %f \n",data->prohod,data->kolstr,data->kolstr1);

if(data->prohod == 0)
 {
  time(&data->vremn);
  data->prohod++;

  if(data->g != 0)
   sprintf(strsql,"delete from Prov where datp >= '%04d-01-01' and \
  datp <= '%04d-12-31'",data->g,data->g);
  if(data->g == 0)
   sprintf(strsql,"delete from Prov where datp = '0000-00-00'");

  if(data->metka_r == 0)
   {
    memset(strsql,'\0',sizeof(strsql));
    sprintf(strsql,"delete from Prov where datp >= '%04d-%02d-01' and \
datp <= '%04d-%02d-31'",data->g,data->m,data->g,data->m);
   }

  if(sql_zap(&bd,strsql) != 0)
    iceb_msql_error(&bd,"",strsql,data->window);

  data->kolud+=sql_rows(&bd);

  sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),data->kolud);
  soob.new_plus(strsql);
  if(data->kolud > 0)
    soob.plus_ps(gettext("Оптимизируем таблицу"));
   
  iceb_printw(soob.ravno_toutf(),data->buffer,data->view);

  return(TRUE);

 }

if(data->prohod == 1)
 {
  data->prohod++;
  if(data->kolud > 0)
   {
    memset(strsql,'\0',sizeof(strsql));
    sprintf(strsql,"optimize table Prov");
    if(data->metkazap == 1)
     {
      if(sql_zap(&bd,strsql) != 0)
        iceb_msql_error(&bd,"",strsql,data->window);
     }
    else
      sql_readkey(&bd,strsql);
   }

  if(data->metka_r == 0)
   {
    data->prohod=4; //конец расчета
    data->kolstr1=4;
   }
  else
   {
    soob.new_plus_ps(gettext("Удаляем стартовые сальдо"));
   
    iceb_printw(soob.ravno_toutf(),data->buffer,data->view);
   }          

  return(TRUE);

 }

if(data->prohod == 2)
 {
  data->prohod++;
  sprintf(strsql,"delete from Saldo where gs=%d",data->g);

  if(sql_zap(&bd,strsql) != 0)
    iceb_msql_error(&bd,"",strsql,data->window);

  data->koluds=sql_rows(&bd);
  data->kolud+=data->koluds;

  sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),data->koluds);
  soob.new_plus(strsql);

  if(data->koluds > 0)
    soob.plus_ps(gettext("Оптимизируем таблицу"));

  iceb_printw(soob.ravno_toutf(),data->buffer,data->view);

  return(TRUE);

 }

if(data->prohod == 3)
 {
  data->prohod++;

  if(data->koluds > 0)
   {
    memset(strsql,'\0',sizeof(strsql));
    sprintf(strsql,"optimize table Saldo");
    if(data->metkazap == 1)
     {
      if(sql_zap(&bd,strsql) != 0)
       iceb_msql_error(&bd,"",strsql,data->window);
     }
    else
      sql_readkey(&bd,strsql);
   }

  return(TRUE);

 }

if(data->prohod == 4)
 {
  data->prohod++;

  iceb_printw_vr(data->vremn,data->buffer,data->view);
  return(TRUE);
 }


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);
return(FALSE);

}
