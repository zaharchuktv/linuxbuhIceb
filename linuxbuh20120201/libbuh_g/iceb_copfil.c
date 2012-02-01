/*$Id: iceb_copfil.c,v 1.2 2011-09-05 08:18:29 sasa Exp $*/
/*18.08.2011	18.08.2011	Белых А.И.	iceb_copfil.c
Меню ввода каталога
Если вернули 0 - каталог выбрали
             1 - нет
*/

#include        <stdlib.h>
#include	"iceb_libbuh.h"
#include <unistd.h>
enum
 {
  E_PUT,
  E_IMAFIL_NEW,
  E_KODIR,
  KOLENTER  
 };

enum
 {
  FK2,
  FK10,
  KOL_FKL
 };
 

class iceb_copfil_data
 {
  public:
  GtkWidget *window;
  GtkWidget *entry[KOLENTER];
  GtkWidget *opt;
  int vihod;
  GtkWidget *knopka[KOL_FKL];
  GtkWidget *knopka_enter[KOLENTER];
  class iceb_u_str imafil;
  class iceb_u_str imafil_new;
  static class iceb_u_str katalog;
  int metka_kodir;    
  iceb_copfil_data()
   {
    vihod=0;
    katalog.plus("");
    metka_kodir=0;
   }
  void read_rek()
   {
    imafil_new.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_IMAFIL_NEW]))));
    katalog.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PUT]))));
   }
 };
class iceb_u_str  iceb_copfil_data::katalog;

extern char *name_system;

gboolean   iceb_copfil_key_press(GtkWidget *widget,GdkEventKey *event,iceb_copfil_data *data);
void  iceb_copfil_vihod(GtkWidget *widget,class iceb_copfil_data *data);
void  iceb_copfil_v_e_knopka(GtkWidget *widget,class iceb_copfil_data *data);
void iceb_copfil_v_vvod(GtkWidget *widget,class iceb_copfil_data *data);


void iceb_copfil(const char *imafil,GtkWidget *wpredok)
{
class iceb_u_str titl;
char bros[1024];
class iceb_copfil_data data;
 
data.imafil.plus(imafil);
data.imafil_new.plus(imafil);

data.window= gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_position( GTK_WINDOW( data.window ), GTK_WIN_POS_CENTER );                      
gtk_container_set_border_width( GTK_CONTAINER( data.window ), 10 );
gtk_object_set_user_data(GTK_OBJECT(data.window),(void*)"1");
gtk_window_set_modal( GTK_WINDOW(data.window ) ,TRUE ); 
gtk_window_set_resizable(GTK_WINDOW(data.window),FALSE); /*запрет на изменение размеров окна*/

class iceb_u_str repl(name_system);
repl.plus(" ",gettext("Копирование файла"));

gtk_window_set_title (GTK_WINDOW (data.window),repl.ravno_toutf());


gtk_signal_connect(GTK_OBJECT (data.window), "delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy), &data);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_copfil_key_press),&data);
if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }



GtkWidget *vbox=gtk_vbox_new(FALSE,0);

GtkWidget *hbox0[KOLENTER];

for(int nom=0; nom < KOLENTER; nom++)
 hbox0[nom]=gtk_hbox_new(FALSE,0);

GtkWidget *hbox=gtk_hbox_new(FALSE,0);

gtk_container_add(GTK_CONTAINER(data.window),vbox);

GtkWidget *label = NULL;
sprintf(bros,"%s:%s",gettext("Копирование файла"),imafil);
label = gtk_label_new (bros);
gtk_box_pack_start(GTK_BOX (vbox),label,FALSE,FALSE,0);

for(int nom=0; nom < KOLENTER; nom++)
  gtk_box_pack_start(GTK_BOX (vbox),hbox0[nom],FALSE,FALSE,0);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(bros,"%s",gettext("Каталог"));
data.knopka_enter[E_PUT]=gtk_button_new_with_label(iceb_u_toutf(bros));
gtk_box_pack_start (GTK_BOX (hbox0[E_PUT]), data.knopka_enter[E_PUT], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PUT]),"clicked",GTK_SIGNAL_FUNC(iceb_copfil_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PUT]),(gpointer)E_PUT);
tooltips_enter[E_PUT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_PUT],data.knopka_enter[E_PUT],gettext("Поиск нужного каталога"),NULL);

data.entry[E_PUT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox0[E_PUT]), data.entry[E_PUT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PUT]), "activate",GTK_SIGNAL_FUNC(iceb_copfil_v_vvod),&data);
if(data.katalog.getdlinna() > 1)
  gtk_entry_set_text(GTK_ENTRY(data.entry[E_PUT]),data.katalog.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PUT]),(gpointer)E_PUT);


label=gtk_label_new(gettext("Имя копии файла"));
gtk_box_pack_start (GTK_BOX (hbox0[E_IMAFIL_NEW]), label, FALSE, FALSE, 0);

data.entry[E_IMAFIL_NEW] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox0[E_IMAFIL_NEW]), data.entry[E_IMAFIL_NEW], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_IMAFIL_NEW]), "activate",GTK_SIGNAL_FUNC(iceb_copfil_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IMAFIL_NEW]),data.imafil_new.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_IMAFIL_NEW]),(gpointer)E_IMAFIL_NEW);



label=gtk_label_new(gettext("Кодировка"));
gtk_box_pack_start (GTK_BOX (hbox0[E_KODIR]), label, FALSE, FALSE, 0);


class iceb_u_spisok menu_vib;
menu_vib.plus(gettext("Копировать без перекодировки"));
menu_vib.plus(gettext("Копировать с перекодировкой в WINDOWS-1251"));

iceb_pm_vibor(&menu_vib,&data.opt,&data.metka_kodir);
gtk_box_pack_start (GTK_BOX (hbox0[E_KODIR]), data.opt, FALSE, FALSE, 0);




gtk_box_pack_start(GTK_BOX (vbox),hbox,FALSE,FALSE,0);

sprintf(bros,"FK2 %s",gettext("Ввести"));
data.knopka[FK2] = gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX (hbox),data.knopka[FK2],TRUE,TRUE,2);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_signal_connect(GTK_OBJECT (data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(iceb_copfil_vihod), &data);

sprintf(bros,"FK10 %s",gettext("Выход"));
data.knopka[FK10] = gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX (hbox),data.knopka[FK10],TRUE,TRUE,2);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_signal_connect(GTK_OBJECT (data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(iceb_copfil_vihod), &data);

gtk_widget_show_all(data.window);
gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return;

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  iceb_copfil_v_e_knopka(GtkWidget *widget,class iceb_copfil_data *data)
{
class iceb_u_str kod("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_PUT:
     if(iceb_dir_select(&kod,gettext("Поиск нужного каталога"),data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_PUT]),kod.ravno_toutf());
      
    return;  
 }
}
/****************/
/*Выполнение*/
/*****************/
int iceb_copfil_run(class iceb_copfil_data *data)
{
data->read_rek();

if(data->katalog.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели каталог для копирования!"),data->window);
  return(1);
 }

if(data->imafil_new.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели имя файла!"),data->window);
  return(1);
 }

class iceb_u_str fulput("");
char imaf_tmp[512];
strcpy(imaf_tmp,data->imafil.ravno());

if(data->metka_kodir == 1)
 {

  sprintf(imaf_tmp,"kkk%d.tmp",getpid());
  /*Делаем копию файла, которую перекодируем*/
  iceb_cp(data->imafil.ravno(),imaf_tmp,0,data->window);

  iceb_perecod(2,imaf_tmp,data->window);
 }

fulput.new_plus(data->katalog.ravno());

fulput.plus(G_DIR_SEPARATOR_S,data->imafil_new.ravno());
printf("%s-%s -> %s\n",__FUNCTION__,imaf_tmp,fulput.ravno());

int voz=iceb_cp(imaf_tmp,fulput.ravno(),0,data->window);

if(data->metka_kodir == 1)
  unlink(imaf_tmp);

return(voz);

}

/*************************/
/*Выход по клавише*/
/*******************/

void  iceb_copfil_vihod(GtkWidget *widget,class iceb_copfil_data *data)
{


int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
/*g_print("iceb_copfil_vihod knop=%d\n",*knop);*/

switch (knop)
 {
  case FK2:
   if(iceb_copfil_run(data) != 0)
    return;

   data->vihod=0;
   gtk_widget_destroy(data->window);

   break;
  
  case FK10:
    data->vihod=1;
    gtk_widget_destroy(data->window);
    break;
 }

}
/***************************/
/*Обработчик нажатия клавиш*/
/***************************/
gboolean   iceb_copfil_key_press(GtkWidget *widget,GdkEventKey *event,iceb_copfil_data *data)
{
switch(event->keyval)
 {
  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(FALSE);

  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);
 }
 
return(TRUE);
}

/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void iceb_copfil_v_vvod(GtkWidget *widget,class iceb_copfil_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("iceb_copfil_v_vvod enter=%d\n",enter);

switch (enter)
 {
/**********
  case E_KOD:
    data->rk.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NAIM:
    data->rk.naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
***************/
 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
