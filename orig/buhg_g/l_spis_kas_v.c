/*$Id: l_spis_kas_v.c,v 1.15 2011-02-21 07:35:53 sasa Exp $*/
/*18.01.2006	18.01.2006	Белых А.И.	l_spis_kas_v.c
Ввод и корректировка записей кодов операций прихода для "Учёта кассовых ордеров"
*/
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "buhg_g.h"
#include "l_kasop.h"

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
  E_SHET,
  E_FIO_KASIRA,
  KOLENTER  
 };



class l_spis_kas_v_data
 {
  public:
  //Реквизиты записи
  iceb_u_str kod;
  iceb_u_str naim;
  iceb_u_str shet;
  iceb_u_str fio_kasira;    



  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *label_naishet;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  iceb_u_str kodk; //Код записи которую корректируют
  
  l_spis_kas_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    kod.plus("");
    naim.plus("");
    shet.plus("");
    fio_kasira.plus("");
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }
 };

gboolean   l_spis_kas_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_spis_kas_v_data *data);
void    l_spis_kas_v_v_vvod(GtkWidget *widget,class l_spis_kas_v_data *data);
void  l_spis_kas_v_v_knopka(GtkWidget *widget,class l_spis_kas_v_data *data);
void spis_kas_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok);
int spis_kas_zap(class l_spis_kas_v_data *data);
int spis_kas_pk(const char *kod,GtkWidget *wpredok);

void  spis_kas_v_e_knopka(GtkWidget *widget,class l_spis_kas_v_data *data);
void  spis_kas_radio2_0(GtkWidget *widget,class l_spis_kas_v_data *data);
void  spis_kas_radio2_1(GtkWidget *widget,class l_spis_kas_v_data *data);

extern char *name_system;
extern SQL_baza bd;
extern char	*shrt;    /*Счета розничной торговли*/
extern double	okrcn;  /*Округление цены*/

int l_spis_kas_v(iceb_u_str *kodzap, //Код записи для корректировки ; возвращается код только что введённой записи
GtkWidget *wpredok)
{
l_spis_kas_v_data data;
data.kodk.new_plus(kodzap->ravno());
iceb_u_str naim_shet;
iceb_u_str naim_shetk;
naim_shet.plus("");
naim_shetk.plus("");
char strsql[512];
iceb_u_str kikz;

if(kodzap->getdlinna() > 1)
 {
  //читаем корректируемую запись
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select * from Kas where kod=%d",kodzap->ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  data.kod.new_plus(row[0]);
  data.naim.new_plus(row[1]);
  data.shet.new_plus(row[2]);
  data.fio_kasira.new_plus(row[3]);
  kikz.plus(iceb_kikz(row[4],row[5],wpredok));

  if(data.shet.getdlinna() > 1)
   {
    //Узнаём наименование счёта
    sprintf(strsql,"select nais from Plansh where ns='%s'",data.shet.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim_shet.new_plus(row[0]);
   }

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_spis_kas_v_v_key_press),&data);

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

GtkWidget *separator1=gtk_hseparator_new();
gtk_container_add (GTK_CONTAINER (vbox), separator1);


gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Код кассы"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_KOD] = gtk_entry_new_with_max_length (4);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(l_spis_kas_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

sprintf(strsql,"%s",gettext("Наименование кассы"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_NAIM] = gtk_entry_new_with_max_length (59);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(l_spis_kas_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);

sprintf(strsql,"%s",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(spis_kas_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор единицы измерения"),NULL);

data.entry[E_SHET] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(l_spis_kas_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

data.label_naishet=gtk_label_new(naim_shet.ravno_toutf());
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.label_naishet,TRUE, TRUE, 0);

sprintf(strsql,"%s",gettext("Фамилия кассира"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_FIO_KASIRA] = gtk_entry_new_with_max_length (59);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO_KASIRA]), label,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO_KASIRA]), data.entry[E_FIO_KASIRA],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_FIO_KASIRA]), "activate",GTK_SIGNAL_FUNC(l_spis_kas_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO_KASIRA]),data.fio_kasira.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_FIO_KASIRA]),(gpointer)E_FIO_KASIRA);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введённой в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_spis_kas_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_spis_kas_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 kodzap->new_plus(data.kod.ravno());
return(data.voz);
}

/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  spis_kas_v_e_knopka(GtkWidget *widget,class l_spis_kas_v_data *data)
{
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {

  case E_SHET:
    if(iceb_vibrek(1,"Plansh",&data->shet,&naim,data->window) == 0)
       gtk_label_set_text(GTK_LABEL(data->label_naishet),naim.ravno_toutf(20));
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet.ravno_toutf());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_spis_kas_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_spis_kas_v_data *data)
{

//printf("l_spis_kas_v_v_key_press\n");
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
void  l_spis_kas_v_v_knopka(GtkWidget *widget,class l_spis_kas_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(spis_kas_zap(data) != 0)
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

void    l_spis_kas_v_v_vvod(GtkWidget *widget,class l_spis_kas_v_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_spis_kas_v_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NAIM:
    data->naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHET:
    data->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    sprintf(strsql,"select nais from Plansh where ns='%s'",data->shet.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naishet),iceb_u_toutf(strsql));
     }    
    break;

  case E_FIO_KASIRA:
    data->fio_kasira.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int spis_kas_zap(class l_spis_kas_v_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);


data->read_rek(); //Читаем реквизиты меню

if(data->kod.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введён код !"),data->window);
  return(1);
 }

if(data->kod.ravno_atoi() == 0)
 {
  iceb_menu_soob(gettext("Код должен быть цифровым и неравным нолю!"),data->window);
  return(1);
 }

if(data->naim.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введено наименование !"),data->window);
  return(1);
 }

class OPSHET opsh;

if(data->shet.getdlinna() > 1)
 if(iceb_prsh1(data->shet.ravno(),&opsh,data->window) != 0)
  return(1);

short metkakor=0;

if(data->kodk.getdlinna() <= 1)
 {

  if(spis_kas_pk(data->kod.ravno(),data->window) != 0)
   return(1);
  
  sprintf(strsql,"insert into Kas \
values (%d,'%s','%s','%s',%d,%ld)",
   data->kod.ravno_atoi(),
   data->naim.ravno_filtr(),
   data->shet.ravno(),
   data->fio_kasira.ravno_filtr(),
   iceb_getuid(data->window),vrem);
 }
else
 {
  
  if(iceb_u_SRAV(data->kodk.ravno(),data->kod.ravno(),0) != 0)
   {
//    iceb_menu_soob(gettext("Код операции корректировать нельзя !"),data->window);
//    return(1);
    if(spis_kas_pk(data->kod.ravno(),data->window) != 0)
     return(1);

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Kas \
set \
kod=%d,\
naik='%s',\
shet='%s',\
fio='%s',\
ktoz=%d,\
vrem=%ld \
where kod=%d",
   data->kod.ravno_atoi(),
   data->naim.ravno_filtr(),
   data->shet.ravno_filtr(),
   data->fio_kasira.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->kodk.ravno_atoi());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);

if(metkakor == 1)
 spis_kas_kkvt(data->kodk.ravno(),data->kod.ravno(),data->window);


return(0);

}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void spis_kas_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Kasord set kassa=%d where kassa=%d",atoi(nkod),atoi(skod));
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
sprintf(strsql,"update Kasord1 set kassa=%d where kassa=%d",atoi(nkod),atoi(skod));
iceb_sql_zapis(strsql,0,0,wpredok);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int spis_kas_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Kasop1 where kod='%s'",kod);
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
