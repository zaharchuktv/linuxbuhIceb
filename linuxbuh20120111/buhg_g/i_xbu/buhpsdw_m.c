/*$Id: buhpsdw_m.c,v 1.7 2011-02-21 07:35:51 sasa Exp $*/
/*11.09.2008	11.09.2008	Белых А.И.	buhpsdw_m.c
Проверка согласованности данных в подсистемах
*/
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "buhpsdw.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAN,
  E_DATAK,
  E_SHET,
  E_PODSYS,
  KOLENTER  
 };

class buhpsdw_data
 {
  public:
  buhpsdw_rr  *rek_r;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  
  short kl_shift;
  short voz;      //0-начать расчет 1 нет

  buhpsdw_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rek_r->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN]))));
    rek_r->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK]))));
    rek_r->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET]))));
    rek_r->podsys.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PODSYS]))));
   }

  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    rek_r->clear_data();
   }
 };

gboolean   buhpsdw_v_key_press(GtkWidget *widget,GdkEventKey *event,class buhpsdw_data *data);
void    buhpsdw_v_vvod(GtkWidget *widget,class buhpsdw_data *data);
void  buhpsdw_v_knopka(GtkWidget *widget,class buhpsdw_data *data);
void buhpsdw_r(class buhpsdw_rr *datap);
void  buhpsdw_v_e_knopka(GtkWidget *widget,class buhpsdw_data *data);

int buhpsdw_vp(class iceb_u_str *podsys,GtkWidget *wpredok);

extern char *name_system;
extern char *organ;

int buhpsdw_m(class buhpsdw_rr *rek)
{

char strsql[512];

if(rek->datan.getdlinna() == 0)
 {
  rek->clear_data();
  time_t vrem;
  time(&vrem);
  struct tm *bf;
  bf=localtime(&vrem);

  sprintf(strsql,"%d.%d.%d",
  1,1,bf->tm_year+1900);
  rek->datan.new_plus(strsql);

  sprintf(strsql,"%d.%d.%d",
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);
  rek->datak.new_plus(strsql);
 }


buhpsdw_data data;

data.rek_r=rek;

 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Проверка согласованности данных в подсистемах"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(buhpsdw_v_key_press),&data);

GtkWidget *label=gtk_label_new(gettext("Проверка согласованности данных в подсистемах"));

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

GtkTooltips *tooltips_enter[KOLENTER];


sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(buhpsdw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);
tooltips_enter[E_DATAN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAN],data.knopka_enter[E_DATAN],gettext("Выбор даты начала отчёта"),NULL);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(buhpsdw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rek_r->datan.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);

sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(buhpsdw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);
tooltips_enter[E_DATAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAK],data.knopka_enter[E_DATAK],gettext("Выбор даты конца отчёта"),NULL);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(buhpsdw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rek_r->datak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);

sprintf(strsql,"%s (,,)",gettext("Счет"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(buhpsdw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(buhpsdw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rek_r->shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

sprintf(strsql,"%s",gettext("Подсистема"));
data.knopka_enter[E_PODSYS]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_PODSYS]), data.knopka_enter[E_PODSYS], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODSYS]),"clicked",GTK_SIGNAL_FUNC(buhpsdw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODSYS]),(gpointer)E_PODSYS);
tooltips_enter[E_PODSYS]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_PODSYS],data.knopka_enter[E_PODSYS],gettext("Выбор группы контрагента"),NULL);

data.entry[E_PODSYS] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODSYS]), data.entry[E_PODSYS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODSYS]), "activate",GTK_SIGNAL_FUNC(buhpsdw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODSYS]),data.rek_r->podsys.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODSYS]),(gpointer)E_PODSYS);



GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Расчет"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчет"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(buhpsdw_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(buhpsdw_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(buhpsdw_v_knopka),&data);
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
void  buhpsdw_v_e_knopka(GtkWidget *widget,class buhpsdw_data *data)
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

    iceb_vibrek(0,"Plansh",&data->rek_r->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rek_r->shet.ravno_toutf());

    return;  

  case E_PODSYS:

    buhpsdw_vp(&data->rek_r->podsys,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODSYS]),data->rek_r->podsys.ravno_toutf());
    return;
 }
}


/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   buhpsdw_v_key_press(GtkWidget *widget,GdkEventKey *event,class buhpsdw_data *data)
{

//printf("buhpsdw_v_key_press\n");
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
void  buhpsdw_v_knopka(GtkWidget *widget,class buhpsdw_data *data)
{
short dn,mn,gn;
short dk,mk,gk;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(iceb_rsdatp(&dn,&mn,&gn,data->rek_r->datan.ravno(),&dk,&mk,&gk,data->rek_r->datak.ravno(),data->window) != 0)
      return;

    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    buhpsdw_v_vvod(GtkWidget *widget,class buhpsdw_data *data)
{

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("buhpsdw_v_vvod enter=%d\n",enter);

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

  case E_PODSYS:
    data->rek_r->podsys.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/**********************************/
/*Выбор подсистем*/
/***********************************/
int buhpsdw_vp(class iceb_u_str *podsys,GtkWidget *wpredok)
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(organ,40);
zagolovok.ps_plus(gettext("Выберите нужное"));

char bros[500];

sprintf(bros,"%s %s",gettext("МУ"),gettext("Материальный учёт"));
punkt_m.plus(bros);//0


sprintf(bros,"%s %s",gettext("УОС"),gettext("Учёт основных средств"));
punkt_m.plus(bros);//1

int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);
switch(nomer)
 {
  case 0:
   podsys->new_plus(gettext("МУ"));
   return(0);  
  case 1:
   podsys->new_plus(gettext("УОС"));
   return(0);  
 }
return(1);
}
