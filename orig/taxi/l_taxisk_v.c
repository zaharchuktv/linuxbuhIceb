/*$Id: l_taxisk_v.c,v 1.4 2011-02-21 07:36:20 sasa Exp $*/
/*01.03.2009	21.06.2006	Белых А.И.	l_taxisk_v.c
Ввод и корректировка
*/
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "taxi.h"
#include "l_taxisk.h"
enum
 {
  E_KODKL,
  E_FIO,
  E_ADRES,
  E_TELEFON,
  E_DENROG,
  E_KOMENT,
  E_POL,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_taxisk_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет
  GtkWidget *opt1;

  class l_taxisk_rek rk;
  class iceb_u_str kodklk;  
  
  l_taxisk_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.kodkl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KODKL]))));
    rk.fio.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_FIO]))));
    rk.adres.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_ADRES]))));
    rk.telef.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_TELEFON]))));
    rk.denrog.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DENROG]))));
    rk.koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT]))));
    
   }
 };


gboolean   l_taxisk_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_taxisk_v_data *data);
void  l_taxisk_v_knopka(GtkWidget *widget,class l_taxisk_v_data *data);
void    l_taxisk_v_vvod(GtkWidget *widget,class l_taxisk_v_data *data);
int l_taxisk_pk(const char *god,GtkWidget*);
int l_taxisk_zap(class l_taxisk_v_data *data);
void l_taxisk_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);


extern SQL_baza  bd;
extern char      *name_system;

int l_taxisk_v(class iceb_u_str *kodklk,GtkWidget *wpredok)
{

class l_taxisk_v_data data;
char strsql[300];
iceb_u_str kikz;
data.kodklk.new_plus(kodklk->ravno());

data.rk.clear_data();

if(data.kodklk.getdlinna() >  1)
 {
  data.rk.kodkl.new_plus(data.kodklk.ravno());
  sprintf(strsql,"select * from Taxiklient where kod='%s'",data.kodklk.ravno());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  data.rk.kodkl.new_plus(row[0]);
  data.rk.fio.new_plus(row[1]);
  data.rk.adres.new_plus(row[2]);
  data.rk.telef.new_plus(row[3]);
  data.rk.denrog.new_plus_sqldata(row[12]);
  data.rk.pol=atoi(row[11]);
  data.rk.koment.new_plus(row[13]);
  kikz.plus(iceb_kikz(row[4],row[5],wpredok));
  
 }
else
 {
  sprintf(strsql,"%d",iceb_get_new_kod("Taxiklient",1,wpredok));
  data.rk.kodkl.new_plus(strsql);
 } 

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.kodklk.getdlinna() <= 1)
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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_taxisk_v_key_press),&data);

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
//GtkWidget *hboxopt1 = gtk_hbox_new (FALSE,1);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);


label=gtk_label_new(gettext("Код"));
gtk_box_pack_start (GTK_BOX (hbox[E_KODKL]), label, FALSE, FALSE, 0);

data.entry[E_KODKL] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_KODKL]), data.entry[E_KODKL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODKL]), "activate",GTK_SIGNAL_FUNC(l_taxisk_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODKL]),data.rk.kodkl.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODKL]),(gpointer)E_KODKL);

label=gtk_label_new(gettext("Фамилия"));
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), label, FALSE, FALSE, 0);

data.entry[E_FIO] = gtk_entry_new_with_max_length (59);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_FIO]), "activate",GTK_SIGNAL_FUNC(l_taxisk_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.rk.fio.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_FIO]),(gpointer)E_FIO);

label=gtk_label_new(gettext("Адрес"));
gtk_box_pack_start (GTK_BOX (hbox[E_ADRES]), label, FALSE, FALSE, 0);

data.entry[E_ADRES] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox[E_ADRES]), data.entry[E_ADRES], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ADRES]), "activate",GTK_SIGNAL_FUNC(l_taxisk_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ADRES]),data.rk.adres.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ADRES]),(gpointer)E_ADRES);

label=gtk_label_new(gettext("Телефон"));
gtk_box_pack_start (GTK_BOX (hbox[E_TELEFON]), label, FALSE, FALSE, 0);

data.entry[E_TELEFON] = gtk_entry_new_with_max_length (39);
gtk_box_pack_start (GTK_BOX (hbox[E_TELEFON]), data.entry[E_TELEFON], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TELEFON]), "activate",GTK_SIGNAL_FUNC(l_taxisk_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TELEFON]),data.rk.telef.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TELEFON]),(gpointer)E_TELEFON);

sprintf(strsql,"%s (%s)",gettext("Дата рождения"),gettext("д.м.г"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DENROG]), label, FALSE, FALSE, 0);

data.entry[E_DENROG] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DENROG]), data.entry[E_DENROG], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DENROG]), "activate",GTK_SIGNAL_FUNC(l_taxisk_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DENROG]),data.rk.denrog.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DENROG]),(gpointer)E_DENROG);


label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(l_taxisk_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.rk.koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);


label=gtk_label_new(gettext("Состояние объекта"));
gtk_box_pack_start (GTK_BOX (hbox[E_POL]), label, FALSE, FALSE, 1);

class iceb_u_spisok bal_st;
bal_st.plus(gettext("Мужчина"));
bal_st.plus(gettext("Женщина"));

iceb_pm_vibor(&bal_st,&data.opt1,&data.rk.pol);
gtk_box_pack_start (GTK_BOX (hbox[E_POL]), data.opt1, TRUE, TRUE,1);


GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_taxisk_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_taxisk_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[1]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 kodklk->new_plus(data.rk.kodkl.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_taxisk_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_taxisk_v_data *data)
{
//char  bros[300];

//printf("l_taxisk_v_key_press\n");
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
void  l_taxisk_v_knopka(GtkWidget *widget,class l_taxisk_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_taxisk_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_taxisk_zap(data) == 0)
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

void    l_taxisk_v_vvod(GtkWidget *widget,class l_taxisk_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_taxisk_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KODKL:
    data->rk.kodkl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int l_taxisk_zap(class l_taxisk_v_data *data)
{
char strsql[1000];
//SQL_str row;
time_t   vrem;
time(&vrem);


if(data->rk.kodkl.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введен код !"),data->window);
  return(1);
 }

if(data->rk.fio.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не ввели фамилию!"),data->window);
  return(1);
 }
if(data->rk.denrog.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата!"),data->window);
  return(1);
 }

if(iceb_u_SRAV(data->kodklk.ravno(),data->rk.kodkl.ravno(),0) != 0)
  if(l_taxisk_pk(data->rk.kodkl.ravno(),data->window) != 0)
     return(1);
short metkakor=0;
printf("kodklk=%s kodkl=%s\n",data->kodklk.ravno(),data->rk.kodkl.ravno());
if(data->kodklk.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Taxiklient (kod,fio,adres,telef,pl,denrog,kom,ktoi,vrem) \
values ('%s','%s','%s','%s',%d,'%s','%s',%d,%ld)",
   data->rk.kodkl.ravno_filtr(),
   data->rk.fio.ravno_filtr(),
   data->rk.adres.ravno_filtr(),
   data->rk.telef.ravno_filtr(),
   data->rk.pol,
   data->rk.denrog.ravno_sqldata(),   
   data->rk.koment.ravno_filtr(),
   iceb_getuid(data->window),vrem);
 }
else
 {
  
  if(iceb_u_SRAV(data->kodklk.ravno(),data->rk.kodkl.ravno(),0) != 0)
   {

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Taxiklient \
set \
kod='%s',\
fio='%s',\
adres='%s',\
telef='%s',\
pl=%d,\
denrog='%s',\
kom='%s',\
ktoi=%d,\
vrem=%ld \
where kod=%d",
   data->rk.kodkl.ravno_filtr(),
   data->rk.fio.ravno_filtr(),
   data->rk.adres.ravno_filtr(),
   data->rk.telef.ravno_filtr(),
   data->rk.pol,
   data->rk.denrog.ravno_sqldata(),   
   data->rk.koment.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->kodklk.ravno_atoi());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

if(metkakor == 1)
 l_taxisk_kkvt(data->kodklk.ravno(),data->rk.kodkl.ravno(),data->window);

return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_taxisk_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[300];

sprintf(strsql,"select kod from Taxiklient where kod='%s'",kod);
//printf("l_taxisk_pk-%s\n",strsql);

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

void l_taxisk_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[300];

sprintf(strsql,"update Taxizak set kodkl='%s' where kodkl='%s'",nkod,skod);

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

sprintf(strsql,"update Taxiksal set kod='%s' where kod='%s'",nkod,skod);

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
