/*$Id: l_uosls_v.c,v 1.1 2011-03-28 06:55:46 sasa Exp $*/
/*16.09.2010	21.06.2006	Белых А.И.	l_uosls_v.c
Ввод и корректировка ликвидационной стоимости
*/
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"

enum
 {
  E_DATAZAP,
  E_LIK_ST_NU,
  E_LIK_ST_BU,
  E_KOMENT,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_uosls_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  int innom;
  class iceb_u_str datazap_z;

  class iceb_u_str datazap;  
  class iceb_u_str lik_st_nu;
  class iceb_u_str lik_st_bu;  
  class iceb_u_str koment;
  
  l_uosls_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_dat();
   }
  void clear_dat()
   {
    datazap.new_plus("");
    lik_st_nu.new_plus("");
    lik_st_bu.new_plus("");
    koment.new_plus("");
   }
  void read_rek()
   {
    datazap.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAZAP]))));
    lik_st_nu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_LIK_ST_NU]))));
    lik_st_bu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_LIK_ST_BU]))));
    koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT]))));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    
   }
 };


gboolean   l_uosls_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosls_v_data *data);
void  l_uosls_v_knopka(GtkWidget *widget,class l_uosls_v_data *data);
void    l_uosls_v_vvod(GtkWidget *widget,class l_uosls_v_data *data);
int l_uosls_zap(class l_uosls_v_data *data);


extern SQL_baza  bd;
extern char      *name_system;

int l_uosls_v(int innom,const char *datazap,GtkWidget *wpredok)
{

class l_uosls_v_data data;
char strsql[512];
iceb_u_str kikz;

data.innom=innom;
data.datazap.new_plus(datazap);
data.datazap_z.new_plus(datazap);

if(datazap[0] != '\0')
 {
  sprintf(strsql,"select * from Uosls where inn=%d and data='%s'",data.innom,iceb_u_dattosql(datazap) );
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.lik_st_nu.new_plus(row[2]);
  data.lik_st_bu.new_plus(row[3]);
  data.koment.new_plus(row[4]);
  kikz.plus(iceb_kikz(row[5],row[6],wpredok));
  
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.datazap_z.getdlinna() <= 1)
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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_uosls_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
 }
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 0);
//gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);


label=gtk_label_new(gettext("Дата"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAZAP]), label, FALSE, FALSE, 0);

data.entry[E_DATAZAP] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAZAP]), data.entry[E_DATAZAP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAZAP]), "activate",GTK_SIGNAL_FUNC(l_uosls_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAZAP]),data.datazap.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAZAP]),(gpointer)E_DATAZAP);

label=gtk_label_new(gettext("Ликвидационная стоимость для налогового учёта"));
gtk_box_pack_start (GTK_BOX (hbox[E_LIK_ST_NU]), label, FALSE, FALSE, 0);

data.entry[E_LIK_ST_NU] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_LIK_ST_NU]), data.entry[E_LIK_ST_NU], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_LIK_ST_NU]), "activate",GTK_SIGNAL_FUNC(l_uosls_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_LIK_ST_NU]),data.lik_st_nu.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_LIK_ST_NU]),(gpointer)E_LIK_ST_NU);

label=gtk_label_new(gettext("Ликвидационная стоимость для бухгалтерского учёта"));
gtk_box_pack_start (GTK_BOX (hbox[E_LIK_ST_BU]), label, FALSE, FALSE, 0);

data.entry[E_LIK_ST_BU] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_LIK_ST_BU]), data.entry[E_LIK_ST_BU], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_LIK_ST_BU]), "activate",GTK_SIGNAL_FUNC(l_uosls_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_LIK_ST_BU]),data.lik_st_bu.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_LIK_ST_BU]),(gpointer)E_LIK_ST_BU);

label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new_with_max_length (255);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(l_uosls_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.lik_st_bu.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_uosls_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введённой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(l_uosls_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_uosls_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uosls_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosls_v_data *data)
{
//char  bros[512];

//printf("l_uosls_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");

    return(TRUE);
  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");

    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");

    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
  //  printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_uosls_v_knopka(GtkWidget *widget,class l_uosls_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uosls_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_uosls_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  

  case FK4:
    data->clear_rek();
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

void    l_uosls_v_vvod(GtkWidget *widget,class l_uosls_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uosls_v_vvod enter=%d\n",enter);

switch (enter)
 {
/***********
  case E_DATAZAP:
    data->rk.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_LIK_ST_NU:
    data->rk.naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
****************/
 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int l_uosls_zap(class l_uosls_v_data *data)
{
char strsql[1024];

if(data->datazap.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата!"),data->window);
  return(1);
 }


if(data->datazap_z.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Uosls values(%d,'%s',%.2f,%.2f,'%s',%d,%ld)",
  data->innom,
  data->datazap.ravno_sqldata(),
  data->lik_st_nu.ravno_atof(),
  data->lik_st_bu.ravno_atof(),
  data->koment.ravno(),
  iceb_getuid(data->window),
  time(NULL));
 }
else
 {
  
  sprintf(strsql,"update Uosls set \
data='%s',\
lsnu=%.2f,\
lsbu=%.2f,\
kom='%s',\
ktoz=%d,\
vrem=%ld \
where inn=%d and data='%s'",
  data->datazap.ravno_sqldata(),
  data->lik_st_nu.ravno_atof(),
  data->lik_st_bu.ravno_atof(),
  data->koment.ravno(),
  iceb_getuid(data->window),
  time(NULL),
  data->innom,
  data->datazap_z.ravno_sqldata());

   
 }

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


return(0);

}
