/*$Id: l_xdkop_v.c,v 1.15 2011-02-21 07:35:54 sasa Exp $*/
/*03.04.2006	18.01.2006	Белых А.И.	l_xdkop_v.c
Ввод и корректировка записей кодов операций прихода для "Учёта кассовых ордеров"
*/
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "buhg_g.h"
#include "l_xdkop.h"

enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KOD,
  E_NAIM,
  KOLENTER  
 };

class l_xdkop_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class xdkop_rek rk;
    
  iceb_u_str kodk; //Код записи которую корректируют
  iceb_u_str tablica; //Таблица с которой работаем
  
  l_xdkop_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }
 };

gboolean   l_xdkop_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_xdkop_v_data *data);
void    l_xdkop_v_v_vvod(GtkWidget *widget,class l_xdkop_v_data *data);
void  l_xdkop_v_v_knopka(GtkWidget *widget,class l_xdkop_v_data *data);
void xdkop_kkvt(const char *skod,const char *nkod,const char*,GtkWidget *wpredok);
int xdkop_zap(class l_xdkop_v_data *data);
int xdkop_pk(const char *kod,const char*,GtkWidget *wpredok);


extern char *name_system;
extern SQL_baza bd;
extern char	*shrt;    /*Счета розничной торговли*/
extern double	okrcn;  /*Округление цены*/

int l_xdkop_v(iceb_u_str *kodzap, //Код записи для корректировки ; возвращается код только что введённой записи
const char *tablica,
GtkWidget *wpredok)
{
class l_xdkop_v_data data;
data.kodk.new_plus(kodzap->ravno());
data.tablica.new_plus(tablica);

char strsql[512];
iceb_u_str kikz;

if(kodzap->getdlinna() > 1)
 {
  //читаем корректируемую запись
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select * from %s where kod='%s'",data.tablica.ravno(),kodzap->ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  data.rk.kod.new_plus(row[0]);
  data.rk.naim.new_plus(row[1]);
  kikz.plus(iceb_kikz(row[2],row[3],wpredok));

 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(kodzap->getdlinna() > 1)
 sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));
else
 sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_xdkop_v_v_key_press),&data);

iceb_u_str zagolov;
if(kodzap->getdlinna() <= 1)
 zagolov.new_plus(gettext("Ввод новой записи"));
else
 {
  zagolov.new_plus(gettext("Корректировка записи"));
  zagolov.ps_plus(kikz.ravno());
 }

GtkWidget *label=gtk_label_new(zagolov.ravno_toutf());

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

sprintf(strsql,"%s",gettext("Код операции"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_KOD] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(l_xdkop_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

sprintf(strsql,"%s",gettext("Наименование операции"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_NAIM] = gtk_entry_new_with_max_length (59);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(l_xdkop_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);


GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введённой в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_xdkop_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_xdkop_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
  kodzap->new_plus(data.rk.kod.ravno());
return(data.voz);
}


/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_xdkop_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_xdkop_v_data *data)
{

//printf("l_xdkop_v_v_key_press\n");
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
void  l_xdkop_v_v_knopka(GtkWidget *widget,class l_xdkop_v_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(xdkop_zap(data) != 0)
      return;

    gtk_widget_destroy(data->window);
    data->window=NULL;

    data->voz=0;
    return;  

    
  case FK10:
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_xdkop_v_v_vvod(GtkWidget *widget,class l_xdkop_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_xdkop_v_v_vvod enter=%d\n",enter);

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
int xdkop_zap(class l_xdkop_v_data *data)
{
char strsql[1024];
//SQL_str row;
time_t   vrem;
time(&vrem);


data->read_rek(); //Читаем реквизиты меню

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

if(iceb_u_SRAV(data->kodk.ravno(),data->rk.kod.ravno(),0) != 0)
 if(xdkop_pk(data->rk.kod.ravno(),data->tablica.ravno(),data->window) != 0)
  return(1);
  
short metkakor=0;

if(data->kodk.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into %s \
values ('%s','%s',%d,%ld)",
   data->tablica.ravno(),
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem);
 }
else
 {
  
  if(iceb_u_SRAV(data->kodk.ravno(),data->rk.kod.ravno(),0) != 0)
   {
    if(iceb_menu_danet(gettext("Корректировать код ? Вы уверены ?"),2,data->window) == 2)
      return(1);
    metkakor=1;
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update %s \
set \
kod='%s',\
naik='%s',\
ktoz=%d,\
vrem=%ld \
where kod='%s'",
   data->tablica.ravno(),
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->kodk.ravno());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);

if(metkakor == 1)
 xdkop_kkvt(data->kodk.ravno(),data->rk.kod.ravno(),data->tablica.ravno(),data->window);


return(0);

}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void xdkop_kkvt(const char *skod,const char *nkod,const char *tablica,GtkWidget *wpredok)
{
char strsql[512];
//Корректируем в документах
sprintf(strsql,"update %s set kod='%s' where kod='%s'",tablica,nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
  return;
 }

//Корректрируем в типовых документах
if(iceb_u_SRAV(tablica,"Pltp",0) == 0)
  sprintf(strsql,"update Tpltp set oper='%s' where oper='%s'",nkod,skod);
if(iceb_u_SRAV(tablica,"Pltt",0) == 0)
  sprintf(strsql,"update Tpltt set oper='%s' where oper='%s'",nkod,skod);

if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

//Корректрируем в проводках

if(iceb_u_SRAV(tablica,"Pltp",0) == 0)
  sprintf(strsql,"update Prov set oper='%s' where oper='%s' and tz=2 and kto='%s'",nkod,skod,gettext("ПЛТ"));

if(iceb_u_SRAV(tablica,"Pltt",0) == 0)
  sprintf(strsql,"update Prov set oper='%s' where oper='%s' and tz=1 and kto='%s'",nkod,skod,gettext("ТРЕ"));

if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int xdkop_pk(const char *kod,const char *tablica,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from %s where kod='%s'",tablica,kod);
//printf("ei_pk-%s\n",strsql);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Такая запись уже есть !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

return(0);

}
