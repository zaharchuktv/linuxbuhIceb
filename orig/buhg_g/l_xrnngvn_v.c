/*$Id: l_xrnngvn_v.c,v 1.11 2011-02-21 07:35:54 sasa Exp $*/
/*14.04.2008	14.04.2008	Белых А.И.	l_xrnngvn_v.c
Ввод и корректировка групп выданных налоговых накладных
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
#include "l_xrnngvn.h"
enum
 {
  E_KOD,
  E_NAIM,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_xrnngvn_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_xrnngvn_rek rk;
  class iceb_u_str kod_gr;  
  
  l_xrnngvn_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD]))));
    rk.naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM]))));
   }
 };


gboolean   l_xrnngvn_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_xrnngvn_v_data *data);
void  l_xrnngvn_v_knopka(GtkWidget *widget,class l_xrnngvn_v_data *data);
void    l_xrnngvn_v_vvod(GtkWidget *widget,class l_xrnngvn_v_data *data);
int l_xrnngvn_pk(const char *god,GtkWidget*);
int l_xrnngvn_zap(class l_xrnngvn_v_data *data);
void l_xrnngvn_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);

extern SQL_baza  bd;
extern char      *name_system;

int l_xrnngvn_v(class iceb_u_str *kod_gr,GtkWidget *wpredok)
{

class l_xrnngvn_v_data data;
char strsql[512];
iceb_u_str kikz;
data.kod_gr.new_plus(kod_gr->ravno());

data.rk.clear_data();

if(data.kod_gr.getdlinna() >  1)
 {
  data.rk.kod.new_plus(data.kod_gr.ravno());
  sprintf(strsql,"select * from Reegrup2 where kod='%s'",data.kod_gr.ravno());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.rk.naim.new_plus(row[1]);
  kikz.plus(iceb_kikz(row[2],row[3],wpredok));
  
 }
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.kod_gr.getdlinna() <= 1)
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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_xrnngvn_v_key_press),&data);

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


label=gtk_label_new(gettext("Код"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE,1);

data.entry[E_KOD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(l_xrnngvn_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

label=gtk_label_new(gettext("Наименование"));
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE,1);

data.entry[E_NAIM] = gtk_entry_new_with_max_length (100);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(l_xrnngvn_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_xrnngvn_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_xrnngvn_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 kod_gr->new_plus(data.rk.kod.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_xrnngvn_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_xrnngvn_v_data *data)
{
//char  bros[512];

//printf("l_xrnngvn_v_key_press\n");
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
void  l_xrnngvn_v_knopka(GtkWidget *widget,class l_xrnngvn_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_xrnngvn_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_xrnngvn_zap(data) == 0)
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

void    l_xrnngvn_v_vvod(GtkWidget *widget,class l_xrnngvn_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_xrnngvn_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->rk.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NAIM:
    data->rk.naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int l_xrnngvn_zap(class l_xrnngvn_v_data *data)
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

short metkakor=0;

if(iceb_u_SRAV(data->kod_gr.ravno(),data->rk.kod.ravno(),0) != 0)
  if(l_xrnngvn_pk(data->rk.kod.ravno(),data->window) != 0)
     return(1);

if(data->kod_gr.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Reegrup2 \
values ('%s','%s',%d,%ld)",
   data->rk.kod.ravno_filtr(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem);
 }
else
 {
  
  if(iceb_u_SRAV(data->kod_gr.ravno(),data->rk.kod.ravno(),0) != 0)
   {

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Reegrup2 \
set \
kod='%s',\
naik='%s',\
ktoz=%d,\
vrem=%ld \
where kod='%s'",
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->kod_gr.ravno());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);

if(metkakor == 1)
 l_xrnngvn_kkvt(data->kod_gr.ravno(),data->rk.kod.ravno(),data->window);

return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_xrnngvn_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Reegrup2 where kod='%s'",kod);
//printf("l_xrnngvn_pk-%s\n",strsql);

if(iceb_sql_readkey(strsql,wpredok) > 0)
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

void l_xrnngvn_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Reenn set gr='%s' where gr='%s'",
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

}
