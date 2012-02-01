/*$Id: admin_dostup.c,v 1.14 2011-09-05 08:18:27 sasa Exp $*/
/*10.08.2011	05.02.2009	Белых А.И.	admin_dostup.c
Организация доступа к базе данных
*/
#define DVERSIQ "10.08.2011"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include        <pwd.h>
#include        <unistd.h>
#include        <sys/stat.h>
#include <string.h>
#include        <locale.h>
//zs
//#include  "buhg_g.h"
#include  "../headers/buhg_g.h"
//ze


enum
 {
  E_IMABAZ,
  E_LOGIN,
  E_PAROL,
  E_HOSTPOL,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class admin_dostup_r
 {
  public:
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  class iceb_u_str parolpol;
  class iceb_u_str hostpol;
  class iceb_u_str login;
  class iceb_u_str imabaz;
  int       dostup; //0-запись чтение 1-только чтение
  int fk4;

  admin_dostup_r()
   {
    clear_data();    
   }  

  void read_rek()
   {
    parolpol.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PAROL]))));
    hostpol.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_HOSTPOL]))));
    login.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_LOGIN]))));
    imabaz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_IMABAZ]))));
   }

  void clear_data()
   {
    parolpol.new_plus("");
    hostpol.new_plus("");
    login.new_plus("");
    imabaz.new_plus("");
    dostup=0;
    fk4=0;
   }
   
 };


gboolean   dortupw_key_press(GtkWidget *,GdkEventKey *,class admin_dostup_r*);
void       dortupw_knop(GtkWidget *,class admin_dostup_r *);
void       dostupb(class admin_dostup_r *);
void dostupw_vm(GtkWidget *,int *);
void    dostup_v_vvod(GtkWidget *widget,class admin_dostup_r *data);
void admin_newuser(const char *imabazr,const char *user,GtkWidget *wpredok);
void admin_dostup_e_knopka(GtkWidget *widget,class admin_dostup_r *data);

extern SQL_baza	bd;

int admin_dostup()
{
char bros[512];
class admin_dostup_r data;

data.hostpol.new_plus("localhost");


data.window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

sprintf(bros,"%s %s",NAME_SYSTEM,gettext("Доступ к базе"));
gtk_window_set_title(GTK_WINDOW(data.window),bros);

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),10);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(dortupw_key_press),&data);

GtkWidget *vbox=gtk_vbox_new(FALSE,2);

sprintf(bros,"%s\n%s %s %s %s %s",
gettext("Установка доступа к базе данных"),
gettext("Версия"),
VERSION,
gettext("от"),
DVERSIQ,
gettext("г."));

GtkWidget *label=gtk_label_new(iceb_u_toutf(bros));

GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *hbox_tipd=gtk_hbox_new(FALSE,2);
GtkWidget *hboxkn=gtk_hbox_new(FALSE,2);

gtk_container_add(GTK_CONTAINER(data.window),vbox);

gtk_box_pack_start((GtkBox*)vbox,label,FALSE,FALSE,0);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);

gtk_box_pack_start((GtkBox*)vbox,hbox_tipd,FALSE,FALSE,0);
gtk_box_pack_start((GtkBox*)vbox,hboxkn,FALSE,FALSE,0);

GtkTooltips *tooltips_enter[KOLENTER];


data.knopka_enter[E_IMABAZ]=gtk_button_new_with_label(gettext("Имя базы данных"));
gtk_box_pack_start (GTK_BOX (hbox[E_IMABAZ]), data.knopka_enter[E_IMABAZ], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_IMABAZ]),"clicked",GTK_SIGNAL_FUNC(admin_dostup_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_IMABAZ]),(gpointer)E_IMABAZ);
tooltips_enter[E_IMABAZ]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_IMABAZ],data.knopka_enter[E_IMABAZ],gettext("Выбор базы"),NULL);

data.entry[E_IMABAZ] = gtk_entry_new_with_max_length (32);
gtk_box_pack_start (GTK_BOX (hbox[E_IMABAZ]), data.entry[E_IMABAZ], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_IMABAZ]), "activate",GTK_SIGNAL_FUNC(dostup_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IMABAZ]),data.imabaz.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_IMABAZ]),(gpointer)E_IMABAZ);


label=gtk_label_new(gettext("Логин оператора"));
gtk_box_pack_start (GTK_BOX (hbox[E_LOGIN]), label, FALSE, FALSE, 0);

data.entry[E_LOGIN] = gtk_entry_new_with_max_length (32);
gtk_box_pack_start (GTK_BOX (hbox[E_LOGIN]), data.entry[E_LOGIN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_LOGIN]), "activate",GTK_SIGNAL_FUNC(dostup_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_LOGIN]),data.login.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_LOGIN]),(gpointer)E_LOGIN);


label=gtk_label_new(gettext("Пароль оператора для работы с базой"));
gtk_box_pack_start (GTK_BOX (hbox[E_PAROL]), label, FALSE, FALSE, 0);

data.entry[E_PAROL] = gtk_entry_new_with_max_length (32);
gtk_box_pack_start (GTK_BOX (hbox[E_PAROL]), data.entry[E_PAROL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PAROL]), "activate",GTK_SIGNAL_FUNC(dostup_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PAROL]),data.parolpol.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PAROL]),(gpointer)E_PAROL);


label=gtk_label_new(gettext("Хост c которого оператору будет разрешен доступ к базе"));
gtk_box_pack_start (GTK_BOX (hbox[E_HOSTPOL]), label, FALSE, FALSE, 0);

data.entry[E_HOSTPOL] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_HOSTPOL]), data.entry[E_HOSTPOL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_HOSTPOL]), "activate",GTK_SIGNAL_FUNC(dostup_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HOSTPOL]),data.hostpol.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_HOSTPOL]),(gpointer)E_HOSTPOL);


label=gtk_label_new(gettext("Тип доступа"));
gtk_box_pack_start((GtkBox*)hbox_tipd,label,FALSE,FALSE,0);
GtkWidget *opt = gtk_option_menu_new();
GtkWidget *menu = gtk_menu_new();


GtkWidget *item =gtk_menu_item_new_with_label (gettext("Запись и чтение"));
gtk_widget_show (item);

gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (gettext("Только чтение"));
gtk_menu_append (GTK_MENU (menu), item);

gtk_option_menu_set_menu (GTK_OPTION_MENU (opt), menu);

gtk_option_menu_set_history(GTK_OPTION_MENU(opt),0);

gtk_box_pack_start (GTK_BOX (hbox_tipd), opt, TRUE, TRUE, 0);

gtk_signal_connect (GTK_OBJECT (opt), "changed",GTK_SIGNAL_FUNC(dostupw_vm),&data.dostup);


GtkTooltips *tooltips[KOL_FK];

sprintf(bros,"F2 %s",gettext("Установить доступ"));
data.knopka[FK2]=gtk_button_new_with_label (bros);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start (GTK_BOX (hboxkn), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(dortupw_knop),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Установить разрещение на работу с базой"),NULL);

sprintf(bros,"F4 %s",gettext("Проверка"));
data.knopka[FK4]=gtk_button_new_with_label (bros);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start (GTK_BOX (hboxkn), data.knopka[FK4], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(dortupw_knop),&data);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Проверить есть ли уже доступ для этого логина"),NULL);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label (bros);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start (GTK_BOX (hboxkn), data.knopka[FK10], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(dortupw_knop),&data);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);

gtk_widget_show_all(data.window);
gtk_main();

return(0);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void admin_dostup_e_knopka(GtkWidget *widget,class admin_dostup_r *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {

  case E_IMABAZ:

    if(admin_bases(1,&data->imabaz,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_IMABAZ]),data->imabaz.ravno());
      
    return;  

 }
}
/*************************/
/*Обработчик кнопок      */
/**************************/

void    dortupw_knop(GtkWidget *widget,class admin_dostup_r *data)
{

//gchar *knop=(gchar*)gtk_widget_get_name(widget);
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
   data->fk4=0;
   dostupb(data);
   return;

  case FK4:
   data->fk4=1;
   dostupb(data);

   return;
   
  case FK10:
    gtk_widget_destroy(data->window);
    return;

 }



}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   dortupw_key_press(GtkWidget *widget,GdkEventKey *event,class admin_dostup_r *data)
{
//char  bros[512];

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
    return(TRUE);

 }
return(TRUE);
}
/*****************************/
/*Установка достува*/
/**************************/

void   dostupb(class admin_dostup_r *data)
{
char strsql[512];
data->read_rek();

if(data->imabaz.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели имя базы!"),data->window);
  return;     
 }
if(data->login.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели логин оператора!"),data->window);
  return;     
 }

if(admin_provbaz(data->imabaz.ravno(),data->window) != 0)
 {
  iceb_menu_soob(gettext("Нет базы данных с таким именем!"),data->window);
  return;
 }

if(data->fk4 != 0)
 {
  
  sprintf(strsql,"select User from db where Db='%s' and User='%s'",data->imabaz.ravno(),data->login.ravno());
  if(iceb_sql_readkey(strsql,data->window) > 0) /*Запись уже есть*/
    iceb_menu_soob(gettext("Такой логин для этой базы уже введён!"),data->window);
  else
    iceb_menu_soob(gettext("Такого логина для этой базы ещё нет"),data->window);
  return;  
 }

if(data->parolpol.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели пароль оператора!"),data->window);
  return;     
 }

SQL_str row;
class SQLCURSOR cur;
int metka_ver=0;
sql_readkey(&bd,"select VERSION()",&row,&cur);
if(iceb_u_SRAV(row[0],"3.",1) != 0)
  metka_ver=1;  

sprintf(strsql,"select User from db where Db='%s' and User='%s' and \
Host='%s'",
data->imabaz.ravno(),data->login.ravno(),data->hostpol.ravno());
if(sql_readkey(&bd,strsql) > 0)
 {
//  printw("Переустановка прав доступа.\n");
  
  if(data->parolpol.ravno()[0] != '\0')
   {
    sprintf(strsql,"SET PASSWORD FOR %s@%s = PASSWORD('%s')",
    data->login.ravno(),data->hostpol.ravno(),data->parolpol.ravno());
    if(mysql_query(&bd,strsql) < 0)
      iceb_msql_error(&bd,"Ошибка установки пароля !",strsql,data->window);
   }

  if(data->hostpol.ravno()[0] != '\0')
   {
    if(data->dostup == 0) 
     {
      if(metka_ver == 0)
        sprintf(strsql,"GRANT INSERT,SELECT,DELETE,UPDATE,CREATE,DROP ON \
%s.* TO '%s'@'%s'",data->imabaz.ravno(),data->login.ravno(),data->hostpol.ravno());
      if(metka_ver == 1)
        sprintf(strsql,"GRANT INSERT,SELECT,DELETE,UPDATE,CREATE,DROP,LOCK TABLES,CREATE TEMPORARY TABLES ON \
%s.* TO '%s'@'%s'",data->imabaz.ravno(),data->login.ravno(),data->hostpol.ravno());
     }
    if(data->dostup == 1)
      sprintf(strsql,"REVOKE INSERT,DELETE,UPDATE,CREATE,DROP ON \
%s.* FROM %s@%s",data->imabaz.ravno(),data->login.ravno(),data->hostpol.ravno());
   }
  else
   {
    if(data->dostup == 0)
     {
      if(metka_ver == 0)
        sprintf(strsql,"GRANT INSERT,SELECT,DELETE,UPDATE,CREATE,DROP ON \
%s.* TO '%s'",data->imabaz.ravno(),data->login.ravno());
      if(metka_ver == 1)
        sprintf(strsql,"GRANT INSERT,SELECT,DELETE,UPDATE,CREATE,DROP,LOCK TABLES,CREATE TEMPORARY TABLES ON \
%s.* TO '%s'",data->imabaz.ravno(),data->login.ravno());
     }
    if(data->dostup == 1)
      sprintf(strsql,"REVOKE INSERT,DELETE,UPDATE,CREATE,DROP ON \
%s.* FROM '%s'",data->imabaz.ravno(),data->login.ravno());
   }
 }
else
 {
  if(data->hostpol.ravno()[0] != '\0')
   {
    if(data->dostup == 0)
     {
      
      if(metka_ver == 0)
        sprintf(strsql,"GRANT INSERT,SELECT,DELETE,UPDATE,CREATE,DROP ON \
%s.* TO '%s'@'%s' IDENTIFIED BY '%s'",data->imabaz.ravno(),data->login.ravno(),data->hostpol.ravno(),data->parolpol.ravno());
      if(metka_ver == 1)
        sprintf(strsql,"GRANT INSERT,SELECT,DELETE,UPDATE,CREATE,DROP,LOCK TABLES,CREATE TEMPORARY TABLES ON \
%s.* TO '%s'@'%s' IDENTIFIED BY '%s'",data->imabaz.ravno(),data->login.ravno(),data->hostpol.ravno(),data->parolpol.ravno());
     }
    if(data->dostup == 1)
      sprintf(strsql,"GRANT SELECT ON \
%s.* TO '%s'@'%s' IDENTIFIED BY '%s'",data->imabaz.ravno(),data->login.ravno(),data->hostpol.ravno(),data->parolpol.ravno());
   }
  else
   {
    if(data->dostup == 0)
     {
      if(metka_ver == 0)
       sprintf(strsql,"GRANT INSERT,SELECT,DELETE,UPDATE,CREATE,DROP ON \
%s.* TO '%s' IDENTIFIED BY '%s'",data->imabaz.ravno(),data->login.ravno(),data->parolpol.ravno());

      if(metka_ver == 1)
       sprintf(strsql,"GRANT INSERT,SELECT,DELETE,UPDATE,CREATE,DROP,LOCK TABLES,CREATE TEMPORARY TABLES ON \
%s.* TO '%s' IDENTIFIED BY '%s'",data->imabaz.ravno(),data->login.ravno(),data->parolpol.ravno());
     }
    if(data->dostup == 1)
      sprintf(strsql,"GRANT SELECT ON \
%s.* TO '%s' IDENTIFIED BY '%s'",data->imabaz.ravno(),data->login.ravno(),data->parolpol.ravno());
   }
 }

if(iceb_sql_zapis(strsql,1,1,data->window) == 0)
 {
  iceb_menu_soob("Привелегии пользователю установлены",data->window);
  admin_newuser(data->imabaz.ravno(),data->login.ravno(),data->window);
 }

iceb_sql_zapis("FLUSH PRIVILEGES",1,1,data->window);


}

void dostupw_vm(GtkWidget *widget,int *data)
{
*data=gtk_option_menu_get_history(GTK_OPTION_MENU(widget));
}

/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    dostup_v_vvod(GtkWidget *widget,class admin_dostup_r *data)
{

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("glkni_v_vvod enter=%d\n",enter);

/***************
switch (enter)
 {
  case E_IMABAZ:
    data->imabaz.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case LOGIN:
    strncpy(data->login,iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))),
    sizeof(data->login)-1);
    break;

  case PAROL:
    strncpy(data->parolpol,iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))),
    sizeof(data->parolpol)-1);
    break;

  case HOSTPOL:
    strncpy(data->hostpol,iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))),
    sizeof(data->hostpol)-1);
    break;

 }
******************/
enter+=1;
if(enter >= KOLENTER)
  enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
