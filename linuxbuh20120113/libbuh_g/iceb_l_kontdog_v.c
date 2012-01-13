/*$Id: iceb_l_kontdog_v.c,v 1.1 2011-02-21 07:36:07 sasa Exp $*/
/*20.02.2011	20.02.2011	Белых А.И.	iceb_l_kontdog_v.c
Ввод и корректировка договоров
*/
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "iceb_libbuh.h"

enum
 {
  E_DAT_DOG,
  E_NOM_DOG,
  E_VID_DOG,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class iceb_l_kontdog_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str dat_dog;
  class iceb_u_str nom_dog;
  class iceb_u_str vid_dog;  

  int un_nom_zap;
  class iceb_u_str kodkon;
  iceb_l_kontdog_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    dat_dog.plus("");
    nom_dog.plus("");
    vid_dog.plus("");
   }

  void read_rek()
   {
    dat_dog.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DAT_DOG]))));
    nom_dog.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOM_DOG]))));
    vid_dog.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VID_DOG]))));
    
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    
   }
 };


gboolean   iceb_l_kontdog_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_kontdog_v_data *data);
void  iceb_l_kontdog_v_knopka(GtkWidget *widget,class iceb_l_kontdog_v_data *data);
void    iceb_l_kontdog_v_vvod(GtkWidget *widget,class iceb_l_kontdog_v_data *data);
int iceb_l_kontdog_pk(const char *god,GtkWidget*);
int iceb_l_kontdog_zap(class iceb_l_kontdog_v_data *data);
void iceb_l_kontdog_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);
void  iceb_l_kontdog_v_e_knopka(GtkWidget *widget,class iceb_l_kontdog_v_data *data);


extern SQL_baza  bd;
extern char      *name_system;

int iceb_l_kontdog_v(int un_nom_zap,const char *kodkon,GtkWidget *wpredok)
{
SQL_str row;
SQLCURSOR cur;
class iceb_u_str naim_kontr("");  

class iceb_l_kontdog_v_data data;
char strsql[1024];
iceb_u_str kikz;

data.un_nom_zap=un_nom_zap;
data.kodkon.new_plus(kodkon);

sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kodkon);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_kontr.new_plus(row[0]);
 
if(data.un_nom_zap >=  0)
 {
  sprintf(strsql,"select * from Kontragent2 where nz=%d",data.un_nom_zap);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.dat_dog.new_plus(iceb_u_datzap(row[2]));
  data.vid_dog.new_plus(row[4]);
  data.nom_dog.new_plus(row[3]);
  
  kikz.plus(iceb_kikz(row[5],row[6],wpredok));
  
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.un_nom_zap < 0)
 {
  sprintf(strsql,"%s %s\n%s:%s %s",
  name_system,gettext("Ввод новой записи"),
  gettext("Контрагент"),kodkon,naim_kontr.ravno());
  label=gtk_label_new(strsql);
 }
else
 {
  sprintf(strsql,"%s %s\n%s:%s %s",name_system,gettext("Корректировка записи"),
  gettext("Контрагент"),kodkon,naim_kontr.ravno());
  class iceb_u_str repl;
  repl.plus(strsql);
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno_toutf());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_l_kontdog_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
 }
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 0);
//gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);

GtkTooltips *tooltips_enter[KOLENTER];

//label=gtk_label_new(gettext("Дата договора"));
//gtk_box_pack_start (GTK_BOX (hbox[E_DAT_DOG]), label, FALSE, FALSE, 0);
sprintf(strsql,"%s (%s)",gettext("Дата договора"),gettext("д.м.г"));
data.knopka_enter[E_DAT_DOG]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DAT_DOG]), data.knopka_enter[E_DAT_DOG], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DAT_DOG]),"clicked",GTK_SIGNAL_FUNC(iceb_l_kontdog_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DAT_DOG]),(gpointer)E_DAT_DOG);
tooltips_enter[E_DAT_DOG]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DAT_DOG],data.knopka_enter[E_DAT_DOG],gettext("Выбор даты"),NULL);

data.entry[E_DAT_DOG] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DAT_DOG]), data.entry[E_DAT_DOG], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DAT_DOG]), "activate",GTK_SIGNAL_FUNC(iceb_l_kontdog_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DAT_DOG]),data.dat_dog.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DAT_DOG]),(gpointer)E_DAT_DOG);

label=gtk_label_new(gettext("Номер договора"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOM_DOG]), label, FALSE, FALSE, 0);

data.entry[E_NOM_DOG] = gtk_entry_new_with_max_length (60);
gtk_box_pack_start (GTK_BOX (hbox[E_NOM_DOG]), data.entry[E_NOM_DOG], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOM_DOG]), "activate",GTK_SIGNAL_FUNC(iceb_l_kontdog_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOM_DOG]),data.nom_dog.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOM_DOG]),(gpointer)E_NOM_DOG);

label=gtk_label_new(gettext("Вид договора"));
gtk_box_pack_start (GTK_BOX (hbox[E_VID_DOG]), label, FALSE, FALSE, 0);

data.entry[E_VID_DOG] = gtk_entry_new_with_max_length (255);
gtk_box_pack_start (GTK_BOX (hbox[E_VID_DOG]), data.entry[E_VID_DOG], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VID_DOG]), "activate",GTK_SIGNAL_FUNC(iceb_l_kontdog_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VID_DOG]),data.vid_dog.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VID_DOG]),(gpointer)E_VID_DOG);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(iceb_l_kontdog_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введённой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(iceb_l_kontdog_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(iceb_l_kontdog_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  iceb_l_kontdog_v_e_knopka(GtkWidget *widget,class iceb_l_kontdog_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DAT_DOG:

    if(iceb_calendar(&data->dat_dog,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DAT_DOG]),data->dat_dog.ravno());
      
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_l_kontdog_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_kontdog_v_data *data)
{
//char  bros[512];

//printf("iceb_l_kontdog_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");

    return(TRUE);
  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");

    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");

    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
  //  printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_l_kontdog_v_knopka(GtkWidget *widget,class iceb_l_kontdog_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("iceb_l_kontdog_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(iceb_l_kontdog_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  

  case FK4:
    data->clear_rek();
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

void    iceb_l_kontdog_v_vvod(GtkWidget *widget,class iceb_l_kontdog_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("iceb_l_kontdog_v_vvod enter=%d\n",enter);

switch (enter)
 {
/**********
  case E_DAT_DOG:
    data->rk.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NOM_DOG:
    data->rk.naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
*************/
 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int iceb_l_kontdog_zap(class iceb_l_kontdog_v_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);

data->read_rek();

if(data->dat_dog.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата !"),data->window);
  return(1);
 }
if(data->dat_dog.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена дата!"),data->window);
  return(1);
 }

if(data->un_nom_zap >= 0) /*Корректировка*/
 {
  sprintf(strsql,"update Kontragent2 set \
datd='%s',\
nomd='%s',\
vidd='%s',\
ktoz=%d,\
vrem=%ld \
where nz=%d",
  data->dat_dog.ravno_sqldata(),
  data->nom_dog.ravno(),
  data->vid_dog.ravno_filtr(),
  iceb_getuid(data->window),
  time(NULL),
  data->un_nom_zap);
  
   
 }
else
 {
  sprintf(strsql,"insert into Kontragent2 (kodkon,datd,nomd,vidd,ktoz,vrem) values ('%s','%s','%s','%s',%d,%ld)",
  data->kodkon.ravno(),
  data->dat_dog.ravno_sqldata(),
  data->nom_dog.ravno(),
  data->vid_dog.ravno_filtr(),
  iceb_getuid(data->window),
  time(NULL));
  
 }

if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
 return(1);

return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int iceb_l_kontdog_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Kateg where kod=%s",kod);
//printf("iceb_l_kontdog_pk-%s\n",strsql);

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

void iceb_l_kontdog_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Kartb set kateg=%s where kateg=%s",nkod,skod);

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
sprintf(strsql,"update Zarn set kateg=%s where kateg=%s",nkod,skod);

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
