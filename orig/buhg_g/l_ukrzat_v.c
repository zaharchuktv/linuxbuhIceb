/*$Id: l_ukrzat_v.c,v 1.15 2011-02-21 07:35:53 sasa Exp $*/
/*11.10.2006	10.10.2006	Белых А.И.	l_ukrzat_v.c
Ввод и корректировка командировочных расходов
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
enum
 {
  E_KOD,
  E_GRUP,
  E_NAIM,
  E_EI,
  E_SHET,
  E_CENA,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

class l_ukrzat_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str kod_zat;  
  class iceb_u_str grup;  
  class iceb_u_str naim;  
  class iceb_u_str ei;  
  class iceb_u_str shet;  
  class iceb_u_str cena;  
  short metka_nds;
  //корректируемый код затрат
  class iceb_u_str kod_zat_k;
    
  l_ukrzat_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_rek();
   }

  void read_rek()
   {
    kod_zat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD]))));
    naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM]))));
    grup.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_GRUP]))));
    ei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_EI]))));
    shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET]))));
    cena.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_CENA]))));
   }
  
  void clear_rek()
   {
    kod_zat.new_plus("");
    grup.new_plus("");
    naim.new_plus("");
    ei.new_plus("");
    shet.new_plus("");
    cena.new_plus("");
    metka_nds=0;
   
   }
   
 };


gboolean   l_ukrzat_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_ukrzat_v_data *data);
void  l_ukrzat_v_knopka(GtkWidget *widget,class l_ukrzat_v_data *data);
void    l_ukrzat_v_vvod(GtkWidget *widget,class l_ukrzat_v_data *data);
int l_ukrzat_pk(const char *god,GtkWidget*);
int l_ukrzat_zap(class l_ukrzat_v_data *data);
void l_ukrzat_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);

void  l_ukrzat_v_e_knopka(GtkWidget *widget,class l_ukrzat_v_data *data);
void   l_ukrzat_get_pm0(GtkWidget *widget,short *data);
void   l_ukrzat_get_pm1(GtkWidget *widget,short *data);

extern SQL_baza  bd;
extern char      *name_system;

int l_ukrzat_v(class iceb_u_str *kod_zat,GtkWidget *wpredok)
{

class l_ukrzat_v_data data;
char strsql[512];
iceb_u_str kikz;
data.kod_zat_k.new_plus(kod_zat->ravno());


if(data.kod_zat_k.getdlinna() >  1)
 {
  data.kod_zat.new_plus(data.kod_zat_k.ravno());
  sprintf(strsql,"select * from Ukrkras where kod=%s",data.kod_zat.ravno());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.naim.new_plus(row[1]);
  if(atoi(row[2]) != 0.)
    data.cena.new_plus(row[2]);
  data.shet.new_plus(row[3]);
  data.grup.new_plus(row[4]);
  data.ei.new_plus(row[5]);
  data.metka_nds=atoi(row[6]);
  
  kikz.plus(iceb_kikz(row[7],row[8],wpredok));
  
 }
else
 {
  sprintf(strsql,"%d",iceb_get_new_kod("Ukrkras",0,wpredok));
  data.kod_zat.new_plus(strsql);
 } 
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.kod_zat_k.getdlinna() <= 1)
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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_ukrzat_v_key_press),&data);

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
GtkWidget *hbox_metka_nds = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hbox_metka_nds);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


label=gtk_label_new(gettext("Код"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);

data.entry[E_KOD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(l_ukrzat_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.kod_zat.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

label=gtk_label_new(gettext("Наименование"));
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE, 0);

data.entry[E_NAIM] = gtk_entry_new_with_max_length (100);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(l_ukrzat_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Код группы"));
data.knopka_enter[E_GRUP]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP]), data.knopka_enter[E_GRUP], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_GRUP]),"clicked",GTK_SIGNAL_FUNC(l_ukrzat_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_GRUP]),(gpointer)E_GRUP);
tooltips_enter[E_GRUP]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_GRUP],data.knopka_enter[E_GRUP],gettext("Выбор группы"),NULL);

data.entry[E_GRUP] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP]), data.entry[E_GRUP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GRUP]), "activate",GTK_SIGNAL_FUNC(l_ukrzat_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUP]),data.grup.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GRUP]),(gpointer)E_GRUP);

sprintf(strsql,"%s",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(l_ukrzat_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(l_ukrzat_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

sprintf(strsql,"%s",gettext("Единица измерения"));
data.knopka_enter[E_EI]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.knopka_enter[E_EI], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_EI]),"clicked",GTK_SIGNAL_FUNC(l_ukrzat_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_EI]),(gpointer)E_EI);
tooltips_enter[E_EI]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_EI],data.knopka_enter[E_EI],gettext("Выбор единицы измерения"),NULL);

data.entry[E_EI] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_EI]), "activate",GTK_SIGNAL_FUNC(l_ukrzat_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.ei.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_EI]),(gpointer)E_EI);

label=gtk_label_new(gettext("Цена"));
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), label, FALSE, FALSE, 0);

data.entry[E_CENA] = gtk_entry_new_with_max_length (100);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), data.entry[E_CENA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_CENA]), "activate",GTK_SIGNAL_FUNC(l_ukrzat_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA]),data.cena.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_CENA]),(gpointer)E_CENA);


label=gtk_label_new(gettext("Цена введена"));
gtk_box_pack_start (GTK_BOX (hbox_metka_nds), label, FALSE, FALSE, 0);

GtkWidget *opt = gtk_option_menu_new();
GtkWidget *menu = gtk_menu_new();

GtkWidget *item;
item = gtk_menu_item_new_with_label (gettext("С НДС"));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(l_ukrzat_get_pm0),&data.metka_nds);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (gettext("Без НДС"));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(l_ukrzat_get_pm1),&data.metka_nds);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

gtk_option_menu_set_menu (GTK_OPTION_MENU (opt), menu);

gtk_option_menu_set_history(GTK_OPTION_MENU(opt),data.metka_nds);

gtk_box_pack_start (GTK_BOX (hbox_metka_nds), opt, FALSE, FALSE, 0);


GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],data.knopka[PFK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK2]),"clicked",GTK_SIGNAL_FUNC(l_ukrzat_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK2]),(gpointer)PFK2);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],data.knopka[PFK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK10]),"clicked",GTK_SIGNAL_FUNC(l_ukrzat_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK10]),(gpointer)PFK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 kod_zat->new_plus(data.kod_zat.ravno());
return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_ukrzat_v_e_knopka(GtkWidget *widget,class l_ukrzat_v_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("ukrzat_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_GRUP:

    if(l_ukrgrup(1,&kod,&naim,data->window) == 0)
     {
      data->grup.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUP]),data->grup.ravno_toutf());
     }

    return;  

  case E_SHET:

    iceb_vibrek(1,"Plansh",&data->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet.ravno_toutf());

    return;  

  case E_EI:

    iceb_vibrek(1,"Edizmer",&data->ei,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),data->ei.ravno_toutf());

    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_ukrzat_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_ukrzat_v_data *data)
{
//char  bros[512];

//printf("l_ukrzat_v_key_press\n");
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
void  l_ukrzat_v_knopka(GtkWidget *widget,class l_ukrzat_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_ukrzat_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case PFK2:
    data->read_rek();
    if(l_ukrzat_zap(data) == 0)
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

void    l_ukrzat_v_vvod(GtkWidget *widget,class l_ukrzat_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_ukrzat_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->kod_zat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NAIM:
    data->naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_GRUP:
    data->grup.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_SHET:
    data->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_EI:
    data->ei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_CENA:
    data->cena.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int l_ukrzat_zap(class l_ukrzat_v_data *data)
{
char strsql[1024];


if(data->kod_zat.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введён код !"),data->window);
  return(1);
 }

if(data->naim.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введено наименование !"),data->window);
  return(1);
 }

if(data->grup.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введено код группы !"),data->window);
  return(1);
 }

//проверяем код группы
sprintf(strsql,"select naik from Ukrgrup where kod=%d",data->grup.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Ненайден код группы"),data->grup.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
//проверяем счёт
if(data->shet.getdlinna() > 1)
 {
  class OPSHET rekshet;
  if(iceb_prsh1(data->shet.ravno(),&rekshet,data->window) != 0)
   return(1);
 }
//проверяем единицу измерения
if(data->ei.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Edizmer where kod='%s'",data->ei.ravno());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Ненайдена единица измерения"),data->ei.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }  

short metkakor=0;

if(iceb_u_SRAV(data->kod_zat.ravno(),data->kod_zat_k.ravno(),0) != 0)
  if(l_ukrzat_pk(data->kod_zat.ravno(),data->window) != 0)
     return(1);

time_t   vrem;
time(&vrem);

if(data->kod_zat_k.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Ukrkras \
values (%d,'%s',%.2f,'%s',%d,'%s',%d,%d,%ld)",
   data->kod_zat.ravno_atoi(),
   data->naim.ravno_filtr(),
   data->cena.ravno_atof(),
   data->shet.ravno(),
   data->grup.ravno_atoi(),
   data->ei.ravno(),
   data->metka_nds,
   iceb_getuid(data->window),vrem);
 }
else
 {
  
  if(iceb_u_SRAV(data->kod_zat.ravno(),data->kod_zat_k.ravno(),0) != 0)
   {

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Ukrkras \
set \
kod=%d,\
naim='%s',\
cena=%.2f,\
shet='%s',\
kgr=%d,\
eiz='%s',\
mnds=%d,\
ktoz=%d,\
vrem=%ld \
where kod=%d",
   data->kod_zat.ravno_atoi(),
   data->naim.ravno_filtr(),
   data->cena.ravno_atof(),
   data->shet.ravno(),
   data->grup.ravno_atoi(),
   data->ei.ravno(),
   data->metka_nds,
   iceb_getuid(data->window),vrem,
   data->kod_zat_k.ravno_atoi());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);

if(metkakor == 1)
 l_ukrzat_kkvt(data->kod_zat_k.ravno(),data->kod_zat.ravno(),data->window);

return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_ukrzat_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod from Ukrkras where kod=%s",kod);
//printf("l_ukrzat_pk-%s\n",strsql);

if(sql_readkey(&bd,strsql) > 0)
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

void l_ukrzat_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Ukrdok1 set kodr=%s where kodr=%s",
nkod,skod);
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
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
 }

}
/**********************/
/*Читаем              */
/**********************/

void   l_ukrzat_get_pm0(GtkWidget *widget,short *data)
{
*data=0;
//printf("l_ukrzat_get_pm0-%d\n",*data);
}
void   l_ukrzat_get_pm1(GtkWidget *widget,short *data)
{
*data=1;
//printf("l_ukrzat_get_pm1-%d\n",*data);
}
