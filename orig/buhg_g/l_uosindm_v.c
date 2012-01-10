/*$Id: l_uosindm_v.c,v 1.10 2011-02-21 07:35:54 sasa Exp $*/
/*03.03.2009	03.03.2009	Белых А.И.	l_uosindm_v.c
Ввод и корректировка 
*/
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"

class l_uosindm_rek
 {
  public:
   class iceb_u_str innom;
   class iceb_u_str kod;
   class iceb_u_str naim;
   class iceb_u_str ei;
   class iceb_u_str ves;   
   l_uosindm_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
    ei.new_plus("");
    ves.new_plus("");
   }
 
 };

enum
 {
  E_KOD,
  E_VES,
  E_EI,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

class l_uosindm_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_uosindm_rek rk;
  class iceb_u_str kod_gr;  
  int innom;
    
  l_uosindm_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    innom=0;
   }

  void read_rek()
   {
    rk.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD]))));
    rk.ves.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VES]))));
    rk.ei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_EI]))));
   }
 };


gboolean   l_uosindm_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosindm_v_data *data);
void  l_uosindm_v_knopka(GtkWidget *widget,class l_uosindm_v_data *data);
void    l_uosindm_v_vvod(GtkWidget *widget,class l_uosindm_v_data *data);
int l_uosindm_pk(int,const char *god,GtkWidget*);
int l_uosindm_zap(class l_uosindm_v_data *data);
void l_uosindm_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);
void  l_uosindm_v_e_knopka(GtkWidget *widget,class l_uosindm_v_data *data);


extern SQL_baza  bd;
extern char      *name_system;

int l_uosindm_v(int innom,class iceb_u_str *kod_gr,GtkWidget *wpredok)
{

class l_uosindm_v_data data;
char strsql[512];
iceb_u_str kikz;
data.kod_gr.new_plus(kod_gr->ravno());
data.innom=innom;
data.rk.clear_data();

if(data.kod_gr.getdlinna() >  1)
 {
  data.rk.kod.new_plus(data.kod_gr.ravno());
  sprintf(strsql,"select * from Uosindm where innom=%d and kod=%s",data.innom,data.kod_gr.ravno());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.rk.kod.new_plus(row[1]);
  data.rk.ves.new_plus(row[3]);
  data.rk.ei.new_plus(row[2]);
  kikz.plus(iceb_kikz(row[4],row[5],wpredok));
    
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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_uosindm_v_key_press),&data);

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

GtkTooltips *tooltips_enter;


//label=gtk_label_new(gettext("Код"));
//gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
sprintf(strsql,"%s",gettext("Код"));
data.knopka_enter[E_KOD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.knopka_enter[E_KOD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD]),"clicked",GTK_SIGNAL_FUNC(l_uosindm_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD]),(gpointer)E_KOD);
tooltips_enter=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter,data.knopka_enter[E_KOD],gettext("Выбор материалла"),NULL);

data.entry[E_KOD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(l_uosindm_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

label=gtk_label_new(gettext("Вес"));
data.entry[E_VES] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_VES]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_VES]), data.entry[E_VES], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VES]), "activate",GTK_SIGNAL_FUNC(l_uosindm_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VES]),data.rk.ves.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VES]),(gpointer)E_VES);



sprintf(strsql,"%s",gettext("Единица измерения"));
data.knopka_enter[E_EI]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.knopka_enter[E_EI], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_EI]),"clicked",GTK_SIGNAL_FUNC(l_uosindm_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_EI]),(gpointer)E_EI);
tooltips_enter=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter,data.knopka_enter[E_EI],gettext("Выбор единицы измерения"),NULL);

data.entry[E_EI] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_EI]), "activate",GTK_SIGNAL_FUNC(l_uosindm_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.rk.ei.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_EI]),(gpointer)E_EI);




GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],data.knopka[PFK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK2]),"clicked",GTK_SIGNAL_FUNC(l_uosindm_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK2]),(gpointer)PFK2);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],data.knopka[PFK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK10]),"clicked",GTK_SIGNAL_FUNC(l_uosindm_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK10]),(gpointer)PFK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 kod_gr->new_plus(data.rk.kod.ravno());
return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_uosindm_v_e_knopka(GtkWidget *widget,class l_uosindm_v_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uosindm_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  case E_KOD:

    if(l_uosdm(1,&kod,&naim,data->window) == 0)
     data->rk.kod.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD]),data->rk.kod.ravno_toutf());

    return;  

  case E_EI:

    if(iceb_l_ei(1,&kod,&naim,data->window) == 0)
     data->rk.ei.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),data->rk.ei.ravno_toutf());

    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uosindm_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosindm_v_data *data)
{
//char  bros[512];

//printf("l_uosindm_v_key_press\n");
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
void  l_uosindm_v_knopka(GtkWidget *widget,class l_uosindm_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uosindm_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case PFK2:
    data->read_rek();
    if(l_uosindm_zap(data) == 0)
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

void    l_uosindm_v_vvod(GtkWidget *widget,class l_uosindm_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uosindm_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->rk.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(data->rk.ei.getdlinna() <= 1)
     {
      char strsql[512];
      SQL_str row;
      class SQLCURSOR cur;
      sprintf(strsql,"select ei from Uosdm where kod=%d",data->rk.kod.ravno_atoi());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
            gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),iceb_u_toutf(row[0]));
       
     }
    break;
  case E_VES:
    data->rk.ves.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int l_uosindm_zap(class l_uosindm_v_data *data)
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
/*Проверяем код*/
sprintf(strsql,"select kod from Uosdm where kod=%d",data->rk.kod.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не найден код материалла"),data->rk.kod.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
if(data->rk.ves.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введено вес!"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }
if(data->rk.ei.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Edizmer where kod='%s'",data->rk.ei.ravno());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s %s!",gettext("Не найдено единицу измерения"),data->rk.ei.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }

if(iceb_u_SRAV(data->kod_gr.ravno(),data->rk.kod.ravno(),0) != 0)
  if(l_uosindm_pk(data->innom,data->rk.kod.ravno(),data->window) != 0)
     return(1);
if(isdigit(data->rk.kod.ravno()[0]) == 0)
 {
  iceb_menu_soob(gettext("Код должен быть цифровым !"),data->window);
  return(1);
 }
short metkakor=0;

if(data->kod_gr.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Uosindm \
values (%d,%d,'%s',%10.10g,%d,%ld)",
   data->innom,
   data->rk.kod.ravno_atoi(),
   data->rk.ei.ravno_filtr(),
   data->rk.ves.ravno_atof(),
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
  sprintf(strsql,"update Uosindm \
set \
kod=%d,\
ei='%s',\
ves=%10.10g,\
ktoz=%d,\
vrem=%ld \
where innom=%d and kod=%d",
   data->rk.kod.ravno_atoi(),
   data->rk.ei.ravno_filtr(),
   data->rk.ves.ravno_atof(),
   iceb_getuid(data->window),vrem,
   data->innom,
   data->kod_gr.ravno_atoi());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

if(metkakor == 1)
 l_uosindm_kkvt(data->kod_gr.ravno(),data->rk.kod.ravno(),data->window);

return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_uosindm_pk(int innom,const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Uosindm where innom=%d kod=%s",innom,kod);
//printf("l_uosindm_pk-%s\n",strsql);

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

void l_uosindm_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Uosindm set kod=%s where kod=%s",nkod,skod);

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
