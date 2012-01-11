/*$Id: l_doksmat_p.c,v 1.11 2011-02-21 07:35:52 sasa Exp $*/
/*25.12.2007	24.06.2004	Белых А.И.	l_doksmat_p.c
Меню задания реквизитов поиска документов
*/
#include        <stdlib.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include        "l_doksmat.h"

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
  
  E_NOMDOK,
  E_DATAN,
  E_DATAK,
  E_KONTR,
  E_KODOP,
  E_SKLAD, 
  E_PR,
  E_DOVER,
  E_NOMNALNAK,
  KOLENTER  
 };

class doksmat_p_data
 {
  public:
  doksmat_poi  *rpoi;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt;
  short kl_shift;
  
  
  doksmat_p_data() //Конструктор
   {
    kl_shift=0;
  
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      
      if(i == E_PR)
        continue;
      
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
     }
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      if(i == E_PR)
       {
        rpoi->pr=0;
        gtk_option_menu_set_history(GTK_OPTION_MENU(opt),rpoi->pr);
        continue;
       }
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
     }
   }
 };




gboolean   doksmat_p_key_press(GtkWidget *widget,GdkEventKey *event,class doksmat_p_data *data);
void    doksmat_p_vvod(GtkWidget *widget,class doksmat_p_data *data);
void  doksmat_p_knopka(GtkWidget *widget,class doksmat_p_data *data);
void   doksmat_p_rekviz(class doksmat_p_data *data);
void  l_doksmat_p_e_knopka(GtkWidget *widget,class doksmat_p_data *data);

void            doksmat_get_pm0(GtkWidget*,int*);
void            doksmat_get_pm1(GtkWidget*,int*);
void            doksmat_get_pm2(GtkWidget*,int*);

extern char *name_system;
extern SQL_baza bd;


int l_doksmat_p(class doksmat_poi *datap,GtkWidget *wpredok)
{
doksmat_p_data data;
data.rpoi=datap;

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Поиск"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(doksmat_p_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=gtk_label_new(gettext("Поиск"));

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
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

sprintf(strsql,"%s (,,)",gettext("Номер документа"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_NOMDOK] = gtk_entry_new_with_max_length (100);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), label, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMDOK]), "activate",GTK_SIGNAL_FUNC(doksmat_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rpoi->nomdok.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMDOK]),(gpointer)E_NOMDOK);


sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(l_doksmat_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);
tooltips_enter[E_DATAN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAN],data.knopka_enter[E_DATAN],gettext("Выбор даты начала отчёта"),NULL);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], FALSE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(doksmat_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rpoi->datan.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);


sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(l_doksmat_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);
tooltips_enter[E_DATAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAK],data.knopka_enter[E_DATAK],gettext("Выбор даты конца отчёта"),NULL);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], FALSE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(doksmat_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rpoi->datak.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);


sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KONTR]),"clicked",GTK_SIGNAL_FUNC(l_doksmat_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KONTR]),(gpointer)E_KONTR);
tooltips_enter[E_KONTR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KONTR],data.knopka_enter[E_KONTR],gettext("Выбор контрагента"),NULL);

data.entry[E_KONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KONTR]), "activate",GTK_SIGNAL_FUNC(doksmat_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rpoi->kontr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KONTR]),(gpointer)E_KONTR);


sprintf(strsql,"%s (,,)",gettext("Код операции"));
data.knopka_enter[E_KODOP]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.knopka_enter[E_KODOP], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODOP]),"clicked",GTK_SIGNAL_FUNC(l_doksmat_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODOP]),(gpointer)E_KODOP);
tooltips_enter[E_KODOP]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODOP],data.knopka_enter[E_KODOP],gettext("Выбор операции"),NULL);

data.entry[E_KODOP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODOP]), "activate",GTK_SIGNAL_FUNC(doksmat_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.rpoi->kodop.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODOP]),(gpointer)E_KODOP);


sprintf(strsql,"%s (,,)",gettext("Склад"));
data.knopka_enter[E_SKLAD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.knopka_enter[E_SKLAD], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SKLAD]),"clicked",GTK_SIGNAL_FUNC(l_doksmat_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SKLAD]),(gpointer)E_SKLAD);
tooltips_enter[E_SKLAD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SKLAD],data.knopka_enter[E_SKLAD],gettext("Выбор склада"),NULL);

data.entry[E_SKLAD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.entry[E_SKLAD], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SKLAD]), "activate",GTK_SIGNAL_FUNC(doksmat_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SKLAD]),data.rpoi->skl.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SKLAD]),(gpointer)E_SKLAD);

sprintf(strsql,"%s (,,)",gettext("Доверенность"));
data.entry[E_DOVER] = gtk_entry_new_with_max_length (100);

label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DOVER]), label, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox[E_DOVER]), data.entry[E_DOVER], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DOVER]), "activate",GTK_SIGNAL_FUNC(doksmat_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DOVER]),data.rpoi->dover.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DOVER]),(gpointer)E_DOVER);

sprintf(strsql,"%s (,,)",gettext("Номер нал.нак."));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMNALNAK]), label, FALSE, FALSE,1);

data.entry[E_NOMNALNAK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_NOMNALNAK]), data.entry[E_NOMNALNAK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMNALNAK]), "activate",GTK_SIGNAL_FUNC(doksmat_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMNALNAK]),data.rpoi->nomnalnak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMNALNAK]),(gpointer)E_NOMNALNAK);



label=gtk_label_new(gettext("Приход/расход"));
gtk_box_pack_start (GTK_BOX (hbox[E_PR]), label, FALSE, FALSE,1);

data.opt = gtk_option_menu_new();
GtkWidget *menu = gtk_menu_new();

GtkWidget *item;
item = gtk_menu_item_new_with_label (gettext("Все записи"));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(doksmat_get_pm0),&data.rpoi->pr);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (gettext("Толька приходы"));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(doksmat_get_pm1),&data.rpoi->pr);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (gettext("Толька расходы"));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(doksmat_get_pm2),&data.rpoi->pr);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

gtk_option_menu_set_menu (GTK_OPTION_MENU (data.opt), menu);

gtk_option_menu_set_history(GTK_OPTION_MENU(data.opt),data.rpoi->pr);

gtk_box_pack_start (GTK_BOX (hbox[E_PR]), data.opt, FALSE, FALSE,1);



GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(doksmat_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F3 %s",gettext("Реквизиты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Просмотр нужных реквизитов"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]),"clicked",GTK_SIGNAL_FUNC(doksmat_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE,1);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(doksmat_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(doksmat_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(0);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_doksmat_p_e_knopka(GtkWidget *widget,class doksmat_p_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rpoi->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rpoi->datan.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->rpoi->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rpoi->datak.ravno());
      
    return;  

  case E_KONTR:

    if(iceb_l_kontr(1,&kod,&naim,data->window) == 0)
      data->rpoi->kontr.z_plus(kod.ravno());
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rpoi->kontr.ravno_toutf());
      
    return;  

  case E_KODOP:

     vib_kodop_mu(&data->rpoi->kodop,data->window);
     
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->rpoi->kodop.ravno_toutf());
      
    return;  

  case E_SKLAD:

    if(iceb_l_sklad(1,&kod,&naim,data->window) == 0)
      data->rpoi->skl.z_plus(kod.ravno());
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rpoi->skl.ravno_toutf());
      
    return;  
   

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   doksmat_p_key_press(GtkWidget *widget,GdkEventKey *event,class doksmat_p_data *data)
{

//printf("doksmat_p_key_press\n");
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
void  doksmat_p_knopka(GtkWidget *widget,class doksmat_p_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(data->rpoi->datan.getdlinna() > 1)
     if(iceb_rsdatp_str(&data->rpoi->datan,&data->rpoi->datak,data->window) != 0)
      return;
    
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->rpoi->metka_poi=1;
    return;  

  case FK3:
    doksmat_p_rekviz(data);
    return;

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->rpoi->metka_poi=0;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    doksmat_p_vvod(GtkWidget *widget,class doksmat_p_data *data)
{
//short d,m,g;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("doksmat_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rpoi->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAK:
    data->rpoi->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KONTR:
    data->rpoi->kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KODOP:
    data->rpoi->kodop.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SKLAD:
    data->rpoi->skl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DOVER:
    data->rpoi->dover.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOMNALNAK:
    data->rpoi->nomnalnak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

/************
  case E_PR:
    data->rpoi->pr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
*****************/

  case E_NOMDOK:
    data->rpoi->nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;

if(enter == E_PR)
  enter+=1;
  
if(enter >= KOLENTER)
enter=0;


gtk_widget_grab_focus(data->entry[enter]);
 
}
/********************/
/*Работа с реквизитами*/
/***********************/
void   doksmat_p_rekviz(class doksmat_p_data *data)
{
short nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Реквизиты"));

zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Просмотр списка контрагентов"));
punkt_m.plus(gettext("Просмотр списка операций приходов"));
punkt_m.plus(gettext("Просмотр списка операций расходов"));
punkt_m.plus(gettext("Просмотр списка складов"));

nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);



iceb_u_str kod;
iceb_u_str naikod;

switch (nomer)
 {
  case -1:
    return;
    
  case 0:
    iceb_vibrek(0,"Kontragent",&data->rpoi->kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rpoi->kontr.ravno_toutf());
    break;

  case 1:
    iceb_vibrek(0,"Prihod",&data->rpoi->kodop,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->rpoi->kodop.ravno_toutf());
    break;

  case 2:
    iceb_vibrek(0,"Rashod",&data->rpoi->kodop,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->rpoi->kodop.ravno_toutf());
    break;

  case 3:
    iceb_vibrek(0,"Sklad",&data->rpoi->skl,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rpoi->skl.ravno_toutf());
    break;

 }

}
/**********************/
/*Читаем              */
/**********************/

void   doksmat_get_pm0(GtkWidget *widget,int *data)
{
*data=0;
printf("doksmat_get_pm0-%d\n",*data);
}
void   doksmat_get_pm1(GtkWidget *widget,int *data)
{
*data=1;
printf("doksmat_get_pm1-%d\n",*data);
}
void   doksmat_get_pm2(GtkWidget *widget,int *data)
{
*data=2;
printf("doksmat_get_pm2-%d\n",*data);
}
