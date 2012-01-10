/*$Id: l_zar_dok_zap_v.c,v 1.13 2011-02-21 07:35:55 sasa Exp $*/
/*10.03.2009	29.10.2006	Белых А.И.	l_zar_dok_zap_v.c
Ввод и корректировка групп подразделений
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"

enum
 {
  E_TABNOM,
  E_KOD_NAH,
  E_SUMA,
  E_SHET,
  E_KOMENT,
  E_PODR,
  E_VMES,
  E_KOL_DNEI,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_zar_dok_zap_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_fio;
  GtkWidget *label_naim_kn;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  short dd,md,gd;
  class iceb_u_str nomdok;  
  int prn;
  int podr_k;
  class iceb_u_str tabnom_k;
  class iceb_u_str kod_nah_k;
  class iceb_u_str shet_k;
  int nomz;
  short godz;
  short mesz;
  
  class iceb_u_str tabnom; /*Табельный номер корректироваться не может*/
  class iceb_u_str kod_nah; /*код начисления корректирваться не может*/
  class iceb_u_str suma;
  class iceb_u_str shet;
  class iceb_u_str koment;
  class iceb_u_str vmes;
  class iceb_u_str podr;
  class iceb_u_str kol_dnei;
      
  l_zar_dok_zap_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_rek();
   }

  void read_rek()
   {
    tabnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM]))));
    kod_nah.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_NAH]))));
    suma.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SUMA]))));
    shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET]))));
    koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT]))));
    vmes.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VMES]))));
    podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PODR]))));
    kol_dnei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOL_DNEI]))));
   }

  void clear_rek()
   {
    tabnom.new_plus("");
    kod_nah.new_plus("");
    suma.new_plus("");
    shet.new_plus("");
    koment.new_plus("");
    vmes.new_plus("");
    podr.new_plus("");
    kol_dnei.new_plus("");
   }

 };


gboolean   l_zar_dok_zap_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zar_dok_zap_v_data *data);
void  l_zar_dok_zap_v_knopka(GtkWidget *widget,class l_zar_dok_zap_v_data *data);
void    l_zar_dok_zap_v_vvod(GtkWidget *widget,class l_zar_dok_zap_v_data *data);
int l_zar_dok_zap_zap(class l_zar_dok_zap_v_data *data);

void  l_zar_dok_zap_v_e_knopka(GtkWidget *widget,class l_zar_dok_zap_v_data *data);

extern SQL_baza  bd;
extern char      *name_system;

int l_zar_dok_zap_v(const char *datad,
const char *nomdok,
int prn, //1-начисление 2-удержание
class iceb_u_str *tabnom,
class iceb_u_str *kod_nah,
int podr,
class iceb_u_str *shet,
class iceb_u_str *nomz, 
class iceb_u_str *vmes, /*В счёт какого месяца*/
GtkWidget *wpredok)
{
printf("l_zar_dok_zap_v\n");

class l_zar_dok_zap_v_data data;
char strsql[512];
iceb_u_str kikz;
char fio[512];
char naim_kn[50];
memset(fio,'\0',sizeof(fio));
memset(naim_kn,'\0',sizeof(naim_kn));

iceb_u_rsdat(&data.dd,&data.md,&data.gd,datad,1);
data.nomdok.new_plus(nomdok);

data.podr_k=podr;
data.podr.new_plus(podr);

data.shet.new_plus(shet->ravno());
data.shet_k.new_plus(shet->ravno());

data.nomz=nomz->ravno_atoi();
data.prn=prn;

data.tabnom.new_plus(tabnom->ravno());
data.tabnom_k.new_plus(tabnom->ravno());

data.kod_nah.new_plus(kod_nah->ravno());
data.kod_nah_k.new_plus(kod_nah->ravno());

data.vmes.new_plus(vmes->ravno());
data.godz=data.gd;
data.mesz=data.md;

if(vmes->getdlinna() > 1)
 iceb_u_rsdat1(&data.mesz,&data.godz,vmes->ravno());

if(data.tabnom.getdlinna() >  1)
 {
  //Узнаём сумму и комментарий
  sprintf(strsql,"select suma,kom,ktoz,vrem,godn,mesn,kdn from Zarp where datz='%04d-%02d-%02d' and \
tabn=%s and prn='%d' and knah=%s and godn=%d and mesn=%d and podr=%d and shet='%s' \
and nomz=%d",
  data.gd,data.md,data.dd,data.tabnom.ravno(),data.prn,data.kod_nah.ravno(),data.godz,data.mesz,data.podr_k,
  data.shet.ravno(),data.nomz);
  printf("%s\n",strsql);
  
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  sprintf(strsql,"%.2f",atof(row[0]));
  data.suma.new_plus(strsql);
  data.koment.new_plus(row[1]);
  kikz.plus(iceb_kikz(row[2],row[3],wpredok));
  
  data.vmes.new_plus(row[5]);
  data.vmes.plus(".");
  data.vmes.plus(row[4]);

  data.kol_dnei.new_plus(row[6]);
    
  //читаем фамилию
  sprintf(strsql,"select fio from Kartb where tabn=%d",data.tabnom_k.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strncpy(fio,row[0],sizeof(fio)-1);
 
  //Читаем наименование начисления
  if(data.prn == 1)
   sprintf(strsql,"select naik from Nash where kod=%d",data.kod_nah_k.ravno_atoi());
  if(data.prn == 2)
   sprintf(strsql,"select naik from Uder where kod=%d",data.kod_nah_k.ravno_atoi());
 
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strncpy(naim_kn,row[0],sizeof(naim_kn)-1); 
 }
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.tabnom.getdlinna() <= 1)
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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_zar_dok_zap_v_key_press),&data);

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

data.label_fio=gtk_label_new(iceb_u_toutf(fio));
gtk_container_add (GTK_CONTAINER (vbox), data.label_fio);

data.label_naim_kn=gtk_label_new(iceb_u_toutf(naim_kn));
gtk_container_add (GTK_CONTAINER (vbox), data.label_naim_kn);


for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];


sprintf(strsql,"%s",gettext("Табельный номер"));
data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_TABNOM]),"clicked",GTK_SIGNAL_FUNC(l_zar_dok_zap_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_TABNOM]),(gpointer)E_TABNOM);
tooltips_enter[E_TABNOM]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_TABNOM],data.knopka_enter[E_TABNOM],gettext("Выбор табельного номера"),NULL);

data.entry[E_TABNOM] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TABNOM]), "activate",GTK_SIGNAL_FUNC(l_zar_dok_zap_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.tabnom.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TABNOM]),(gpointer)E_TABNOM);

if(prn == 1)
  sprintf(strsql,"%s",gettext("Код начисления"));
if(prn == 2)
  sprintf(strsql,"%s",gettext("Код удержания"));
data.knopka_enter[E_KOD_NAH]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_NAH]), data.knopka_enter[E_KOD_NAH], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_NAH]),"clicked",GTK_SIGNAL_FUNC(l_zar_dok_zap_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_NAH]),(gpointer)E_KOD_NAH);
tooltips_enter[E_KOD_NAH]=gtk_tooltips_new();
if(prn == 1)
 gtk_tooltips_set_tip(tooltips_enter[E_KOD_NAH],data.knopka_enter[E_KOD_NAH],gettext("Выбор кода начисления"),NULL);
if(prn == 2)
 gtk_tooltips_set_tip(tooltips_enter[E_KOD_NAH],data.knopka_enter[E_KOD_NAH],gettext("Выбор кода удержания"),NULL);

data.entry[E_KOD_NAH] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_NAH]), data.entry[E_KOD_NAH], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_NAH]), "activate",GTK_SIGNAL_FUNC(l_zar_dok_zap_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_NAH]),data.kod_nah.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_NAH]),(gpointer)E_KOD_NAH);


label=gtk_label_new(gettext("Сумма"));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), label, FALSE, FALSE, 0);

data.entry[E_SUMA] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMA]), "activate",GTK_SIGNAL_FUNC(l_zar_dok_zap_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.suma.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMA]),(gpointer)E_SUMA);

sprintf(strsql,"%s",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(l_zar_dok_zap_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(l_zar_dok_zap_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new_with_max_length (39);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(l_zar_dok_zap_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);

sprintf(strsql,"%s (%s)",gettext("В счёт какого месяца"),gettext("м.г"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_VMES]), label, FALSE, FALSE, 0);

data.entry[E_VMES] = gtk_entry_new_with_max_length (8);
gtk_box_pack_start (GTK_BOX (hbox[E_VMES]), data.entry[E_VMES], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VMES]), "activate",GTK_SIGNAL_FUNC(l_zar_dok_zap_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VMES]),data.vmes.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VMES]),(gpointer)E_VMES);


sprintf(strsql,"%s",gettext("Подразделение"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODR]),"clicked",GTK_SIGNAL_FUNC(l_zar_dok_zap_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODR]),(gpointer)E_PODR);
tooltips_enter[E_PODR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_PODR],data.knopka_enter[E_PODR],gettext("Выбор подразделения"),NULL);

data.entry[E_PODR] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODR]), "activate",GTK_SIGNAL_FUNC(l_zar_dok_zap_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.podr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODR]),(gpointer)E_PODR);

sprintf(strsql,"%s",gettext("Количество дней"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_DNEI]), label, FALSE, FALSE, 0);

data.entry[E_KOL_DNEI] = gtk_entry_new_with_max_length (3);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_DNEI]), data.entry[E_KOL_DNEI], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOL_DNEI]), "activate",GTK_SIGNAL_FUNC(l_zar_dok_zap_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOL_DNEI]),data.kol_dnei.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOL_DNEI]),(gpointer)E_KOL_DNEI);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_zar_dok_zap_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_zar_dok_zap_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 {
  tabnom->new_plus(data.tabnom.ravno());
  kod_nah->new_plus(data.kod_nah.ravno());
  shet->new_plus(data.shet.ravno());
  nomz->new_plus(data.nomz);
 }
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_zar_dok_zap_v_e_knopka(GtkWidget *widget,class l_zar_dok_zap_v_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
char naim_r[50];
memset(naim_r,'\0',sizeof(naim_r));

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {

  case E_TABNOM:
    if(l_sptbn(1,&kod,&naim,0,data->window) == 0)
     data->tabnom.new_plus(kod.ravno());

    //читаем фамилию
    sprintf(strsql,"select fio from Kartb where tabn=%d",data->tabnom.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     strncpy(naim_r,row[0],sizeof(naim_r)-1);
   
    gtk_label_set_text(GTK_LABEL(data->label_fio),iceb_u_toutf(naim_r));


    gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->tabnom.ravno_toutf());
    return;  
   
  case E_SHET:

    iceb_vibrek(1,"Plansh",&data->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet.ravno_toutf());

    return;  

  case E_PODR:

    if(l_zarpodr(1,&kod,&naim,data->window) == 0)
     data->podr.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->podr.ravno_toutf());

    return;  

  case E_KOD_NAH:
    if(data->prn == 1)
     if(l_zarnah(1,&kod,&naim,data->window) == 0)
      data->kod_nah.new_plus(kod.ravno());
    if(data->prn == 2)
     if(l_zarud(1,&kod,&naim,data->window) == 0)
      data->kod_nah.new_plus(kod.ravno());

    //Читаем наименование начисления
    if(data->prn == 1)
     sprintf(strsql,"select naik,shet from Nash where kod=%d",data->kod_nah.ravno_atoi());
    if(data->prn == 2)
     sprintf(strsql,"select naik,shet from Uder where kod=%d",data->kod_nah.ravno_atoi());
   
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      strncpy(naim_r,row[0],sizeof(naim_r)-1);
      if(row[1][0] != '\0')
       {
        data->shet.new_plus(row[1]);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet.ravno_toutf());
       }
     }   
    gtk_label_set_text(GTK_LABEL(data->label_naim_kn),iceb_u_toutf(naim_r));

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_NAH]),data->kod_nah.ravno_toutf());

    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zar_dok_zap_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zar_dok_zap_v_data *data)
{
//char  bros[512];

//printf("l_zar_dok_zap_v_key_press\n");
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
void  l_zar_dok_zap_v_knopka(GtkWidget *widget,class l_zar_dok_zap_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_zar_dok_zap_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_zar_dok_zap_zap(data) == 0)
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

void    l_zar_dok_zap_v_vvod(GtkWidget *widget,class l_zar_dok_zap_v_data *data)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
char naim_r[50];
memset(naim_r,'\0',sizeof(naim_r));

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_zar_dok_zap_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_TABNOM:
    data->tabnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    //читаем фамилию
    sprintf(strsql,"select fio from Kartb where tabn=%d",data->tabnom.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     strncpy(naim_r,row[0],sizeof(naim_r)-1);
   
    gtk_label_set_text(GTK_LABEL(data->label_fio),iceb_u_toutf(naim_r));

    break;
  case E_KOMENT:
    data->koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KOD_NAH:
    data->kod_nah.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    //Читаем наименование начисления
    if(data->prn == 1)
     sprintf(strsql,"select naik,shet from Nash where kod=%d",data->kod_nah.ravno_atoi());
    if(data->prn == 2)
     sprintf(strsql,"select naik,shet from Uder where kod=%d",data->kod_nah.ravno_atoi());
   
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      strncpy(naim_r,row[0],sizeof(naim_r)-1);
      if(row[1][0] != '\0')
       {
        data->shet.new_plus(row[1]);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet.ravno_toutf());
       }
     }   
    gtk_label_set_text(GTK_LABEL(data->label_naim_kn),iceb_u_toutf(naim_r));


    break;
  case E_SHET:
    data->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_SUMA:
    data->suma.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int l_zar_dok_zap_zap(class l_zar_dok_zap_v_data *data)
{
char strsql[1024];


if(l_nahud_prov_blok(data->md,data->gd,data->tabnom.ravno_atoi(),data->window) != 0)
   return(1);;


if(data->tabnom_k.getdlinna() > 1)
 if(data->tabnom_k.ravno_atoi() != data->tabnom.ravno_atoi())
  {   
   iceb_menu_soob(gettext("Корректировка табельного номера невозможна !"),data->window);
   return(1);
  }

if(data->kod_nah_k.getdlinna() > 1)
 if(data->kod_nah_k.ravno_atoi() != data->kod_nah.ravno_atoi())
  {   
   iceb_menu_soob(gettext("Корректировка кода начисления невозможна !"),data->window);
   return(1);
  }

//Проверяем табельный номер
sprintf(strsql,"select tabn from Kartb where tabn=%d",data->tabnom.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),data->tabnom.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }



//Проверяем код начисления
if(data->prn == 1)
  sprintf(strsql,"select kod from Nash where kod=%d",data->kod_nah.ravno_atoi());
if(data->prn == 2)
  sprintf(strsql,"select kod from Uder where kod=%d",data->kod_nah.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  if(data->prn == 1)
    sprintf(strsql,"%s %d !",gettext("Не найден код начисления"),data->kod_nah.ravno_atoi());
  if(data->prn == 2)
    sprintf(strsql,"%s %d !",gettext("Не найден код удержания"),data->kod_nah.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

struct OPSHET rek_shet;
//Проверяем счёт
if(iceb_prsh1(data->shet.ravno(),&rek_shet,data->window) != 0)
 return(1);

/*проверяем дату*/
short godz=data->godz;
short mesz=data->mesz;
if(data->vmes.getdlinna() > 1)
 if(iceb_u_rsdat1(&mesz,&godz,data->vmes.ravno()) != 0)
  {
   iceb_menu_soob(gettext("Неправильно введена дата!"),data->window);
   return(1);
  }
SQL_str row;
class SQLCURSOR cur;
/*Проверяем код подразделения*/
if(data->podr.ravno_atoi() > 0)
 {
  sprintf(strsql,"select kod from Podr where kod=%d",data->podr.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s %s!",gettext("Ненайден код подразделения!"),data->podr.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }
else
 {
  /*Читаем в карточке код подразделения*/
  sprintf(strsql,"select podr from Kartb where tabn=%d",data->tabnom.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    sprintf(strsql,"%s %s!",gettext("Ненайдена карточка"),data->tabnom.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);
    
   }
  data->podr.new_plus(row[0]);
  
 }


//Записываем список начислений если его ещё нет
if(data->tabnom_k.getdlinna() <= 1)
 provzzagw(data->md,data->gd,data->tabnom.ravno_atoi(),data->prn,data->podr.ravno_atoi(),data->window); //если подразделение 0,то запишет с кодом подразделения, который в карточке

class ZARP     zp;
zp.tabnom=data->tabnom.ravno_atoi();
zp.prn=data->prn;
zp.knu=data->kod_nah.ravno_atoi();

if(data->tabnom_k.getdlinna() > 1) 
  zp.dz=data->dd; //если день равен нолю, то запись не удаляется

zp.mz=data->md;
zp.gz=data->gd;
zp.mesn=data->mesz; zp.godn=data->godz;
zp.nomz=data->nomz;
zp.podr=data->podr_k;
strcpy(zp.shet,data->shet_k.ravno());
 
int kvoz;
for(int nomzap=data->nomz; nomzap < 1000 ; nomzap++)
 {

  double suma=data->suma.ravno_atof();
  if(data->prn == 2)
   suma*=-1;
  kvoz=zapzarpw(&zp,suma,data->dd,mesz,godz,data->kol_dnei.ravno_atoi(),data->shet.ravno(),data->koment.ravno(),nomzap,data->podr.ravno_atoi(),data->nomdok.ravno(),data->window);
  
  if(kvoz == 0) //записали
   break;

  if(zp.dz != 0) //Корректировка записи - не может быть неуспешного завершения
   return(1);

  if(kvoz != 1) //Не равно - запись уже есть
   return(1);

 }

zarsocw(data->md,data->gd,data->tabnom.ravno_atoi(),NULL,data->window);
zaravprw(data->tabnom.ravno_atoi(),data->md,data->gd,NULL,data->window);

return(0);

}




