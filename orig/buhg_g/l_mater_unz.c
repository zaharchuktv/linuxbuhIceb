/*$Id: l_mater_unz.c,v 1.13 2011-02-21 07:35:53 sasa Exp $*/
/*12.11.2008	18.05.2004	Белых А.И.	l_mater_unz.c
Удаление не используемых записей кодов материалов
*/
#include "buhg_g.h"

class mater_udnz_data
 {
  public:
  SQLCURSOR cur;
//  FILE *ff;
  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *bar;
  GtkWidget *label;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  gfloat kolstr1; //Количество прочитанных строк
  int    kolstr;  //Количество строк в курсоре
  int koludk;
  time_t vremn;
  
  //Конструктор
  mater_udnz_data()
   {
    koludk=0;
    kolstr1=0.;
//    ff=NULL;
   }
 };
gint   mater_udnz11(class mater_udnz_data *data);
void  mater_r_v_knopka(GtkWidget *widget,class mater_udnz_data *data);
gboolean   mater_r_key_press(GtkWidget *widget,GdkEventKey *event,class mater_udnz_data *data);

extern SQL_baza	bd;
extern char *name_system;
extern iceb_u_str shrift_ravnohir;

void  mater_udnz(GtkWidget *wpredok)
{
char strsql[512];
mater_udnz_data data;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
//gtk_window_set_default_size (GTK_WINDOW(data.window),400,300);

if(wpredok != NULL)
 {
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

sprintf(strsql,"%s %s",name_system,gettext("Удаление неиспользуемых кодов материалов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

//gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));
gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

GtkWidget *label=gtk_label_new(gettext("Удаление неиспользуемых кодов материалов"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),400,300);

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста
PangoFontDescription *font_pango=pango_font_description_from_string(shrift_ravnohir.ravno());
gtk_widget_modify_font(GTK_WIDGET(data.view),font_pango);
pango_font_description_free(font_pango);

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
//gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);

data.bar=gtk_progress_bar_new();

gtk_progress_bar_set_bar_style(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_CONTINUOUS);

gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_LEFT_TO_RIGHT);

gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна
gtk_widget_show_all(data.window);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
GtkTooltips *tooltops=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltops,data.knopka,gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(mater_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);


gtk_idle_add((GtkFunction)mater_udnz11,&data);

gtk_main();

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  mater_r_v_knopka(GtkWidget *widget,class mater_udnz_data *data)
{
 printf("mater_r_v_knopka\n");
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   mater_r_key_press(GtkWidget *widget,GdkEventKey *event,class mater_udnz_data *data)
{
 printf("mater_r_key_press\n");

switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
}
return(TRUE);
}


/**********************************/
/**********************************/
gint   mater_udnz11(class mater_udnz_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur1;
//GtkTextIter iter;

//printf("mater_udnz11 %f\n",data->kolstr1);

if(data->kolstr1 == 0)
 {
  time(&data->vremn);
  
  sprintf(strsql,"select kodm,naimat from Material order by kodm asc");

  if((data->kolstr=data->cur.make_cursor(&bd,strsql)) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

  if(data->kolstr == 0)
   return(FALSE);
 }

while(data->cur.read_cursor(&row) != 0)
 {
  iceb_u_str soob;
  iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

//  iceb_printw(soob.ravno_toutf(),data->buffer,data->view);

  if(mater_pvu(0,row[0],data->window) != 0)
    return(TRUE);

  sprintf(strsql,"%-5s %-30.30s %s\n",row[0],row[1],gettext("Удаляем"));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  sprintf(strsql,"delete from Material where kodm = %s",row[0]);
  if(iceb_sql_zapis(strsql,1,0,data->window) == ER_DBACCESS_DENIED_ERROR) //нет полномочий для выполнения этой операции
   return(FALSE);

  return(TRUE);
 }

iceb_printw_vr(data->vremn,data->buffer,data->view);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

return(FALSE);
}
