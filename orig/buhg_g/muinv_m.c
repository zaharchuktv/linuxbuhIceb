/*$Id: muinv_m.c,v 1.15 2011-02-21 07:35:55 sasa Exp $*/
/*16.10.2007	21.11.2004	Белых А.И.	muinv_m.c
Меню для расчёта инвентаризационной ведомости
*/
#include "buhg_g.h"
#include "muinv.h"

enum
{
  FK2,
  FK3,
  FK4,
  FK5,
  FK10,
  KOL_F_KL
};

enum
 {
  E_SKLAD,
  E_GRUPA,
  E_SHET,
  E_KODMAT,
  E_DATAO,
  KOLENTER  
 };

class muinv_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *radiobutton[2];
  short kl_shift;
  short voz;      //0-начать расчет 1 нет
  class muinv_data *rk;
  
  muinv_m_data() //Конструктор
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


gboolean   muinv_v_key_press(GtkWidget *widget,GdkEventKey *event,class muinv_m_data *data);
void    muinv_v_vvod(GtkWidget *widget,class muinv_m_data *data);
void  muinv_v_knopka(GtkWidget *widget,class muinv_m_data *data);
void   muinv_rekviz(class muinv_m_data *data);
void  muinvw_v_e_knopka(GtkWidget *widget,class muinv_m_data *data);
void muinv_konf(GtkWidget *wpredok);

void       muinv_radio0(GtkWidget *,class muinv_m_data *);
void       muinv_radio1(GtkWidget *,class muinv_m_data *);

extern char *name_system;
extern SQL_baza bd;
extern char *organ;

int   muinv_m(class muinv_data *rek_ras)
{
muinv_m_data data;

data.rk=rek_ras;

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать инвентаризационные ведомости"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(muinv_v_key_press),&data);

iceb_u_str repl;
repl.new_plus(organ);
repl.ps_plus(gettext("Распечатать инвентаризационные ведомости"));

GtkWidget *label=gtk_label_new(repl.ravno_toutf());

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *hboxradio = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

//Вставляем радиокнопки
GSList *group;

data.radiobutton[0]=gtk_radio_button_new_with_label(NULL,gettext("Все карточки"));
if(data.rk->metkainnom == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[0]),TRUE); //Устанавливем активной кнопку
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton[0]), "clicked",GTK_SIGNAL_FUNC(muinv_radio0),&data);
//gtk_object_set_user_data(GTK_OBJECT(data.radiobutton0),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton[0]));

data.radiobutton[1]=gtk_radio_button_new_with_label(group,gettext("Только карточки с инвентарными номерами"));
if(data.rk->metkainnom == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[1]),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data.radiobutton[1]), "clicked",GTK_SIGNAL_FUNC(muinv_radio1),&data);
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[1], TRUE, TRUE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxradio);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s (,,)",gettext("Склад"));
data.knopka_enter[E_SKLAD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.knopka_enter[E_SKLAD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SKLAD]),"clicked",GTK_SIGNAL_FUNC(muinvw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SKLAD]),(gpointer)E_SKLAD);
tooltips_enter[E_SKLAD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SKLAD],data.knopka_enter[E_SKLAD],gettext("Выбор склада"),NULL);

data.entry[E_SKLAD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.entry[E_SKLAD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SKLAD]), "activate",GTK_SIGNAL_FUNC(muinv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SKLAD]),data.rk->sklad.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SKLAD]),(gpointer)E_SKLAD);

sprintf(strsql,"%s (,,)",gettext("Группа"));
data.knopka_enter[E_GRUPA]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.knopka_enter[E_GRUPA], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_GRUPA]),"clicked",GTK_SIGNAL_FUNC(muinvw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_GRUPA]),(gpointer)E_GRUPA);
tooltips_enter[E_GRUPA]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_GRUPA],data.knopka_enter[E_GRUPA],gettext("Выбор группы"),NULL);

data.entry[E_GRUPA] = gtk_entry_new_with_max_length (100);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.entry[E_GRUPA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GRUPA]), "activate",GTK_SIGNAL_FUNC(muinv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUPA]),data.rk->grupa.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GRUPA]),(gpointer)E_GRUPA);

sprintf(strsql,"%s (,,)",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(muinvw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET] = gtk_entry_new_with_max_length (100);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(muinv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk->shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

sprintf(strsql,"%s (,,)",gettext("Код материалла"));
data.knopka_enter[E_KODMAT]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KODMAT]), data.knopka_enter[E_KODMAT], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODMAT]),"clicked",GTK_SIGNAL_FUNC(muinvw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODMAT]),(gpointer)E_KODMAT);
tooltips_enter[E_KODMAT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODMAT],data.knopka_enter[E_KODMAT],gettext("Выбор материалла"),NULL);

data.entry[E_KODMAT] = gtk_entry_new_with_max_length (100);
gtk_box_pack_start (GTK_BOX (hbox[E_KODMAT]), data.entry[E_KODMAT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODMAT]), "activate",GTK_SIGNAL_FUNC(muinv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODMAT]),data.rk->kodmat.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODMAT]),(gpointer)E_KODMAT);

sprintf(strsql,"%s",gettext("Дата остатка"));
data.knopka_enter[E_DATAO]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAO]), data.knopka_enter[E_DATAO], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAO]),"clicked",GTK_SIGNAL_FUNC(muinvw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAO]),(gpointer)E_DATAO);
tooltips_enter[E_DATAO]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAO],data.knopka_enter[E_DATAO],gettext("Выбор даты"),NULL);

//sprintf(strsql,"%s",gettext("Дата остатка"));
//label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_DATAO] = gtk_entry_new_with_max_length (10);
//gtk_box_pack_start (GTK_BOX (hbox[E_DATAO]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAO]), data.entry[E_DATAO], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAO]), "activate",GTK_SIGNAL_FUNC(muinv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAO]),data.rk->datao.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAO]),(gpointer)E_DATAO);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Расчет"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчет"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(muinv_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Реквизиты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Просмотр нужных реквизитов"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]),"clicked",GTK_SIGNAL_FUNC(muinv_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(muinv_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F5 %s",gettext("Настройка"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Ввод информации, которая будет распечатана в отчёте"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]),"clicked",GTK_SIGNAL_FUNC(muinv_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK5], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(muinv_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);

}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  muinv_radio0(GtkWidget *widget,class muinv_m_data *data)
{
//g_print("muinv_radio0\n");

if(data->rk->metkainnom == 0)
  return;
data->rk->metkainnom=0;

}
void  muinv_radio1(GtkWidget *widget,class muinv_m_data *data)
{
//g_print("muinv_radio1\n");
if(data->rk->metkainnom == 1)
  return;
data->rk->metkainnom=1;
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  muinvw_v_e_knopka(GtkWidget *widget,class muinv_m_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case E_DATAO:

    if(iceb_calendar(&data->rk->datao,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAO]),data->rk->datao.ravno());
      
    return;  



  case E_SKLAD:
    iceb_vibrek(0,"Sklad",&data->rk->sklad,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk->sklad.ravno_toutf());
    return;

  case E_SHET:
    iceb_vibrek(0,"Plansh",&data->rk->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno_toutf());
    return;

  case E_GRUPA:
    iceb_vibrek(0,"Grup",&data->rk->grupa,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPA]),data->rk->grupa.ravno_toutf());
    return;



  case E_KODMAT:
    if(l_mater(1,&kod,&naim,0,0,data->window) == 0)
      data->rk->kodmat.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODMAT]),data->rk->kodmat.ravno_toutf());
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   muinv_v_key_press(GtkWidget *widget,GdkEventKey *event,class muinv_m_data *data)
{

//printf("muinv_v_key_press\n");
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

  case GDK_F5:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
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
void  muinv_v_knopka(GtkWidget *widget,class muinv_m_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(data->rk->datao.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Не верно введена дата !"),data->window);
      return;
     }
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case FK3:
    muinv_rekviz(data);

    return;

  case FK4:
    data->clear_rek();
    return;

  case FK5:
     muinv_konf(data->window);
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

void    muinv_v_vvod(GtkWidget *widget,class muinv_m_data *data)
{
//short d,m,g;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("muinv_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SKLAD:
    data->rk->sklad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_GRUPA:
    data->rk->grupa.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


  case E_KODMAT:
    data->rk->kodmat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHET:
    data->rk->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAO:
    data->rk->datao.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/********************/
/*Работа с реквизитами*/
/***********************/
void   muinv_rekviz(class muinv_m_data *data)
{
short nomer=0;

iceb_u_str titl;
iceb_u_str zamuinvlovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Реквизиты"));

zamuinvlovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Просмотр списка счетов"));
punkt_m.plus(gettext("Просмотр списка материаллов"));
punkt_m.plus(gettext("Просмотр списка групп"));
punkt_m.plus(gettext("Просмотр списка складов"));

nomer=iceb_menu_mv(&titl,&zamuinvlovok,&punkt_m,nomer,data->window);



iceb_u_str kod;
iceb_u_str naikod;

switch (nomer)
 {
  case -1:
    return;
    
  case 0:
    iceb_vibrek(0,"Plansh",&data->rk->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),iceb_u_toutf(data->rk->shet.ravno_toutf()));
    break;


  case 1:
     if(l_mater(1,&kod,&naikod,0,0,NULL) == 0)
      {
       data->rk->kodmat.z_plus(kod.ravno());
       gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODMAT]),iceb_u_toutf(data->rk->kodmat.ravno_toutf()));
      }
    break;

  case 2:
    iceb_vibrek(0,"Grup",&data->rk->grupa,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPA]),iceb_u_toutf(data->rk->grupa.ravno_toutf()));
    break;

  case 3:
    iceb_vibrek(0,"Sklad",&data->rk->sklad,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),iceb_u_toutf(data->rk->sklad.ravno_toutf()));
    break;

 }

}

/*******************/
/*Работа с настроечными файлами*/
/*********************/

void muinv_konf(GtkWidget *wpredok)
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Настройка начала документа"));//0
punkt_m.plus(gettext("Настройка конца инвентаризационной ведомости"));//1
punkt_m.plus(gettext("Настройка конца Приложения 12"));//2


int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);

switch(nomer)
 {
  case 0:
    iceb_f_redfil("inventm.alx",0,wpredok);
    break;
  case 1:
    iceb_f_redfil("inved_end.alx",0,wpredok);
    break;
  case 2:
    iceb_f_redfil("in_d12_end.alx",0,wpredok);
    break;



 }
}
