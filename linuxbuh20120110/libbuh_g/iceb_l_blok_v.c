/*$Id: iceb_l_blok_v.c,v 1.5 2011-02-21 07:36:07 sasa Exp $*/
/*09.11.2010	06.09.2010	Белых А.И.	iceb_l_blok_v.c
Ввод и корректировка единиц измерения
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "iceb_libbuh.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATA,
  E_LOGIN,
  E_SHET,
  KOLENTER  
 };

class iceb_l_blok_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str datablok;
  class iceb_u_str login;
  class iceb_u_str shet;

  short godk,mesk;
    
  iceb_l_blok_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    datablok.plus("");
    login.plus("");
    shet.plus("");  
   }

  void read_rek()
   {
    datablok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA]))));
    login.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_LOGIN]))));
    shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET]))));
   }
 };


gboolean   iceb_l_blok_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_blok_v_data *data);
void  iceb_l_blok_v_knopka(GtkWidget *widget,class iceb_l_blok_v_data *data);
void    iceb_l_blok_v_vvod(GtkWidget *widget,class iceb_l_blok_v_data *data);
int iceb_l_blok_zap(class iceb_l_blok_v_data *data);

extern int iceb_kod_podsystem;
extern char *name_system;

int iceb_l_blok_v(short *mesk,short *godk,GtkWidget *wpredok)
{

class iceb_l_blok_v_data data;
char strsql[512];
class iceb_u_str kikz;

data.mesk=*mesk;
data.godk=*godk;

if(data.godk != 0 )
 {
  sprintf(strsql,"select * from Blok where kod=%d and god=%d and mes=%d",
  iceb_kod_podsystem,data.godk,data.mesk);
//  printf("%s-%s\n",__FUNCTION__,strsql);
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  data.datablok.new_plus(data.mesk);
  data.datablok.plus(".");
  data.datablok.plus(data.godk);
  data.login.new_plus(row[3]);
  data.shet.new_plus(row[4]);
  kikz.plus(iceb_kikz(row[5],row[6],wpredok));
  
 }
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.godk == 0)
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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_l_blok_v_key_press),&data);

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
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox), hboxknop, FALSE, FALSE, 0);


sprintf(strsql,"%s (%s)",gettext("Дата"),gettext("м.г"));
label=gtk_label_new(strsql);
data.entry[E_DATA] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.entry[E_DATA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA]), "activate",GTK_SIGNAL_FUNC(iceb_l_blok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA]),data.datablok.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA]),(gpointer)E_DATA);

sprintf(strsql,"%s (,,)",gettext("Логины операторов для котoрых разблокировано"));
label=gtk_label_new(strsql);
data.entry[E_LOGIN] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_LOGIN]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_LOGIN]), data.entry[E_LOGIN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_LOGIN]), "activate",GTK_SIGNAL_FUNC(iceb_l_blok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_LOGIN]),data.login.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_LOGIN]),(gpointer)E_LOGIN);

sprintf(strsql,"%s (,,)",gettext("Заблокированные счета"));
label=gtk_label_new(strsql);
data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(iceb_l_blok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

//GtkWidget *knopka[KOL_PFK];
GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(iceb_l_blok_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(iceb_l_blok_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 {
  iceb_u_polen(data.datablok.ravno(),mesk,1,'.');
  iceb_u_polen(data.datablok.ravno(),godk,2,'.');
  
 }
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_l_blok_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_blok_v_data *data)
{
//char  bros[512];

//printf("ei_v_key_press\n");
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
void  iceb_l_blok_v_knopka(GtkWidget *widget,class iceb_l_blok_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("ei_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    if(iceb_l_blok_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
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

void    iceb_l_blok_v_vvod(GtkWidget *widget,class iceb_l_blok_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("ei_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATA:
    data->datablok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_LOGIN:
    data->login.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter++;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int iceb_l_blok_zap(class iceb_l_blok_v_data *data)
{
char strsql[1024];
short mesi=0,godi=0;

data->read_rek();

if(iceb_l_blok_prov(data->window) != 0)
  return(1);
if(data->datablok.ravno()[0] != '*')
 {
 
  if(iceb_u_rsdat1(&mesi,&godi,data->datablok.ravno()) != 0)
   {
    iceb_menu_soob(gettext("Неправильно введена дата!"),data->window);
    return(1);
   }     
 }
else  
  iceb_u_rsdat1(&mesi,&godi,data->datablok.ravno());
 
if(godi == 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата!"),data->window);
  return(1);
 }     


 /*проверяем может такая запись уже есть*/
if(data->godk == 0 || (mesi != data->mesk || godi != data->godk))
 {
  sprintf(strsql,"select kod from Blok where kod=%d and god=%d and mes=%d",
  iceb_kod_podsystem,mesi,godi);
  if(iceb_sql_readkey(strsql,data->window) == 1)
   {
    iceb_menu_soob(gettext("Такая запись уже есть!"),data->window);
    return(1);
   }
 }      

short metka_bg=0;
if(data->datablok.ravno()[0] == '*')
  {
  if(iceb_menu_danet(gettext("Заблокировать весь год? Вы уверены?"),2,data->window) == 2)
    return(1);
   metka_bg=1;
  }

 if(data->godk != 0) /*корректировка записи*/
  {
   

   sprintf(strsql,"update Blok set god=%d,mes=%d,log='%s',shet='%s',ktoz=%d,vrem=%ld\
  where kod=%d and god=%d and mes=%d",
   godi,mesi,data->login.ravno(),data->shet.ravno(),iceb_getuid(data->window),time(NULL),
   iceb_kod_podsystem,data->godk,data->mesk);
          
  
  }
 else  /*новая запись*/
  {
   sprintf(strsql,"insert into Blok values(%d,%d,%d,'%s','%s',%d,%ld)",
   iceb_kod_podsystem,godi,mesi,data->login.ravno(),data->shet.ravno(),iceb_getuid(data->window),time(NULL));
  }

 if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
  return(1);

 if(metka_bg == 1)
  {
   sprintf(strsql,"delete from Blok where kod=%d and god=%d and mes != 0",iceb_kod_podsystem,godi);
   iceb_sql_zapis(strsql,1,0,data->window);
  }


return(0);

}
