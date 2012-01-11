/*$Id: admin_makebaz_r.c,v 1.30 2011-08-29 07:13:43 sasa Exp $*/
/*23.12.2010	02.09.2004	Белых А.И.	admin_makebaz_r.c
Создание таблиц базы данных
*/
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <iceb_libbuh.h>

class admin_makebaz_r_data
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
  class iceb_u_str imabaz;
  class iceb_u_str host; 
  class iceb_u_str parol;
  class iceb_u_str kodirovka;
  class iceb_u_str fromnsi;
  class iceb_u_str fromdoc;
  //Конструктор  
  admin_makebaz_r_data()
   {
    kolstr=126;
    kolstr1=0.;
   }

 };

gboolean   admin_makebaz_r_key_press(GtkWidget *widget,GdkEventKey *event,class admin_makebaz_r_data *data);
gint admin_makebaz_r1(class admin_makebaz_r_data *data);
void  admin_makebaz_r_v_knopka(GtkWidget *widget,class admin_makebaz_r_data *data);

int admin_cp_alx(const char *from_alx,const char *to_alx,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok);
int alxinw(const char *put_alx,int metka,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok);
void docinw(const char *put_na_doc,GtkWidget *wpredok);

extern SQL_baza bd;
extern char *name_system;

void admin_makebaz_r(const char *imabaz,const char *host,const char *parol,const char *kodirovka,
const char *fromnsi,
const char *fromdoc,
GtkWidget *wpredok)
{
char strsql[1024];
iceb_u_str soob;

admin_makebaz_r_data data;
data.imabaz.plus(imabaz);
data.host.plus(host);
data.parol.plus(parol);
data.kodirovka.plus(kodirovka);
data.fromnsi.new_plus(fromnsi);
data.fromdoc.new_plus(fromdoc);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Разметка таблиц базы данных"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(admin_makebaz_r_key_press),&data);

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

sprintf(strsql,"%s:%s",gettext("Разметка таблиц базы данных"),data.imabaz.ravno());

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(admin_makebaz_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);
gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна


gtk_idle_add((GtkFunction)admin_makebaz_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  admin_makebaz_r_v_knopka(GtkWidget *widget,class admin_makebaz_r_data *data)
{
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   admin_makebaz_r_key_press(GtkWidget *widget,GdkEventKey *event,class admin_makebaz_r_data *data)
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

gint admin_makebaz_r1(class admin_makebaz_r_data *data)
{
time(&data->vremn);
char strsql[4096];
char bros[1024];
iceb_u_str repl;
SQL_str row;
class SQLCURSOR cur;
int metka_ver=0;



iceb_sql_readkey("select VERSION()",&row,&cur,data->window);
if(atof(row[0]) > 4)
  metka_ver=1;  

sprintf(strsql,"Версия базы данных - %s\n",row[0]);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


iceb_printw(iceb_u_toutf("Создаем базу данных\n"),data->buffer,data->view);

if(metka_ver == 0)
  sprintf(strsql,"create database %s",data->imabaz.ravno());
else
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
iceb_start_rf();


/*1*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создание таблицы плана счетов Plansh !\n"),data->buffer,data->view);


memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Plansh (\
ns	char(20) not null default '' primary key,\
nais    varchar(60) not null default '',\
tips    smallint not null default 0,\
vids	smallint not null default 0,\
ktoi    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
saldo   smallint not null default 0,\
stat    smallint not null default 0,\
val     smallint not null default 0,\
ksb     smallint not null default 0)");

/*
0 ns	Номер счета
1 nais   Наименование счета
2 tips   0-активный 1-пассивный 2-активно-пассивный
3 vids   0-счет 1-субсчет
4 ktoi  Кто записал
5 vrem  Время записи
6 saldo 0-свернутое 3-развернутое
7 stat  0-балансовый счет 1-вне балансовый счет
8 val   0-национальная валюта или номер валюты из справочника валют (пока не задействовано)
9 ksb   код суббаланса
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  iceb_printw(bros,data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*2*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы суббалансов Glksubbal !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Glksubbal (\
kod    smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");

/*
0 kod   код cуббаланса
1 naik  наименование суббаланса
2 ktoz  кто записал
3 vrem  время записи
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);




/*3*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создание таблицы групп конрагентов Gkont !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Gkont (\
kod    smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");

/*
0 kod   код группы
1 naik  наименование группы контрагентов
2 ktoz  кто записал
3 vrem  время записи
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*4*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создание таблицы контрагентов Kontragent !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kontragent (\
kodkon char(20) not NULL default '' primary key,\
naikon varchar(80) not null default '',\
naiban varchar(80) not null default '',\
adres  varchar(100) not null default '',\
adresb varchar(100) not null default '',\
kod    varchar(20) not null default '',\
mfo    varchar(20) not null default '',\
nomsh  varchar(30) not null default '',\
innn   varchar(20) not null default '',\
nspnds varchar(20) not null default '',\
telef  varchar(80) not null default '',\
grup   varchar(10) not null default '',\
ktoz   smallint unsigned not null default 0,\
vrem   int unsigned not null default 0,\
dogov  varchar(80) not null default '',\
regnom varchar(30) not null default '',\
pnaim  varchar(100) not null default '',\
rsnds  varchar(30) not null default '',\
unique(naikon),\
index(kod))");

/*
0  kodkon Код контрагента
1  naikon Наименование контрагента
2  naiban Наименование банка
3  adres  Адрес контрагента
4  adresb Адрес банка
5  kod    Код ЕГРПОУ
6  mfo    МФО
7  nomsh  Номер счета
8  innn   Индивидуальный налоговый номер
9  nspnds Номер свидетельства плательщика НДС
10 telef  Телефон
11 grup   Группа
12 ktoz   Кто записал
13 vrem   Время записи
14 dogov  договор
15 regnom регистрационный номер частного предпринимателя
16 pnaim  полное наименование контрагента
17 rsnds  расчетный счет для ндс
*/


if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*5*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создание таблицы Kontragent1 !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kontragent1 (\
kodkon varchar(20) not null default '',\
nomz   smallint unsigned not null default 0,\
zapis  varchar(255) not null default '',\
unique(kodkon,nomz))");

/*
0 kodkon  код контрагента
1 nomz    номер записи
3 zapis   содержимое записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создание таблицы Kontragent2 !\n"),data->buffer,data->view);


memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kontragent2 (\
nz int unsigned PRIMARY KEY AUTO_INCREMENT NOT NULL ,\
kodkon varchar(32) not null default '',\
datd DATE NOT NULL DEFAULT '0000-00-00',\
nomd VARCHAR (64) NOT NULL DEFAULT '',\
vidd VARCHAR (255) NOT NULL DEFAULT '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
index(kodkon))");

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);
/*
0 nz     - уникальный номер записи
1 kodkon - код контрагента
2 datd   - дата документа
3 nomd   - номер документа
4 vidd   - вид документа
5 ktoz   - кто записал
6 vrem   - время записи
*/

/*6*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создание таблицы списка контрагентов для счетов Skontr!\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Skontr (\
ns	char(20) not null default '' references Plansh,\
kodkon  char(20) not null default '' references Kontragent,\
ktoi    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
unique(ns,kodkon))");

/*
0 ns	номер счета
1 kodkon  Код контрагента
2 ktoi    кто записал
3 vrem    время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*7*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создание таблицы сальдо для счетов Saldo !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Saldo \(\
kkk 	char(2) not null default '',\
gs      smallint not null default 0,\
ns	char(20) not null default '' references Plansh,\
kodkon  char(20) not null default '' references Kontragent,\
deb     double(14,2) not null default 0,\
kre     double(14,2) not null default 0,\
ktoi    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
index(kkk,gs,ns))");
/*
0 kkk     0- счета обычные 1- с развернутым сальдо
1 gs      Год сальдо
2 ns	Номер счета
3 kodkon  Код контрагента
4 deb     Дебет
5 kre     Кредит
6 ktoi    Кто записал
7 vrem    Время записи
*/


if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*8*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы проводок Prov !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Prov (\
val smallint not null default 0,\
datp DATE not null default '0000-00-00',\
sh char(20) not null default '',\
shk char(20) not null default '',\
kodkon char(20) not null default '',\
kto char(8) not null default '',\
nomd char(20) not null default '',\
pod int not null default 0,\
oper char(8) not null default '',\
deb double(14,2) not null default 0,\
kre double(14,2) not null default 0,\
ktoi smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
komen varchar(100) not null default '',\
datd DATE not null default '0000-00-00',\
tz tinyint not null default 0,\
kekv smallint not null default 0,\
index(datp,sh,shk),\
index(kto,pod,nomd,datd),\
index(vrem))");

/*
0  val    0-балансовые проводки -1 не балансовые
1  datp   дата проводки
2  sh     счет
3  shk    счет корреспондент
4  kodkon код контрагента для счетов с развернутым сальдо
5  kto    из какой подсистемы сделана проводка
6  nomd   Номер документа к которому относится проводка
7  pod    подразделение
8  oper   код операции
9  deb    сумма по дебету
10 kre    сумма по кредиту
11 ktoi   номер логина того кто записал проводку
12 vrem   время записи проводки
13 komen  комментарий
14 datd   дата документа к которому относится проводка
15 tz     1-приходный документ 2-расходеый 0-не определено
16 kekv   код экономической классификации расходов
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*9*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня групп материалов Grup !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Grup \
(kod    smallint not null default 0 primary key,\
naik    varchar(80) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");
/*
0 kod   код группы
1 naik  наименование группы
2 ktoz  кто записал
3 vrem  время записи
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*10*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня единиц измерения Edizmer !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Edizmer \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");
/*
0 kod    Код единицы измерения
1 naik   Наименование единицы измерения
2 ktoz   Кто записал
3 vrem   Время записи
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*11*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня материалов Material !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Material (\
kodm int not NULL default 0 primary key,\
kodgr smallint not null default 0 references Grup,\
naimat varchar(100) not null default '',\
strihkod varchar(50) not null default '',\
ei char(8) not null default '' references Edizmer,\
kriost double not null default 0,\
cenapr double (15,6) not null default 0,\
krat double not null default 0,\
fasv double not null default 0,\
kodt int not null default 0,\
ktoz smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
nds  real not null default 0,\
msp smallint not null default 0,\
artikul varchar(20) not null default '',\
unique (naimat),\
index (strihkod))");

/*

0  kodm      Код материалла
1  kodgr     Код группы материалла
2  naimat    Наименование материалла
3  strihkod  Штрих-код
4  ei        Единица измерения
5  kriost    Критический остаток
6  cenapr    Цена реализации
7  krat      Кратность
8  fasv      Фасовка
9  kodt      Код тары
10 ktoz      Кто записал
11 vrem      Время записи
12 nds       1-цена введена без НДС 0-цена с НДС
13 msp       Метка скоропортящегося материалла
14 artikul   артикул
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*12*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня операций приходов Prihod !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Prihod \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
vido    smallint not null default 0,\
prov    smallint not null default 0)");

/*
0 kod  код операции
1 naik наименование операции
2 ktoz кто записал
3 vrem время записи
4 vido 0 - внешняя 1-внутренняя 2-изменения стоимости
5 prov 0 проводки нужно делать 1 - не нужно
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*13*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня операций расходов Rashod !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Rashod \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
vido    smallint not null default 0,\
sheta   varchar(100) not null default '',\
prov    smallint not null default 0)");

/*
0 kod  код операции
1 naik наименование операции
2 ktoz кто записал
3 vrem время записи
4 vido  видоперации 0-внешняя 1-внутренняя 2-изменения стоимости
5 sheta возможные счета учета для операции
6 prov 0 проводки нужно делать 1 - не нужно
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*14*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня форм оплаты Foroplat !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Foroplat \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");

/*
0 kod    Код формы оплаты
1 naik   Наименование формы оплаты
2 ktoz   Кто записал
3 vrem   Время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*15*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня складов Sklad !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Sklad \
(kod    smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
fmol    varchar(30) not null default '',\
dolg    varchar(30) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
mi tinyint not null)");

/*
0 kod     Код склада
1 naik    Наименование склада
2 fmol    Фамилия материальноответственного лица
3 dolg    Должность
4 ktoz 	  кто записал
5 vrem    время записи
6 mi      метка использования склада 0-используется 1-нет
*/


if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*16*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы розничной торговли Roznica !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Roznica (\
datrp  DATE not null default '0000-00-00',\
kassa  smallint not null default 0,\
nomer  smallint not null default 0,\
metka  smallint not null default 0,\
kodm   int not null default 0 references Material (kodm),\
ei char(8) not null default '' references Edizmer (kod),\
kolih  double (14,4) not null default 0,\
cena   double (9,2) not null default 0,\
nds    float(2) not null default 0,\
sklad  smallint not null default 0,\
ktoi   smallint unsigned not null default 0,\
vrem   int unsigned not null default 0,\
unique (datrp,kassa,nomer,metka,kodm),\
index (metka,kodm,sklad)\
)");
/*
0 datrp  дата реализации
1 kassa  номер рабочего места реализации
2 nomer  Номер продажи
3 metka  1-товар 2- услуга
4 kodm   код товара или материалла
5 ei     единица измерения
6 kolih  количество
7 cena   цена
8 nds    1-цена введена без НДС 0-цена с НДС
9 sklad  отдел
10 ktoi   кто продал
11 vrem   время продажи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*17*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы заголовков документов Dokummat !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Dokummat (\
tip    smallint not null default 0,\
datd   DATE not null default '0000-00-00',\
sklad  smallint not null default 0 references Sklad (kod),\
kontr  char(20) not null default '' references Kontragent (kontr),\
nomd   char(16) not null default '',\
nomnn  char(16) not null default '',\
kodop  char(8) not null default '',\
pod    smallint not null default 0,\
pro    smallint not null default 0,\
ktoi   smallint unsigned not null default 0,\
vrem   int unsigned not null default 0,\
nomon  char(16) not null default '',\
datpnn DATE not null default '0000-00-00',\
pn float(5,2) not null default 0,\
mo tinyint NOT NULL default 0,\
unique (datd,sklad,nomd),\
index (nomnn)\
)");

/*
0 tip    1-приход 2-расход
1 datd   дата
2 sklad  код склада
3 kontr  код контрагента
4 nomd   номер документа
5 nomnn  номер налоговой накладной
6 kodop  код операции
7 pod    0-не подтверждена 1-подтверждена
8 pro    0-не выполнены проводки 1-выполнены
9 ktoi   кто записал
10 vrem   мремя записи
11 nomon  Номер парной накладной при внутреннем перемещении или номер документа поставщика для приходного документа
12 datpnn дата получения налоговой накладной
13 pn     процент НДС действовавший на момент создания документа
14 mo     метка оплаты 0-неоплачено 1-оплачено
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*18*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей к документу Dokummat1 !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Dokummat1 (\
datd DATE not null,\
sklad  smallint not null default 0 references Sklad (kod),\
nomd   char(16) not null default '',\
nomkar int not null default 0,\
kodm   int not null default 0 references Material (kodm),\
kolih  double(16,6) not null default 0,\
cena   double(16,6) not null default 0,\
ei     varchar(8) not null default '' references Edizmer (kod),\
voztar smallint not null default 0,\
nds    float(2) not null default 0,\
mnds   smallint not null default 0,\
ktoi   smallint unsigned not null default 0,\
vrem   int unsigned not null default 0,\
tipz   smallint not null default 0,\
nomkarp int not null default 0,\
shet   varchar(12) not null default '',\
dnaim  varchar(70) not null default '',\
nomz   varchar(20) not null default '',\
index (sklad,nomd,kodm,nomkar),\
index (sklad,nomkar),\
index (kodm,sklad),\
index (datd,sklad,nomd))");

/*
0  datd   Дата документа
1  sklad  Склад
2  nomd   Номер документа
3  nomkar Номер карточки
4  kodm   Код материалла
5  kolih  Количество
6  cena   Цена
7  ei     Единица измерения
8  voztar 1-Метка возвратной тары
9  nds    НДС
10 mnds   0- цена без НДС 1-цена с НДС в том числе
11 ktoi   Кто записал
12 vrem   Время записи
13 tipz   1-приход 2-расход
14 nomkarp номер карточки парного документа
15 shet   счет списания для расходного документа
16 dnaim  Дополнительное наименование материалла
17 nomz   Номер заказа
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*19*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы дополнительных записей к документу Dokummat2 !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Dokummat2 (\
god    smallint not null default 0,\
sklad  smallint not null default 0 references Sklad (kod),\
nomd   char(16) not null default '',\
nomerz smallint not null default 0,\
sodz   varchar(80) not null default '',\
unique (god,nomd,sklad,nomerz)\
)");
/*
0 god    
1 sklad
2 nomd
3 nomerz=1 - доверенность#дата выдачи
         2 - через кого
         3 - основание для расходного документа / комментарий для приходного документа
         4 - документ заблокирован для программы xpro если есть эта запись
         5 - login распечатавшего чек
         6 - Сумма НДС для приходного документа введенная вручную
         7 - условие продажи
         8 - код формы оплаты
         9 - дата оплаты или отсрочки платежа
        10 - государственный номер машины
        11 - НДС
             0 20% НДС.
             1 0% НДС реализация на територии Украины.
             2 0% НДС экспорт.
             3 0% Освобождение от НДС статья 5.
        12 - Кто заблокировал документ
        13 - Сумма корректировки по документа +/-
        14 - дата накладной поставщика
        15 - Номер путевого листа
        16 - Марка автомобиля
        17 - Марка и номер прицепа
        18 - Перевозчик груза
        19 - Заказчик (плательщик)
        20 - Фамилия водителя
        21 - Пункт загрузки
        22 - Пункт разгрузки
        23 - Акцизный сбор        
        24 - Лицензия на алкоголь|период действия лиценции для контрагента 00 в формате дд.мм.гг-дд.мм.гг
        25 - Лицензия на алкоголь|период действия лиценции для контрагента получателя в формате дд.мм.гг-дд.мм.гг
4 sodz  Содержимое записи
 */

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*20*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы услуг для документа Dokummat3 !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Dokummat3 (\
datd  DATE not null,\
sklad  smallint not null default 0 references Sklad (kod),\
nomd   char(16) not null default '',\
ei     char(12) not null default '',\
kolih double not null default 0,\
cena double not null default 0,\
nds double not null default 0,\
naimu varchar(100) not null default '',\
ktoi smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
ku int not null,\
nz int unsigned PRIMARY KEY AUTO_INCREMENT NOT NULL,\
index(datd,sklad,nomd))");

/*
 0 datd  дата документа
 1 sklad код склада
 2 nomd  номер документа
 3 ei    единица измерения
 4 kolih количество
 5 cena  цена/сумма
 6 nds   НДС (процент)
 7 naimu наименование услуги
 8 ktoi  кто записал
 9 vrem  время записи
10 ku    код услуги
11 nz    уникальный номер записи устанавливаемый базой данных автоматически
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*21*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы оплат по документам Opldok !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Opldok (\
ps    tinyint not null default 0,\
datd  DATE not null default '0000-00-00',\
pd    int not null default 0,\
nomd  char(20) not null default '',\
dato  DATE NOT NULL default '0000-00-00',\
shet  char(20) not null default '',\
suma  double(10,2) not null default 0,\
kekv  smallint not null default 0,\
nomp  char(20) not null default '',\
kom   varchar(100) not null default '',\
ktoi smallint unsigned not null default 0,\
vrem int unsigned not null,\
index(ps,datd,pd,nomd))");

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*
0 ps    подсистема 1-материальный учёт 2- учёт услуг 3-учёт основных средств
1 datd  дата документа
2 pd    подразделение (для Учёта основных средств pd=0 так как нумерация документов не зависит от подразделения)
3 nomd  номер документа
4 dato  дата оплаты
5 shet  счёт 
6 suma  сумма закрытая на документ
7 kekv  код экономической классификации затрат
8 nomp  номер платёжного поручения
9 kom   комментарий
10 ktoi  кто записал
11 vrem  время записи
*/

/*22*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы карточек материалов Kart !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kart (\
sklad  smallint not null default 0 references Sklad (kod),\
nomk    int not null default 0,\
kodm   int not null default 0 references Material (kodm),\
mnds    smallint not null default 0,\
ei char(8) not null default '' references Edizmer (kod),\
shetu   char(12) not null default '' references Plansh (ns),\
cena    double(15,6) not null default 0,\
cenap   double(15,6) not null default 0,\
krat    decimal(10,2) not null default 0,\
nds	float(2) not null default 0,\
fas     float(2) not null default 0,\
kodt    int not null default 0,\
ktoi    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
datv    DATE not null default '0000-00-00',\
innom   varchar(16) not null default '',\
rnd     varchar(20) not null default '',\
nomz    varchar(20) not null default '',\
dat_god DATE NOT NULL default '0000-00-00',\
unique (sklad,nomk),\
index (kodm,sklad,shetu,cena,ei,nds,cenap,krat,mnds),\
index(innom),\
index(nomz)\
)");

/*
0  sklad    Код склада
1  nomk     Номер карточки
2  kodm     Код материалла
3  mnds     0-цена c НДС 1-цена без НДС
4  ei       Единица измерения
5  shetu    Счет учета
6  cena     Цена учета
7  cenap    Цена продажи для розничной торговли
8  krat     Кратность
9  nds      Н.Д.С
10 fas      Фасовка
11 kodt     Код тары
12 ktoi     Кто записал
13 vrem     Время записи
14 datv     Дата ввода в эксплуатацию (для малоценки) / Дата регистрации препарата для медикаментов
15 innom    Инвентарный номер (для малоценки) / Серия для медикаментов
16 rnd      Регистрационный номер документа для мед. препарата
17 nomz     Номер заказа
18 dat_god  Конечная дата годности товара
*/


if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*23*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей в карточках Zkart !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zkart (\
sklad  smallint not null default 0 references Sklad (kod),\
nomk    int not null default 0,\
nomd    char(16) not null default '',\
datdp	DATE not null default '0000-00-00',\
datd    DATE not null default '0000-00-00',\
tipz	smallint not null default 0,\
kolih   double(16,6) not null default 0,\
cena    double(16,6) not null default 0,\
ktoi    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
unique (sklad,nomk,nomd,datdp), \
index (datd,nomd,sklad,nomk),\
index (datdp)\
)");

/*
0 sklad	Склад
1 nomk 	Номер карточки
2 nomd 	Номер документа
3 datdp	дата получения-отпуска (дата подтверждения докумена)
4 datd  Дата выписки документа
5 tipz 	1 - приход 2 - расход
6 kolih	Количество
7 cena 	Цена
8 ktoi 	Кто записал
9 vrem 	Время записи
*/


if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/********************************/
/* Таблицы для зароботной платы */
/*********************************/

/*24*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы Kartb !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kartb (\
tabn int not null primary key,\
fio  varchar(60) not null default '',\
dolg varchar(60) not null default '',\
zvan smallint not null default 0,\
podr int not null default 0,\
kateg smallint not null default 0,\
datn DATE not null default '0000-00-00',\
datk DATE not null default '0000-00-00',\
shet char(12) not null default '' references Plansh (ns),\
sovm smallint not null default 0,\
inn  char(15) not null default '',\
adres varchar(100) not null default '',\
nomp varchar(30) not null default '',\
vidan varchar(100) not null default '',\
telef varchar(50) not null default '',\
kodg smallint not null default 0,\
harac varchar(50) not null default '',\
lgoti varchar(30) not null default '',\
bankshet varchar(30) not null default '',\
datvd DATE not null default '0000-00-00',\
tipkk varchar(30) not null default '',\
denrog DATE not null default '0000-00-00',\
pl tinyint not null default 0,\
dppz DATE not null default '0000-00-00',\
kb int not null default 0,\
kss varchar(16) not null default '',\
index(fio),\
index(inn))");

/*
0  tabn  табельный номер
1  fio   фамилия имя отчество
2  dolg  должность
3  zvan  звание
4  podr  подразделение
5  kateg категория
6  datn  дата начала работы
7  datk  дата конца работы
8  shet  счет учета
9  sovm  метка совместителя 0-не совм. 1-совместитель
10 inn   индивидуальный налоговый номер
11 adres адрес
12 nomp  номер паспорта
13 vidan кем выдан
14 telef телефон
15 kodg  код города налоговой инспекции
16 harac характер работы
17 lgoti код льгот для формы 8ДР
18 bankshet банковский счет на который перечисляется зарплата
19 datvd дата выдачи документа
20 tipkk тип кредитной карты
21 denrog день рождения
22 pl     0-мужчина 1-женщина
23 dppz   дата последнего повышения зарплаты
24 kb    код банка для перечисления зарплаты на карт счёт
25 kss   Код основания для учёта спец.стажа
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*25*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы Kartb1 !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kartb1 (\
tabn int not null default 0,\
nomz   smallint unsigned not null default 0,\
zapis  varchar(255) not null default '',\
unique(tabn,nomz))");

/*
0 tabn табельный номер
1 nomz    номер записи
2 zapis   содержимое записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы списка картсчетов работников Zarkh !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zarkh (\
tn int not null default 0,\
kb  int not null default 0,\
nks varchar(40) not null default '',\
ktoz smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
unique(tn,kb))");
/****
0 tn - табельный номер
1 kb - код банка
2 nks - номер карт-счёта
3 ktoz кто записал
4 vrem время записи
****/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);



/*26*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы начислений Nash !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Nash (\
kod	smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
shet    char(12)  not null default '' references Plansh (ns),\
kodt    smallint not null default 0,\
vidn    char(2) not null default '',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
prov	smallint not null default 0,\
fz      smallint not null default 0)");

/*
0 kod	код начисления
1 naik    наименование начисления
2 shet    счет на который ложится начисление
3 kodt    код табеля к которому привязано начисление
4 vidn    0-основная 1-дополнительная 2-прочая    
5 ktoz    кто записал
6 vrem    время записи
7 prov    0-делать проводки 1-не делать
8 fz      0-входит в фонд зарплаты 1-не входит
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*27*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы удержаний Uder !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uder (\
kod	smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
shet    varchar(12) not null default '' references Plansh (ns),\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
prov	smallint not null default 0,\
shetb   varchar(32) not null default '')");
/*
0 kod	код удержание
1 naik  наименование удержания
2 shet  хозрасчётный
3 ktoz  кто записал
4 vrem  время записи
5 prov    0-делать проводки 1-не делать
6 shetb бюджетный счет
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*28*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы групп подразделений Zargrupp !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zargrupp \
(kod   char(10) not null default '' primary key,\
naik   varchar(100) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");

/*
0 kod    код группы
1 naik   наименование группы
2 ktoz	кто записал
3 vrem	время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*29*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы списка банков Zarsb!\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zarsb \
(kod    int not null default 0 primary key,\
naik    varchar(80) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");
/*
0 kod   код банка
1 naik  наименование банка
2 ktoz  кто записал
3 vrem  время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*29*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы подразделений Podr !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Podr (\
kod	smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
grup    varchar(10) not null default '')");
/*
0 kod	код подразделения
1 naik    наименование подразделения
2 ktoz  Кто записал
3 vrem  Время записи
4 grup  код группы
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*30*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы категорий Kateg !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kateg (\
kod	smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0)");
/*
0 kod	код категории
1 naik    наименование категории
2 ktoz  Кто записал
3 vrem  Время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*31*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы званий Zvan !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zvan (\
kod	smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0)");
/*
0 kod	код звания
1 naik    наименование звания
2 ktoz  Кто записал
3 vrem  Время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*32*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы городов налоговых инспекций Gnali !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Gnali (\
kod	smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
kontr   char(20) not null default '',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0)");
/*
0 kod	код города
1 naik    наименование города
2 kontr   код контрагента налоговой инспекции
3 ktoz  кто записал
4 vrem  время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*33*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы табелей Tabel !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Tabel (\
kod	smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0)");
/*
0 kod	код табеля
1 naik    наименование табеля
2 ktoz  Кто записал
3 vrem  Время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);



/*34*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня социальных отчислений Zarsoc !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zarsoc (\
kod    smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
shet	varchar(30) not null default '',\
proc    float(6,2) not null default 0,\
kodn    varchar(255) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");
/*
0 kod   код фонда
1 naik  наименование фонда
2 shet  счет
3 proc  процент отчисления
4 kodn  коды начислений не входящие в расчет этого отчисления
5 ktoz  кто записал
6 vrem  время записи
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*35*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы социальных отчислений Zarsocz !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zarsocz (\
datz    DATE not null default '0000-00-00',\
tabn    int unsigned not null default 0,\
kodz    smallint not null default 0,\
shet	varchar(30) not null default '',\
sumas   double(8,2) not null default 0,\
sumap   double(8,2) not null default 0,\
proc    float(6,2) not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
sumapb  double(8,2) not null default 0,\
sumasb  double(8,2) not null default 0, \
nz int unsigned PRIMARY KEY AUTO_INCREMENT NOT NULL,\
index(datz,tabn,kodz))");

/*
0  datz дата записи, день всегда равен 1
1  tabn табельный номер
2  kodz код социального отчисления
3  shet счет социального отчисления
4  sumas сумма с которой расчитано отчисление
5  sumap сумма отчисления
6  proc  процент отчисления
7  ktoz  кто записал
8  vrem  время записи
9  sumapb сумма с бюджетных счетов
10 sumasb бюджетная сумма с кототорой расчитано отчисление
11 nz  уникальный номер записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*36*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы сальдо по зарплате Zsal !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zsal (\
god	smallint not null default 0,\
tabn    int not null default 0,\
saldo   double(12,2) not null default 0,\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
saldob  double(12,2) not null default 0,\
unique(god,tabn))");
/*
0 god	год
1 tabn	табельный номер
2 saldo   значение сальдо
3 ktoz  кто записал
4 vrem  время записи
5 saldob  сальдо по бюджетным счетам
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*37*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей табелей Ztab !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Ztab (\
god	smallint not null default 0,\
mes     smallint not null default 0,\
tabn    int not null default 0,\
kodt    smallint not null default 0,\
dnei    float not null default 0,\
has     float not null default 0,\
kdnei   float not null default 0,\
datn    DATE not null default '0000-00-00',\
datk    DATE not null default '0000-00-00',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
kolrd float(5,2) not null default 0,\
kolrh float(5,2) not null default 0,\
kom  varchar(40) not null default '',\
nomz smallint not null default 0,\
unique(tabn,god,mes,kodt,nomz),\
index(god,mes,tabn))");

/*
0  god	год
1  mes     месяц
2  tabn	табельный номер
3  kodt    код табеля
4  dnei    отработано дней
5  has     отработано часов
6  kdnei   отработано календарных дней
7  datn    дата начала
8  datk    дата конца
9  ktoz    кто записал
10 vrem    время модификации
11 kolrd   количество рабочих дней в месяце по данному табелю для конкретного работника
12 kolrh   скольки часовой рабочий день
13 kom     комментарий
14 nomz    номер записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*38*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей зарплаты Zarp !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zarp (\
datz	DATE not null default '0000-00-00',\
tabn    int not null default 0,\
prn     char(2) not null default '',\
knah    smallint not null default 0,\
suma    double (14,2) not null default 0,\
shet    varchar(12) not null default '' references Plansh (ns),\
godn    smallint not null default 0,\
mesn    smallint not null default 0,\
kdn     smallint not null default 0,\
nomz    smallint not null default 0,\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
kom	varchar(40) not null default '',\
podr	int not null default 0,\
nd      VARCHAR(20) NOT NULL default '',\
unique (datz,tabn,prn,knah,godn,mesn,podr,shet,nomz),\
index (tabn,datz),\
index tabn1 (tabn,godn,mesn),\
index (nd,datz))");

/*
0  datz    дата начисления/удержания
1  tabn	табельный номер
2  prn	1- начисление 2 удержание
3  knah	код начисления
4  suma	сума начисления/удержания
5  shet    счет
6  godn    год в счет которого начисления/удержания
7  mesn    месяц в счет которого начисления/удержания
8  kdn     количество дней для больничного
9  nomz    номер записи
10 ktoz    кто записал
11 vrem    дата записи
12 kom     комментарий
13 podr    код подразделения
14 nd      номер документа
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*39*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей текущей настройки карточки Zarn !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zarn (\
god  smallint not null default 0,\
mes  smallint not null default 0,\
podr int not null default 0,\
tabn int not null default 0,\
kateg smallint not null default 0,\
sovm smallint not null default 0,\
zvan smallint not null default 0,\
shet varchar(20) not null default '' references Plansh (ns),\
lgot varchar(60) not null default '',\
prov smallint not null default 0,\
datn DATE not null default '0000-00-00',\
datk DATE not null default '0000-00-00',\
dolg varchar(60) not null default '',\
blok smallint not null default 0,\
unique(tabn,god,mes),\
index(god,mes,podr))");

/*
0  god	   год
1  mes	   месяц
2  podr    подразделение
3  kateg   категория
4  tabn	   табельный номер
5  sovm	   0-нет 1-совместитель
6  zvan	   код звания
7  shet	   счет 
8  lgot	   льготы
9  prov    0-проводки не сделаны 1-сделаны
10 datn    Дата приема на работу
11 datk    Дата увольнения с работы
12 dolg    Должность
13 blok    0- не заблокирована или логин заблокировавшего карточку
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*40*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей текущей настройки начислений/удержаний Zarn1 !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zarn1 (\
tabn	int not null default 0,\
prn	smallint not null default 0,\
knah    smallint not null default 0,\
datd    DATE not null default '0000-00-00',\
shet	varchar(20) not null default '' references Plansh (ns),\
unique(tabn,prn,knah))");
/*
0 tabn	табельный номер
1 prn	1-начисление 2 удержание
2 knah    код начисления/удержания
3 datd    дата до которой действует начисление/удержание
4 shet	счет
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*Создаем таблицы для формы 1дф*/

/*41*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы заголовка документа формы 1дф !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE F8dr (\
god    smallint not null default 0,\
kvrt   smallint not null default 0,\
nomd   varchar(20) not null default '' primary key,\
vidd   smallint not null default 0,\
nomdn  varchar(20) not null default '',\
datad  DATE not null default '0000-00-00',\
datan  DATE not null default '0000-00-00',\
metbl  smallint not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");

/*
0 god    год
1 kvrt   квартал
2 nomd   номер документа
3 vidd   вид документа 0-отчетный 1-новый отчетный 2-уточняющий
4 nomdn  номер под которым документ зарегистрирован в налоговой
5 datad  дата подачи документа
6 datan  дата когда документ зарегистрирован в налоговой
7 metbl  0-документ не заблокирован или логин того кто заблокировал
8 ktoz  кто записал
9 vrem  время записи
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*42*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей в документе формы 1дф !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE F8dr1 (\
nomd   varchar(20) not null default '' references F8dr,\
inn    char(10)  not null default '',\
sumad  double(10,2) not null default 0,\
suman  double(8,2) not null default 0,\
priz   char(4) not null default '',\
datap  DATE not null default '0000-00-00',\
datau  DATE not null default '0000-00-00',\
lgota  char(4) not null default '',\
ktoz   smallint unsigned not null default 0,\
vrem   int unsigned not null default 0,\
fio    varchar(60) not null default '',\
sumadn  double(10,2) not null default 0,\
sumann  double(8,2) not null default 0,\
pr     smallint not null default 0,\
unique(nomd,inn,priz,lgota,pr))");

/*
0 nomd   номер документа
1 inn    индивидуальный налоговый номер
2 sumad  сума выплаченного дохода
3 suman  суммы выплаченного подоходного налога
4 priz   признак дохода
5 datap  дата приема на работу
6 datau  дата увольнения
7 lgota  признак льготы по подоходному налогу
8 ktoz  кто записал
9 vrem  время записи
10 fio фамилия для произвольного ввода записи
11 sumadn  сума начисленного дохода
12 sumann  суммы начисленного подоходного налога
13 pr    0-новая запись 1-удалить запись
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*43*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы заголовков нарядов Zardok !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zardok (\
god smallint NOT NULL default 0,\
datd DATE not null default '0000-00-00',\
nomd varchar(20) not null default '',\
pd int not null default 0,\
koment varchar(100) not null default '',\
ktoi smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
td tinyint NOT NULL default 0,\
unique(nomd,god),\
index(datd,nomd))");

/************
0  god     год документа
1  datd    дата документа
2  nomd      номер документа
3  pd      код подразделения
4  koment  комментарий
5  ktoi    Кто записал
6  vrem    Время записи
7  td      1-приходы 2-расходы
***************/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);




/***********************************************************/
/*Таблицы для программы работы с бухгалтерскими документами*/
/************************************************************/

/*44*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы заголовков платежных поручений Pltp\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Pltp (\
datd	DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
oper    varchar(12) not null default '',\
suma	double (14,2) not null default 0,\
nds     double not null default 0,\
uslb    double not null default 0,\
shet    varchar(30) not null default '',\
plat	varchar(255) not null default '',\
polu    varchar(255) not null default '',\
podt    char(2) not null default '',\
prov    char(2) not null default '',\
vidpl   char(2) not null default '',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
datv    DATE not null default '0000-00-00',\
kodnr   varchar(10) not null default '',\
unique(datd,nomd))");
/*
0  datd	дата документа
1  nomd    номер документа
2  oper    код операции
3  suma	сумма платежа
4  nds     НДС
5  uslb    сумма за услуги банка
6  shet    счет на который платят за услуги банка
7  plat	реквизиты плательщика
8  polu    реквизиты получателя
9  podt    0-не подтверждена 1- подтверждена
10 prov    0-выполнены проводки 1-нет
11 vidpl   вид платежа О,В,К  - в настоящий момент не используется
12 ktoz   кто записал
13 vrem   время записи
14 datv   Дата валютирования
15 kodnr  Код не резидента
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*45*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей к платежному поручению Pltpz\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Pltpz (\
datd	DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
mz      smallint not null default 0,\
nz      smallint not null default 0,\
zapis   varchar(255) not null default '',\
index(datd,nomd))");

/*
0 datd	дата документа
1 nomd    номер документа
2 mz      метка записи
          0-текст назначения платежа
3 nz      номер записи
4 zapis   содержимое записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*46*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы заголовков платежных требований Pltt\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Pltt (\
datd	DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
oper    varchar(12) not null default '',\
suma	double (14,2) not null default 0,\
nds     double not null default 0,\
uslb    double not null default 0,\
shet    varchar(30) not null default '',\
plat	varchar(255) not null default '',\
polu    varchar(255) not null default '',\
podt    char(2) not null default '',\
prov    char(2) not null default '',\
vidpl   char(2) not null default '',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
datv    DATE not null default '0000-00-00',\
kodnr   varchar(10) not null default '',\
index(datd,nomd))");
/*
0  datd	дата документа
1  nomd    номер документа
2  oper    код операции
3  suma	сумма платежа
4  nds     НДС
5  uslb    сумма за услуги банка
6  shet    счет на который платят за услуги банка
7  plat	реквизиты плательщика
8  polu    реквизиты получателя
9  podt    0-не подтверждена 1- подтверждена
10 prov    0-выполнены проводки 1-нет
11 vidpl   вид платежа (здесь пока не применяется)
12 ktoz   кто записал
13 vrem   время записи
14 datv   Дата валютирования
15 kodnr  Код не резидента
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*47*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей к платежному требованию Plttz\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Plttz (\
datd	DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
mz      smallint not null default 0,\
nz      smallint not null default 0,\
zapis   varchar(255) not null default '',\
index(datd,nomd))");

/*
0 datd	дата документа
1 nomd    номер документа
2 mz      метка записи
          0-текст назначения платежа
3 nz      номер записи
4 zapis   содержимое записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*48*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей к платежному требованию Plttz\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Tpltt (\
datd	DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
oper    varchar(12) not null default '',\
suma	double (14,2) not null,\
nds     double not null,\
uslb    double not null,\
shet    varchar(30) not null default '',\
plat	varchar(255) not null default '',\
polu    varchar(255) not null default '',\
podt    char(2) not null default '',\
prov    char(2) not null default '',\
vidpl   char(2) not null default '',\
ktoz    smallint unsigned not null,\
vrem    int unsigned not null,\
datv    DATE not null default '0000-00-00',\
kodnr   varchar(10) not null default '',\
unique(datd,nomd))");
/*
0  datd	дата документа
1  nomd    номер документа
2  oper    код операции
3  suma	сумма платежа
4  nds     НДС
5  uslb    сумма за услуги банка
6  shet    счет на который платят за услуги банка
7  plat	реквизиты плательщика
8  polu    реквизиты получателя
9  podt    0-не подтверждена 1- подтверждена
10 prov    0-выполнены проводки 1-нет
11 vidpl   здесть не используется
12 ktoz   кто записал
13 vrem   время записи
14 datv   Дата валютирования
15 kodnr  Код не резидента
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*49*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей к типовому платежному требованию Tplttz\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Tplttz (\
datd	DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
mz      smallint not null default 0,\
nz      smallint not null default 0,\
zapis   varchar(255) not null default '',\
index(datd,nomd))");

/*
0 datd	дата документа
1 nomd    номер документа
3 mz      метка записи
        0-текст назначения платежа
4 nz      номер записи
5 zapis   содержимое записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*50*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы заголовков типовых платежных поручений Tpltp\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Tpltp (\
datd	DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
oper    varchar(12) not null default '',\
suma	double (14,2) not null default 0,\
nds     double not null default 0,\
uslb    double not null default 0,\
shet    varchar(30) not null default '',\
plat	varchar(255) not null default '',\
polu    varchar(255) not null default '',\
podt    char(2) not null default '',\
prov    char(2) not null default '',\
vidpl   char(2) not null default '',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
datv    DATE not null default '0000-00-00',\
kodnr   varchar(10) not null default '',\
unique(datd,nomd))");
/*
0  datd	дата документа
1  nomd    номер документа
2  oper    код операции
3  suma	сумма платежа
4  nds     НДС
5  uslb    сумма за услуги банка
6  shet    счет на который платят за услуги банка
7  plat	реквизиты плательщика
8  polu    реквизиты получателя
9  podt    0-не подтверждена 1- подтверждена
10 prov    0-выполнены проводки 1-нет
11 vidpl   вид платежа О,В,К - в настоящий момент не используется
12 ktoz   кто записал
13 vrem   время записи
14 datv   Дата валютирования
15 kodnr  Код не резидента
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*51*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей к типовому платежному поручению Tpltpz\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Tpltpz (\
datd	DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
mz      smallint not null default 0,\
nz      smallint not null default 0,\
zapis   varchar(255) not null default '',\
index(datd,nomd))");

/*
0 datd	дата документа
1 nomd    номер документа
3 mz      метка записи
        0-текст назначения платежа
4 nz      номер записи
5 zapis   содержимое записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*52*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня операций платежных поручений Opltp !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Opltp \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");
/*
0 kod    Код операции платежного поручения
1 naik   Наименование операции платежного поручения
2 ktoz   Кто записал
3 vrem   Время записи
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*53*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня операций платежных требований Opltt !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Opltt \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");
/*
0 kod    Код операции платежного требования
1 naik   Наименование операции платежного требования
2 ktoz   Кто записал
3 vrem   Время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*54*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы объявок cдачи налички в банк Obqvka !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Obqvka (\
datd	DATE not null default '0000-00-00',\
nomd	varchar(20) not null default '',\
suma    double (14,2) not null default 0,\
kodop   varchar(8) not null default '',\
fio	varchar(40) not null default '',\
prov    char(2) not null default '',\
podt    char(2) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(datd,nomd))");

/*
0 datd	дата документа
1 nomd	номер документа
2 suma    сумма документа
3 kodop   код операции
4 fio	фамилия сдающего деньги
5 prov    0-проводки не выполнены 1-выполнены
6 podt    0-не подтвержден 1-подтвержден
7 ktoz  
8 vrem
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*55*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня операций cдачи налички в банк Oznvb !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Oznvb \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");
/*
0 kod    Код операции
1 naik   Наименование операции
2 ktoz   Кто записал
3 vrem   Время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*56*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня типовых платежек Ktnp !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Ktnp \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");
/*
0 kod    Код типового платежного поручения
1 naik   Наименование типового платежного поручения
2 ktoz   Кто записал
3 vrem   Время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*57*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня типовых требований Ktnt !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Ktnt \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");
/*
0 kod    Код типового платежного требования
1 naik   Наименование типового платежного требования
2 ktoz   Кто записал
3 vrem   Время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*58*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня сдающих в банк наличку Ksn !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Ksn \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");
/*
0 kod    Код сдающего наличку в банк
1 naik   Фамилия сдающего наличку в банк
2 ktoz   Кто записал
3 vrem   Время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*59*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы подтверждающих записей документов Pzpd !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Pzpd (\
tp	smallint not null default 0,\
datd    DATE not null default '0000-00-00',\
datp	DATE not null default '0000-00-00',\
nomd	varchar(20) not null default '',\
suma	double (14,2) not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
index(tp,nomd))");

/*
0 tp	0-платежкка 1-требование
1 datd    дата документа
2 datp	дата подтверждения
3 nomd	номер документа
4 suma	сумма подтверждения
5 ktoz  кто записал
6 vrem  время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/************************************************/
/***********Учет основных средств****************/
/************************************************/
/*60*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня операций приходов Uospri!\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uospri \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
vido    smallint not null default 0,\
prov    smallint not null default 0)");

/*
0 kod  код операции
1 naik наименование операции
2 ktoz Кто записал
3 vrem время записи
4 vido 0 - внешняя 1-внутренняя 2-изменения стоимости
5 prov 0 проводки нужно делать 1 - не нужно
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*61*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня операций расходов Uosras!\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosras \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
vido    smallint not null default 0,\
prov    smallint not null default 0)");
/*
0 kod  код операции
1 naik наименование операции
2 ktoz Кто записал
3 vrem время записи
4 vido 0 - внешняя 1-внутренняя 2-изменения стоимости
5 prov 0 проводки нужно делать 1 - не нужно
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы ликвидационной цены Uosls!\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosls (\
inn INT NOT NULL DEFAULT 0,\
data DATE NOT NULL DEFAULT '0000-00-00',\
lsnu DOUBLE NOT NULL DEFAULT 0.,\
lsbu DOUBLE NOT NULL DEFAULT 0.,\
kom VARCHAR(255) NOT NULL DEFAULT '',\
ktoz smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
index(inn,data))");

/*
0 inn    инвентарный номер
1 data  дата записи
2 lsnu  ликвидационная цена для налогового учёта
3 lsbu  ликвидационная цена для бух. учёта
4 kom   коментарий
5 ktoz  кто записал
6 vrem  время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*62*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня подразделений Uospod !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uospod \
(kod    smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
mi tinyint not null)");
/*
0 kod    Код подразделения
1 naik   Наименование подразделения
2 ktoz   Кто записал
3 vrem   Время записи
4 mi     0-используется 1-неиспользуется (уволен и остатки по нему нулевые)
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*63*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня драг-металлов Uosdm !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosdm \
(kod    smallint not null default 0 primary key,\
naik    varchar(100) not null default '',\
ei      varchar(20) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");
/*
0 kod    Код драгоценного металла
1 naik   Наименование металла
2 ei     Единица измерения
3 ktoz   Кто записал
4 vrem   Время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*64*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы содержимого драг-металлов в инвентарном номере Uosindm !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosindm \
(innom int not null,\
kod    smallint not null default 0,\
ei     varchar(20) not null default '',\
ves    double(13,8) not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(innom,kod))");
/*
0 innom    инвентарный номер
1 kod    Код драгоценного металла
2 ei     Единица измерения
3 ves    вес материалла
4 ktoz   Кто записал
5 vrem   Время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);




/*65*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня груп Uosgrup для налогового учета !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosgrup \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
kof     float not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
ta tinyint not null,\
ar tinyint not null)");
/*
0 kod    код
1 naik   наименование
2 kof    годовой процент амортизационного отчисления
3 ktoz   Кто записал
4 vrem   Время записи
5 ta     Тип амортизации 0-по объектная 1-в целом по группе
6 ar     алготитм расчёта 0-от начальной балансовой стоимости 1-от остаточной балансовой стоимости
*/


if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*66*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня груп Uosgrup1 для бухгалтерского учета !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosgrup1 \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
kof     float not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
ar tinyint not null)");

/*
0 kod    код
1 naik   наименование
2 kof    коэффициент нормы амортотчисления
3 ktoz
4 vrem
5 ar     алготитм расчёта 0-от начальной балансовой стоимости 1-от остаточной балансовой стоимости
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*67*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы шифров аналитического учета Uoshau !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uoshau \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");
/*
0 kod    Код шифра аналитического учета
1 naik   Наименование шифра аналитического учета
2 ktoz   Кто записал
3 vrem   Время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*68*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы шифров затрат Uoshz !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uoshz \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");
/*
0 kod    Код шифра затрат
1 naik   Наименование шифра затрат
2 ktoz   Кто записал
3 vrem   Время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*69*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы ответственных Uosol !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosol \
(kod    int not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
mi tinyint not null)");
/*
0 kod    Код ответственного лица
1 naik   Фамилия ответственного лица
2 ktoz   Кто записал
3 vrem   Время записи
4 mi     0-используется 1-неиспользуется (уволен и остатки по нему нулевые)
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*70*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы инвентарных номеров Uosin !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosin (\
innom   int unsigned not null default 0 primary key,\
god     smallint not null default 0,\
naim	varchar(80) not null default '',\
zaviz   varchar(60) not null default '',\
pasp	varchar(60) not null default '',\
model   varchar(60) not null default '',\
zavnom  varchar(60) not null default '',\
datvv   DATE not null default '0000-00-00',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");
/*
0 innom   инвентарный номер
1 god     год изготовления
2 naim	наименование
3 zaviz   завод изготовитель
4 pasp	паспорт
5 model   модель
6 zavnom  заводской номер
7 datvv   дата ввода в эксплуатацию
8 ktoz  кто записал
9 vrem  время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*71*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы Uosin1 !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uosin1 (\
innom int unsigned not null default 0,\
nomz   smallint unsigned not null default 0,\
zapis  varchar(255) not null default '',\
unique(innom,nomz))");

/*
0 innom инвентарный номер
1 nomz    номер записи
3 zapis   содержимое записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*72*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы переменных данных инвентарных номеров Uosinp !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosinp (\
innom   int unsigned not null default 0,\
mes	smallint not null default 0,\
god     smallint not null default 0,\
shetu   varchar(20) not null default '' references Plansh (ns),\
hzt     varchar(20) not null default '',\
hau     varchar(20) not null default '',\
hna     varchar(20) not null default '',\
popkof  double(12,6) not null default 0.,\
soso    smallint not null default 0,\
nomz	varchar(40) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
hnaby   varchar(20) not null default '',\
popkofby double(12,6) not null default 0.,\
unique(innom,god,mes))");
/*
0  innom   инвентарный номер
1  mes	   месяц записи
2  god     год записи,\
3  shetu   счет учета
4  hzt     шифр затрат
5  hau     шифр аналитического учета
6  hna     шифр нормы амотротчислений (группа) для налогового учета
7  popkof  поправочный коэффициент для налогового учета
8  soso    состояние объекта 0-амортизация считается для бухучета и налогового учета
                             1-амортизация не считается для бух.учета и налогового учета
                             2-считается для бух.учета и не считается для налогового
                             3-не считается для бух учета и считается для налогового
9  nomz	   номерной знак (для автомобилей)
10 ktoz    кто записал
11 vrem    время записи
12 hnaby   шифр нормы амотротчислений (группа) для бух. учета
13 popkofby  поправочный коэффициент для бух. учета
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*73*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы заголовков документов Uosdok !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosdok (\
datd	DATE not null default '0000-00-00',\
tipz	smallint not null default 0,\
kodop   varchar(20) not null default '',\
kontr   varchar(20) not null default '',\
nomd	varchar(20) not null default '',\
podr	smallint not null default 0,\
kodol   int not null default 0,\
podrv	smallint not null default 0,\
nomdv	varchar(20) not null default '',\
kodolv  int not null default 0,\
prov	smallint not null default 0,\
podt	smallint not null default 0,\
osnov   varchar(60) not null default '',\
ktoz	smallint unsigned not null,\
vrem	int unsigned not null,\
dover varchar(30) not null default '',\
datvd DATE not null default '0000-00-00',\
forop varchar(10) not null default '',\
datpnn DATE not null default '0000-00-00',\
nomnn varchar(16) not null default '',\
sherez varchar(40) not null default '',\
datop DATE not null default '0000-00-00',\
nds smallint not null default 0,\
pn float(5,2) not null default 0,\
dvnn DATE NOT NULL default '0000-00-00',\
mo tinyint NOT NULL default 0,\
vd varchar(255) NOT NULL DEFAULT '',\
unique(datd,nomd),\
index(nomnn))");
/*
0  datd	дата документа
1  tipz	1-приход 2-расход
2  kodop   код операции
3  kontr   код контрагента
4  nomd	номер документа
5  podr	подразделение
6  kodol  код ответственного лица
7  podrv  подразделение встречное
8  nomdv  номер парного документа для внутреннего перемещения или номер документа поставщика для приходного документа
9  kodolv  код встречного ответственного лица
10 prov	  0-провоки выполнены 1-нет
11 podt	  0-неподтвержден 1-подтвержден
12 osnov  основание
13 ktoz	 кто записал
14 vrem	 время записи
15 dover доверенность
16 datvd дата выдачи доверенности
17 forop код формы оплаты
18 datpnn дата выдачи налоговой накладной
19 nomnn номер налоговой накладной
20 sherez через кого
21 datop дата оплаты
22 nds    НДС
       0 20% НДС.
       1 0% НДС реализация на територии Украины.
       2 0% НДС экспорт.
       3 Освобождение от НДС статья 5.
23 pn - процент НДС дейстованший на момент создания документа
24 dvnn - дата выписки налоговой накладной
25 mo - метка оплаты 0- неоплачено 1-оплачено
26 vd - вида договора (для налоговой накладной)
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*74*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей в документе Uosdok1 !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosdok1 (\
datd	DATE not null default '0000-00-00',\
tipz	smallint not null default 0,\
podt	smallint not null default 0,\
innom   int unsigned not null default 0,\
nomd	varchar(20) not null default '',\
podr	smallint not null default 0,\
kodol   int not null default 0,\
kol     smallint not null default 0,\
bs	double(12,2) not null default 0,\
iz	double(12,2) not null,\
kind	float not null default 0,\
kodop   varchar(20) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
bsby	double(12,2) not null default 0,\
izby    double(12,2) not null default 0,\
cena	double(14,2) not null default 0,\
shs     varchar(20) not null default '',\
unique(datd,nomd,innom),\
index(innom,datd))");

/*
0  datd	  дата документа
1  tipz	  1-приход 2-расход
2  podt	  0-не подтверждена 1-подтверждена
3  innom  инвентарный номер
4  nomd	  номер документа
5  podr   подразделение
6  kodol  код ответственного лица
7  kol    количество : +1 приход -1 расход 0 изменение стоимости
8  bs	  балансовая стоимость для налогового учета
9  iz	  износ для налогового учета
10 kind	  коэффициент индексации
11 kodop  код операции
12 ktoz   кто записал
13 vrem   время записи
14 bsby	  балансовая стоимость для бух-учета
15 izby	  износ для бух-учета
16 cena   цена продажи
17 shs    счёт получения/списания
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*75*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей амортизации Uosamor для налогового учета!\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosamor (\
innom   int not null default 0,\
mes	smallint not null default 0,\
god	smallint not null default 0,\
podr	smallint not null default 0,\
hzt	varchar(20) not null default '',\
hna	varchar(20) not null default '',\
suma	double (14,2) not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(innom,god,mes))");

/*
0 innom   инвентарный номер
1 mes	месяц расчета
2 god	год расчета
3 podr	подразделение
4 hzt	шифр затрат
5 hna	шифр нормы аморт отчислений
6 suma	сумма
7 ktoz	кто записал
8 vrem	дата записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*76*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей амортизации Uosamor1 для бух учета !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosamor1 (\
innom   int not null default 0,\
mes	smallint not null default 0,\
god	smallint not null default 0,\
podr	smallint not null default 0,\
hzt	varchar(20) not null default '',\
hna	varchar(20) not null default '',\
suma	double (14,2) not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(innom,god,mes))");

/*
0 innom   инвентарный номер
1 mes	месяц расчета
2 god	год расчета
3 podr	подразделение
4 hzt	шифр затрат
5 hna	шифр нормы аморт отчислений
6 suma	сумма
7 ktoz	кто записал
8 vrem	дата записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*********Учет услуг*************************/
/*77*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня групп услуг Uslgrup !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uslgrup \
(kod    smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");
/*
0 kod    Код группы услуг
1 naik   Наименование группы услуг
2 ktoz   Кто записал
3 vrem   Время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*78*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня операций услуг на приход Usloper1 !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Usloper1 \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
vido    smallint not null default 0,\
prov    smallint not null default 0,\
shet	varchar(20) not null default '')");

/*
0 kod  код операции
1 naik наименование операции
2 ktoz
3 vrem
4 vido 0 - внешняя 1-внутренняя
5 prov 0 проводки нужно делать 1 - не нужно
6 shet счет учета операции
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*79*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня операций услуг на расход Usloper2 !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Usloper2 \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
vido    smallint not null default 0,\
prov    smallint not null default 0,\
shet	varchar(20) not null default '')");
/*
0 kod  код операции
1 naik наименование операции
2 ktoz
3 vrem
4 vido 0 - внешняя 1-внутренняя
5 prov 0 проводки нужно делать 1 - не нужно
6 shet счет учета операции
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*80*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня подразделений Uslpodr !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uslpodr \
(kod    smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");

/*
0 kod  код подразделения
1 naik наименование подразделения
2 ktoz   Кто записал
3 vrem   Время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*81*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня услуг Uslugi !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));


strcpy(strsql,"CREATE TABLE Uslugi (\
kodus   int not null default 0 primary key,\
kodgr   smallint not null default 0,\
naius   varchar(80) not null default '',\
ei      varchar(20) not null default '',\
shetu   varchar(20) not null default '' references Plansh (ns),\
cena    double(10,2) not null default 0,\
nds	real not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
art varchar(64) not null default '')");

/*
0 kodus   код услуги
1 kodgr   код группы
2 naius   наименование услуги
3 ei      единица измерения
4 shetu   счет учета
5 cena    цена услуги
6 nds	0-НДС не применяется 1-применяется
7 ktoz	кто записал
8 vrem	время записи
9 art  артикул
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*82*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы заголовков документов Usldokum !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Usldokum (\
tp     smallint not null default 0,\
datd   DATE not null default '0000-00-00',\
podr   smallint not null default 0 references Uslpod (kod),\
kontr  varchar(20) not null default '' references Kontragent (kontr),\
nomd   varchar(20) not null default '',\
nomnn  varchar(20) not null default '',\
kodop  varchar(10) not null default '' references Usloper (kod),\
nds    smallint not null default 0,\
forop  varchar(20) not null default '',\
datop  DATE not null default '0000-00-00',\
uslpr  varchar(80) not null default '',\
sumkor double(10,2) not null default 0,\
pod    smallint not null default 0,\
pro    smallint not null default 0,\
oplata smallint not null default 0,\
blokir smallint unsigned not null default 0,\
nomdp  varchar(20) not null default '',\
datdp  DATE not null default '0000-00-00',\
dover  varchar(20) not null default '',\
datdov DATE not null default '0000-00-00',\
sherez varchar(40) not null default '',\
ktoi   smallint unsigned not null default 0,\
vrem   int unsigned not null default 0,\
osnov  varchar(80) not null default '',\
datpnn DATE not null default '0000-00-00',\
sumnds double(10,2) not null default 0,\
pn float(5,2) not null default 0,\
mo tinyint NOT NULL default 0,\
unique (datd,podr,nomd,tp),\
index (nomnn)\
)");

/*
0  tp     1-приход 2-расход
1  datd   дата
2  podr   код подразделения
3  kontr  код контрагента
4  nomd   номер документа
5  nomnn  номер налоговой накладной
6  kodop  код операции
7  nds    НДС
       0 20% НДС.
       1 0% НДС реализация на територии Украины.
       2 0% НДС экспорт.
       3 0% Освобождение от НДС статья 5.
8  forop  код формы оплаты
9  datop  дата оплаты или отсрочки
10 uslpr  условие продажи
11 sumkor сумма корректировки к документа
12 pod    0-не подтверждена 1-подтверждена
13 pro    0-не выполнены проводки 1-выполнены
14 oplata 0-не оплачена 1-оплачена
15 blokir код того кто заблокировал документ
16 nomdp  номер документа поставщика
17 datdp  Дата виписки документа поставщика
18 dover  довереность
19 datdov дата доверенности
20 sherez через кого
21 ktoi   кто записал
22 vrem   время записи
23 osnov  основание
24 datpnn дата получения налоговой накладной
25 sumnds Сумма НДС для приходных документов введенная вручную
26 pn - процент НДС дейстованший на момент создания документа
27 mo - метка оплаты 0-неоплачено 1-оплачено
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*83*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей к документа Usldokum1 !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Usldokum1 (\
tp     smallint not null default 0,\
datd   DATE not null default '0000-00-00',\
nomd   varchar(20) not null default '',\
metka  smallint not null default 0,\
kodzap int not null default 0,\
kolih  double(16,6) not null default 0,\
cena   double(16,6) not null default 0,\
ei     varchar(20) not null default '' references Edizmer (kod),\
shetu  varchar(20) not null default '' references Plansh (ns),\
podr   smallint not null default 0 references Uslpod (kod),\
ktoi   smallint unsigned not null default 0,\
vrem   int unsigned not null default 0,\
dnaim  varchar(70) not null default '',\
nz     smallint not null default 0,\
shsp   varchar(20) not null default '',\
unique (datd,podr,tp,nomd,metka,kodzap,nz))");

/*
0  tp     1-приход 2-расход
1  datd   Дата документа
2  nomd   Номер документа
3  metka  0-материал 1-услуга
4  kodzap Код материалла/услуги
5  kolih  Количество
6  cena   Цена
7  ei     Единица измерения
8  shetu  Счет учета
9  podr   Код подразделения
10 ktoi   Кто записал
11 vrem   Время записи
12 dnaim  Дополнительное наименование услуги
13 nz     номер записи
14 shsp   счёт списания/приобретения
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*84*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы подтверждающих записей к документу Usldokum2 !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Usldokum2 (\
tp     smallint not null default 0,\
datd   DATE not null default '0000-00-00',\
datp   DATE not null default '0000-00-00',\
nomd   varchar(20) not null default '',\
metka  smallint not null default 0,\
kodzap int not null default 0,\
kolih  double(16,6) not null default 0,\
cena   double(16,6) not null default 0,\
ei     varchar(20) not null default '' references Edizmer (kod),\
shetu  varchar(20) not null default '' references Plansh (ns),\
podr   smallint not null default 0 references Uslpod (kod),\
ktoi   smallint unsigned not null default 0,\
vrem   int unsigned not null default 0,\
nz     smallint not null default 0,\
unique (datd,datp,podr,tp,nomd,metka,kodzap,nz),\
index (datp))");

/*
0  tp     1-приход 2-расход
1  datd   Дата документа
2  datp   Дата подтверждения документа
3  nomd   Номер документа
4  metka  0-материал 1-услуга
5  kodzap Код услуги
6  kolih  Количество
7  cena   Цена
8  ei     Единица измерения
9  shetu  Счет учета
10 podr   Код подразделения
11 ktoi   Кто записал
12 vrem   Время записи
13 nz     номер записи в документе
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

//Записывается произвольный текст
/*85*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы приложения к счету Usldokum3 !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Usldokum3 (\
podr    int not null default 0,\
god     smallint not null default 0,\
tp      smallint not null default 0,\
nomd    varchar(20) not null default '',\
nomz    smallint unsigned not null default 0,\
zapis   varchar(255) not null default '',\
unique(podr,god,tp,nomd,nomz))");

/*
0 podr    подразделение
1 god     год
2 tp      1-приход 2-расход
3 nomd    номер документа
4 nomz    номер записи
5 zapis   содержимое записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/***********************/
/*Учет кассовых ордеров*/
/***********************/

/*86*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы перечня касс Kas !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Kas \
(kod    smallint not null default 0 primary key,\
naik    char(60) not null default '',\
shet    char(20) not null default '',\
fio     char(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");

/*
0 kod     Код кассы
1 naik    Наименование кассы
2 shet   Счёт кассы
3 fio     Фамилия кассира
4 ktoz   Кто записал
5 vrem   Время записи
*/


if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*87*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы операций на приход Kasop1 !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Kasop1 \
(kod    char(10) not null default '' primary key,\
naik    char(250) not null default '',\
shetkas char(20) not null default '',\
shetkor char(20) not null default '',\
metkapr smallint not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
kcn     smallint not null default 0)");

/*
0 kod     Код операции
1 naik    Наименование операции
2 shetkas счет кассы
3 shetkor счет корреспондент
4 metkapr 0-проводки не выполняются 1-выполняются
5 ktoz    кто записал
6 vrem    время записи
7 kcn     код целевого назначения
*/


if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*88*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы операций на расход Kasop2 !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Kasop2 \
(kod    char(10) not null default '' primary key,\
naik    char(250) not null default '',\
shetkas char(20) not null default '',\
shetkor char(20) not null default '',\
metkapr smallint not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
kcn     smallint not null default 0)");

/*
0 kod     Код операции
1 naik    Наименование операции
2 shetkas счет кассы
3 shetkor счет корреспондент
4 metkapr 0-проводки не выполняются 1-выполняются
5 ktoz    кто записал
6 vrem    время записи
7 kcn     код целевого назначения
*/


if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*89*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы кассовых ордеров Kasord !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kasord (\
kassa   smallint not null default 0,\
tp	smallint not null default 0,\
datd    DATE NOT NULL default '0000-00-00',\
nomd    varchar(20) NOT NULL default '',\
shetk   varchar(20) not null default '',\
kodop   varchar(10) not null default '',\
prov    smallint not null default 0,\
podt    smallint not null default 0,\
osnov   varchar(250) not null default '',\
dopol   varchar(80) not null default '',\
fio     varchar(60) not null default '',\
dokum   varchar(80) not null default '',\
god     smallint not null default 0,\
ktoi    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
nb      varchar(20) not null default '',\
nomv    varchar(20) not null default '',\
unique(nomd,kassa,tp,god),\
index(datd,kassa),\
index(nb))");

/*
0  kassa   номер кассы
1  tp	   1-приход 2-расход
2  datd    дата документа
3  nomd    номер документа
4  shetk   счет корреспондент
5  kodop   код операции
6  prov    0 провоки не сделаны 1-сделаны
7  podt    0 не подтвержден 1-подтвержден
8  osnov   Основание 
9  dopol   дополнение
10 fio     фамилия имя отчество кому выданы деньги если есть ведомомть к ордеру
11 dokum   наименование дата и номер документа
12 god     год
13 ktoi    Кто записал
14 vrem    Время записи
15 nb      номер бланка
16 nomv    номер ведомости на выплату средств кореспондируется с Kasnomved
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*90*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей кассовых ордеров Kasord1 !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kasord1 (\
kassa   smallint not null default 0,\
tp	smallint not null default 0,\
datd    DATE not null default '0000-00-00',\
datp    DATE not null default '0000-00-00',\
nomd    varchar(20) NOT NULL default '',\
kontr   varchar(20) not null default '',\
suma    double(14,2) not null default 0,\
god     smallint not null default 0,\
metka   smallint not null default 0,\
ktoi    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
koment  varchar(100) not null default '',\
unique(kassa,god,tp,nomd,kontr),\
index(datd),\
index(datp))");

/*
0  kassa   номер кассы
1  tp	  1-приход 2-расход
2  datd    дата документа
3  datp    дата подтверждения записи
4  nomd    номер документа
5  kontr   код контрагента
6  suma    сумма
7  god     год
8  metka   0 запись не перегружена 1-перегружена (В расчет зарплаты)
9  ktoi    Кто записал
10 vrem    Время записи
11 koment  комментарий
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*91*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы журнала ведомостей Kasnomved !\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kasnomved (\
datd    DATE not null default '0000-00-00',\
god     smallint not null default 0,\
nomv    varchar(10) not null default '',\
suma    double(14,2) not null default 0,\
koment  varchar(100) not null default '',\
ktoi    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
nomd varchar(20) not null default '',\
unique(god,nomv),\
index(datd,nomv))");

/*
0  datd    дата документа
1  god     год документа
2  nomv    номер ведомости
3  suma    сумма по документу
4  koment  комментарий
5  ktoi    Кто записал
6  vrem    Время записи
7  nomd    номер кассового ордера из таблицы Kasord
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*92*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы целевых назначений Kascn !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kascn (\
kod    smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");

/*
kod   код целевого назначения
naik  наименование целевого назначения
ktoz  кто записал
vrem  время записи
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*93*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы сальдо по целевым назначениям Kascnsl !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kascnsl (\
god   smallint not null default 0,\
kod   smallint not null default 0,\
ks    int not null default 0,\
saldo double(10,2) not null default 0,\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0,\
unique(god,kod,ks))");

/*
0 god   год
1 kod   код целевого назначения
2 ks    код кассы
3 saldo сальдо
4 ktoz  кто записал
5 vrem  время записи
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

//************************************************************************
//Подсистема "Учёт командировочных расходов"
//************************************************************************



/*94*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы видов командировок Ukrvkr\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Ukrvkr (\
kod	char(10) not null default '' primary key,\
naik    varchar(80) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");

/*
0 kod   код вида командировок
1 naik  наименование вида командировок
2 ktoz  кто записал
3 vrem  время записи
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*95*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы групп видов командировок Ukrgrup\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Ukrgrup (\
kod    smallint not null default 0 primary key,\
naik    varchar(80) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");

/*
0 kod   код группы вида командировок
1 naik  наименование группы
2 ktoz  кто записал
3 vrem  время записи
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*96*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы видов расходов Ukrkras\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Ukrkras (\
kod     int not null default 0 primary key,\
naim    varchar(80) not null default '',\
cena    double(10,2) not null default 0,\
shet    varchar(20) not null default '',\
kgr     int not null default 0,\
eiz     varchar(10) not null default '',\
mnds    smallint not null default 0,\
ktoz    smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(naim))");

/*
0 kod   код вида расходов
1 naim  наименование вида расхода
2 cena  цена
3 shet  счет учета
4 kgr   код группы
5 eiz   единица измерения
6 mnds  0-цена введена с НДС 1-без НДС
7 ktoz  кто записал
8 vrem  время записи
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*97*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы шапок документов Ukrdok\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Ukrdok (\
god     smallint not null default 0,\
nomd	varchar(20) not null default '',\
datd    DATE not null default '0000-00-00',\
kont	varchar(20) not null default '',\
pnaz    varchar(80) not null default '',\
organ   varchar(80) not null default '',\
celk    varchar(80) not null default '',\
datn    DATE not null default '0000-00-00',\
datk    DATE not null default '0000-00-00',\
nompr   varchar(20) not null default '',\
datp    DATE not null default '0000-00-00',\
mprov   smallint not null default 0,\
mpodt   smallint not null default 0,\
vkom    varchar(10) not null default '',\
nomao   varchar(10) not null default '',\
datao   DATE not null default '0000-00-00',\
ktoz    smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(god,nomd),\
index(datd,nomd),\
index(datao,nomao))");

/*
 0 god   год документа
 1 nomd  Номер документа
 2 datd  Дата документа
 3 kont  код контрагента
 4 pnaz  пунк назначения
 5 organ предприятия и организации
 6 celk  Цель командировки
 7 datn  Дата начала командировки
 8 datk  Дата конца командировки
 9 nompr Номер приказа
10 datp  Дата приказа
11 mprov Метка выполнения проводок 0-не выполнены 1-выполнены
12 mpodt Метка присутствия записей в документе 0-нет записей 1-есть
13 vkom  Вид командировки
14 nomao Номер авансового отчета
15 datao Дата авансового отчета
16 ktoz  кто записал
17 vrem  время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*98*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей в документе Ukrdok1\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Ukrdok1 (\
datd	DATE not null default '0000-00-00',\
nomd	varchar(20) not null default '',\
kodr    int not null default 0,\
shet	varchar(20) not null default '',\
kolih   double(15,4) not null default 0,\
cena    double(15,2) not null default 0,\
ediz    varchar(10) not null default '',\
snds    double(10,2) not null default 0,\
ktoz    smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
nomz	smallint not null default 0,\
kontr   varchar(20) not null default '',\
kdrnn   varchar(20) not null default '',\
ndrnn   varchar(30) not null default '',\
sn      varchar(10) NOT NULL default '',\
dvnn DATE NOT NULL default '0000-00-00',\
ss double(10,2) not null default 0,\
unique(datd,nomd,kodr,shet,nomz))" );

/*
 0 datd  Дата документа
 1 nomd  Номер документа
 2 kodr  Код расхода
 3 shet  Счет
 4 kolih Количество
 5 cena  Сумма без НДС
 6 ediz  Единица измерения
 7 snds  Сумма НДС
 8 ktoz  Кто записал
 9 vrem  Время записи
10 nomz  Номер записи
11 kontr Код контрагента от кого получена услуга или товар за наличные(вводится чтобы была сделана дополнительная проводка)
12 kdrnn Код контрагента для реестра налоговых накладных
13 ndrnn Номер документа, который в реестре нал. накладных будет записан как номер налоговой накладной
14 sn    счёт НДС (Иногда нужно чтобы в одном документе разные записи шли на разные счета)
15 dvnn  дата выписки налоговой накладной
16 ss    Старховой сбор
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


//*************************************************************
//Подсистема "Учёт путевых листов"
//************************************************************

/************31.07.2003******************/

/*99*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы подразделений Uplpodr !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplpodr (\
kod   smallint not null default 0 primary key,\
naik  varchar(80) not null default '',\
ktoz  smallint unsigned default 0,\
vrem  int unsigned default 0)");

/*
0 kod   код подразделения
1 naik  наименование подразделения
2 ktoz  кто записал
3 vrem  время записи
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);



/*100*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы марок топлива Uplmt !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplmt (\
kodt  char(20) not null default '' primary key,\
kodm  int not null default 0,\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0)");

/*
0 kodt  код марки топлива
1 kodm  код материалла в списке материалов
2 ktoz  кто записал
3 vrem  время записи
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*101*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы объектов учета остатков топлива Uplouot !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplouot (\
kod   smallint not null default 0 primary key,\
mt    smallint not null default 0,\
skl   smallint not null default 0,\
naik  varchar(80) not null default '',\
kontr varchar(20) not null default '',\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0)");

/*
0 kod   код объекта
1 mt    0-заправка 1-водитель
2 skl   код склада в материальном учете
3 naik  наименование
4 kontr код контрагента
5 ktoz  кто записал
6 vrem  время записи
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*102*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы списка автомобилей Uplavt !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplavt (\
kod   smallint not null default 0 primary key,\
nomz  varchar(20) not null default '',\
lnr   float(7,3) not null default 0,\
znr   float(7,3) not null default 0,\
kzv   smallint not null default 0,\
naik  varchar(80) not null default '',\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0,\
lnrzg float(7,3) not null default 0,\
znrzg float(7,3) not null default 0,\
nrtk  float(7,3) not null default 0,\
nrmh  float(7,3) not null default 0,\
index(nomz))");

/*
0  kod   код автомобиля
1  nomz  номерной знак
2  lnr   летняя норма расхода (в городе)
3  znr   зимняя норма расхода (в городе)
4  kzv   код закрепленного водителя
5  naik  наименование
6  ktoz  кто записал
7  vrem  время записи
8  lnrzg летняя норма расхода (за городом)
9  znrzg зимняя норма расхода (за городом)
10 nrtk  норма расхода в тоно-километрах
11 nrmh  норма расхода в моточасах
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*103*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы шапки путевых листов Upldok !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Upldok (\
datd DATE not null default '0000-00-00',\
kp smallint not null default 0,\
nomd varchar(20) not null default '',\
god smallint not null default 0,\
nomb varchar(20) not null default '',\
ka smallint not null default 0,\
kv smallint not null default 0,\
psv int unsigned not null default 0,\
psz int unsigned not null default 0,\
ztpn float(8,3) not null default 0,\
ztfa float(8,3) not null default 0,\
prob int not null default 0,\
denn DATE not null default '0000-00-00',\
denk DATE not null default '0000-00-00',\
vremn TIME not null default '000:00:00',\
vremk TIME not null default '000:00:00',\
mt smallint not null default 0,\
mp smallint not null default 0,\
otmet varchar(250) not null default '',\
nst float(7,3) not null default 0,\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0,\
ztpnzg float(8,3) not null default 0,\
ztfazg float(8,3) not null default 0,\
probzg int not null default 0,\
nstzg float(7,3) not null default 0,\
vesg  float(10,2) not null default 0,\
ztvsn float(8,3) not null default 0,\
ztvsf float(8,3) not null default 0,\
vrr   float(8,2) not null default 0,\
ztvrn float(8,3) not null default 0,\
ztvrf float(8,3) not null default 0,\
tk    float(8,3) not null default 0,\
nz smallint not null default 0,\
unique(god,kp,nomd),\
unique(ka,datd,nz),\
index(datd),\
index(nomd))");

/*
0  datd  дата документа
1  kp    код подразделения (если равно 0 - акт списания)
2  nomd  номер документа
3  god   год документа
4  nomb  номер бланка документа строгой отчетности
5  ka    код автомобиля
6  kv    код водителя
7  psv   показания спидометра при выезде
8  psz   показания спидометра при возвращении
9  ztpn  затраты топлива по норме (по городу)
10 ztfa  затраты топлива фактические (по городу)
11 prob  пробег (по городу)
12 denn  Дата выезда
13 denk  Дата возвращения
14 vremn время выезда
15 vremk время возвращения
16 mt    0-не списано (или не полностью списано) топливо пообъектно 1-списано
17 mp    0-проводки не сделаны 1-сделаны
18 otmet Особенные отметки
19 nst   Норма списания топлива (по городу)
20 ktoz  кто записал
21 vrem  время записи

22 ztpnzg затраты топлива по норме (за городом)
23 ztfazg затраты топлива фактические (за городом)
24 probzg пробег (за городом)
25 nstzg  Норма списания топлива (за городом)

26 vesg   вес перевозимого груза
27 ztvsn  норма списания на вес перевозимого груза
28 ztvsf  затраты топлива по факту на груз
29 vrr    Время работы двигателя
30 ztvrn  норма списания на время работы двигателя
31 ztvrf  затраты топлива по факту на время работы двигателя
32 tk     тонно-километры
33 nz     номер по порядку документа выписанного в один и тотже день на один и тотже автомобиль
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*104*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей движения топлива Upldok1 !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Upldok1 (\
datd  DATE not null default '0000-00-00',\
kp smallint not null default 0,\
nomd varchar(10) not null default '',\
nzap smallint not null default 0,\
ka smallint not null default 0,\
kv smallint not null default 0,\
kodsp smallint not null default 0,\
tz smallint not null default 0,\
kodtp varchar(20) not null default '',\
kolih double(10,3) not null default 0,\
kom varchar(80) not null default '',\
kap smallint not null default 0,\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0,\
nz smallint not null default 0,\
unique(datd,kp,nomd,nzap),\
index(datd,ka))");

/*
 0 datd  дата документа
 1 kp    код подразделения (если равно 0 - акт списания)
 2 nomd  номер документа
 3 nzap  номер записи в документе
 4 ka    код автомобиля
 5 kv    код водителя
 6 kodsp для приходной записи код поставщика топлива(заправка или водитель) / для расходной записи 0
 7 tz    1-приход 2-расход
 8 kodtp код топлива
 9 kolih количество
10 kom   комментарий
11 kap   код автомобиля с которого передали топливо (Если kodsp это код водителя, иначе 0)
12 ktoz  кто записал
13 vrem  время записи
14 nz    номер по порядку документа выписанного в один и тотже день на один и тотже автомобиль
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);



/*105*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей списания топлива по объектам и счетам Upldok2 !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Upldok2 (\
datd  DATE not null default '0000-00-00',\
kp smallint not null default 0,\
nomd varchar(10) not null default '',\
kt varchar(20) not null default '',\
shet varchar(10) not null,\
ko int not null default 0,\
pr float(6,1) not null default 0,\
zt float(8,3) not null default 0,\
hs float(8,2) not null default 0,\
kv smallint not null default 0,\
ka smallint not null default 0,\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0,\
przg float(6,1) not null default 0,\
ztzg float(8,3) not null default 0,\
ztgr float(8,3) not null default 0,\
ztmh float(8,3) not null default 0,\
prgr int not null default 0,\
vrrd float(8,2) not null default 0,\
unique(datd,kp,nomd,kt,shet,ko))");

/*
0 datd  дата документа
1 kp    код подразделения
2 nomd  номер документа
3 kt    код топлива
4 shet  счет списания
5 ko    код объекта списания
6 pr    пробег город
7 zt    затраты топлива город
8 hs    отработанные часы
9 kv    код водителя
10 ka   код автомобиля
11 ktoz  кто записал
12 vrem  время записи
13 przg    пробег за городом
14 ztzg    затраты топлива за городом
15 ztgr  затраты топлива на перевоз груза
16 ztmh  затраты топлива на отработанное двигателем время
17 prgr  Тоннокилометры 
18 vrrd  Время отработанное двигателем
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*106*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы сальдо по топливу Uplsal !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplsal (\
god smallint not null default 0,\
ka smallint not null default 0,\
kv smallint not null default 0,\
kmt varchar(20) not null default '',\
sal double(10,2) not null default 0,\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0,\
unique(god,ka,kv,kmt))");

/*
0 god   год
1 ka    код автомобиля
2 kv    код водителя
3 kmt   код марки топлива
4 sal   сальдо
5 ktoz  кто записал
6 vrem  время записи
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*107*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы объектов списания топлива Uplost !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplost (\
kod int not null default 0 primary key,\
kgr smallint not null default 0,\
naik varchar(80) not null default '',\
shet varchar(10) not null default '',\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0,\
unique (naik))");

/*
0 kod   код объекта
1 kgr   код группы объекта
2 naik  наименование объекта
3 shet  счет списания объекта
4 ktoz  кто записал
5 vrem  время записи
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*108*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы групп объектов списания топлива Uplgost !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplgost (\
kod smallint not null default 0 primary key,\
naik varchar(80) not null default '',\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0)");

/*
0 kod   код группы
1 naik  наименование группы
2 ktoz  кто записал
3 vrem  время записи
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*109*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы средних цен на топливо Uplmtsc !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplmtsc (\
datsc DATE NOT NULL default '0000-00-00',\
kodt  char(20) not null default '',\
srcn  float(7,2) not null default 0,\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0,\
kp    float(7,3) not null default 0,\
unique (datsc,kodt))");

/*
0 datsc дата начала действия настройки
1 kodt  код топлива
2 srcn  средняя цена топлива
3 ktoz  кто записал
4 vrem  время записи
5 kp    коэффициент перевода литры => киллограммы
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/***********************************************/

/*110*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы спецификаций Specif !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Specif \
(kodi   int not null default 0,\
kodd    int not null default 0,\
kolih   double(20,10) not null default 0,\
koment  varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
kz      smallint not null default 0,\
ei      varchar(10) not null default '',\
unique(kodi,kodd))");

/*
0 kodi    код изделия
1 kodd    код детали
2 kolih   количество
3 koment  комментарий
4 ktoz	кто записал
5 vrem	время записи
6 kz    код записи 0-материалы 1-услуги
7 ei    единица измерения
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*111*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы реестра выданных налоговых накладных Reenn !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Reenn (\
nz int unsigned AUTO_INCREMENT NOT NULL,\
datd DATE not null default '0000-00-00',\
nnn varchar(20) not null default '',\
sumd double(10,2) not null default 0,\
snds double(10,2) not null default 0,\
mt tinyint not null default 0,\
inn varchar(30) not null default '',\
nkontr varchar(100) not null default '',\
mi tinyint not null default 0,\
datdi DATE not null default '0000-00-00',\
nomdi varchar(20) not null default '',\
koment varchar(100) not null default '',\
ktoi smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
gr varchar(10) not null default '',\
ko varchar(10) not null default '',\
vd char(8) not null default '',\
datv DATE not null default '0000-00-00',\
dnz int NOT NULL DEFAULT 0,\
nnni int NOT NULL DEFAULT 0,\
PRIMARY KEY (nz),\
index(datd,nnn))");

/************
0  nz      уникальный номер записи устанавливаемый базой данных автоматически
1  datd    дата документа
2  nnn     номер налоговой накладной
3  sumd    общая сумма по документу
4  snds    сумма НДС
5  mt      метка документа (каким НДС обкладывается)
6  inn     индивидуальный налоговый номер
7  nkontr  наименование контрагента
8  mi      метка импорта документа 1-материальный учёт 2-учёт услуг 3-учёт основных средств 4-учёт командировочных расходов 5-главная книга
9 datdi   дата документа импорта
10 nomdi   номер документа импорта
11 koment  комментарий
12 ktoi    Кто записал
13 vrem    Время записи
14 gr      код группы документа
15 ko      код операции в импортируемом документе
16 vd      вид документа
17 datv    дата выписки налоговой накладной
18 dnz     дневная нумерация записей
19 nnni    номер налоговой накладной в цифровой форме - для правильной сортрировки
***************/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*112*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы реестра полученных налоговых накладных Reenn1 !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Reenn1 (\
nz int unsigned AUTO_INCREMENT NOT NULL,\
datd DATE not null default '0000-00-00',\
nnn varchar(20) not null default '',\
sumd1 double(10,2) not null default 0,\
snds1 double(10,2) not null default 0,\
sumd2 double(10,2) not null default 0,\
snds2 double(10,2) not null default 0,\
sumd3 double(10,2) not null default 0,\
snds3 double(10,2) not null default 0,\
sumd4 double(10,2) not null default 0,\
snds4 double(10,2) not null default 0,\
inn varchar(30) not null default '',\
nkontr varchar(100) not null default '',\
mi tinyint not null default 0,\
datdi DATE not null default '0000-00-00',\
nomdi varchar(20) not null default '',\
koment varchar(100) not null default '',\
gr varchar(10) not null default '',\
ktoi smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
ko varchar(10) not null default '',\
dvd DATE NOT NULL default '0000-00-00',\
vd char(8) not null default '',\
PRIMARY KEY (nz),\
index(datd,nnn))");

/************
0  nz      уникальный номер записи устанавливаемый базой данных автоматически
1  datd    дата получения налоговой накладной
2  nnn     номер налоговой накладной
3  sumd1    общая сумма по документу
4  snds1    сумма НДС
5  sumd2    общая сумма по документу
6  snds2    сумма НДС
7  sumd3    общая сумма по документу
8  snds3    сумма НДС
9  sumd4    общая сумма по документу
10 snds4    сумма НДС
11 inn     индивидуальный налоговый номер
12 nkontr  наименование контрагента
13 mi      метка импорта документа 1-материальный учёт 2-учёт услуг 3-учёт основных средств 4-учёт командировочных расходов 5-главная книга
14 datdi   дата документа импорта
15 nomdi   номер документа импорта
16 koment  комментарий
17 gr      группа документа
18 ktoi    Кто записал
19 vrem    Время записи
20 ko      код операции в импортируемом документе
21 dvd     Дата выписки налоговой накладной
22 vd      вид документа
***************/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*113*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы групп полученных налоговых накладных Reegrup1 !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Reegrup1 \
(kod    varchar(10) not null default '' primary key,\
naik    varchar(80) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");
/*
0 kod   код группы
1 naik  наименование группы
2 ktoz  кто записал
3 vrem  время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*114*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы групп выданных налоговых накладных Reegrup2 !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Reegrup2 \
(kod    varchar(10) not null default '' primary key,\
naik    varchar(80) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");
/*
0 kod   код группы
1 naik  наименование группы
2 ktoz  кто записал
3 vrem  время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*115*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы заголовков доверенностей Uddok !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uddok \
(datd   DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
datds   DATE NOT NULL default '0000-00-00',\
komu    varchar(80) NOT NULL default '',\
post    varchar(80) NOT NULL default '',\
podok   varchar(50) NOT NULL default '',\
otis    varchar(100) NOT NULL default '',\
datn    DATE NOT NULL default '0000-00-00',\
nomn    varchar(100) NOT NULL default '',\
seriq varchar(20) NOT NULL default '',\
nomerd varchar(20) NOT NULL default '',\
datavd DATE NOT NULL default '0000-00-00',\
vidan  varchar(200) NOT NULL default '',\
dolg   varchar(50) NOT NULL default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(datd,nomd),\
index(nomd))");
/*
0  datd Дата выдачи доверенности
1  nomd номер доверенности
2  datds действительна до
3  komu  кому выдана доверенность
4  post  поставщик
5  podok по документа
6  otis  отметка об использовании
7  datn  дата наряда
8  nomn  номер наряда
9 seriq серия
10 nomerd номер документа
11 datavd дата выдачи документа
12 vidan  кем выдан
13 dolg   должность
14  ktoz  кто записал
15 vrem  время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);


/*116*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы записей в доверенности Uddok1 !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uddok1 \
(datd  DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
nz      smallint NOT NULL default 0,\
zapis   varchar(200) NOT NULL default '',\
ei      varchar(20) NOT NULL default '',\
kol     double(16,4) NOT NULL default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(datd,nomd,nz))");
/*
0  datd Дата выдачи доверенности
1  nomd номер доверенности
2  nz    уникальный номер записи
3  zapis содержимое записи
4  ei    единица измерения
5  kol   количество
6  ktoz  кто записал
7  vrem  время записи
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
  //iceb_menu_soob(bros,data->window);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы операторов icebuser\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE icebuser \
(login char(32) not null default '' primary key,\
fio varchar(255) not null default '',\
un  int unsigned not null default 0,\
kom varchar(255) not null default '',\
logz   varchar(32) not null default '',\
vrem   int unsigned not null default 0,\
gkd tinyint not null default 0,\
gkb tinyint not null default 0,\
gkn tinyint not null default 0,\
mud tinyint not null default 0,\
mub tinyint not null default 0,\
mun tinyint not null default 0,\
pdd tinyint not null default 0,\
pdb tinyint not null default 0,\
pdn tinyint not null default 0,\
zpd tinyint not null default 0,\
zpb tinyint not null default 0,\
zpn tinyint not null default 0,\
osd tinyint not null default 0,\
osb tinyint not null default 0,\
osn tinyint not null default 0,\
uud tinyint not null default 0,\
uub tinyint not null default 0,\
uun tinyint not null default 0,\
kod tinyint not null default 0,\
kob tinyint not null default 0,\
kon tinyint not null default 0,\
krd tinyint not null default 0,\
krb tinyint not null default 0,\
krn tinyint not null default 0,\
pld tinyint not null default 0,\
plb tinyint not null default 0,\
pln tinyint not null default 0,\
nnd tinyint not null default 0,\
nnb tinyint not null default 0,\
nnn tinyint not null default 0,\
udd tinyint not null default 0,\
udb tinyint not null default 0,\
udn tinyint not null default 0,\
unique(un))");

/*
 0 login  логин оператора
 1 fio    фамилия имя отчество
 2 un     уникальный номер оператора
 3 kom    коментарий
 4 logz   логин сделавшего запись
 5 vrem   Время записи
 6 gkd    если равно 0 то разрешена работа с подсистемой "Главная книга"
 7 gkb    если равно 0 то разрешена блокировка подсистемы "Главная книга"
 8 gkn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Главная книга"
 9 mud    если равно 0 то разрешена работа с подсистемой "Материальный учёт"
10 mub    если равно 0 то разрешена блокировка подсистемы "Материальный учёт"
11 mun    если равно 0 то разрешена работа с настроечными файлами подсистемы "Материальный учёт"
12 pdd    если равно 0 то разрешена работа с подсистемой "Платёжные документы"
13 pdb    если равно 0 то разрешена блокировка подсистемы "Платёжные документы"
14 pdn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Платёжные документы"
15 zpd    если равно 0 то разрешена работа с подсистемой "Заработная плата"
16 zpb    если равно 0 то разрешена блокировка подсистемы "Зароботная плата"
17 zpn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Зароботная плата"
18 osd    если равно 0 то разрешена работа с подсистемой "Учёт основных средств"
19 osb    если равно 0 то разрешена блокировка подсистемы "Учёт основных средств"
20 osn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт основных средств"
21 uud    если равно 0 то разрешена работа с подсистемой "Учёт услуг"
22 uub    если равно 0 то разрешена блокировка подсистемы "Учёт услуг"
23 uun    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт услуг"
24 kod    если равно 0 то разрешена работа с подсистемой "Учёт кассовых ордеров"
25 kob    если равно 0 то разрешена блокировка подсистемы "Учёт кассовых ордеров"
26 kon    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт кассовых ордеров"
27 krd    если равно 0 то разрешена работа с подсистемой "Учёт командировочных расходов"
28 krb    если равно 1 то разрешена блокировка подсистемы "Учёт командировочных расходов"
29 krn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт командироваочных расходов"
30 pld    если равно 0 то разрешена работа с подсистемой "Учёт путевых листов"
31 plb    если равно 0 то разрешена блокировка подсистемы "Учёт путувых листов"
32 pln    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт путевых листов"
33 nnd    если равно 0 то разрешена работа с подсистемой "Реестр налоговых накладных"
34 nnb    если равно 0 то разрешена блокировка подсистемы "Реестр налоговых накладных"
35 nnn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Реестр налоговых накладных"
36 udd    если равно 0 то разрешена работа с подсистемой "Учёт доверенностей"
37 udb    если равно 0 то разрешена блокировка подсистемы "Учёт доверенностей"
38 udn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт доверенностей"
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы блокироваок Blok\n"),data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Blok \
(kod int not null default 0,\
god smallint unsigned not null default 0,\
mes smallint unsigned not null default 0,\
log varchar(255) not null default '',\
shet varchar(255) not null default '',\
ktoz int not null default 0,\
vrem int unsigned not null default 0,\
unique(kod,god,mes))");

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*
0 kod  - код подсистемы
1 god  - год
2 mes  - месяц
3 log  - логины операторов которым отменена блокировка
4 shet - счёта которые заблокированы (только для подсистемы "Главная книга*)
5 ktoz - номер оператора сделавшего запись
6 vrem - время записи
*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы настроек Alx !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Alx \
(fil varchar(255) not null default '',\
ns int not null default 0,\
ktoz int not null default 0,\
vrem int unsigned not null default 0,\
str TEXT not null default '',\
unique(fil,ns))");

if(sql_zap(&bd,strsql) != 0)
 {
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*
0 fil  - имя файла
1 ns   - номер строки
2 ktoz - номер оператора сделавшего запись
3 vrem - время записи
4 str  - строка текста
*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы документации Doc!\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Doc \
(fil varchar(255) not null default '',\
ns int not null default 0,\
str TEXT not null default '',\
unique(fil,ns))");

if(sql_zap(&bd,strsql) != 0)
 {
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*
0 fil  - имя файла
1 ns   - номер строки
2 str  - строка текста
*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы налогов Nalog !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Nalog \
(vn int not null default 0,\
dndn DATE not null default '0000-00-00',\
pr double not null default 0,\
kom varchar(255) not null default '',\
ktoz int not null default 0,\
vrem int unsigned not null default 0,\
unique(vn,dndn))");

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
 } 
else
 {
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);
  sprintf(strsql,"insert into Nalog values (0,'1990-1-1',20.,'',%d,%ld)",
  getuid(),time(NULL));
  iceb_sql_zapis(strsql,1,0,data->window);
 }

/*
0 vn   - вид налога 0-НДС
1 dndn - Дата начала действия налога
2 pr   - процент
3 kom  - коментарий
4 ktoz - номер оператора сделавшего запись
5 vrem - время записи
*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы процентов Zaresv !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zaresv \
(kf int not null default 0,\
datnd DATE not null default '0000-00-00',\
pr float not null default 0.,\
pr1 float not null default 0.,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(kf,datnd))");

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
 } 
/******
0 kf    - код фонда
1 datnd - дата начала действия
2 pr    - процент начисления на фонд зарплаты
3 pr1   - процент удержания с работника
4 ktoz  - кто записал
5 vrem  - время записи
**/

/*Загружаем настроечные файлы в базу данных*/
iceb_alxzag(data->fromnsi.ravno(),0,data->view,data->buffer,data->window);

/*Загружаем документацию для терминальной версии*/
iceb_printw(iceb_u_toutf("Загружаем документацию!\n"),data->buffer,data->view);
docinw(data->fromdoc.ravno(),data->window);


sprintf(strsql,"Количество таблиц:%.f\n",data->kolstr1);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


iceb_printw_vr(data->vremn,data->buffer,data->view);


gtk_label_set_text(GTK_LABEL(data->label),iceb_u_toutf("Создание базы завершено"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);







return(FALSE);

}

