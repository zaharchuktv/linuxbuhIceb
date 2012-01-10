/*$Id: l_upldok_vzp.c,v 1.10 2011-02-21 07:35:54 sasa Exp $*/
/*24.03.2008	14.03.2008	Белых А.И.	l_upldok_vzp.c
Ввод топлива на приход
*/
#include <stdlib.h>
#include "buhg_g.h"
#include <unistd.h>
enum
 {
  E_KOD_POST,
  E_KOD_TOP,
  E_KOLIH,
  E_KOMENT,
  E_KOD_AVT,
  KOLENTER  
 };

enum
 {
  FK2,
  FK10,
  KOL_FK
 };


class l_upldok_vzp_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_post;
  GtkWidget *label_avt;
  GtkWidget *hbox_avt;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str data_dok;
  class iceb_u_str kod_pod;
  class iceb_u_str nomdok;
  int nomzap;
  /*реквизиты шапки документа нужные для работы*/
  class iceb_u_str kod_vodh;
  class iceb_u_str kod_avth;
  int nom_zap_hap;  /*номер записи шапки документа*/

  short metka_r;  

  class iceb_u_str kod_post;
  class iceb_u_str kod_top;
  class iceb_u_str kolih;
  class iceb_u_str koment;
  class iceb_u_str kod_avt;
  short metka_vod_zap; /*0-заправка 1-водитель*/
  
  l_upldok_vzp_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear();
    nom_zap_hap=0;
   }

  void read_rek()
   {
    kod_post.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_POST]))));
    kod_top.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_TOP]))));
    kolih.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOLIH]))));
    koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT]))));
    kod_avt.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_AVT]))));
  
   }

  void clear()
   {
    kod_post.new_plus("");
    kod_top.new_plus("");
    kolih.new_plus("");
    koment.new_plus("");
    kod_avt.new_plus("");
    metka_vod_zap=0;
   }

 };


gboolean   l_upldok_vzp_key_press(GtkWidget *widget,GdkEventKey *event,class l_upldok_vzp_data *data);
void  l_upldok_vzp_knopka(GtkWidget *widget,class l_upldok_vzp_data *data);
void    l_upldok_vzp_vvod(GtkWidget *widget,class l_upldok_vzp_data *data);
int l_upldok_vzp_zap(class l_upldok_vzp_data *data);

void  l_upldok_vzp_e_knopka(GtkWidget *widget,class l_upldok_vzp_data *data);
void l_upldok_vzp_avt(class l_upldok_vzp_data *data);
void l_upldok_vzp_post(class l_upldok_vzp_data *data);
double vvodtop_ost(short dd,short md,short gd,const char *kodvod,const char *kodtop,const char *kodavt,GtkWidget *wpredok);


extern SQL_baza  bd;
extern char      *name_system;
extern short startgodupl; //Стартовый год для "Учета путевых листов"

int l_upldok_vzp(const char *data_dok,
int kod_pod,
const char *nomdok,
int nomzap,
int metka_r, /*0-ввод новой записи 1-корректировка*/
GtkWidget *wpredok)
{
class iceb_u_str naim_post("");
class iceb_u_str naim_avt("");
class l_upldok_vzp_data data;
char strsql[512];
iceb_u_str kikz;
data.data_dok.new_plus(data_dok);
data.kod_pod.new_plus(kod_pod);
data.nomdok.new_plus(nomdok);
data.nomzap=nomzap;
data.metka_r=metka_r;

sprintf(strsql,"select ka,kv,nz from Upldok where god=%d and kp=%d and nomd='%s'",
data.data_dok.ravno_god(),data.kod_pod.ravno_atoi(),data.nomdok.ravno());

SQL_str row;
SQLCURSOR cur;
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
 {
  iceb_menu_soob(gettext("Не найдена шапка документа!"),wpredok);
  return(1);
 }

data.kod_avth.new_plus(row[0]);
data.kod_vodh.new_plus(row[1]);
data.nom_zap_hap=atoi(row[2]);

if(data.metka_r == 1)
 {
  sprintf(strsql,"select * from Upldok1 where datd='%s' and kp=%d and nomd='%s' and nzap=%d",
  data.data_dok.ravno_sqldata(),data.kod_pod.ravno_atoi(),data.nomdok.ravno(),data.nomzap);
  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  data.kod_post.new_plus(row[6]);
  data.kod_top.new_plus(row[8]);
  data.kolih.new_plus(row[9]);
  data.koment.new_plus(row[10]);
  data.kod_avt.new_plus(row[4]);
  
  kikz.plus(iceb_kikz(row[12],row[13],wpredok));

  sprintf(strsql,"select mt,naik from Uplouot where kod=%d",data.kod_post.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    data.metka_vod_zap=atoi(row[0]);
    naim_post.new_plus(row[1]);
   }
  if(data.metka_vod_zap == 1)
   {
    sprintf(strsql,"select nomz,naik from Uplavt where kod=%d",data.kod_avt.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     {
      naim_avt.new_plus(row[0]);
      naim_avt.new_plus(" ");
      naim_avt.new_plus(row[1]);
     }
   }  

 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.metka_r == 0)
 {
  sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));
  label=gtk_label_new(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",name_system,gettext("Корректировка выбранной записи"));
  iceb_u_str repl;
  repl.plus(gettext("Корректировка выбранной записи"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno_toutf());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_upldok_vzp_key_press),&data);

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
data.hbox_avt = gtk_hbox_new(FALSE,1);
for(int i=0; i < KOLENTER-1; i++)
 hbox[i] = gtk_hbox_new (FALSE,1);
data.hbox_avt=gtk_hbox_new (FALSE,1);
GtkWidget *hboxknop = gtk_hbox_new (FALSE,1);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER-1; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], FALSE, FALSE, 1);


gtk_box_pack_start (GTK_BOX (vbox), data.hbox_avt, FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (vbox), hboxknop, FALSE, FALSE, 1);



GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Код поставщика топлива"));
data.knopka_enter[E_KOD_POST]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_POST]), data.knopka_enter[E_KOD_POST], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_POST]),"clicked",GTK_SIGNAL_FUNC(l_upldok_vzp_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_POST]),(gpointer)E_KOD_POST);
tooltips_enter[E_KOD_POST]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_POST],data.knopka_enter[E_KOD_POST],gettext("Выбор поставщика"),NULL);

data.entry[E_KOD_POST] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_POST]), data.entry[E_KOD_POST], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_POST]), "activate",GTK_SIGNAL_FUNC(l_upldok_vzp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_POST]),data.kod_post.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_POST]),(gpointer)E_KOD_POST);

data.label_post=gtk_label_new(naim_post.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_POST]), data.label_post, FALSE, FALSE,1);


sprintf(strsql,"%s",gettext("Код топлива"));
data.knopka_enter[E_KOD_TOP]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TOP]), data.knopka_enter[E_KOD_TOP], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_TOP]),"clicked",GTK_SIGNAL_FUNC(l_upldok_vzp_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_TOP]),(gpointer)E_KOD_TOP);
tooltips_enter[E_KOD_TOP]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_TOP],data.knopka_enter[E_KOD_TOP],gettext("Выбор топлива"),NULL);

data.entry[E_KOD_TOP] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TOP]), data.entry[E_KOD_TOP], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_TOP]), "activate",GTK_SIGNAL_FUNC(l_upldok_vzp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_TOP]),data.kod_top.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_TOP]),(gpointer)E_KOD_TOP);


label=gtk_label_new(gettext("Количество"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), label, FALSE, FALSE,1);

data.entry[E_KOLIH] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), data.entry[E_KOLIH], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOLIH]), "activate",GTK_SIGNAL_FUNC(l_upldok_vzp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH]),data.kolih.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOLIH]),(gpointer)E_KOLIH);


label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE,1);

data.entry[E_KOMENT] = gtk_entry_new_with_max_length (79);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(l_upldok_vzp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);


sprintf(strsql,"%s",gettext("Код автомобиля"));
data.knopka_enter[E_KOD_AVT]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (data.hbox_avt), data.knopka_enter[E_KOD_AVT], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_AVT]),"clicked",GTK_SIGNAL_FUNC(l_upldok_vzp_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_AVT]),(gpointer)E_KOD_AVT);
tooltips_enter[E_KOD_AVT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_AVT],data.knopka_enter[E_KOD_AVT],gettext("Выбор автомобиля"),NULL);

data.entry[E_KOD_AVT] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (data.hbox_avt), data.entry[E_KOD_AVT], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_AVT]), "activate",GTK_SIGNAL_FUNC(l_upldok_vzp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_AVT]),data.kod_avt.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_AVT]),(gpointer)E_KOD_AVT);

data.label_avt=gtk_label_new(naim_avt.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (data.hbox_avt), data.label_avt, FALSE, FALSE,1);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_upldok_vzp_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_upldok_vzp_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

if(data.metka_vod_zap == 0)
  gtk_widget_hide(data.hbox_avt);
 
gtk_main();

 
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_upldok_vzp_e_knopka(GtkWidget *widget,class l_upldok_vzp_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {

  case E_KOD_POST:
    if(l_uplout(1,&kod,&naim,0,data->window) == 0)
     {
      data->kod_post.new_plus(kod.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_post),naim.ravno_toutf(20));
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_POST]),data->kod_post.ravno());
    
    l_upldok_vzp_post(data);
      
    return;  

  case E_KOD_TOP:
    if(l_uplmt(1,&kod,&naim,data->window) == 0)
      data->kod_top.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_TOP]),data->kod_top.ravno_toutf());
      
    return;  

  case E_KOD_AVT:
    if(l_uplavt(1,&kod,&naim,data->window) == 0)
      data->kod_avt.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_AVT]),data->kod_avt.ravno());
    
    l_upldok_vzp_avt(data);
      
    return;  
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_upldok_vzp_key_press(GtkWidget *widget,GdkEventKey *event,class l_upldok_vzp_data *data)
{
//char  bros[512];

//printf("l_upldok_vzp_key_press\n");
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
void  l_upldok_vzp_knopka(GtkWidget *widget,class l_upldok_vzp_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_upldok_vzp_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_upldok_vzp_zap(data) == 0)
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

void    l_upldok_vzp_vvod(GtkWidget *widget,class l_upldok_vzp_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_upldok_vzp_vvod enter=%d\n",enter);
switch (enter)
 {
  case E_KOD_POST:
    data->kod_post.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    l_upldok_vzp_post(data);
    break;

  case E_KOD_AVT:
    data->kod_avt.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    l_upldok_vzp_avt(data);
    break;

    

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/**********************************/
/*установки после чтения кода автомобиля*/
/****************************************/

void l_upldok_vzp_avt(class l_upldok_vzp_data *data)
{
char strsql[512];
class iceb_u_str naim("");
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select * from Uplavt where kod=%d",data->kod_avt.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  naim.new_plus(row[1]);
  naim.plus(" ");
  naim.plus(row[5]);

 }

gtk_label_set_text(GTK_LABEL(data->label_avt),naim.ravno_toutf(20));

}
/**************************/
/*установки после чтения кода поставщика*/
/**************************************/

void l_upldok_vzp_post(class l_upldok_vzp_data *data)
{

char strsql[512];
class iceb_u_str naim("");
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select mt,naik from Uplouot where kod=%d",data->kod_post.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  data->metka_vod_zap=atoi(row[0]);
  naim.new_plus(row[1]);

 }

gtk_label_set_text(GTK_LABEL(data->label_post),naim.ravno_toutf(20));

if(data->metka_vod_zap == 0)
  gtk_widget_hide(data->hbox_avt);
else
  gtk_widget_show_all (data->hbox_avt);

}
/****************************/
/*запись*/
/******************************/
int l_upldok_vzp_zap(class l_upldok_vzp_data *data)
{
if(l_upldok_pb(data->data_dok.ravno(),data->window) != 0)
 return(1);

if(data->kod_post.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели код поставщика!"),data->window);
  return(1);
 }

if(data->kolih.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели количество!"),data->window);
  return(1);
 }

if(data->kod_top.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели код топлива!"),data->window);
  return(1);
 }

if(data->kod_vodh.ravno_atoi() == data->kod_post.ravno_atoi())
if(data->kod_avth.ravno_atoi() == data->kod_avt.ravno_atoi())
 {
  iceb_menu_soob(gettext("Передавать топливо самоту себе нет смысла!"),data->window);
  return(1);
 }

char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select mt,naik from Uplouot where kod=%d",data->kod_post.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не найден код поставщика"),data->kod_post.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
short dd=0,md=0,gd=0;

iceb_u_rsdat(&dd,&md,&gd,data->data_dok.ravno(),1);

data->metka_vod_zap=atoi(row[0]);
if(data->metka_vod_zap == 1 ) //Водитель
 {
  if(data->kod_avt.getdlinna() <= 1)
   {
    iceb_menu_soob(gettext("Не ввели код автомобиля!"),data->window);
    return(1);
   }
  //Проверяем код автомобиля 
  sprintf(strsql,"select kod from Uplavt where kod=%d",data->kod_avt.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s %d!",gettext("Не найден код автомобиля"),data->kod_avt.ravno_atoi());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
  double ostatok=vvodtop_ost(dd,md,gd,data->kod_post.ravno(),data->kod_top.ravno(),data->kod_avt.ravno(),data->window);
  if(ostatok-data->kolih.ravno_atof() < -0.009)
   {
    sprintf(strsql,"%s\n%s:%.3f",
    gettext("Нет на остатке столько топлива у этого водителя на этом автомобиле!"),
    gettext("Остаток"),
    ostatok);
    
    iceb_menu_soob(strsql,data->window);
   }
 }

sprintf(strsql,"select kodt from Uplmt where kodt='%s'",data->kod_top.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден код топлива"),data->kod_top.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }



    
if(data->metka_r == 0)
 {
  if(upl_zapvdokw(dd,md,gd,data->kod_pod.ravno_atoi(),data->nomdok.ravno(),data->kod_avth.ravno_atoi(),data->kod_vodh.ravno_atoi(),
  data->kod_post.ravno_atoi(),
  1,
  data->kod_top.ravno(),
  data->kolih.ravno_atof(),
  data->koment.ravno_filtr(),
  data->kod_avt.ravno_atoi(),
  data->nom_zap_hap,data->window) != 0)
    return(1);
 }
else
  {
   sprintf(strsql,"update Upldok1 set \
kodsp=%d,\
tz=1,\
kodtp='%s',\
kolih=%.10g,\
kom='%s',\
kap=%d,\
ktoz=%d,\
vrem=%ld \
where datd='%d-%d-%d' and kp=%d and nomd='%s' and nzap=%d",
   data->kod_post.ravno_atoi(),
   data->kod_top.ravno(),
   data->kolih.ravno_atof(),
   data->koment.ravno_filtr(),
   data->kod_avt.ravno_atoi(),
   iceb_getuid(data->window),time(NULL),
   gd,md,dd,data->kod_pod.ravno_atoi(),data->nomdok.ravno(),data->nomzap);

   iceb_sql_zapis(strsql,1,0,data->window);

 }

return(0);
}

/****************************/
/*Получение остатка топлива по нужной марке топлива водителю и автомобилю*/
/***********************************************************/

double vvodtop_ost(short dd,short md,short gd,
const char *kodvod,const char *kodtop,const char *kodavt,
GtkWidget *wpredok)
{
char strsql[512];
int kolstr;
short gn=startgodupl;

sprintf(strsql,"select sal from Uplsal where god=%d and ka=%s and kv=%s and kmt='%s' ",
gn,kodavt,kodvod,kodtop);

SQLCURSOR cur;
SQL_str row;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }
double kolih=0.;
while(cur.read_cursor(&row) != 0)
 {
  kolih+=atof(row[0]);
 }

sprintf(strsql,"select tz,kolih from Upldok1 where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d' and ka=%s and kv=%s and kap=0 order by datd,nomd asc",
gn,1,1,gd,md,dd,kodavt,kodvod);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s\n",row[0],row[1]);
  if(atoi(row[0]) == 2)
    kolih+=atof(row[1])*-1;
  else
     kolih+=atof(row[1]);
 }

//Смотрим передачи топлива другим водителям
sprintf(strsql,"select kolih from Upldok1 where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d' and kap=%s and kodsp=%s and tz=1 and kodtp='%s' order by datd,nomd asc",
gn,1,1,gd,md,dd,kodavt,kodvod,kodtop);

//printw("\nostva-%s\n",strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  
  kolih-=atof(row[0]);
  
 }


kolih=iceb_u_okrug(kolih,0.001);
return(kolih);

}
