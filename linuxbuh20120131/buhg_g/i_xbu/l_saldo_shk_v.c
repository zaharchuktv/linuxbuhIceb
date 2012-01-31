/*$Id: l_saldo_shk_v.c,v 1.18 2011-02-21 07:35:53 sasa Exp $*/
/*05.06.2009	31.12.2003	Белых А.И.	l_salsdo_shk_v.c
Ввод и корректировка сальдо по счетам
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze

enum
 {
  E_GOD,
  E_SHET,
  E_KONTR,
  E_DEBET,
  E_KREDIT,
  KOLENTER  
 };

enum
 {
  FK2,
  FK3,
  FK10,
  KOL_FK
 };

class saldo_shkv_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_naim_kontr;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  int god_k;
  class iceb_u_str shet_k;
  class iceb_u_str kontr_k;  
  
  class iceb_u_str god;
  class iceb_u_str shet;
  class iceb_u_str kontr;
  class iceb_u_str debet;
  class iceb_u_str kredit;


  saldo_shkv_data() //Конструктор
   {
    clear();
    kl_shift=0;
    voz=1;
   
   }

  void clear()      
   {
    god.new_plus("");  
    shet.new_plus("");  
    kontr.new_plus("");
    debet.new_plus("");  
    kredit.new_plus("");  
   }

  void read_rek()
   {
    god.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_GOD]))));
    shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET]))));
    kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR]))));
    debet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DEBET]))));
    kredit.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KREDIT]))));
   }
 };

gboolean   saldo_shk_v_key_press(GtkWidget *widget,GdkEventKey *event,class saldo_shkv_data *data);
void  saldo_shk_v_knopka(GtkWidget *widget,class saldo_shkv_data *data);
void    saldo_shk_v_vvod(GtkWidget *widget,class saldo_shkv_data *data);
int saldo_shk_pk(const char *god,const char *shet,const char *kontr,GtkWidget*);
int saldo_shk_zap(class saldo_shkv_data *data);

void  l_saldo_shk_v_e_knopka(GtkWidget *widget,class saldo_shkv_data *data);

extern char *name_system;

int l_saldo_shk_v(class iceb_u_str *god_k,class iceb_u_str *shet_k,class iceb_u_str *kontr_k,GtkWidget *wpredok)
{
char strsql[512];
class saldo_shkv_data data;
class iceb_u_str kikz;
class iceb_u_str naim_kontr("");

data.god_k=god_k->ravno_atoi();
data.shet_k.new_plus(shet_k->ravno());
data.kontr_k.new_plus(kontr_k->ravno());

if(data.god_k != 0)
 {

  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select * from Saldo where kkk=1 and gs=%d and ns='%s' and kodkon='%s'",
  data.god_k,data.shet_k.ravno_filtr(),data.kontr_k.ravno_filtr());
  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.god.new_plus(row[1]);
  data.shet.new_plus(row[2]);
  data.kontr.new_plus(row[3]);
  data.debet.new_plus(row[4]);
  data.kredit.new_plus(row[5]);

  kikz.plus(iceb_kikz(row[6],row[7],wpredok));
  
  /*Читаем наименование контрагента*/
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data.kontr_k.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_kontr.new_plus(row[0]);
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.god_k == 0)
  sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));
else
  sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

GtkWidget *label=NULL;

if(data.god_k == 0)
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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(saldo_shk_v_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *vbox = gtk_vbox_new (FALSE,1);
GtkWidget *hbox1 = gtk_hbox_new (FALSE,1);
GtkWidget *hbox2 = gtk_hbox_new (FALSE,1);
GtkWidget *hbox3 = gtk_hbox_new (FALSE,1);
GtkWidget *hbox4 = gtk_hbox_new (FALSE,1);
GtkWidget *hbox5 = gtk_hbox_new (FALSE,1);
GtkWidget *hboxknop = gtk_hbox_new (FALSE,1);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (vbox), hbox1, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (vbox), hbox2, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (vbox), hbox3, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (vbox), hbox4, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (vbox), hbox5, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (vbox), hboxknop, FALSE, FALSE,1);

GtkTooltips *tooltips_enter[KOLENTER];

label=gtk_label_new(gettext("Год"));
data.entry[E_GOD] = gtk_entry_new_with_max_length (4);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox1), data.entry[E_GOD], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GOD]), "activate",GTK_SIGNAL_FUNC(saldo_shk_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD]),data.god.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GOD]),(gpointer)E_GOD);


sprintf(strsql,"%s",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox2), data.knopka_enter[E_SHET], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(l_saldo_shk_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox2), data.entry[E_SHET], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(saldo_shk_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);


sprintf(strsql,"%s",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox3), data.knopka_enter[E_KONTR], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KONTR]),"clicked",GTK_SIGNAL_FUNC(l_saldo_shk_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KONTR]),(gpointer)E_KONTR);
tooltips_enter[E_KONTR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KONTR],data.knopka_enter[E_KONTR],gettext("Выбор контрагента"),NULL);

data.entry[E_KONTR] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox3), data.entry[E_KONTR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KONTR]), "activate",GTK_SIGNAL_FUNC(saldo_shk_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.kontr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KONTR]),(gpointer)E_KONTR);

data.label_naim_kontr=gtk_label_new(naim_kontr.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (hbox3), data.label_naim_kontr, TRUE, TRUE,1);


label=gtk_label_new(gettext("Дебет"));
data.entry[E_DEBET] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox4), data.entry[E_DEBET], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DEBET]), "activate",GTK_SIGNAL_FUNC(saldo_shk_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DEBET]),data.debet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DEBET]),(gpointer)E_DEBET);


label=gtk_label_new(gettext("Кредит"));
data.entry[E_KREDIT] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox5), label, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox5), data.entry[E_KREDIT], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KREDIT]), "activate",GTK_SIGNAL_FUNC(saldo_shk_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KREDIT]),data.kredit.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KREDIT]),(gpointer)E_KREDIT);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(saldo_shk_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop),data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F3 %s",gettext("Контрагенты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Просмотр общего списка контрагентов"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]),"clicked",GTK_SIGNAL_FUNC(saldo_shk_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_box_pack_start(GTK_BOX(hboxknop),data.knopka[FK3], TRUE, TRUE,1);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(saldo_shk_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop),data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 {
  god_k->new_plus(data.god.ravno());
  shet_k->new_plus(data.shet.ravno());
  kontr_k->new_plus(data.kontr.ravno());
 }

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);


}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_saldo_shk_v_e_knopka(GtkWidget *widget,class saldo_shkv_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  case E_SHET:

    iceb_vibrek(1,"Plansh",&data->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet.ravno_toutf());

    return;  

  case E_KONTR:
    if(iceb_poi_kontr(&data->kontr,&naim,data->window) == 0)
     {
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->kontr.ravno_toutf());
      gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),naim.ravno_toutf());
     }
/*******
    iceb_vibrek(1,"Kontragent",&data->kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->kontr.ravno_toutf());
*********/
    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   saldo_shk_v_key_press(GtkWidget *widget,GdkEventKey *event,class saldo_shkv_data *data)
{
iceb_u_str kod;
iceb_u_str naikod;

//printf("saldo_shk_v_key_press\n");
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
void  saldo_shk_v_knopka(GtkWidget *widget,class saldo_shkv_data *data)
{
//char bros[512];
iceb_u_str kod;
iceb_u_str naikod;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(saldo_shk_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  

  case FK3:
     if(iceb_l_kontr(1,&kod,&naikod,data->window) == 0)
      {
       data->kontr.new_plus(kod.ravno());
       gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->kontr.ravno_toutf());
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

void    saldo_shk_v_vvod(GtkWidget *widget,class saldo_shkv_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("saldo_shk_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_GOD:
    data->god.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_SHET:
    data->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KONTR:
    data->kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(data->kontr.getdlinna() > 1)
     {
      SQL_str row;
      class SQLCURSOR cur;
      char strsql[512];
      class iceb_u_str naim("");
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->kontr.ravno());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
        naim.new_plus(row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),naim.ravno_toutf(20));
     }
    break;
  case E_DEBET:
    data->debet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KREDIT:
    data->kredit.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int saldo_shk_zap(class saldo_shkv_data *data)
{
char strsql[1024];
//SQL_str row;
time_t   vrem;
time(&vrem);


for(int i=0; i < KOLENTER; i++)
 {
  gtk_signal_emit_by_name(GTK_OBJECT(data->entry[i]),"activate");
 }

if(data->god.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введен год !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }

if(data->shet.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введен счет !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }

if(iceb_pvglkni(1,data->god.ravno_atoi(),data->window) != 0)
 return(1);

if(iceb_pbsh(1,data->god.ravno_atoi(),data->shet.ravno(),data->shet_k.ravno(),"",data->window) != 0)
 return(1);

if(data->god_k != 0)
  if(data->god_k != data->god.ravno_atoi())
   if(iceb_pvglkni(1,data->god_k,data->window) != 0)
    return(1);

OPSHET rekshet;

if(iceb_prsh1(data->shet.ravno(),&rekshet,data->window) != 0)
 return(1);

if(rekshet.saldo != 3)
 {
  iceb_u_str sp;
  sp.plus(gettext("Счет имеет свернутое сальдо !"));
  iceb_menu_soob(&sp,data->window);
  return(1);

 }

if(iceb_u_atof(data->debet.ravno()) == 0. && iceb_u_atof(data->kredit.ravno()) == 0.)
 {
  iceb_u_str sp;
  sp.plus(gettext("Дебет и кредит равны нолю !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }


if(data->god_k == 0)
 {

  if(saldo_shk_pk(data->god.ravno(),data->shet.ravno(),data->kontr.ravno(),data->window) != 0)
   return(1);
  
  sprintf(strsql,"insert into Saldo \
values (%d,%d,'%s','%s',%.2f,%.2f,%d,%ld)",
   1,
   atoi(data->god.ravno_filtr()),
   data->shet.ravno_filtr(),
   data->kontr.ravno_filtr(),
   iceb_u_atof(data->debet.ravno()),
   iceb_u_atof(data->kredit.ravno()),
   iceb_getuid(data->window),vrem);
 }
else
 {

  if(data->god_k != data->god.ravno_atoi() ||
  iceb_u_SRAV(data->shet.ravno(),data->shet_k.ravno(),0) != 0 ||
  iceb_u_SRAV(data->kontr.ravno(),data->kontr_k.ravno(),0) != 0 )
   {
    if(saldo_shk_pk(data->god.ravno(),data->shet.ravno(),data->kontr.ravno(),data->window) != 0)
     return(1);
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Saldo \
set \
gs=%d,\
ns='%s',\
kodkon='%s',\
deb=%.2f,\
kre=%.2f,\
ktoi=%d,\
vrem=%ld \
where kkk=%d and gs=%d and ns='%s' and kodkon='%s'",
   atoi(data->god.ravno()),
   data->shet.ravno_filtr(),
   data->kontr.ravno_filtr(),
   iceb_u_atof(data->debet.ravno()),
   iceb_u_atof(data->kredit.ravno()),
   iceb_getuid(data->window),vrem,
   1,
   data->god_k,
   data->shet_k.ravno_filtr(),
   data->kontr_k.ravno_filtr());
   
 }

/*printf("saldo_shk_zap-strsql=%s\n",strsql);*/
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

if(data->god_k == 0)
 {
  //Проверяем есть ли контрагент в списке счета
  sprintf(strsql,"select ns from Skontr where ns='%s' and kodkon='%s'",
  data->shet.ravno_filtr(),
  data->kontr.ravno_filtr());
  if(iceb_sql_readkey(strsql,data->window) < 1)
   {
    sprintf(strsql,"insert into Skontr values ('%s','%s',%d,%ld)",
    data->shet.ravno_filtr(),
    data->kontr.ravno_filtr(),
    iceb_getuid(data->window),vrem);

    iceb_sql_zapis(strsql,0,0,data->window);
    
   }
  
 }


return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int saldo_shk_pk(const char *god,const char *shet,const char *kontr,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kkk from Saldo where kkk=1 and gs=%d and ns='%s' and kodkon='%s'",
atoi(god),shet,kontr);
//printf("saldo_shk_pk-%s\n",strsql);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  iceb_menu_soob(gettext("Такая запись уже есть !"),wpredok);
  return(1);
 }

return(0);

}
