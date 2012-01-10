/*$Id: l_uplavt_v.c,v 1.11 2011-02-21 07:35:54 sasa Exp $*/
/*06.03.2008	06.03.2008	Белых А.И.	l_uplavt_v.c
Ввод и корректировка автомобилей
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
#include "l_uplavt.h"
enum
 {
  E_KOD,
  E_NAIM,
  E_GOS_NOMER,
  E_LNZ_G,
  E_ZNZ_G,
  E_LNZ_ZG,
  E_ZNZ_ZG,
  E_NZ_GRUZ,
  E_NZ_OV,
  E_KOD_VOD,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_uplavt_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *label_kod_vod;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_uplavt_rek rk;
  class iceb_u_str kod_avt;  
  
  l_uplavt_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD]))));
    rk.naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM]))));
    rk.gos_nomer.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_GOS_NOMER]))));
    rk.lnz_g.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_LNZ_G]))));
    rk.znz_g.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_ZNZ_G]))));
    rk.lnz_zg.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_LNZ_ZG]))));
    rk.znz_zg.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_ZNZ_ZG]))));
    rk.nz_gruz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NZ_GRUZ]))));
    rk.nz_ov.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NZ_OV]))));
    rk.kod_vod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_VOD]))));
   }
 };


gboolean   l_uplavt_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplavt_v_data *data);
void  l_uplavt_v_knopka(GtkWidget *widget,class l_uplavt_v_data *data);
void    l_uplavt_v_vvod(GtkWidget *widget,class l_uplavt_v_data *data);
int l_uplavt_pk(const char *god,GtkWidget*);
int l_uplavt_zap(class l_uplavt_v_data *data);
void l_uplavt_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);

void  l_uplavt_v_e_knopka(GtkWidget *widget,l_uplavt_v_data *data);

extern SQL_baza  bd;
extern char      *name_system;

int l_uplavt_v(class iceb_u_str *kod_avt,GtkWidget *wpredok)
{
class iceb_u_str naim_kod_vod("");
class l_uplavt_v_data data;
char strsql[512];
iceb_u_str kikz;
data.kod_avt.new_plus(kod_avt->ravno());

data.rk.clear_data();

if(data.kod_avt.getdlinna() >  1)
 {
  data.rk.kod.new_plus(data.kod_avt.ravno());
  sprintf(strsql,"select * from Uplavt where kod=%d",data.kod_avt.ravno_atoi());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  data.rk.gos_nomer.new_plus(row[1]);
  data.rk.lnz_g.new_plus(row[2]);
  data.rk.znz_g.new_plus(row[3]);
  if(row[4][0] != '0')
    data.rk.kod_vod.new_plus(row[4]);
  data.rk.naim.new_plus(row[5]);
  kikz.plus(iceb_kikz(row[6],row[7],wpredok));
  data.rk.lnz_zg.new_plus(row[8]);
  data.rk.znz_zg.new_plus(row[9]);
  data.rk.nz_gruz.new_plus(row[10]);
  data.rk.nz_ov.new_plus(row[11]);  
  /*узнаём фамилию водителя*/
  sprintf(strsql,"select naik from Uplouot where kod=%d",data.rk.kod_vod.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_kod_vod.new_plus(row[0]);    
 }
else
 {
  sprintf(strsql,"%d",iceb_get_new_kod("Uplavt",0,wpredok));
  data.rk.kod.new_plus(strsql);
 } 
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.kod_avt.getdlinna() <= 1)
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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_uplavt_v_key_press),&data);

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
GtkWidget *hboxknop = gtk_hbox_new (FALSE,1);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


label=gtk_label_new(gettext("Код"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE,1);

data.entry[E_KOD] = gtk_entry_new_with_max_length (4);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(l_uplavt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

label=gtk_label_new(gettext("Наименование"));
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE,1);

data.entry[E_NAIM] = gtk_entry_new_with_max_length (100);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(l_uplavt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);

label=gtk_label_new(gettext("Государственный номерной знак автотранспортного средства"));
gtk_box_pack_start (GTK_BOX (hbox[E_GOS_NOMER]), label, FALSE, FALSE,1);

data.entry[E_GOS_NOMER] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_GOS_NOMER]), data.entry[E_GOS_NOMER], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GOS_NOMER]), "activate",GTK_SIGNAL_FUNC(l_uplavt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOS_NOMER]),data.rk.gos_nomer.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GOS_NOMER]),(gpointer)E_GOS_NOMER);


label=gtk_label_new(gettext("Летняя норма расхода в городе"));
gtk_box_pack_start (GTK_BOX (hbox[E_LNZ_G]), label, FALSE, FALSE,1);

data.entry[E_LNZ_G] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_LNZ_G]), data.entry[E_LNZ_G], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_LNZ_G]), "activate",GTK_SIGNAL_FUNC(l_uplavt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_LNZ_G]),data.rk.lnz_g.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_LNZ_G]),(gpointer)E_LNZ_G);


label=gtk_label_new(gettext("Зимняя норма расхода в городе"));
gtk_box_pack_start (GTK_BOX (hbox[E_ZNZ_G]), label, FALSE, FALSE,1);

data.entry[E_ZNZ_G] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_ZNZ_G]), data.entry[E_ZNZ_G], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ZNZ_G]), "activate",GTK_SIGNAL_FUNC(l_uplavt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZNZ_G]),data.rk.znz_g.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ZNZ_G]),(gpointer)E_ZNZ_G);


label=gtk_label_new(gettext("Летняя норма расхода за городом"));
gtk_box_pack_start (GTK_BOX (hbox[E_LNZ_ZG]), label, FALSE, FALSE,1);

data.entry[E_LNZ_ZG] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_LNZ_ZG]), data.entry[E_LNZ_ZG], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_LNZ_ZG]), "activate",GTK_SIGNAL_FUNC(l_uplavt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_LNZ_ZG]),data.rk.lnz_zg.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_LNZ_ZG]),(gpointer)E_LNZ_ZG);


label=gtk_label_new(gettext("Зимняя норма расхода за городом"));
gtk_box_pack_start (GTK_BOX (hbox[E_ZNZ_ZG]), label, FALSE, FALSE,1);

data.entry[E_ZNZ_ZG] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_ZNZ_ZG]), data.entry[E_ZNZ_ZG], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ZNZ_ZG]), "activate",GTK_SIGNAL_FUNC(l_uplavt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZNZ_ZG]),data.rk.znz_zg.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ZNZ_ZG]),(gpointer)E_ZNZ_ZG);


label=gtk_label_new(gettext("Норма расхода на перевоз груза"));
gtk_box_pack_start (GTK_BOX (hbox[E_NZ_GRUZ]), label, FALSE, FALSE,1);

data.entry[E_NZ_GRUZ] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_NZ_GRUZ]), data.entry[E_NZ_GRUZ], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NZ_GRUZ]), "activate",GTK_SIGNAL_FUNC(l_uplavt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NZ_GRUZ]),data.rk.nz_gruz.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NZ_GRUZ]),(gpointer)E_NZ_GRUZ);

label=gtk_label_new(gettext("Норма расхода на отработанное время"));
gtk_box_pack_start (GTK_BOX (hbox[E_NZ_OV]), label, FALSE, FALSE,1);

data.entry[E_NZ_OV] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_NZ_OV]), data.entry[E_NZ_OV], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NZ_OV]), "activate",GTK_SIGNAL_FUNC(l_uplavt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NZ_OV]),data.rk.nz_ov.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NZ_OV]),(gpointer)E_NZ_OV);


//label=gtk_label_new(gettext("Код водителя"));
//gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), label, FALSE, FALSE,1);

sprintf(strsql,"%s",gettext("Код водителя"));
data.knopka_enter[E_KOD_VOD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.knopka_enter[E_KOD_VOD], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_VOD]),"clicked",GTK_SIGNAL_FUNC(l_uplavt_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_VOD]),(gpointer)E_KOD_VOD);
GtkTooltips *tooltips_kod_vod;
tooltips_kod_vod=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_kod_vod,data.knopka_enter[E_KOD_VOD],gettext("Выбор водителя"),NULL);

data.entry[E_KOD_VOD] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.entry[E_KOD_VOD], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_VOD]), "activate",GTK_SIGNAL_FUNC(l_uplavt_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_VOD]),data.rk.kod_vod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_VOD]),(gpointer)E_KOD_VOD);

data.label_kod_vod=gtk_label_new(naim_kod_vod.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.label_kod_vod, TRUE, TRUE,1);


GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_uplavt_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_uplavt_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 kod_avt->new_plus(data.rk.kod.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_uplavt_v_e_knopka(GtkWidget *widget,l_uplavt_v_data *data)
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

  case E_KOD_VOD:

    if(l_uplout(1,&kod,&naim,1,data->window) == 0)
     data->rk.kod_vod.new_plus(kod.ravno());
    
    /*Читаем наименование склада*/
    sprintf(strsql,"select naik from Uplouot where kod=%d",data->rk.kod_vod.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
      naim.new_plus(row[0]);
    gtk_label_set_text(GTK_LABEL(data->label_kod_vod),naim.ravno_toutf(20));
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_VOD]),data->rk.kod_vod.ravno_toutf());

    return;  



 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uplavt_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplavt_v_data *data)
{
//char  bros[512];

//printf("l_uplavt_v_key_press\n");
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
void  l_uplavt_v_knopka(GtkWidget *widget,class l_uplavt_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uplavt_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_uplavt_zap(data) == 0)
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

void    l_uplavt_v_vvod(GtkWidget *widget,class l_uplavt_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uplavt_v_vvod enter=%d\n",enter);

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
int l_uplavt_zap(class l_uplavt_v_data *data)
{
char strsql[1024];
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

if(data->rk.kod_vod.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Uplouot where kod=%d and mt=1",data->rk.kod_vod.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {

    sprintf(strsql,"%s %s !",gettext("Не найден код водителя"),
    data->rk.kod_vod.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 
 }

short metkakor=0;

if(iceb_u_SRAV(data->kod_avt.ravno(),data->rk.kod.ravno(),0) != 0)
  if(l_uplavt_pk(data->rk.kod.ravno(),data->window) != 0)
     return(1);

if(data->kod_avt.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Uplavt \
values (%d,'%s',%.3f,%.3f,%d,'%s',%d,%ld,%.3f,%.3f,%.3f,%.3f)",
   data->rk.kod.ravno_atoi(),
   data->rk.gos_nomer.ravno_filtr(),
   data->rk.lnz_g.ravno_atof(),
   data->rk.znz_g.ravno_atof(),
   data->rk.kod_vod.ravno_atoi(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.lnz_zg.ravno_atof(),
   data->rk.znz_zg.ravno_atof(),
   data->rk.nz_gruz.ravno_atof(),
   data->rk.nz_ov.ravno_atof());
 }
else
 {
  
  if(iceb_u_SRAV(data->kod_avt.ravno(),data->rk.kod.ravno(),0) != 0)
   {

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Uplavt \
set \
kod=%d,\
nomz='%s',\
lnr=%.3f,\
znr=%.3f,\
kzv=%d,\
naik='%s',\
ktoz=%d,\
vrem=%ld,\
lnrzg=%.3f,\
znrzg=%.3f,\
nrtk=%.3f,\
nrmh=%.3f \
where kod=%d",
   data->rk.kod.ravno_atoi(),
   data->rk.gos_nomer.ravno_filtr(),
   data->rk.lnz_g.ravno_atof(),
   data->rk.znz_g.ravno_atof(),
   data->rk.kod_vod.ravno_atoi(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.lnz_zg.ravno_atof(),
   data->rk.znz_zg.ravno_atof(),
   data->rk.nz_gruz.ravno_atof(),
   data->rk.nz_ov.ravno_atof(),
   data->kod_avt.ravno_atoi());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);

if(metkakor == 1)
 l_uplavt_kkvt(data->kod_avt.ravno(),data->rk.kod.ravno(),data->window);

return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_uplavt_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Uplavt where kod=%d",atoi(kod));
//printf("l_uplavt_pk-%s\n",strsql);

if(iceb_sql_readkey(strsql,wpredok) > 0)
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

void l_uplavt_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Upldok set ka='%s' where ka=%s",nkod,skod);

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
sprintf(strsql,"update Uplsal set ka='%s' where ka=%s",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

}
