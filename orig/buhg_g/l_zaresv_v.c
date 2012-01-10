/*$Id: l_zaresv_v.c,v 1.1 2011-02-21 07:38:50 sasa Exp $*/
/*23.01.2011	23.01.2011	Белых А.И.	l_zaresv_v.c
Ввод и корректировка процентов
*/
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"

enum
 {
  E_DATAND,
  E_PRF,
  E_PRR,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_zaresv_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет
  
  int kod_fonda;
  
  class iceb_u_str datand_zap;  
  class iceb_u_str datand;  
  class iceb_u_str prf;  
  class iceb_u_str prr;  
  
  l_zaresv_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    prf.plus("");
    prr.plus("");
    
   }

  void read_rek()
   {
    datand.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAND]))));
    prf.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PRF]))));
    prr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PRR]))));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    
   }
 };


gboolean   l_zaresv_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zaresv_v_data *data);
void  l_zaresv_v_knopka(GtkWidget *widget,class l_zaresv_v_data *data);
void    l_zaresv_v_vvod(GtkWidget *widget,class l_zaresv_v_data *data);
int l_zaresv_zap(class l_zaresv_v_data *data);


extern SQL_baza  bd;
extern char      *name_system;

int l_zaresv_v(int kod_fonda,const char *datand,GtkWidget *wpredok)
{

class l_zaresv_v_data data;
char strsql[512];
iceb_u_str kikz;
data.datand.new_plus(datand);
data.datand_zap.new_plus(datand);
data.kod_fonda=kod_fonda;

if(datand[0] != '\0')
 {
  sprintf(strsql,"select * from Zaresv where kf=%d and datnd='%s'",data.kod_fonda,data.datand.ravno_sqldata());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.prf.new_plus(row[2]);
  data.prr.new_plus(row[3]);
  kikz.plus(iceb_kikz(row[4],row[5],wpredok));
  
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.datand.getdlinna() <= 1)
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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_zaresv_v_key_press),&data);

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


label=gtk_label_new(gettext("Дата начала действия"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAND]), label, FALSE, FALSE, 0);

data.entry[E_DATAND] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAND]), data.entry[E_DATAND], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAND]), "activate",GTK_SIGNAL_FUNC(l_zaresv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAND]),data.datand.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAND]),(gpointer)E_DATAND);

label=gtk_label_new(gettext("Процент начисления на фонд зарплаты"));
gtk_box_pack_start (GTK_BOX (hbox[E_PRF]), label, FALSE, FALSE, 0);

data.entry[E_PRF] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_PRF]), data.entry[E_PRF], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PRF]), "activate",GTK_SIGNAL_FUNC(l_zaresv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PRF]),data.prf.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PRF]),(gpointer)E_PRF);

label=gtk_label_new(gettext("Процент удержания с работкика"));
gtk_box_pack_start (GTK_BOX (hbox[E_PRR]), label, FALSE, FALSE, 0);

data.entry[E_PRR] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_PRR]), data.entry[E_PRR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PRR]), "activate",GTK_SIGNAL_FUNC(l_zaresv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PRR]),data.prr.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PRR]),(gpointer)E_PRR);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_zaresv_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введённой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(l_zaresv_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_zaresv_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[1]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zaresv_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zaresv_v_data *data)
{
//char  bros[512];

//printf("l_zaresv_v_key_press\n");
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
void  l_zaresv_v_knopka(GtkWidget *widget,class l_zaresv_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_zaresv_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_zaresv_zap(data) == 0)
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

void    l_zaresv_v_vvod(GtkWidget *widget,class l_zaresv_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_zaresv_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAND:
    data->datand.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_PRF:
    data->prf.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int l_zaresv_zap(class l_zaresv_v_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);


if(data->datand.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введена дата!"),data->window);
  return(1);
 }



if(iceb_u_sravmydat(data->datand_zap.ravno(),data->datand.ravno()) != 0)
 {
  sprintf(strsql,"select kf from Zaresv where kf=%d and datnd='%s'",data->kod_fonda,data->datand.ravno_sqldata());
  if(iceb_sql_readkey(strsql,data->window) == 1)
   {
    
    iceb_menu_soob(gettext("Такая запись уже есть!"),data->window);
    return(1);
   }
 }

if(data->datand_zap.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Zaresv \
values (%d,'%s',%.2f,%.2f,%d,%ld)",
   data->kod_fonda,
   data->datand.ravno_sqldata(),
   data->prf.ravno_atof(),
   data->prr.ravno_atof(),
   iceb_getuid(data->window),vrem);
 }
else
 {
  
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Zaresv \
set \
datnd='%s',\
pr=%.2f,\
pr1=%.2f,\
ktoz=%d,\
vrem=%ld \
where kf=%d and datnd='%s'",
   data->datand.ravno_sqldata(),
   data->prf.ravno_atof(),
   data->prr.ravno_atof(),
   iceb_getuid(data->window),vrem,
   data->kod_fonda,
   data->datand_zap.ravno_sqldata());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


return(0);

}
