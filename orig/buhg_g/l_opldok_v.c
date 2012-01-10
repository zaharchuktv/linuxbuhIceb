/*$Id: l_opldok_v.c,v 1.8 2011-02-21 07:35:53 sasa Exp $*/
/*22.02.2010	13.10.2009	Белых А.И.	l_opldok_v.c
Ввод и корректировка 
*/
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
enum
 {
  E_DATOPL,
  E_SHET,
  E_KEKV,
  E_SUMA,
  E_NOMDOK_OPL,
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

class l_opldok_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  /*уникальные реквизиты записи*/
  int metka_ps;
  short dd,md,gd;
  class iceb_u_str nomdok;
  int podr;
  time_t vremzap;
    
  /*Реквизиты ввода*/
  class iceb_u_str datopl;
  class iceb_u_str shet;
  class iceb_u_str kekv;
  class iceb_u_str suma;
  class iceb_u_str koment;
  class iceb_u_str nomdok_opl;
  
  l_opldok_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear();
   }

  void read_rek()
   {
    datopl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATOPL]))));
    shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET]))));
    kekv.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KEKV]))));
    suma.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SUMA]))));
    koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT]))));
    nomdok_opl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK_OPL]))));
   }
  void clear()
   {
    datopl.new_plus("");
    shet.new_plus("");
    kekv.new_plus("");
    suma.new_plus("");
    koment.new_plus("");
    nomdok_opl.new_plus("");
   }
 };


gboolean   l_opldok_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_opldok_v_data *data);
void  l_opldok_v_knopka(GtkWidget *widget,class l_opldok_v_data *data);
void    l_opldok_v_vvod(GtkWidget *widget,class l_opldok_v_data *data);
int l_opldok_zap(class l_opldok_v_data *data);
void  l_opldok_v_e_knopka(GtkWidget *widget,class l_opldok_v_data *data);


extern SQL_baza  bd;
extern char      *name_system;

int l_opldok_v(int metka_ps, //1-материальный учёт 2-учёт услуг 3-учёт основных средств
short dd,short md,short gd, //Дата документа
const char *nomdok, //номер документа
int podr, //подразделение или склад
time_t vremzap,
GtkWidget *wpredok)
{

class l_opldok_v_data data;
char strsql[512];
iceb_u_str kikz;

data.dd=dd;
data.md=md;
data.gd=gd;
data.nomdok.new_plus(nomdok);
data.vremzap=vremzap;
data.podr=podr;
data.metka_ps=metka_ps;

if(data.vremzap != 0)
 {
  class SQLCURSOR cur;
  SQL_str row;
  
  sprintf(strsql,"select * from Opldok where vrem=%ld",data.vremzap);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки!"),wpredok);
    return(1);   
   }

  data.datopl.new_plus(iceb_u_sqldata(row[4]));

  data.shet.new_plus(row[5]);
  
  sprintf(strsql,"%.2f",atof(row[6]));
  data.suma.new_plus(strsql);

  data.kekv.new_plus(row[7]);
  data.nomdok_opl.new_plus(row[8]);
  data.koment.new_plus(row[9]);

  kikz.plus(iceb_kikz(row[10],row[11],wpredok));
   
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.vremzap == 0)
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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_opldok_v_key_press),&data);

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
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s (%s)",gettext("Дата оплаты"),gettext("д.м.г"));
data.knopka_enter[E_DATOPL]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATOPL]), data.knopka_enter[E_DATOPL], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATOPL]),"clicked",GTK_SIGNAL_FUNC(l_opldok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATOPL]),(gpointer)E_DATOPL);
tooltips_enter[E_DATOPL]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATOPL],data.knopka_enter[E_DATOPL],gettext("Выбор даты"),NULL);

data.entry[E_DATOPL] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATOPL]), data.entry[E_DATOPL], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATOPL]), "activate",GTK_SIGNAL_FUNC(l_opldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATOPL]),data.datopl.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATOPL]),(gpointer)E_DATOPL);

sprintf(strsql,"%s",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(l_opldok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор даты"),NULL);

data.entry[E_SHET] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(l_opldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);


label=gtk_label_new(gettext("КЭКЗ"));
gtk_box_pack_start (GTK_BOX (hbox[E_KEKV]), label, FALSE, FALSE, 0);

data.entry[E_KEKV] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KEKV]), data.entry[E_KEKV], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KEKV]), "activate",GTK_SIGNAL_FUNC(l_opldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KEKV]),data.kekv.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KEKV]),(gpointer)E_KEKV);


label=gtk_label_new(gettext("Номер документа"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK_OPL]), label, FALSE, FALSE, 0);

data.entry[E_NOMDOK_OPL] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK_OPL]), data.entry[E_NOMDOK_OPL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMDOK_OPL]), "activate",GTK_SIGNAL_FUNC(l_opldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK_OPL]),data.nomdok_opl.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMDOK_OPL]),(gpointer)E_NOMDOK_OPL);

label=gtk_label_new(gettext("Сумма"));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), label, FALSE, FALSE, 0);

data.entry[E_SUMA] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMA]), "activate",GTK_SIGNAL_FUNC(l_opldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.suma.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMA]),(gpointer)E_SUMA);


label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(l_opldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);


GtkTooltips *tooltips[KOL_FK];




sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_opldok_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_opldok_v_knopka),&data);
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
void  l_opldok_v_e_knopka(GtkWidget *widget,class l_opldok_v_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATOPL:

    if(iceb_calendar(&data->datopl,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATOPL]),data->datopl.ravno());
      
    return;  

   
  case E_SHET:

    iceb_vibrek(0,"Plansh",&data->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet.ravno_toutf());

    return;  




 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_opldok_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_opldok_v_data *data)
{
//char  bros[512];

//printf("l_opldok_v_key_press\n");
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
void  l_opldok_v_knopka(GtkWidget *widget,class l_opldok_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_opldok_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_opldok_zap(data) == 0)
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

void    l_opldok_v_vvod(GtkWidget *widget,class l_opldok_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_opldok_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATOPL:
    data->koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KEKV:
    data->kekv.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int l_opldok_zap(class l_opldok_v_data *data)
{
char strsql[1024];
//SQL_str row;
time_t   vrem;
time(&vrem);

if(data->datopl.getdlinna() <= 1 ||\
   data->shet.getdlinna() <= 1 ||\
   data->nomdok_opl.getdlinna() <= 1 ||\
   data->suma.getdlinna() <=1)
 {
  iceb_menu_soob(gettext("Не введены все обязательные реквизиты!"),data->window);
  return(1);
 }       

struct OPSHET op_shet;

if(iceb_prsh1(data->shet.ravno(),&op_shet,data->window) != 0)
 return(1);
short d,m,g;
if(iceb_u_rsdat(&d,&m,&g,data->datopl.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата!"),data->window);
  return(1);
 }
 
if(data->vremzap == 0) //Ввод новой записи
  sprintf(strsql,"insert into Opldok values(%d,'%04d-%02d-%02d','%d','%s','%04d-%02d-%02d','%s',%.2f,%d,'%s','%s',%d,%ld)",
  data->metka_ps,
  data->gd,data->md,data->dd,
  data->podr,
  data->nomdok.ravno(),
  g,m,d,
  data->shet.ravno(),
  data->suma.ravno_atof(),
  data->kekv.ravno_atoi(),
  data->nomdok_opl.ravno(),
  data->koment.ravno_filtr(),
  iceb_getuid(data->window),
  time(NULL));
else
  sprintf(strsql,"update Opldok set \
dato='%04d-%02d-%02d',\
shet='%s',\
suma=%.2f,\
kekv=%d,\
nomp='%s',\
kom='%s',\
ktoi=%d,\
vrem=%ld \
where vrem=%ld",  
  g,m,d,
  data->shet.ravno(),
  data->suma.ravno_atof(),
  data->kekv.ravno_atoi(),
  data->nomdok_opl.ravno(),
  data->koment.ravno_filtr(),
  iceb_getuid(data->window),
  time(NULL),
  data->vremzap);


if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);
return(0);

}
