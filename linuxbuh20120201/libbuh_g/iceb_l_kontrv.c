/*$Id: iceb_l_kontrv.c,v 1.52 2011-08-29 07:13:46 sasa Exp $*/
/*11.08.2011	03.12.2003	Белых А.И.	iceb_l_kontrv.c
Ввод и корректировка записей в список контрагентов
*/
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include "iceb_libbuh.h"
#include "iceb_l_kontr.h"
enum
 {
  E_KONTR,
  E_NAIMK,
  E_ADRES,
  E_KOD,
  E_NOMSH,
  E_MFO,
  E_NAIMB,
  E_ADRESB,
  E_NSPNDS,
  E_GRUP,
  E_TELEF,
  E_DOGOV,
  E_REGNOM,
  E_INNN,
  E_NAIMP,
  E_SHNDS,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK5,
  FK6,
  FK7,
  FK10,
  KOL_FK
 };

class kontr_korkod_data
 {
  public:
  GtkWidget *window;
  iceb_u_str skod;//Старый код контрагента
  iceb_u_str nkod;//Новый код контрагента
 };

class iceb_l_kontrv_data
 {
  public:
    
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class kontr_rek rk;
  
  class iceb_u_str kontr_i;
  class iceb_u_str shetsk; //Счёт списка контрагента    
  
  iceb_l_kontrv_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.kodkontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR])));
    rk.naimkon.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIMK])));
    rk.naimp.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIMP])));
    rk.naimban.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIMB])));
    rk.adres.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_ADRES])));
    rk.adresb.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_ADRESB])));
    rk.kod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD])));
    rk.mfo.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_MFO])));
    rk.nomsh.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMSH])));
    rk.nspnds.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NSPNDS])));
    rk.telef.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_TELEF])));
    rk.grup.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GRUP])));
    rk.regnom.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_REGNOM])));
    rk.innn.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_INNN])));
    rk.dogov.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DOGOV])));
    rk.shnds.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHNDS])));
/*******
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
************/
   }
 };

void       kontr_vdi(const char *kodkon,GtkWidget*);
gboolean   kontrv_delete_event(GtkWidget *widget,GdkEvent *event,class iceb_l_kontrv_data *data);
gboolean   kontrv_key_press(GtkWidget *,GdkEventKey *,class iceb_l_kontrv_data*);
void       kontrv_knopka(GtkWidget *widget,class iceb_l_kontrv_data*);
int        kontr_zap(class iceb_l_kontrv_data*);
int        kontr_pk(const char *);
void       kontr_vvod(GtkWidget *widget,class iceb_l_kontrv_data *data);
gint       kontr_korkod1(class kontr_korkod_data *data);
void  iceb_l_kontrv_v_e_knopka(GtkWidget *widget,class iceb_l_kontrv_data *data);


void iceb_l_kontrv_rk(const char *kontr,GtkWidget *wpredok);


extern char *imabaz;
extern char *name_system;
extern SQL_baza bd;
extern char *organ;

int iceb_l_kontrv(class iceb_u_str *kod_kontr,const char *shetsk,GtkWidget *wpredok)
{
class iceb_l_kontrv_data data;
char    strsql[1024];
SQL_str row;
time_t  vremz=0;
int     ktoz=0;
iceb_u_str stroka;
iceb_u_str strokatit;
iceb_u_str kikz;


data.kontr_i.new_plus(kod_kontr->ravno());    
data.shetsk.new_plus(shetsk);
if(data.shetsk.getdlinna() > 1 && data.kontr_i.getdlinna() <= 1)
 if(iceb_menu_vvod1(gettext("Введите код контрагента"),&data.kontr_i,20,wpredok) != 0)
     return(1);

data.kl_shift=0;

data.rk.clear_zero();
if(data.kontr_i.getdlinna() > 1)
 {

  if(data.kontr_i.ravno()[0] == '\0')
   {
    iceb_menu_soob(gettext("Не выбрана запись !"),wpredok);
    return(1);
   }
  SQLCURSOR cur;
  sprintf(strsql,"select * from Kontragent where kodkon='%s'",data.kontr_i.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.rk.kodkontr.new_plus(row[0]);
  data.rk.naimkon.new_plus(row[1]);
  data.rk.naimban.new_plus(row[2]);
  data.rk.adres.new_plus(row[3]);
  data.rk.adresb.new_plus(row[4]);
  data.rk.kod.new_plus(row[5]);
  data.rk.mfo.new_plus(row[6]);
  data.rk.nomsh.new_plus(row[7]);
  data.rk.innn.new_plus(row[8]);
  data.rk.nspnds.new_plus(row[9]);
  data.rk.telef.new_plus(row[10]);
  data.rk.grup.new_plus(row[11]);
  data.rk.dogov.new_plus(row[14]);
  data.rk.regnom.new_plus(row[15]);
  data.rk.naimp.new_plus(row[16]);
  data.rk.shnds.new_plus(row[17]);
  kikz.plus(iceb_kikz(row[12],row[13],wpredok));
  
  ktoz=atoi(row[12]);
  vremz=atol(row[13]);
 }
else
 {
  sprintf(strsql,"%d",iceb_get_new_kod("Kontragent",1,wpredok));
  data.rk.kodkontr.new_plus(strsql);
 }


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.kontr_i.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));
  strokatit.new_plus(strsql);
  stroka.new_plus(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));
  strokatit.new_plus(strsql);

  stroka.new_plus(gettext("Корректировка записи"));

  stroka.ps_plus(kikz.ravno());  

 }

gtk_window_set_title(GTK_WINDOW(data.window),strokatit.ravno_toutf());

/************
//gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(kontrv_delete_event),data);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_widget_destroyed),&data.window);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(kontrv_key_press),data);
****************/

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(kontrv_key_press),&data);



if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label=gtk_label_new(stroka.ravno_toutf());


GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox1 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox2 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox3 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox4 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox5 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox6 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox7 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox8 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox9 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox10 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox11 = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
gtk_container_add (GTK_CONTAINER (vbox), hbox);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hbox3);
gtk_container_add (GTK_CONTAINER (vbox), hbox4);
gtk_container_add (GTK_CONTAINER (vbox), hbox5);
gtk_container_add (GTK_CONTAINER (vbox), hbox6);
gtk_container_add (GTK_CONTAINER (vbox), hbox7);
gtk_container_add (GTK_CONTAINER (vbox), hbox8);
gtk_container_add (GTK_CONTAINER (vbox), hbox9);
gtk_container_add (GTK_CONTAINER (vbox), hbox10);
gtk_container_add (GTK_CONTAINER (vbox), hbox11);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

label=gtk_label_new(gettext("Код контрагента"));
data.entry[E_KONTR] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), data.entry[E_KONTR], TRUE, TRUE, 0);
//gtk_signal_connect(GTK_OBJECT (data.entry[E_KONTR]), "activate",GTK_SIGNAL_FUNC(iceb_get_text),data.vvod.kodkontr.ravno());
gtk_signal_connect(GTK_OBJECT (data.entry[E_KONTR]), "activate",GTK_SIGNAL_FUNC(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rk.kodkontr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KONTR]),(gpointer)E_KONTR);

label=gtk_label_new(gettext("Наименование"));
data.entry[E_NAIMK] = gtk_entry_new_with_max_length (40);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data.entry[E_NAIMK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIMK]), "activate",GTK_SIGNAL_FUNC(kontr_vvod),&data);
gtk_widget_set_usize(data.entry[E_NAIMK],400,-1);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIMK]),data.rk.naimkon.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIMK]),(gpointer)E_NAIMK);


label=gtk_label_new(gettext("Адрес"));
data.entry[E_ADRES] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data.entry[E_ADRES], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ADRES]), "activate",GTK_SIGNAL_FUNC(kontr_vvod),&data);
gtk_widget_set_usize(data.entry[E_ADRES],400,-1);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ADRES]),data.rk.adres.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ADRES]),(gpointer)E_ADRES);

label=gtk_label_new(gettext("Код ЕГРПОУ"));
data.entry[E_KOD] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox3), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox3), data.entry[E_KOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

label=gtk_label_new(gettext("Счет"));
data.entry[E_NOMSH] = gtk_entry_new_with_max_length (29);
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox4), data.entry[E_NOMSH], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMSH]), "activate",GTK_SIGNAL_FUNC(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMSH]),data.rk.nomsh.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMSH]),(gpointer)E_NOMSH);

label=gtk_label_new(gettext("МФО"));
data.entry[E_MFO] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox4), data.entry[E_MFO], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_MFO]), "activate",GTK_SIGNAL_FUNC(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MFO]),data.rk.mfo.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_MFO]),(gpointer)E_MFO);

label=gtk_label_new(gettext("Наименование банка"));
data.entry[E_NAIMB] = gtk_entry_new_with_max_length (79);
gtk_box_pack_start (GTK_BOX (hbox5), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox5), data.entry[E_NAIMB], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIMB]), "activate",GTK_SIGNAL_FUNC(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIMB]),data.rk.naimban.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIMB]),(gpointer)E_NAIMB);

label=gtk_label_new(gettext("Город банка"));
data.entry[E_ADRESB] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox6), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox6), data.entry[E_ADRESB], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ADRESB]), "activate",GTK_SIGNAL_FUNC(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ADRESB]),data.rk.adresb.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ADRESB]),(gpointer)E_ADRESB);

label=gtk_label_new(gettext("Номер св. НДС"));
data.entry[E_NSPNDS] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox6), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox6), data.entry[E_NSPNDS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NSPNDS]), "activate",GTK_SIGNAL_FUNC(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NSPNDS]),data.rk.nspnds.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NSPNDS]),(gpointer)E_NSPNDS);


sprintf(strsql,"%s",gettext("Группа"));
data.knopka_enter[E_GRUP]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox7), data.knopka_enter[E_GRUP], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_GRUP]),"clicked",GTK_SIGNAL_FUNC(iceb_l_kontrv_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_GRUP]),(gpointer)E_GRUP);
tooltips_enter[E_GRUP]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_GRUP],data.knopka_enter[E_GRUP],gettext("Просмотр списка групп контрагентов"),NULL);

data.entry[E_GRUP] = gtk_entry_new_with_max_length (9);
gtk_box_pack_start (GTK_BOX (hbox7), data.entry[E_GRUP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GRUP]), "activate",GTK_SIGNAL_FUNC(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUP]),data.rk.grup.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GRUP]),(gpointer)E_GRUP);


label=gtk_label_new(gettext("Телефон"));
data.entry[E_TELEF] = gtk_entry_new_with_max_length (79);
gtk_box_pack_start (GTK_BOX (hbox7), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox7), data.entry[E_TELEF], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TELEF]), "activate",GTK_SIGNAL_FUNC(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TELEF]),data.rk.telef.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TELEF]),(gpointer)E_TELEF);

label=gtk_label_new(gettext("Договор"));
data.entry[E_DOGOV] = gtk_entry_new_with_max_length (79);
gtk_box_pack_start (GTK_BOX (hbox8), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox8), data.entry[E_DOGOV], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DOGOV]), "activate",GTK_SIGNAL_FUNC(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DOGOV]),data.rk.dogov.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DOGOV]),(gpointer)E_DOGOV);

label=gtk_label_new(gettext("Реги.ном.ч/п"));
data.entry[E_REGNOM] = gtk_entry_new_with_max_length (29);
gtk_box_pack_start (GTK_BOX (hbox9), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox9), data.entry[E_REGNOM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_REGNOM]), "activate",GTK_SIGNAL_FUNC(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_REGNOM]),data.rk.regnom.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_REGNOM]),(gpointer)E_REGNOM);

label=gtk_label_new(gettext("Инд. налог.номер"));
data.entry[E_INNN] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox9), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox9), data.entry[E_INNN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_INNN]), "activate",GTK_SIGNAL_FUNC(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INNN]),data.rk.innn.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_INNN]),(gpointer)E_INNN);

label=gtk_label_new(gettext("Полное наименование"));
data.entry[E_NAIMP] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox10), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox10), data.entry[E_NAIMP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIMP]), "activate",GTK_SIGNAL_FUNC(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIMP]),data.rk.naimp.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIMP]),(gpointer)E_NAIMP);

label=gtk_label_new(gettext("Спец счёт НДС"));
data.entry[E_SHNDS] = gtk_entry_new_with_max_length (29);
gtk_box_pack_start (GTK_BOX (hbox11), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox11), data.entry[E_SHNDS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHNDS]), "activate",GTK_SIGNAL_FUNC(kontr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHNDS]),data.rk.shnds.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHNDS]),(gpointer)E_SHNDS);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(kontrv_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);


sprintf(strsql,"F4 %s",gettext("Код контрагента"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Получить новый код контрагента (В случае если контрагент с таким кодом уже введен.)"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(kontrv_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);

sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатать реквизиты контрагента"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]),"clicked",GTK_SIGNAL_FUNC(kontrv_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);

sprintf(strsql,"F6 %s",gettext("Доп.инф."));
data.knopka[FK6]=gtk_button_new_with_label(strsql);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Ввод дополнительной информации для контрагента"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]),"clicked",GTK_SIGNAL_FUNC(kontrv_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);

sprintf(strsql,"F7 %s",gettext("Договора"));
data.knopka[FK7]=gtk_button_new_with_label(strsql);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Ввод списка договоров"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]),"clicked",GTK_SIGNAL_FUNC(kontrv_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(kontrv_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);

for(int ii=0; ii < KOL_FK; ii++)
 gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[ii], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[1]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

if(data.voz == 0)
 kod_kontr->new_plus(data.rk.kodkontr.ravno());

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  iceb_l_kontrv_v_e_knopka(GtkWidget *widget,class iceb_l_kontrv_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_GRUP:
   
    if(iceb_l_gkontr(1,&kod,&naim,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUP]),kod.ravno_toutf());
      
    return;  
 }
}


/*****************************/
/*Обработчик сигнала delete_event*/
/****************************/
gboolean kontrv_delete_event(GtkWidget *widget,GdkEvent *event,class kontr_data *data)
{
//printf("vkontr_delete_event\n");
gtk_widget_destroy(widget);
return(FALSE);
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kontrv_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_kontrv_data *data)
{

switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
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
//    printf("vkontr_key_press F10\n");
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
void  kontrv_knopka(GtkWidget *widget,class iceb_l_kontrv_data *data)
{
char bros[512];
class iceb_u_str naim;
naim.plus("");
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("kontrv_knopka knop=%d\n",knop);
data->kl_shift=0;

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(kontr_zap(data) == 0)
     {
      gtk_widget_destroy(data->window);
      data->voz=0;
     }
    return;  


  case FK4:
    
    sprintf(bros,"%d",iceb_get_new_kod("Kontragent",1,data->window));
    data->rk.kodkontr.new_plus(bros);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),iceb_u_toutf(data->rk.kodkontr.ravno()));
    return;  

  case FK5:
    iceb_l_kontrv_rk(data->rk.kodkontr.ravno(),data->window);
    return;

  case FK6:
    kontr_vdi(data->rk.kodkontr.ravno(),data->window);
    return;

  case FK7:
    iceb_l_kontdog(0,data->rk.kodkontr.ravno(),data->window);
    return;

  case FK10:
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;
 }
}
/************************************/
/*Запись контрагента в таблицу      */
/*************************************/
int kontr_zap(class iceb_l_kontrv_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);
short metkakor=0;


if(data->rk.kodkontr.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введён код контрагента !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }

if(data->rk.naimkon.ravno()[0] == '\0')
 {
  iceb_u_str soob;
  soob.plus(gettext("Не введено наименование контрагента !"));
  iceb_menu_soob(&soob,data->window);
  
  return(1);
 }

if(data->rk.grup.ravno()[0] != '\0')
 {
  sprintf(strsql,"select kod from Gkont where kod=%s",data->rk.grup.ravno());
  if(sql_readkey(&bd,strsql) < 1)
   {
    iceb_u_str sp;
    sprintf(strsql,"%s %s !",gettext("Не найдена группа"),data->rk.grup.ravno());
    sp.plus(strsql);
    iceb_menu_soob(&sp,data->window);
    return(1);
   }

 }

if(data->kontr_i.getdlinna() <= 1)
 {

  if(kontr_pk(data->rk.kodkontr.ravno()) != 0)
   return(1);
  
  sprintf(strsql,"insert into Kontragent \
values ('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',\
%d,%ld,'%s','%s','%s','%s')",
   data->rk.kodkontr.ravno_filtr(),
   data->rk.naimkon.ravno_filtr(),
   data->rk.naimban.ravno_filtr(),
   data->rk.adres.ravno_filtr(),
   data->rk.adresb.ravno_filtr(),
   data->rk.kod.ravno_filtr(),
   data->rk.mfo.ravno_filtr(),
   data->rk.nomsh.ravno_filtr(),
   data->rk.innn.ravno_filtr(),
   data->rk.nspnds.ravno_filtr(),
   data->rk.telef.ravno_filtr(),
   data->rk.grup.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.dogov.ravno_filtr(),
   data->rk.regnom.ravno_filtr(),
   data->rk.naimp.ravno_filtr(),
   data->rk.shnds.ravno_filtr());
 }
else
 {

  if(iceb_u_SRAV(data->kontr_i.ravno(),data->rk.kodkontr.ravno(),0) != 0)
   {
    if(kontr_pk(data->rk.kodkontr.ravno()) != 0)
     return(1);
  
    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Kontragent \
set \
kodkon='%s',\
naikon='%s',\
naiban='%s',\
adres='%s',\
adresb='%s',\
kod='%s',\
mfo='%s',\
nomsh='%s',\
innn='%s',\
nspnds='%s',\
telef='%s',\
grup='%s',\
ktoz=%d,\
vrem=%ld,\
dogov='%s', \
regnom='%s',\
pnaim='%s',\
rsnds='%s' \
where kodkon='%s'",
   data->rk.kodkontr.ravno_filtr(),
   data->rk.naimkon.ravno_filtr(),
   data->rk.naimban.ravno_filtr(),
   data->rk.adres.ravno_filtr(),
   data->rk.adresb.ravno_filtr(),
   data->rk.kod.ravno_filtr(),
   data->rk.mfo.ravno_filtr(),
   data->rk.nomsh.ravno_filtr(),
   data->rk.innn.ravno_filtr(),
   data->rk.nspnds.ravno_filtr(),
   data->rk.telef.ravno_filtr(),
   data->rk.grup.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->rk.dogov.ravno_filtr(),
   data->rk.regnom.ravno_filtr(),
   data->rk.naimp.ravno_filtr(),
   data->rk.shnds.ravno_filtr(),
   data->kontr_i.ravno_filtr());
 }

//printf("kontr_zap-strsql=%s\n",strsql);

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

if(data->shetsk.ravno()[0] != '\0')
 {
  if(data->kontr_i.getdlinna() <= 1)
    sprintf(strsql,"insert into Skontr \
values ('%s','%s',%d,%ld)",
    data->shetsk.ravno_filtr(),
    data->rk.kodkontr.ravno_filtr(),
    iceb_getuid(data->window),vrem);
  else
    sprintf(strsql,"update Skontr \
set \
kodkon='%s',\
ktoi=%d,\
vrem=%ld \
where ns='%s' and kodkon='%s'",
    data->rk.kodkontr.ravno_filtr(),iceb_getuid(data->window),vrem,
    data->shetsk.ravno_filtr(),
    data->kontr_i.ravno_filtr());
  
  iceb_sql_zapis(strsql,0,0,data->window);
 }

gtk_widget_hide(data->window);

if(metkakor == 1 && data->shetsk.ravno()[0] == '\0')
 {
  gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  printf("Корректировка кода %s %s\n",data->kontr_i.ravno(),data->rk.kodkontr.ravno());
  kontr_korkod(data->kontr_i.ravno(),data->rk.kodkontr.ravno(),data->window);
 }

return(0);
}
/******************************/
/*Пооверка кода контрагента*/
/*****************************/

int kontr_pk(const char *kontr)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr);
if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
 {
  iceb_u_str repl;
  repl.plus_ps(gettext("Такой код контрагента уже есть !"));
  repl.plus(row[0]);
  iceb_menu_soob(&repl,NULL);  
  return(1);
 }
return(0);

}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    kontr_vvod(GtkWidget *widget,class iceb_l_kontrv_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("kontr_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KONTR:
    data->rk.kodkontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NAIMK:
    data->rk.naimkon.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_ADRES:
    data->rk.adres.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_ADRESB:
    data->rk.adresb.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOD:
    data->rk.kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOMSH:
    data->rk.nomsh.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_MFO:
    data->rk.mfo.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NAIMB:
    data->rk.naimban.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NSPNDS:
    data->rk.nspnds.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_GRUP:
    data->rk.grup.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


  case E_TELEF:
    data->rk.telef.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DOGOV:
    data->rk.dogov.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_REGNOM:
    data->rk.regnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_INNN:
    data->rk.innn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NAIMP:
    data->rk.naimp.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHNDS:
    data->rk.shnds.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
 }
enter++;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/**********************************/
/*Ввод дополнительной информации*/
/*********************************/

void  kontr_vdi(const char *kodkon,GtkWidget *wpredok)
{
char strsql[312];

//Установить флаг работы с карточкой
sprintf(strsql,"k%s%s",imabaz,kodkon);
if(sql_flag(&bd,strsql,0,0) != 0)
 {
  iceb_u_str rep;
  rep.plus(gettext("С карточкой уже работает другой оператор !"));
  iceb_menu_soob(&rep,NULL);
  return;
 }


sprintf(strsql,"kontr%d.tmp",getpid());
iceb_dikont('-',strsql,kodkon,"Kontragent1");
iceb_vizred(strsql,wpredok);

iceb_dikont('+',strsql,kodkon,"Kontragent1");
unlink(strsql);

/*Снять флаг работы с карточкой*/
sprintf(strsql,"k%s%s",imabaz,kodkon);
sql_flag(&bd,strsql,0,1);

}
/*************************************/
/*Меню корректировки кода контрагента*/
/*************************************/


void  kontr_korkod(const char *skod,const char *nkod,GtkWidget *wpredok)
{
char strsql[312];

kontr_korkod_data data;
data.skod.plus(skod);
data.nkod.plus(nkod);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

sprintf(strsql,"%s %s",name_system,gettext("Корректировка"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

//gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

GtkWidget *label=gtk_label_new(gettext("Ждите !!!"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

label=gtk_label_new(gettext("Выполняется каскадная корректировка в таблицах"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

/**************
data.bar=gtk_progress_bar_new();

gtk_progress_bar_set_bar_style(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_CONTINUOUS);

gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_LEFT_TO_RIGHT);

gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);
**************/


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна
gtk_widget_show_all(data.window);

gtk_idle_add((GtkFunction)kontr_korkod1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

}

/********************************/
/*Корректировка кода контрагента*/
/********************************/
gint  kontr_korkod1(class kontr_korkod_data *data)
{
char strsql[512];

iceb_refresh();

sprintf(strsql,"update Skontr set kodkon='%s' where kodkon='%s'",data->nkod.ravno(),data->skod.ravno());

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   iceb_u_str SOOB;
   SOOB.plus(gettext("У вас нет полномочий для выполнения этой операции !"));
   iceb_menu_soob(&SOOB,NULL);
   return(FALSE);
  }
 else
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);
 }

iceb_refresh();

sprintf(strsql,"update Prov set kodkon='%s' where kodkon='%s'",data->nkod.ravno(),data->skod.ravno());

if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);


iceb_refresh();

sprintf(strsql,"update Saldo set kodkon='%s' where kodkon='%s'",data->nkod.ravno(),data->skod.ravno());

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DUP_ENTRY) //Запись уже есть
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

iceb_refresh();

sprintf(strsql,"update Dokummat set kontr='%s' where kontr='%s'",data->nkod.ravno(),data->skod.ravno());

if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

iceb_refresh();

sprintf(strsql,"update Gnali set kontr='%s' where kontr='%s'",data->nkod.ravno(),data->skod.ravno());

if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

iceb_refresh();

sprintf(strsql,"update Uosdok set kontr='%s' where kontr='%s'",data->nkod.ravno(),data->skod.ravno());

if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

iceb_refresh();

sprintf(strsql,"update Usldokum set kontr='%s' where kontr='%s'",data->nkod.ravno(),data->skod.ravno());

if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

iceb_refresh();

sprintf(strsql,"update Kasord1 set kontr='%s' where kontr='%s'",data->nkod.ravno(),data->skod.ravno());

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

iceb_refresh();

sprintf(strsql,"update Kontragent1 set kodkon='%s' where kodkon='%s'",data->nkod.ravno(),data->skod.ravno());

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

iceb_refresh();

SQLCURSOR cur;
SQL_str row;
char bros[512];

sprintf(strsql,"select distinct polu from Pltp where polu like '%s#%%'",
data->skod.ravno());
if(cur.make_cursor(&bd,strsql) > 0)
 while(cur.read_cursor(&row) != 0)
  {      
  
   class iceb_u_str rek(data->skod.ravno());
   iceb_u_polen(row[0],bros,sizeof(bros),1,'#');
   rek.plus(iceb_u_adrsimv(iceb_u_strlen(bros),row[0]));

   sprintf(strsql,"update Pltp set polu='%s' where polu='%s'",rek.ravno_filtr(),row[0]);
   iceb_sql_zapis(strsql,1,0,data->window);

  }

sprintf(strsql,"select distinct polu from Pltt where polu like '%s#%%'",
data->skod.ravno());
if(cur.make_cursor(&bd,strsql) > 0)
 while(cur.read_cursor(&row) != 0)
  {      
  
   class iceb_u_str rek(data->skod.ravno());
   iceb_u_polen(row[0],bros,sizeof(bros),1,'#');
   rek.plus(iceb_u_adrsimv(iceb_u_strlen(bros),row[0]));

   sprintf(strsql,"update Pltt set polu='%s' where polu='%s'",rek.ravno_filtr(),row[0]);
   iceb_sql_zapis(strsql,1,0,data->window);


  }


sprintf(strsql,"select distinct polu from Tpltt where polu like '%s#%%'",
data->skod.ravno());
if(cur.make_cursor(&bd,strsql) > 0)
 while(cur.read_cursor(&row) != 0)
  {      
  
   class iceb_u_str rek(data->skod.ravno());
   iceb_u_polen(row[0],bros,sizeof(bros),1,'#');
   rek.plus(iceb_u_adrsimv(iceb_u_strlen(bros),row[0]));

   sprintf(strsql,"update Tpltt set polu='%s' where polu='%s'",rek.ravno_filtr(),row[0]);
   iceb_sql_zapis(strsql,1,0,data->window);


  }
sprintf(strsql,"select distinct polu from Tpltp where polu like '%s#%%'",
data->skod.ravno());
if(cur.make_cursor(&bd,strsql) > 0)
 while(cur.read_cursor(&row) != 0)
  {      
  
   class iceb_u_str rek(data->skod.ravno());
   iceb_u_polen(row[0],bros,sizeof(bros),1,'#');
   rek.plus(iceb_u_adrsimv(iceb_u_strlen(bros),row[0]));

   sprintf(strsql,"update Tpltp set polu='%s' where polu='%s'",rek.ravno_filtr(),row[0]);
   iceb_sql_zapis(strsql,1,0,data->window);

  }

iceb_refresh();

sprintf(strsql,"update Ukrdok1 set kontr='%s' where kontr='%s'",data->nkod.ravno(),data->skod.ravno());

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

iceb_refresh();

sprintf(strsql,"update Ukrdok1 set kdrnn='%s' where kdrnn='%s'",data->nkod.ravno(),data->skod.ravno());

if(sql_zap(&bd,strsql) != 0)
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

gtk_widget_destroy(data->window);
return(FALSE);

}
/******************************/
/*распечатка реквизитов контрагента*/
/*********************************/
void iceb_l_kontrv_rk(const char *kontr,GtkWidget *wpredok)
{
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
FILE *ff;
char imaf[64];


sprintf(strsql,"select * from Kontragent where kodkon='%s'",kontr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) <= 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден контрагент"),kontr);
  
  iceb_menu_soob(strsql,wpredok);
  return;
 }


sprintf(imaf,"kontr%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }


iceb_u_zagolov(gettext("Реквизиты контрагента"),0,0,0,0,0,0,organ,ff);
fprintf(ff,"%s:%s %s\n",gettext("Контрагент"),kontr,row[1]);
fprintf(ff,"---------------------------------------------------------\n");

fprintf(ff,"%s%s\n",gettext("Адрес..............."),row[3]);
fprintf(ff,"%s%s\n",gettext("Код ЕГРПОУ.........."),row[5]);
fprintf(ff,"%s%s\n",gettext("Наименование банка.."),row[2]);
fprintf(ff,"%s%s\n",gettext("МФО................."),row[6]);
fprintf(ff,"%s%s\n",gettext("Номер счета........."),row[7]);
fprintf(ff,"%s%s\n",gettext("Инд. нал. номер....."),row[8]);
fprintf(ff,"%s%s\n",gettext("Город (банка)......."),row[4]);
fprintf(ff,"%s%s\n",gettext("Номер св. пл. НДС..."),row[9]);
fprintf(ff,"%s%s\n",gettext("Номер телефона......"),row[10]);
fprintf(ff,"%s%s\n",gettext("Группа контрагента.."),row[11]);
fprintf(ff,"%s%s\n",gettext("Договор............."),row[14]);
fprintf(ff,"%s%s\n",gettext("Рег. номер Ч.П. ...."),row[15]);
fprintf(ff,"%s%s\n",gettext("Полное наименование."),row[16]);


sprintf(strsql,"select zapis from Kontragent1 where kodkon='%s' order by nomz asc",kontr);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr > 0)
 {
  fprintf(ff,"\n%s\n",gettext("Дополнительная информация"));
  fprintf(ff,"-----------------------------------------------\n");
  while(cur.read_cursor(&row) != 0)
   fprintf(ff,"%s\n",row[0]);
 }
iceb_podpis(ff,wpredok);
fclose(ff);


iceb_u_spisok fil;
iceb_u_spisok naimf;
fil.plus(imaf);
naimf.plus(gettext("Реквизиты контрагента"));
iceb_ustpeh(imaf,3,wpredok);

iceb_rabfil(&fil,&naimf,"",0,wpredok);

}
