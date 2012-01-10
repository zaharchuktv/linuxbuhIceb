/*$Id: admin_makebaz_m.c,v 1.8 2011-09-05 08:18:27 sasa Exp $*/
/*17.10.2010	22.05.2009	Белых А.И.	admin_makebaz_m.c
Меню для разметки базы
*/
#include <errno.h>
#include <stdlib.h>
//zs
//#include  "buhg_g.h"
#include  "../headers/buhg_g.h"
//ze
#include <unistd.h>

enum
 {
  E_IMABAZ,
  E_KODIROVKA,
  E_FROMNSI,
  E_FROMDOC,
  E_TONSI,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class admin_makebaz_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str imabaz;
  static class iceb_u_str kodirovka;  
  static class iceb_u_str fromnsi;
  static class iceb_u_str fromdoc;    
  
  admin_makebaz_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    imabaz.new_plus("");
   }

  void read_rek()
   {
    imabaz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_IMABAZ]))));
    kodirovka.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KODIROVKA]))));
    fromnsi.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_FROMNSI]))));
    fromdoc.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_FROMDOC]))));
 
   }

  void clear_data()
   {
    clear();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
  void clear()
   {
    imabaz.new_plus("");
    kodirovka.new_plus("");
    fromnsi.new_plus("");
    fromdoc.new_plus("");
   }
 };

class iceb_u_str admin_makebaz_m_data::fromnsi;
class iceb_u_str admin_makebaz_m_data::kodirovka;
class iceb_u_str admin_makebaz_m_data::fromdoc;

void admin_makebaz_m_clear(class admin_makebaz_m_data *data);
void    admin_makebaz_m_vvod(GtkWidget *widget,class admin_makebaz_m_data *data);
void  admin_makebaz_m_knopka(GtkWidget *widget,class admin_makebaz_m_data *data);
gboolean   admin_makebaz_m_key_press(GtkWidget *widget,GdkEventKey *event,class admin_makebaz_m_data *data);
void admin_makebaz_m_e_knopka(GtkWidget *widget,class admin_makebaz_m_data *data);

extern char *name_system;
extern SQL_baza bd;

int admin_makebaz_m(class iceb_u_str *imabaz,class iceb_u_str *kodirovka,
class iceb_u_str *fromnsi,
class iceb_u_str *fromdoc,
GtkWidget *wpredok)
{
class admin_makebaz_m_data data;
char strsql[512];
class iceb_u_str version;
SQL_str row;
class SQLCURSOR cur;
sql_readkey(&bd,"select VERSION()",&row,&cur);
version.plus(row[0]);

if(version.ravno_atof() >= 4.1)
 if(data.kodirovka.getdlinna() <= 1)
 //zs
  //data.kodirovka.new_plus("koi8u");
  data.kodirovka.new_plus("utf8");
 //ze 
if(data.fromnsi.getdlinna() <= 1)
 data.fromnsi.plus("");
if(data.fromdoc.getdlinna() <= 1)
 data.fromdoc.plus("");
 
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Разметка базы данных для системы бух. учёта iceB"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(admin_makebaz_m_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label=NULL;
sprintf(strsql,"%s\n%s MySQL:%s",
gettext("Разметка базы данных для системы бух. учёта LinuxBuh.RU"),
gettext("Верия"),
version.ravno());

label=gtk_label_new(iceb_u_toutf(strsql));


GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);

GtkTooltips *tooltips_enter[KOLENTER];

label=gtk_label_new(gettext("Имя базы данных"));
gtk_box_pack_start (GTK_BOX (hbox[E_IMABAZ]), label, FALSE, FALSE, 0);

data.entry[E_IMABAZ] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_IMABAZ]), data.entry[E_IMABAZ], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_IMABAZ]), "activate",GTK_SIGNAL_FUNC(admin_makebaz_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IMABAZ]),data.imabaz.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_IMABAZ]),(gpointer)E_IMABAZ);

label=gtk_label_new(gettext("Кодировка хранения данных"));
gtk_box_pack_start (GTK_BOX (hbox[E_KODIROVKA]), label, FALSE, FALSE, 0);

data.entry[E_KODIROVKA] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KODIROVKA]), data.entry[E_KODIROVKA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODIROVKA]), "activate",GTK_SIGNAL_FUNC(admin_makebaz_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODIROVKA]),data.kodirovka.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODIROVKA]),(gpointer)E_KODIROVKA);


data.knopka_enter[E_FROMNSI]=gtk_button_new_with_label(gettext("Каталог откуда импортировать настроечные файлы"));
gtk_box_pack_start (GTK_BOX (hbox[E_FROMNSI]), data.knopka_enter[E_FROMNSI], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_FROMNSI]),"clicked",GTK_SIGNAL_FUNC(admin_makebaz_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_FROMNSI]),(gpointer)E_FROMNSI);
tooltips_enter[E_FROMNSI]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_FROMNSI],data.knopka_enter[E_FROMNSI],gettext("Выбор каталога"),NULL);

data.entry[E_FROMNSI] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_FROMNSI]), data.entry[E_FROMNSI], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_FROMNSI]), "activate",GTK_SIGNAL_FUNC(admin_makebaz_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FROMNSI]),data.fromnsi.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_FROMNSI]),(gpointer)E_FROMNSI);


data.knopka_enter[E_FROMDOC]=gtk_button_new_with_label(gettext("Каталог откуда импортировать документацию"));
gtk_box_pack_start (GTK_BOX (hbox[E_FROMDOC]), data.knopka_enter[E_FROMDOC], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_FROMDOC]),"clicked",GTK_SIGNAL_FUNC(admin_makebaz_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_FROMDOC]),(gpointer)E_FROMDOC);
tooltips_enter[E_FROMDOC]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_FROMDOC],data.knopka_enter[E_FROMDOC],gettext("Выбор каталога"),NULL);

data.entry[E_FROMDOC] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_FROMDOC]), data.entry[E_FROMDOC], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_FROMDOC]), "activate",GTK_SIGNAL_FUNC(admin_makebaz_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FROMDOC]),data.fromdoc.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_FROMDOC]),(gpointer)E_FROMDOC);


GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Разметить"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Разметить базу данных"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(admin_makebaz_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(admin_makebaz_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Не размечать базу данных"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(admin_makebaz_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

if(version.ravno_atof() < 4.1)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data.entry[E_KODIROVKA]),FALSE);//Недоступна
 }

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 {
  imabaz->new_plus(data.imabaz.ravno());
  kodirovka->new_plus(data.kodirovka.ravno());
  fromnsi->new_plus(data.fromnsi.ravno());
  fromdoc->new_plus(data.fromdoc.ravno());
 }

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void admin_makebaz_m_e_knopka(GtkWidget *widget,class admin_makebaz_m_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_FROMNSI:

    iceb_dir_select(&data->fromnsi,gettext("Выбор каталога"),data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_FROMNSI]),data->fromnsi.ravno());
      
    return;  


  case E_FROMDOC:

    iceb_dir_select(&data->fromdoc,gettext("Выбор каталога"),data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_FROMDOC]),data->fromdoc.ravno());
      
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   admin_makebaz_m_key_press(GtkWidget *widget,GdkEventKey *event,class admin_makebaz_m_data *data)
{
//char  bros[512];

//printf("vl_zarkateg_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(FALSE);

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
void  admin_makebaz_m_knopka(GtkWidget *widget,class admin_makebaz_m_data *data)
{
FILE *ff;
class iceb_u_str imafilprov("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("admin_makebaz_m_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();

    if(admin_provbaz(data->imabaz.ravno(),data->window) == 0)
     {
      iceb_menu_soob(gettext("База данных с таким именем уже есть!"),data->window);
      return;
     }

    /*Проверяем возможность открытия настроечного файла*/   
    imafilprov.new_plus(data->fromnsi.ravno());
    imafilprov.plus(G_DIR_SEPARATOR_S,"nastsys.alx");
    
    
    if((ff = fopen(imafilprov.ravno(),"r")) == NULL)
     {
      iceb_er_op_fil(imafilprov.ravno(),gettext("Неправильно указан путь откуда брать настроечные файлы!"),errno,data->window);
      return;
     }
    fclose(ff);

    /*Проверяем возможность открытия файла с документацией*/   
    imafilprov.new_plus(data->fromdoc.ravno());
    imafilprov.plus(G_DIR_SEPARATOR_S,"buhg.txt");
    
    
    if((ff = fopen(imafilprov.ravno(),"r")) == NULL)
     {
      iceb_er_op_fil(imafilprov.ravno(),gettext("Неправильно указан путь на файлы с документацией!"),errno,data->window);
      return;
     }
    fclose(ff);


    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_data();
    return;  


  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    admin_makebaz_m_vvod(GtkWidget *widget,class admin_makebaz_m_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (enter)
 {
  case E_KODIROVKA:
    data->kodirovka.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_KODIROVKA]))));
    break;
  case E_IMABAZ:
    data->imabaz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_IMABAZ]))));
    break;
 }

enter+=1;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
