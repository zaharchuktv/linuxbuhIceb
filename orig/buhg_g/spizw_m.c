/*$Id: spizw_m.c,v 1.14 2011-02-21 07:35:57 sasa Exp $*/
/*20.10.2009	01.05.2005	Белых А.И.	spizw_m.c
Меню для ввода реквизитов списания изделия в метериальном учёте
*/
#include <stdlib.h>
#include "buhg_g.h"
#include "spizw.h"
enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAS,
  E_SKLAD,
  E_NOMDOK,
  E_KOLIZ,
  E_KODOP,
  KOLENTER  
 };

class spizw_data
 {
  public:
  spizw_rr  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет

  spizw_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->kodop.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KODOP]))));
    rk->datas.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAS]))));
    rk->sklad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SKLAD]))));
    rk->nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK]))));
    rk->koliz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOLIZ]))));
/****
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
**********/
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }
 };



gboolean   spizw_v_key_press(GtkWidget *widget,GdkEventKey *event,class spizw_data *data);
void    spizw_v_vvod(GtkWidget *widget,class spizw_data *data);
void  spizw_v_knopka(GtkWidget *widget,class spizw_data *data);
void  spizw_v_e_knopka(GtkWidget *widget,class spizw_data *data);

void spizw_m_nomdok(class spizw_data *data);

extern char *name_system;
extern SQL_baza bd;

int   spizw_m(class spizw_rr *rek_ras,GtkWidget *wpredok)
{
spizw_data data;
char strsql[512];
char naim[512];
SQL_str row;
SQLCURSOR cur;

memset(naim,'\0',sizeof(naim));
data.rk=rek_ras;

if(data.rk->datas.getdlinna() == 0)
  data.rk->clear_data();
//Читаем наименование изделия, котрое надо списать
sprintf(strsql,"select naimat from Material where kodm=%s",data.rk->kod_izdel.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 strncpy(naim,row[0],sizeof(naim)-1);  

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Списание материалов изделия с карточек"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(spizw_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
iceb_u_str repl;
repl.plus(gettext("Списание материалов изделия с карточек"));
repl.ps_plus(data.rk->kod_izdel.ravno());
repl.plus(" ");
repl.plus(naim);

GtkWidget *label=gtk_label_new(repl.ravno_toutf());

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

//GtkWidget *hboxradio = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

GtkWidget *separator1=gtk_hseparator_new();
gtk_container_add (GTK_CONTAINER (vbox), separator1);


gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

sprintf(strsql,"%s (%s)",gettext("Дата списания"),gettext("д.м.г"));
data.knopka_enter[E_DATAS]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAS]), data.knopka_enter[E_DATAS], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAS]),"clicked",GTK_SIGNAL_FUNC(spizw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAS]),(gpointer)E_DATAS);

data.entry[E_DATAS] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAS]), data.entry[E_DATAS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAS]), "activate",GTK_SIGNAL_FUNC(spizw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAS]),data.rk->datas.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAS]),(gpointer)E_DATAS);


sprintf(strsql,"%s",gettext("Склад"));
data.knopka_enter[E_SKLAD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.knopka_enter[E_SKLAD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SKLAD]),"clicked",GTK_SIGNAL_FUNC(spizw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SKLAD]),(gpointer)E_SKLAD);

data.entry[E_SKLAD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.entry[E_SKLAD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SKLAD]), "activate",GTK_SIGNAL_FUNC(spizw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SKLAD]),data.rk->sklad.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SKLAD]),(gpointer)E_SKLAD);

sprintf(strsql,"%s",gettext("Номер документа"));
data.knopka_enter[E_NOMDOK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.knopka_enter[E_NOMDOK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_NOMDOK]),"clicked",GTK_SIGNAL_FUNC(spizw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_NOMDOK]),(gpointer)E_NOMDOK);

data.entry[E_NOMDOK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMDOK]), "activate",GTK_SIGNAL_FUNC(spizw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk->nomdok.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMDOK]),(gpointer)E_NOMDOK);


sprintf(strsql,"%s",gettext("Количество изделий"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIZ]),label, FALSE, FALSE, 0);

data.entry[E_KOLIZ] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIZ]), data.entry[E_KOLIZ], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOLIZ]), "activate",GTK_SIGNAL_FUNC(spizw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIZ]),data.rk->koliz.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOLIZ]),(gpointer)E_KOLIZ);

sprintf(strsql,"%s",gettext("Код операции"));
data.knopka_enter[E_KODOP]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.knopka_enter[E_KODOP], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODOP]),"clicked",GTK_SIGNAL_FUNC(spizw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODOP]),(gpointer)E_KODOP);

data.entry[E_KODOP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODOP]), "activate",GTK_SIGNAL_FUNC(spizw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.rk->kodop.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODOP]),(gpointer)E_KODOP);


GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Расчет"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчет"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(spizw_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(spizw_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(spizw_v_knopka),&data);
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
void  spizw_v_e_knopka(GtkWidget *widget,class spizw_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case E_DATAS:

    if(iceb_calendar(&data->rk->datas,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAS]),data->rk->datas.ravno());
      
    return;  


  case E_NOMDOK:
   spizw_m_nomdok(data);
   return;  

  case E_KODOP:
   if(iceb_l_opmur(1,&kod,&naim,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),iceb_u_toutf(kod.ravno()));
    
   return;  


  case E_SKLAD:
    iceb_vibrek(1,"Sklad",&data->rk->sklad,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk->sklad.ravno_toutf());
    return;


 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   spizw_v_key_press(GtkWidget *widget,GdkEventKey *event,class spizw_data *data)
{

//printf("spizw_v_key_press\n");
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
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  spizw_v_knopka(GtkWidget *widget,class spizw_data *data)
{
char strsql[512];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(data->rk->datas.prov_dat() != 0)
      return;

    if(data->rk->sklad.ravno_atoi() == 0)
     {
      iceb_menu_soob(gettext("Не введён склад !"),data->window);
      return;
     }

    sprintf(strsql,"select kod from Sklad where kod=%d",data->rk->sklad.ravno_atoi());
    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код склада"),data->rk->sklad.ravno());
      iceb_menu_soob(strsql,data->window);
      return;
     }

    if(data->rk->nomdok.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введён номер документа !"),data->window);
      return;
     }
    if(data->rk->koliz.ravno_atof() == 0.)
     {
      iceb_menu_soob(gettext("Не введёно количество !"),data->window);
      return;
     }

    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
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

void    spizw_v_vvod(GtkWidget *widget,class spizw_data *data)
{
//short d,m,g;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("spizw_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAS:
    data->rk->datas.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


  case E_SKLAD:
    data->rk->sklad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOMDOK:
    data->rk->nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;



  case E_KOLIZ:
    data->rk->koliz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Автоматическая установка номера документа*/
/******************************/
void spizw_m_nomdok(class spizw_data *data)
{
short d,m,g;
if(data->rk->sklad.ravno_atoi() == 0)
 {
  iceb_menu_soob(gettext("Не введён код склада !"),data->window);
  return;
 }


if(iceb_u_rsdat(&d,&m,&g,data->rk->datas.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Не правільно введена дата !"),data->window);
  return;
 }

/*Автоматическая установка номера накладной*/

iceb_nomnak(g,data->rk->sklad.ravno(),&data->rk->nomdok,2,0,0,data->window);

gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk->nomdok.ravno_toutf());

}
