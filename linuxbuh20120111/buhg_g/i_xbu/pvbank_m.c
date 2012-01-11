/*$Id: pvbank_m.c,v 1.15 2011-02-21 07:35:56 sasa Exp $*/
/*17.06.2006	07.04.2004	Белых А.И.	pvbank_m.c
Меню ввода реквизитов для расчета перечисления на карт-счета
*/
#include <stdlib.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "pvbank.h"

enum
{
  FK2,
  FK3,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_SHET,
  E_DATAN,
  E_DATAK,
  E_KONTR,
  KOLENTER  
 };

class pvbank_data
 {
  public:
  class pvbank_rr  *rek_r;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет
  GtkWidget *radiobutton[2];

  pvbank_data() //Конструктор
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
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }
 };

gboolean   pvbank_v_key_press(GtkWidget *widget,GdkEventKey *event,class pvbank_data *data);
void    pvbank_v_vvod(GtkWidget *widget,class pvbank_data *data);
void  pvbank_v_knopka(GtkWidget *widget,class pvbank_data *data);
void   pvbank_rekviz(class pvbank_data *data);
void  pvbank_v_e_knopka(GtkWidget *widget,class pvbank_data *data);
void       pvbank_radio0(GtkWidget *,class pvbank_data *);
void       pvbank_radio1(GtkWidget *,class pvbank_data *);

extern char *name_system;
extern SQL_baza bd;

int   pvbank_m(class pvbank_rr *rek_ras)
{
pvbank_data data;

data.rek_r=rek_ras;
char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать документы для перечисления на карт-счета"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(pvbank_v_key_press),&data);

GtkWidget *label=gtk_label_new(gettext("Распечатать документы для перечисления на карт-счета"));

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *hboxradio = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxradio);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

//Вставляем радиокнопки
GSList *group;

data.radiobutton[0]=gtk_radio_button_new_with_label(NULL,gettext("Дебет"));
if(data.rek_r->metka_r == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[0]),TRUE); //Устанавливем активной кнопку
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton[0]), "clicked",GTK_SIGNAL_FUNC(pvbank_radio0),&data);
//gtk_object_set_user_data(GTK_OBJECT(data.radiobutton0),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton[0]));

data.radiobutton[1]=gtk_radio_button_new_with_label(group,gettext("Кредит"));
if(data.rek_r->metka_r == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[1]),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data.radiobutton[1]), "clicked",GTK_SIGNAL_FUNC(pvbank_radio1),&data);
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[1], TRUE, TRUE, 0);

GtkTooltips *tooltips_enter[KOLENTER];

data.knopka_enter[E_SHET]=gtk_button_new_with_label(gettext("Счет"));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(pvbank_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(pvbank_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rek_r->shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(pvbank_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);
tooltips_enter[E_DATAN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAN],data.knopka_enter[E_DATAN],gettext("Выбор даты начала отчёта"),NULL);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(pvbank_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rek_r->datan.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);

sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(pvbank_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);
tooltips_enter[E_DATAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAK],data.knopka_enter[E_DATAK],gettext("Выбор даты конца отчёта"),NULL);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(pvbank_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rek_r->datak.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);

sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KONTR]),"clicked",GTK_SIGNAL_FUNC(pvbank_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KONTR]),(gpointer)E_KONTR);
tooltips_enter[E_KONTR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KONTR],data.knopka_enter[E_KONTR],gettext("Выбор контрагента"),NULL);

data.entry[E_KONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KONTR]), "activate",GTK_SIGNAL_FUNC(pvbank_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rek_r->kontr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KONTR]),(gpointer)E_KONTR);


GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Расчет"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчет"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(pvbank_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Реквизиты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]),"clicked",GTK_SIGNAL_FUNC(pvbank_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(pvbank_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(pvbank_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);

}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  pvbank_radio0(GtkWidget *widget,class pvbank_data *data)
{
//g_print("pvbank_radio0\n");

if(data->rek_r->metka_r == 0)
  return;
data->rek_r->metka_r=0;

}
void  pvbank_radio1(GtkWidget *widget,class pvbank_data *data)
{
//g_print("pvbank_radio1\n");
if(data->rek_r->metka_r == 1)
  return;
data->rek_r->metka_r=1;
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  pvbank_v_e_knopka(GtkWidget *widget,class pvbank_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rek_r->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rek_r->datan.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->rek_r->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rek_r->datak.ravno());
      
    return;  
   
  case E_SHET:

    iceb_vibrek(1,"Plansh",&data->rek_r->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rek_r->shet.ravno_toutf());

    return;  

  case E_KONTR:

    iceb_vibrek(0,"Kontragent",&data->rek_r->kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rek_r->kontr.ravno_toutf());

    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   pvbank_v_key_press(GtkWidget *widget,GdkEventKey *event,class pvbank_data *data)
{

//printf("pvbank_v_key_press\n");
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
void  pvbank_v_knopka(GtkWidget *widget,class pvbank_data *data)
{
char  strsql[512];
char  bros[512];
SQLCURSOR cur;
SQL_str   row;
iceb_u_str kod;
iceb_u_str naikod;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(iceb_rsdatp_str(&data->rek_r->datan,&data->rek_r->datak,data->window) != 0)
      return;

    if(data->rek_r->shet.getdlinna() <= 1)
     {
      iceb_u_str repl;
      repl.plus(gettext("Не введён счет !"));
      iceb_menu_soob(&repl,data->window);
      return;
     }

    sprintf(strsql,"select nais,vids,saldo,stat from Plansh where ns='%s'",data->rek_r->shet.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
     {
      iceb_u_str repl;
      sprintf(bros,gettext("Нет счета %s в плане счетов !"),data->rek_r->shet.ravno());
      repl.plus(bros);
//      repl.ps_plus(strsql);
      iceb_menu_soob(&repl,data->window);
      return;
      
     }

    data->rek_r->naimshet.new_plus(row[0]);            
    data->rek_r->vds=atoi(row[1]);
    data->rek_r->saldo=atoi(row[2]);
    data->rek_r->stat=atoi(row[3]);
        
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case FK3:
    pvbank_rekviz(data);

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

void    pvbank_v_vvod(GtkWidget *widget,class pvbank_data *data)
{
//short d,m,g;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("pvbank_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rek_r->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAK:
    data->rek_r->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHET:
    data->rek_r->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KONTR:
    data->rek_r->kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
void   pvbank_rekviz(class pvbank_data *data)
{
short nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Реквизиты"));

zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Просмотр списка счетов"));
punkt_m.plus(gettext("Просмотр общего списка контрагентов"));

nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);



iceb_u_str kod;
iceb_u_str naikod;

switch (nomer)
 {
  case -1:
    return;
    
  case 0:
    iceb_vibrek(1,"Plansh",&data->rek_r->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),iceb_u_toutf(data->rek_r->shet.ravno()));
    break;

  case 1:
    iceb_vibrek(1,"Kontragent",&data->rek_r->kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rek_r->kontr.ravno());
    break;

 }

}
