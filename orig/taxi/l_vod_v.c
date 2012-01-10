/*$Id: l_vod_v.c,v 1.4 2011-02-21 07:36:21 sasa Exp $*/
/*01.03.2009	21.06.2006	Белых А.И.	l_vod_v.c
Ввод и корректировка категорий работников
*/
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "taxi.h"
#include "l_vod.h"
enum
 {
  E_KODVOD,
  E_FIO,
  E_ADRES,
  E_TELEF,
  E_GOSN,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_vod_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_vod_rek rk;
  class iceb_u_str kod_vod;  
  
  l_vod_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.kodvod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KODVOD]))));
    rk.fio.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_FIO]))));
   }
 };


gboolean   l_vod_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_vod_v_data *data);
void  l_vod_v_knopka(GtkWidget *widget,class l_vod_v_data *data);
void    l_vod_v_vvod(GtkWidget *widget,class l_vod_v_data *data);
int l_vod_pk(const char *god,GtkWidget*);
int l_vod_zap(class l_vod_v_data *data);
void l_vod_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);


extern SQL_baza  bd;
extern char      *name_system;

int l_vod_v(class iceb_u_str *kod_vod,GtkWidget *wpredok)
{

class l_vod_v_data data;
char strsql[300];
iceb_u_str kikz;
data.kod_vod.new_plus(kod_vod->ravno());

data.rk.clear_data();

if(data.kod_vod.getdlinna() >  1)
 {
  data.rk.kodvod.new_plus(data.kod_vod.ravno());
  sprintf(strsql,"select * from Taxivod where kod=%s",data.kod_vod.ravno());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.rk.kodvod.new_plus(row[0]);
  data.rk.fio.new_plus(row[1]);
  data.rk.adres.new_plus(row[2]);
  data.rk.telef.new_plus(row[3]);
  data.rk.gosn.new_plus(row[4]);

  kikz.plus(iceb_kikz(row[6],row[7],wpredok));
  
 }
else
 {
  sprintf(strsql,"%d",iceb_get_new_kod("Taxivod",1,wpredok));
  data.rk.kodvod.new_plus(strsql);
 } 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.kod_vod.getdlinna() <= 1)
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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_vod_v_key_press),&data);

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


label=gtk_label_new(gettext("Код"));
gtk_box_pack_start (GTK_BOX (hbox[E_KODVOD]), label, FALSE, FALSE, 0);

data.entry[E_KODVOD] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KODVOD]), data.entry[E_KODVOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODVOD]), "activate",GTK_SIGNAL_FUNC(l_vod_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODVOD]),data.rk.kodvod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODVOD]),(gpointer)E_KODVOD);

label=gtk_label_new(gettext("Фамилия"));
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), label, FALSE, FALSE, 0);

data.entry[E_FIO] = gtk_entry_new_with_max_length (59);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_FIO]), "activate",GTK_SIGNAL_FUNC(l_vod_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.rk.fio.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_FIO]),(gpointer)E_FIO);

label=gtk_label_new(gettext("Адрес"));
gtk_box_pack_start (GTK_BOX (hbox[E_ADRES]), label, FALSE, FALSE, 0);

data.entry[E_ADRES] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox[E_ADRES]), data.entry[E_ADRES], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ADRES]), "activate",GTK_SIGNAL_FUNC(l_vod_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ADRES]),data.rk.adres.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ADRES]),(gpointer)E_ADRES);

label=gtk_label_new(gettext("Телефон"));
gtk_box_pack_start (GTK_BOX (hbox[E_TELEF]), label, FALSE, FALSE, 0);

data.entry[E_TELEF] = gtk_entry_new_with_max_length (39);
gtk_box_pack_start (GTK_BOX (hbox[E_TELEF]), data.entry[E_TELEF], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TELEF]), "activate",GTK_SIGNAL_FUNC(l_vod_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TELEF]),data.rk.telef.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TELEF]),(gpointer)E_TELEF);

label=gtk_label_new(gettext("Гос. номер"));
gtk_box_pack_start (GTK_BOX (hbox[E_GOSN]), label, FALSE, FALSE, 0);

data.entry[E_GOSN] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_GOSN]), data.entry[E_GOSN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GOSN]), "activate",GTK_SIGNAL_FUNC(l_vod_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOSN]),data.rk.gosn.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GOSN]),(gpointer)E_GOSN);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_vod_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_vod_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[1]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 kod_vod->new_plus(data.rk.kodvod.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_vod_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_vod_v_data *data)
{
//char  bros[300];

//printf("l_vod_v_key_press\n");
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
void  l_vod_v_knopka(GtkWidget *widget,class l_vod_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_vod_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_vod_zap(data) == 0)
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

void    l_vod_v_vvod(GtkWidget *widget,class l_vod_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_vod_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KODVOD:
    data->rk.kodvod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_FIO:
    data->rk.fio.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int l_vod_zap(class l_vod_v_data *data)
{
char strsql[1000];
//SQL_str row;
time_t   vrem;
time(&vrem);


if(data->rk.kodvod.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введен код !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }

if(data->rk.fio.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введено наименование !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }


if(iceb_u_SRAV(data->kod_vod.ravno(),data->rk.kodvod.ravno(),0) != 0)
  if(l_vod_pk(data->rk.kodvod.ravno(),data->window) != 0)
     return(1);

short metkakor=0;

if(data->kod_vod.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Taxivod \
values ('%s','%s','%s','%s','%s',%d,%d,%ld)",
   data->rk.kodvod.ravno_filtr(),
   data->rk.fio.ravno_filtr(),
   data->rk.adres.ravno_filtr(),
   data->rk.telef.ravno_filtr(),
   data->rk.gosn.ravno_filtr(),
   0,
   iceb_getuid(data->window),vrem);
 }
else
 {
  
  if(iceb_u_SRAV(data->kod_vod.ravno(),data->rk.kodvod.ravno(),0) != 0)
   {

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Taxivod \
set \
kod='%s',\
fio='%s',\
adres='%s',\
telef='%s',\
gosn='%s',\
ktoi=%d,\
vrem=%ld \
where kod=%d",
   data->rk.kodvod.ravno_filtr(),
   data->rk.fio.ravno_filtr(),
   data->rk.adres.ravno_filtr(),
   data->rk.telef.ravno_filtr(),
   data->rk.gosn.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->kod_vod.ravno_atoi());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

if(metkakor == 1)
 l_vod_kkvt(data->kod_vod.ravno(),data->rk.kodvod.ravno(),data->window);

return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_vod_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[300];

sprintf(strsql,"select kod from Taxivod where kod='%s'",kod);
//printf("l_vod_pk-%s\n",strsql);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Такая запись уже есть !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

return(0);

}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void l_vod_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[300];

sprintf(strsql,"update Taxizak set kv='%s' where kv='%s'",nkod,skod);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   iceb_u_str SOOB;
   SOOB.plus(gettext("У вас нет полномочий для выполнения этой операции !"));
   iceb_menu_soob(&SOOB,wpredok);
   return;
  }
 else
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
 }


}
