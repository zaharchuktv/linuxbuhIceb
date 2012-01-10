/*$Id: l_uplscnt_v.c,v 1.11 2011-02-21 07:35:54 sasa Exp $*/
/*18.02.2008	21.06.2006	Белых А.И.	l_uplscnt_v.c
Ввод и корректировка средневзвешенных цен на топливо
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
#include "l_uplscnt.h"
enum
 {
  E_DATA,
  E_KOD_TOP,
  E_CENA,
  E_KOEF,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_uplscnt_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter;

  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_uplscnt_rek rk;
  class iceb_u_str data_k;  
  class iceb_u_str kod_top_k;  
  
  l_uplscnt_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.data.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA]))));
    rk.kod_top.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_TOP]))));
    rk.cena.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_CENA]))));
    rk.koef.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOEF]))));
   }
 };


gboolean   l_uplscnt_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplscnt_v_data *data);
void  l_uplscnt_v_knopka(GtkWidget *widget,class l_uplscnt_v_data *data);
void    l_uplscnt_v_vvod(GtkWidget *widget,class l_uplscnt_v_data *data);
int l_uplscnt_pk(class l_uplscnt_v_data *data);
int l_uplscnt_zap(class l_uplscnt_v_data *data);

void  l_uplscnt_v_e_knopka(GtkWidget *widget,class l_uplscnt_v_data *data);

extern SQL_baza  bd;
extern char      *name_system;

int l_uplscnt_v(class iceb_u_str *data_k,class iceb_u_str *kod_top_k,GtkWidget *wpredok)
{

class l_uplscnt_v_data data;
char strsql[512];
iceb_u_str kikz;
data.data_k.new_plus(data_k->ravno());
data.kod_top_k.new_plus(kod_top_k->ravno());

data.rk.clear_data();

if(data.data_k.getdlinna() >  1)
 {
  data.rk.data.new_plus(data.data_k.ravno());
  sprintf(strsql,"select * from Uplmtsc where datsc='%s' and kodt='%s'",data_k->ravno_sqldata(),kod_top_k->ravno());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.rk.kod_top.new_plus(row[1]);
  data.rk.cena.new_plus(row[2]);
  kikz.plus(iceb_kikz(row[3],row[4],wpredok));
  data.rk.koef.new_plus(row[5]);
  
 }
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.data_k.getdlinna() <= 1)
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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_uplscnt_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox = gtk_vbox_new (FALSE,1);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE,1);
GtkWidget *hboxknop = gtk_hbox_new (FALSE,1);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


label=gtk_label_new(gettext("Дата"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), label, FALSE, FALSE,1);

data.entry[E_DATA] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.entry[E_DATA], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA]), "activate",GTK_SIGNAL_FUNC(l_uplscnt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA]),data.rk.data.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA]),(gpointer)E_DATA);

//label=gtk_label_new(gettext("Код топлива"));
//gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TOP]), label, FALSE, FALSE,1);

sprintf(strsql,"%s",gettext("Код топлива"));
data.knopka_enter=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TOP]), data.knopka_enter, FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter),"clicked",GTK_SIGNAL_FUNC(l_uplscnt_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter),(gpointer)E_KOD_TOP);

GtkTooltips *tooltips_enter;

tooltips_enter=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter,data.knopka_enter,gettext("Выбор топлива"),NULL);

data.entry[E_KOD_TOP] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TOP]), data.entry[E_KOD_TOP], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_TOP]), "activate",GTK_SIGNAL_FUNC(l_uplscnt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_TOP]),data.rk.kod_top.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_TOP]),(gpointer)E_KOD_TOP);

label=gtk_label_new(gettext("Цена"));
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), label, FALSE, FALSE,1);

data.entry[E_CENA] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), data.entry[E_CENA], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_CENA]), "activate",GTK_SIGNAL_FUNC(l_uplscnt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA]),data.rk.cena.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_CENA]),(gpointer)E_CENA);

label=gtk_label_new(gettext("Коэффициент"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOEF]), label, FALSE, FALSE,1);

data.entry[E_KOEF] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOEF]), data.entry[E_KOEF], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOEF]), "activate",GTK_SIGNAL_FUNC(l_uplscnt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOEF]),data.rk.koef.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOEF]),(gpointer)E_KOEF);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_uplscnt_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_uplscnt_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 {
  data_k->new_plus(data.rk.data.ravno());
  kod_top_k->new_plus(data.rk.kod_top.ravno());
 }
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_uplscnt_v_e_knopka(GtkWidget *widget,class l_uplscnt_v_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_KOD_TOP:
    if(l_uplmt(1,&kod,&naim,data->window) == 0)
     data->rk.kod_top.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_TOP]),data->rk.kod_top.ravno_toutf());
     
    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uplscnt_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplscnt_v_data *data)
{
//char  bros[512];

//printf("l_uplscnt_v_key_press\n");
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
void  l_uplscnt_v_knopka(GtkWidget *widget,class l_uplscnt_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uplscnt_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_uplscnt_zap(data) == 0)
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

void    l_uplscnt_v_vvod(GtkWidget *widget,class l_uplscnt_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uplscnt_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATA:
    data->rk.data.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KOD_TOP:
    data->rk.kod_top.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int l_uplscnt_zap(class l_uplscnt_v_data *data)
{
char strsql[1024];
//SQL_str row;
time_t   vrem;
time(&vrem);


if(data->rk.data.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не ввели дату!"),data->window);
  return(1);
 }

if(data->rk.kod_top.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не ввели код топлива!"),data->window);
  return(1);
 }

if(data->rk.data.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Неправильно ввели дату!"),data->window);
  return(1);
 }

sprintf(strsql,"select kodt from Uplmt where kodt='%s'",data->rk.kod_top.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не нашли код топлива"),data->rk.kod_top.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

if(iceb_pbpds(data->rk.data.ravno(),data->window) != 0)
  return(1);


if(iceb_pbpds(data->data_k.ravno(),data->window) != 0)
  return(1);


if(iceb_u_SRAV(data->kod_top_k.ravno(),data->rk.kod_top.ravno(),0) != 0 || \
iceb_u_sravmydat(data->data_k.ravno(),data->rk.data.ravno()) != 0)
  if(l_uplscnt_pk(data) != 0)
     return(1);

if(data->data_k.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Uplmtsc \
values ('%s','%s',%.2f,%d,%ld,%.3f)",
   data->rk.data.ravno_sqldata(),
   data->rk.kod_top.ravno_filtr(),
   data->rk.cena.ravno_atof(),
   iceb_getuid(data->window),vrem,
   data->rk.koef.ravno_atof());
 }
else
 {
  
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Uplmtsc \
set \
datsc='%s',\
kodt='%s',\
srcn=%.2f,\
ktoz=%d,\
vrem=%ld,\
kp=%.3f \
where datsc='%s' and kodt='%s'",
   data->rk.data.ravno_sqldata(),
   data->rk.kod_top.ravno_filtr(),
   data->rk.cena.ravno_atof(),
   iceb_getuid(data->window),vrem,
   data->rk.koef.ravno_atof(),
   data->data_k.ravno_sqldata(),
   data->kod_top_k.ravno_filtr());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);


return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_uplscnt_pk(class l_uplscnt_v_data *data)
{
char strsql[512];

sprintf(strsql,"select datsc from Uplmtsc where datsc='%s' and kodt='%s'",data->rk.data.ravno_sqldata(),data->rk.kod_top.ravno_filtr());
//printf("l_uplscnt_pk-%s\n",strsql);

if(iceb_sql_readkey(strsql,data->window) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Такая запись уже есть !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 }

return(0);

}











