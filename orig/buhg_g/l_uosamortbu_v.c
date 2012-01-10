/*$Id: l_uosamortbu_v.c,v 1.10 2011-02-21 07:35:53 sasa Exp $*/
/*11.12.2007	11.12.2007	Белых А.И.	l_uosamortbu_v.c
Ввод и коррертировка амортизационных отчислений для налогового учёта
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"

enum
 {
  E_DATA,
  E_SUMA,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK10,
  KOL_PFK
 };

class l_uosamortbu_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  int innom;
  int podr;
  class iceb_u_str hzt;
  class iceb_u_str hna;
  short mes;
  short god;
    
  class iceb_u_str mesgod;
  class iceb_u_str sao; /*Cумма амортотчисления*/
  
  l_uosamortbu_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    sao.plus("");
    mesgod.plus("");
   }

  void read_rek()
   {
    mesgod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA]))));
    sao.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SUMA]))));
   }
 };


gboolean   l_uosamortbu_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosamortbu_v_data *data);
void  l_uosamortbu_v_knopka(GtkWidget *widget,class l_uosamortbu_v_data *data);
void    l_uosamortbu_v_vvod(GtkWidget *widget,class l_uosamortbu_v_data *data);
int l_uosamortbu_pk(char *god,GtkWidget*);
int l_uosamortbu_zap(class l_uosamortbu_v_data *data);
void l_uosamortbu_kkvt(char *kods,char *kodn,GtkWidget *wpredok);

void  l_uosamortbu_v_e_knopka(GtkWidget *widget,class l_uosamortbu_v_data *data);

extern SQL_baza  bd;
extern char      *name_system;

int l_uosamortbu_v(int innom,int podr,const char *mes_god,const char *hzt,const char *hna,GtkWidget *wpredok)
{

class l_uosamortbu_v_data data;
char strsql[512];
class iceb_u_str kikz;

data.innom=innom;
iceb_u_rsdat1(&data.mes,&data.god,mes_god);
data.mesgod.new_plus(mes_god);
data.hzt.new_plus(hzt);
data.hna.new_plus(hna);
data.podr=podr;

if(data.mes != 0)
 {
  sprintf(strsql,"select suma,ktoz,vrem from Uosamor1 where innom=%d and god=%d and mes=%d",innom,data.god,data.mes);
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  sprintf(strsql,"%.2f",atof(row[0]));
  data.sao.new_plus(strsql);
  kikz.plus(iceb_kikz(row[1],row[2],wpredok));
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.mes == 0)
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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_uosamortbu_v_key_press),&data);

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


sprintf(strsql,"%s (%s)",gettext("Дата"),gettext("м.г"));
/********
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), label, FALSE, FALSE,1);
************/
data.knopka_enter[E_DATA]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.knopka_enter[E_DATA], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA]),"clicked",GTK_SIGNAL_FUNC(l_uosamortbu_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA]),(gpointer)E_DATA);
GtkTooltips *tooltips_enter=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter,data.knopka_enter[E_DATA],gettext("Выбор даты"),NULL);

data.entry[E_DATA] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.entry[E_DATA], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA]), "activate",GTK_SIGNAL_FUNC(l_uosamortbu_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA]),data.mesgod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA]),(gpointer)E_DATA);

label=gtk_label_new(gettext("Сумма"));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), label, FALSE, FALSE,1);

data.entry[E_SUMA] = gtk_entry_new_with_max_length (60);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMA]), "activate",GTK_SIGNAL_FUNC(l_uosamortbu_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.sao.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMA]),(gpointer)E_SUMA);


GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],data.knopka[PFK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK2]),"clicked",GTK_SIGNAL_FUNC(l_uosamortbu_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK2]),(gpointer)PFK2);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],data.knopka[PFK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK10]),"clicked",GTK_SIGNAL_FUNC(l_uosamortbu_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK10]),(gpointer)PFK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE,1);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}

/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_uosamortbu_v_e_knopka(GtkWidget *widget,class l_uosamortbu_v_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case E_DATA:

    if(iceb_calendar1(&data->mesgod,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA]),data->mesgod.ravno());
      
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uosamortbu_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosamortbu_v_data *data)
{

//printf("l_uosamortbu_v_key_press\n");
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
void  l_uosamortbu_v_knopka(GtkWidget *widget,class l_uosamortbu_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uosamortbu_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case PFK2:
    data->read_rek();
    if(l_uosamortbu_zap(data) == 0)
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

void    l_uosamortbu_v_vvod(GtkWidget *widget,class l_uosamortbu_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uosamortbu_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATA:
    data->mesgod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_SUMA:
    data->sao.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/*****************************/
/*Запись*/
/***************************/
int l_uosamortbu_zap(class l_uosamortbu_v_data *data)
{
char strsql[512];

if(data->mesgod.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена дата!"),data->window); 
  return(1);
 }

short m1,g1;

if(iceb_u_rsdat1(&m1,&g1,data->mesgod.ravno()) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата!"),data->window); 
  return(1);
 }



if(iceb_pbpds(m1,g1,data->window) != 0)
  return(1);

if(data->mes != 0)
 {
  if(iceb_pbpds(data->mes,data->god,data->window) != 0)
    return(1);

 }

if(data->sao.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена сумма аморт-отчислений!"),data->window);
  return(1);
 }


if(data->mes == 0)
 sprintf(strsql,"insert into Uosamor1 \
values (%d,%d,%d,%d,'%s','%s',%.2f,%d,%ld)",
 data->innom,m1,g1,data->podr,data->hzt.ravno(),data->hna.ravno(),data->sao.ravno_atof(),iceb_getuid(data->window),time(NULL));
else
 sprintf(strsql,"update Uosamor1 \
set \
god=%d,\
mes=%d,\
podr=%d,\
hzt='%s',\
hna='%s',\
suma=%.2f,\
ktoz=%d,\
vrem=%ld \
where innom=%d and god=%d and mes=%d",
  g1,m1,data->podr,data->hzt.ravno(),data->hna.ravno(),data->sao.ravno_atof(),iceb_getuid(data->window),time(NULL),data->innom,data->god,data->mes);


if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
 return(1);


return(0);
}
