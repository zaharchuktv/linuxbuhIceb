/*$Id: l_uslopr_v.c,v 1.18 2011-02-21 07:35:54 sasa Exp $*/
/*27.03.2008	18.07.2005	Белых А.И.	l_uslopr_v.c
Ввод и корректировка записей 
*/
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "buhg_g.h"
#include "l_uslop.h"

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
  KOLENTER  
 };

class l_uslopr_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *radiobutton1[2];
  GtkWidget *radiobutton2[2];
  GtkWidget *label_naishet;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class uslop_rek rk;
  
  iceb_u_str kodk; //Код записи которую корректируют
  
  l_uslopr_v_data() //Конструктор
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

gboolean   l_uslopr_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uslopr_v_data *data);
void    l_uslopr_v_v_vvod(GtkWidget *widget,class l_uslopr_v_data *data);
void  l_uslopr_v_v_knopka(GtkWidget *widget,class l_uslopr_v_data *data);
void uslopr_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok);
int uslopr_zap(class l_uslopr_v_data *data);
int uslopr_pk(const char *kod,GtkWidget *wpredok);

void  uslopr_v_e_knopka(GtkWidget *widget,class l_uslopr_v_data *data);
void  uslopr_radio1_0(GtkWidget *widget,class l_uslopr_v_data *data);
void  uslopr_radio1_1(GtkWidget *widget,class l_uslopr_v_data *data);
void  uslopr_radio2_0(GtkWidget *widget,class l_uslopr_v_data *data);
void  uslopr_radio2_1(GtkWidget *widget,class l_uslopr_v_data *data);

extern char *name_system;
extern SQL_baza bd;
extern char	*shrt;    /*Счета розничной торговли*/
extern double	okrcn;  /*Округление цены*/

int l_uslopr_v(iceb_u_str *kodzap, //Код записи для корректировки; возвращается введённый код
GtkWidget *wpredok)
{
l_uslopr_v_data data;
data.kodk.new_plus(kodzap->ravno());
iceb_u_str naim_shet;
naim_shet.plus("");
char strsql[512];
iceb_u_str kikz;

if(kodzap->getdlinna() > 1)
 {
  //читаем корректируемую запись
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select * from Usloper2 where kod='%s'",kodzap->ravno());
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
  data.rk.vido=atoi(row[4]);
  data.rk.prov=atoi(row[5]);
  data.rk.shet.new_plus(row[6]);
  if(data.rk.shet.getdlinna() > 1)
   {
    //Узнаём наименование счёта
    sprintf(strsql,"select nais from Plansh where ns='%s'",data.rk.shet.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim_shet.new_plus(row[0]);
   }
 }
/*******************
else
 {
  sprintf(strsql,"%d",iceb_get_new_kod("Uslgrup",0));
//  printf("%s\n",strsql);
  data.rk.kod.new_plus(strsql);
 }
*************/ 

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_uslopr_v_v_key_press),&data);

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
GtkWidget *separator2=gtk_hseparator_new();
GtkWidget *separator3=gtk_hseparator_new();
gtk_container_add (GTK_CONTAINER (vbox), separator1);
//Вставляем радиокнопки
GSList *group;

data.radiobutton1[0]=gtk_radio_button_new_with_label(NULL,gettext("Внешняя"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton1[0]), "clicked",GTK_SIGNAL_FUNC(uslopr_radio1_0),&data);
if(data.rk.vido == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton1[0]),TRUE); //Устанавливем активной кнопку
//gtk_object_set_user_data(GTK_OBJECT(data.radiobutton0),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton1[0]));

data.radiobutton1[1]=gtk_radio_button_new_with_label(group,gettext("Внутренняя"));
gtk_signal_connect(GTK_OBJECT(data.radiobutton1[1]), "clicked",GTK_SIGNAL_FUNC(uslopr_radio1_1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[1], TRUE, TRUE, 0);
if(data.rk.vido == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton1[1]),TRUE); //Устанавливем активной кнопку

/*************************/
gtk_container_add (GTK_CONTAINER (vbox), separator2);

data.radiobutton2[0]=gtk_radio_button_new_with_label(NULL,gettext("Проводки нужно делать"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton2[0]), "clicked",GTK_SIGNAL_FUNC(uslopr_radio2_0),&data);
if(data.rk.prov == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[0]),TRUE); //Устанавливем активной кнопку
//gtk_object_set_user_data(GTK_OBJECT(data.radiobutton0),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton2[0]));

data.radiobutton2[1]=gtk_radio_button_new_with_label(group,gettext("Проводки не нужно делать"));
gtk_signal_connect(GTK_OBJECT(data.radiobutton2[1]), "clicked",GTK_SIGNAL_FUNC(uslopr_radio2_1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[1], TRUE, TRUE, 0);
if(data.rk.prov == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[1]),TRUE); //Устанавливем активной кнопку

gtk_container_add (GTK_CONTAINER (vbox), separator3);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Код операции"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_KOD] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(l_uslopr_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

sprintf(strsql,"%s",gettext("Наименование операции"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_NAIM] = gtk_entry_new_with_max_length (59);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(l_uslopr_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);

sprintf(strsql,"%s",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(uslopr_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор единицы измерения"),NULL);

data.entry[E_SHET] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(l_uslopr_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk.shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

data.label_naishet=gtk_label_new(naim_shet.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.label_naishet,TRUE, TRUE, 0);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введённой в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_uslopr_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_uslopr_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

if(data.voz == 0)
 kodzap->new_plus(data.rk.kod.ravno());
return(data.voz);
}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  uslopr_radio1_0(GtkWidget *widget,class l_uslopr_v_data *data)
{
//g_print("uslopr_radio0\n");

data->rk.vido=0;
//printf("vido=0\n");
}
void  uslopr_radio1_1(GtkWidget *widget,class l_uslopr_v_data *data)
{
//g_print("uslopr_radio1\n");
data->rk.vido=1;
}

/**************************/

void  uslopr_radio2_0(GtkWidget *widget,class l_uslopr_v_data *data)
{
//g_print("uslopr_radio0\n");

data->rk.prov=0;
//printf("prov=0\n");

}
void  uslopr_radio2_1(GtkWidget *widget,class l_uslopr_v_data *data)
{
//g_print("uslopr_radio1\n");
data->rk.prov=1;
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  uslopr_v_e_knopka(GtkWidget *widget,class l_uslopr_v_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {

  case E_SHET:
    if(iceb_vibrek(1,"Plansh",&data->rk.shet,&naim,data->window) == 0)
       gtk_label_set_text(GTK_LABEL(data->label_naishet),naim.ravno_toutf(20));
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk.shet.ravno_toutf());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uslopr_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uslopr_v_data *data)
{

//printf("l_uslopr_v_v_key_press\n");
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
void  l_uslopr_v_v_knopka(GtkWidget *widget,class l_uslopr_v_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(uslopr_zap(data) != 0)
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

void    l_uslopr_v_v_vvod(GtkWidget *widget,class l_uslopr_v_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uslopr_v_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->rk.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NAIM:
    data->rk.naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHET:
    data->rk.shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    sprintf(strsql,"select nais from Plansh where ns='%s'",data->rk.shet.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naishet),iceb_u_toutf(strsql));
     }    
    else
      gtk_label_set_text(GTK_LABEL(data->label_naishet),"");
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
int uslopr_zap(class l_uslopr_v_data *data)
{
char strsql[1024];
//SQL_str row;
time_t   vrem;
time(&vrem);

printf("uslopr_v_zap vido=%d prov=%d\n",data->rk.vido,data->rk.prov);

data->read_rek(); //Читаем реквизиты меню
//for(int i=0; i < KOLENTER; i++)
//  gtk_signal_emit_by_name(GTK_OBJECT(data->entry[i]),"activate");

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
class OPSHET opsh;
if(data->rk.shet.getdlinna() > 1)
 if(iceb_prsh1(data->rk.shet.ravno(),&opsh,data->window) != 0)
  return(1);

if(iceb_u_SRAV(data->kodk.ravno(),data->rk.kod.ravno(),0) != 0)
 if(uslopr_pk(data->rk.kod.ravno(),data->window) != 0)
   return(1);
  
short metkakor=0;

if(data->kodk.getdlinna() <= 1)
 {

  
  sprintf(strsql,"insert into Usloper2 \
values ('%s','%s',%d,%ld,%d,%d,'%s')",
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.vido,
   data->rk.prov,
   data->rk.shet.ravno());
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
  sprintf(strsql,"update Usloper2 \
set \
kod='%s',\
naik='%s',\
ktoz=%d,\
vrem=%ld,\
vido=%d,\
prov=%d,\
shet='%s' \
where kod='%s'",
   data->rk.kod.ravno(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.vido,
   data->rk.prov,
   data->rk.shet.ravno(),
   data->kodk.ravno());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);

if(metkakor == 1)
 uslopr_kkvt(data->kodk.ravno(),data->rk.kod.ravno(),data->window);


return(0);

}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void uslopr_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{
char strsql[512];
sprintf(strsql,"update Usldokum set kodop='%s' where kodop='%s' and tp=2",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
  return;
 }

sprintf(strsql,"update Prov set oper='%s' where oper='%s' and tz=2 and kto='%s'",nkod,skod,gettext("УСЛ"));
iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Reenn set ko='%s' where ko='%s' and mi=2",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int uslopr_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Usloper2 where kod='%s'",kod);
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
