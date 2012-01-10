/*$Id: rest_oth_v.c,v 1.21 2011-02-21 07:36:21 sasa Exp $*/
/*11.11.2006	04.07.2004	Белых А.И.	rest_oth_v.c
Меню для ввода данных для получения отчёта
*/
#include "i_rest.h"
#include "rest_oth.h"

enum
 {
  E_DATAN,
  E_VREMN,
  E_DATAK,
  E_VREMK,
  E_PODR,
  E_SKL,
  E_KOD_OP,
  KOLENTER
 };

enum
 {
  FK2,
  FK4,
  FK3,
  FK10,
  KOL_F_KL 
 };

class rest_oth_v_data
 {
  public:

  GtkWidget *window;
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  int metka_voz;   //0-выход из меню без запуска отчета 1- с запуском отчета
  int kl_shift;
  short voz; //0-расчёт выполнять 1-нет

  rest_oth_data *rk;
  

  rest_oth_v_data()
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");

   }
  void clear_rek()
   {
    rk->clear();
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }
   
 };

iceb_u_str  rest_oth_data::datan;
iceb_u_str  rest_oth_data::datak;
iceb_u_str  rest_oth_data::vremn;
iceb_u_str  rest_oth_data::vremk;
iceb_u_str  rest_oth_data::podr;
iceb_u_str  rest_oth_data::skl;
iceb_u_str  rest_oth_data::kod_op;

gboolean   rest_oth_v_key_press(GtkWidget *widget,GdkEventKey *event,class rest_oth_v_data *data);
void    rest_oth_v_vvod(GtkWidget *widget,class rest_oth_v_data *data);
void  rest_oth_v_knopka(GtkWidget *widget,class rest_oth_v_data *data);
void   rest_oth_rekviz(class rest_oth_v_data *data);
void  rest_oth_v_v_e_knopka(GtkWidget *widget,class rest_oth_v_data *data);

void   rest_oth_v_rekviz(class rest_oth_v_data *data);

extern char *name_system;
extern SQL_baza bd;
extern class iceb_get_dev_data config_dev;
 
int rest_oth_v(class rest_oth_data *datap)
{
rest_oth_v_data data;
data.rk=datap;
 
char strsql[300];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
if(config_dev.metka_screen == 1)
  gtk_widget_set_usize(data.window,-1,600);

sprintf(strsql,"%s %s",name_system,gettext("Распечатка отчётов"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rest_oth_v_key_press),&data);
iceb_u_str repl;
repl.plus(gettext("Распечатка отчётов"));
repl.ps_plus(gettext("Отчёт делается по времени оплаты"));

GtkWidget *label=gtk_label_new(repl.ravno_toutf());

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



//sprintf(strsql,"%s (%s)",gettext("Дата начала"),gettext("д.м.г"));
//label=gtk_label_new(iceb_u_toutf(strsql));
//gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), label, FALSE, FALSE, 0);
GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s (%s)",gettext("Дата начала"),gettext("д.м.г"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(rest_oth_v_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);
tooltips_enter[E_DATAN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAN],data.knopka_enter[E_DATAN],gettext("Выбор даты"),NULL);


data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(rest_oth_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);

if(config_dev.metka_klav == 0) //Клавиатура используется
 {
  sprintf(strsql,"%s (%s)",gettext("Время начала"),gettext("ч:м:с"));
  label=gtk_label_new(iceb_u_toutf(strsql));
  gtk_box_pack_start (GTK_BOX (hbox[E_VREMN]), label, FALSE, FALSE, 0);
 }
else
 {
  sprintf(strsql,"%s (%s)",gettext("Время начала"),gettext("ч:м:с"));
  data.knopka_enter[E_VREMN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
  gtk_box_pack_start (GTK_BOX (hbox[E_VREMN]), data.knopka_enter[E_VREMN], FALSE, FALSE, 0);
  gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_VREMN]),"clicked",GTK_SIGNAL_FUNC(rest_oth_v_v_e_knopka),&data);
  gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_VREMN]),(gpointer)E_VREMN);
  tooltips_enter[E_VREMN]=gtk_tooltips_new();
  gtk_tooltips_set_tip(tooltips_enter[E_VREMN],data.knopka_enter[E_VREMN],gettext("Ввод времени"),NULL);
 }
data.entry[E_VREMN] = gtk_entry_new_with_max_length (8);
gtk_box_pack_start (GTK_BOX (hbox[E_VREMN]), data.entry[E_VREMN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VREMN]), "activate",GTK_SIGNAL_FUNC(rest_oth_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VREMN]),data.rk->vremn.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VREMN]),(gpointer)E_VREMN);

//sprintf(strsql,"%s (%s)",gettext("Дата конца"),gettext("д.м.г"));
//label=gtk_label_new(iceb_u_toutf(strsql));
//gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), label, FALSE, FALSE, 0);
sprintf(strsql,"%s (%s)",gettext("Дата конца"),gettext("д.м.г"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(rest_oth_v_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);
tooltips_enter[E_DATAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAK],data.knopka_enter[E_DATAK],gettext("Выбор даты"),NULL);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(rest_oth_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);

if(config_dev.metka_klav == 0) //Клавиатура используется
 {
  sprintf(strsql,"%s (%s)",gettext("Время конца"),gettext("ч:м:с"));
  label=gtk_label_new(iceb_u_toutf(strsql));
  gtk_box_pack_start (GTK_BOX (hbox[E_VREMK]), label, FALSE, FALSE, 0);
 }
else
 {
  sprintf(strsql,"%s (%s)",gettext("Время конца"),gettext("ч:м:с"));
  data.knopka_enter[E_VREMK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
  gtk_box_pack_start (GTK_BOX (hbox[E_VREMK]), data.knopka_enter[E_VREMK], FALSE, FALSE, 0);
  gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_VREMK]),"clicked",GTK_SIGNAL_FUNC(rest_oth_v_v_e_knopka),&data);
  gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_VREMK]),(gpointer)E_VREMK);
  tooltips_enter[E_VREMK]=gtk_tooltips_new();
  gtk_tooltips_set_tip(tooltips_enter[E_VREMK],data.knopka_enter[E_VREMK],gettext("Ввод времени"),NULL);
 }
data.entry[E_VREMK] = gtk_entry_new_with_max_length (8);
gtk_box_pack_start (GTK_BOX (hbox[E_VREMK]), data.entry[E_VREMK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VREMK]), "activate",GTK_SIGNAL_FUNC(rest_oth_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VREMK]),data.rk->vremk.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VREMK]),(gpointer)E_VREMK);

//sprintf(strsql,"%s (,,)",gettext("Подразделение"));
//label=gtk_label_new(iceb_u_toutf(strsql));
//gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), label, FALSE, FALSE, 0);
sprintf(strsql,"%s (,,)",gettext("Подразделение"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODR]),"clicked",GTK_SIGNAL_FUNC(rest_oth_v_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODR]),(gpointer)E_PODR);
tooltips_enter[E_PODR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_PODR],data.knopka_enter[E_PODR],gettext("Выбор подразделения"),NULL);

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODR]), "activate",GTK_SIGNAL_FUNC(rest_oth_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk->podr.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODR]),(gpointer)E_PODR);

//sprintf(strsql,"%s (,,)",gettext("Склад"));
//label=gtk_label_new(iceb_u_toutf(strsql));
//gtk_box_pack_start (GTK_BOX (hbox[E_SKL]), label, FALSE, FALSE, 0);

sprintf(strsql,"%s (,,)",gettext("Склад"));
data.knopka_enter[E_SKL]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SKL]), data.knopka_enter[E_SKL], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SKL]),"clicked",GTK_SIGNAL_FUNC(rest_oth_v_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SKL]),(gpointer)E_SKL);
tooltips_enter[E_SKL]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SKL],data.knopka_enter[E_SKL],gettext("Выбор склада"),NULL);

data.entry[E_SKL] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SKL]), data.entry[E_SKL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SKL]), "activate",GTK_SIGNAL_FUNC(rest_oth_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SKL]),data.rk->skl.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SKL]),(gpointer)E_SKL);

if(config_dev.metka_klav == 0) //Клавиатура используется
 {
  sprintf(strsql,"%s (,,)",gettext("Код оператора"));
  label=gtk_label_new(iceb_u_toutf(strsql));
  gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OP]), label, FALSE, FALSE, 0);
 }
else
 {
  sprintf(strsql,"%s (,,)",gettext("Код оператора"));
  data.knopka_enter[E_KOD_OP]=gtk_button_new_with_label(iceb_u_toutf(strsql));
  gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OP]), data.knopka_enter[E_KOD_OP], FALSE, FALSE, 0);
  gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_OP]),"clicked",GTK_SIGNAL_FUNC(rest_oth_v_v_e_knopka),&data);
  gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_OP]),(gpointer)E_KOD_OP);
  tooltips_enter[E_KOD_OP]=gtk_tooltips_new();
  gtk_tooltips_set_tip(tooltips_enter[E_KOD_OP],data.knopka_enter[E_KOD_OP],gettext("Ввод времени"),NULL);
 }

data.entry[E_KOD_OP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OP]), data.entry[E_KOD_OP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_OP]), "activate",GTK_SIGNAL_FUNC(rest_oth_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_OP]),data.rk->kod_op.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_OP]),(gpointer)E_KOD_OP);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Расчет"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчет"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(rest_oth_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Реквизиты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]),"clicked",GTK_SIGNAL_FUNC(rest_oth_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(rest_oth_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(rest_oth_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  rest_oth_v_v_e_knopka(GtkWidget *widget,class rest_oth_v_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.new_plus("");
naim.new_plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case E_DATAN:
    if(config_dev.metka_klav == 0) //Клавиатура используется
     if(iceb_calendar(&data->rk->datan,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno());

    if(config_dev.metka_klav == 1) //Клавиатура не используется
     {
//      gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_DATAN]),"activate");
  
      iceb_mous_klav(gettext("Дата начала"),&data->rk->datan,10,0,0,0,data->window);

      gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno());
     }      
    return;  

  case E_DATAK:

    if(config_dev.metka_klav == 0) //Клавиатура используется
     if(iceb_calendar(&data->rk->datak,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rk->datak.ravno());

    if(config_dev.metka_klav == 1) //Клавиатура не используется
     {
//      gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_DATAK]),"activate");
  
      iceb_mous_klav(gettext("Дата конца"),&data->rk->datak,10,0,0,0,data->window);

      gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rk->datak.ravno());
     }      
      
    return;  

  case E_VREMK:
      iceb_mous_klav(gettext("Время конца"),&data->rk->vremk,10,0,0,0,data->window);

      gtk_entry_set_text(GTK_ENTRY(data->entry[E_VREMK]),data->rk->vremk.ravno());

    return;  

  case E_KOD_OP:
      iceb_mous_klav(gettext("Время конца"),&data->rk->kod_op,100,0,0,0,data->window);

      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_OP]),data->rk->kod_op.ravno());

    return;  

  case E_VREMN:
      iceb_mous_klav(gettext("Время начала"),&data->rk->vremn,10,0,0,0,data->window);

      gtk_entry_set_text(GTK_ENTRY(data->entry[E_VREMN]),data->rk->vremn.ravno());

    return;  

  case E_PODR:
    if(rpod_l(1,&kod,&naim,data->window) == 0)
     {
      data->rk->podr.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk->podr.ravno());
     }

    return;  

  case E_SKL:
    
//    if(iceb_l_sklad(1,&kod,&naim,data->window) == 0)
    if(l_spisok(0,0,&kod,0,data->window) == 0)
     {
      data->rk->skl.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKL]),data->rk->skl.ravno());
     }
    return;  
   
 }
}


/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rest_oth_v_key_press(GtkWidget *widget,GdkEventKey *event,class rest_oth_v_data *data)
{

//printf("rest_oth_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);

  case GDK_F3:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
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
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rest_oth_v_knopka(GtkWidget *widget,class rest_oth_v_data *data)
{
iceb_u_str kod;
iceb_u_str naikod;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    if(iceb_rsdatp_str(&data->rk->datan,&data->rk->datak,data->window) != 0)
      return;
    if(iceb_rstimep_str(&data->rk->vremn,&data->rk->vremk,data->window) != 0)
      return;
        
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case FK3:
    rest_oth_v_rekviz(data);
    return;

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    rest_oth_v_vvod(GtkWidget *widget,class rest_oth_v_data *data)
{
//short d,m,g;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("rest_oth_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAK:
    data->rk->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_VREMN:
    data->rk->vremn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_VREMK:
    data->rk->vremk.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_PODR:
    data->rk->podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SKL:
    data->rk->skl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOD_OP:
    data->rk->kod_op.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/********************/
/*Работа с реквизитами*/
/***********************/
void   rest_oth_v_rekviz(class rest_oth_v_data *data)
{
short nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Реквизиты"));

zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Просмотр списка подразделений"));
punkt_m.plus(gettext("Просмотр списка складов"));

nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);



iceb_u_str kod;
iceb_u_str naikod;

switch (nomer)
 {
  case -1:
    return;
    
  case 0:
    if(rpod_l(1,&kod,&naikod,data->window) == 0)
     {
      if(data->rk->podr.getdlinna() > 1)
       data->rk->podr.plus(",");
      data->rk->podr.plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk->podr.ravno());
     }
    break;

  case 1:
    if(iceb_l_sklad(1,&kod,&naikod,data->window) == 0)
     {
      if(data->rk->skl.getdlinna() > 1)
       data->rk->skl.plus(",");
      data->rk->skl.plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKL]),data->rk->skl.ravno());
     }

    break;

 }

}
