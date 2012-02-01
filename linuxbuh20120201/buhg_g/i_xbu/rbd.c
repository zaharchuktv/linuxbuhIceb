/*$Id: rbd.c,v 1.9 2011-02-21 07:35:57 sasa Exp $*/
/*29.03.2004	29.03.2004	Белых А.И.	rbd.c
Ревизия базы данных - определение годов за которые введена 
информация
*/
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze

class rbd_r_data
 {
  public:
  
  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  gfloat kolstr1; //Количество выполненных проходов
  int    kolstr;  //Количество проходов при расчете
  short     prohod;

  time_t vremn;
  SQLCURSOR cur;
  int       metka;
  short     god;
        
  //Конструктор  
  rbd_r_data()
   {
    kolstr1=0.;
    kolstr=9;
    prohod=0;
    god=0;
   }

 };

gboolean   rbd_r_key_press(GtkWidget *widget,GdkEventKey *event,class rbd_r_data *data);
gint rbd_r1(class rbd_r_data *data);
void  rbd_r_v_knopka(GtkWidget *widget,class rbd_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern iceb_u_str shrift_ravnohir;

void rbd(int metka)
/*
1-главная книга
2-материальный учет
3-расчет зарплаты
4-учет основных средств
5-платежные поручения
6-платежные требования
7-объявки в банк
9-учет услуг
*/
{
rbd_r_data data;

char strsql[512];
iceb_u_str soob;



printf("rbd\n");

data.metka=metka;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать список годов за которые введена информация"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rbd_r_key_press),&data);

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

if(metka == 1)
  sprintf(strsql,"%s",gettext("Главная книга"));

if(metka == 2)
  sprintf(strsql,"%s",gettext("Материальный учет"));

if(metka == 3)
  sprintf(strsql,"%s",gettext("Зороботная плата"));

if(metka == 4)
  sprintf(strsql,"%s",gettext("Учет основных средств"));

if(metka == 5)
  sprintf(strsql,"%s",gettext("Платёжные поручения"));

if(metka == 6)
  sprintf(strsql,"%s",gettext("Платёжные требования"));

if(metka == 8)
  sprintf(strsql,"%s",gettext("Учёт услуг"));

soob.new_plus(strsql);

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(rbd_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)rbd_r1,&data);

gtk_main();


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rbd_r_v_knopka(GtkWidget *widget,class rbd_r_data *data)
{
 printf("rbd_r_v_knopka\n");
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rbd_r_key_press(GtkWidget *widget,GdkEventKey *event,class rbd_r_data *data)
{
 printf("rbd_r_key_press\n");

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

gint rbd_r1(class rbd_r_data *data)
{
char strsql[512];
iceb_u_str soob;

//printf("rbd_r1 data->prohod=%d\n",data->prohod);

if(data->prohod == 0)
 {
  data->prohod++;
  time(&data->vremn);

  if(data->metka == 1)
    sprintf(strsql,"select distinct datp from Prov order by datp asc");

  if(data->metka == 2)
   sprintf(strsql,"select distinct datd from Dokummat order by datd asc");

  if(data->metka == 3)
    sprintf(strsql,"select distinct god from Zarn order by god asc");

  if(data->metka == 4)
   sprintf(strsql,"select distinct datd from Uosdok order by datd asc");

  if(data->metka == 5)
    sprintf(strsql,"select distinct datd from Pltp order by datd asc");

  if(data->metka == 6)
    sprintf(strsql,"select distinct datd from Pltt order by datd asc");

  if(data->metka == 7)
    sprintf(strsql,"select distinct datd from Obqvka order by datd asc");

  if(data->metka == 8)
    sprintf(strsql,"select distinct datd from Usldokum order by datd asc");

  if((data->kolstr=data->cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    return(FALSE);
   }

  if(data->kolstr == 0)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдено ни одной записи !"));
    iceb_menu_soob(&repl,data->window);
    return(FALSE);
   }

  return(TRUE);
 }

if(data->prohod == 1)
 {
  short d,m,g;
  SQL_str row;
  
  while(data->cur.read_cursor(&row) != 0)
   {
    iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

    if(data->metka == 3)
      g=atoi(row[0]);
    else
      iceb_u_rsdat(&d,&m,&g,row[0],2);

    if(data->god != g)
     {
      sprintf(strsql,"%d\n",g);
      iceb_printw(strsql,data->buffer,data->view);
      data->god=g;
     }
    return(TRUE);
   }

  data->prohod++;
  return(TRUE);
 }

if(data->prohod == 2)
 {
  data->prohod++;

  iceb_printw_vr(data->vremn,data->buffer,data->view);
  return(TRUE);
 }


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);
printf("rbd_r1 end\n");
return(FALSE);

}
