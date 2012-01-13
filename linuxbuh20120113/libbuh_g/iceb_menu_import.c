/*$Id: iceb_menu_import.c,v 1.9 2011-02-21 07:36:07 sasa Exp $*/
/*16.11.2008	14.11.2004	Белых А.И.	iceb_menu_import.c
Меню для ввода имени файла из которого нужно импортировать документы 
*/
#include <stdlib.h>
#include <errno.h>
#include        <sys/stat.h>
#include "iceb_libbuh.h"

enum
{
  FK2,
  FK3,
  FK4,
  FK5,
  FK6,
  FK7,
  FK10,
  KOL_F_KL
};

enum
 {
  E_IMAF,
  KOLENTER  
 };

class impmatdok_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет

  class iceb_u_str *imafz;
  const char *imaf_nast;
  impmatdok_m_data() //Конструктор
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

gboolean   impmatdok_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class impmatdok_m_data *data);
void    impmatdok_m_v_vvod(GtkWidget *widget,class impmatdok_m_data *data);
void  impmatdok_m_v_knopka(GtkWidget *widget,class impmatdok_m_data *data);


extern char *name_system;
extern SQL_baza bd;

int iceb_menu_import(class iceb_u_str *imafz,const char *zagolov,const char *imaf_nast,GtkWidget *wpredok)
{
impmatdok_m_data data;

char strsql[312];
data.imafz=imafz;
data.imaf_nast=imaf_nast;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,iceb_u_toutf(zagolov));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(impmatdok_m_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=gtk_label_new(iceb_u_toutf(zagolov));

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

sprintf(strsql,"%s",gettext("Введите имя файла"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_IMAF] = gtk_entry_new_with_max_length (100);
gtk_box_pack_start (GTK_BOX (hbox[E_IMAF]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_IMAF]), data.entry[E_IMAF], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_IMAF]), "activate",GTK_SIGNAL_FUNC(impmatdok_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IMAF]),data.imafz->ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_IMAF]),(gpointer)E_IMAF);


GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Импорт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать импорт документов"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(impmatdok_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Просмотр"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Просмотр и редактиривание файла"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]),"clicked",GTK_SIGNAL_FUNC(impmatdok_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);

sprintf(strsql,"F6 %s",gettext("Очистить"));
data.knopka[FK6]=gtk_button_new_with_label(strsql);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]),"clicked",GTK_SIGNAL_FUNC(impmatdok_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK6], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Выполнить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Выполнить команды настроенные по клавише F5"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(impmatdok_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F5 %s",gettext("Настройка"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Настройка выполнения команд/программ для клавиши F4"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]),"clicked",GTK_SIGNAL_FUNC(impmatdok_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK5], TRUE, TRUE, 0);

if(data.imaf_nast[0] == '\0')
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[FK4]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[FK5]),FALSE);//Недоступна
 }

sprintf(strsql,"F7 %s",gettext("Найти"));
data.knopka[FK7]=gtk_button_new_with_label(strsql);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Найти нужный файл"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]),"clicked",GTK_SIGNAL_FUNC(impmatdok_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK7], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(impmatdok_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   impmatdok_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class impmatdok_m_data *data)
{

//printf("impmatdok_m_v_key_press\n");
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

  case GDK_F6:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
    return(TRUE);

  case GDK_F7:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK7]),"clicked");
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
void  impmatdok_m_v_knopka(GtkWidget *widget,class impmatdok_m_data *data)
{
struct stat work;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
/*g_print("impmatdok_m_v_knopka knop=%d\n",*knop);*/

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(data->imafz->getdlinna() <= 1)
     {
      iceb_u_str repl;
      iceb_menu_soob(gettext("Не введено имя файла !"),data->window);
      return;
     }    


    if(stat(data->imafz->ravno(),&work) != 0)
     {
      iceb_u_str repl; 
      repl.new_plus(gettext("Нет такого файла !"));
      repl.ps_plus(data->imafz->ravno());
      iceb_menu_soob(&repl,data->window);
      return;
     }


    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case FK3:
    data->read_rek(); //Читаем реквизиты меню
    if(data->imafz->getdlinna() > 1)
     iceb_f_redfil(data->imafz->ravno(),1,data->window);
    return;

  case FK4:
   if(data->imaf_nast[0] == '\0')
    return;
   iceb_fsystem(data->imaf_nast,data->window);
   break;

  case FK5:
/*   printf("imaf->nast=%s\n",data->imaf_nast);*/
   if(data->imaf_nast[0] == '\0')
    return;
  
   iceb_f_redfil(data->imaf_nast,0,data->window);
   break;
   
  case FK6:
    data->clear_rek();
    return;

  case FK7:
    iceb_file_selection(data->imafz,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_IMAF]),data->imafz->ravno());
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

void    impmatdok_m_v_vvod(GtkWidget *widget,class impmatdok_m_data *data)
{

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("impmatdok_m_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_IMAF:
    data->imafz->new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }
 
}
