/*$Id: l_uplout_v.c,v 1.12 2011-02-21 07:35:54 sasa Exp $*/
/*05.03.2008	05.03.2008	Белых А.И.	l_uplout_v.c
Ввод и корректировка объектов учёта топлива
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
#include "l_uplout.h"
enum
 {
  E_KOD,
  E_NAIM,
  E_SKLAD,
  E_KONTR,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_uplout_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *opt;
  GtkWidget *label_naim[KOLENTER];

  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_uplout_rek rk;
  class iceb_u_str kod_k;  
  class iceb_u_str kodkon; /*код контрагента в корректируемой записи*/
    
  l_uplout_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    kodkon.plus("");
   }

  void read_rek()
   {
    rk.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD]))));
    rk.naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM]))));
    rk.sklad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SKLAD]))));
    rk.kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR]))));
   }
 };


gboolean   l_uplout_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplout_v_data *data);
void  l_uplout_v_knopka(GtkWidget *widget,class l_uplout_v_data *data);
void    l_uplout_v_vvod(GtkWidget *widget,class l_uplout_v_data *data);
int l_uplout_pk(const char *god,GtkWidget*);
int l_uplout_zap(class l_uplout_v_data *data);
void l_uplout_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);

void  l_uplout_v_e_knopka(GtkWidget *widget,l_uplout_v_data *data);

extern SQL_baza  bd;
extern char      *name_system;

int l_uplout_v(class iceb_u_str *kod_k,GtkWidget *wpredok)
{
class iceb_u_str naim_sklad("          ");
class iceb_u_str naim_kontr("          ");
class l_uplout_v_data data;
char strsql[512];
iceb_u_str kikz;
data.kod_k.new_plus(kod_k->ravno());

data.rk.clear_data();

if(data.kod_k.getdlinna() >  1)
 {
  data.rk.kod.new_plus(data.kod_k.ravno());
  sprintf(strsql,"select * from Uplouot where kod='%s'",data.kod_k.ravno());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  data.rk.metka_kod=atoi(row[1]);
  data.rk.sklad.new_plus(row[2]);
  data.rk.naim.new_plus(row[3]);

  data.rk.kontr.new_plus(row[4]);
  data.kodkon.new_plus(row[4]);

  kikz.plus(iceb_kikz(row[5],row[6],wpredok));
  /*Читаем наименование склада*/
  sprintf(strsql,"select naik from Sklad where kod=%d",data.rk.sklad.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_sklad.new_plus(row[0]);  
  /*Читаем наименование контрагента*/
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data.rk.kontr.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_kontr.new_plus(row[0]);  
 }
else
 {
  sprintf(strsql,"%d",iceb_get_new_kod("Uplouot",0,wpredok));
  data.rk.kod.new_plus(strsql);
 } 
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.kod_k.getdlinna() <= 1)
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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_uplout_v_key_press),&data);

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
GtkWidget *hbox_metka_kod = gtk_hbox_new (FALSE,1);
GtkWidget *hboxknop = gtk_hbox_new (FALSE,1);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hbox_metka_kod);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];


label=gtk_label_new(gettext("Код"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE,1);

data.entry[E_KOD] = gtk_entry_new_with_max_length (4);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(l_uplout_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

label=gtk_label_new(gettext("Наименование"));
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE,1);

data.entry[E_NAIM] = gtk_entry_new_with_max_length (79);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(l_uplout_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);


sprintf(strsql,"%s",gettext("Склад"));
data.knopka_enter[E_SKLAD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.knopka_enter[E_SKLAD], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SKLAD]),"clicked",GTK_SIGNAL_FUNC(l_uplout_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SKLAD]),(gpointer)E_SKLAD);
tooltips_enter[E_SKLAD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SKLAD],data.knopka_enter[E_SKLAD],gettext("Выбор склада"),NULL);

data.entry[E_SKLAD] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.entry[E_SKLAD], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SKLAD]), "activate",GTK_SIGNAL_FUNC(l_uplout_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SKLAD]),data.rk.sklad.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SKLAD]),(gpointer)E_SKLAD);

data.label_naim[E_SKLAD]=gtk_label_new(naim_sklad.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.label_naim[E_SKLAD], FALSE, FALSE,1);


sprintf(strsql,"%s",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KONTR]),"clicked",GTK_SIGNAL_FUNC(l_uplout_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KONTR]),(gpointer)E_KONTR);
tooltips_enter[E_KONTR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KONTR],data.knopka_enter[E_KONTR],gettext("Выбор контрагента"),NULL);

data.entry[E_KONTR] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KONTR]), "activate",GTK_SIGNAL_FUNC(l_uplout_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rk.sklad.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KONTR]),(gpointer)E_KONTR);

data.label_naim[E_KONTR]=gtk_label_new(naim_kontr.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.label_naim[E_KONTR], FALSE, FALSE,1);



label=gtk_label_new(gettext("Вид объекта"));
gtk_box_pack_start (GTK_BOX (hbox_metka_kod), label, FALSE, FALSE,1);

class iceb_u_spisok metka_ob;
metka_ob.plus(gettext("Заправка"));
metka_ob.plus(gettext("Водитель"));

iceb_pm_vibor(&metka_ob,&data.opt,&data.rk.metka_kod);
gtk_box_pack_start (GTK_BOX (hbox_metka_kod), data.opt, TRUE,TRUE,1);


GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_uplout_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_uplout_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 kod_k->new_plus(data.rk.kod.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_uplout_v_e_knopka(GtkWidget *widget,l_uplout_v_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {

  case E_SKLAD:

    iceb_vibrek(1,"Sklad",&data->rk.sklad,data->window);

    /*Читаем наименование склада*/
    sprintf(strsql,"select naik from Sklad where kod=%d",data->rk.sklad.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
      naim.new_plus(row[0]);
    gtk_label_set_text(GTK_LABEL(data->label_naim[E_SKLAD]),naim.ravno_toutf(20));
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk.sklad.ravno_toutf());

    return;  

  case E_KONTR:

    iceb_vibrek(1,"Kontragent",&data->rk.kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rk.kontr.ravno_toutf());

    /*Читаем наименование контрагента*/
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->rk.kontr.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
      naim.new_plus(row[0]);  
    gtk_label_set_text(GTK_LABEL(data->label_naim[E_KONTR]),naim.ravno_toutf(20));

    return;  



 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uplout_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplout_v_data *data)
{
//char  bros[512];

//printf("l_uplout_v_key_press\n");
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
void  l_uplout_v_knopka(GtkWidget *widget,class l_uplout_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uplout_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_uplout_zap(data) == 0)
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

void    l_uplout_v_vvod(GtkWidget *widget,class l_uplout_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uplout_v_vvod enter=%d\n",enter);
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str naim("");
switch (enter)
 {

  case E_KOD:
    data->rk.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NAIM:
    data->rk.naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SKLAD:
    data->rk.sklad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    /*Читаем наименование склада*/
    sprintf(strsql,"select naik from Sklad where kod=%d",data->rk.sklad.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
      naim.new_plus(row[0]);
    gtk_label_set_text(GTK_LABEL(data->label_naim[E_SKLAD]),naim.ravno_toutf(20));

    break;

  case E_KONTR:
    data->rk.kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    /*Читаем наименование контрагента*/
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->rk.kontr.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
      naim.new_plus(row[0]);  
    gtk_label_set_text(GTK_LABEL(data->label_naim[E_KONTR]),naim.ravno_toutf(20));
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
int l_uplout_zap(class l_uplout_v_data *data)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
time_t   vrem;
time(&vrem);


if(data->rk.kod.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введён код !"),data->window);
  return(1);
 }

if(data->rk.kod.ravno_atoi() == 0)
 {
  iceb_menu_soob(gettext("Код должен быть цифровым и неравным нолю!"),data->window);
  return(1);
 }

if(data->rk.naim.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введено наименование !"),data->window);
  return(1);
 }

sprintf(strsql,"select naik from Sklad where kod=%d",data->rk.sklad.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не нашли код склада"),data->rk.sklad.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->rk.kontr.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не нашли код контрагента"),data->rk.kontr.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }


if(iceb_u_SRAV(data->kodkon.ravno(),data->rk.kontr.ravno(),0) != 0)
 {
  //Проверяем может этот код контрагента уже введен для другого объекта
  sprintf(strsql,"select kod,naik from Uplouot where kontr='%s' and mt=%d",data->rk.kontr.ravno(),data->rk.metka_kod);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
   {
    class iceb_u_str repl;
    repl.plus(gettext("Этот код контрагента уже ввели для другого объекта!"));
    
    sprintf(strsql,"%s %s",row[0],row[1]);
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,data->window);
    return(1);
   }
 }

short metkakor=0;

if(iceb_u_SRAV(data->kod_k.ravno(),data->rk.kod.ravno(),0) != 0)
  if(l_uplout_pk(data->rk.kod.ravno(),data->window) != 0)
     return(1);

if(data->kod_k.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Uplouot \
values (%d,%d,%d,'%s','%s',%d,%ld)",
   data->rk.kod.ravno_atoi(),
   data->rk.metka_kod,
   data->rk.sklad.ravno_atoi(),
   data->rk.naim.ravno_filtr(),
   data->rk.kontr.ravno_filtr(),
   iceb_getuid(data->window),vrem);
 }
else
 {
  
  if(iceb_u_SRAV(data->kod_k.ravno(),data->rk.kod.ravno(),0) != 0)
   {

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Uplouot \
set \
kod=%d,\
mt=%d,\
skl=%d,\
naik='%s',\
kontr='%s',\
ktoz=%d,\
vrem=%ld \
where kod=%d",
   data->rk.kod.ravno_atoi(),
   data->rk.metka_kod,
   data->rk.sklad.ravno_atoi(),
   data->rk.naim.ravno_filtr(),
   data->rk.kontr.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->kod_k.ravno_atoi());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);

if(metkakor == 1)
 l_uplout_kkvt(data->kod_k.ravno(),data->rk.kod.ravno(),data->window);

return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_uplout_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Uplouot where kod=%d",atoi(kod));
//printf("l_uplout_pk-%s\n",strsql);

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

void l_uplout_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];


sprintf(strsql,"update Upldok set kv=%s where kv=%s",nkod,skod);
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
  {
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
   return;
  }
 }
sprintf(strsql,"update Upldok1 set kv=%s where kv=%s",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Uplsal set kv=%s where kv=%s",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Uplavt set kzv='%s' where kzv=%s",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

}
