/*$Id: iceb_menu_vibkat.c,v 1.1 2011-08-29 07:13:46 sasa Exp $*/
/*27.07.2011	27.07.2011	Белых А.И.	iceb_menu_vibkat.c
Меню ввода каталога
Если вернули 0 - каталог выбрали
             1 - нет
*/

#include        <stdlib.h>
#include	"iceb_libbuh.h"

enum
 {
  E_PUT,
  KOLENTER  
 };

enum
 {
  FK2,
  FK10,
  KOL_FKL
 };
 

class iceb_menu_vibkat_data
 {
  public:
  GtkWidget *window;
  GtkWidget *entry[KOLENTER];
  int vihod;
  class iceb_u_str text;
  GtkWidget *knopka[KOL_FKL];
  GtkWidget *knopka_enter[KOLENTER];
  
  iceb_menu_vibkat_data()
   {
    vihod=0;
    text.plus("");
   }
 };
extern char *name_system;

gboolean   iceb_menu_vibkat_key_press(GtkWidget *widget,GdkEventKey *event,iceb_menu_vibkat_data *data);
void iceb_menu_vibkat_get_text(GtkWidget *widget,class iceb_menu_vibkat_data *data);
void  iceb_menu_vibkat_vihod(GtkWidget *widget,class iceb_menu_vibkat_data *data);
gboolean  iceb_menu_vibkat_dest(GtkWidget *widget,GdkEvent *event,class iceb_menu_vibkat_data *data);
void  iceb_menu_vibkat_v_e_knopka(GtkWidget *widget,class iceb_menu_vibkat_data *data);

int iceb_menu_vibkat(const char *zapros,class iceb_u_str *stroka,GtkWidget *wpredok)
{
 class iceb_u_spisok prom;
 prom.plus(zapros);
 return(iceb_menu_vibkat(&prom,stroka,wpredok));

}

int iceb_menu_vibkat(class iceb_u_spisok *zapros,class iceb_u_str *stroka,GtkWidget *wpredok)
{
class iceb_u_str titl;
char bros[512];
class iceb_menu_vibkat_data data;
/*Избавляемся от перевода строки*/
if(iceb_u_polen(zapros->ravno(0),&titl,1,'\n') != 0)
 titl.new_plus(zapros->ravno(0));
 
int kolih=zapros->kolih();


data.window= gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_position( GTK_WINDOW( data.window ), GTK_WIN_POS_CENTER );                      
gtk_container_set_border_width( GTK_CONTAINER( data.window ), 10 );
gtk_object_set_user_data(GTK_OBJECT(data.window),(void*)"1");
gtk_window_set_modal( GTK_WINDOW(data.window ) ,TRUE ); 
gtk_window_set_resizable(GTK_WINDOW(data.window),FALSE); /*запрет на изменение размеров окна*/

class iceb_u_str repl(name_system);
repl.plus(" ",titl.ravno());

gtk_window_set_title (GTK_WINDOW (data.window),repl.ravno_toutf());


gtk_signal_connect(GTK_OBJECT (data.window), "delete_event",GTK_SIGNAL_FUNC(iceb_menu_vibkat_dest), &data);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_menu_vibkat_key_press),&data);
if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


//data.entry = gtk_entry_new();

//if(stroka->getdlinna() > 1)
//  gtk_entry_set_text (GTK_ENTRY (data.entry),stroka->ravno());

GtkWidget *vbox=gtk_vbox_new(FALSE,0);

GtkWidget *hbox0=gtk_hbox_new(FALSE,0);
GtkWidget *hbox=gtk_hbox_new(FALSE,0);

gtk_container_add(GTK_CONTAINER(data.window),vbox);

GtkWidget *label = NULL;
for(int i=0 ; i< kolih; i++)
 {

  label = gtk_label_new (zapros->ravno_toutf(i));
  gtk_box_pack_start(GTK_BOX (vbox),label,FALSE,FALSE,0);
 }
gtk_box_pack_start(GTK_BOX (vbox),hbox0,FALSE,FALSE,0);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(bros,"%s",gettext("Каталог"));
data.knopka_enter[E_PUT]=gtk_button_new_with_label(iceb_u_toutf(bros));
gtk_box_pack_start (GTK_BOX (hbox0), data.knopka_enter[E_PUT], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PUT]),"clicked",GTK_SIGNAL_FUNC(iceb_menu_vibkat_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PUT]),(gpointer)E_PUT);
tooltips_enter[E_PUT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_PUT],data.knopka_enter[E_PUT],gettext("Поиск нужного каталога"),NULL);

data.entry[E_PUT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox0), data.entry[E_PUT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PUT]), "activate",GTK_SIGNAL_FUNC(iceb_menu_vibkat_get_text),&data);
if(stroka->getdlinna() > 1)
  gtk_entry_set_text(GTK_ENTRY(data.entry[E_PUT]),stroka->ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PUT]),(gpointer)E_PUT);





//gtk_box_pack_start (GTK_BOX (vbox), data.entry, TRUE, TRUE, 0);
//gtk_signal_connect(GTK_OBJECT(data.entry), "activate",GTK_SIGNAL_FUNC(iceb_menu_vibkat_get_text),&data);

gtk_box_pack_start(GTK_BOX (vbox),hbox,FALSE,FALSE,0);

sprintf(bros,"FK2 %s",gettext("Ввести"));
data.knopka[FK2] = gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX (hbox),data.knopka[FK2],TRUE,TRUE,2);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_signal_connect(GTK_OBJECT (data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(iceb_menu_vibkat_vihod), &data);

sprintf(bros,"FK10 %s",gettext("Выход"));
data.knopka[FK10] = gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX (hbox),data.knopka[FK10],TRUE,TRUE,2);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_signal_connect(GTK_OBJECT (data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(iceb_menu_vibkat_vihod), &data);

gtk_widget_show_all(data.window);
gtk_main();

if(data.vihod == 0)
 stroka->new_plus(data.text.ravno_fromutf());

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.vihod);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  iceb_menu_vibkat_v_e_knopka(GtkWidget *widget,class iceb_menu_vibkat_data *data)
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



/***************************/
/*Выход по нажатию крестика*/
/***************************/
gboolean  iceb_menu_vibkat_dest(GtkWidget *widget,GdkEvent *event,class iceb_menu_vibkat_data *data)
{
data->vihod=1;
/*printf("iceb_menu_vibkat_dest=%d Удаляем виджет\n",data->vihod);*/
data->text.new_plus("");
gtk_widget_destroy(data->window);
return(FALSE);
}

/*************************/
/*Выход по клавише*/
/*******************/

void  iceb_menu_vibkat_vihod(GtkWidget *widget,class iceb_menu_vibkat_data *data)
{


int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
/*g_print("iceb_menu_vibkat_vihod knop=%d\n",*knop);*/

switch (knop)
 {
  case FK2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_PUT]),"activate");
    break;
  
  case FK10:
    data->vihod=1;
    data->text.new_plus("");
    gtk_widget_destroy(data->window);
    break;
 }

}
/*****************************/
/*Чтение введенной информации*/
/*****************************/
void iceb_menu_vibkat_get_text(GtkWidget *widget,class iceb_menu_vibkat_data *data)
{
data->text.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(widget)));
data->vihod=0;
gtk_widget_destroy(data->window);

}
/***************************/
/*Обработчик нажатия клавиш*/
/***************************/
gboolean   iceb_menu_vibkat_key_press(GtkWidget *widget,GdkEventKey *event,iceb_menu_vibkat_data *data)
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

