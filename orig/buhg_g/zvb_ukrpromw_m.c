/*$Id: zvb_ukrpromw_m.c,v 1.12 2011-02-21 07:36:00 sasa Exp $*/
/*22.06.2006	05.05.2004	Белых А.И.	zvb_ukrpromw_m.c
Меню для ввода реквизитов поиска 
*/
#include <unistd.h>
#include "buhg_g.h"
#include "zvb_ukrpromw.h"
enum
 {
  E_NOMER_FIL,
  E_KOD_OR,
  E_IMA_MAH,
  E_IMA_SET_DISK,
  E_PAROL,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK5,
  FK10,
  KOL_FK
 };

class zvb_ukrprom_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class zvb_ukrpromw_rek *rk;
  
  
  zvb_ukrprom_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
     }
   }

  void clear_data()
   {
    rk->clear_rek();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
 };

void zvb_ukrprom_m_clear(class zvb_ukrprom_m_data *data);
void    zvb_ukrprom_m_vvod(GtkWidget *widget,class zvb_ukrprom_m_data *data);
void  zvb_ukrprom_m_knopka(GtkWidget *widget,class zvb_ukrprom_m_data *data);
gboolean   zvb_ukrprom_m_key_press(GtkWidget *widget,GdkEventKey *event,class zvb_ukrprom_m_data *data);

void zvb_ukrpromw_read_rek(class zvb_ukrprom_m_data *data);

extern char      *name_system;
extern SQL_baza bd;

int zvb_ukrpromw_m(class zvb_ukrpromw_rek *rek_poi,GtkWidget *wpredok)
{
class zvb_ukrprom_m_data data;
char strsql[512];
data.rk=rek_poi;


zvb_ukrpromw_read_rek(&data);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Укрпромбанк"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zvb_ukrprom_m_key_press),&data);

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
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


GtkWidget *label=gtk_label_new(gettext("Номер файла за текущий день"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_FIL]), label, FALSE, FALSE, 0);

data.entry[E_NOMER_FIL] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_FIL]), data.entry[E_NOMER_FIL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMER_FIL]), "activate",GTK_SIGNAL_FUNC(zvb_ukrprom_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMER_FIL]),data.rk->nomer_fil.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMER_FIL]),(gpointer)E_NOMER_FIL);


label=gtk_label_new(gettext("Код организации"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OR]), label, FALSE, FALSE, 0);

data.entry[E_KOD_OR] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OR]), data.entry[E_KOD_OR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_OR]), "activate",GTK_SIGNAL_FUNC(zvb_ukrprom_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_OR]),data.rk->kod_or.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_OR]),(gpointer)E_KOD_OR);


label=gtk_label_new(gettext("Имя машины"));
gtk_box_pack_start (GTK_BOX (hbox[E_IMA_MAH]), label, FALSE, FALSE, 0);

data.entry[E_IMA_MAH] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_IMA_MAH]), data.entry[E_IMA_MAH], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_IMA_MAH]), "activate",GTK_SIGNAL_FUNC(zvb_ukrprom_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IMA_MAH]),data.rk->ima_mah.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_IMA_MAH]),(gpointer)E_IMA_MAH);

label=gtk_label_new(gettext("Имя сетевого диска"));
gtk_box_pack_start (GTK_BOX (hbox[E_IMA_SET_DISK]), label, FALSE, FALSE, 0);

data.entry[E_IMA_SET_DISK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_IMA_SET_DISK]), data.entry[E_IMA_SET_DISK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_IMA_SET_DISK]), "activate",GTK_SIGNAL_FUNC(zvb_ukrprom_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IMA_SET_DISK]),data.rk->ima_set_disk.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_IMA_SET_DISK]),(gpointer)E_IMA_SET_DISK);

label=gtk_label_new(gettext("Пароль"));
gtk_box_pack_start (GTK_BOX (hbox[E_PAROL]), label, FALSE, FALSE, 0);

data.entry[E_PAROL] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_PAROL]), data.entry[E_PAROL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PAROL]), "activate",GTK_SIGNAL_FUNC(zvb_ukrprom_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PAROL]),data.rk->parol.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PAROL]),(gpointer)E_PAROL);


GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчёт"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(zvb_ukrprom_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(zvb_ukrprom_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F5 %s",gettext("Настройка"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Настройка расчёта"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]),"clicked",GTK_SIGNAL_FUNC(zvb_ukrprom_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK5], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(zvb_ukrprom_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

return(data.voz);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zvb_ukrprom_m_key_press(GtkWidget *widget,GdkEventKey *event,class zvb_ukrprom_m_data *data)
{
//char  bros[512];

//printf("vl_gruppod_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(FALSE);

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
void  zvb_ukrprom_m_knopka(GtkWidget *widget,class zvb_ukrprom_m_data *data)
{
//char bros[512];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("zvb_ukrprom_m_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_data();
    return;  

  case FK5:
    if(iceb_f_redfil("zvb_ukrprom.alx",0,data->window) == 0)
       zvb_ukrpromw_read_rek(data);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMER_FIL]),data->rk->nomer_fil.ravno_toutf());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_OR]),data->rk->kod_or.ravno_toutf());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_IMA_MAH]),data->rk->ima_mah.ravno_toutf());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_IMA_SET_DISK]),data->rk->ima_set_disk.ravno_toutf());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PAROL]),data->rk->parol.ravno_toutf());
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

void    zvb_ukrprom_m_vvod(GtkWidget *widget,class zvb_ukrprom_m_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("zvb_ukrprom_m_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_NOMER_FIL:
    data->rk->nomer_fil.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KOD_OR:
    data->rk->kod_or.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_IMA_MAH:
    data->rk->ima_mah.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_IMA_SET_DISK:
    data->rk->ima_set_disk.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_PAROL:
    data->rk->parol.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
 }
enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/**************************/
/*Чтение настроек*/
/***********************/
void zvb_ukrpromw_read_rek(class zvb_ukrprom_m_data *data)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"zvb_ukrprom.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,data->window);
  return;
 }

char naim_str[1024];
char bros[1024];

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;


  iceb_u_polen(row_alx[0],naim_str,sizeof(naim_str),1,'|');
  if(iceb_u_SRAV(naim_str,"Имя машины",0) == 0)
   {
    iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
    data->rk->ima_mah.new_plus(bros);
    continue;   
   }
  if(iceb_u_SRAV(naim_str,"Имя сетевого диска",0) == 0)
   {
    iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
    data->rk->ima_set_disk.new_plus(bros);
    continue;   
   }
  if(iceb_u_SRAV(naim_str,"Пароль",0) == 0)
   {
    iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
    data->rk->parol.new_plus(bros);
    continue;   
   }
  if(iceb_u_SRAV(naim_str,"Код организации",0) == 0)
   {
    iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
    data->rk->kod_or.new_plus(bros);
    continue;   
   }
 }



}
