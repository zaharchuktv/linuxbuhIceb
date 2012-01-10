/*$Id: l_kaskcnsl_v.c,v 1.14 2011-02-21 07:35:52 sasa Exp $*/
/*24.04.2008	21.06.2006	Белых А.И.	l_kaskcnsl_v.c
Ввод и корректировка групп подразделений
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"

enum
 {
  E_GOD,
  E_KAS,
  E_KOD,
  E_SALDO,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

class l_kaskcnsl_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str god;  
  class iceb_u_str kas;  
  class iceb_u_str kod;  
  class iceb_u_str saldo;  


  //Ключевые реквизиты корректируемой строки
  class iceb_u_str kod_cn_z;  
  class iceb_u_str kas_z;  
  class iceb_u_str god_z;  
  
  l_kaskcnsl_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_rek();
   }

  void read_rek()
   {
    kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD]))));
    god.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_GOD]))));
    kas.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KAS]))));
    saldo.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SALDO]))));
   }
  void clear_rek()
   {
    god.new_plus("");
    kod.new_plus("");
    kas.new_plus("");
    saldo.new_plus("");
   } 
 };


gboolean   l_kaskcnsl_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_kaskcnsl_v_data *data);
void  l_kaskcnsl_v_knopka(GtkWidget *widget,class l_kaskcnsl_v_data *data);
void    l_kaskcnsl_v_vvod(GtkWidget *widget,class l_kaskcnsl_v_data *data);
int l_kaskcnsl_pk(char *god,GtkWidget*);
int l_kaskcnsl_zap(class l_kaskcnsl_v_data *data);

void  kaskcnsl_v_e_knopka(GtkWidget *widget,class l_kaskcnsl_v_data *data);

extern SQL_baza  bd;
extern char      *name_system;

int l_kaskcnsl_v(class iceb_u_str *god,
class iceb_u_str *kas,
class iceb_u_str *kod_cn,
GtkWidget *wpredok)
{
class l_kaskcnsl_v_data data;
char strsql[512];
iceb_u_str kikz;

data.kod_cn_z.new_plus(kod_cn->ravno());
data.god_z.new_plus(god->ravno());
data.kas_z.new_plus(kas->ravno());


if(data.kod_cn_z.getdlinna() >  1)
 {
  data.kod.new_plus(kod_cn->ravno());
  data.god.new_plus(god->ravno());
  data.kas.new_plus(kas->ravno());

  sprintf(strsql,"select * from Kascnsl where god=%s and kod=%s and ks=%s",
  data.god_z.ravno(),
  data.kod_cn_z.ravno(),
  data.kas_z.ravno());
  
  SQL_str row;
  SQLCURSOR cur;
  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  
  data.saldo.new_plus(row[3]);
  
  kikz.plus(iceb_kikz(row[4],row[5],wpredok));
  
 }
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.kod_cn_z.getdlinna() <= 1)
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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_kaskcnsl_v_key_press),&data);

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


GtkTooltips *tooltips_enter[KOLENTER];

label=gtk_label_new(gettext("Год"));
data.entry[E_GOD] = gtk_entry_new_with_max_length (4);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), data.entry[E_GOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GOD]), "activate",GTK_SIGNAL_FUNC(l_kaskcnsl_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD]),data.god.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GOD]),(gpointer)E_GOD);

//label=gtk_label_new(gettext("Код кассы"));
//gtk_box_pack_start (GTK_BOX (hbox[E_KAS]), label, FALSE, FALSE, 0);

sprintf(strsql,"%s",gettext("Код кассы"));
data.knopka_enter[E_KAS]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KAS]), data.knopka_enter[E_KAS], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KAS]),"clicked",GTK_SIGNAL_FUNC(kaskcnsl_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KAS]),(gpointer)E_KAS);
tooltips_enter[E_KAS]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KAS],data.knopka_enter[E_KAS],gettext("Выбор кассы"),NULL);

data.entry[E_KAS] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KAS]), data.entry[E_KAS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KAS]), "activate",GTK_SIGNAL_FUNC(l_kaskcnsl_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KAS]),data.kas.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KAS]),(gpointer)E_KAS);


//label=gtk_label_new(gettext("Код"));
//gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
sprintf(strsql,"%s",gettext("Код целевого назначения"));
data.knopka_enter[E_KOD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.knopka_enter[E_KOD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD]),"clicked",GTK_SIGNAL_FUNC(kaskcnsl_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD]),(gpointer)E_KOD);
tooltips_enter[E_KOD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD],data.knopka_enter[E_KOD],gettext("Выбор кода целевого назначения"),NULL);

data.entry[E_KOD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(l_kaskcnsl_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.kod.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

label=gtk_label_new(gettext("Сальдо"));
data.entry[E_SALDO] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SALDO]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SALDO]), data.entry[E_SALDO], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SALDO]), "activate",GTK_SIGNAL_FUNC(l_kaskcnsl_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SALDO]),data.saldo.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SALDO]),(gpointer)E_SALDO);

GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],data.knopka[PFK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK2]),"clicked",GTK_SIGNAL_FUNC(l_kaskcnsl_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK2]),(gpointer)PFK2);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],data.knopka[PFK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK10]),"clicked",GTK_SIGNAL_FUNC(l_kaskcnsl_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK10]),(gpointer)PFK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 {
  kod_cn->new_plus(data.kod.ravno());
  god->new_plus(data.god.ravno());
  kas->new_plus(data.kas.ravno());
 }
return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  kaskcnsl_v_e_knopka(GtkWidget *widget,class l_kaskcnsl_v_data *data)
{
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {

  case E_KAS:
    l_spis_kas(1,&data->kas,&naim,data->window);
   
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KAS]),data->kas.ravno_toutf());
    return;

  case E_KOD:
    l_kaskcn(1,&data->kod,&naim,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD]),data->kod.ravno_toutf());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_kaskcnsl_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_kaskcnsl_v_data *data)
{
//char  bros[512];

//printf("l_kaskcnsl_v_key_press\n");
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
void  l_kaskcnsl_v_knopka(GtkWidget *widget,class l_kaskcnsl_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_kaskcnsl_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case PFK2:
    data->read_rek();
    if(l_kaskcnsl_zap(data) == 0)
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

void    l_kaskcnsl_v_vvod(GtkWidget *widget,class l_kaskcnsl_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_kaskcnsl_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_GOD:
    data->god.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KAS:
    data->kas.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SALDO:
    data->saldo.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int l_kaskcnsl_zap(class l_kaskcnsl_v_data *data)
{
char strsql[1024];


if(data->kod.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введён код !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }
//проверяем код целевого назначения
sprintf(strsql,"select kod from Kascn where kod=%d",data->kod.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код целевого назначения"),data->kod.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
//Проверяем код кассы
sprintf(strsql,"select kod from Kas where kod=%d",data->kas.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код кассы"),data->kas.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
if(data->god.ravno_atoi() <= 0)
 {
  iceb_menu_soob(gettext("Не введено год !"),data->window);
  return(1);
 }

if(iceb_pbpds(1,data->god.ravno_atoi(),data->window) != 0)
 return(1);

if(data->god_z.ravno_atoi() != 0)
 if(data->god_z.ravno_atoi() != data->god.ravno_atoi())
  if(iceb_pbpds(1,data->god_z.ravno_atoi(),data->window) != 0)
   return(1);
   
if(data->saldo.ravno_atof() == 0.)
 {
  iceb_menu_soob(gettext("Не введено сальдо !"),data->window);
  return(1);
 }

time_t   vrem;
time(&vrem);

if(data->kod_cn_z.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Kascnsl \
values (%d,%d,%d,%.2f,%d,%ld)",
   data->god.ravno_atoi(),
   data->kod.ravno_atoi(),
   data->kas.ravno_atoi(),
   data->saldo.ravno_atof(),
   iceb_getuid(data->window),vrem);
 }
else
 {
  
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Kascnsl \
set \
god=%d,\
kod=%d,\
ks=%d,\
saldo=%.2f,\
ktoz=%d,\
vrem=%ld \
where god=%s and kod=%s and ks=%s",
   data->god.ravno_atoi(),
   data->kod.ravno_atoi(),
   data->kas.ravno_atoi(),
   data->saldo.ravno_atof(),
   iceb_getuid(data->window),vrem,
   data->god_z.ravno(),
   data->kod_cn_z.ravno(),
   data->kas_z.ravno());
   
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
int l_kaskcnsl_pk(char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Zargrupp where kod='%s'",kod);
//printf("l_kaskcnsl_pk-%s\n",strsql);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Такая запись уже есть !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

return(0);

}
