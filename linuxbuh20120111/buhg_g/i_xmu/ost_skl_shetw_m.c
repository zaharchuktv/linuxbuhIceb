/*$Id: ost_skl_shetw_m.c,v 1.8 2011-02-21 07:35:55 sasa Exp $*/
/*17.11.2008	17.11.2008	Белых А.И. ost_skl_shetw_m.c
Меню для ввода данных 
*/
#include <stdlib.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "ost_skl_shetw.h"
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
  E_SKLAD,
  E_GRUPA,
  E_SHET,
  E_KODMAT,
  E_DATA_OST,
  KOLENTER  
 };

class ost_skl_shetw_m_data
 {
  public:
  ost_skl_shetw_data  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет
  
  ost_skl_shetw_m_data() //Конструктор
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
    rk->clear_data();
   }
 };



gboolean   ost_skl_shetw_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class ost_skl_shetw_m_data *data);
void    ost_skl_shetw_m_v_vvod(GtkWidget *widget,class ost_skl_shetw_m_data *data);
void  ost_skl_shetw_m_v_knopka(GtkWidget *widget,class ost_skl_shetw_m_data *data);
void   ost_skl_shetw_m_rekviz(class ost_skl_shetw_m_data *data);
void  ost_skl_shetw_v_e_knopka(GtkWidget *widget,class ost_skl_shetw_m_data *data);


void ost_skl_shetw_sr(class ost_skl_shetw_m_data *data);

extern char *name_system;
extern SQL_baza bd;

int   ost_skl_shetw_m(class ost_skl_shetw_data *rek_ras,GtkWidget *wpredok)
{
//ost_skl_shetw_m_rr  rek_ras;
ost_skl_shetw_m_data data;
char strsql[512];

data.rk=rek_ras;
if(data.rk->data_ost.getdlinna() <= 1)
 {
  short dt,mt,gt;
  iceb_u_poltekdat(&dt,&mt,&gt);
  sprintf(strsql,"%d.%d.%d",dt,mt,gt);
  data.rk->data_ost.new_plus(strsql);  
 }


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Расчёт остатков по складам и счетам учёта"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(ost_skl_shetw_m_v_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=gtk_label_new(gettext("Расчёт остатков по складам и счетам учёта"));

GtkWidget *vbox = gtk_vbox_new (FALSE, 3);
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

sprintf(strsql,"%s (,,)",gettext("Слад"));
data.knopka_enter[E_SKLAD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.knopka_enter[E_SKLAD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SKLAD]),"clicked",GTK_SIGNAL_FUNC(ost_skl_shetw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SKLAD]),(gpointer)E_SKLAD);
tooltips_enter[E_SKLAD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SKLAD],data.knopka_enter[E_SKLAD],gettext("Выбор склада"),NULL);

data.entry[E_SKLAD] = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.entry[E_SKLAD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SKLAD]), "activate",GTK_SIGNAL_FUNC(ost_skl_shetw_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SKLAD]),data.rk->sklad.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SKLAD]),(gpointer)E_SKLAD);

sprintf(strsql,"%s (,,)",gettext("Группа"));
data.knopka_enter[E_GRUPA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.knopka_enter[E_GRUPA], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_GRUPA]),"clicked",GTK_SIGNAL_FUNC(ost_skl_shetw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_GRUPA]),(gpointer)E_GRUPA);
tooltips_enter[E_GRUPA]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_GRUPA],data.knopka_enter[E_GRUPA],gettext("Выбор группы"),NULL);

data.entry[E_GRUPA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.entry[E_GRUPA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GRUPA]), "activate",GTK_SIGNAL_FUNC(ost_skl_shetw_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUPA]),data.rk->grupa.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GRUPA]),(gpointer)E_GRUPA);

sprintf(strsql,"%s (,,)",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(ost_skl_shetw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(ost_skl_shetw_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk->shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

sprintf(strsql,"%s (,,)",gettext("Код материалла"));
data.knopka_enter[E_KODMAT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODMAT]), data.knopka_enter[E_KODMAT], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODMAT]),"clicked",GTK_SIGNAL_FUNC(ost_skl_shetw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODMAT]),(gpointer)E_KODMAT);
tooltips_enter[E_KODMAT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODMAT],data.knopka_enter[E_KODMAT],gettext("Выбор материалла"),NULL);

data.entry[E_KODMAT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODMAT]), data.entry[E_KODMAT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODMAT]), "activate",GTK_SIGNAL_FUNC(ost_skl_shetw_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODMAT]),data.rk->kodmat.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODMAT]),(gpointer)E_KODMAT);

sprintf(strsql,"%s",gettext("Дата остатка"));
data.knopka_enter[E_DATA_OST]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_OST]), data.knopka_enter[E_DATA_OST], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_OST]),"clicked",GTK_SIGNAL_FUNC(ost_skl_shetw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_OST]),(gpointer)E_DATA_OST);
tooltips_enter[E_DATA_OST]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_OST],data.knopka_enter[E_DATA_OST],gettext("Выбор даты"),NULL);

data.entry[E_DATA_OST] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_OST]), data.entry[E_DATA_OST], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_OST]), "activate",GTK_SIGNAL_FUNC(ost_skl_shetw_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_OST]),data.rk->data_ost.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_OST]),(gpointer)E_DATA_OST);



GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Расчет"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчет"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(ost_skl_shetw_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Реквизиты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Просмотр нужных списков, для заполнения нужными кодами реквизитов меню"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]),"clicked",GTK_SIGNAL_FUNC(ost_skl_shetw_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(ost_skl_shetw_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(ost_skl_shetw_m_v_knopka),&data);
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
void  ost_skl_shetw_v_e_knopka(GtkWidget *widget,class ost_skl_shetw_m_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.new_plus("");
naim.new_plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case E_DATA_OST:

    if(iceb_calendar(&data->rk->data_ost,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_OST]),data->rk->data_ost.ravno());
      
    return;  

   
  case E_SHET:

    iceb_vibrek(0,"Plansh",&data->rk->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno_toutf());

    return;  

  case E_SKLAD:

    iceb_vibrek(0,"Sklad",&data->rk->sklad,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk->sklad.ravno_toutf());

    return;  

  case E_GRUPA:

    iceb_vibrek(0,"Grup",&data->rk->grupa,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPA]),data->rk->grupa.ravno_toutf());

    return;  

  case E_KODMAT:

    if(l_mater(1,&kod,&naim,0,0,data->window) == 0)
     {
      data->rk->kodmat.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODMAT]),data->rk->kodmat.ravno_toutf());
     }

    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   ost_skl_shetw_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class ost_skl_shetw_m_data *data)
{

//printf("ost_skl_shetw_m_v_key_press\n");
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
void  ost_skl_shetw_m_v_knopka(GtkWidget *widget,class ost_skl_shetw_m_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(data->rk->data_ost.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Не верно введена дата !"),data->window);
      return;       
     }
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case FK3:

    ost_skl_shetw_sr(data);
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

void    ost_skl_shetw_m_v_vvod(GtkWidget *widget,class ost_skl_shetw_m_data *data)
{
//short d,m,g;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("ost_skl_shetw_m_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SKLAD:
    data->rk->sklad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_GRUPA:
    data->rk->grupa.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHET:
    data->rk->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KODMAT:
    data->rk->kodmat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATA_OST:
    data->rk->data_ost.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/*******************/
/*выбрать вид списка*/
/*********************/

void ost_skl_shetw_sr(class ost_skl_shetw_m_data *data)
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Просмотр списка складов"));//0
punkt_m.plus(gettext("Просмотр списка групп"));//1
punkt_m.plus(gettext("Просмотр списка счетов"));//2
punkt_m.plus(gettext("Просмотр списка материалов"));//3


int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);
iceb_u_str kod;
iceb_u_str naim;
kod.new_plus("");
naim.new_plus("");

switch(nomer)
 {
  case 0:
    if(iceb_vibrek(0,"Sklad",&data->rk->sklad,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk->sklad.ravno_toutf());
    break;

  case 1:
    if(iceb_vibrek(0,"Grup",&data->rk->grupa,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPA]),data->rk->grupa.ravno_toutf());
    break;

  case 2:
    if(iceb_vibrek(0,"Plansh",&data->rk->shet,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno_toutf());
    break;

  case 3:
    if(l_mater(1,&kod,&naim,0,0,data->window) == 0)
     {
      data->rk->kodmat.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODMAT]),data->rk->kodmat.ravno_toutf());
     }
    break;
 
 }
 


}
