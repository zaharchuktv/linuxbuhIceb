/*$Id: iceb_l_opmur_v.c,v 1.23 2011-01-13 13:49:59 sasa Exp $*/
/*11.11.2008	05.05.2004	Белых А.И.	iceb_l_opmur_v.c
Ввод и корректировка операций расхода для "Материального учёта"
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "iceb_libbuh.h"
#include "iceb_l_opmur.h"

enum
 {
  E_KOD,
  E_NAIM,
  E_SHETA,
  KOLENTER  
 };

class iceb_l_opmur_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class opmur_rek rk;
  class iceb_u_str kod_op;  
  
  iceb_l_opmur_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
     }
   }
 };

gboolean   opmur_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_opmur_v_data *data);
void  opmur_v_knopka(GtkWidget *widget,class iceb_l_opmur_v_data *data);
void    opmur_v_vvod(GtkWidget *widget,class iceb_l_opmur_v_data *data);
int opmur_pk(const char *god,GtkWidget*);
int opmur_zap(class iceb_l_opmur_v_data *data);

GtkWidget *opmur_vidop_v(int *nomerstr);
GtkWidget *opmur_vidop_p(int *nomerstr);
void            opmur_get_pm0(GtkWidget*,int*);
void            opmur_get_pm1(GtkWidget*,int*);
void            opmur_get_pm2(GtkWidget*,int*);
void opmur_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok);

int iceb_l_opmur_v(class iceb_u_str *kod_op,GtkWidget *wpredok)
{
class iceb_l_opmur_v_data data;
char strsql[512];
SQL_str row;
SQLCURSOR cur;
data.kod_op.new_plus(kod_op->ravno());
data.rk.clear_zero();

if(data.kod_op.getdlinna() > 1)
 {
  data.rk.kod.new_plus(data.kod_op.ravno());
  
  sprintf(strsql,"select * from Rashod where kod='%s'",data.kod_op.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  data.rk.naim.new_plus(row[1]);
  data.rk.vidop=atoi(row[4]);
  data.rk.sheta.new_plus(row[5]);
  data.rk.prov=atoi(row[6]);
 }
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.kod_op.getdlinna() <= 1)
  sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));
else
  sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(opmur_v_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

if(data.kod_op.getdlinna() <= 1)
  sprintf(strsql,"%s",gettext("Ввод новой записи"));
else
 {
  sprintf(strsql,"%s\n%s:%s %s:%s",
  gettext("Корректировка записи"),
  gettext("Записал"),
  iceb_kszap(row[2],1,wpredok),
  gettext("Дата и время записи"),
  iceb_u_vremzap(row[3]));
 } 

GtkWidget *label=gtk_label_new(iceb_u_toutf(strsql));

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox1 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox2 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox3 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox4 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox5 = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hbox3);
gtk_container_add (GTK_CONTAINER (vbox), hbox4);
gtk_container_add (GTK_CONTAINER (vbox), hbox5);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


label=gtk_label_new(gettext("Код"));
data.entry[E_KOD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data.entry[E_KOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(opmur_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

label=gtk_label_new(gettext("Наименование"));
data.entry[E_NAIM] = gtk_entry_new_with_max_length (80);
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data.entry[E_NAIM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(opmur_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);

sprintf(strsql,"%s (,,)",gettext("Счета"));
label=gtk_label_new(strsql);
data.entry[E_SHETA] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox3), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox3), data.entry[E_SHETA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHETA]), "activate",GTK_SIGNAL_FUNC(opmur_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETA]),data.rk.sheta.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHETA]),(gpointer)E_SHETA);

label=gtk_label_new(gettext("Вид операции"));
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE, 0);
GtkWidget *menuvid = opmur_vidop_v(&data.rk.vidop);
gtk_box_pack_start (GTK_BOX (hbox4), menuvid, FALSE, FALSE, 0);

label=gtk_label_new(gettext("Проводки"));
gtk_box_pack_start (GTK_BOX (hbox5), label, FALSE, FALSE, 0);
menuvid = opmur_vidop_p(&data.rk.prov);
gtk_box_pack_start (GTK_BOX (hbox5), menuvid, FALSE, FALSE, 0);


//GtkWidget *knopka[KOL_PFK];
GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],data.knopka[PFK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK2]),"clicked",GTK_SIGNAL_FUNC(opmur_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK2]),(gpointer)PFK2);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],data.knopka[PFK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK10]),"clicked",GTK_SIGNAL_FUNC(opmur_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK10]),(gpointer)PFK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0)
 kod_op->new_plus(data.rk.kod.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   opmur_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_opmur_v_data *data)
{
//char  bros[512];

//printf("opmur_v_key_press\n");
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
void  opmur_v_knopka(GtkWidget *widget,class iceb_l_opmur_v_data *data)
{
//char bros[512];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
/*g_print("opmur_v_knopka knop=%d\n",*knop);*/

switch (knop)
 {
  case PFK2:
    if(opmur_zap(data) == 0)
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

void    opmur_v_vvod(GtkWidget *widget,class iceb_l_opmur_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("opmur_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->rk.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NAIM:
    data->rk.naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHETA:
    data->rk.sheta.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int opmur_zap(class iceb_l_opmur_v_data *data)
{
char strsql[1000];
//SQL_str row;
time_t   vrem;
time(&vrem);

printf("opmur_v_zap\n");

for(int i=0; i < KOLENTER; i++)
  gtk_signal_emit_by_name(GTK_OBJECT(data->entry[i]),"activate");

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

if(iceb_u_SRAV(data->kod_op.ravno(),data->rk.kod.ravno(),0) != 0)
 if(opmur_pk(data->rk.kod.ravno(),data->window) != 0)
  return(1);

short metkakor=0;

if(data->kod_op.getdlinna() <= 1)
 {

  
  sprintf(strsql,"insert into Rashod \
values ('%s','%s',%d,%ld,%d,'%s',%d)",
   data->rk.kod.ravno_filtr(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.vidop,
   data->rk.sheta.ravno_filtr(),
   data->rk.prov);
 }
else
 {
  
  if(iceb_u_SRAV(data->kod_op.ravno(),data->rk.kod.ravno(),0) != 0)
   {
    if(iceb_menu_danet(gettext("Корректировать код ? Вы уверены ?"),2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Rashod \
set \
kod='%s',\
naik='%s',\
ktoz=%d,\
vrem=%ld,\
vido=%d,\
sheta='%s',\
prov=%d \
where kod='%s'",
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.vidop,
   data->rk.sheta.ravno_filtr(),
   data->rk.prov,
   data->kod_op.ravno());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

if(data->kod_op.getdlinna() <= 1 && data->rk.vidop > 0)
 { 
  sprintf(strsql,"insert into Prihod \
values ('%s','%s',%d,%ld,%d,%d)",
   data->rk.kod.ravno_filtr(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.vidop,
   1);
  iceb_sql_zapis(strsql,0,0,data->window);
 }
gtk_widget_hide(data->window);

if(metkakor == 1)
 opmur_kkvt(data->kod_op.ravno(),data->rk.kod.ravno(),data->window);


return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int opmur_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Rashod where kod='%s'",kod);
//printf("opmur_pk-%s\n",strsql);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Такая запись уже есть !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

return(0);

}

/*********************************************/
/**********************************************/

GtkWidget *opmur_make_menu_item( gchar *name,GtkSignalFunc callback,gpointer data)
{
GtkWidget *item;
  
item = gtk_menu_item_new_with_label (name);
gtk_signal_connect (GTK_OBJECT (item), "activate",callback, data);
gtk_widget_show (item);
//printf("make_menu_item\n");
return(item);
}



/*****************************/
/*Меню выбора вида операции     */
/*****************************/

GtkWidget *opmur_vidop_v(int *nomerstr)
{

GtkWidget *opt = gtk_option_menu_new();
GtkWidget *menu = gtk_menu_new();
GtkWidget *item = opmur_make_menu_item (gettext("Внешняя") ,GTK_SIGNAL_FUNC(opmur_get_pm0),nomerstr);
gtk_menu_append (GTK_MENU (menu), item);

item = opmur_make_menu_item (gettext("Внутренняя"), GTK_SIGNAL_FUNC (opmur_get_pm1),nomerstr);
gtk_menu_append (GTK_MENU (menu), item);

item = opmur_make_menu_item (gettext("Изменения стоимости"), GTK_SIGNAL_FUNC (opmur_get_pm2),nomerstr);
gtk_menu_append (GTK_MENU (menu), item);

gtk_option_menu_set_menu (GTK_OPTION_MENU (opt), menu);

gtk_option_menu_set_history(GTK_OPTION_MENU(opt),*nomerstr);
return(opt);
}

/*****************************/
/*Меню выбора проводок     */
/*****************************/

GtkWidget *opmur_vidop_p(int *nomerstr)
{

GtkWidget *opt = gtk_option_menu_new();
GtkWidget *menu = gtk_menu_new();
GtkWidget *item = opmur_make_menu_item (gettext("Нужно делать") ,GTK_SIGNAL_FUNC(opmur_get_pm0),nomerstr);
gtk_menu_append (GTK_MENU (menu), item);

item = opmur_make_menu_item (gettext("Не нужно делать"), GTK_SIGNAL_FUNC (opmur_get_pm1),nomerstr);
gtk_menu_append (GTK_MENU (menu), item);

gtk_option_menu_set_menu (GTK_OPTION_MENU (opt), menu);

gtk_option_menu_set_history(GTK_OPTION_MENU(opt),*nomerstr);
return(opt);
}
/**********************/
/*Читаем              */
/**********************/

void   opmur_get_pm0(GtkWidget *widget,int *data)
{
*data=0;
printf("opmur_get_pm0-%d\n",*data);
}
void   opmur_get_pm1(GtkWidget *widget,int *data)
{
*data=1;
printf("opmur_get_pm1-%d\n",*data);
}
void   opmur_get_pm2(GtkWidget *widget,int *data)
{
*data=2;
printf("opmur_get_pm2-%d\n",*data);
}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void opmur_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Dokummat set kodop='%s' where kodop='%s' and tip=2",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
  return;
 }

sprintf(strsql,"update Prov set oper='%s' where oper='%s' and tz=2 and kto='%s'",nkod,skod,gettext("МУ"));
iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Reenn set ko='%s' where ko='%s' and mi=1",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

}

