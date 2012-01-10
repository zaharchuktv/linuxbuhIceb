/*$Id: l_upldok_vs1.c,v 1.7 2011-02-21 07:35:54 sasa Exp $*/
/*13.03.2008	13.03.2008	Белых А.И.	l_upldok_vs1.c
Ввод первой дополнительной страницы к шапке документа
*/

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
#include "l_upldok_rekh.h"

enum
 {
  E_VES_GRUZA,
  E_TONO_KIL,
  E_NZT_NA_GRUZ,
  E_ZTF_NA_GRUZ,
  E_VR_DVIG,
  E_NZT_VR_DVIG,
  E_ZTF_VR_DVIG,
  KOLENTER  

 };

enum
 {
  FK2,
  FK10,
  KOL_FK
 };

class l_upldok_vs1_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_vod;
  GtkWidget *label_pod;
  GtkWidget *label_avt;
  short kl_shift;
  short voz;      //0-ввели 1 нет


  class iceb_u_str ves_gruza;    /*вес груза*/
  class iceb_u_str tono_kil;     /*тоно-километры*/
  class iceb_u_str nzt_na_gruz;  /*норма затрат топлива на перевозку груза*/
  class iceb_u_str ztf_na_gruz;  /*затраты топлива фактические на перевозку груза*/
  class iceb_u_str vr_dvig;      /*время работы двигателя*/
  class iceb_u_str nzt_vr_dvig;  /*норма затрат на время работы двигателя*/
  class iceb_u_str ztf_vr_dvig;  /*затраты фактические на время работы двигателя*/

  
  l_upldok_vs1_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    ves_gruza.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VES_GRUZA]))));
    tono_kil.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_TONO_KIL]))));
    nzt_na_gruz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NZT_NA_GRUZ]))));
    ztf_na_gruz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_ZTF_NA_GRUZ]))));
    vr_dvig.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VR_DVIG]))));
    nzt_vr_dvig.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NZT_VR_DVIG]))));   
    ztf_vr_dvig.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_ZTF_VR_DVIG]))));
    
    
   }
 };


gboolean   l_upldok_vs1_key_press(GtkWidget *widget,GdkEventKey *event,class l_upldok_vs1_data *data);
void  l_upldok_vs1_knopka(GtkWidget *widget,class l_upldok_vs1_data *data);
void    l_upldok_vs1_vvod(GtkWidget *widget,class l_upldok_vs1_data *data);

extern SQL_baza  bd;
extern char      *name_system;

int l_upldok_vs1(class l_upldok_rekh *rekhd,GtkWidget *wpredok)
{
char strsql[512];
class l_upldok_vs1_data data;

data.ves_gruza.plus(rekhd->ves_gruza.ravno());
data.tono_kil.plus(rekhd->tono_kil.ravno());
data.nzt_na_gruz.plus(rekhd->nzt_na_gruz.ravno());
data.ztf_na_gruz.plus(rekhd->ztf_na_gruz.ravno());
data.vr_dvig.plus(rekhd->vr_dvig.ravno());
data.nzt_vr_dvig.plus(rekhd->nzt_vr_dvig.ravno());
data.ztf_vr_dvig.plus(rekhd->ztf_vr_dvig.ravno());


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",name_system,gettext("Ввод дополнительной информации"));
label=gtk_label_new(gettext("Ввод дополнительной информации"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_upldok_vs1_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox = gtk_vbox_new (FALSE,1);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE,1);
GtkWidget *hboxknop = gtk_hbox_new (FALSE,1);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

label=gtk_label_new(gettext("Вес груза"));
gtk_box_pack_start (GTK_BOX (hbox[E_VES_GRUZA]), label, FALSE, FALSE,1);

data.entry[E_VES_GRUZA] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_VES_GRUZA]), data.entry[E_VES_GRUZA], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VES_GRUZA]), "activate",GTK_SIGNAL_FUNC(l_upldok_vs1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VES_GRUZA]),data.ves_gruza.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VES_GRUZA]),(gpointer)E_VES_GRUZA);


label=gtk_label_new(gettext("Тоннокилометры"));
gtk_box_pack_start (GTK_BOX (hbox[E_TONO_KIL]), label, FALSE, FALSE,1);

data.entry[E_TONO_KIL] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_TONO_KIL]), data.entry[E_TONO_KIL], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TONO_KIL]), "activate",GTK_SIGNAL_FUNC(l_upldok_vs1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TONO_KIL]),data.tono_kil.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TONO_KIL]),(gpointer)E_TONO_KIL);


label=gtk_label_new(gettext("Норма расхода топлива на перевозку груза"));
gtk_box_pack_start (GTK_BOX (hbox[E_NZT_NA_GRUZ]), label, FALSE, FALSE,1);

data.entry[E_NZT_NA_GRUZ] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_NZT_NA_GRUZ]), data.entry[E_NZT_NA_GRUZ], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NZT_NA_GRUZ]), "activate",GTK_SIGNAL_FUNC(l_upldok_vs1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NZT_NA_GRUZ]),data.nzt_na_gruz.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NZT_NA_GRUZ]),(gpointer)E_NZT_NA_GRUZ);


label=gtk_label_new(gettext("Расход топлива фактический на перевозку груза"));
gtk_box_pack_start (GTK_BOX (hbox[E_ZTF_NA_GRUZ]), label, FALSE, FALSE,1);

data.entry[E_ZTF_NA_GRUZ] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_ZTF_NA_GRUZ]), data.entry[E_ZTF_NA_GRUZ], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ZTF_NA_GRUZ]), "activate",GTK_SIGNAL_FUNC(l_upldok_vs1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZTF_NA_GRUZ]),data.ztf_na_gruz.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ZTF_NA_GRUZ]),(gpointer)E_ZTF_NA_GRUZ);


label=gtk_label_new(gettext("Время работы двигателя"));
gtk_box_pack_start (GTK_BOX (hbox[E_VR_DVIG]), label, FALSE, FALSE,1);

data.entry[E_VR_DVIG] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_VR_DVIG]), data.entry[E_VR_DVIG], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VR_DVIG]), "activate",GTK_SIGNAL_FUNC(l_upldok_vs1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VR_DVIG]),data.vr_dvig.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VR_DVIG]),(gpointer)E_VR_DVIG);


label=gtk_label_new(gettext("Норма расхода топлива на время работы двигителя"));
gtk_box_pack_start (GTK_BOX (hbox[E_NZT_VR_DVIG]), label, FALSE, FALSE,1);

data.entry[E_NZT_VR_DVIG] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_NZT_VR_DVIG]), data.entry[E_NZT_VR_DVIG], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NZT_VR_DVIG]), "activate",GTK_SIGNAL_FUNC(l_upldok_vs1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NZT_VR_DVIG]),data.nzt_vr_dvig.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NZT_VR_DVIG]),(gpointer)E_NZT_VR_DVIG);


label=gtk_label_new(gettext("Расход топлива фактический на время работы двигителя"));
gtk_box_pack_start (GTK_BOX (hbox[E_ZTF_VR_DVIG]), label, FALSE, FALSE,1);

data.entry[E_ZTF_VR_DVIG] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_ZTF_VR_DVIG]), data.entry[E_ZTF_VR_DVIG], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ZTF_VR_DVIG]), "activate",GTK_SIGNAL_FUNC(l_upldok_vs1_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZTF_VR_DVIG]),data.ztf_vr_dvig.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ZTF_VR_DVIG]),(gpointer)E_ZTF_VR_DVIG);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_upldok_vs1_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_upldok_vs1_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0)
 {
  rekhd->ves_gruza.new_plus(data.ves_gruza.ravno());
  rekhd->tono_kil.new_plus(data.tono_kil.ravno());
  rekhd->nzt_na_gruz.new_plus(data.nzt_na_gruz.ravno());
  rekhd->ztf_na_gruz.new_plus(data.ztf_na_gruz.ravno());
  rekhd->vr_dvig.new_plus(data.vr_dvig.ravno());
  rekhd->nzt_vr_dvig.new_plus(data.nzt_vr_dvig.ravno());
  rekhd->ztf_vr_dvig.new_plus(data.ztf_vr_dvig.ravno());
 }
 
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_upldok_vs1_key_press(GtkWidget *widget,GdkEventKey *event,class l_upldok_vs1_data *data)
{
//char  bros[512];

//printf("l_upldok_vs1_key_press\n");
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
void  l_upldok_vs1_knopka(GtkWidget *widget,class l_upldok_vs1_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_upldok_vs1_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    data->voz=0;
    gtk_widget_destroy(data->window);
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

void    l_upldok_vs1_vvod(GtkWidget *widget,class l_upldok_vs1_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_upldok_vs1_vvod enter=%d\n",enter);
switch (enter)
 {
  case E_VES_GRUZA:
    data->ves_gruza.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
 
  case E_TONO_KIL:
    data->tono_kil.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    data->nzt_na_gruz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_NZT_NA_GRUZ]))));
    data->ztf_na_gruz.new_plus(data->tono_kil.ravno_atof() * data->nzt_na_gruz.ravno_atof()/100.);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_ZTF_NA_GRUZ]),data->ztf_na_gruz.ravno_toutf());
    break;

  case E_VR_DVIG:
    data->vr_dvig.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    data->nzt_vr_dvig.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_NZT_VR_DVIG]))));
    data->ztf_vr_dvig.new_plus(data->vr_dvig.ravno_atof() * data->nzt_vr_dvig.ravno_atof()/100.);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_ZTF_VR_DVIG]),data->ztf_vr_dvig.ravno_toutf());
    break;


 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
