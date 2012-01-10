/*$Id: podzapusl.c,v 1.12 2011-02-21 07:35:55 sasa Exp $*/
/*05.05.2006	20.09.2004	Белых А.И.	podzapusl.c
Подтверждение записи в учёте услуг
*/
#include <stdlib.h>
#include <math.h>
#include "buhg_g.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAP,
  E_KOLIH,
  KOLENTER  
 };

class podzapusl_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-подтвердили запись 1-нет

  iceb_u_str datap;
  iceb_u_str kolih;
  
  //полученные реквизиты
  short dd,md,gd; //Дата документа
  int tipz;  //1-приход 2-расход
  int podr;  //Склад
  iceb_u_str nomdok; //Номер документа
  int kodzap;
  int metka_zap;
  int nom_zap;

  //Прочитанные реквизиты
  double kolih_dok; //Количество в документа
  iceb_u_str shetu;
  iceb_u_str ei;
  double cena;  
  double kolih_pod; //Уже подтверждённое количество
  
  podzapusl_data() //Конструктор
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

gboolean   podzapusl_v_key_press(GtkWidget *widget,GdkEventKey *event,class podzapusl_data *data);
void    podzapusl_v_vvod(GtkWidget *widget,class podzapusl_data *data);
void  podzapusl_v_knopka(GtkWidget *widget,class podzapusl_data *data);

int podzapusl_zapis(class podzapusl_data *data);


extern char *name_system;
extern SQL_baza bd;

int   podzapusl(short dd,short md,short gd, //Дата документа
int tipz,  //1-приход 2-расход
int podr,  //Склад
const char *nomdok, //Номер документа
int kodzap,
int metka_zap,
int nom_zap,
GtkWidget *wpredok)
{
class podzapusl_data data;

char strsql[512];
SQL_str row;
SQLCURSOR cur;

data.dd=dd;
data.md=md;
data.gd=gd;
data.tipz=tipz;
data.podr=podr;
data.nomdok.plus(nomdok);
data.kodzap=kodzap;
data.metka_zap=metka_zap;
data.nom_zap=nom_zap;

//Читаем нужные реквизиты
sprintf(strsql,"select kolih,cena,ei,shetu from Usldokum1 where \
datd='%04d-%02d-%02d' and podr=%d and tp=%d and nomd='%s' and \
metka=%d and kodzap=%d and nz=%d",
gd,md,dd,podr,tipz,nomdok,metka_zap,kodzap,nom_zap);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  iceb_menu_soob(gettext("Не найдена запись !"),wpredok);
  return(1);
 }

data.kolih_dok=atof(row[0]);
data.cena=atof(row[1]);
data.ei.new_plus(row[2]);
data.shetu.new_plus(row[3]);

data.kolih_pod=readkoluw(dd,md,gd,podr,nomdok,tipz,metka_zap,kodzap,nom_zap,wpredok);

if(fabs(data.kolih_dok- data.kolih_pod) < 0.00001)
 {
  iceb_menu_soob(gettext("Запись уже подтверждена !"),wpredok);
  return(1);
 }


data.kolih.plus(data.kolih_dok-data.kolih_pod);

if(data.kolih_pod == 0.)
 {
  sprintf(strsql,"%d.%d.%d",dd,md,gd);
  data.datap.plus(strsql);
 }
else
  data.datap.plus("");
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Подтверждение записи"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(podzapusl_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=gtk_label_new(gettext("Подтверждение записи"));

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

sprintf(strsql,"%s",gettext("Дата подтверждения"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_DATAP] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAP]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAP]), data.entry[E_DATAP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAP]), "activate",GTK_SIGNAL_FUNC(podzapusl_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAP]),data.datap.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAP]),(gpointer)E_DATAP);

label=gtk_label_new(gettext("Количество"));
data.entry[E_KOLIH] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), data.entry[E_KOLIH], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOLIH]), "activate",GTK_SIGNAL_FUNC(podzapusl_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH]),data.kolih.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOLIH]),(gpointer)E_KOLIH);


GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введенной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(podzapusl_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(podzapusl_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(podzapusl_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);

}


/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   podzapusl_v_key_press(GtkWidget *widget,GdkEventKey *event,class podzapusl_data *data)
{

//printf("podzapusl_v_key_press\n");
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
void  podzapusl_v_knopka(GtkWidget *widget,class podzapusl_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    if( podzapusl_zapis(data) != 0)
      return;
        
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

void    podzapusl_v_vvod(GtkWidget *widget,class podzapusl_data *data)
{
//short d,m,g;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("podzapusl_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAP:
    data->datap.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KOLIH:
    data->kolih.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/**********************************/
/*Запись введенной информации*/
/**********************************/
int podzapusl_zapis(class podzapusl_data *data)
{
short d,m,g;

if(iceb_u_rsdat(&d,&m,&g,data->datap.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата !"),data->window);
  return(1);
 }
        

if(iceb_u_sravmydat(d,m,g,data->dd,data->md,data->gd) < 0)
 {
  iceb_menu_soob(gettext("Дата меньше чем дата документа ! Подтверждение невозможно !"),data->window);
  return(1);
 }

if(data->kolih_dok - data->kolih_pod - data->kolih.ravno_atof() < -0.00001)
 {
  iceb_menu_soob(gettext("Неправильно введено количество!"),data->window);
  return(1);
 }



zappoduw(data->tipz,data->dd,data->md,data->gd,d,m,g,data->nomdok.ravno(),data->metka_zap,data->kodzap,
data->kolih.ravno_atof(),data->cena,data->ei.ravno(),data->shetu.ravno(),data->podr,data->nom_zap,data->window);

return(0);

}
