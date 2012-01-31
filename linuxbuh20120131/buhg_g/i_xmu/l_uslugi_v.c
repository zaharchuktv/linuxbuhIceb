/*$Id: l_uslugi_v.c,v 1.27 2011-02-21 07:35:54 sasa Exp $*/
/*06.04.2009	18.07.2005	Белых А.И.	l_uslugi_v.c
Ввод и корректировка записей в список 
*/
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "l_uslugi.h"

enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KOD,
  E_GRUPA,
  E_NAIM,
  E_EI,
  E_SHET,
  E_CENA,
  E_ARTIKUL,
//  E_NDS,
  KOLENTER  
 };

class l_uslugi_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *radiobutton[2];
  GtkWidget *label_naigr;
  GtkWidget *label_naiei;
  GtkWidget *label_naishet;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class uslugi_rek rk;
  
  iceb_u_str kodk; //Код записи которую корректируют
  
  l_uslugi_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
//   for(int i=0; i < KOLENTER; i++)
//      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }
 };

gboolean   l_uslugi_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uslugi_v_data *data);
void    l_uslugi_v_v_vvod(GtkWidget *widget,class l_uslugi_v_data *data);
void  l_uslugi_v_v_knopka(GtkWidget *widget,class l_uslugi_v_data *data);
void uslugi_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok);
int uslugi_zap(class l_uslugi_v_data *data);
int uslugi_pk(const char *kod,GtkWidget *wpredok);
void  uslugi_v_e_knopka(GtkWidget *widget,class l_uslugi_v_data *data);

void  uslugi_radio0(GtkWidget *widget,class l_uslugi_v_data *data);
void  uslugi_radio1(GtkWidget *widget,class l_uslugi_v_data *data);

extern char *name_system;
extern SQL_baza bd;
extern char	*shrt;    /*Счета розничной торговли*/
extern double	okrcn;  /*Округление цены*/

int l_uslugi_v(iceb_u_str *kodzap, //Код записи для корректировки; возвращаем код введённой записи
GtkWidget *wpredok)
{
l_uslugi_v_data data;
data.kodk.new_plus(kodzap->ravno());

char strsql[512];
iceb_u_str kikz;

if(kodzap->getdlinna() > 1)
 {
  //читаем корректируемую запись
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select * from Uslugi where kodus=%s",kodzap->ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  data.rk.kod.new_plus(row[0]);
  data.rk.grupa.new_plus(row[1]);
  
  data.rk.naim.new_plus(row[2]);
  data.rk.ei.new_plus(row[3]);
  data.rk.shet.new_plus(row[4]);
  data.rk.cena.new_plus(row[5]);
  data.rk.metka_cen=atoi(row[6]);
  kikz.plus(iceb_kikz(row[7],row[8],wpredok));
  data.rk.artikul.new_plus(row[9]);
 }
else
 {
  sprintf(strsql,"%d",nomkmw(wpredok));
  data.rk.kod.new_plus(strsql);
 }
 

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(kodzap->getdlinna() > 1)
 sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));
else
 sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_uslugi_v_v_key_press),&data);

iceb_u_str zagolov;
if(kodzap->getdlinna() <= 1)
 zagolov.new_plus(gettext("Ввод новой записи"));
else
 {
  zagolov.new_plus(gettext("Корректировка записи"));
  zagolov.ps_plus(kikz.ravno());
 }

GtkWidget *label=gtk_label_new(zagolov.ravno_toutf());

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

//Вставляем радиокнопки
GSList *group;

data.radiobutton[0]=gtk_radio_button_new_with_label(NULL,gettext("Цена без НДС"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton[0]), "clicked",GTK_SIGNAL_FUNC(uslugi_radio0),&data);
//gtk_object_set_user_data(GTK_OBJECT(data.radiobutton0),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton[0]));

data.radiobutton[1]=gtk_radio_button_new_with_label(group,gettext("Цена с НДС"));
gtk_signal_connect(GTK_OBJECT(data.radiobutton[1]), "clicked",GTK_SIGNAL_FUNC(uslugi_radio1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton[1], TRUE, TRUE, 0);

if(data.rk.metka_cen == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[1]),TRUE); //Устанавливем активной кнопку


gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Код услуги"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_KOD] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(l_uslugi_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);


sprintf(strsql,"%s",gettext("Код группы"));
data.knopka_enter[E_GRUPA]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.knopka_enter[E_GRUPA], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_GRUPA]),"clicked",GTK_SIGNAL_FUNC(uslugi_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_GRUPA]),(gpointer)E_GRUPA);
tooltips_enter[E_GRUPA]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_GRUPA],data.knopka_enter[E_GRUPA],gettext("Выбор группы"),NULL);

data.entry[E_GRUPA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.entry[E_GRUPA],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GRUPA]), "activate",GTK_SIGNAL_FUNC(l_uslugi_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUPA]),data.rk.grupa.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GRUPA]),(gpointer)E_GRUPA);

data.label_naigr=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.label_naigr,TRUE, TRUE, 0);

sprintf(strsql,"%s",gettext("Наименование услуги"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_NAIM] = gtk_entry_new_with_max_length (79);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(l_uslugi_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);

sprintf(strsql,"%s",gettext("Единица измерения"));
data.knopka_enter[E_EI]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.knopka_enter[E_EI], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_EI]),"clicked",GTK_SIGNAL_FUNC(uslugi_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_EI]),(gpointer)E_EI);
tooltips_enter[E_EI]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_EI],data.knopka_enter[E_EI],gettext("Выбор единицы измерения"),NULL);

data.entry[E_EI] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_EI]), "activate",GTK_SIGNAL_FUNC(l_uslugi_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.rk.ei.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_EI]),(gpointer)E_EI);

data.label_naiei=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.label_naiei,TRUE, TRUE, 0);

sprintf(strsql,"%s",gettext("Цена"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), label, FALSE, FALSE, 0);

data.entry[E_CENA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), data.entry[E_CENA],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_CENA]), "activate",GTK_SIGNAL_FUNC(l_uslugi_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA]),data.rk.cena.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_CENA]),(gpointer)E_CENA);

sprintf(strsql,"%s",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(uslugi_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор единицы измерения"),NULL);

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(l_uslugi_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk.shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

data.label_naishet=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.label_naishet,TRUE, TRUE, 0);

sprintf(strsql,"%s",gettext("Артикул"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_ARTIKUL]), label, FALSE, FALSE, 0);

data.entry[E_ARTIKUL] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_ARTIKUL]), data.entry[E_ARTIKUL],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ARTIKUL]), "activate",GTK_SIGNAL_FUNC(l_uslugi_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ARTIKUL]),data.rk.artikul.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ARTIKUL]),(gpointer)E_ARTIKUL);


GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введённой в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_uslugi_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_uslugi_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

if(kodzap->getdlinna() <= 1)
  gtk_widget_grab_focus(data.entry[1]);
else
  gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 kodzap->new_plus(data.rk.kod.ravno());
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  uslugi_radio0(GtkWidget *widget,class l_uslugi_v_data *data)
{
//g_print("uslugi_radio0\n");

data->rk.metka_cen=0;

}
void  uslugi_radio1(GtkWidget *widget,class l_uslugi_v_data *data)
{
//g_print("uslugi_radio1\n");
data->rk.metka_cen=1;
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  uslugi_v_e_knopka(GtkWidget *widget,class l_uslugi_v_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {

  case E_EI:
    if(iceb_vibrek(1,"Edizmer",&data->rk.ei,&naim,data->window) == 0)
       gtk_label_set_text(GTK_LABEL(data->label_naiei),naim.ravno_toutf(20));
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),data->rk.ei.ravno_toutf());
    return;

  case E_SHET:
    if(iceb_vibrek(1,"Plansh",&data->rk.shet,&naim,data->window) == 0)
       gtk_label_set_text(GTK_LABEL(data->label_naishet),naim.ravno_toutf(20));
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk.shet.ravno_toutf());
    return;

  case E_GRUPA:
    if(l_uslgrup(1,&kod,&naim,data->window) == 0)
     {
      gtk_label_set_text(GTK_LABEL(data->label_naigr),naim.ravno_toutf(20));
      data->rk.grupa.plus(kod.ravno());
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPA]),data->rk.grupa.ravno_toutf());
    return;
 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uslugi_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uslugi_v_data *data)
{

//printf("l_uslugi_v_v_key_press\n");
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
void  l_uslugi_v_v_knopka(GtkWidget *widget,class l_uslugi_v_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(uslugi_zap(data) != 0)
      return;

    gtk_widget_destroy(data->window);
    data->window=NULL;

    data->voz=0;
    return;  

    
  case FK10:
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_uslugi_v_v_vvod(GtkWidget *widget,class l_uslugi_v_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uslugi_v_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->rk.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_GRUPA:
    data->rk.grupa.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    sprintf(strsql,"select naik from Uslgrup where kod=%d",data->rk.grupa.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
       gtk_label_set_text(GTK_LABEL(data->label_naigr),iceb_u_toutf(strsql));
     }    
    break;

  case E_EI:
    data->rk.ei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    sprintf(strsql,"select naik from Edizmer where kod='%s'",data->rk.ei.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naiei),iceb_u_toutf(strsql));
     }    
    break;

  case E_CENA:
    data->rk.cena.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHET:
    data->rk.shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    sprintf(strsql,"select nais from Plansh where ns='%s'",data->rk.shet.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naishet),iceb_u_toutf(strsql));
     }    
    break;

  case E_NAIM:
    data->rk.naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int uslugi_zap(class l_uslugi_v_data *data)
{
char strsql[1024];
//SQL_str row;
time_t   vrem;
time(&vrem);

printf("uslugi_v_zap\n");

data->read_rek(); //Читаем реквизиты меню
//for(int i=0; i < KOLENTER; i++)
//  gtk_signal_emit_by_name(GTK_OBJECT(data->entry[i]),"activate");

if(data->rk.kod.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введён код !"),data->window);
  return(1);
 }

if(data->rk.naim.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введено наименование !"),data->window);
  return(1);
 }
if(data->rk.grupa.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели группу!"),data->window);
  return(1);
 }
short metkakor=0;

if(data->kodk.getdlinna() <= 1)
 {

  if(uslugi_pk(data->rk.kod.ravno(),data->window) != 0)
   return(1);
  
  sprintf(strsql,"insert into Uslugi \
values (%d,%s,'%s','%s','%s',%.7g,%d,%d,%ld,'%s')",
   data->rk.kod.ravno_atoi(),
   data->rk.grupa.ravno(),
   data->rk.naim.ravno_filtr(),
   data->rk.ei.ravno_filtr(),
   data->rk.shet.ravno_filtr(),
   data->rk.cena.ravno_atof(),
   data->rk.metka_cen,
   iceb_getuid(data->window),vrem,
   data->rk.artikul.ravno_filtr());
 }
else
 {
  
  if(iceb_u_SRAV(data->kodk.ravno(),data->rk.kod.ravno(),0) != 0)
   {
    if(uslugi_pk(data->rk.kod.ravno(),data->window) != 0)
     return(1);

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Uslugi \
set \
kodus=%d,\
kodgr=%s,\
naius='%s',\
ei='%s',\
shetu='%s',\
cena=%.7g,\
nds=%d,\
ktoz=%d,\
vrem=%ld,\
art='%s' \
where kodus=%s",
   data->rk.kod.ravno_atoi(),
   data->rk.grupa.ravno(),
   data->rk.naim.ravno_filtr(),
   data->rk.ei.ravno_filtr(),
   data->rk.shet.ravno_filtr(),
   data->rk.cena.ravno_atof(),
   data->rk.metka_cen,
   iceb_getuid(data->window),vrem,
   data->rk.artikul.ravno_filtr(),
   data->kodk.ravno());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);

if(metkakor == 1)
 uslugi_kkvt(data->kodk.ravno(),data->rk.kod.ravno(),data->window);


return(0);

}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void uslugi_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Uslugi set kodus=%s where kodus=%s",nkod,skod);
if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
  return;

sprintf(strsql,"update Usldokum1 set kodzap=%s where kodzap=%s and metka=1",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Usldokum2 set kodzap=%s where kodzap=%s and metka=1",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);


sprintf(strsql,"update Roznica set kodm=%s where kodm=%s and metka=2",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Specif set kodd=%s where kodd=%s and kz=1",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);


}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int uslugi_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kodus from Uslugi where kodus=%s",kod);
//printf("ei_pk-%s\n",strsql);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Такая запись уже есть !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

return(0);

}
