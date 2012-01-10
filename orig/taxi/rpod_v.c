/*$Id: rpod_v.c,v 1.13 2011-01-13 13:50:09 sasa Exp $*/
/*12.01.2006	29.04.2004	Белых А.И.	rpod_v.c
Ввод и корректировка сальдо по счетам
*/
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include "i_rest.h"
#include "rpod.h"


gboolean   rpod_v_key_press(GtkWidget *widget,GdkEventKey *event,class rpod_data *data);
void  rpod_v_knopka(GtkWidget *widget,class rpod_data *data);
void    rpod_v_vvod(GtkWidget *widget,class rpod_data *data);
int rpod_pk(const char *god,GtkWidget*);
int rpod_zap(class rpod_data *data);
void rpod_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);

extern uid_t kod_operatora;

void rpod_v(class rpod_data *data)
{
char strsql[300];

//printf("l_rpod_v\n");
if(data->vwindow != NULL)
  return;
    
data->kl_shift=0;
data->vvod.clear_zero();

if(data->metkazapisi == 1)
 {
  data->vvod.kod.new_plus(data->kodv.ravno());
  data->vvod.naim.new_plus(data->naimv.ravno());
  
 }
else
 {
  sprintf(strsql,"%d",iceb_get_new_kod("Restpod",0,data->window));

  data->vvod.kod.new_plus(strsql);

 }
 
data->vwindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data->vwindow),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data->vwindow),TRUE);
if(data->metkazapisi == 0)
  sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));
if(data->metkazapisi == 1)
  sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));


gtk_window_set_title(GTK_WINDOW(data->vwindow),strsql);
gtk_signal_connect(GTK_OBJECT(data->vwindow),"destroy",GTK_SIGNAL_FUNC(gtk_widget_destroyed),&data->vwindow);
gtk_signal_connect_after(GTK_OBJECT(data->vwindow),"key_press_event",GTK_SIGNAL_FUNC(rpod_v_key_press),data);


//Удерживать окно над породившем его окном всегда
gtk_window_set_transient_for(GTK_WINDOW(data->vwindow),GTK_WINDOW(data->window));
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data->vwindow),TRUE);

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox1 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox2 = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data->vwindow), vbox);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


GtkWidget *label=gtk_label_new(gettext("Код подразделения"));
data->entry[E_KOD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data->entry[E_KOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data->entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(rpod_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD]),data->vvod.kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data->entry[E_KOD]),(gpointer)E_KOD);

label=gtk_label_new(gettext("Наименование подразделения"));
data->entry[E_NAIM] = gtk_entry_new_with_max_length (80);
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data->entry[E_NAIM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data->entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(rpod_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_NAIM]),data->vvod.naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data->entry[E_NAIM]),(gpointer)E_NAIM);

//GtkWidget *knopka[KOL_PFK];
GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data->knopka_pv[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],data->knopka_pv[PFK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data->knopka_pv[PFK2]),"clicked",GTK_SIGNAL_FUNC(rpod_v_knopka),data);
gtk_object_set_user_data(GTK_OBJECT(data->knopka_pv[PFK2]),(gpointer)PFK2);

sprintf(strsql,"F10 %s",gettext("Выход"));
data->knopka_pv[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],data->knopka_pv[PFK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data->knopka_pv[PFK10]),"clicked",GTK_SIGNAL_FUNC(rpod_v_knopka),data);
gtk_object_set_user_data(GTK_OBJECT(data->knopka_pv[PFK10]),(gpointer)PFK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data->knopka_pv[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data->knopka_pv[PFK10], TRUE, TRUE, 0);

if(data->metkazapisi == 1)
  gtk_widget_grab_focus(data->entry[0]);
else
  gtk_widget_grab_focus(data->entry[1]);

gtk_widget_show_all (data->vwindow);


}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rpod_v_key_press(GtkWidget *widget,GdkEventKey *event,class rpod_data *data)
{
//char  bros[300];

//printf("rpod_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka_pv[FK2]),"clicked");

    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka_pv[FK10]),"clicked");

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
void  rpod_v_knopka(GtkWidget *widget,class rpod_data *data)
{
//char bros[300];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case PFK2:
    if(rpod_zap(data) == 0)
     {
      gtk_widget_destroy(data->vwindow);
      data->vwindow=NULL;
     }
    return;  


  case PFK10:
    gtk_widget_destroy(data->vwindow);
    data->vwindow=NULL;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    rpod_v_vvod(GtkWidget *widget,class rpod_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("rpod_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->vvod.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NAIM:
    data->vvod.naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int rpod_zap(class rpod_data *data)
{
char strsql[1000];
//SQL_str row;
time_t   vrem;
time(&vrem);

printf("rpod_v_zap\n");

for(int i=0; i < KOLENTER; i++)
  gtk_signal_emit_by_name(GTK_OBJECT(data->entry[i]),"activate");

if(data->vvod.kod.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введен код !"));
  iceb_menu_soob(&sp,data->vwindow);
  return(1);
 }

if(isdigit(data->vvod.kod.ravno()[0]) == 0)
 {
  iceb_u_str sp;
  sp.plus(gettext("Код должен быть цифровым !"));
  iceb_menu_soob(&sp,data->vwindow);
  return(1);
 }

if(data->vvod.naim.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введено наименование !"));
  iceb_menu_soob(&sp,data->vwindow);
  return(1);
 }

short metkakor=0;

if(data->metkazapisi == 0)
 {

  if(rpod_pk(data->vvod.kod.ravno(),data->vwindow) != 0)
   return(1);
  
  sprintf(strsql,"insert into Restpod \
values (%d,'%s',%d,%ld)",
   atoi(data->vvod.kod.ravno()),
   data->vvod.naim.ravno_filtr(),
   kod_operatora,vrem);
 }
else
 {
  printf("metkazapisi=%d\n",data->metkazapisi);
  
  if(iceb_u_SRAV(data->kodv.ravno(),data->vvod.kod.ravno(),0) != 0)
   {
    if(rpod_pk(data->vvod.kod.ravno(),data->vwindow) != 0)
     return(1);

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->vwindow) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Restpod \
set \
kod=%d,\
naik='%s',\
ktoi=%d,\
vrem=%ld \
where kod=%d",
   atoi(data->vvod.kod.ravno()),
   data->vvod.naim.ravno_filtr(),
   kod_operatora,vrem,
   atoi(data->kodv.ravno()));
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->vwindow) != 0)
 return(1);

gtk_widget_hide(data->vwindow);

if(metkakor == 1)
 rpod_kkvt(data->kodv.ravno(),data->vvod.kod.ravno(),data->vwindow);

rpod_create_list(data);


return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int rpod_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[300];

sprintf(strsql,"select kod from Restpod where kod=%d",atoi(kod));
//printf("rpod_pk-%s\n",strsql);

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

void rpod_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok)
{

char strsql[300];

sprintf(strsql,"update Restdok set podr=%s where podr=%s",kodn,kods);

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
sprintf(strsql,"update Restdok1 set podr=%s where podr=%s",kodn,kods);

if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);


}
