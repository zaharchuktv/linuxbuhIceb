/*$Id: iceb_menu_vvod1.c,v 1.34 2011-02-21 07:36:07 sasa Exp $*/
/*20.05.2010	16.09.2003	Белых А.И.	iceb_menu_vvod1.c
Ввод строки на запрос
Если вернули 0- строка введена
             1-нет
*/
#include        <stdlib.h>
#include	"iceb_libbuh.h"

enum
 {
  FK2,
  FK10,
  KOL_FKL
 };
 

struct vihodvvod1
 {
  GtkWidget *window;
  GtkWidget *entry;
  int vihod;
  class iceb_u_str text;
  GtkWidget *knopka[KOL_FKL];
 };

gboolean  vvod1_dest(GtkWidget*,GdkEvent*,struct vihodvvod1*);
void  vvod1_vihod(GtkWidget*,struct vihodvvod1*);
void vvod1_get_text(GtkWidget *widget,struct vihodvvod1 *data);
gboolean   vvod1_key_press(GtkWidget *widget,GdkEventKey *event,struct vihodvvod1 *data);

extern char *name_system;

/*********************************************/
/*********************************************/
int iceb_menu_vvod1(const char *zapros,class iceb_u_str *stroka,int DLINNA,GtkWidget *wpredok)
{
iceb_u_spisok zz;
zz.plus(zapros);

return(iceb_menu_vvod1(&zz,stroka,DLINNA,wpredok));
}

/*********************************************/
/*********************************************/
int iceb_menu_vvod1(class iceb_u_str *zapros,class iceb_u_str *stroka,int DLINNA,GtkWidget *wpredok)
{
iceb_u_spisok zz;
zz.plus(zapros->ravno());

return(iceb_menu_vvod1(&zz,stroka,DLINNA,wpredok));
}

/***************************************/
/********Вторая функция*****************/
/***************************************/

int iceb_menu_vvod1(class iceb_u_spisok *zapros,class iceb_u_str *stroka,int DLINNA,GtkWidget *wpredok)
{
class iceb_u_str titl;
char bros[512];
vihodvvod1 data;
/*Избавляемся от перевода строки*/
if(iceb_u_polen(zapros->ravno(0),&titl,1,'\n') != 0)
 titl.new_plus(zapros->ravno(0));
 
int kolih=zapros->kolih();

data.vihod=1;
data.text.plus("");

data.window= gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_position( GTK_WINDOW( data.window ), GTK_WIN_POS_CENTER );                      
gtk_container_set_border_width( GTK_CONTAINER( data.window ), 10 );
gtk_object_set_user_data(GTK_OBJECT(data.window),(void*)"1");
gtk_window_set_modal( GTK_WINDOW(data.window ) ,TRUE ); 
gtk_window_set_resizable(GTK_WINDOW(data.window),FALSE); /*запрет на изменение размеров окна*/

class iceb_u_str repl(name_system);
repl.plus(" ",titl.ravno());

gtk_window_set_title (GTK_WINDOW (data.window),repl.ravno_toutf());


gtk_signal_connect(GTK_OBJECT (data.window), "delete_event",GTK_SIGNAL_FUNC(vvod1_dest), &data);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(vvod1_key_press),&data);
if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
sprintf(bros,"FK2 %s",gettext("Ввести"));
data.knopka[FK2] = gtk_button_new_with_label(bros);

sprintf(bros,"FK10 %s",gettext("Выход"));
data.knopka[FK10] = gtk_button_new_with_label(bros);

data.entry = gtk_entry_new_with_max_length (DLINNA-1);

if(stroka->getdlinna() > 1)
  gtk_entry_set_text (GTK_ENTRY (data.entry),stroka->ravno_toutf());

GtkWidget *vbox=gtk_vbox_new(FALSE,0);

GtkWidget *hbox=gtk_hbox_new(FALSE,0);

gtk_container_add(GTK_CONTAINER(data.window),vbox);

GtkWidget *label = NULL;
for(int i=0 ; i< kolih; i++)
 {

  label = gtk_label_new (zapros->ravno_toutf(i));
  gtk_box_pack_start(GTK_BOX (vbox),label,FALSE,FALSE,0);
 }
gtk_box_pack_start (GTK_BOX (vbox), data.entry, TRUE, TRUE, 0);

gtk_box_pack_start(GTK_BOX (vbox),hbox,FALSE,FALSE,0);

gtk_box_pack_start(GTK_BOX (hbox),data.knopka[FK2],TRUE,TRUE,2);
gtk_box_pack_start(GTK_BOX (hbox),data.knopka[FK10],TRUE,TRUE,2);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);

gtk_signal_connect(GTK_OBJECT (data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(vvod1_vihod), &data);
gtk_signal_connect(GTK_OBJECT (data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(vvod1_vihod), &data);
gtk_signal_connect(GTK_OBJECT(data.entry), "activate",GTK_SIGNAL_FUNC(vvod1_get_text),&data);

gtk_widget_show_all(data.window);
gtk_main();

if(data.vihod == 0)
 stroka->new_plus(data.text.ravno_fromutf());

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.vihod);

}



/************************/
/*Перегружаемая функция*/
/*************************/

int iceb_menu_vvod1(const char *zapros,char *stroka,int DLINNA,GtkWidget *wpredok)
{
class iceb_u_str titl;
char bros[512];
vihodvvod1 data;

/*Избавляемся от перевода строки*/
if(iceb_u_polen(zapros,&titl,1,'\n') != 0)
 titl.new_plus(zapros);

data.vihod=1;
data.text.plus("");

data.window= gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_position( GTK_WINDOW( data.window ), GTK_WIN_POS_CENTER );                      
gtk_container_set_border_width( GTK_CONTAINER( data.window ), 10 );
gtk_object_set_user_data(GTK_OBJECT(data.window),(void*)"1");
gtk_window_set_modal( GTK_WINDOW(data.window ) ,TRUE ); 
gtk_window_set_resizable(GTK_WINDOW(data.window),FALSE); /*запрет на изменение размеров окна*/


class iceb_u_str repl(name_system);
repl.plus(" ",titl.ravno());

gtk_window_set_title (GTK_WINDOW (data.window),repl.ravno_toutf());

gtk_signal_connect(GTK_OBJECT (data.window), "delete_event",GTK_SIGNAL_FUNC(vvod1_dest), &data);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(vvod1_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label = gtk_label_new (iceb_u_toutf(zapros));

sprintf(bros,"FK2 %s",gettext("Ввести"));
data.knopka[FK2] = gtk_button_new_with_label(bros);

sprintf(bros,"FK10 %s",gettext("Выход"));
data.knopka[FK10] = gtk_button_new_with_label(bros);

//GtkWidget *knvihod = gtk_button_new_with_label(gettext("Ввести"));
data.entry = gtk_entry_new_with_max_length (DLINNA-1);

if(stroka[0] != '\0')
 gtk_entry_set_text (GTK_ENTRY (data.entry),iceb_u_toutf(stroka));


GtkWidget *vbox=gtk_vbox_new(FALSE,0);
GtkWidget *hbox=gtk_hbox_new(FALSE,0);

gtk_container_add(GTK_CONTAINER(data.window),vbox);

gtk_box_pack_start(GTK_BOX (vbox),label,FALSE,FALSE,0);

gtk_box_pack_start (GTK_BOX (vbox), data.entry, TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX (vbox),hbox,FALSE,FALSE,0);

gtk_box_pack_start(GTK_BOX (hbox),data.knopka[FK2],TRUE,TRUE,2);
gtk_box_pack_start(GTK_BOX (hbox),data.knopka[FK10],TRUE,TRUE,2);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);

gtk_signal_connect(GTK_OBJECT (data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(vvod1_vihod), &data);
gtk_signal_connect(GTK_OBJECT (data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(vvod1_vihod), &data);
gtk_signal_connect(GTK_OBJECT(data.entry), "activate",GTK_SIGNAL_FUNC(vvod1_get_text),&data);

//gtk_box_pack_start(GTK_BOX (vbox),knvihod,FALSE,FALSE,0);

//gtk_signal_connect(GTK_OBJECT (knvihod), "clicked",GTK_SIGNAL_FUNC(vvod1_vihod), &data);
//gtk_signal_connect(GTK_OBJECT(data.entry), "activate",GTK_SIGNAL_FUNC(vvod1_get_text),&data);

gtk_widget_show_all(data.window);
gtk_main();

if(data.vihod == 0)
 strncpy(stroka,data.text.ravno_fromutf(),DLINNA-1);
 
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.vihod);

}
/**************************************/

int iceb_menu_vvod1(iceb_u_str *zapros,char *stroka,int DLINNA,GtkWidget *wpredok)
{

return(iceb_menu_vvod1(zapros->ravno(),stroka,DLINNA,wpredok));

}

/***************************/
/*Выход по нажатию крестика*/
/***************************/
gboolean  vvod1_dest(GtkWidget *widget,GdkEvent *event,struct vihodvvod1 *data)
{
data->vihod=1;
/*printf("vvod1_dest=%d Удаляем виджет\n",data->vihod);*/
data->text.new_plus("");
gtk_widget_destroy(data->window);
return(FALSE);
}

/*************************/
/*Выход по клавише*/
/*******************/

void  vvod1_vihod(GtkWidget *widget,struct vihodvvod1 *data)
{


int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
/*g_print("vvod1_vihod knop=%d\n",*knop);*/

switch (knop)
 {
  case FK2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry),"activate");
    break;
  
  case FK10:
    data->vihod=1;
    data->text.new_plus("");
    gtk_widget_destroy(data->window);
    break;
 }


//printf("vvod1_vihod=%d\n",data->vihod);

//gtk_widget_destroy(data->window); //удаляется после чтения

//printf("vvod1_vihod-end\n");
}
/*****************************/
/*Чтение введенной информации*/
/*****************************/
void vvod1_get_text(GtkWidget *widget,struct vihodvvod1 *data)
{
/*printf("vvod1_get_text\n");*/
data->text.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(widget)));
data->vihod=0;
/*printf("vvod1_get_text  Удаляем виджет %s\n",data->text.ravno());*/
gtk_widget_destroy(data->window);

}
/***************************/
/*Обработчик нажатия клавиш*/
/***************************/
gboolean   vvod1_key_press(GtkWidget *widget,GdkEventKey *event,struct vihodvvod1 *data)
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

