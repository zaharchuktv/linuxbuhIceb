/*$Id: vrshet_v.c,v 1.47 2011-02-21 07:36:21 sasa Exp $*/
/*17.04.2007	22.02.2004	Белых А.И.	vrshet_v.c
Выписка ресторанного счета
Если вернули 0-записали
             1-нет
*/
#include <stdlib.h>
#include <unistd.h>
#include "i_rest.h"

enum
{
  FK2,
  FK10,
  KOL_F_KL
};
 
enum
 {
  E_NOMST,
  E_KODKL,
  E_FAMIL,
  E_KOMENT,
  E_PODR,
  E_DATAZ, //ДАТА ПРедварительного заказа
  E_VREMZ, //Вемя ПРедварительного заказа
  KOLENTER  
 };

class vrshet_rekh
 {
  public:

  iceb_u_str nomdok;
  short      d,m,g;
  short      metka; //0-новая запись 1-корректировка

  iceb_u_str nomstol;
  iceb_u_str kodkl;
  iceb_u_str famil;
  iceb_u_str koment;
  iceb_u_str podr;
  iceb_u_str dataz;
  iceb_u_str vremz;
  float proc_sk; //Процент скидки      
  vrshet_rekh()  //Конструктор
   {
    clear_date();
   }

  void clear_date()
   {
    nomdok.new_plus("");
    metka=d=m=g=0;

    nomstol.new_plus("");
    kodkl.new_plus("");
    famil.new_plus("");
    koment.new_plus("");
    podr.new_plus("");  

    time_t vrem;
    time(&vrem);
    struct tm *bf;
    bf=localtime(&vrem);

    dataz.new_plus(bf->tm_mday);
    dataz.plus(".");
    dataz.plus(bf->tm_mon+1);
    dataz.plus(".");
    dataz.plus(bf->tm_year+1900);

    vremz.new_plus(bf->tm_hour);
    vremz.plus(":");
    vremz.plus(bf->tm_min);
    vremz.plus(":");
    vremz.plus(bf->tm_sec);
    
   }
 };


class vrshet_v_data
 {
  public:
  vrshet_rekh  rekh;
  
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать проверку 1 нет

  guint     timer;
  
  vrshet_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    timer=0;
   }
};

void  vrshet_v_knopka(GtkWidget *widget,class vrshet_v_data *data);
void  vrshet_v_e_knopka(GtkWidget *widget,class vrshet_v_data *data);
gboolean   vrshet_v_key_press(GtkWidget *widget,GdkEventKey *event,class vrshet_v_data *data);
void    vrshet_v_vvod(GtkWidget *widget,class vrshet_v_data *data);
int  vrshet_zaph(class vrshet_rekh *data,GtkWidget *wpredok);


extern int kodpodr; //Код подразделения
extern char *name_system;
extern SQL_baza	bd;
extern int kodpodr;
extern iceb_u_str naimpodr;
extern char *imabaz;
extern short metka_crk; //0-расчёт клиентов по подразделениям 1-централизованный
extern uid_t  kod_operatora;
extern class iceb_get_dev_data config_dev;

int  vrshet_v(short *god,iceb_u_str *nomdok,GtkWidget *wpredok)
{
vrshet_v_data data;
char strsql[300];


if(*god > 0)
 {
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select * from Restdok where god=%d and nomd='%s'",
  *god,nomdok->ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  
  data.rekh.nomdok.new_plus(row[1]);
  iceb_u_rsdat(&data.rekh.d,&data.rekh.m,&data.rekh.g,row[2],2);  
   
  data.rekh.nomstol.new_plus(row[4]);
  data.rekh.kodkl.new_plus(row[5]);
  data.rekh.famil.new_plus(row[6]);
  data.rekh.koment.new_plus(row[7]);
  data.rekh.metka=1;
 }


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
if(config_dev.metka_screen == 1)
  gtk_widget_set_usize(data.window,-1,600);
if(wpredok != NULL)
 {
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

if(*god == 0)
  sprintf(strsql,"%s %s",name_system,gettext("Ввод нового счета"));
else
  sprintf(strsql,"%s %s",name_system,gettext("Корректировка шапки счета"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(vrshet_v_key_press),&data);

time_t vrem;
struct tm *bf;
time(&vrem);

bf=localtime(&vrem);
GtkWidget *label;

if(*god == 0)
 {
  iceb_u_str nadpis;
  nadpis.plus(gettext("Ввод нового счета"));
  sprintf(strsql,"%s %d.%d.%d",gettext("Текущая дата"),
  bf->tm_mday,
  bf->tm_mon+1,
  bf->tm_year+1900);
  nadpis.ps_plus(strsql);
    
  label=gtk_label_new(nadpis.ravno_toutf());
 }
else
 label=gtk_label_new(gettext("Корректировка шапки счета"));


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

data.knopka_enter[E_NOMST]=gtk_button_new_with_label(gettext("Номер столика"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMST]), data.knopka_enter[E_NOMST], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_NOMST]),"clicked",GTK_SIGNAL_FUNC(vrshet_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_NOMST]),(gpointer)E_NOMST);

data.entry[E_NOMST] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMST]), data.entry[E_NOMST], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMST]), "activate",GTK_SIGNAL_FUNC(vrshet_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMST]),data.rekh.nomstol.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMST]),(gpointer)E_NOMST);

data.knopka_enter[E_KODKL]=gtk_button_new_with_label(gettext("Код клиента"));
gtk_box_pack_start (GTK_BOX (hbox[E_KODKL]), data.knopka_enter[E_KODKL], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODKL]),"clicked",GTK_SIGNAL_FUNC(vrshet_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODKL]),(gpointer)E_KODKL);

data.entry[E_KODKL] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KODKL]), data.entry[E_KODKL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODKL]), "activate",GTK_SIGNAL_FUNC(vrshet_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODKL]),data.rekh.kodkl.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODKL]),(gpointer)E_KODKL);

#ifdef READ_CARD
extern tdcon           td_server; 
if(td_server >= 0)
 {
  class iceb_read_card_enter_data card;
  card.window=data.window;
  card.entry=data.entry[E_KODKL];
  data.timer=card.timer=gtk_timeout_add(500,(GtkFunction)iceb_read_card_enter,&card);
 }
#endif


data.knopka_enter[E_FAMIL]=gtk_button_new_with_label(gettext("Фамилия клиента"));
gtk_box_pack_start (GTK_BOX (hbox[E_FAMIL]), data.knopka_enter[E_FAMIL], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_FAMIL]),"clicked",GTK_SIGNAL_FUNC(vrshet_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_FAMIL]),(gpointer)E_FAMIL);


data.entry[E_FAMIL] = gtk_entry_new_with_max_length (60);
gtk_box_pack_start (GTK_BOX (hbox[E_FAMIL]), data.entry[E_FAMIL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_FAMIL]), "activate",GTK_SIGNAL_FUNC(vrshet_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FAMIL]),data.rekh.famil.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_FAMIL]),(gpointer)E_FAMIL);

data.knopka_enter[E_KOMENT]=gtk_button_new_with_label(gettext("Комментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.knopka_enter[E_KOMENT], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOMENT]),"clicked",GTK_SIGNAL_FUNC(vrshet_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOMENT]),(gpointer)E_KOMENT);

data.entry[E_KOMENT] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(vrshet_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.rekh.koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);

data.knopka_enter[E_PODR]=gtk_button_new_with_label(gettext("Подразделение"));
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODR]),"clicked",GTK_SIGNAL_FUNC(vrshet_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODR]),(gpointer)E_PODR);

data.entry[E_PODR] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODR]), "activate",GTK_SIGNAL_FUNC(vrshet_v_vvod),&data);

if(kodpodr != 0)
 {
  sprintf(strsql,"%d %s",kodpodr,naimpodr.ravno());
  data.rekh.podr.new_plus(strsql);
 }
 
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rekh.podr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODR]),(gpointer)E_PODR);

data.knopka_enter[E_DATAZ]=gtk_button_new_with_label(gettext("Дата заказа"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAZ]), data.knopka_enter[E_DATAZ], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAZ]),"clicked",GTK_SIGNAL_FUNC(vrshet_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAZ]),(gpointer)E_DATAZ);

//label=gtk_label_new(gettext("Дата заказа"));
data.entry[E_DATAZ] = gtk_entry_new_with_max_length (10);
//gtk_box_pack_start (GTK_BOX (hbox[nomhbox]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAZ]), data.entry[E_DATAZ], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAZ]), "activate",GTK_SIGNAL_FUNC(vrshet_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAZ]),data.rekh.dataz.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAZ]),(gpointer)E_DATAZ);

data.knopka_enter[E_VREMZ]=gtk_button_new_with_label(gettext("Время заказа"));
gtk_box_pack_start (GTK_BOX (hbox[E_VREMZ]), data.knopka_enter[E_VREMZ], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_VREMZ]),"clicked",GTK_SIGNAL_FUNC(vrshet_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_VREMZ]),(gpointer)E_VREMZ);

data.entry[E_VREMZ] = gtk_entry_new_with_max_length (8);
gtk_box_pack_start (GTK_BOX (hbox[E_VREMZ]), data.entry[E_VREMZ], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VREMZ]), "activate",GTK_SIGNAL_FUNC(vrshet_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VREMZ]),data.rekh.vremz.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VREMZ]),(gpointer)E_VREMZ);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(vrshet_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введенной в меню информации"),NULL);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(vrshet_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

//gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));

gtk_main();

*god=data.rekh.g;
nomdok->new_plus(data.rekh.nomdok.ravno());
return(data.voz);
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vrshet_v_key_press(GtkWidget *widget,GdkEventKey *event,class vrshet_v_data *data)
{

//printf("vrshet_v_key_press\n");
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
/*Обработчик нажатия F кнопок  */
/*****************************/
void  vrshet_v_knopka(GtkWidget *widget,class vrshet_v_data *data)
{
char strsql[300];
iceb_u_str shet;
iceb_u_str naim;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

iceb_sql_flag flagg;
switch ((gint)knop)
 {
  case FK2:
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(data->entry[i]),"activate");
    iceb_refresh();
    printf("kodkl=%s\n",data->rekh.kodkl.ravno());    

    memset(strsql,'\0',sizeof(strsql));
    if(data->rekh.kodkl.getdlinna() > 1)
     {
      //Проверяем есть код клиента в списке клиентов
      sprintf(strsql,"select kod from Taxiklient where kod='%s'",data->rekh.kodkl.ravno());
      if(iceb_sql_readkey(strsql,data->window) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден код клиента"),data->rekh.kodkl.ravno());
        iceb_menu_soob(strsql,data->window);
        return;
        
       }
     }
    if(data->rekh.metka == 0 && data->rekh.kodkl.getdlinna() > 1)
      if(rest_flag_kl(&flagg,data->rekh.kodkl.ravno(),data->window) != 0)
       return;
      
    if(vrshet_zaph(&data->rekh,data->window) != 0)
     return;  
      

    if(data->timer != 0)
        gtk_timeout_remove(data->timer);
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
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  vrshet_v_e_knopka(GtkWidget *widget,class vrshet_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case E_NOMST:

    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_NOMST]),"activate");

    iceb_mous_klav(gettext("Номер столика"),&data->rekh.nomstol,10,1,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMST]),data->rekh.nomstol.ravno_toutf());

    return;  

  case E_KODKL:

    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_KODKL]),"activate");

    iceb_mous_klav(gettext("Код клиента"),&data->rekh.kodkl,20,1,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODKL]),data->rekh.kodkl.ravno_toutf());

    return;  

  case E_FAMIL:

    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_FAMIL]),"activate");

    iceb_mous_klav(gettext("Фамилия клиента"),&data->rekh.famil,60,1,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_FAMIL]),data->rekh.famil.ravno_toutf());

    return;  

  case E_KOMENT:

    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_KOMENT]),"activate");

    iceb_mous_klav(gettext("Коментарий"),&data->rekh.koment,99,1,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOMENT]),data->rekh.koment.ravno_toutf());

    return;  

  case E_PODR:

    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_PODR]),"activate");

    iceb_mous_klav(gettext("Подразделение"),&data->rekh.podr,10,0,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rekh.podr.ravno_toutf());

    return;  

  case E_DATAZ:

    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_DATAZ]),"activate");

    iceb_mous_klav(gettext("Дата заказа"),&data->rekh.dataz,10,0,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAZ]),data->rekh.dataz.ravno_toutf());

    return;  

  case E_VREMZ:

    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_VREMZ]),"activate");

    iceb_mous_klav(gettext("Время заказа"),&data->rekh.vremz,8,0,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VREMZ]),data->rekh.vremz.ravno_toutf());

    return;  

 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    vrshet_v_vvod(GtkWidget *widget,class vrshet_v_data *data)
{
char strsql[400];
SQLCURSOR cur;
SQL_str   row;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("vrshet_v_vvod enter=%d\n",enter);

switch (enter)
 {

  case E_NOMST:
    data->rekh.nomstol.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KODKL:
    data->rekh.kodkl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(data->rekh.kodkl.getdlinna() > 1)
//     if(data->rekh.famil.getdlinna() <= 1)
      {
       sprintf(strsql,"select fio from Taxiklient where kod='%s'",data->rekh.kodkl.ravno());
       if(sql_readkey(&bd,strsql,&row,&cur) == 1)
        {
         data->rekh.famil.new_plus(row[0]);
         gtk_entry_set_text(GTK_ENTRY(data->entry[E_FAMIL]),data->rekh.famil.ravno_toutf());
        }
      }
    break;

  case E_FAMIL:
    data->rekh.famil.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOMENT:
    data->rekh.koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAZ:
    data->rekh.dataz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_VREMZ:
    data->rekh.vremz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_PODR:
    data->rekh.podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    sprintf(strsql,"select naik from Restpod where kod=%d",
    atoi(data->rekh.podr.ravno()));
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      kodpodr=atoi(data->rekh.podr.ravno());
      naimpodr.new_plus(row[0]);
      sprintf(strsql,"%d %s",kodpodr,naimpodr.ravno());
      data->rekh.podr.new_plus(strsql);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rekh.podr.ravno_toutf());
     }
     
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/*****************************/
/*Запись шапки дркумента*/
/***************************/
int  vrshet_zaph(class vrshet_rekh *data,GtkWidget *wpredok)
{
char strsql[300];
time_t    vrem;
short     dd,md,gd;
short     has,min,sek;
printf("vrshet_zaph\n");

if(metka_crk == 1 && data->kodkl.getdlinna() <= 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не введён код клиента !"));
  iceb_menu_soob(&repl,NULL);
  return(1);

 }

if(iceb_u_rsdat(&dd,&md,&gd,data->dataz.ravno(),1) != 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не верно введена дата !"));
  iceb_menu_soob(&repl,NULL);
  return(1);
 }

if(iceb_u_rstime(&has,&min,&sek,data->vremz.ravno()) != 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Неверно введено время !"));
  iceb_menu_soob(&repl,NULL);
  return(1);
 }

sprintf(strsql,"select naik from Restpod where kod=%d",
data->podr.ravno_atoi());

if(iceb_sql_readkey(strsql,NULL) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не верно введён код подразделения !"));
  iceb_menu_soob(&repl,NULL);
  return(1);
 }

float proc_sk=0.; //Процент скидки      
char dvk[30]; //Дата возврата карточки
memset(dvk,'\0',sizeof(dvk));

if(data->kodkl.getdlinna() > 1)
 {
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select ps,dvk from Taxiklient where kod='%s'",data->kodkl.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найден код клиента"));
    repl.plus(" ");
    repl.plus(data->kodkl.ravno());
    repl.plus(" !");
    iceb_menu_soob(&repl,NULL);
    return(1);
   }
  proc_sk=atof(row[0]);
  strncpy(dvk,row[1],sizeof(dvk)-1);
 }

//Проверяем может уже есть неоплаченный заказ
if(data->kodkl.getdlinna() > 1)
 {
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select god,nomd from Restdok where kodkl='%s' and mo=0 and vrem >= %ld order by datd desc limit 1",
  data->kodkl.ravno(),iceb_u_datetime_sec(dvk));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) > 0)
   {
    data->g=atoi(row[0]);
    data->nomdok.new_plus(row[1]);
    printf("Неоплаченный заказ, ныряем в него.\n");
    return(0);  
   }
 }
struct  tm      bf1;

bf1.tm_mday=(int)dd;
bf1.tm_mon=(int)(md-1);
bf1.tm_year=(int)(gd-1900);

bf1.tm_hour=(int)has;
bf1.tm_min=(int)min;
bf1.tm_sec=sek;

time_t vremz=mktime(&bf1); //дата и время заказа


if(data->metka == 0) //Ввод новой записи
 {

  iceb_u_poltekdat(&data->d,&data->m,&data->g);

  int voz=zap_rest_dok(data->d,data->m,data->g,&data->nomdok, data->nomstol.ravno(), data->kodkl.ravno(),
  data->famil.ravno(),data->koment.ravno(), kod_operatora,data->podr.ravno_atoi(),
  vremz, proc_sk);
  printf("Записали шапку voz=%d\n",voz);  
  return(voz);
/************
  //номер документа получаем из подпрограммы
  return(zap_rest_dok(data->d,data->m,data->g,&data->nomdok, data->nomstol.ravno(), data->kodkl.ravno(),
  data->famil.ravno(),data->koment.ravno(), kod_operatora,data->podr.ravno_atoi(),
  vremz, proc_sk));
*************/
 }

if(data->metka == 1)//Корректировка записи
 {
  time(&vrem);

  sprintf(strsql,"update Restdok set \
nomst='%s',\
kodkl='%s',\
fio='%s',\
koment='%s',\
ktoi=%d,\
vrem=%ld,\
podr=%d,\
vremz=%ld,\
ps=%.2f \
where god=%d and nomd='%s'",
  data->nomstol.ravno(),
  data->kodkl.ravno(),
  data->famil.ravno(),
  data->koment.ravno(),
  kod_operatora,
  vrem,
  data->podr.ravno_atoi(),
  vremz,
  proc_sk,
  data->g,data->nomdok.ravno());

  if(iceb_sql_zapis(strsql,0,0,wpredok) != 0)
   return(1);
    
 }
 
 
return(0);

}







