/*$Id: l_mater_v.c,v 1.28 2011-01-13 13:49:50 sasa Exp $*/
/*01.03.2009	05.05.2004	Белых А.И.	iceb_l_mater_v.c
Ввод и корректировка материалов
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
#include "l_mater.h"

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };
enum
 {
  E_KOD,
  E_GRUP,
  E_NAIM,
  E_CENA,
  E_MCENA,
  E_EI,
  E_KRAT,
  E_KODTAR,
  E_FAS,
  E_HTRIX,
  E_ARTIK,
  E_KRIOST,
  KOLENTER  
 };

class  mater_data_vvod
 {
  public:
  GtkWidget *window;
  GtkWidget *window_grup;
  GtkWidget *window_ei;
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka_pv[KOL_PFK];
  int voz;
  class iceb_u_str kodmat;
  class iceb_u_str naimkodmat;
  
  short     kl_shift; //0-отжата 1-нажата  
  
  mater_rek vvod;
  mater_data_vvod()
   {
    voz=1;
    kl_shift=0;
    kodmat.plus("");
    naimkodmat.plus("");  
   }


 };
 
gboolean   mater_v_key_press(GtkWidget *widget,GdkEventKey *event,class mater_data_vvod *data);
void  mater_v_knopka(GtkWidget *widget,class mater_data_vvod *data);
void    mater_v_vvod(GtkWidget *widget,class mater_data_vvod *data);
int mater_pk(const char *god,GtkWidget*);
int mater_zap(class mater_data_vvod *data);

void  grupa_get(GtkWidget *widget,class mater_data_vvod *data);
void  ediz_get(GtkWidget *widget,class mater_data_vvod *data);
int mater_pnaim(const char *naim,GtkWidget *wpredok);


int l_mater_v(class iceb_u_str *kodmat,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str kikz;
class  mater_data_vvod data;
data.kodmat.new_plus(kodmat->ravno());

printf("l_mater_v\n");
    
data.kl_shift=0;
data.vvod.clear_zero();
SQL_str row;
SQLCURSOR cur;

if(data.kodmat.getdlinna() > 1)
 {
  data.vvod.kod.new_plus(data.kodmat.ravno());
  sprintf(strsql,"select * from Material where kodm=%s",data.kodmat.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    iceb_menu_soob(&repl,data.window);
    return(1);
   }
  data.vvod.naim.new_plus(row[2]);
  data.naimkodmat.new_plus(row[2]);
  if(row[6][0] != '0')
    data.vvod.cena.new_plus(row[6]);
  if(row[7][0] != '0')
   data.vvod.krat.new_plus(row[7]);
  if(row[9][0] != '0')
    data.vvod.kodtar.new_plus(row[9]);
  if(row[8][0] != '0')
   data.vvod.fas.new_plus(row[8]);
  data.vvod.htrix.new_plus(row[3]);
  data.vvod.mcena=atoi(row[12]);
  data.vvod.artik.new_plus(row[14]);
  if(row[5][0] != '0')
   data.vvod.kriost.new_plus(row[5]);
  data.vvod.grupa.new_plus(row[1]);
  data.vvod.ei.new_plus(row[4]);

  kikz.plus(iceb_kikz(row[10],row[11],wpredok));
  
  sprintf(strsql,"select naik from Edizmer where kod='%s'",data.vvod.ei.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    data.vvod.ei.plus(" ");
    data.vvod.ei.plus(row[0]);
   }

  sprintf(strsql,"select naik from Grup where kod='%s'",data.vvod.grupa.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    data.vvod.grupa.plus(" ");
    data.vvod.grupa.plus(row[0]);
   }
 }
else
 {
//  sprintf(strsql,"%d",iceb_get_new_kod("Material","kodm",0));
  sprintf(strsql,"%d",nomkmw(wpredok));

  data.vvod.kod.new_plus(strsql);
  
 } 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.kodmat.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));
  label=gtk_label_new(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));
  iceb_u_str repl;
  repl.plus(gettext("Корректировка записи"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno_toutf());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
//gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_widget_destroyed),&data.window);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(mater_v_key_press),&data);

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
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


label=gtk_label_new(gettext("Код"));
data.entry[E_KOD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(mater_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.vvod.kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);


label=gtk_label_new(gettext("Код группы"));
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP]), label, FALSE, FALSE, 0);

//Создаем выбор группы
/********************************************/
GList *glist=NULL;

sprintf(strsql,"select kod,naik from Grup order by naik asc");
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);

iceb_u_spisok VOD;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%2s %s",row[0],row[1]);
  VOD.plus(iceb_u_toutf(strsql));
 }

if(kolstr == 0)
 VOD.plus(iceb_u_toutf("Не введен список групп !!!"));

data.window_grup=gtk_combo_new();


for(int i=0 ; i < kolstr; i++)
  glist=g_list_append(glist,(void*)VOD.ravno(i));

gtk_combo_set_popdown_strings(GTK_COMBO(data.window_grup),glist);
gtk_combo_disable_activate(GTK_COMBO(data.window_grup));
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP]), data.window_grup, TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (GTK_COMBO(data.window_grup)->entry), "activate",GTK_SIGNAL_FUNC(grupa_get),&data);
gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(data.window_grup)->entry),data.vvod.grupa.ravno_toutf());


label=gtk_label_new(gettext("Наименование"));
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE, 0);

data.entry[E_NAIM] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(mater_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.vvod.naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);

label=gtk_label_new(gettext("Цена"));
data.entry[E_CENA] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), data.entry[E_CENA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_CENA]), "activate",GTK_SIGNAL_FUNC(mater_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA]),data.vvod.cena.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_CENA]),(gpointer)E_CENA);



label=gtk_label_new(gettext("Цена введена"));
gtk_box_pack_start (GTK_BOX (hbox[E_MCENA]), label, FALSE, FALSE, 0);

GtkWidget *opt = gtk_option_menu_new();
GtkWidget *menu = gtk_menu_new();

GtkWidget *item;
item = gtk_menu_item_new_with_label (gettext("С НДС"));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(mater_get_pm0),&data.vvod.mcena);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (gettext("Без НДС"));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(mater_get_pm1),&data.vvod.mcena);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

gtk_option_menu_set_menu (GTK_OPTION_MENU (opt), menu);

gtk_option_menu_set_history(GTK_OPTION_MENU(opt),data.vvod.mcena);

gtk_box_pack_start (GTK_BOX (hbox[E_MCENA]), opt, FALSE, FALSE, 0);



label=gtk_label_new(gettext("Единица измерения"));
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), label, FALSE, FALSE, 0);
//Создаем выбор единиц измерения
/********************************************/
GList *glist_ei=NULL;

sprintf(strsql,"select kod,naik from Edizmer order by naik asc");
kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);

iceb_u_spisok VOD1;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%-5s %s",row[0],row[1]);
  VOD1.plus(iceb_u_toutf(strsql));
 }

if(kolstr == 0)
 VOD1.plus(iceb_u_toutf("Не введен список единиц измерения !!!"));

data.window_ei=gtk_combo_new();


for(int i=0 ; i < kolstr; i++)
  glist_ei=g_list_append(glist_ei,(void*)VOD1.ravno(i));

gtk_combo_set_popdown_strings(GTK_COMBO(data.window_ei),glist_ei);
gtk_combo_disable_activate(GTK_COMBO(data.window_ei));
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.window_ei, TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (GTK_COMBO(data.window_ei)->entry), "activate",GTK_SIGNAL_FUNC(ediz_get),&data);
gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(data.window_ei)->entry),data.vvod.ei.ravno_toutf());

label=gtk_label_new(gettext("Кратность"));
data.entry[E_KRAT] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KRAT]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KRAT]), data.entry[E_KRAT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KRAT]), "activate",GTK_SIGNAL_FUNC(mater_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KRAT]),data.vvod.krat.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KRAT]),(gpointer)E_KRAT);

label=gtk_label_new(gettext("Код тары"));
data.entry[E_KODTAR] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KODTAR]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KODTAR]), data.entry[E_KODTAR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODTAR]), "activate",GTK_SIGNAL_FUNC(mater_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODTAR]),data.vvod.kodtar.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODTAR]),(gpointer)E_KODTAR);

label=gtk_label_new(gettext("Фасовка"));
data.entry[E_FAS] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_FAS]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_FAS]), data.entry[E_FAS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_FAS]), "activate",GTK_SIGNAL_FUNC(mater_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FAS]),data.vvod.fas.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_FAS]),(gpointer)E_FAS);

label=gtk_label_new(gettext("Штрих код"));
data.entry[E_HTRIX] = gtk_entry_new_with_max_length (49);
gtk_box_pack_start (GTK_BOX (hbox[E_HTRIX]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_HTRIX]), data.entry[E_HTRIX], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_HTRIX]), "activate",GTK_SIGNAL_FUNC(mater_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HTRIX]),data.vvod.htrix.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_HTRIX]),(gpointer)E_HTRIX);

label=gtk_label_new(gettext("Артикул"));
data.entry[E_ARTIK] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_ARTIK]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_ARTIK]), data.entry[E_ARTIK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ARTIK]), "activate",GTK_SIGNAL_FUNC(mater_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ARTIK]),data.vvod.artik.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ARTIK]),(gpointer)E_ARTIK);

label=gtk_label_new(gettext("Критичный остаток"));
data.entry[E_KRIOST] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KRIOST]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KRIOST]), data.entry[E_KRIOST], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KRIOST]), "activate",GTK_SIGNAL_FUNC(mater_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KRIOST]),data.vvod.kriost.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KRIOST]),(gpointer)E_KRIOST);

GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka_pv[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],data.knopka_pv[PFK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka_pv[PFK2]),"clicked",GTK_SIGNAL_FUNC(mater_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_pv[PFK2]),(gpointer)PFK2);

sprintf(strsql,"F4 %s",gettext("Код"));
data.knopka_pv[PFK4]=gtk_button_new_with_label(strsql);
tooltips[PFK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK4],data.knopka_pv[PFK4],gettext("Получить новый код материалла"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka_pv[PFK4]),"clicked",GTK_SIGNAL_FUNC(mater_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_pv[PFK4]),(gpointer)PFK4);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka_pv[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],data.knopka_pv[PFK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka_pv[PFK10]),"clicked",GTK_SIGNAL_FUNC(mater_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_pv[PFK10]),(gpointer)PFK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka_pv[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka_pv[PFK4], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka_pv[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();


if(data.voz == 0)
 kodmat->new_plus(data.vvod.kod.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   mater_v_key_press(GtkWidget *widget,GdkEventKey *event,class mater_data_vvod *data)
{
//char  bros[512];

//printf("mater_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka_pv[PFK2]),"clicked");

    return(TRUE);

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka_pv[PFK4]),"clicked");

    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka_pv[PFK10]),"clicked");

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
void  mater_v_knopka(GtkWidget *widget,class mater_data_vvod *data)
{
char bros[512];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case PFK2:
    if(mater_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
      data->window=NULL;
     }
    return;  


  case PFK4:
    sprintf(bros,"%d",iceb_get_new_kod("Material","kodm",0,data->window));
    data->vvod.kod.new_plus(bros);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD]),data->vvod.kod.ravno_toutf());
    return;
    
  case PFK10:
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    mater_v_vvod(GtkWidget *widget,class mater_data_vvod *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("mater_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->vvod.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    gtk_widget_grab_focus(GTK_COMBO(data->window_grup)->entry);
    return;

  case E_NAIM:
    data->vvod.naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_CENA:
    data->vvod.cena.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    gtk_widget_grab_focus(GTK_COMBO(data->window_ei)->entry);
    return;

  case E_KRAT:
    data->vvod.krat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KODTAR:
    data->vvod.kodtar.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_FAS:
    data->vvod.fas.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_HTRIX:
    data->vvod.htrix.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_ARTIK:
    data->vvod.artik.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KRIOST:
    data->vvod.kriost.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int mater_zap(class mater_data_vvod *data)
{
char strsql[1024];
time_t   vrem;

printf("mater_v_zap\n");

for(int i=0; i < KOLENTER; i++)
 {
  if(i == E_GRUP)
   {
    gtk_signal_emit_by_name(GTK_OBJECT(GTK_COMBO(data->window_grup)->entry),"activate");
    continue;
   }
  if(i == E_MCENA)
   {
    continue;
   }
  if(i == E_EI)
   {
    gtk_signal_emit_by_name(GTK_OBJECT(GTK_COMBO(data->window_ei)->entry),"activate");
    continue;
   }
  gtk_signal_emit_by_name(GTK_OBJECT(data->entry[i]),"activate");

 }

if(data->vvod.kod.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введён код !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }

if(data->vvod.naim.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введено наименование !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }

if(data->vvod.grupa.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введено код группы !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }

//Проверяем код группы
sprintf(strsql,"select kod from Grup where kod=%s",data->vvod.grupa.ravno_filtr_dpp());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",
  gettext("Не найдено код группы"),
  data->vvod.grupa.ravno_filtr_dpp());

  iceb_menu_soob(strsql,data->window);
  return(1);
 }

//Проверяем единицу измерения
if(data->vvod.ei.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Edizmer where kod='%s'",data->vvod.ei.ravno_filtr_dpp());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s %s !",
    gettext("Не найденa единица измерения"),
    data->vvod.ei.ravno_filtr_dpp());

    iceb_menu_soob(strsql,data->window);
    return(1);
   }
  
 }
 


short metkakor=0;
time(&vrem);

if(data->kodmat.getdlinna() <= 1)
 {

  if(mater_pk(data->vvod.kod.ravno(),data->window) != 0)
   return(1);
  
  if(mater_pnaim(data->vvod.naim.ravno(),data->window) != 0)
   return(1);
  
  sprintf(strsql,"insert into Material \
values (%s,%d,'%s','%s','%s',%.10g,%.10g,%.10g,%.10g,%d,%d,%ld,%d,%d,'%s')",
   data->vvod.kod.ravno_filtr(),
   data->vvod.grupa.ravno_atoi(),
   data->vvod.naim.ravno_filtr(),
   data->vvod.htrix.ravno_filtr(),
   data->vvod.ei.ravno_filtr_dpp(),
   data->vvod.kriost.ravno_atof(),
   data->vvod.cena.ravno_atof(),
   data->vvod.krat.ravno_atof(),
   data->vvod.fas.ravno_atof(),
   data->vvod.kodtar.ravno_atoi(),
   iceb_getuid(data->window),vrem,
   data->vvod.mcena,
   0,
   data->vvod.artik.ravno_filtr());
   
 }
else
 {
//  printf("metkazapisi=%d\n",data->metkazapisi);
  if(iceb_u_SRAV(data->naimkodmat.ravno(),data->vvod.naim.ravno(),0) != 0)
   {
    if(mater_pnaim(data->vvod.naim.ravno(),data->window) != 0)
     return(1);

   //проверяем разрешено ли оператору корректировать наименование
   class iceb_u_str spisok_log;
   if(iceb_poldan("Разрешено корректировать наименование материалла",&spisok_log,"matnast.alx",data->window) == 0)
    {
     if(spisok_log.getdlinna() > 1)
      {
        if(iceb_u_proverka(spisok_log.ravno(),iceb_u_getlogin(),0,0) != 0)
         {
          iceb_menu_soob(gettext("Вам запрещено корректировать наименование материалла !"),data->window);
          return(1);
         }
      }
    }

   }

  
  if(iceb_u_SRAV(data->kodmat.ravno(),data->vvod.kod.ravno(),0) != 0)
   {
    if(mater_pk(data->vvod.kod.ravno(),data->window) != 0)
     return(1);
    /*Проверка возможности корректировки кода*/
    if(l_mater_v_provkor(data->vvod.kod.ravno_atoi(),data->kodmat.ravno_atoi(),data->window) != 0)
     return(1);

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  sprintf(strsql,"update Material \
set \
kodm = %s,\
kodgr= %d,\
naimat='%s',\
strihkod='%s',\
ei='%s',\
kriost=%.10g,\
cenapr=%.10g,\
krat=%.10g,\
fasv=%.10g,\
kodt=%d,\
ktoz=%d,\
vrem=%ld,\
nds=%d,\
msp=%d,\
artikul='%s' \
where kodm = %s",
   data->vvod.kod.ravno_filtr(),
   data->vvod.grupa.ravno_atoi(),
   data->vvod.naim.ravno_filtr(),
   data->vvod.htrix.ravno_filtr(),
   data->vvod.ei.ravno_filtr_dpp(),
   data->vvod.kriost.ravno_atof(),
   data->vvod.cena.ravno_atof(),
   data->vvod.krat.ravno_atof(),
   data->vvod.fas.ravno_atof(),
   data->vvod.kodtar.ravno_atoi(),
   iceb_getuid(data->window),vrem,
   data->vvod.mcena,
   0,
   data->vvod.artik.ravno_filtr(),
   data->kodmat.ravno());
   
 }

//printf("mater_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);

if(metkakor == 1)
 mater_kkvt(data->kodmat.ravno_atoi(),data->vvod.kod.ravno_atoi(),data->window);

return(0);

}
/*******************************/
/*Проверка кода материалла на уже введенную запись*/
/**********************************/
int mater_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kodm from Material where kodm=%s",kod);
//printf("mater_pk-%s\n",strsql);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Такой код уже введено !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

return(0);

}

/*******************************/
/*Проверка наименования материалла на уже введенную запись*/
/**********************************/
int mater_pnaim(const char *naim,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select naimat from Material where naimat='%s'",naim);
//printf("mater_pk-%s\n",strsql);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Такое наименование уже введено !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

return(0);

}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void mater_kkvt(int skod,int nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Roznica set kodm=%d where kodm=%d and metka=1",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   iceb_u_str SOOB;
   SOOB.plus(gettext("У вас нет полномочий для выполнения этой операции !"));
   iceb_menu_soob(&SOOB,wpredok);
   return;
  }
 else
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
 }
sprintf(strsql,"update Dokummat1 set kodm=%d where kodm=%d",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

sprintf(strsql,"update Kart set kodm=%d where kodm=%d",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

sprintf(strsql,"update Uplmt set kodm=%d where kodm=%d",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

sprintf(strsql,"update Usldokum1 set kodzap=%d where kodzap=%d and metka=0",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

sprintf(strsql,"update Usldokum2 set kodzap=%d where kodzap=%d and metka=0",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

sprintf(strsql,"update Specif set kodi=%d where kodi=%d and kz=0",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

sprintf(strsql,"update Specif set kodd=%d where kodd=%d and kz=0",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

sprintf(strsql,"update Restdok1 set kodz=%d where kodz=%d and mz=0",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);


}
/*************************************/
/*Обработчик сигнала выбора группы*/
/************************************/
void  grupa_get(GtkWidget *widget,class mater_data_vvod *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;

//printf("mater_get\n");

data->vvod.grupa.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

 
sprintf(strsql,"select kod,naik from Grup where kod=%d",atoi(data->vvod.grupa.ravno()));
//printf("strsql=%s\n",strsql);

if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  data->vvod.grupa.new_plus(row[0]);
  data->vvod.grupa.plus(" ");
  data->vvod.grupa.plus(row[1]);
 }



gtk_entry_set_text(GTK_ENTRY(widget),data->vvod.grupa.ravno_toutf());
//gtk_entry_select_region(GTK_ENTRY(widget),0,strlen(kod));
//printf("grupa_get kod=%s\n",data->vvod.grupa.ravno());

gtk_widget_grab_focus(data->entry[E_NAIM]);

}
/*************************************/
/*Обработчик сигнала выбора единицы измерения*/
/************************************/
void  ediz_get(GtkWidget *widget,class mater_data_vvod *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;

//printf("voditel_get\n");

//strcpy(kod,iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
data->vvod.ei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

sprintf(strsql,"select kod,naik from Edizmer where kod='%s'",data->vvod.ei.ravno());
//printf("strsql=%s\n",strsql);

if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  data->vvod.ei.new_plus(row[0]);
  data->vvod.ei.plus(" ");
  data->vvod.ei.plus(row[1]);
 }
 

gtk_entry_set_text(GTK_ENTRY(widget),data->vvod.ei.ravno_toutf());
//gtk_entry_select_region(GTK_ENTRY(widget),0,strlen(kod));

gtk_widget_grab_focus(data->entry[E_KRAT]);

}
/*******************************************/
/*проверка возможности корректировки кода*/
/*******************************************/
int l_mater_v_provkor(int nkod,int skod,GtkWidget *wpredok)
{

char strsql[512];

/*Проверяем есть ли изделие по старому коду и входит ли в него код на который хотим заменить*/
sprintf(strsql,"select kodi from Specif where kodi=%d and kodd=%d limit 1",skod,nkod);
if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  sprintf(strsql,"%s Specif\n%s\n%s",gettext("Таблица"),
  gettext("Коды взаимно связаны!"),
  gettext("Корректировка не возможна!"));
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }
/*Проверяем есть ли издели по коду на который меняем и входит ли в него старый код*/
sprintf(strsql,"select kodi from Specif where kodi=%d and kodd=%d limit 1",nkod,skod);
if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  sprintf(strsql,"%s Specif\n%s\n%s",gettext("Таблица"),
  gettext("Коды взаимно связаны!"),
  gettext("Корректировка не возможна!"));
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }
return(0);


}
