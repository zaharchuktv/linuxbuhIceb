/*$Id: l_kasvedz_v.c,v 1.9 2011-02-21 07:35:53 sasa Exp $*/
/*30.06.2009	09.06.2009	Белых А.И.	l_kasvedz_v.c
Ввод и корректировка
*/
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
#include "l_kasvedz.h"
enum
 {
  E_DATA,
  E_NOMD,
  E_SUMA,
  E_KOMENT,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_kasvedz_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_kasvedz_rek rk;
  class iceb_u_str datadok;  
  class iceb_u_str nomdok;  
  l_kasvedz_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA]))));
    rk.nomd.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMD]))));
    rk.suma.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SUMA]))));
    rk.koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT]))));
   }
 };


gboolean   l_kasvedz_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_kasvedz_v_data *data);
void  l_kasvedz_v_knopka(GtkWidget *widget,class l_kasvedz_v_data *data);
void    l_kasvedz_v_vvod(GtkWidget *widget,class l_kasvedz_v_data *data);
int l_kasvedz_zap(class l_kasvedz_v_data *data);

void  l_kasvedz_v_e_knopka(GtkWidget *widget,class l_kasvedz_v_data *data);

extern SQL_baza  bd;
extern char      *name_system;

int l_kasvedz_v(class iceb_u_str *datadok,class iceb_u_str *nomdok,GtkWidget *wpredok)
{

class l_kasvedz_v_data data;
char strsql[512];
iceb_u_str kikz;
data.datadok.new_plus(datadok->ravno());
data.nomdok.new_plus(nomdok->ravno());

data.rk.clear_data();

if(data.nomdok.getdlinna() >  1)
 {
  sprintf(strsql,"select * from Kasnomved where datd='%s' and nomv='%s'",data.datadok.ravno_sqldata(),data.nomdok.ravno());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  data.rk.datan.new_plus(iceb_u_datzap(row[0]));
  data.rk.nomd.new_plus(row[2]);
  data.rk.suma.new_plus(row[3]);
  data.rk.koment.new_plus(row[4]);
  kikz.plus(iceb_kikz(row[5],row[6],wpredok));
  
 }
else
 {
  short dt,mt,gt;
  iceb_u_poltekdat(&dt,&mt,&gt);
  sprintf(strsql,"%d",l_kasvedz_v_maxnd(gt,wpredok));
  data.rk.nomd.new_plus(strsql);
 } 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.datadok.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));
  label=gtk_label_new(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));
  iceb_u_str repl;
  repl.plus(gettext("Корректировка записи"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno_toutf());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_kasvedz_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);


GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s (%s)",gettext("Дата начала"),gettext("д.м.г"));
data.knopka_enter[E_DATA]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.knopka_enter[E_DATA], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA]),"clicked",GTK_SIGNAL_FUNC(l_kasvedz_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA]),(gpointer)E_DATA);
tooltips_enter[E_DATA]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA],data.knopka_enter[E_DATA],gettext("Выбор даты"),NULL);

data.entry[E_DATA] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.entry[E_DATA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA]), "activate",GTK_SIGNAL_FUNC(l_kasvedz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA]),data.rk.datan.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA]),(gpointer)E_DATA);


sprintf(strsql,"%s",gettext("Номер документа"));
data.knopka_enter[E_NOMD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMD]), data.knopka_enter[E_NOMD], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_NOMD]),"clicked",GTK_SIGNAL_FUNC(l_kasvedz_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_NOMD]),(gpointer)E_NOMD);
tooltips_enter[E_NOMD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_NOMD],data.knopka_enter[E_NOMD],gettext("Получение номера документа"),NULL);


data.entry[E_NOMD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMD]), data.entry[E_NOMD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMD]), "activate",GTK_SIGNAL_FUNC(l_kasvedz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMD]),data.rk.nomd.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMD]),(gpointer)E_NOMD);

label=gtk_label_new(gettext("Сумма"));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), label, FALSE, FALSE, 0);

data.entry[E_SUMA] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMA]), "activate",GTK_SIGNAL_FUNC(l_kasvedz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.rk.suma.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMA]),(gpointer)E_SUMA);


label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(l_kasvedz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.rk.koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_kasvedz_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_kasvedz_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

if(data.voz == 0)
 {
  datadok->new_plus(data.rk.datan.ravno());
  nomdok->new_plus(data.rk.nomd.ravno());
 }

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_kasvedz_v_e_knopka(GtkWidget *widget,class l_kasvedz_v_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);
short god=0;
switch (knop)
 {
  case E_DATA:

    if(iceb_calendar(&data->rk.datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA]),data->rk.datan.ravno());
      
    return;  

  case E_NOMD:
     data->rk.datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATA]))));
     god=data->rk.datan.ravno_god();
     if(god == 0)
       god=iceb_u_poltekgod();
     data->rk.datan.new_plus(l_kasvedz_v_maxnd(god,data->window));
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMD]),data->rk.datan.ravno());
      
    return;  


   
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_kasvedz_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_kasvedz_v_data *data)
{
//char  bros[512];

//printf("l_kasvedz_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");

    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");

    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_kasvedz_v_knopka(GtkWidget *widget,class l_kasvedz_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_kasvedz_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_kasvedz_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  


  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_kasvedz_v_vvod(GtkWidget *widget,class l_kasvedz_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_kasvedz_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATA:
    data->rk.datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NOMD:
    data->rk.nomd.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int l_kasvedz_zap(class l_kasvedz_v_data *data)
{
char strsql[1024];
//SQL_str row;
time_t   vrem;
time(&vrem);

if(data->rk.datan.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата !"),data->window);
  return(1);
 }

if(data->rk.nomd.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели номер документа!"),data->window);
  return(1);
 }
if(data->rk.suma.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена сумма!"),data->window);
  return(1);
 }

if(data->rk.koment.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели комментарий!"),data->window);
  return(1);
 }

if(data->datadok.ravno_god() != data->rk.datan.ravno_god() ||\
iceb_u_SRAV(data->nomdok.ravno(),data->rk.nomd.ravno(),0) != 0)
  {
   sprintf(strsql,"select god from Kasnomved where god=%d and nomv='%s'",
   data->rk.datan.ravno_god(),data->rk.nomd.ravno());
   if(iceb_sql_readkey(strsql,data->window) > 0)
    {
     sprintf(strsql,gettext("Документ с номером %s уже есть!"),data->rk.nomd.ravno());
     iceb_menu_soob(strsql,data->window);
     return(1);
   
    }

  }

if(data->datadok.getdlinna() <= 1)
 {
  sprintf(strsql,"insert into Kasnomved \
values ('%s',%d,'%s',%.2f,'%s',%d,%ld,'')",
  data->rk.datan.ravno_sqldata(),
  data->rk.datan.ravno_god(),
  data->rk.nomd.ravno_filtr(),
  data->rk.suma.ravno_atof(),
  data->rk.koment.ravno_filtr(),
  iceb_getuid(data->window),
  time(NULL));

 }
else
 {
  
  sprintf(strsql,"update Kasnomved \
set \
datd='%s',\
god=%d,\
nomv='%s',\
suma=%.2f,\
koment='%s',\
ktoi=%d,\
vrem=%ld \
where god=%d and nomv=%s",
  data->rk.datan.ravno_sqldata(),
  data->rk.datan.ravno_god(),
  data->rk.nomd.ravno_filtr(),
  data->rk.suma.ravno_atof(),
  data->rk.koment.ravno_filtr(),
  iceb_getuid(data->window),
  time(NULL),
  data->datadok.ravno_god(),
  data->nomdok.ravno_filtr());
   
 }

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


return(0);

}
/********************************************/
/*Определение максимального номера документа*/
/********************************************/
int l_kasvedz_v_maxnd(short god,GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
int nomer_ved=0;
int nm=0;
int kolstr=0;
sprintf(strsql,"select nomv from Kasnomved where god=%d",god);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }

while(cur.read_cursor(&row) != 0)
 {
  nm=atoi(row[0]);
  if(nm > nomer_ved)
   nomer_ved=nm;
 }
return(++nomer_ved); 
}
