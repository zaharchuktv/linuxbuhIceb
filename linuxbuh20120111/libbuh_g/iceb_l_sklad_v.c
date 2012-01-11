/*$Id: iceb_l_sklad_v.c,v 1.20 2011-01-13 13:49:59 sasa Exp $*/
/*11.11.2008	05.05.2004	Белых А.И.	iceb_l_sklad_v.c
Ввод и корректировка склада
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "iceb_libbuh.h"
#include "iceb_l_sklad.h"

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

enum
 {
  E_KOD,
  E_NAIM,
  E_FIO,
  E_DOLG,
  KOLENTER  
 };

class iceb_l_sklad_v
 {
  public:
    
  GtkWidget *radiobutton2[2];
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class  sklad_rek rk;
  
  class iceb_u_str sklad_i;
  
  iceb_l_sklad_v() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[0])) == TRUE)
     rk.metka_isp_skl=0;
    else
     rk.metka_isp_skl=1;
   }
 };

gboolean   sklad_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_sklad_v *data);
void  sklad_v_knopka(GtkWidget *widget,class iceb_l_sklad_v *data);
void    sklad_v_vvod(GtkWidget *widget,class iceb_l_sklad_v *data);
int sklad_pk(const char *god,GtkWidget*);
int sklad_zap(class iceb_l_sklad_v *data);
void sklad_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);


int iceb_l_sklad_v(class iceb_u_str *sklad_k,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str kikz;
class iceb_l_sklad_v data;
data.sklad_i.new_plus(sklad_k->ravno());
    
data.rk.clear_zero();

if(data.sklad_i.getdlinna() > 1)
 {
  data.rk.kod.new_plus(data.sklad_i.ravno());
  sprintf(strsql,"select * from Sklad where kod=%s",data.sklad_i.ravno());
  SQL_str row;
  SQLCURSOR cur;
  if(sql_readkey(&bd,strsql,&row,&cur) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.rk.fmol.new_plus(row[2]);
  data.rk.dolg.new_plus(row[3]);
  data.rk.naim.new_plus(row[1]);
  kikz.plus(iceb_kikz(row[4],row[5],wpredok));
  data.rk.metka_isp_skl=atoi(row[6]);  
 }
else
 {
  sprintf(strsql,"%d",iceb_get_new_kod("Sklad",0,wpredok));
//  printf("%s\n",strsql);
  data.rk.kod.new_plus(strsql);
 }
  
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.sklad_i.getdlinna() <= 1)
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
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),&data.window);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(sklad_v_key_press),&data);

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
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 2);
// gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

GtkWidget *separator1=gtk_hseparator_new();
GtkWidget *separator3=gtk_hseparator_new();
gtk_box_pack_start (GTK_BOX (vbox),separator1, TRUE, TRUE, 2);

//Вставляем радиокнопки

data.radiobutton2[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Используется"));
data.radiobutton2[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton2[0]),gettext("Неиспользуется"));

gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[0], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[1], TRUE, TRUE, 1);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[data.rk.metka_isp_skl]),TRUE); //Устанавливем активной кнопку

gtk_box_pack_start (GTK_BOX (vbox),separator3, TRUE, TRUE, 2);

  gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 2);
//gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


label=gtk_label_new(gettext("Код"));
data.entry[E_KOD] = gtk_entry_new_with_max_length (4);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(sklad_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

label=gtk_label_new(gettext("Наименование"));
data.entry[E_NAIM] = gtk_entry_new_with_max_length (80);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(sklad_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);

label=gtk_label_new(gettext("Фамилия материально-ответственного"));
data.entry[E_FIO] = gtk_entry_new_with_max_length (29);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_FIO]), "activate",GTK_SIGNAL_FUNC(sklad_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.rk.fmol.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_FIO]),(gpointer)E_FIO);

label=gtk_label_new(gettext("Должность материально-ответственного"));
data.entry[E_DOLG] = gtk_entry_new_with_max_length (29);
gtk_box_pack_start (GTK_BOX (hbox[E_DOLG]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DOLG]), data.entry[E_DOLG], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DOLG]), "activate",GTK_SIGNAL_FUNC(sklad_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DOLG]),data.rk.dolg.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DOLG]),(gpointer)E_DOLG);

GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],data.knopka[PFK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK2]),"clicked",GTK_SIGNAL_FUNC(sklad_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK2]),(gpointer)PFK2);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],data.knopka[PFK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK10]),"clicked",GTK_SIGNAL_FUNC(sklad_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK10]),(gpointer)PFK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[1]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0)
 sklad_k->new_plus(data.rk.kod.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
 
return(data.voz);

}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   sklad_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_sklad_v *data)
{
//char  bros[512];

//printf("sklad_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PFK2]),"clicked");

    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PFK10]),"clicked");

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
void  sklad_v_knopka(GtkWidget *widget,class iceb_l_sklad_v *data)
{
//char bros[512];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("sklad_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case PFK2:
    data->read_rek();
    if(sklad_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  


  case PFK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    sklad_v_vvod(GtkWidget *widget,class iceb_l_sklad_v *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("sklad_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->rk.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NAIM:
    data->rk.naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_FIO:
    data->rk.fmol.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_DOLG:
    data->rk.dolg.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter++;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int sklad_zap(class iceb_l_sklad_v *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);

//printf("sklad_v_zap\n");

if(data->rk.kod.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введён код !"),data->window);
  return(1);
 }
if(data->rk.kod.ravno_atoi() == 0)
 {
  iceb_menu_soob(gettext("Код должен быть цифровым и не равным нолю!"),data->window);
  return(1);
 }

if(data->rk.naim.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введено наименование !"),data->window);
  return(1);
 }

short metkakor=0;

if(data->sklad_i.getdlinna() <= 1)
 {

  if(sklad_pk(data->rk.kod.ravno(),data->window) != 0)
   return(1);
  
  sprintf(strsql,"insert into Sklad \
values ('%s','%s','%s','%s',%d,%ld,%d)",
   data->rk.kod.ravno_filtr(),
   data->rk.naim.ravno_filtr(),
   data->rk.fmol.ravno_filtr(),
   data->rk.dolg.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.metka_isp_skl);
 }
else
 {
  
  if(iceb_u_SRAV(data->sklad_i.ravno(),data->rk.kod.ravno(),0) != 0)
   {
    if(sklad_pk(data->rk.kod.ravno(),data->window) != 0)
     return(1);

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Sklad \
set \
kod='%s',\
naik='%s',\
fmol='%s',\
dolg='%s',\
ktoz=%d,\
vrem=%ld,\
mi=%d \
where kod='%s'",
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   data->rk.fmol.ravno_filtr(),
   data->rk.dolg.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.metka_isp_skl,
   data->sklad_i.ravno());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);

if(metkakor == 1)
 sklad_kkvt(data->sklad_i.ravno(),data->rk.kod.ravno(),data->window);


return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int sklad_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Sklad where kod=%s",kod);
//printf("sklad_pk-%s\n",strsql);

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

void sklad_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Kart set sklad=%s where sklad=%s",
nkod,skod);
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

sprintf(strsql,"update Zkart set sklad=%s where sklad=%s",
nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

sprintf(strsql,"update Dokummat set sklad=%s where sklad=%s",
nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

sprintf(strsql,"update Dokummat1 set sklad=%s where sklad=%s",
nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);


sprintf(strsql,"update Dokummat2 set sklad=%s where sklad=%s",
nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

sprintf(strsql,"update Dokummat3 set sklad=%s where sklad=%s",
nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

sprintf(strsql,"update Uplouot set skl=%s where skl=%s",
nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

}
