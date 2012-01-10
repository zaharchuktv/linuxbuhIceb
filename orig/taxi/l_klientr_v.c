/*$Id: l_klientr_v.c,v 1.36 2011-02-21 07:36:20 sasa Exp $*/
/*26.08.2007	01.03.2004	Белых А.И.	l_klientr_v.c
Ввод и коррктировка записей клиентов
*/
#include <stdlib.h>
#include <unistd.h>
#include "i_rest.h"
#include "klient.h"

enum
{
 FK2,
// FK4,
 FK10,
 KOL_F_KL
};

enum
{
 E_KOD,
 E_FIO,
 E_ADRES,
 E_TELEFON,
 E_SP_PODR,
 E_DEN_ROG,
 E_KOMENT,
 KOLENTER
};

class klientr_v_data 
 {
  public:
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *entry[KOLENTER];
  GtkWidget *opt;
  GtkWidget *opt_mk;
  GtkWidget *opt_pol;
  GtkWidget *opt_blok;
  short     kl_shift; //0-отжата 1-нажата  

  int       voz;
       
  class klient_rek_data vvod;
  iceb_u_str kodkl;
  iceb_u_spisok menu_kodgr;
  int           vib_kodgr;
  guint     timer;
      
  klientr_v_data()
   {
    vib_kodgr=0;
    voz=0;
    kl_shift=0;
    timer=0;
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


gboolean   klientr_v_key_press(GtkWidget *widget,GdkEventKey *event,class klientr_v_data *data);
void  klientr_v_knopka(GtkWidget *widget,class klientr_v_data *data);
void    klientr_v_vvod(GtkWidget *widget,class klientr_v_data *data);
int klientr_pk(const char *,GtkWidget*);
int klientr_zap(class klientr_v_data *data);
void klientr_kodgr(class klientr_v_data *data);
void  l_klientr_v_v_e_knopka(GtkWidget *widget,class klientr_v_data *data);
void klientr_kontrvv(class klientr_v_data *data);
void klientr_pol(class klientr_v_data *data);
void klientr_blok(class klientr_v_data *data);

extern SQL_baza	bd;
extern char *name_system;


int l_klientr_v(class iceb_u_str *kodkl,int metka_card, //0-не считывать карточеки 1-считывать
GtkWidget *wpredok)
{
char strsql[512];
class klientr_v_data data;
iceb_u_str kikz;

printf("l_klientr_v\n");
    
data.kl_shift=0;
data.vvod.clear_data();
data.kodkl.new_plus(kodkl->ravno());

if(kodkl->getdlinna() > 1)
 {
  SQL_str row;
  SQLCURSOR cur;
  char strsql[300];
  sprintf(strsql,"select * from Taxiklient where kod='%s'",data.kodkl.ravno_filtr());
  
  if(sql_readkey(&bd,strsql,&row,&cur) < 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    iceb_menu_soob(&repl,data.window);
    return(1);
   }

  data.vvod.kod.new_plus(row[0]);
  data.vvod.fio.new_plus(row[1]);
  data.vvod.adres.new_plus(row[2]);
  data.vvod.telef.new_plus(row[3]);

  kikz.plus(iceb_kikz(row[4],row[5],wpredok));   
  
  data.vvod.pr_sk.new_plus(row[6]);
  data.vvod.kodgr.new_plus(row[7]);  
  data.vvod.sp_podr.new_plus(row[8]);  
  data.vvod.metka_kvv=atoi(row[9]);
  data.vvod.pol=atoi(row[11]);
  if(row[12][0] != '0')
   data.vvod.denrog.new_plus(iceb_u_sqldata(row[12]));
  data.vvod.koment.new_plus(row[13]);
  data.vvod.metka_bl=atoi(row[14]);
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);


gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
//gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_widget_destroyed),&data.window);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(klientr_v_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label;
if(kodkl->getdlinna() <= 1)
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

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER+4];
for(int i=0 ; i < KOLENTER+4; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0 ; i < KOLENTER+4; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

label=gtk_label_new(gettext("Код"));
data.entry[E_KOD] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(klientr_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.vvod.kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

#ifdef READ_CARD
extern tdcon           td_server; 
if(td_server >= 0)
if(metka_card == 1)
 {
  class iceb_read_card_enter_data card;
  card.window=data.window;
  card.entry=data.entry[E_KOD];
  data.timer=card.timer=gtk_timeout_add(500,(GtkFunction)iceb_read_card_enter,&card);
 }
#endif

GtkTooltips *tooltips_enter[KOLENTER];

label=gtk_label_new(gettext("Фамилия клиента"));
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), label, FALSE, FALSE, 1);

data.entry[E_FIO] = gtk_entry_new_with_max_length (59);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO], TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_FIO]), "activate",GTK_SIGNAL_FUNC(klientr_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.vvod.fio.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_FIO]),(gpointer)E_FIO);

label=gtk_label_new(gettext("Адрес"));
data.entry[E_ADRES] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox[E_ADRES]), label, FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (hbox[E_ADRES]), data.entry[E_ADRES], TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ADRES]), "activate",GTK_SIGNAL_FUNC(klientr_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ADRES]),data.vvod.adres.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ADRES]),(gpointer)E_ADRES);


label=gtk_label_new(gettext("Телефон"));
data.entry[E_TELEFON] = gtk_entry_new_with_max_length (39);
gtk_box_pack_start (GTK_BOX (hbox[E_TELEFON]), label, FALSE, FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox[E_TELEFON]), data.entry[E_TELEFON], TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TELEFON]), "activate",GTK_SIGNAL_FUNC(klientr_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TELEFON]),data.vvod.telef.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TELEFON]),(gpointer)E_TELEFON);

sprintf(strsql,"%s (,,)",gettext("Список подразделений вход в которые запрещён"));
data.knopka_enter[E_SP_PODR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SP_PODR]), data.knopka_enter[E_SP_PODR], FALSE, FALSE, 1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SP_PODR]),"clicked",GTK_SIGNAL_FUNC(l_klientr_v_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SP_PODR]),(gpointer)E_SP_PODR);
tooltips_enter[E_SP_PODR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SP_PODR],data.knopka_enter[E_SP_PODR],gettext("Выбор подразделения"),NULL);

data.entry[E_SP_PODR] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox[E_SP_PODR]), data.entry[E_SP_PODR], TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SP_PODR]), "activate",GTK_SIGNAL_FUNC(klientr_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SP_PODR]),data.vvod.sp_podr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SP_PODR]),(gpointer)E_SP_PODR);


sprintf(strsql,"%s (%s)",gettext("Дата рождения"),gettext("д.м.г"));
data.knopka_enter[E_DEN_ROG]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DEN_ROG]), data.knopka_enter[E_DEN_ROG], FALSE, FALSE, 1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DEN_ROG]),"clicked",GTK_SIGNAL_FUNC(l_klientr_v_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DEN_ROG]),(gpointer)E_DEN_ROG);
tooltips_enter[E_DEN_ROG]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DEN_ROG],data.knopka_enter[E_DEN_ROG],gettext("Выбор даты"),NULL);

data.entry[E_DEN_ROG] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DEN_ROG]), data.entry[E_DEN_ROG], FALSE, FALSE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DEN_ROG]), "activate",GTK_SIGNAL_FUNC(klientr_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DEN_ROG]),data.vvod.denrog.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DEN_ROG]),(gpointer)E_DEN_ROG);

label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 1);

data.entry[E_KOMENT] = gtk_entry_new_with_max_length (59);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(klientr_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.vvod.koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);

label=gtk_label_new(gettext("Группа"));
gtk_box_pack_start (GTK_BOX (hbox[KOLENTER]), label, FALSE, FALSE, 1);
klientr_kodgr(&data);
gtk_box_pack_start (GTK_BOX (hbox[KOLENTER]), data.opt, TRUE, TRUE, 1);

label=gtk_label_new(gettext("Контроль входа/выхода"));
gtk_box_pack_start (GTK_BOX (hbox[KOLENTER+1]), label, FALSE, FALSE, 1);
klientr_kontrvv(&data);
gtk_box_pack_start (GTK_BOX (hbox[KOLENTER+1]), data.opt_mk, TRUE, TRUE, 1);

label=gtk_label_new(gettext("Пол"));
gtk_box_pack_start (GTK_BOX (hbox[KOLENTER+2]), label, FALSE, FALSE, 1);
klientr_pol(&data);
gtk_box_pack_start (GTK_BOX (hbox[KOLENTER+2]), data.opt_pol, TRUE, TRUE, 1);

label=gtk_label_new(gettext("Блокировка"));
gtk_box_pack_start (GTK_BOX (hbox[KOLENTER+3]), label, FALSE, FALSE, 1);
klientr_blok(&data);
gtk_box_pack_start (GTK_BOX (hbox[KOLENTER+3]), data.opt_blok, TRUE, TRUE, 1);


GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(klientr_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(klientr_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

if(data.voz == 0)
 kodkl->new_plus(data.vvod.kod.ravno());

return(data.voz);



}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_klientr_v_v_e_knopka(GtkWidget *widget,class klientr_v_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case E_SP_PODR:
    if(rpod_l(1,&kod,&naim,data->window) == 0)
     data->vvod.sp_podr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SP_PODR]),data->vvod.sp_podr.ravno());
    return;  

  case E_DEN_ROG:
    iceb_calendar(&data->vvod.denrog,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DEN_ROG]),data->vvod.denrog.ravno());
    return;  
    
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   klientr_v_key_press(GtkWidget *widget,GdkEventKey *event,class klientr_v_data *data)
{

//printf("klientr_v_key_press\n");
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
void  klientr_v_knopka(GtkWidget *widget,class klientr_v_data *data)
{
//char bros[300];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case FK2:
    data->read_rek();
    if(klientr_zap(data) == 0)
     {
      if(data->timer != 0)
        gtk_timeout_remove(data->timer);
      gtk_widget_destroy(data->window);
      data->window=NULL;
     }
    return;  


  case FK10:
    if(data->timer != 0)
        gtk_timeout_remove(data->timer);
    gtk_widget_destroy(data->window);
    data->window=NULL;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    klientr_v_vvod(GtkWidget *widget,class klientr_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("klientr_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->vvod.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_FIO:
    data->vvod.fio.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KOMENT:
    data->vvod.koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_ADRES:
    data->vvod.adres.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_TELEFON:
    data->vvod.telef.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_SP_PODR:
    data->vvod.sp_podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int klientr_zap(class klientr_v_data *data)
{
char strsql[1000];
time_t   vrem;
time(&vrem);
short metkakor=0;
if(data->vvod.kod.getdlinna() <= 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не введён код !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 }
if(data->vvod.denrog.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата рождения !"),data->window);
  return(1);
 }
printf("klientr_v_zap %d\n",data->vib_kodgr);

data->vvod.kodgr.new_plus(data->menu_kodgr.ravno(data->vib_kodgr));

if(data->kodkl.getdlinna() <= 1)
 {
   if(klientr_pk(data->vvod.kod.ravno(),data->window) != 0)
    return(1);

  sprintf(strsql,"insert into Taxiklient \
values ('%s','%s','%s','%s',%d,%ld,%.2f,'%s','%s',%d,'0000-00-00 0:0:0',%d,'%s','%s',%d)",
  data->vvod.kod.ravno_filtr(),
  data->vvod.fio.ravno_filtr(),
  data->vvod.adres.ravno_filtr(),
  data->vvod.telef.ravno_filtr(),
  iceb_getuid(data->window),vrem,
  0.,
  data->vvod.kodgr.ravno_filtr(),
  data->vvod.sp_podr.ravno_filtr(),
  data->vvod.metka_kvv,
  data->vvod.pol,
  data->vvod.denrog.ravno_sqldata(),
  data->vvod.koment.ravno(),
  data->vvod.metka_bl);

 }
else
 {
  if(iceb_u_SRAV(data->vvod.kod.ravno(),data->kodkl.ravno(),0) != 0)
   {
   if(klientr_pk(data->vvod.kod.ravno(),data->window) != 0)
    return(1);
    iceb_u_str repl;
    repl.plus("Корректировать код клиента ? Вы уверены ?");
    if(iceb_menu_danet(&repl,2,NULL) == 2)
      return(1);
    metkakor=1;
   }

  sprintf(strsql,"update Taxiklient \
set \
kod='%s',\
fio='%s',\
adres='%s',\
telef='%s',\
ktoi=%d,\
vrem=%ld,\
grup='%s',\
sp='%s',\
mk=%d,\
pl=%d,\
denrog='%s',\
kom='%s',\
mb=%d \
where kod='%s'",
  data->vvod.kod.ravno_filtr(),
  data->vvod.fio.ravno_filtr(),
  data->vvod.adres.ravno_filtr(),
  data->vvod.telef.ravno_filtr(),
  iceb_getuid(data->window),
  vrem,
  data->vvod.kodgr.ravno_filtr(),
  data->vvod.sp_podr.ravno_filtr(),
  data->vvod.metka_kvv,
  data->vvod.pol,
  data->vvod.denrog.ravno_sqldata(),
  data->vvod.koment.ravno(),
  data->vvod.metka_bl,
  data->kodkl.ravno_filtr()); 
  

 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

if(metkakor == 1)
  iceb_klient_kor(data->kodkl.ravno(),data->vvod.kod.ravno()); 

return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int klientr_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[300];

sprintf(strsql,"select kod from Taxiklient where kod='%s'",
kod);

//printf("klientr_pk-%s\n",strsql);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Такая запись уже есть !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

return(0);

}
/************************************/
/*Подпрограмма создания пункта меню - группа */
/**************************************************/

void klientr_kodgr(class klientr_v_data *data)
{

data->opt = gtk_option_menu_new();
GtkWidget *menu = gtk_menu_new();
int nomer=0;
GtkWidget *item;
char strsql[300];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select kod,naik from Grupklient");
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  data->menu_kodgr.plus(row[0]);
  sprintf(strsql,"%s %s",row[0],row[1]);
    
  item = gtk_menu_item_new_with_label (iceb_u_toutf(strsql));
  gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(iceb_get_menu),&data->vib_kodgr);
  gtk_object_set_user_data(GTK_OBJECT(item),(gpointer)nomer++);
  gtk_widget_show (item);
  gtk_menu_append (GTK_MENU (menu), item);

 }

gtk_option_menu_set_menu (GTK_OPTION_MENU (data->opt), menu);

data->vib_kodgr=data->menu_kodgr.find(data->vvod.kodgr.ravno());

if(data->vib_kodgr < 0)
  data->vib_kodgr=0;

gtk_option_menu_set_history(GTK_OPTION_MENU(data->opt),data->vib_kodgr);

}

/************************************/
/*Подпрограмма создания пункта меню контроль входа-выхода */
/**************************************************/

void klientr_kontrvv(class klientr_v_data *data)
{

data->opt_mk = gtk_option_menu_new();
GtkWidget *menu = gtk_menu_new();
int nomer=0;
GtkWidget *item;

    
item = gtk_menu_item_new_with_label (gettext("Включено"));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(iceb_get_menu),&data->vvod.metka_kvv);
gtk_object_set_user_data(GTK_OBJECT(item),(gpointer)nomer++);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (gettext("Выключено"));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(iceb_get_menu),&data->vvod.metka_kvv);
gtk_object_set_user_data(GTK_OBJECT(item),(gpointer)nomer++);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

gtk_option_menu_set_menu (GTK_OPTION_MENU (data->opt_mk), menu);
printf("metka_kvv=%d\n",data->vvod.metka_kvv);
gtk_option_menu_set_history(GTK_OPTION_MENU(data->opt_mk),data->vvod.metka_kvv);

}

/************************************/
/*Подпрограмма создания пункта меню пол */
/**************************************************/

void klientr_pol(class klientr_v_data *data)
{

data->opt_pol = gtk_option_menu_new();
GtkWidget *menu = gtk_menu_new();
int nomer=0;
GtkWidget *item;

    
item = gtk_menu_item_new_with_label (gettext("Мужчина"));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(iceb_get_menu),&data->vvod.pol);
gtk_object_set_user_data(GTK_OBJECT(item),(gpointer)nomer++);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (gettext("Женщина"));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(iceb_get_menu),&data->vvod.pol);
gtk_object_set_user_data(GTK_OBJECT(item),(gpointer)nomer++);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

gtk_option_menu_set_menu (GTK_OPTION_MENU (data->opt_pol), menu);
printf("pol=%d\n",data->vvod.pol);
gtk_option_menu_set_history(GTK_OPTION_MENU(data->opt_pol),data->vvod.pol);

}

/************************************/
/*Подпрограмма создания пункта меню блокировки карточки */
/**************************************************/

void klientr_blok(class klientr_v_data *data)
{

data->opt_blok = gtk_option_menu_new();
GtkWidget *menu = gtk_menu_new();
int nomer=0;
GtkWidget *item;

    
item = gtk_menu_item_new_with_label (gettext("Разблокирована"));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(iceb_get_menu),&data->vvod.metka_bl);
gtk_object_set_user_data(GTK_OBJECT(item),(gpointer)nomer++);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (gettext("Заблокирована"));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(iceb_get_menu),&data->vvod.metka_bl);
gtk_object_set_user_data(GTK_OBJECT(item),(gpointer)nomer++);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

gtk_option_menu_set_menu (GTK_OPTION_MENU (data->opt_blok), menu);
gtk_option_menu_set_history(GTK_OPTION_MENU(data->opt_blok),data->vvod.metka_bl);

}

