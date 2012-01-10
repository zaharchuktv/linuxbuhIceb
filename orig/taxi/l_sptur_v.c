/*$Id: l_sptur_v.c,v 1.10 2011-02-21 07:36:20 sasa Exp $*/
/*07.01.2007	18.07.2005	Белых А.И.	l_sptur_v.c
Ввод и корректировка записей в список турникетов
*/
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "i_rest.h"

enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_NOMER,
  E_PODR,
  E_KOMENT,
  KOLENTER  
 };

class l_sptur_v_data
 {
  public:
  //Реквизиты записи
  iceb_u_str nomer;
  iceb_u_str podr;
  iceb_u_str koment;    



  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *label_naimpodr;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  
  int nomer_k; //номер записи которую корректирують

  l_sptur_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_rek_zap();
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }
  void clear_rek_zap()
   {
    nomer.new_plus("");
    podr.new_plus("");
    koment.new_plus("");
   }
 };

gboolean   l_sptur_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_sptur_v_data *data);
void    l_sptur_v_v_vvod(GtkWidget *widget,class l_sptur_v_data *data);
void  l_sptur_v_v_knopka(GtkWidget *widget,class l_sptur_v_data *data);
void uslopp_kkvt(char *skod,char *nkod,GtkWidget *wpredok);
int uslopp_zap(class l_sptur_v_data *data);
int uslopp_pk(int nomer,GtkWidget *wpredok);

void  sptur_v_e_knopka(GtkWidget *widget,class l_sptur_v_data *data);

extern char *name_system;
extern SQL_baza bd;
extern char	*shrt;    /*Счета розничной торговли*/
extern double	okrcn;  /*Округление цены*/

int l_sptur_v(iceb_u_str *nomer,GtkWidget *wpredok)
{
l_sptur_v_data data;
iceb_u_str naim_podr;
naim_podr.plus("");
char strsql[300];
iceb_u_str kikz;

data.nomer_k=nomer->ravno_atoi();

if(data.nomer_k != 0)
 {
  //читаем корректируемую запись
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select * from Resst where nm=%d",data.nomer_k);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  data.nomer.new_plus(row[0]);
  data.podr.new_plus(row[1]);
  data.koment.new_plus(row[2]);
  
  kikz.plus(iceb_kikz(row[3],row[4],wpredok));

  //Узнаём наименование подразделения
  sprintf(strsql,"select naik from Restpod where kod=%d",data.podr.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim_podr.new_plus(row[0]);
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.nomer_k != 0)
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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_sptur_v_v_key_press),&data);

iceb_u_str zagolov;
if(data.nomer_k != 0)
 {
  zagolov.new_plus(gettext("Корректировка записи"));
  zagolov.ps_plus(kikz.ravno());
 }
else
  zagolov.new_plus(gettext("Ввод новой записи"));

GtkWidget *label=gtk_label_new(zagolov.ravno_toutf());

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 1);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], TRUE, TRUE, 1);

gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 1);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Номер турникета"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_NOMER] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER]), data.entry[E_NOMER],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMER]), "activate",GTK_SIGNAL_FUNC(l_sptur_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMER]),data.nomer.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMER]),(gpointer)E_NOMER);

sprintf(strsql,"%s",gettext("Коментарий"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_KOMENT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(l_sptur_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);

sprintf(strsql,"%s",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODR]),"clicked",GTK_SIGNAL_FUNC(sptur_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODR]),(gpointer)E_PODR);
tooltips_enter[E_PODR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_PODR],data.knopka_enter[E_PODR],gettext("Выбор подразделения"),NULL);

data.entry[E_PODR] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODR]), "activate",GTK_SIGNAL_FUNC(l_sptur_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.podr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODR]),(gpointer)E_PODR);

data.label_naimpodr=gtk_label_new(naim_podr.ravno_toutf());
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.label_naimpodr,TRUE, TRUE, 0);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введённой в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_sptur_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_sptur_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 nomer->new_plus(data.nomer.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
 
return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  sptur_v_e_knopka(GtkWidget *widget,class l_sptur_v_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {

  case E_PODR:
    if(rpod_l(1,&kod,&naim,data->window) == 0)
     {
      data->podr.new_plus(kod.ravno());
       gtk_label_set_text(GTK_LABEL(data->label_naimpodr),naim.ravno_toutf(20));
     }    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->podr.ravno_toutf());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_sptur_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_sptur_v_data *data)
{

//printf("l_sptur_v_v_key_press\n");
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
void  l_sptur_v_v_knopka(GtkWidget *widget,class l_sptur_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    if(uslopp_zap(data) != 0)
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

void    l_sptur_v_v_vvod(GtkWidget *widget,class l_sptur_v_data *data)
{
char strsql[300];
SQL_str row;
SQLCURSOR cur;
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_sptur_v_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_NOMER:
    data->nomer.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOMENT:
    data->koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_PODR:
    data->podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    sprintf(strsql,"select naik from Restpod where kod=%d",data->podr.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.20s",row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naimpodr),iceb_u_toutf(strsql));
     }    
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
int uslopp_zap(class l_sptur_v_data *data)
{
char strsql[1000];
SQL_str row;
SQLCURSOR cur;
time_t   vrem;
time(&vrem);

printf("sptur_v_zap\n");

data->read_rek(); //Читаем реквизиты меню
//for(int i=0; i < KOLENTER; i++)
//  gtk_signal_emit_by_name(GTK_OBJECT(data->entry[i]),"activate");

if(data->nomer.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введен номер турникета !"),data->window);
  return(1);
 }

if(data->podr.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введено код подразделения !"),data->window);
  return(1);
 }

//проверяем код подразделения
sprintf(strsql,"select kod from Restpod where kod=%s",data->podr.ravno());  
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код подразделения"),data->podr.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 } 
if(data->nomer_k == 0)
 {

  if(uslopp_pk(data->nomer.ravno_atoi(),data->window) != 0)
   return(1);
  
  sprintf(strsql,"insert into Resst \
values (%d,%d,'%s',%d,%ld)",
   data->nomer.ravno_atoi(),
   data->podr.ravno_atoi(),
   data->koment.ravno_filtr(),
   iceb_getuid(data->window),vrem);
 }
else
 {
  
  if(data->nomer_k != data->nomer.ravno_atoi())
   {
    if(uslopp_pk(data->nomer.ravno_atoi(),data->window) != 0)
     return(1);
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Resst \
set \
nm=%d,\
podr=%d,\
koment='%s',\
ktoi=%d,\
vrem=%ld \
where nm=%d",
   data->nomer.ravno_atoi(),
   data->podr.ravno_atoi(),
   data->koment.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->nomer_k);
 }

//printf("l_sptur_v-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int uslopp_pk(int nomer,GtkWidget *wpredok)
{
char strsql[300];

sprintf(strsql,"select nomer from Resst where nm=%d",nomer);
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
