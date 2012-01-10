/*$Id: l_uosdok_v.c,v 1.9 2011-02-21 07:35:53 sasa Exp $*/
/*14.12.2007	16.11.2007	Белых А.И.	l_uosdok_v.c
Ввод записи в документ "Учет основных средств"
*/


#include "buhg_g.h"

enum
{
 FK2,
 FK10,
 KOL_F_KL
};

enum
 {
  E_BAL_ST_NU,
  E_IZ_NU,
  E_BAL_ST_BU,
  E_IZ_BU,
  E_SHET_SP,
  E_CENA,
  KOLENTER  
 };

class l_uosdok_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет
  
  class iceb_u_str bal_st_nu;
  class iceb_u_str iz_nu;
  class iceb_u_str bal_st_bu;
  class iceb_u_str iz_bu;
  class iceb_u_str shetsp;
  class iceb_u_str cena;
  
  int metka_m;  
  l_uosdok_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear();
    metka_m=0;
   }

  void read_rek()
   {
    bal_st_nu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_BAL_ST_NU]))));
    iz_nu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_IZ_NU]))));
    bal_st_bu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_BAL_ST_BU]))));
    iz_bu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_IZ_BU]))));
    if(metka_m >= 1)
      shetsp.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET_SP]))));
    if(metka_m == 2)
      cena.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_CENA]))));
   }
  void clear()
   {
    bal_st_nu.new_plus("");
    iz_nu.new_plus("");
    bal_st_bu.new_plus("");
    iz_bu.new_plus("");
    shetsp.new_plus("");
    cena.new_plus("");  
   }
 };

gboolean   l_uosdok_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosdok_v_data *data);
void    l_uosdok_v_v_vvod(GtkWidget *widget,class l_uosdok_v_data *data);
void  l_uosdok_v_v_knopka(GtkWidget *widget,class l_uosdok_v_data *data);
void uosopp_kkvt(char *skod,char *nkod,GtkWidget *wpredok);
int uosopp_pk(char *kod,GtkWidget *wpredok);
void  l_uosdok_v_e_knopka(GtkWidget *widget,class l_uosdok_v_data *data);
void  l_uosdok_v_v_e_knopka(GtkWidget *widget,class l_uosdok_v_data *data);

extern char *name_system;

int l_uosdok_v(int innom, /*Инвентарный номер*/
double *bsnu,
double *iznu,
double *bsbu,
double *izbu,
class iceb_u_str *shetsp,
double *cena,
int tipz,
int metka_m, /*0-только стоимость 1-плюс счёт 2-плюс цена*/
int kto,
time_t vremz,
GtkWidget *wpredok)
{
class l_uosdok_v_data data;
char strsql[512];
class iceb_u_str kikz;

if(kto != 0)
 kikz.plus(iceb_kikz(kto,vremz,wpredok));

data.metka_m=metka_m;
if(*bsnu != 0.)
  data.bal_st_nu.new_plus(*bsnu);
if(*iznu != 0.)
  data.iz_nu.new_plus(*iznu);
if(*bsnu != 0.)
 data.bal_st_bu.new_plus(*bsbu);
if(*izbu != 0.)
  data.iz_bu.new_plus(*izbu);

data.shetsp.new_plus(shetsp->ravno());
if(*cena != 0.)
 data.cena.new_plus(*cena);
SQL_str row;
class SQLCURSOR cur;  
class iceb_u_str naim_innom("");
/*Читаем наименование инвентарного номера*/
sprintf(strsql,"select naim from Uosin where innom=%d",innom);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_innom.new_plus(row[0]);


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_uosdok_v_v_key_press),&data);


sprintf(strsql,"%s:%d/%s",gettext("Инвентарный номер"),innom,naim_innom.ravno());
class iceb_u_str zagol;
zagol.plus(strsql);
if(kikz.getdlinna() > 1)
 zagol.ps_plus(kikz.ravno());

GtkWidget *label=gtk_label_new(zagol.ravno_toutf());

GtkWidget *vbox = gtk_vbox_new (FALSE, 1);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 1);


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 1);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], FALSE, FALSE, 1);

GtkWidget *hboxknop = gtk_hbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (vbox), hboxknop, FALSE, FALSE, 1);


sprintf(strsql,"%s",gettext("Балансовая стоимость (налоговый учёт)"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_BAL_ST_NU]), label,FALSE, FALSE, 1);

data.entry[E_BAL_ST_NU] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_BAL_ST_NU]), data.entry[E_BAL_ST_NU],TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_BAL_ST_NU]), "activate",GTK_SIGNAL_FUNC(l_uosdok_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_BAL_ST_NU]),data.bal_st_nu.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_BAL_ST_NU]),(gpointer)E_BAL_ST_NU);

sprintf(strsql,"%s",gettext("Износ (налоговый учёт)"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_IZ_NU]), label,FALSE, FALSE, 1);

data.entry[E_IZ_NU] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_IZ_NU]), data.entry[E_IZ_NU],TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_IZ_NU]), "activate",GTK_SIGNAL_FUNC(l_uosdok_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IZ_NU]),data.iz_nu.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_IZ_NU]),(gpointer)E_IZ_NU);


sprintf(strsql,"%s",gettext("Балансовая стоимость (бухгалтерский учёт)"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_BAL_ST_BU]), label,FALSE, FALSE, 1);

data.entry[E_BAL_ST_BU] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_BAL_ST_BU]), data.entry[E_BAL_ST_BU],TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_BAL_ST_BU]), "activate",GTK_SIGNAL_FUNC(l_uosdok_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_BAL_ST_BU]),data.bal_st_bu.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_BAL_ST_BU]),(gpointer)E_BAL_ST_BU);

sprintf(strsql,"%s",gettext("Износ (бухгалтерский учёт)"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_IZ_BU]), label,FALSE, FALSE, 1);

data.entry[E_IZ_BU] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_IZ_BU]), data.entry[E_IZ_BU],TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_IZ_BU]), "activate",GTK_SIGNAL_FUNC(l_uosdok_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IZ_BU]),data.iz_bu.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_IZ_BU]),(gpointer)E_IZ_BU);

if(metka_m >= 1)
 {
  if(tipz == 1) 
   sprintf(strsql,"%s",gettext("Счёт получения"));
  if(tipz == 2) 
   sprintf(strsql,"%s",gettext("Счёт списания"));
/********
  label=gtk_label_new(iceb_u_toutf(strsql));
  gtk_box_pack_start (GTK_BOX (hbox[E_SHET_SP]), label,FALSE, FALSE, 1);
**********/
  data.knopka_enter[E_SHET_SP]=gtk_button_new_with_label(iceb_u_toutf(strsql));
  gtk_box_pack_start (GTK_BOX (hbox[E_SHET_SP]), data.knopka_enter[E_SHET_SP], FALSE, FALSE, 0);
  gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET_SP]),"clicked",GTK_SIGNAL_FUNC(l_uosdok_v_v_e_knopka),&data);
  gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET_SP]),(gpointer)E_SHET_SP);
  GtkTooltips *tooltips_enter=gtk_tooltips_new();
  gtk_tooltips_set_tip(tooltips_enter,data.knopka_enter[E_SHET_SP],gettext("Выбор счёта в плане счетов"),NULL);

  data.entry[E_SHET_SP] = gtk_entry_new_with_max_length (20);
  gtk_box_pack_start (GTK_BOX (hbox[E_SHET_SP]), data.entry[E_SHET_SP],TRUE, TRUE, 1);
  gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET_SP]), "activate",GTK_SIGNAL_FUNC(l_uosdok_v_v_vvod),&data);
  gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET_SP]),data.shetsp.ravno_toutf());
  gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET_SP]),(gpointer)E_SHET_SP);
 }

if(metka_m == 2)
 {
  sprintf(strsql,"%s",gettext("Цена продажи"));

  label=gtk_label_new(iceb_u_toutf(strsql));
  gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), label,FALSE, FALSE, 1);

  data.entry[E_CENA] = gtk_entry_new_with_max_length (20);
  gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), data.entry[E_CENA],TRUE, TRUE, 1);
  gtk_signal_connect(GTK_OBJECT (data.entry[E_CENA]), "activate",GTK_SIGNAL_FUNC(l_uosdok_v_v_vvod),&data);
  gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA]),data.cena.ravno());
  gtk_object_set_user_data(GTK_OBJECT(data.entry[E_CENA]),(gpointer)E_CENA);
 }
 
GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введённой в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_uosdok_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 1);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_uosdok_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 {
  *bsnu=data.bal_st_nu.ravno_atof();
  *iznu=data.iz_nu.ravno_atof();
  *bsbu=data.bal_st_bu.ravno_atof();
  *izbu=data.iz_bu.ravno_atof();
  shetsp->new_plus(data.shetsp.ravno());
  *cena=data.cena.ravno_atof();
 }

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_uosdok_v_v_e_knopka(GtkWidget *widget,class l_uosdok_v_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case E_SHET_SP:

    if(iceb_l_plansh(1,&kod,&naim,data->window) == 0)
     data->shetsp.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET_SP]),data->shetsp.ravno_toutf());
      
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uosdok_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosdok_v_data *data)
{

//printf("l_uosdok_v_v_key_press\n");
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
void  l_uosdok_v_v_knopka(GtkWidget *widget,class l_uosdok_v_data *data)
{
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

/*g_print("l_uosdok_v_v_knopka knop=%d\n",knop);*/
struct OPSHET reksh;
switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(data->shetsp.getdlinna() > 1)
     if(iceb_prsh1(data->shetsp.ravno(),&reksh,data->window) != 0)
      return;
          
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

void    l_uosdok_v_v_vvod(GtkWidget *widget,class l_uosdok_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

//g_print("l_uosdok_v_v_vvod enter=%d\n",enter);
struct OPSHET rek_shet;
switch (enter)
 {
/********
  case E_SHETU:

    break;
*********/
 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
