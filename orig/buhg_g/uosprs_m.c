/*$Id: uosprs_m.c,v 1.9 2011-04-06 09:49:17 sasa Exp $*/
/*26.12.2007	26.12.2007	Белых А.И.	uosprs_m.c
Меню для ввода реквизитов для расчёта
*/
#include "buhg_g.h"
#include "uosprs.h"
enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAR,
  E_SUMA,
  E_GRUPA,
  E_INNOM,
  E_DATAD,
  E_SHETUH,
  KOLENTER  
 };

class uosprs_m_data
 {
  public:
  class uosprs_data *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt1;
  GtkWidget *opt2;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет
  GtkWidget *radiobutton[2];

  uosprs_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->datar.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAR]))));
    rk->suma.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SUMA]))));
    rk->grup.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_GRUPA]))));
    rk->innom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_INNOM]))));
    rk->datad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAD]))));
    rk->shetuh.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHETUH]))));

   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    gtk_option_menu_set_history(GTK_OPTION_MENU(opt1),0);
    gtk_option_menu_set_history(GTK_OPTION_MENU(opt2),0);
    rk->clear();
   }
 };


gboolean   uosprs_v_key_press(GtkWidget *widget,GdkEventKey *event,class uosprs_m_data *data);
void    uosprs_v_vvod(GtkWidget *widget,class uosprs_m_data *data);
void  uosprs_v_knopka(GtkWidget *widget,class uosprs_m_data *data);
void  uosprs_v_e_knopka(GtkWidget *widget,class uosprs_m_data *data);

int uosprs_m_provr(class uosprs_m_data *data);

extern char *name_system;
extern SQL_baza bd;

int   uosprs_m(class uosprs_data *rek_ras)
{
char strsql[512];


class uosprs_m_data data;

data.rk=rek_ras;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Пропорциональное распределение сумм"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uosprs_v_key_press),&data);

GtkWidget *label=gtk_label_new(gettext("Пропорциональное распределение сумм"));

GtkWidget *vbox = gtk_vbox_new (FALSE, 1);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE,1);

GtkWidget *hboxknop = gtk_hbox_new (FALSE,1);
GtkWidget *hboxopt1 = gtk_hbox_new (FALSE,1);
GtkWidget *hboxopt2 = gtk_hbox_new (FALSE,1);


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE,1);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], TRUE, TRUE,1);

gtk_box_pack_start (GTK_BOX (vbox), hboxopt1, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (vbox), hboxopt2, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (vbox), hboxknop, FALSE, FALSE,1);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s (%s)",gettext("Дата расчёта балансовой стоимости"),gettext("д.м.г"));
data.knopka_enter[E_DATAR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAR]), data.knopka_enter[E_DATAR], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAR]),"clicked",GTK_SIGNAL_FUNC(uosprs_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAR]),(gpointer)E_DATAR);
tooltips_enter[E_DATAR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAR],data.knopka_enter[E_DATAR],gettext("Выбор даты"),NULL);

data.entry[E_DATAR] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAR]), data.entry[E_DATAR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAR]), "activate",GTK_SIGNAL_FUNC(uosprs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAR]),data.rk->datar.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAR]),(gpointer)E_DATAR);


sprintf(strsql,"%s",gettext("Сумма"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), label, FALSE, FALSE,1);

data.entry[E_SUMA] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMA]), "activate",GTK_SIGNAL_FUNC(uosprs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.rk->suma.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMA]),(gpointer)E_SUMA);

sprintf(strsql,"%s (%s)",gettext("Дата документа"),gettext("д.м.г"));
data.knopka_enter[E_DATAD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.knopka_enter[E_DATAD], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAD]),"clicked",GTK_SIGNAL_FUNC(uosprs_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAD]),(gpointer)E_DATAD);
tooltips_enter[E_DATAD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAD],data.knopka_enter[E_DATAD],gettext("Выбор даты"),NULL);

data.entry[E_DATAD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.entry[E_DATAD], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAD]), "activate",GTK_SIGNAL_FUNC(uosprs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAD]),data.rk->datad.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAD]),(gpointer)E_DATAD);


sprintf(strsql,"%s (,,)",gettext("Инвентарный номер"));
data.knopka_enter[E_INNOM]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_INNOM]), data.knopka_enter[E_INNOM], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_INNOM]),"clicked",GTK_SIGNAL_FUNC(uosprs_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_INNOM]),(gpointer)E_INNOM);
tooltips_enter[E_INNOM]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_INNOM],data.knopka_enter[E_INNOM],gettext("Выбор инвентарного номера"),NULL);

data.entry[E_INNOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_INNOM]), data.entry[E_INNOM], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_INNOM]), "activate",GTK_SIGNAL_FUNC(uosprs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INNOM]),data.rk->innom.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_INNOM]),(gpointer)E_INNOM);

sprintf(strsql,"%s",gettext("Группа"));
data.knopka_enter[E_GRUPA]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.knopka_enter[E_GRUPA], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_GRUPA]),"clicked",GTK_SIGNAL_FUNC(uosprs_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_GRUPA]),(gpointer)E_GRUPA);
tooltips_enter[E_GRUPA]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_GRUPA],data.knopka_enter[E_GRUPA],gettext("Выбор группы"),NULL);

data.entry[E_GRUPA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.entry[E_GRUPA], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GRUPA]), "activate",GTK_SIGNAL_FUNC(uosprs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUPA]),data.rk->grup.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GRUPA]),(gpointer)E_GRUPA);


sprintf(strsql,"%s (,,)",gettext("Счёт учёта"));
data.knopka_enter[E_SHETUH]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHETUH]), data.knopka_enter[E_SHETUH], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHETUH]),"clicked",GTK_SIGNAL_FUNC(uosprs_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHETUH]),(gpointer)E_SHETUH);
tooltips_enter[E_SHETUH]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHETUH],data.knopka_enter[E_SHETUH],gettext("Выбор счёта"),NULL);

data.entry[E_SHETUH] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHETUH]), data.entry[E_SHETUH], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHETUH]), "activate",GTK_SIGNAL_FUNC(uosprs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETUH]),data.rk->shetuh.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHETUH]),(gpointer)E_SHETUH);




label=gtk_label_new(gettext("База для расчёта"));
gtk_box_pack_start (GTK_BOX (hboxopt1), label, FALSE, FALSE, 1);

class iceb_u_spisok bal_st;
bal_st.plus(gettext("Остаточная балансовая стоимость"));
bal_st.plus(gettext("Начальная балансовая стоимость"));

iceb_pm_vibor(&bal_st,&data.opt1,&data.rk->baza_r);
gtk_box_pack_start (GTK_BOX (hboxopt1), data.opt1, TRUE, TRUE,1);


label=gtk_label_new(gettext("Способ расчёта"));
gtk_box_pack_start (GTK_BOX (hboxopt2), label, FALSE, FALSE, 1);

class iceb_u_spisok sp_r;
sp_r.plus(gettext("По объектно"));
sp_r.plus(gettext("В целом по группе"));

iceb_pm_vibor(&sp_r,&data.opt2,&data.rk->sposob_r);
gtk_box_pack_start (GTK_BOX (hboxopt2), data.opt2, TRUE, TRUE,1);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Расчет"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчет"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(uosprs_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(uosprs_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(uosprs_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  uosprs_v_e_knopka(GtkWidget *widget,class uosprs_m_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case E_DATAR:

    if(iceb_calendar(&data->rk->datar,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAR]),data->rk->datar.ravno());
      
    return;  

  case E_DATAD:

    if(iceb_calendar(&data->rk->datad,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAD]),data->rk->datad.ravno());
      
    return;  

  case E_INNOM:

    if(l_uosin(1,&kod,&naim,data->window) == 0)
      data->rk->innom.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_INNOM]),data->rk->innom.ravno());
      
    return;  

  case E_SHETUH:

    iceb_vibrek(0,"Plansh",&data->rk->shetuh,&naim,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETUH]),data->rk->shetuh.ravno());
      
    return;  

  case E_GRUPA:

    if(l_uosgrnu(1,&kod,&naim,data->window) == 0)
      data->rk->grup.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPA]),data->rk->grup.ravno());
      
    return;  

   

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uosprs_v_key_press(GtkWidget *widget,GdkEventKey *event,class uosprs_m_data *data)
{

//printf("uosprs_v_key_press\n");
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
void  uosprs_v_knopka(GtkWidget *widget,class uosprs_m_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    if(uosprs_m_provr(data) != 0)
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

void    uosprs_v_vvod(GtkWidget *widget,class uosprs_m_data *data)
{
//short d,m,g;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("uosprs_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAR:
    data->rk->datar.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAD:
    data->rk->datad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_INNOM:
    data->rk->innom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_GRUPA:
    data->rk->grup.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


  case E_SUMA:
    data->rk->suma.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/**************************************/
/*Проверка реквизитов*/
/*******************************/
int uosprs_m_provr(class uosprs_m_data *data)
{
if(data->rk->datar.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена дата расчёта!"),data->window);
  return(1);
 }     

if(data->rk->datar.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата расчёта!"),data->window);
  return(1);
 }
if(data->rk->datad.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата документа!"),data->window);
  return(1);
 }
if(data->rk->suma.ravno_atof() == 0.)
 {
  iceb_menu_soob(gettext("Не введена сумма для распределения!"),data->window);
  return(1);
 }
if(data->rk->grup.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён код группы"),data->window);
  return(1);
 }
char strsql[512];
/*Проверяем код группы*/
sprintf(strsql,"select naik from Uosgrup where kod='%s'",data->rk->grup.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  iceb_menu_soob(gettext("Не найден код группы!"),data->window);
  return(1);
 }

if(data->rk->datad.getdlinna() > 1)
 {

  iceb_poldan("Код операции дооценки",&data->rk->kodop,"uosnast.alx",data->window);

  if(data->rk->kodop.getdlinna() <= 1)
   {
    iceb_menu_soob(gettext("В файле настройки не введено \"Код операции дооценки\"!"),data->window);
    return(1);
    
   }
  sprintf(strsql,"select kod from Uospri where kod='%s'",data->rk->kodop.ravno());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Ненайден код операции"),data->rk->kodop.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);
    
   }
/***********
  if(iceb_poldan("Н.Д.С.",strsql,"matnast.alx",data->window) != 0)
   {
    iceb_menu_soob(gettext("В файле настройки matnast.alx не найдено \"Н.Д.С.\"!"),data->window);
    return(1);
   }
  data->rk->dpnds=iceb_u_atof(strsql);
*************/
  data->rk->dpnds=iceb_pnds(data->window);
 }


return(0);

}
