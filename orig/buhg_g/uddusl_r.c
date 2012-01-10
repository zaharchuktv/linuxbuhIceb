/*$Id: uddusl_r.c,v 1.8 2011-02-21 07:35:57 sasa Exp $*/
/*27.11.2005	19.10.2004	Белых А.И.	uddusl_r.c

*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "buhg_g.h"

class uddusl_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  short prkk;

  iceb_u_str god;
  uddusl_r_data()
   {
    god.plus("");
   }

 };

gboolean   uddusl_r_key_press(GtkWidget *widget,GdkEventKey *event,class uddusl_r_data *data);
gint uddusl_r1(class uddusl_r_data *data);
void  uddusl_r_v_knopka(GtkWidget *widget,class uddusl_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern iceb_u_str shrift_ravnohir;
 
void uddusl_r(GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class uddusl_r_data data;

repl_s.plus(gettext("Удаление документов за год"));
repl_s.plus(gettext("Введите год"));
if(iceb_menu_vvod1(&repl_s,&data.god,5,wpredok) != 0)
 return;

sprintf(strsql,gettext("Удалить документы за %d год ? Вы уверены ?"),data.god.ravno_atoi());

if(iceb_menu_danet(strsql,2,wpredok) == 2)
 return;
 
if(iceb_parol(0,wpredok) != 0)
 return;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Копировать записи из других документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uddusl_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Копировать записи из других документов"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),450,300);

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(uddusl_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)uddusl_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uddusl_r_v_knopka(GtkWidget *widget,class uddusl_r_data *data)
{
 printf("uddusl_r_v_knopka\n");
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uddusl_r_key_press(GtkWidget *widget,GdkEventKey *event,class uddusl_r_data *data)
{
 printf("uddusl_r_key_press\n");

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

gint uddusl_r1(class uddusl_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];

sprintf(strsql,"%s %d.\n",gettext("Удаление всех документов за год"),data->god.ravno_atoi());

sprintf(strsql,"%s.\n",gettext("Удаление подтверждающих записей"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

int		metkazap=0;
SQL_str row;
SQLCURSOR cur;

strcpy(strsql,"select VERSION()");
iceb_sql_readkey(strsql,&row,&cur,data->window);

if(iceb_u_SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  

sprintf(strsql,"VERSION:%s\n",row[0]);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

int god=data->god.ravno_atoi();

sprintf(strsql,"delete from Usldokum2 where datd >= '%d-1-1' and \
datd <= '%d-12-31'",god,god);

if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
 {
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
 
int kolud=sql_rows(&bd);
int ikolud=kolud;
sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),kolud);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
if(kolud > 0)
 { 
  sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Usldokum2");
  if(metkazap == 1)
    iceb_sql_zapis(strsql,1,0,data->window);
  else
    iceb_sql_readkey(strsql,data->window);
 }


sprintf(strsql,"%s.\n",gettext("Удаление приложений к документам"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"delete from Usldokum3 where god=%d",god);

iceb_sql_zapis(strsql,1,0,data->window);

kolud=sql_rows(&bd);
ikolud+=kolud;
sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),kolud);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
if(kolud > 0)
 { 
  sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Usldokum3");
  if(metkazap == 1)
    iceb_sql_zapis(strsql,1,0,data->window);
  else
    iceb_sql_readkey(strsql,data->window);
 }


sprintf(strsql,"%s.\n",gettext("Удаление записей в документах"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"delete from Usldokum1 where datd >= '%d-1-1' and \
datd <= '%d-12-31'",god,god);

iceb_sql_zapis(strsql,1,0,data->window);

kolud=sql_rows(&bd);
ikolud+=kolud;
sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),kolud);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
if(kolud > 0)
 { 
  sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Usldokum1");
  if(metkazap == 1)
    iceb_sql_zapis(strsql,1,0,data->window);
  else
    iceb_sql_readkey(strsql,data->window);
 }

sprintf(strsql,"%s.\n",gettext("Удаление заголовков документов"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"delete from Usldokum where datd >= '%d-1-1' and \
datd <= '%d-12-31'",god,god);

iceb_sql_zapis(strsql,1,0,data->window);

kolud=sql_rows(&bd);
ikolud+=kolud;
sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),kolud);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

if(kolud > 0)
 { 
  sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Usldokum");
  if(metkazap == 1)
    iceb_sql_zapis(strsql,1,0,data->window);
  else
    iceb_sql_readkey(strsql,data->window);
 }


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

sprintf(strsql,"%s:%d",gettext("Общее количество удалённых записей"),ikolud);
iceb_menu_soob(strsql,data->window);

return(FALSE);
}
