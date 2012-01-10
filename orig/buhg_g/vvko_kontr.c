/*$Id: vvko_kontr.c,v 1.18 2011-02-21 07:35:58 sasa Exp $*/
/*18.01.2010	13.02.2006	Белых А.И.	vvko_kontr.c
Ввод в кассовый ордер контрагента
*/
#include "buhg_g.h"
#include <unistd.h>
enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KONTR,
  E_SUMA,
  E_KOMENT,
  KOLENTER  
 };

class vvko_kontr_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *label_naim_kontr;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  //Ключевые реквизиты докумета
  iceb_u_str datad_k;
  iceb_u_str nomdok_k;
  iceb_u_str kassa_k;
  int tipz; //не корректируемый реквизит  
  iceb_u_str kontr_k;
  class iceb_u_str shetd;    
  //Реквизиты записи
  iceb_u_str kontr;
  iceb_u_str suma;
  class iceb_u_str koment;
  vvko_kontr_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_data();
   }

  void read_rek()
   {
    koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT]))));
    kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR]))));
    suma.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SUMA]))));
   }
  void clear_data()
   {
    shetd.new_plus("");
    kontr.new_plus("");
    suma.new_plus("");
    koment.new_plus("");
   }
 };

gboolean   vvko_kontr_v_key_press(GtkWidget *widget,GdkEventKey *event,class vvko_kontr_data *data);
void    vvko_kontr_v_vvod(GtkWidget *widget,class vvko_kontr_data *data);
void  vvko_kontr_v_knopka(GtkWidget *widget,class vvko_kontr_data *data);
void vvko_kontr_kkvt(char *skod,char *nkod,GtkWidget *wpredok);
int vvko_kontr_pk(char *kod,GtkWidget *wpredok);

void  vvko_kontr_v_e_knopka(GtkWidget *widget,class vvko_kontr_data *data);
int vvko_kontr_zap(class vvko_kontr_data *data);

extern char *name_system;
extern SQL_baza bd;
extern char	*shrt;    /*Счета розничной торговли*/


int vvko_kontr(const char *datad, //дата документа
const char *nomdok,
const char *kassa,
int tipz,
iceb_u_str *kontr, //приходит корректируемый - уходит записанный
const char *shetd,
GtkWidget *wpredok)
{
class vvko_kontr_data data;
char strsql[512];
iceb_u_str kikz;
iceb_u_str naim_kontr;
naim_kontr.plus("");

data.datad_k.plus(datad);
data.nomdok_k.plus(nomdok);
data.kassa_k.plus(kassa);
data.tipz=tipz; //меню для работы только с приходными документами
data.kontr_k.new_plus(kontr->ravno());
data.shetd.new_plus(shetd);

if(data.kontr_k.getdlinna() > 1) //корректировка шапки документа
 {
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select suma,ktoi,vrem,koment from Kasord1 where nomd='%s' and kassa=%d and \
tp=%d and god=%d and kontr='%s'",
  data.nomdok_k.ravno(),data.kassa_k.ravno_atoi(),data.tipz,data.datad_k.ravno_god(),
  data.kontr_k.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }

  data.suma.new_plus(row[0]);
  kikz.plus(iceb_kikz(row[1],row[2],wpredok));
  data.koment.new_plus(row[3]);
  //Узнаём наименование контрагента
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data.kontr_k.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim_kontr.new_plus(row[0]);


}

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.kontr_k.getdlinna() > 1)
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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(vvko_kontr_v_key_press),&data);

iceb_u_str zagolov;
if(data.kontr_k.getdlinna() <= 1)
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

GtkWidget *separator1=gtk_hseparator_new();
gtk_container_add (GTK_CONTAINER (vbox), separator1);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];


sprintf(strsql,"%s",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KONTR]),"clicked",GTK_SIGNAL_FUNC(vvko_kontr_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KONTR]),(gpointer)E_KONTR);
tooltips_enter[E_KONTR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KONTR],data.knopka_enter[E_KONTR],gettext("Выбор контрагента."),NULL);

data.entry[E_KONTR] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KONTR]), "activate",GTK_SIGNAL_FUNC(vvko_kontr_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.kontr_k.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KONTR]),(gpointer)E_KONTR);

data.label_naim_kontr=gtk_label_new(naim_kontr.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.label_naim_kontr,TRUE, TRUE, 0);

sprintf(strsql,"%s",gettext("Сумма"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), label, FALSE, FALSE, 0);

data.entry[E_SUMA] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMA]), "activate",GTK_SIGNAL_FUNC(vvko_kontr_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.suma.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMA]),(gpointer)E_SUMA);

sprintf(strsql,"%s",gettext("Коментарий"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(vvko_kontr_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);


GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введённой в меню информации."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(vvko_kontr_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(vvko_kontr_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();
if(data.voz == 0)
 kontr->new_plus(data.kontr.ravno());
return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  vvko_kontr_v_e_knopka(GtkWidget *widget,class vvko_kontr_data *data)
{
class iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case E_KONTR:

    if(iceb_poi_kontr(&data->kontr,&naim,data->window) != 0)
       return;
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->kontr.ravno_toutf());
    gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),naim.ravno_toutf());
    
/********
    iceb_vibrek(1,"Kontragent",&data->kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->kontr.ravno_toutf());
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->kontr.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),iceb_u_toutf(strsql));
     }
    else
      gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),"");
**********/
    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vvko_kontr_v_key_press(GtkWidget *widget,GdkEventKey *event,class vvko_kontr_data *data)
{

//printf("vvko_kontr_v_key_press\n");
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
void  vvko_kontr_v_knopka(GtkWidget *widget,class vvko_kontr_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(vvko_kontr_zap(data) != 0)
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

void    vvko_kontr_v_vvod(GtkWidget *widget,class vvko_kontr_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("vvko_kontr_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SUMA:
    data->suma.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KONTR:

    data->kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->kontr.ravno());


    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),iceb_u_toutf(strsql));
     }    
    else
      gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),"");
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/*********/
/*Запись*/
/*********/
int vvko_kontr_zap(class vvko_kontr_data *data)
{
if(data->kontr.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён код контрагента !"),data->window);
  return(1);
 }
if(data->suma.ravno_atof() == 0.)
 {
  iceb_menu_soob(gettext("Не введена сумма !"),data->window);
  return(1);
 }
char strsql[512];

//Проверяем есть ли в списке контрагентов
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->kontr.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  iceb_menu_soob(gettext("Не найден код контрагента в общем списке !"),data->window);
  return(1);
 }


if(iceb_u_SRAV(data->kontr_k.ravno(),data->kontr.ravno(),0) != 0)
 {
   /*Проверяем может он уже введен в ордер*/
   sprintf(strsql,"select kontr from Kasord1 where kassa=%d and \
god=%d and tp=%d and nomd='%s' and kontr='%s'",
   data->kassa_k.ravno_atoi(),data->datad_k.ravno_god(),data->tipz,data->nomdok_k.ravno(),data->kontr.ravno());
   if(iceb_sql_readkey(strsql,data->window) >= 1)
    {
     sprintf(strsql,gettext("Контрагент %s уже введён !"),data->kontr.ravno());
     iceb_menu_soob(strsql,data->window);
     return(1);
    }
 }
 
int kodv=0;

short dd,md,gd;
iceb_u_rsdat(&dd,&md,&gd,data->datad_k.ravno(),1);
if((kodv=zapzkorw(data->kassa_k.ravno(),data->tipz,dd,md,gd,data->nomdok_k.ravno(),data->kontr.ravno(),
 data->suma.ravno_atof(),data->kontr_k.ravno(),data->koment.ravno_filtr(),data->window)) != 0)
  {
   if(kodv == 1)
    {
     sprintf(strsql,gettext("Контрагент %s уже введён !"),data->kontr.ravno());
     iceb_menu_soob(strsql,data->window);
    }
   return(1);
 
  }

struct OPSHET   shetv;
iceb_prsh(data->shetd.ravno(),&shetv,data->window);
if(shetv.saldo == 3)
 {
  sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",data->shetd.ravno(),data->kontr.ravno());
  if(iceb_sql_readkey(strsql,data->window) <= 0)
   {
    sprintf(strsql,"insert into Skontr (ns,kodkon,ktoi,vrem) values ('%s','%s',%d,%ld)",
    data->shetd.ravno(),
    data->kontr.ravno(),
    iceb_getuid(data->window),time(NULL));
    if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
       return(1);
   }
 }

return(0);

}
