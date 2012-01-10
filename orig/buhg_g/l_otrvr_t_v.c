/*$Id: l_otrvr_t_v.c,v 1.15 2011-02-21 07:35:53 sasa Exp $*/
/*19.09.2006	08.09.2006	Белых А.И.	l_otrvr_t_v.c
Ввод и корректировка вида табеля
*/
#include <stdlib.h>
#include "buhg_g.h"
enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KOL_OTR_DNEI,
  E_KOL_OTR_HAS,
  E_KOL_KAL_DNEI,
  E_KOL_RAB_DNEI,
  E_KOL_RAB_HAS,
  E_DATAN,
  E_DATAK,
  E_KOMENT,
  E_NOM_ZAP,
  KOLENTER  
 };

class l_otrvr_t_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *radiobutton1[2];
  GtkWidget *radiobutton2[2];
  GtkWidget *label_naishet;
  short kl_shift;
  short voz;      //0-ввели 1 нет
  
  //Реквизиты меню
  class iceb_u_str kol_otr_dnei;
  class iceb_u_str kol_otr_has;
  class iceb_u_str kol_kal_dnei;
  class iceb_u_str kol_rab_dnei;
  class iceb_u_str kol_rab_has;
  class iceb_u_str datan;
  class iceb_u_str datak;
  class iceb_u_str koment;
  class iceb_u_str nom_zap;
    
  //Реквизиты корректируемой записи
  int tabnom;
  short mp,gp;
  int kodk; //Код записи которую корректируют
  int nom_zapk;
  int metka_rr;
      
  float dnei; //Количество рабочих дней в просмотриваемом месяце
  float hasov; //Количество рабочих часов в просмотриваемом месяце
  short	maxkkd; /*Максимальное количество календарных дней*/
  
  double oklad; /*Найденный оклад*/
  float has1;   /*Количество часов в одном рабочем дне*/
  float kof;    /*Коэффициент оплаты ставки*/
  int met;      /*0-оклад 1-часовая ставка 2-не пересчитываемый оклад*/
  int koddo;    /*Код должносного оклада*/
  int metka;    /*0-нет метки 1-есть Метка пересчета на процент выполнения плана подразделением*/
  int metka1;    /*0-расчет выполнять по отработанным часам 1-дням*/
  char shet_do[50];
  
  l_otrvr_t_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_rek();
    maxkkd=1;
   }

  void read_rek()
   {
    kol_otr_dnei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOL_OTR_DNEI]))));
    kol_otr_has.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOL_OTR_HAS]))));
    kol_kal_dnei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOL_KAL_DNEI]))));
    kol_rab_dnei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOL_RAB_DNEI]))));
    kol_rab_has.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOL_RAB_HAS]))));
    datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN]))));
    datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK]))));
    koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT]))));
    nom_zap.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOM_ZAP]))));
   }

  void clear_rek()
   {
  
    kol_otr_dnei.new_plus("");
    kol_otr_has.new_plus("");
    kol_kal_dnei.new_plus("");
    kol_rab_dnei.new_plus("");
    kol_rab_has.new_plus("");
    datan.new_plus("");
    datak.new_plus("");
    koment.new_plus("");
    nom_zap.new_plus("");
  
   }
 };

gboolean   l_otrvr_t_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_otrvr_t_v_data *data);
void    l_otrvr_t_v_v_vvod(GtkWidget *widget,class l_otrvr_t_v_data *data);
void  l_otrvr_t_v_v_knopka(GtkWidget *widget,class l_otrvr_t_v_data *data);
void otrvr_t_kkvt(char *skod,char *nkod,GtkWidget *wpredok);
int otrvr_t_zap(class l_otrvr_t_v_data *data);
int otrvr_t_pk(char *kod,GtkWidget *wpredok);
void  otrvr_t_v_e_knopka(GtkWidget *widget,class l_otrvr_t_v_data *data);

int l_otrvr_t_v_zap(class l_otrvr_t_v_data *data);




extern char *name_system;
extern SQL_baza bd;



int l_otrvr_t_v(int metka_rr, //0-ввод новой записи 1- корректировка
int tabnom,short mp,short gp,int vid_tab,int nom_zap,GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
class iceb_u_str kikz;
class l_otrvr_t_v_data data;
data.tabnom=tabnom;
data.mp=mp;
data.gp=gp;
data.kodk=vid_tab;
data.nom_zapk=nom_zap;
data.metka_rr=metka_rr;
char naim_tab[100];
memset(naim_tab,'\0',sizeof(naim_tab));

redkalw(data.mp,data.gp,&data.dnei,&data.hasov,wpredok);

iceb_u_dpm(&data.maxkkd,&data.mp,&data.gp,5);
int metka_no=0;
dolhn1w(data.tabnom,&data.oklad,&data.has1,&data.kof,&data.met,&data.koddo,&data.metka,&data.metka1,data.shet_do,&metka_no,wpredok);

//Читаем наименование табеля
sprintf(strsql,"select naik from Tabel where kod=%d",data.kodk);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 strncpy(naim_tab,row[0],sizeof(naim_tab)-1);




if(data.metka_rr != 0)
 { 
  sprintf(strsql,"select * from Ztab where tabn=%d and god=%d and mes=%d and kodt=%d and nomz=%d",
  data.tabnom,data.gp,data.mp,data.kodk,data.nom_zapk);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    class iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  if(row[4][0] != '0')  
   data.kol_otr_dnei.new_plus(atof(row[4]));
  if(row[5][0] != '0')  
   data.kol_otr_has.new_plus(atof(row[5]));
  if(row[6][0] != '0')  
   data.kol_kal_dnei.new_plus(atof(row[6]));
  if(row[11][0] != '0')  
   data.kol_rab_dnei.new_plus(atof(row[11]));
  if(row[12][0] != '0')  
   data.kol_rab_has.new_plus(atof(row[12]));
  data.datan.new_plus(iceb_u_datzap(row[7]));
  data.datak.new_plus(iceb_u_datzap(row[8]));
  data.koment.new_plus(row[13]);
  data.nom_zap.new_plus(row[14]);

  kikz.plus(iceb_kikz(row[9],row[10],wpredok));
  
 }







data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.metka_rr != 0)
 sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));
else
 sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_otrvr_t_v_v_key_press),&data);

iceb_u_str zagolov;
if(data.metka_rr == 0)
 zagolov.new_plus(gettext("Ввод новой записи"));
else
 {
  zagolov.new_plus(gettext("Корректировка записи"));
  zagolov.ps_plus(kikz.ravno());
 }
zagolov.ps_plus(data.kodk);
zagolov.plus(" ");
zagolov.plus(naim_tab);

GtkWidget *label=gtk_label_new(zagolov.ravno_toutf());

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

GtkWidget *separator1=gtk_hseparator_new();
gtk_container_add (GTK_CONTAINER (vbox), separator1);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

GtkWidget *separator2=gtk_hseparator_new();
gtk_container_add (GTK_CONTAINER (vbox), separator2);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];
/**********
sprintf(strsql,"%s",gettext("Количество рабочих дней"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_OTR_DNEI]), label, FALSE, FALSE, 0);
****************/
sprintf(strsql,"%s",gettext("Количество рабочих дней"));
data.knopka_enter[E_KOL_OTR_DNEI]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_OTR_DNEI]), data.knopka_enter[E_KOL_OTR_DNEI], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOL_OTR_DNEI]),"clicked",GTK_SIGNAL_FUNC(otrvr_t_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOL_OTR_DNEI]),(gpointer)E_KOL_OTR_DNEI);
tooltips_enter[E_KOL_OTR_DNEI]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOL_OTR_DNEI],data.knopka_enter[E_KOL_OTR_DNEI],gettext("Вычисление количества дней если введена дата начала и дата конца"),NULL);

data.entry[E_KOL_OTR_DNEI] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_OTR_DNEI]), data.entry[E_KOL_OTR_DNEI],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOL_OTR_DNEI]), "activate",GTK_SIGNAL_FUNC(l_otrvr_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOL_OTR_DNEI]),data.kol_otr_dnei.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOL_OTR_DNEI]),(gpointer)E_KOL_OTR_DNEI);

sprintf(strsql,"%s",gettext("Количество часов"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_OTR_HAS]), label, FALSE, FALSE, 0);

data.entry[E_KOL_OTR_HAS] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_OTR_HAS]), data.entry[E_KOL_OTR_HAS],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOL_OTR_HAS]), "activate",GTK_SIGNAL_FUNC(l_otrvr_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOL_OTR_HAS]),data.kol_otr_has.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOL_OTR_HAS]),(gpointer)E_KOL_OTR_HAS);
/****************
sprintf(strsql,"%s",gettext("Количество календарних дней"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_KAL_DNEI]), label, FALSE, FALSE, 0);
*****************/
sprintf(strsql,"%s",gettext("Количество календарних дней"));
data.knopka_enter[E_KOL_KAL_DNEI]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_KAL_DNEI]), data.knopka_enter[E_KOL_KAL_DNEI], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOL_KAL_DNEI]),"clicked",GTK_SIGNAL_FUNC(otrvr_t_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOL_KAL_DNEI]),(gpointer)E_KOL_KAL_DNEI);
tooltips_enter[E_KOL_KAL_DNEI]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOL_KAL_DNEI],data.knopka_enter[E_KOL_KAL_DNEI],gettext("Вычисление количества дней если введена дата начала и дата конца"),NULL);

data.entry[E_KOL_KAL_DNEI] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_KAL_DNEI]), data.entry[E_KOL_KAL_DNEI],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOL_KAL_DNEI]), "activate",GTK_SIGNAL_FUNC(l_otrvr_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOL_KAL_DNEI]),data.kol_kal_dnei.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOL_KAL_DNEI]),(gpointer)E_KOL_KAL_DNEI);

sprintf(strsql,"%s",gettext("Количество рабочих дней в месяце"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_RAB_DNEI]), label, FALSE, FALSE, 0);

data.entry[E_KOL_RAB_DNEI] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_RAB_DNEI]), data.entry[E_KOL_RAB_DNEI],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOL_RAB_DNEI]), "activate",GTK_SIGNAL_FUNC(l_otrvr_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOL_RAB_DNEI]),data.kol_rab_dnei.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOL_RAB_DNEI]),(gpointer)E_KOL_RAB_DNEI);

sprintf(strsql,"%s",gettext("Количество рабочих часов в дне"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_RAB_HAS]), label, FALSE, FALSE, 0);

data.entry[E_KOL_RAB_HAS] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_RAB_HAS]), data.entry[E_KOL_RAB_HAS],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOL_RAB_HAS]), "activate",GTK_SIGNAL_FUNC(l_otrvr_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOL_RAB_HAS]),data.kol_rab_has.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOL_RAB_HAS]),(gpointer)E_KOL_RAB_HAS);

sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(otrvr_t_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);
tooltips_enter[E_DATAN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAN],data.knopka_enter[E_DATAN],gettext("Выбор даты"),NULL);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(l_otrvr_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.datan.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);

//sprintf(strsql,"%s",gettext("Дата конца"));
//label=gtk_label_new(iceb_u_toutf(strsql));
//gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), label, FALSE, FALSE, 0);
sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(otrvr_t_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);
tooltips_enter[E_DATAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAK],data.knopka_enter[E_DATAK],gettext("Выбор даты"),NULL);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(l_otrvr_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.datak.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);

sprintf(strsql,"%s",gettext("Коментарий"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new_with_max_length (40);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(l_otrvr_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);

sprintf(strsql,"%s",gettext("Номер записи"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NOM_ZAP]), label, FALSE, FALSE, 0);

data.entry[E_NOM_ZAP] = gtk_entry_new_with_max_length (40);
gtk_box_pack_start (GTK_BOX (hbox[E_NOM_ZAP]), data.entry[E_NOM_ZAP],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOM_ZAP]), "activate",GTK_SIGNAL_FUNC(l_otrvr_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOM_ZAP]),data.nom_zap.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOM_ZAP]),(gpointer)E_NOM_ZAP);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введённой в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_otrvr_t_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_otrvr_t_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);



}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  otrvr_t_v_e_knopka(GtkWidget *widget,class l_otrvr_t_v_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");
short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;
char bros[112];
int nom=0;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {

  case E_KOL_OTR_DNEI:
    data->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATAN]))));
    data->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATAK]))));
    if(data->datan.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введена дата начала!"),data->window);
      return;
     }      
    if(data->datak.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введена дата конца!"),data->window);
      return;
     }      
    if(iceb_rsdatp(&dn,&mn,&gn,data->datan.ravno(),&dk,&mk,&gk,data->datak.ravno(),data->window) != 0)
     return;
       
    for(nom=0; iceb_u_sravmydat(dn,mn,gn,dk,mk,gk) <= 0; nom++)
      {
       iceb_u_dpm(&dn,&mn,&gn,1);     
       if(iceb_vixod(dn,mn,gn,data->window) == 0)
        nom--;
      }

    sprintf(bros,"%d",nom);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOL_OTR_DNEI]),bros);
        
    return;  

  case E_KOL_KAL_DNEI:
    data->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATAN]))));
    data->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATAK]))));
    if(data->datan.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введена дата начала!"),data->window);
      return;
     }      
    if(data->datak.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введена дата конца!"),data->window);
      return;
     }      
    if(iceb_rsdatp(&dn,&mn,&gn,data->datan.ravno(),&dk,&mk,&gk,data->datak.ravno(),data->window) != 0)
     return;
       
    for(nom=0; iceb_u_sravmydat(dn,mn,gn,dk,mk,gk) <= 0; nom++)
     iceb_u_dpm(&dn,&mn,&gn,1);     
    sprintf(bros,"%d",nom);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOL_KAL_DNEI]),bros);
        
    return;  
  case E_DATAN:

    if(iceb_calendar(&data->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->datan.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->datak.ravno());
      
    return;  
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_otrvr_t_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_otrvr_t_v_data *data)
{

//printf("l_otrvr_t_v_v_key_press\n");
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
void  l_otrvr_t_v_v_knopka(GtkWidget *widget,class l_otrvr_t_v_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    if(l_otrvr_t_v_zap(data) != 0)
     return;

    gtk_widget_destroy(data->window);

    data->voz=0;
    return;  

    
  case FK10:
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_otrvr_t_v_v_vvod(GtkWidget *widget,class l_otrvr_t_v_data *data)
{
double bb;
char strsql[512];
int d;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_otrvr_t_v_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOL_OTR_DNEI:
    data->kol_otr_dnei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    if(data->kol_otr_dnei.ravno_atoi() != 0)
     {
      data->kol_otr_has.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_KOL_OTR_HAS]))));
      data->kol_kal_dnei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_KOL_KAL_DNEI]))));
      if(data->kol_otr_has.getdlinna() <= 1)
       {
        if(data->has1 != 0.)
          bb=data->kol_otr_dnei.ravno_atof()*data->has1*data->kof;
        else
         {
          if(data->kol_otr_dnei.ravno_atof() == data->dnei && data->hasov > 0.)
           bb=data->hasov;
          else
           bb=data->kol_otr_dnei.ravno_atof()*8;
         }
        bb=iceb_u_okrug(bb,0.01);
        sprintf(strsql,"%.6g",bb);
        data->kol_otr_has.new_plus(strsql);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOL_OTR_HAS]),strsql);
       }
      if(data->kol_kal_dnei.getdlinna() <= 1 && data->dnei == data->kol_otr_dnei.ravno_atof())
       {
        sprintf(strsql,"%d",data->maxkkd);
        data->kol_kal_dnei.new_plus(strsql);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOL_KAL_DNEI]),strsql);
       }
     }
    break;

  case E_KOL_OTR_HAS:
    data->kol_otr_has.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    bb=data->kol_otr_has.ravno_atof();
    if(iceb_u_polen(data->kol_otr_has.ravno(),strsql,sizeof(strsql),2,':') == 0)
     {
      bb=(bb*60+iceb_u_atof(strsql))/60;
     }
    if(bb != 0.)
     {
      sprintf(strsql,"%.10g",bb);
      data->kol_otr_has.new_plus(strsql);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOL_OTR_HAS]),strsql);
     }
    break;

  case E_KOL_KAL_DNEI:
    data->kol_kal_dnei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    d=data->kol_kal_dnei.ravno_atoi();
    if(d > data->maxkkd)
     {
      
      sprintf(strsql,"%s %d !",gettext("В этом месяце календарных дней не может быть больше"),data->maxkkd);
      iceb_menu_soob(strsql,data->window);
      
     }
    break;

  case E_KOL_RAB_DNEI:
    data->kol_rab_dnei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KOL_RAB_HAS:
    data->kol_rab_has.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_DATAN:
    data->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_DATAK:
    data->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KOMENT:
    data->koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NOM_ZAP:
    data->nom_zap.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/*********************/
/*запись*/
/*********************/

int l_otrvr_t_v_zap(class l_otrvr_t_v_data *data)
{
data->read_rek();

short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;

if(data->datan.getdlinna() > 1)
 if(iceb_u_rsdat(&dn,&mn,&gn,data->datan.ravno(),1) != 0)
  {
   iceb_menu_soob(gettext("Не правильно введена дата начала !"),data->window);
   return(1);
  }

if(data->datak.getdlinna() > 1)
 if(iceb_u_rsdat(&dk,&mk,&gk,data->datak.ravno(),1) != 0)
  {
   iceb_menu_soob(gettext("Не правильно введена дата конца !"),data->window);
   return(1);
  }

int voz=zaptabelw(data->metka_rr,data->mp,data->gp,data->tabnom,data->kodk,
data->kol_otr_dnei.ravno_atof(),
data->kol_otr_has.ravno_atof(),
data->kol_kal_dnei.ravno_atof(),
dn,mn,gn,
dk,mk,gk,
data->kol_rab_dnei.ravno_atof(),
data->kol_rab_has.ravno_atof(),
data->koment.ravno(),
data->nom_zap.ravno_atoi(),
data->nom_zapk,
data->window);
 
return(voz);

}
