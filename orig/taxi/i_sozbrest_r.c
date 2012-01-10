/*$Id: i_sozbrest_r.c,v 1.39 2011-02-21 07:36:20 sasa Exp $*/
/*16.05.2010	02.09.2004	Белых А.И.	i_sozbrest_r.c
Создание таблиц базы данных
*/
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <iceb_libbuh.h>

class i_sozbrest_r_data
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
  class iceb_u_str kodir_zap;
  //Конструктор  
  i_sozbrest_r_data()
   {
    kolstr=11;
    kolstr1=0.;
   }

 };

gboolean   i_sozbrest_r_key_press(GtkWidget *widget,GdkEventKey *event,class i_sozbrest_r_data *data);
gint i_sozbrest_r1(class i_sozbrest_r_data *data);
void  i_sozbrest_r_v_knopka(GtkWidget *widget,class i_sozbrest_r_data *data);

extern SQL_baza bd;
extern char *name_system;

void i_sozbrest_r(const char *imabaz,const char *host,const char *parol,const char *putnansi,const char *kodir_zap,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str soob;

i_sozbrest_r_data data;
data.imabaz.plus(imabaz);
data.host.plus(host);
data.parol.plus(parol);
data.putnansi.plus(putnansi);
data.kodir_zap.plus(kodir_zap);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,iceb_u_toutf("Создание таблиц базы данных"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(i_sozbrest_r_key_press),&data);

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

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
GtkTooltips *tooltops=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltops,data.knopka,gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(i_sozbrest_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);
gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна


gtk_idle_add((GtkFunction)i_sozbrest_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  i_sozbrest_r_v_knopka(GtkWidget *widget,class i_sozbrest_r_data *data)
{
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   i_sozbrest_r_key_press(GtkWidget *widget,GdkEventKey *event,class i_sozbrest_r_data *data)
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

gint i_sozbrest_r1(class i_sozbrest_r_data *data)
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
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
  return(FALSE);

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
//iceb_start_rf();
sprintf(strsql,"SET NAMES %s",data->kodir_zap.ravno());
iceb_sql_zapis(strsql,1,0,data->window);
//2
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создание таблицы подразделений Restpod !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Restpod (\
kod     int not null primary key,\
naik    varchar(100) not null,\
ktoi    smallint unsigned not null,\
vrem    int unsigned not null)");

/*
0 kod     код подразделения
1 naik    наименование подразделения
2 ktoi    кто записал
3 vrem    время записи
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

//3
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создание таблицы шапок документов Restdok!\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Restdok (\
god     smallint not null,\
nomd    varchar(10) not null,\
datd    DATE not null,\
mo      tinyint not null,\
nomst   varchar(10) not null,\
kodkl   varchar(20) not null,\
fio     varchar(60) not null,\
koment  varchar(100) not null,\
ms      tinyint not null,\
ktoi    smallint unsigned not null,\
vrem    int unsigned not null,\
podr    int not null,\
vremz   int unsigned not null,\
vremo   int unsigned not null,\
ps      float not null,\
unique(god,nomd),\
index(mo,podr),\
index(datd,podr),\
index(kodkl,mo),\
index(kodkl,datd))");

/*
0 god     год
1 nomd    номер документа
2 datd    дата документа
3 mo      0-заказ не оплачен 1-оплачен
4 nomst   номер стола
5 kodkl   код клиента
6 fio     фамилия имя отчество
7 koment  коментарий
8 ms      0-не списано 1-списано
9 ktoi    кто записал
10 vrem    время записи
11 podr   Код подразделения
12 vremz  Время предварительного заказа
13 vremo  время оплаты заказа
14 ps     процент скидки на клиента
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

//4
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создание таблицы записей документов Restdok1 !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Restdok1 (\
datd    DATE not null,\
nomd    varchar(10) not null,\
skl     smallint not null,\
mz      tinyint not null,\
kodz    int not null,\
ei      varchar(10) not null,\
kolih   double(12,4) not null,\
cena    double(12,6) not null,\
koment  varchar(100) not null,\
ms      tinyint not null,\
ktoi    smallint unsigned not null,\
vrem    int unsigned not null,\
podr    int not null,\
nz      smallint not null,\
mr      tinyint not null,\
nds     tinyint not null,\
ps      float not null,\
unique(datd,nomd,skl,mz,kodz,nz),\
index(skl,ms,mz),\
index(kodz,skl,mz,ms))");

/*
0  datd    дата документа
1  nomd    номер документа
2  skl     склад
3  mz      0-товар 1-услуга
4  kodz    код записи
5  ei      единица измерения
6  kolih   количество
7  cena    цена
8  koment  коментарий
9  ms      0-не списано 1-списано
10 ktoi    кто записал
11 vrem    время записи
12 podr    Код подразделения
13 nz      номер записи 
14 mr      метка распечатки 0-запись не распечатана 1-распечатана
15 nds     0-цена c НДС 1-без НДС
16 ps      процент скидки
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

//5,6
#include "gr_klient.h"



//7
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создание таблицы расчётов по клиентам и кассам Restkas !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Restkas (\
kodkl   varchar(10) not null,\
nk      int not null,\
datz    DATE not null,\
vrem    TIME not null,\
kodz    tinyint not null,\
suma    double(10,2) not null,\
nomd    varchar(10) not null,\
datd    DATE not null,\
podr    int not null,\
ktoz    smallint not null,\
koment  varchar(100) not null,\
index(datz,kodkl),\
index(nk,datz),\
index(kodkl,datz),\
index(nk,kodz,datz))");
/*
0  kodkl код клиента (для сальдо по кассе код клиента не вводится)
1  nk    номер кассы
2  datz    дата записи
3  vremz  время записи 
4  kodz   0-расчеты по кассе (внесени/изъятие денег) как по клиенту так и в целом по кассе 
          1-внесение (изъятие) безналичных денег по клиенту
          2-списание потраченных сумм с клиента (для определения сальдо по клиенту)
5  suma    сумма
6  nomd    номер документа (000 для записи с сальдо)
7  datd    дата документа
8  podr    подразделение
9  ktoz    код оператора
10  koment  коментарий
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

iceb_printw(iceb_u_toutf("Создание таблицы временных интервалов Restvi !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Restvi (\
dv      DATETIME not null,\
kp      int not null,\
cena    float(7,2) not null,\
grup    varchar(100) not null,\
koment  varchar(100) not null,\
ktoi    smallint unsigned not null,\
vrem    int unsigned not null,\
cdg     float(7,2) not null,\
unique(dv),\
index(kp,dv))");
/*
 0 dv     - дата+время
 1 kp     - код подразделения
 2 cena   - цена входа
 3 grup   - группы которым бесплатно
 4 koment - коментарий
 5 ktoi   - кто записал
 6 vrem   - время записи
 7 cdg    - цена входа для женщины
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана.\n\n"),data->buffer,data->view);

//9
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создание таблицы регистрации входов-выходов Restvv !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Restvv (\
dv DATETIME not null,\
kk varchar(30) not null,\
kp int not null,\
tp tinyint not null,\
index(dv),\
index(kk,dv))");
/*
 0 dv     - дата+время
 1 kk     - код карточки
 2 kp     - код подразделения
 3 tp     - 1-вход 2-выход -1-отключение турникета
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана.\n\n"),data->buffer,data->view);
//10
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создание таблицы списка турникетов Resst !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Resst (\
nm     int not null primary key,\
podr   int not null,\
koment varchar(60) not null,\
ktoi   smallint unsigned not null,\
vrem   int unsigned not null)");
/*
 0 nm     номер турникета
 1 podr   код подразделения где установлен турникет
 2 koment коментарий
 3 ktoi   кто записал
 4 vrem   время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана.\n\n"),data->buffer,data->view);

/*Загружаем настроечные файлы в базу данных*/
iceb_alxzag(data->putnansi.ravno(),0,data->view,data->buffer,data->window);

sql_closebaz(&bd);


iceb_printw_vr(data->vremn,data->buffer,data->view);


gtk_label_set_text(GTK_LABEL(data->label),iceb_u_toutf("Создание базы завершено"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

return(FALSE);

}

