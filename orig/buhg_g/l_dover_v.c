/*$Id: l_dover_v.c,v 1.7 2011-02-21 07:35:52 sasa Exp $*/
/*22.04.2009	22.04.2009	Белых А.И.	l_dover_v.c
Ввод и корректировка записи в доверенность
*/
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"

enum
 {
  E_NAIM,
  E_EI,
  E_KOLIH,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_dover_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str naim;
  class iceb_u_str ei;
  class iceb_u_str kolih;
  
  class iceb_u_str datdov;
  class iceb_u_str nomdov;
  int nomerz;    

  l_dover_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear();    
   }

  void read_rek()
   {
    naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM]))));
    ei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_EI]))));
    kolih.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOLIH]))));
   }
  void clear()
   {
    naim.new_plus("");
    ei.new_plus("");
    kolih.new_plus("");
   }
 };


gboolean   l_dover_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_dover_v_data *data);
void  l_dover_v_knopka(GtkWidget *widget,class l_dover_v_data *data);
void    l_dover_v_vvod(GtkWidget *widget,class l_dover_v_data *data);
int l_dover_zap(class l_dover_v_data *data);

void  l_dover_e_knopka(GtkWidget *widget,class l_dover_v_data *data);

extern SQL_baza  bd;
extern char      *name_system;

int l_dover_v(const char *datadov,const char *nomdov,int nomerz,GtkWidget *wpredok)
{
class l_dover_v_data data;
char strsql[512];
class iceb_u_str kikz;

data.datdov.new_plus(datadov);
data.nomdov.new_plus(nomdov);
data.nomerz=nomerz;
printf("nomerz=%d\n",nomerz);
if(data.nomerz != 0)
 {
  sprintf(strsql,"select zapis,ei,kol,ktoz,vrem from Uddok1 where datd='%s' and nomd='%s' and nz=%d",
  data.datdov.ravno_sqldata(),data.nomdov.ravno(),data.nomerz);
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.naim.new_plus(row[0]);
  data.ei.new_plus(row[1]);
  data.kolih.new_plus(row[2]);
  kikz.plus(iceb_kikz(row[3],row[4],wpredok));
  
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.nomerz == 0)
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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_dover_v_key_press),&data);

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

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];


label=gtk_label_new(gettext("Наименование"));
data.entry[E_NAIM] = gtk_entry_new_with_max_length (199);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(l_dover_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);
/*******
label=gtk_label_new(gettext("Единица измерения"));
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), label, FALSE, FALSE, 0);
***********/

sprintf(strsql,"%s",gettext("Единица измерения"));
data.knopka_enter[E_EI]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.knopka_enter[E_EI], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_EI]),"clicked",GTK_SIGNAL_FUNC(l_dover_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_EI]),(gpointer)E_EI);
tooltips_enter[E_EI]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_EI],data.knopka_enter[E_EI],gettext("Выбор единицы измерения"),NULL);

data.entry[E_EI] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_EI]), "activate",GTK_SIGNAL_FUNC(l_dover_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.ei.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_EI]),(gpointer)E_EI);

label=gtk_label_new(gettext("Количество"));
data.entry[E_KOLIH] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), data.entry[E_KOLIH], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOLIH]), "activate",GTK_SIGNAL_FUNC(l_dover_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH]),data.kolih.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOLIH]),(gpointer)E_KOLIH);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_dover_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_dover_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

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
void  l_dover_e_knopka(GtkWidget *widget,class l_dover_v_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_zarnah_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  case E_EI:
    if(iceb_l_ei(1,&kod,&naim,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),kod.ravno_toutf());
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_dover_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_dover_v_data *data)
{
//char  bros[512];

//printf("l_dover_v_key_press\n");
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
void  l_dover_v_knopka(GtkWidget *widget,class l_dover_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_dover_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_dover_zap(data) == 0)
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

void    l_dover_v_vvod(GtkWidget *widget,class l_dover_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_dover_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_EI:
    data->ei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KOLIH:
    data->kolih.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NAIM:
    data->naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int l_dover_zap(class l_dover_v_data *data)
{
char strsql[1024];



if(data->naim.getdlinna() <= 1 || \
data->ei.getdlinna() <= 1 || \
data->kolih.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введены все обязательные реквизиты!"),data->window);
  return(1);
 } 

class iceb_lock_tables KKK("LOCK TABLES Uddok1 WRITE,icebuser READ");


if(data->nomerz == 0) /*Ввод новой записи*/
 {
  int nomzap=0;
  /*Узнаём свободный номер*/
  for(nomzap=1; nomzap < 10000; nomzap++)
   {
    sprintf(strsql,"select nz from Uddok1 where datd='%s' and nomd='%s' and nz=%d",
    data->datdov.ravno_sqldata(),data->nomdov.ravno(),nomzap);
    if(iceb_sql_readkey(strsql,data->window) == 0)
     break;
   }


  sprintf(strsql,"insert into Uddok1 values('%s','%s',%d,'%s','%s',%10.10g,%d,%ld)",
  data->datdov.ravno_sqldata(),data->nomdov.ravno(),nomzap,
  data->naim.ravno_filtr(),  
  data->ei.ravno_filtr(),  
  data->kolih.ravno_atof(),  
  iceb_getuid(data->window),
  time(NULL));
 }
else /*Корректировка существующей*/
 {
  sprintf(strsql,"update Uddok1 set \
zapis='%s',\
ei='%s',\
kol=%10.10g,\
ktoz=%d,\
vrem=%ld \
where datd='%s' and nomd='%s' and nz=%d",
  data->naim.ravno_filtr(),  
  data->ei.ravno_filtr(),  
  data->kolih.ravno_atof(),  
  iceb_getuid(data->window),
  time(NULL),
  data->datdov.ravno_sqldata(),data->nomdov.ravno(),data->nomerz);

 }
printf("strsql=%s\n",strsql);

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


return(0);

}
