/*$Id: xdkudgod_r.c,v 1.11 2011-02-21 07:35:59 sasa Exp $*/
/*18.11.2008	10.03.2006	Белых А.И.	xdkudgod_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"

class xdkudgod_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  short god;
  
  xdkudgod_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   xdkudgod_r_key_press(GtkWidget *widget,GdkEventKey *event,class xdkudgod_r_data *data);
gint xdkudgod_r1(class xdkudgod_r_data *data);
void  xdkudgod_r_v_knopka(GtkWidget *widget,class xdkudgod_r_data *data);

void	sapkasord(short dd,short md,short gd,int nomlist,FILE *ffva);
void   hgrkd(FILE *ff);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern double   okrg1;
extern double	okrcn;

int xdkudgod_r(short god,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class xdkudgod_r_data data;
data.god=god;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Удаление данных за год"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(xdkudgod_r_key_press),&data);

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

repl.plus(gettext("Удаление данных за год"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(xdkudgod_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)xdkudgod_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  xdkudgod_r_v_knopka(GtkWidget *widget,class xdkudgod_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   xdkudgod_r_key_press(GtkWidget *widget,GdkEventKey *event,class xdkudgod_r_data *data)
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

/******************************************/
/******************************************/

gint xdkudgod_r1(class xdkudgod_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);


int		metkazap=0;

SQLCURSOR cur;
SQL_str row;

strcpy(strsql,"select VERSION()");

iceb_sql_readkey(strsql,&row,&cur,data->window);

if(iceb_u_SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  

sprintf(strsql,"VERSION:%s\n",row[0]);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

int kolstr=10;

sprintf(strsql,"%s\n",gettext("Удаление платёжных поручений"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"delete from Pltp where datd >= '%d-01-01' and \
datd <= '%d-12-31'",data->god,data->god);

iceb_sql_zapis(strsql,0,0,data->window);

float kolstr1=0.;
iceb_pbar(data->bar,kolstr,++kolstr1);    


sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Pltp");

if(metkazap == 1)
  iceb_sql_zapis(strsql,0,0,data->window);
else
  iceb_sql_readkey(strsql,data->window);

iceb_pbar(data->bar,kolstr,++kolstr1);    


sprintf(strsql,"%s\n",gettext("Удаление комментариев к платёжным поручениям"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"delete from Pltpz where datd >= '%d-01-01' and \
datd <= '%d-12-31'",data->god,data->god);

iceb_sql_zapis(strsql,0,0,data->window);

iceb_pbar(data->bar,kolstr,++kolstr1);    


sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Pltpz");

if(metkazap == 1)
  iceb_sql_zapis(strsql,0,0,data->window);
else
  iceb_sql_readkey(strsql,data->window);

iceb_pbar(data->bar,kolstr,++kolstr1);    

sprintf(strsql,"%s\n",gettext("Удаление платёжных требований"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"delete from Pltt where datd >= '%d-01-01' and \
datd <= '%d-12-31'",data->god,data->god);

iceb_sql_zapis(strsql,0,0,data->window);

iceb_pbar(data->bar,kolstr,++kolstr1);    

sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Pltt");

if(metkazap == 1)
  iceb_sql_zapis(strsql,0,0,data->window);
else
  iceb_sql_readkey(strsql,data->window);

iceb_pbar(data->bar,kolstr,++kolstr1);    

sprintf(strsql,"%s\n",gettext("Удаление комментариев к платёжным требованиям"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"delete from Plttz where datd >= '%d-01-01' and \
datd <= '%d-12-31'",data->god,data->god);

iceb_sql_zapis(strsql,0,0,data->window);

iceb_pbar(data->bar,kolstr,++kolstr1);    

sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Plttz");

if(metkazap == 1)
  iceb_sql_zapis(strsql,0,0,data->window);
else
  iceb_sql_readkey(strsql,data->window);

iceb_pbar(data->bar,kolstr,++kolstr1);    

sprintf(strsql,"%s\n",gettext("Удаление объявок"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"delete from Obqvka where datd >= '%d-01-01' and \
datd <= '%d-12-31'",data->god,data->god);

iceb_sql_zapis(strsql,0,0,data->window);

iceb_pbar(data->bar,kolstr,++kolstr1);    

sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Obqvka");

if(metkazap == 1)
 iceb_sql_zapis(strsql,0,0,data->window);
else
  iceb_sql_readkey(strsql,data->window);


iceb_pbar(data->bar,kolstr,++kolstr1);    


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
