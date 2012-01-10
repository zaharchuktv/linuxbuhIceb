/*$Id: podzapxdk.c,v 1.10 2011-02-21 07:35:55 sasa Exp $*/
/*05.05.2006	20.09.2004	Белых А.И.	podzapxdk.c
Подтверждение записи в учёте услуг
*/
#include <stdlib.h>
#include <math.h>
#include "buhg_g.h"
#include "dok4w.h"
#include <unistd.h>
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
  E_SUMA,
  KOLENTER  
 };

class podzapxdk_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-подтвердили запись 1-нет

  iceb_u_str datap;
  iceb_u_str suma_pod;
  
  char tablica[50];
  
  
  podzapxdk_data() //Конструктор
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

gboolean   podzapxdk_v_key_press(GtkWidget *widget,GdkEventKey *event,class podzapxdk_data *data);
void    podzapxdk_v_vvod(GtkWidget *widget,class podzapxdk_data *data);
void  podzapxdk_v_knopka(GtkWidget *widget,class podzapxdk_data *data);

int podzapxdk_zapis(class podzapxdk_data *data);


extern char *name_system;
extern SQL_baza bd;
extern class REC rec;

int   podzapxdk(char *tablica,
GtkWidget *wpredok)
{

if(iceb_u_SRAV(tablica,"Tpltp",0) == 0 || iceb_u_SRAV(tablica,"Tpltt",0) == 0)
 {
  iceb_menu_soob(gettext("Типовые документы не нужно подтверждать !"),wpredok);
  return(1);
 }
if(rec.nomdk_i.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Документ не записан !"),wpredok);
  return(1);
 }

class podzapxdk_data data;

short d,m,g;
double sumpp=0.;
double sump=sumpzpdw(tablica,&d,&m,&g,&sumpp,wpredok);

data.suma_pod.plus(rec.sumd+rec.uslb-sump);



char strsql[512];
//SQL_str row;
//SQLCURSOR cur;

strcpy(data.tablica,tablica);


//data.kolih_pod=readkoluw(dd,md,gd,podr,nomdok,tipz,metka_zap,kodzap,nom_zap,wpredok);

if(data.suma_pod.ravno_atof() == 0.)
 {
  iceb_menu_soob(gettext("Документ уже подтверждён !"),wpredok);
  return(1);
 }


if(sump == 0.)
 {
  sprintf(strsql,"%d.%d.%d",rec.dd,rec.md,rec.gd);
  data.datap.plus(strsql);
 }
else
  data.datap.plus("");
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Подтверждение документа"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(podzapxdk_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=gtk_label_new(gettext("Подтверждение документа"));

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
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAP]), "activate",GTK_SIGNAL_FUNC(podzapxdk_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAP]),data.datap.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAP]),(gpointer)E_DATAP);



label=gtk_label_new(gettext("Сумма"));
data.entry[E_SUMA] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMA]), "activate",GTK_SIGNAL_FUNC(podzapxdk_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.suma_pod.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMA]),(gpointer)E_SUMA);


GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введенной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(podzapxdk_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(podzapxdk_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(podzapxdk_v_knopka),&data);
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

gboolean   podzapxdk_v_key_press(GtkWidget *widget,GdkEventKey *event,class podzapxdk_data *data)
{

//printf("podzapxdk_v_key_press\n");
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
void  podzapxdk_v_knopka(GtkWidget *widget,class podzapxdk_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    if( podzapxdk_zapis(data) != 0)
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

void    podzapxdk_v_vvod(GtkWidget *widget,class podzapxdk_data *data)
{
//short d,m,g;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("podzapxdk_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAP:
    data->datap.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_SUMA:
    data->suma_pod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int podzapxdk_zapis(class podzapxdk_data *data)
{
short d,m,g;

if(iceb_u_rsdat(&d,&m,&g,data->datap.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата !"),data->window);
  return(1);
 }
        

if(iceb_u_sravmydat(d,m,g,rec.dd,rec.md,rec.gd) < 0)
 {
  iceb_menu_soob(gettext("Дата меньше чем дата документа ! Подтверждение невозможно !"),data->window);
  return(1);
 }


char strsql[512];

if(iceb_pbpds(m,g,data->window) != 0)
  return(1);

time_t vrem;
time(&vrem);

if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0)
 sprintf(strsql,"insert into Pzpd \
 values (0,'%d-%02d-%02d','%d-%02d-%02d','%s',%s,%d,%ld)",
 rec.gd,rec.md,rec.dd,g,m,d,rec.nomdk.ravno(),data->suma_pod.ravno(),iceb_getuid(data->window),vrem);   

if(iceb_u_SRAV(data->tablica,"Pltt",0) == 0)
  sprintf(strsql,"insert into Pzpd \
  values (1,'%d-%02d-%02d','%d-%02d-%02d','%s',%s,%d,%ld)",
  rec.gd,rec.md,rec.dd,g,m,d,rec.nomdk.ravno(),data->suma_pod.ravno(),iceb_getuid(data->window),vrem);

if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
 return(1);

provpdw(data->tablica,data->window);

return(0);

}
