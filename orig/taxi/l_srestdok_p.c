/*$Id: l_srestdok_p.c,v 1.19 2011-02-21 07:36:20 sasa Exp $*/
/*06.11.2007	01.03.2004	Белых А.И.	l_srestdok_p.c
Ввод и коррктировка реквизитов поиска
*/
#include <unistd.h>
#include "i_rest.h"
#include "l_srestdok.h"

enum
{
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
};

enum
 {
  E_DATAN,
  E_DATAK,
  E_NOMDOK,
  E_NOMERST,
  E_KODKL,
  E_FIO,
  E_KOMENT,
  E_PODR,
  KOLENTER  
 };

class  srestdok_p_data
 {
  public:
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopkap[KOL_PFK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  
  srestdok_r_data *rk; //Реквизиты поиска документов
  short kl_shift;
  
 };
 
gboolean   srestdok_p_key_press(GtkWidget *widget,GdkEventKey *event,class srestdok_p_data *data);
void  srestdok_p_knopka(GtkWidget *widget,class srestdok_p_data *data);
void  srestdok_v_e_knopka(GtkWidget *widget,class srestdok_p_data *data);
void    srestdok_p_vvod(GtkWidget *widget,class srestdok_p_data *data);

//extern SQL_baza	bd;
extern char *name_system;


void l_srestdok_p(class srestdok_r_data *datap,GtkWidget *wpredok)
{
printf("l_srestdok_p\n");

class  srestdok_p_data data;
data.rk=datap;

char strsql[300];

    
data.kl_shift=0;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Поиск"));


gtk_window_set_title(GTK_WINDOW(data.window),strsql);
//gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_widget_destroyed),&data.window);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(srestdok_p_key_press),&data);


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
for(int i=0 ; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0 ; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


data.knopka_enter[E_DATAN]=gtk_button_new_with_label(gettext("Дата начала"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(srestdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(srestdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);

data.knopka_enter[E_DATAK]=gtk_button_new_with_label(gettext("Дата конца"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(srestdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(srestdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);


data.knopka_enter[E_NOMDOK]=gtk_button_new_with_label(gettext("Номер документа"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.knopka_enter[E_NOMDOK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_NOMDOK]),"clicked",GTK_SIGNAL_FUNC(srestdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_NOMDOK]),(gpointer)E_NOMDOK);

data.entry[E_NOMDOK] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMDOK]), "activate",GTK_SIGNAL_FUNC(srestdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk->nomdokp.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMDOK]),(gpointer)E_NOMDOK);

data.knopka_enter[E_NOMERST]=gtk_button_new_with_label(gettext("Номер столика"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMERST]), data.knopka_enter[E_NOMERST], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_NOMERST]),"clicked",GTK_SIGNAL_FUNC(srestdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_NOMERST]),(gpointer)E_NOMERST);

data.entry[E_NOMERST] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMERST]), data.entry[E_NOMERST], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMERST]), "activate",GTK_SIGNAL_FUNC(srestdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMERST]),data.rk->nomstol.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMERST]),(gpointer)E_NOMERST);

data.knopka_enter[E_KODKL]=gtk_button_new_with_label(gettext("Код клиента"));
gtk_box_pack_start (GTK_BOX (hbox[E_KODKL]), data.knopka_enter[E_KODKL], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODKL]),"clicked",GTK_SIGNAL_FUNC(srestdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODKL]),(gpointer)E_KODKL);

data.entry[E_KODKL] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_KODKL]), data.entry[E_KODKL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODKL]), "activate",GTK_SIGNAL_FUNC(srestdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODKL]),data.rk->kodkl.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODKL]),(gpointer)E_KODKL);

data.knopka_enter[E_FIO]=gtk_button_new_with_label(gettext("Фамилия клиента"));
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.knopka_enter[E_FIO], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_FIO]),"clicked",GTK_SIGNAL_FUNC(srestdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_FIO]),(gpointer)E_FIO);

data.entry[E_FIO] = gtk_entry_new_with_max_length (59);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_FIO]), "activate",GTK_SIGNAL_FUNC(srestdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.rk->fio.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_FIO]),(gpointer)E_FIO);

data.knopka_enter[E_KOMENT]=gtk_button_new_with_label(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.knopka_enter[E_KOMENT], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOMENT]),"clicked",GTK_SIGNAL_FUNC(srestdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOMENT]),(gpointer)E_KOMENT);

data.entry[E_KOMENT] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(srestdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.rk->koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);


sprintf(strsql,"%s (,,)",gettext("Подразделение"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODR]),"clicked",GTK_SIGNAL_FUNC(srestdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODR]),(gpointer)E_PODR);

data.entry[E_PODR] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODR]), "activate",GTK_SIGNAL_FUNC(srestdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk->koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODR]),(gpointer)E_PODR);


GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopkap[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],data.knopkap[PFK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopkap[PFK2]),"clicked",GTK_SIGNAL_FUNC(srestdok_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopkap[PFK2]),(gpointer)PFK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopkap[PFK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopkap[PFK4]=gtk_button_new_with_label(strsql);
tooltips[PFK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK4],data.knopkap[PFK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopkap[PFK4]),"clicked",GTK_SIGNAL_FUNC(srestdok_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopkap[PFK4]),(gpointer)PFK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopkap[PFK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopkap[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],data.knopkap[PFK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopkap[PFK10]),"clicked",GTK_SIGNAL_FUNC(srestdok_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopkap[PFK10]),(gpointer)PFK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopkap[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  srestdok_v_e_knopka(GtkWidget *widget,class srestdok_p_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case E_DATAN:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_DATAN]),"activate");
    iceb_mous_klav(gettext("Дата начала"),&data->rk->datan,10,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno_toutf());
    return;  

  case E_DATAK:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_DATAK]),"activate");
    iceb_mous_klav(gettext("Дата конца"),&data->rk->datak,10,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rk->datak.ravno_toutf());
    return;  

  case E_NOMDOK:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_NOMDOK]),"activate");
    iceb_mous_klav(gettext("Номер документа"),&data->rk->datak,10,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk->datak.ravno_toutf());
    return;  

  case E_NOMERST:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_NOMERST]),"activate");
    iceb_mous_klav(gettext("Номер столика"),&data->rk->nomstol,10,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMERST]),data->rk->nomstol.ravno_toutf());
    return;  

  case E_KODKL:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_KODKL]),"activate");
    iceb_mous_klav(gettext("Код клиента"),&data->rk->kodkl,10,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODKL]),data->rk->kodkl.ravno_toutf());
    return;  

  case E_FIO:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_FIO]),"activate");
    iceb_mous_klav(gettext("Фамилия клиента"),&data->rk->fio,59,1,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_FIO]),data->rk->fio.ravno_toutf());
    return;  

  case E_KOMENT:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_KOMENT]),"activate");
    iceb_mous_klav(gettext("Коментарий"),&data->rk->koment,99,1,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOMENT]),data->rk->koment.ravno_toutf());
    return;  

  case E_PODR:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_PODR]),"activate");
    iceb_mous_klav(gettext("Подразделение"),&data->rk->podr,99,0,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk->podr.ravno_toutf());
    return;  
 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   srestdok_p_key_press(GtkWidget *widget,GdkEventKey *event,class srestdok_p_data *data)
{
//char  bros[300];

//printf("srestdok_p_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopkap[PFK2]),"clicked");
    return(TRUE);

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopkap[PFK4]),"clicked");
    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopkap[PFK10]),"clicked");
    return(TRUE);
//    return(FALSE);

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
void  srestdok_p_knopka(GtkWidget *widget,class srestdok_p_data *data)
{
//char bros[300];
short d,m,g;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("srestdok_p_knopka knop=%d\n",knop);

switch ((gint)knop)
 {
  case PFK2:
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(data->entry[i]),"activate");

    if(data->rk->datan.getdlinna() > 1)
    if(iceb_u_rsdat(&d,&m,&g,data->rk->datan.ravno(),1) != 0)
     {
      iceb_u_str repl;
      repl.plus(gettext("Не верно введена дата начала !"));
      iceb_menu_soob(&repl,data->window);
      return;
     }

    if(data->rk->datak.getdlinna() > 1)
     if(iceb_u_rsdat(&d,&m,&g,data->rk->datak.ravno(),1) != 0)
      {
       iceb_u_str repl;
       repl.plus(gettext("Не верно введена дата конца !"));
       iceb_menu_soob(&repl,data->window);
       return;
      }

    data->rk->metkapoi=1;
    gtk_widget_destroy(data->window);
    data->window=NULL;

    return;  

  case PFK4:
    data->rk->clear_data(); 
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(data->entry[i]),"");
    return;

  case PFK10:
    data->rk->metkapoi=0;
    gtk_widget_destroy(data->window);
    data->window=NULL;

    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    srestdok_p_vvod(GtkWidget *widget,class srestdok_p_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("srestdok_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
     
    break;
  case E_DATAK:
    data->rk->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_FIO:
    data->rk->fio.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOMERST:
    data->rk->nomstol.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KODKL:
    data->rk->kodkl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOMDOK:
    data->rk->nomdokp.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOMENT:
    data->rk->koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_PODR:
    data->rk->podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
