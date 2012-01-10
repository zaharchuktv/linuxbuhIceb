/*$Id: l_uosgrnu_v.c,v 1.16 2011-03-28 06:55:46 sasa Exp $*/
/*13.04.2009	18.10.2007	Белых А.И.	l_uosgrnu_v.c
Ввод и корректировка групп для налогового учёта для учёта основных средств
*/
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
#include "l_uosgrnu.h"
enum
 {
  E_KOD,
  E_NAIM,
  E_PROC,
  E_TA,
  E_AR,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK10,
  KOL_PFK
 };

class l_uosgrnu_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt;
  GtkWidget *opt1;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_uosgrnu_rek rk;
  class iceb_u_str kod_podr;  
  
  l_uosgrnu_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD]))));
    rk.naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM]))));
    rk.kof.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PROC]))));
   }
 };


gboolean   l_uosgrnu_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosgrnu_v_data *data);
void  l_uosgrnu_v_knopka(GtkWidget *widget,class l_uosgrnu_v_data *data);
void    l_uosgrnu_v_vvod(GtkWidget *widget,class l_uosgrnu_v_data *data);
int l_uosgrnu_pk(const char *god,GtkWidget*);
int l_uosgrnu_zap(class l_uosgrnu_v_data *data);
void l_uosgrnu_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);


extern SQL_baza  bd;
extern char      *name_system;

int l_uosgrnu_v(class iceb_u_str *kod_podr,GtkWidget *wpredok)
{

class l_uosgrnu_v_data data;
char strsql[512];
iceb_u_str kikz;
data.kod_podr.new_plus(kod_podr->ravno());

data.rk.clear_data();

if(data.kod_podr.getdlinna() >  1)
 {
  data.rk.kod.new_plus(data.kod_podr.ravno());
  sprintf(strsql,"select * from Uosgrup where kod='%s'",data.kod_podr.ravno());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.rk.naim.new_plus(row[1]);
  data.rk.kof.new_plus(row[2]);
  kikz.plus(iceb_kikz(row[3],row[4],wpredok));
  data.rk.metka_ta=atoi(row[5]);
  data.rk.metka_ar=atoi(row[6]);
 }
else
 {
//  sprintf(strsql,"%d",iceb_get_new_kod("Uosgrup",1,wpredok));
//  data.rk.kod.new_plus(strsql);
 } 

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.kod_podr.getdlinna() <= 1)
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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_uosgrnu_v_key_press),&data);

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
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


label=gtk_label_new(gettext("Код"));
data.entry[E_KOD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(l_uosgrnu_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

label=gtk_label_new(gettext("Наименование"));
data.entry[E_NAIM] = gtk_entry_new_with_max_length (60);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(l_uosgrnu_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);

label=gtk_label_new(gettext("Годовой процент амортизации"));
data.entry[E_PROC] = gtk_entry_new_with_max_length (60);
gtk_box_pack_start (GTK_BOX (hbox[E_PROC]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_PROC]), data.entry[E_PROC], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PROC]), "activate",GTK_SIGNAL_FUNC(l_uosgrnu_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PROC]),data.rk.kof.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PROC]),(gpointer)E_PROC);

label=gtk_label_new(gettext("Тип амортизации"));
gtk_box_pack_start (GTK_BOX (hbox[E_TA]), label, FALSE, FALSE, 0);

class iceb_u_spisok menu_vib;
menu_vib.plus(gettext("Пообъектно"));
menu_vib.plus(gettext("В целом по группе"));

iceb_pm_vibor(&menu_vib,&data.opt,&data.rk.metka_ta);
gtk_box_pack_start (GTK_BOX (hbox[E_TA]), data.opt, FALSE, FALSE, 0);

label=gtk_label_new(gettext("Алгоритм амортизации"));
gtk_box_pack_start (GTK_BOX (hbox[E_AR]), label, FALSE, FALSE, 0);

menu_vib.free_class();

menu_vib.plus(gettext("От начальной балансовой стоимости"));
menu_vib.plus(gettext("От остаточной балансовой стоимости"));

iceb_pm_vibor(&menu_vib,&data.opt1,&data.rk.metka_ar);
gtk_box_pack_start (GTK_BOX (hbox[E_AR]), data.opt1, FALSE, FALSE, 0);

GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],data.knopka[PFK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK2]),"clicked",GTK_SIGNAL_FUNC(l_uosgrnu_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK2]),(gpointer)PFK2);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],data.knopka[PFK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK10]),"clicked",GTK_SIGNAL_FUNC(l_uosgrnu_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK10]),(gpointer)PFK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 kod_podr->new_plus(data.rk.kod.ravno());
return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uosgrnu_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosgrnu_v_data *data)
{
//char  bros[512];

//printf("l_uosgrnu_v_key_press\n");
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
void  l_uosgrnu_v_knopka(GtkWidget *widget,class l_uosgrnu_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uosgrnu_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case PFK2:
    data->read_rek();
    if(l_uosgrnu_zap(data) == 0)
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

void    l_uosgrnu_v_vvod(GtkWidget *widget,class l_uosgrnu_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uosgrnu_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->rk.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NAIM:
    data->rk.naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_PROC:
    data->rk.kof.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int l_uosgrnu_zap(class l_uosgrnu_v_data *data)
{
char strsql[1024];
//SQL_str row;
time_t   vrem;
time(&vrem);


if(data->rk.kod.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введён код !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }

if(data->rk.naim.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введено наименование !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }


if(iceb_u_SRAV(data->kod_podr.ravno(),data->rk.kod.ravno(),0) != 0)
  if(l_uosgrnu_pk(data->rk.kod.ravno(),data->window) != 0)
     return(1);

if(isdigit(data->rk.kod.ravno()[0]) == 0)
 {
  iceb_menu_soob(gettext("Код должен быть цифровым"),data->window);
  return(1);
 }

 
short metkakor=0;

if(data->kod_podr.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Uosgrup \
values ('%s','%s',%.10g,%d,%ld,%d,%d)",
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   data->rk.kof.ravno_atof(),
   iceb_getuid(data->window),vrem,
   data->rk.metka_ta,
   data->rk.metka_ar);
 }
else
 {
  
  if(iceb_u_SRAV(data->kod_podr.ravno(),data->rk.kod.ravno(),0) != 0)
   {

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }

  memset(strsql,'\0',sizeof(strsql));

  sprintf(strsql,"update Uosgrup \
set \
kod='%s',\
naik='%s',\
kof=%.10g,\
ktoz=%d,\
vrem=%ld,\
ta=%d, \
ar=%d \
where kod='%s'",
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   data->rk.kof.ravno_atof(),
   iceb_getuid(data->window),vrem,
   data->rk.metka_ta,
   data->rk.metka_ar,
   data->kod_podr.ravno());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


if(metkakor == 1)
 l_uosgrnu_kkvt(data->kod_podr.ravno(),data->rk.kod.ravno(),data->window);

return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_uosgrnu_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Uosgrup where kod='%s'",kod);
//printf("l_uosgrnu_pk-%s\n",strsql);

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

void l_uosgrnu_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Uosdok set podr=%s where podr=%s",nkod,skod);
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
sprintf(strsql,"update Uosdok1 set podr=%s where podr=%s",nkod,skod);
iceb_sql_zapis(strsql,0,0,wpredok);

sprintf(strsql,"update Uosamor set podr=%s where podr=%s",nkod,skod);
iceb_sql_zapis(strsql,0,0,wpredok);

sprintf(strsql,"update Uosamor1 set podr=%s where podr=%s",nkod,skod);
iceb_sql_zapis(strsql,0,0,wpredok);

}




