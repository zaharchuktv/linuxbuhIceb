/*$Id: iceb_l_kontrp.c,v 1.18 2011-02-21 07:36:07 sasa Exp $*/
/*29.02.2008	03.12.2003	Белых А.И.	iceb_l_kontrp.c
Ввод и корректировка реквизитов поиска записей
*/
#include        <stdlib.h>
#include        <time.h>
#include        <unistd.h>
#include        <errno.h>
#include	"iceb_libbuh.h"
#include        "iceb_l_kontr.h"

enum
 {
  E_KONTR,
  E_NAIMK,
  E_ADRES,
  E_KOD,
  E_NOMSH,
  E_MFO,
  E_NAIMB,
  E_ADRESB,
  E_NSPNDS,
  E_GRUP,
  E_TELEF,
  E_DOGOV,
  E_REGNOM,
  E_INNN,
  E_NAIMP,
  E_SHNDS,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

class iceb_l_kontrp_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class kontr_rek *rk;
  
  
  iceb_l_kontrp_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
     }
   }
 };

void    kontrp_vvod(GtkWidget *widget,class iceb_l_kontrp_data *data);
void  kontrp_knopka(GtkWidget *widget,class iceb_l_kontrp_data *data);
gboolean   kontrp_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_kontrp_data *data);
//gboolean kontrp_delete_event(GtkWidget *widget,GdkEvent *event,class iceb_l_kontrp_data *data);
void kontrp_clear(class iceb_l_kontrp_data *data);
extern char *name_system;


int      iceb_l_kontrp(class kontr_rek *rek_poi,GtkWidget *wpredok)
{

char    strsql[512];

class iceb_l_kontrp_data data;
data.rk=rek_poi;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Поиск"));
/*********
gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_widget_destroyed),&data.window);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(kontrp_key_press),data);
***************/
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(kontrp_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox1 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox2 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox3 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox4 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox5 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox6 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox7 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox8 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox9 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox10 = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), hbox);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hbox3);
gtk_container_add (GTK_CONTAINER (vbox), hbox4);
gtk_container_add (GTK_CONTAINER (vbox), hbox5);
gtk_container_add (GTK_CONTAINER (vbox), hbox6);
gtk_container_add (GTK_CONTAINER (vbox), hbox7);
gtk_container_add (GTK_CONTAINER (vbox), hbox8);
gtk_container_add (GTK_CONTAINER (vbox), hbox9);
gtk_container_add (GTK_CONTAINER (vbox), hbox10);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkWidget *label=gtk_label_new(gettext("Код контрагента"));
data.entry[E_KONTR] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), data.entry[E_KONTR], TRUE, TRUE, 0);
//gtk_signal_connect(GTK_OBJECT (data.entry[E_KONTR]), "activate",GTK_SIGNAL_FUNC(iceb_get_text),data->rk->kodkontr.ravno());
gtk_signal_connect(GTK_OBJECT (data.entry[E_KONTR]), "activate",GTK_SIGNAL_FUNC(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),iceb_u_toutf(data.rk->kodkontr.ravno()));
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KONTR]),(gpointer)E_KONTR);

label=gtk_label_new(gettext("Наименование"));
data.entry[E_NAIMK] = gtk_entry_new_with_max_length (79);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data.entry[E_NAIMK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIMK]), "activate",GTK_SIGNAL_FUNC(kontrp_vvod),&data);
gtk_widget_set_usize(data.entry[E_NAIMK],400,-1);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIMK]),iceb_u_toutf(data.rk->naimkon.ravno()));
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIMK]),(gpointer)E_NAIMK);


label=gtk_label_new(gettext("Адрес"));
data.entry[E_ADRES] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data.entry[E_ADRES], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ADRES]), "activate",GTK_SIGNAL_FUNC(kontrp_vvod),&data);
gtk_widget_set_usize(data.entry[E_ADRES],400,-1);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ADRES]),iceb_u_toutf(data.rk->adres.ravno()));
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ADRES]),(gpointer)E_ADRES);

label=gtk_label_new(gettext("Код ЕГРПОУ"));
data.entry[E_KOD] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox3), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox3), data.entry[E_KOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),iceb_u_toutf(data.rk->kod.ravno()));
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

label=gtk_label_new(gettext("Счет"));
data.entry[E_NOMSH] = gtk_entry_new_with_max_length (29);
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox4), data.entry[E_NOMSH], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMSH]), "activate",GTK_SIGNAL_FUNC(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMSH]),iceb_u_toutf(data.rk->nomsh.ravno()));
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMSH]),(gpointer)E_NOMSH);

label=gtk_label_new(gettext("МФО"));
data.entry[E_MFO] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox4), data.entry[E_MFO], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_MFO]), "activate",GTK_SIGNAL_FUNC(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MFO]),iceb_u_toutf(data.rk->mfo.ravno()));
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_MFO]),(gpointer)E_MFO);

label=gtk_label_new(gettext("Наименование банка"));
data.entry[E_NAIMB] = gtk_entry_new_with_max_length (79);
gtk_box_pack_start (GTK_BOX (hbox5), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox5), data.entry[E_NAIMB], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIMB]), "activate",GTK_SIGNAL_FUNC(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIMB]),iceb_u_toutf(data.rk->naimban.ravno()));
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIMB]),(gpointer)E_NAIMB);

label=gtk_label_new(gettext("Город банка"));
data.entry[E_ADRESB] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox6), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox6), data.entry[E_ADRESB], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ADRESB]), "activate",GTK_SIGNAL_FUNC(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ADRESB]),iceb_u_toutf(data.rk->adresb.ravno()));
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ADRESB]),(gpointer)E_ADRESB);

label=gtk_label_new(gettext("Номер св. НДС"));
data.entry[E_NSPNDS] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox6), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox6), data.entry[E_NSPNDS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NSPNDS]), "activate",GTK_SIGNAL_FUNC(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NSPNDS]),iceb_u_toutf(data.rk->nspnds.ravno()));
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NSPNDS]),(gpointer)E_NSPNDS);

label=gtk_label_new(gettext("Группа"));
data.entry[E_GRUP] = gtk_entry_new_with_max_length (9);
gtk_box_pack_start (GTK_BOX (hbox7), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox7), data.entry[E_GRUP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GRUP]), "activate",GTK_SIGNAL_FUNC(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUP]),iceb_u_toutf(data.rk->grup.ravno()));
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GRUP]),(gpointer)E_GRUP);

label=gtk_label_new(gettext("Телефон"));
data.entry[E_TELEF] = gtk_entry_new_with_max_length (79);
gtk_box_pack_start (GTK_BOX (hbox7), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox7), data.entry[E_TELEF], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TELEF]), "activate",GTK_SIGNAL_FUNC(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TELEF]),iceb_u_toutf(data.rk->telef.ravno()));
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TELEF]),(gpointer)E_TELEF);

label=gtk_label_new(gettext("Договор"));
data.entry[E_DOGOV] = gtk_entry_new_with_max_length (79);
gtk_box_pack_start (GTK_BOX (hbox8), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox8), data.entry[E_DOGOV], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DOGOV]), "activate",GTK_SIGNAL_FUNC(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DOGOV]),iceb_u_toutf(data.rk->dogov.ravno()));
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DOGOV]),(gpointer)E_DOGOV);

label=gtk_label_new(gettext("Реги.ном.ч/п"));
data.entry[E_REGNOM] = gtk_entry_new_with_max_length (29);
gtk_box_pack_start (GTK_BOX (hbox9), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox9), data.entry[E_REGNOM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_REGNOM]), "activate",GTK_SIGNAL_FUNC(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_REGNOM]),iceb_u_toutf(data.rk->regnom.ravno()));
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_REGNOM]),(gpointer)E_REGNOM);

label=gtk_label_new(gettext("Инд. налог.номер"));
data.entry[E_INNN] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox9), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox9), data.entry[E_INNN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_INNN]), "activate",GTK_SIGNAL_FUNC(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INNN]),iceb_u_toutf(data.rk->innn.ravno()));
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_INNN]),(gpointer)E_INNN);

label=gtk_label_new(gettext("Полное наименование"));
data.entry[E_NAIMP] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox10), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox10), data.entry[E_NAIMP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIMP]), "activate",GTK_SIGNAL_FUNC(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIMP]),data.rk->naimp.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIMP]),(gpointer)E_NAIMP);

label=gtk_label_new(gettext("Спец счёт для НДС"));
data.entry[E_SHNDS] = gtk_entry_new_with_max_length (29);
gtk_box_pack_start (GTK_BOX (hbox10), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox10), data.entry[E_SHNDS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHNDS]), "activate",GTK_SIGNAL_FUNC(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHNDS]),data.rk->shnds.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHNDS]),(gpointer)E_SHNDS);

GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],data.knopka[PFK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK2]),"clicked",GTK_SIGNAL_FUNC(kontrp_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK2]),(gpointer)PFK2);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[PFK4]=gtk_button_new_with_label(strsql);
tooltips[PFK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK4],data.knopka[PFK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK4]),"clicked",GTK_SIGNAL_FUNC(kontrp_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK4]),(gpointer)PFK4);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],data.knopka[PFK10],gettext("Поиск не выполнять"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK10]),"clicked",GTK_SIGNAL_FUNC(kontrp_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK10]),(gpointer)PFK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK4], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);
         
gtk_widget_grab_focus(data.entry[1]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}


/*****************************/
/*Обработчик сигнала delete_event*/
/****************************/
/*****************
gboolean kontrp_delete_event(GtkWidget *widget,GdkEvent *event,class kontr_data *data)
{
//printf("vkontr_delete_event\n");
gtk_widget_destroy(widget);
//data->window=NULL;

return(FALSE);
}
**************************/
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kontrp_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_kontrp_data *data)
{

switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PFK2]),"clicked");
    return(TRUE);

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PFK4]),"clicked");
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
void  kontrp_knopka(GtkWidget *widget,class iceb_l_kontrp_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//printf("kontrp_knopka knop=%d\n",knop);
data->kl_shift=0;
switch (knop)
 {
  case PFK2:
    data->read_rek();
    data->voz=0;
    
    gtk_widget_destroy(data->window);
    return;  

  case PFK4:
    kontrp_clear(data);
    return;  


  case PFK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    kontrp_vvod(GtkWidget *widget,class iceb_l_kontrp_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("kontrp_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KONTR:
    data->rk->kodkontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NAIMK:
    data->rk->naimkon.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_ADRES:
    data->rk->adres.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_ADRESB:
    data->rk->adresb.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOD:
    data->rk->kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOMSH:
    data->rk->nomsh.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_MFO:
    data->rk->mfo.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NAIMB:
    data->rk->naimban.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NSPNDS:
    data->rk->nspnds.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_GRUP:
    data->rk->grup.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


  case E_TELEF:
    data->rk->telef.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DOGOV:
    data->rk->dogov.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_REGNOM:
    data->rk->regnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_INNN:
    data->rk->innn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NAIMP:
    data->rk->naimp.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHNDS:
    data->rk->shnds.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
 }
enter++;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Очистка меню от введенной информации*/
/**************************************/
void kontrp_clear(class iceb_l_kontrp_data *data)
{

data->rk->clear_zero();
for(int i=0; i< KOLENTER; i++)
 gtk_entry_set_text(GTK_ENTRY(data->entry[i]),"");

gtk_widget_grab_focus(data->entry[0]);

}
