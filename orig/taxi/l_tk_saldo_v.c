/*$Id: l_tk_saldo_v.c,v 1.14 2011-02-21 07:36:20 sasa Exp $*/
/*01.04.2005	31.03.2005	Белых А.И.	l_tk_saldo_v.c
Ввод или корректировка записи сальдо по клиенту
*/
#include <unistd.h>
#include "taxi.h"
#include "l_tk_saldo.h"

enum
{
 FK2,
// FK4,
 FK10,
 KOL_F_KL
};

enum
{
 E_GOD,
 E_KOD,
 E_SALDO,
 KOLENTER
};

class tk_saldo_v_data 
 {
  public:
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *entry[KOLENTER];
  GtkWidget *opt;
  short     kl_shift; //0-отжата 1-нажата  

  int       voz;
       
  class tk_saldo_rek_data vvod;

  iceb_u_str *kodkl; //Код клиента для корректировки
  int  god;   //Год клиента для корректировки
  
  tk_saldo_v_data()
   {
    voz=0;
    kl_shift=0;
   }
  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }
  
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }
 };


gboolean   tk_saldo_v_key_press(GtkWidget *widget,GdkEventKey *event,class tk_saldo_v_data *data);
void  tk_saldo_v_knopka(GtkWidget *widget,class tk_saldo_v_data *data);
void    tk_saldo_v_vvod(GtkWidget *widget,class tk_saldo_v_data *data);
int tk_saldo_pk(int,const char *,GtkWidget*);
int tk_saldo_zap(class tk_saldo_v_data *data);
void tk_saldo_kodgr(class tk_saldo_v_data *data);
void  tk_saldo_v_v_e_knopka(GtkWidget *widget,class tk_saldo_v_data *data);

extern SQL_baza	bd;
extern char *name_system;


int l_tk_saldo_v(int god,iceb_u_str *kodkl,GtkWidget *wpredok)
{
char strsql[300];
class tk_saldo_v_data data;
iceb_u_str kikz;

printf("l_tk_saldo_v\n");
    
data.kl_shift=0;
data.vvod.clear_data();
data.kodkl=kodkl;
data.god=god;

if(kodkl->getdlinna() > 1)
 {
  SQL_str row;
  SQLCURSOR cur;
  char strsql[300];
  sprintf(strsql,"select * from Taxiklsal where god=%d and kod='%s'",god,kodkl->ravno());
  
  if(sql_readkey(&bd,strsql,&row,&cur) < 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    iceb_menu_soob(&repl,data.window);
    return(1);
   }

  data.vvod.kod.new_plus(row[1]);
  data.vvod.god.new_plus(row[0]);
  data.vvod.saldo.new_plus(row[2]);

  kikz.plus(iceb_kikz(row[3],row[4],wpredok));   
  
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);


gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
//gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_widget_destroyed),&data.window);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(tk_saldo_v_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label;
if(kodkl->ravno()[0] == '\0')
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

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER+1];
for(int i=0 ; i < KOLENTER+1; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0 ; i < KOLENTER+1; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

label=gtk_label_new(iceb_u_toutf("Год"));
data.entry[E_GOD] = gtk_entry_new_with_max_length (5);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), data.entry[E_GOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GOD]), "activate",GTK_SIGNAL_FUNC(tk_saldo_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD]),data.vvod.god.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GOD]),(gpointer)E_GOD);


data.knopka_enter[E_KOD]=gtk_button_new_with_label(iceb_u_toutf("Код клиента"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.knopka_enter[E_KOD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD]),"clicked",GTK_SIGNAL_FUNC(tk_saldo_v_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD]),(gpointer)E_KOD);
tooltips_enter[E_KOD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD],data.knopka_enter[E_KOD],iceb_u_toutf("Выбор кода клиента в списке"),NULL);

data.entry[E_KOD] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(tk_saldo_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.vvod.kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

label=gtk_label_new(iceb_u_toutf("Сальдо"));
data.entry[E_SALDO] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_SALDO]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SALDO]), data.entry[E_SALDO], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SALDO]), "activate",GTK_SIGNAL_FUNC(tk_saldo_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SALDO]),data.vvod.saldo.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SALDO]),(gpointer)E_SALDO);



GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(tk_saldo_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(tk_saldo_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
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
void  tk_saldo_v_v_e_knopka(GtkWidget *widget,class tk_saldo_v_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case E_KOD:

//    if(lklient(1,&kod,data->window) == 0)
    if(l_taxisk(1,&kod,&naim,data->window) == 0)
     {
      data->vvod.kod.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD]),data->vvod.kod.ravno());
     }      
    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   tk_saldo_v_key_press(GtkWidget *widget,GdkEventKey *event,class tk_saldo_v_data *data)
{

//printf("tk_saldo_v_key_press\n");
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
void  tk_saldo_v_knopka(GtkWidget *widget,class tk_saldo_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case FK2:
    data->read_rek();
    if(tk_saldo_zap(data) == 0)
     {
      gtk_widget_destroy(data->window);
      data->window=NULL;
     }
    return;  


  case FK10:
    gtk_widget_destroy(data->window);
    data->window=NULL;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    tk_saldo_v_vvod(GtkWidget *widget,class tk_saldo_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("tk_saldo_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->vvod.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_GOD:
    data->vvod.god.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_SALDO:
    data->vvod.saldo.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int tk_saldo_zap(class tk_saldo_v_data *data)
{
char strsql[1000];
time_t   vrem;
time(&vrem);

if(data->vvod.kod.getdlinna() <= 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не введён код !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 }
if(data->vvod.god.getdlinna() <= 1)
 {
  iceb_u_str repl;
  repl.plus("Не введён год !");
  iceb_menu_soob(&repl,data->window);
  return(1);
 }
if(data->vvod.saldo.getdlinna() <= 1)
 {
  iceb_u_str repl;
  repl.plus("Не введёно сальдо !");
  iceb_menu_soob(&repl,data->window);
  return(1);
 }


if(data->kodkl->ravno()[0] == '\0') //Ввод новой записи
 {
  if(tk_saldo_pk(data->vvod.god.ravno_atoi(),data->vvod.kod.ravno(),data->window) != 0)
    return(1);

  sprintf(strsql,"insert into Taxiklsal \
values (%d,'%s',%.2f,%d,%ld)",
  data->vvod.god.ravno_atoi(),
  data->vvod.kod.ravno_filtr(),
  data->vvod.saldo.ravno_atof(),
  iceb_getuid(data->window),vrem);

 }
else //Корректировка записи
 {
  if(iceb_u_SRAV(data->vvod.kod.ravno(),data->kodkl->ravno(),0) != 0 || data->god != data->vvod.god.ravno_atoi())
   {
    if(tk_saldo_pk(data->vvod.god.ravno_atoi(),data->vvod.kod.ravno(),data->window) != 0)
     return(1);
   }

  sprintf(strsql,"update Taxiklsal \
set \
god=%d,\
kod='%s',\
saldo=%.2f,\
ktoi=%d,\
vrem=%ld \
where god=%d and kod='%s'",
  data->vvod.god.ravno_atoi(),
  data->vvod.kod.ravno_filtr(),
  data->vvod.saldo.ravno_atof(),
  iceb_getuid(data->window),
  vrem,
  data->god,
  data->kodkl->ravno() ); 
  

 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int tk_saldo_pk(int god,const char *kod,GtkWidget *wpredok)
{
char strsql[300];

sprintf(strsql,"select kod from Taxiklient where god=%d ang kod='%s'",
god,kod);

//printf("tk_saldo_pk-%s\n",strsql);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Такая запись уже есть !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

return(0);

}
