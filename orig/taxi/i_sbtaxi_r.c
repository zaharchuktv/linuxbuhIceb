/*$Id: i_sbtaxi_r.c,v 1.12 2011-02-21 07:36:20 sasa Exp $*/
/*19.05.2010	02.09.2004	Белых А.И.	i_sbtaxi_r.c
создание базы для диспетчиризации такси
*/
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <iceb_libbuh.h>

class i_sbtaxi_r_data
 {
  public:
  

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  time_t    vremn;
  int       kolstr;
  gfloat    kolstr1;  
  iceb_u_str imabaz;
  iceb_u_str host; 
  iceb_u_str parol;
  class iceb_u_str putnansi;
  class iceb_u_str kodirovka;
  class iceb_u_str kodirovka_zaprosov;
  //Конструктор  
  i_sbtaxi_r_data()
   {
    kolstr=9;
    kolstr1=0.;
   }

 };

gboolean   i_sbtaxi_r_key_press(GtkWidget *widget,GdkEventKey *event,class i_sbtaxi_r_data *data);
gint i_sbtaxi_r1(class i_sbtaxi_r_data *data);
void  i_sbtaxi_r_v_knopka(GtkWidget *widget,class i_sbtaxi_r_data *data);

extern SQL_baza bd;
extern char *name_system;

void i_sbtaxi_r(const char *imabaz,const char *host,const char *parol,const char *putnansi,
const char *kodirovka,const char *kodirovka_zaprosov,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str soob;

i_sbtaxi_r_data data;
data.imabaz.plus(imabaz);
data.host.plus(host);
data.parol.plus(parol);
data.putnansi.plus(putnansi);
data.kodirovka.plus(kodirovka);
data.kodirovka_zaprosov.plus(kodirovka_zaprosov);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,iceb_u_toutf("Создание таблиц базы данных"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(i_sbtaxi_r_key_press),&data);

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

sprintf(strsql,"%s:%s","Создание таблиц базы данных",data.imabaz.ravno());

soob.new_plus(strsql);

GtkWidget *label=gtk_label_new(soob.ravno_toutf());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);


data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),400,300);

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста
PangoFontDescription *font_pango=pango_font_description_from_string("Nimbus Mono L, Bold 10");
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(i_sbtaxi_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)i_sbtaxi_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  i_sbtaxi_r_v_knopka(GtkWidget *widget,class i_sbtaxi_r_data *data)
{
 printf("i_sbtaxi_r_v_knopka\n");
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   i_sbtaxi_r_key_press(GtkWidget *widget,GdkEventKey *event,class i_sbtaxi_r_data *data)
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

gint i_sbtaxi_r1(class i_sbtaxi_r_data *data)
{
time(&data->vremn);
char strsql[1024];
char bros[1024];
iceb_u_str repl;
//1
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

if(mysql_init(&bd) == NULL)
 {
  iceb_printw(iceb_u_toutf("Ошибка инициализации базы данных!\n"),data->buffer,data->view);

  sprintf(strsql,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  
  gtk_label_set_text(GTK_LABEL(data->label),iceb_u_toutf("Создание базы аварийно завершено"));
  return(FALSE);


 }

if(mysql_real_connect(&bd,data->host.ravno(),"root",data->parol.ravno(),"mysql",0,NULL,0) == NULL)
 { 
  iceb_printw(iceb_u_toutf("Ошибка соединения с демоном базы данных!\n"),data->buffer,data->view);

  sprintf(strsql,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  gtk_label_set_text(GTK_LABEL(data->label),iceb_u_toutf("Создание базы аварийно завершено"));
  return(FALSE);

 } 

iceb_printw(iceb_u_toutf("Создаем базу данных\n"),data->buffer,data->view);

//sprintf(strsql,"create database %s",data->imabaz.ravno());
sprintf(strsql,"create database %s DEFAULT CHARACTER SET '%s'",data->imabaz.ravno(),data->kodirovka.ravno());

if(sql_zap(&bd,strsql) != 0)
 {
  //База уже может быть создана
  sprintf(strsql,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
 }


iceb_printw(iceb_u_toutf("Открываем базу\n"),data->buffer,data->view);

if(sql_openbaz(&bd,data->imabaz.ravno(),data->host.ravno(),"root",data->parol.ravno()) != 0)
 {
  iceb_eropbaz(data->imabaz.ravno(),0,"",1);

  gtk_label_set_text(GTK_LABEL(data->label),iceb_u_toutf("Создание базы аварийно завершено"));

  gtk_widget_grab_focus(data->knopka);
  gtk_widget_show_all(data->window);
  return(FALSE);

 }
sprintf(strsql,"SET NAMES %s",data->kodirovka_zaprosov.ravno());
iceb_sql_zapis(strsql,1,0,data->window);
//iceb_start_rf();

//2,3
#include "gr_klient.h"

//4
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создание таблицы водителей Taxivod !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Taxivod (\
kod     char(20) not null primary key,\
fio     varchar(60) not null,\
adres   varchar(100) not null,\
telef	varchar(40) not null,\
gosn    varchar(20) not null,\
sm      smallint not null,\
ktoi    smallint unsigned not null,\
vrem    int unsigned not null)");

/*
0 kod	код
1 fio     фамилия
2 adres   адрес
3 telef	телефон
4 gosn    гос. номер машины
5 sm      0-на смене 1-не на смене
6 ktoi    кто записал
7 vrem    время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
//  repl.plus_ps(strsql);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана.\n\n"),data->buffer,data->view);

//5
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создание таблицы кодов завершения заказов Taxikzz !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Taxikzz (\
kod     smallint not null primary key,\
naik    varchar(60) not null,\
ktoi    smallint unsigned not null,\
vrem    int unsigned not null)");

/*
0 kod	код
1 naik  наименование кода
2 ktoi  кто записал
3 vrem  время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
//  repl.plus_ps(strsql);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана.\n\n"),data->buffer,data->view);

//6
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создание таблицы заказов Taxizak !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Taxizak (\
kz      smallint not null,\
kodk    varchar(20) not null,\
kv      varchar(20) not null,\
fio     varchar(60) not null,\
telef   varchar(40) not null,\
kolp    smallint not null,\
datvz   DATE not null,\
vremvz  TIME not null,\
datz    DATE not null,\
vremz   TIME not null,\
adreso  varchar(100) not null,\
adresk  varchar(100) not null,\
koment  varchar(250) not null,\
suma    float not null,\
vremzz  int unsigned not null,\
ktoi    smallint unsigned not null,\
vrem    int unsigned not null,\
zvuk    varchar(40) not null,\
index(kz,datvz,vremvz),\
index(datvz,vremvz),\
index(kodk,datvz),\
index(telef,datvz))");

/*
0  kz      код завершения
1  kodk    код клиента
2  kv      код водителя
3  fio     фамилия заказчика
4  telef   телефон клиента
5  kolp    количество пассажиров
6  datvz   дата когда нужно выполнить заказ
7  vremvz  время когда нужно выполнить заказ
8  datz    дата заказа
9  vremz   время заказа
10 adreso  адрес отуда забрать клиента
11 adresk  адрес куда отвезти
12 koment  коментарий
13 suma    сума
14 vremzz  время завершения заказа
15 ktoi    кто записал
16 vrem    время записи
17 zvuk    имя файла с записью разговора
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
//  repl.plus_ps(strsql);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана.\n\n"),data->buffer,data->view);


//7
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создание таблицы телефонов Taxitel !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Taxitel (\
tel     varchar(20) not null primary key,\
adres   varchar(100) not null,\
ktoi    smallint unsigned not null,\
vrem    int unsigned not null)");

/*
0 tel	номер телефона
1 adres адрес
2 ktoi  кто записал
3 vrem  время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
//  repl.plus_ps(strsql);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана.\n\n"),data->buffer,data->view);

//8
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создание таблицы сальдо по клиентам Taxiklsal !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Taxiklsal (\
god     smallint not null,\
kod     char(20) not null,\
saldo   double(10,2) not null,\
ktoi    smallint unsigned not null,\
vrem    int unsigned not null,\
unique(god,kod))");

/*
0 god	год 
1 kod   код клиента
2 saldo сальдо по клиенту
3 ktoi  кто записал
4 vrem  время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
//  repl.plus_ps(strsql);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*Загружаем настроечные файлы в базу данных*/
iceb_alxzag(data->putnansi.ravno(),0,data->view,data->buffer,data->window);


sql_closebaz(&bd);


iceb_printw_vr(data->vremn,data->buffer,data->view);


gtk_label_set_text(GTK_LABEL(data->label),iceb_u_toutf("Создание базы завершено"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);
return(FALSE);
}
