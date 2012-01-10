/*$Id: ukrvhd.c,v 1.15 2011-02-21 07:35:58 sasa Exp $*/
/*18.11.2008	18.02.2008	Белых А.И.	ukrvhd.c
Ввод и корректировка шапки документа для "Учёта командировочных расходов"
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"


enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

enum
 {
  E_KONTR,
  E_DATADOK,
  E_NOMDOK,
  E_VIDKOM,
  E_PUNKT_NAZ,
  E_ORGANIZ,
  E_CEL_KOM,
  E_NOMER_PRIK,
  E_DATA_PRIK,
  E_DATA_OTEZ,
  E_DATA_PRIEZ,
  E_NOMER_AO,
  E_DATA_AO,
  KOLENTER  
 };

class ukrvhd_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_naim[2];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str datadokz;
  class iceb_u_str nomdokz;
  class iceb_u_str kontrz;
  /*Реквизиты меню*/
  class iceb_u_str kontr;
  class iceb_u_str datadok;
  class iceb_u_str nomdok;
  class iceb_u_str vidkom;
  class iceb_u_str punkt_naz;
  class iceb_u_str organiz;
  class iceb_u_str cel_kom;
  class iceb_u_str nomer_prik;
  class iceb_u_str data_prik;
  class iceb_u_str data_otez;
  class iceb_u_str data_priez;
  class iceb_u_str nomer_ao;
  class iceb_u_str data_ao;
  
    
  ukrvhd_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    kontrz.plus("");
   }

  void read_rek()
   {
    kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR]))));
    datadok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATADOK]))));
    nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK]))));
    vidkom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VIDKOM]))));
    punkt_naz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PUNKT_NAZ]))));
    organiz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_ORGANIZ]))));
    cel_kom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_CEL_KOM]))));
    nomer_prik.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMER_PRIK]))));
    data_prik.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_PRIK]))));
    data_otez.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_OTEZ]))));
    data_priez.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_PRIEZ]))));
    nomer_ao.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMER_AO]))));
    data_ao.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_AO]))));
   }

  void clear()
   {
    kontr.new_plus("");
    datadok.new_plus("");
    nomdok.new_plus("");
    vidkom.new_plus("");
    punkt_naz.new_plus("");
    organiz.new_plus("");
    cel_kom.new_plus("");
    nomer_prik.new_plus("");
    data_prik.new_plus("");
    data_otez.new_plus("");
    data_priez.new_plus("");
    nomer_ao.new_plus("");
    data_ao.new_plus("");

   }
 };


gboolean   ukrvhd_key_press(GtkWidget *widget,GdkEventKey *event,class ukrvhd_data *data);
void  ukrvhd_knopka(GtkWidget *widget,class ukrvhd_data *data);
void    ukrvhd_vvod(GtkWidget *widget,class ukrvhd_data *data);
int l_gruppod_zap(class ukrvhd_data *data);
void  ukrvhd_v_e_knopka(GtkWidget *widget,class ukrvhd_data *data);


extern SQL_baza  bd;
extern char      *name_system;

int ukrvhd(class iceb_u_str *datadok,class iceb_u_str *nomdok,GtkWidget *wpredok)
{
class iceb_u_str naim_kontr("");
class iceb_u_str naim_vidkom("");

class ukrvhd_data data;
char strsql[512];
data.clear();
data.datadokz.plus(datadok->ravno());
data.nomdokz.plus(nomdok->ravno());

if(data.datadokz.getdlinna() >  1)
 {
  sprintf(strsql,"select * from Ukrdok where god=%d and nomd='%s'",
  data.datadokz.ravno_god(),
  data.nomdokz.ravno());
  
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.nomdok.new_plus(row[1]);
  data.datadok.new_plus(iceb_u_datzap(row[2]));

  data.kontr.new_plus(row[3]);
  data.kontrz.new_plus(row[3]);

  data.punkt_naz.new_plus(row[4]);
  data.organiz.new_plus(row[5]);
  data.cel_kom.new_plus(row[6]);
  data.data_otez.new_plus(iceb_u_datzap(row[7]));
  data.data_priez.new_plus(iceb_u_datzap(row[8]));
  data.nomer_prik.new_plus(row[9]);
  data.data_prik.new_plus(iceb_u_datzap(row[10]));
  data.vidkom.new_plus(row[13]);
  data.nomer_ao.new_plus(row[14]);
  data.data_ao.new_plus(iceb_u_datzap(row[15]));
    
  /*Узнаём наименование контрагента*/
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data.kontr.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_kontr.new_plus(row[0]);  
  /*узнаём наименование вида командировки*/
  sprintf(strsql,"select naik from Ukrvkr where kod='%s'",data.vidkom.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_vidkom.new_plus(row[0]);  
  
 }
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.datadokz.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",name_system,gettext("Ввод нового документа"));
  label=gtk_label_new(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",name_system,gettext("Просмотр шапки документа"));
  iceb_u_str repl;
  repl.plus(gettext("Просмотр шапки документа"));
  label=gtk_label_new(repl.ravno_toutf());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(ukrvhd_key_press),&data);

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
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];


sprintf(strsql,"%s",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KONTR]),"clicked",GTK_SIGNAL_FUNC(ukrvhd_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KONTR]),(gpointer)E_KONTR);
tooltips_enter[E_KONTR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KONTR],data.knopka_enter[E_KONTR],gettext("Выбор контрагента"),NULL);

data.entry[E_KONTR] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KONTR]), "activate",GTK_SIGNAL_FUNC(ukrvhd_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.kontr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KONTR]),(gpointer)E_KONTR);


data.label_naim[0]=gtk_label_new(naim_kontr.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.label_naim[0], TRUE, TRUE,1);



sprintf(strsql,"%s (%s)",gettext("Дата документа"),gettext("д.м.г"));
data.knopka_enter[E_DATADOK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATADOK]), data.knopka_enter[E_DATADOK], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATADOK]),"clicked",GTK_SIGNAL_FUNC(ukrvhd_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATADOK]),(gpointer)E_DATADOK);
tooltips_enter[E_DATADOK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATADOK],data.knopka_enter[E_DATADOK],gettext("Выбор даты"),NULL);

data.entry[E_DATADOK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATADOK]), data.entry[E_DATADOK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATADOK]), "activate",GTK_SIGNAL_FUNC(ukrvhd_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATADOK]),data.datadok.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATADOK]),(gpointer)E_DATADOK);


sprintf(strsql,"%s",gettext("Номер документа"));
data.knopka_enter[E_NOMDOK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.knopka_enter[E_NOMDOK], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_NOMDOK]),"clicked",GTK_SIGNAL_FUNC(ukrvhd_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_NOMDOK]),(gpointer)E_NOMDOK);
tooltips_enter[E_NOMDOK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_NOMDOK],data.knopka_enter[E_NOMDOK],gettext("Получение номера документа"),NULL);

data.entry[E_NOMDOK] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMDOK]), "activate",GTK_SIGNAL_FUNC(ukrvhd_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.nomdok.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMDOK]),(gpointer)E_NOMDOK);


sprintf(strsql,"%s",gettext("Вид командировки"));
data.knopka_enter[E_VIDKOM]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_VIDKOM]), data.knopka_enter[E_VIDKOM], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_VIDKOM]),"clicked",GTK_SIGNAL_FUNC(ukrvhd_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_VIDKOM]),(gpointer)E_VIDKOM);
tooltips_enter[E_VIDKOM]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_VIDKOM],data.knopka_enter[E_VIDKOM],gettext("Выбор вида командировки"),NULL);

data.entry[E_VIDKOM] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_VIDKOM]), data.entry[E_VIDKOM], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VIDKOM]), "activate",GTK_SIGNAL_FUNC(ukrvhd_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VIDKOM]),data.nomdok.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VIDKOM]),(gpointer)E_VIDKOM);

data.label_naim[1]=gtk_label_new(naim_vidkom.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (hbox[E_VIDKOM]), data.label_naim[1], TRUE, TRUE,1);

label=gtk_label_new(gettext("Пунк назначения"));
gtk_box_pack_start (GTK_BOX (hbox[E_PUNKT_NAZ]), label, FALSE, FALSE,1);

data.entry[E_PUNKT_NAZ] = gtk_entry_new_with_max_length (79);
gtk_box_pack_start (GTK_BOX (hbox[E_PUNKT_NAZ]), data.entry[E_PUNKT_NAZ], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PUNKT_NAZ]), "activate",GTK_SIGNAL_FUNC(ukrvhd_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PUNKT_NAZ]),data.punkt_naz.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PUNKT_NAZ]),(gpointer)E_PUNKT_NAZ);


label=gtk_label_new(gettext("Организация"));
gtk_box_pack_start (GTK_BOX (hbox[E_ORGANIZ]), label, FALSE, FALSE,1);

data.entry[E_ORGANIZ] = gtk_entry_new_with_max_length (79);
gtk_box_pack_start (GTK_BOX (hbox[E_ORGANIZ]), data.entry[E_ORGANIZ], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ORGANIZ]), "activate",GTK_SIGNAL_FUNC(ukrvhd_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ORGANIZ]),data.organiz.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ORGANIZ]),(gpointer)E_ORGANIZ);


label=gtk_label_new(gettext("Цель командировки"));
gtk_box_pack_start (GTK_BOX (hbox[E_CEL_KOM]), label, FALSE, FALSE,1);

data.entry[E_CEL_KOM] = gtk_entry_new_with_max_length (79);
gtk_box_pack_start (GTK_BOX (hbox[E_CEL_KOM]), data.entry[E_CEL_KOM], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_CEL_KOM]), "activate",GTK_SIGNAL_FUNC(ukrvhd_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CEL_KOM]),data.cel_kom.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_CEL_KOM]),(gpointer)E_CEL_KOM);


label=gtk_label_new(gettext("Номер приказа"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_PRIK]), label, FALSE, FALSE,1);

data.entry[E_NOMER_PRIK] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_PRIK]), data.entry[E_NOMER_PRIK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMER_PRIK]), "activate",GTK_SIGNAL_FUNC(ukrvhd_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMER_PRIK]),data.nomer_prik.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMER_PRIK]),(gpointer)E_NOMER_PRIK);



sprintf(strsql,"%s (%s)",gettext("Дата приказа"),gettext("д.м.г"));
data.knopka_enter[E_DATA_PRIK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_PRIK]), data.knopka_enter[E_DATA_PRIK], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_PRIK]),"clicked",GTK_SIGNAL_FUNC(ukrvhd_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_PRIK]),(gpointer)E_DATA_PRIK);
tooltips_enter[E_DATA_PRIK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_PRIK],data.knopka_enter[E_DATA_PRIK],gettext("Выбор даты"),NULL);

data.entry[E_DATA_PRIK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_PRIK]), data.entry[E_DATA_PRIK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_PRIK]), "activate",GTK_SIGNAL_FUNC(ukrvhd_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_PRIK]),data.data_prik.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_PRIK]),(gpointer)E_DATA_PRIK);


sprintf(strsql,"%s (%s)",gettext("Дата отъезда"),gettext("д.м.г"));
data.knopka_enter[E_DATA_OTEZ]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_OTEZ]), data.knopka_enter[E_DATA_OTEZ], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_OTEZ]),"clicked",GTK_SIGNAL_FUNC(ukrvhd_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_OTEZ]),(gpointer)E_DATA_OTEZ);
tooltips_enter[E_DATA_OTEZ]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_OTEZ],data.knopka_enter[E_DATA_OTEZ],gettext("Выбор даты"),NULL);

data.entry[E_DATA_OTEZ] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_OTEZ]), data.entry[E_DATA_OTEZ], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_OTEZ]), "activate",GTK_SIGNAL_FUNC(ukrvhd_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_OTEZ]),data.data_otez.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_OTEZ]),(gpointer)E_DATA_OTEZ);


sprintf(strsql,"%s (%s)",gettext("Дата приезда"),gettext("д.м.г"));
data.knopka_enter[E_DATA_PRIEZ]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_PRIEZ]), data.knopka_enter[E_DATA_PRIEZ], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_PRIEZ]),"clicked",GTK_SIGNAL_FUNC(ukrvhd_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_PRIEZ]),(gpointer)E_DATA_PRIEZ);
tooltips_enter[E_DATA_PRIEZ]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_PRIEZ],data.knopka_enter[E_DATA_PRIEZ],gettext("Выбор даты"),NULL);

data.entry[E_DATA_PRIEZ] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_PRIEZ]), data.entry[E_DATA_PRIEZ], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_PRIEZ]), "activate",GTK_SIGNAL_FUNC(ukrvhd_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_PRIEZ]),data.data_priez.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_PRIEZ]),(gpointer)E_DATA_PRIEZ);



label=gtk_label_new(gettext("Номер авансового отчёта"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_AO]), label, FALSE, FALSE,1);

data.entry[E_NOMER_AO] = gtk_entry_new_with_max_length (9);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_AO]), data.entry[E_NOMER_AO], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMER_AO]), "activate",GTK_SIGNAL_FUNC(ukrvhd_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMER_AO]),data.nomer_ao.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMER_AO]),(gpointer)E_NOMER_AO);



sprintf(strsql,"%s (%s)",gettext("Дата авансового отчёта"),gettext("д.м.г"));
data.knopka_enter[E_DATA_AO]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_AO]), data.knopka_enter[E_DATA_AO], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_AO]),"clicked",GTK_SIGNAL_FUNC(ukrvhd_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_AO]),(gpointer)E_DATA_AO);
tooltips_enter[E_DATA_AO]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_AO],data.knopka_enter[E_DATA_AO],gettext("Выбор даты"),NULL);

data.entry[E_DATA_AO] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_AO]), data.entry[E_DATA_AO], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_AO]), "activate",GTK_SIGNAL_FUNC(ukrvhd_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_AO]),data.data_ao.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_AO]),(gpointer)E_DATA_AO);



GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(ukrvhd_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(ukrvhd_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0)
 {
  datadok->new_plus(data.datadok.ravno());
  nomdok->new_plus(data.nomdok.ravno());
 }

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  ukrvhd_v_e_knopka(GtkWidget *widget,class ukrvhd_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");
short d,m,g;
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATADOK:

    if(iceb_calendar(&data->datadok,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATADOK]),data->datadok.ravno());
      
    return;  

  case E_DATA_PRIK:

    if(iceb_calendar(&data->data_prik,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_PRIK]),data->data_prik.ravno());
      
    return;  

  case E_DATA_OTEZ:

    if(iceb_calendar(&data->data_otez,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_OTEZ]),data->data_otez.ravno());
      
    return;  

  case E_DATA_PRIEZ:

    if(iceb_calendar(&data->data_priez,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_PRIEZ]),data->data_priez.ravno());
      
    return;  

  case E_DATA_AO:

    if(iceb_calendar(&data->data_ao,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_AO]),data->data_ao.ravno());
      
    return;  

  case E_NOMDOK:
     data->datadok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATADOK]))));
     if(data->datadok.getdlinna() <= 1)
      {
       iceb_menu_soob(gettext("Не введена дата документа!"),data->window);
       return;
      }

     if(iceb_u_rsdat(&d,&m,&g,data->datadok.ravno(),1) != 0)
      {
       iceb_menu_soob(gettext("Неправильно введена дата документа!"),data->window);
       return;
      }

     data->nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_NOMDOK]))));

     if(data->nomdok.getdlinna() > 1)
      {
       iceb_menu_soob(gettext("Номер документа уже введён!"),data->window);
       return;
      }
     
     iceb_nomnak(g,"",&data->nomdok,0,3,0,data->window);
     
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->nomdok.ravno_toutf());
      
    return;  

  case E_VIDKOM:
     if(l_ukrvk(1,&kod,&naim,data->window) == 0)
      data->vidkom.new_plus(kod.ravno());
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_VIDKOM]),data->vidkom.ravno_toutf());
     gtk_label_set_text(GTK_LABEL(data->label_naim[1]),naim.ravno_toutf(20));
      
    return;  

  case E_KONTR:
     if(iceb_l_kontr(1,&kod,&naim,data->window) == 0)
      data->kontr.new_plus(kod.ravno());
 
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->kontr.ravno_toutf());
     
     gtk_label_set_text(GTK_LABEL(data->label_naim[0]),naim.ravno_toutf(20));
      
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   ukrvhd_key_press(GtkWidget *widget,GdkEventKey *event,class ukrvhd_data *data)
{

//printf("ukrvhd_key_press\n");
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

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  ukrvhd_knopka(GtkWidget *widget,class ukrvhd_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("ukrvhd_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_gruppod_zap(data) == 0)
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

void    ukrvhd_vvod(GtkWidget *widget,class ukrvhd_data *data)
{

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("ukrvhd_vvod enter=%d\n",enter);

short d,m,g;
class iceb_u_str naim("");
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
switch (enter)
 {
  case E_KONTR:

    data->kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->kontr.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     naim.new_plus(row[0]);
     
    gtk_label_set_text(GTK_LABEL(data->label_naim[0]),naim.ravno_toutf(20));
    break;

  case E_VIDKOM:

    data->vidkom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    sprintf(strsql,"select naik from Ukrvkr where kod='%s'",data->vidkom.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     naim.new_plus(row[0]);
     
    gtk_label_set_text(GTK_LABEL(data->label_naim[1]),naim.ravno_toutf(20));
    break;

  case E_DATADOK:

    data->datadok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    if(iceb_u_rsdat(&d,&m,&g,data->datadok.ravno(),1) != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата документа!"),data->window);
      return;
     }

    data->nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_NOMDOK]))));
    if(data->nomdok.getdlinna() <= 1)
     {
      iceb_nomnak(g,"",&data->nomdok,0,3,0,data->window);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->nomdok.ravno_toutf());
     } 
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
int l_gruppod_zap(class ukrvhd_data *data)
{
char strsql[1024];
time_t   vrem;
time(&vrem);

if(l_ukrdok_pb(data->datadokz.ravno(),data->window) != 0)
 return(1);
if(l_ukrdok_pb(data->datadok.ravno(),data->window) != 0)
 return(1);

if(data->kontr.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введён код !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->kontr.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),data->kontr.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
short dd,md,gd;
if(iceb_u_rsdat(&dd,&md,&gd,data->datadok.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата документа!"),data->window);
  return(1);
 }
if(iceb_pbpds(md,gd,data->window) != 0)
 return(1);

if(data->vidkom.getdlinna() <= 1)  //Вид командировки
 {
  iceb_menu_soob(gettext("Не ввели вид командировки!"),data->window);
  return(1);
 }
short dp=0,mp=0,gp=0;  
if(data->data_prik.getdlinna() > 1)  //Дата приказа
 {
  if(iceb_u_rsdat(&dp,&mp,&gp,data->data_prik.ravno(),0) != 0)
   {
    iceb_menu_soob(gettext("Неправильно ввели дату приказа!"),data->window);
    return(1);
   }
 }     
short dot=0,mot=0,got=0;
if(data->data_otez.getdlinna() > 1)
 {
  if(iceb_u_rsdat(&dot,&mot,&got,data->data_otez.ravno(),0) != 0)
   {
    iceb_menu_soob(gettext("Неправильно ввели дату отъезда!"),data->window);
    return(1);
   }
 }
short dpr=0,mpr=0,gpr=0;
if(data->data_priez.getdlinna() > 1)  //Дата приезда
 {
  if(iceb_u_rsdat(&dpr,&mpr,&gpr,data->data_priez.ravno(),0) != 0)
   {
    iceb_menu_soob(gettext("Неправильно ввели дату приезда!"),data->window);
    return(1);
   }
 }
short dao=0,mao=0,gao=0;
if(data->data_ao.getdlinna() > 1)  //Дата авансового отчёта
 {
  if(iceb_u_rsdat(&dao,&mao,&gao,data->data_ao.ravno(),0) != 0)
   {
    iceb_menu_soob(gettext("Неправильно ввели дату авансового отчёта!"),data->window);
    return(1);
   }
 }

if(iceb_u_SRAV(data->nomdokz.ravno(),data->nomdok.ravno(),0) != 0)
 {
  //Проверяем номер документа
  sprintf(strsql,"select god from Ukrdok where god=%d and nomd='%s'",
  gd,data->nomdok.ravno());
  if(iceb_sql_readkey(strsql,data->window) != 0)
   {

    sprintf(strsql,gettext("С номером %s документ уже есть !"),data->nomdok.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }


time(&vrem);

if(data->nomdokz.getdlinna() <= 1) //Новый документ
 {
  sprintf(strsql,"insert into Ukrdok \
values (%d,'%s','%04d-%d-%d','%s','%s','%s','%s','%04d-%d-%d','%04d-%d-%d',\
'%s','%04d-%d-%d',%d,%d,'%s','%s','%04d-%d-%d',%d,%ld)",
  gd,
  data->nomdok.ravno_filtr(),
  gd,md,dd,
  data->kontr.ravno_filtr(),
  data->punkt_naz.ravno_filtr(),
  data->organiz.ravno_filtr(),
  data->cel_kom.ravno_filtr(),
  got,mot,dot,
  gpr,mpr,dpr,
  data->nomer_prik.ravno_filtr(),
  gp,mp,dp,
  0,
  0,
  data->vidkom.ravno_filtr(),
  data->nomer_ao.ravno_filtr(),
  gao,mao,dao,
  iceb_getuid(data->window),
  vrem);
 }
else //Корректировка старого документа
 {
  short dz=0,mz=0,gz=0;
  iceb_u_rsdat(&dz,&mz,&gz,data->datadokz.ravno(),1);
  if(mz != md || gz != gd)
   {
    if(iceb_pvkdd(8,dz,mz,gz,md,gd,0,data->nomdokz.ravno(),0,data->window) != 0)
     return(1);
   }
  sprintf(strsql,"update Ukrdok \
set \
god=%d,\
nomd='%s',\
datd='%04d-%d-%d',\
kont='%s',\
pnaz='%s',\
organ='%s',\
celk='%s',\
datn='%04d-%d-%d',\
datk='%04d-%d-%d',\
nompr='%s',\
datp='%04d-%d-%d',\
mprov=%d,\
mpodt=%d,\
vkom='%s',\
nomao='%s',\
datao='%04d-%d-%d',\
ktoz=%d,\
vrem=%ld \
where god=%d and nomd='%s'",
  gd,
  data->nomdok.ravno_filtr(),
  gd,md,dd,
  data->kontr.ravno_filtr(),
  data->punkt_naz.ravno_filtr(),
  data->organiz.ravno_filtr(),
  data->cel_kom.ravno_filtr(),
  got,mot,dot,
  gpr,mpr,dpr,
  data->nomer_prik.ravno_filtr(),
  gp,mp,dp,
  0,
  0,
  data->vidkom.ravno_filtr(),
  data->nomer_ao.ravno_filtr(),
  gao,mao,dao,
  iceb_getuid(data->window),
  vrem,
  data->datadokz.ravno_god(),data->nomdokz.ravno());
  
 }

if(sql_zap(&bd,strsql) != 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,data->window);
  return(1);
 }

if(data->nomdokz.getdlinna() > 1) //Корректировка записей в документе и в проводках
 {
  short godz=0,mesz=0,denz=0;
  iceb_u_rsdat(&denz,&mesz,&godz,data->datadokz.ravno(),1);
  
  sprintf(strsql,"update Ukrdok1 set datd='%04d-%d-%d', nomd='%s' \
where  datd='%04d-%d-%d' and nomd='%s'",
  gd,md,dd,  data->nomdok.ravno_filtr(),
  data->datadokz.ravno_god(),mesz,denz,data->nomdokz.ravno_filtr());

  iceb_sql_zapis(strsql,1,0,data->window);
//  if(sql_zap(&bd,strsql) != 0)
//    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  /*Исправляем проводки в которых дата проводки совпадает с
  датой документа*/

  sprintf(strsql,"update Prov \
set \
oper='%s',\
nomd='%s',\
datp='%04d-%02d-%02d',\
datd='%04d-%02d-%02d',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%d-%d' and datp='%04d-%d-%d' and nomd='%s' and \
pod=%s and kto='%s'",
  data->vidkom.ravno_filtr(),data->nomdok.ravno_filtr(),gd,md,dd,gd,md,dd,iceb_getuid(data->window),vrem,
  data->datadokz.ravno_god(),mesz,denz,data->datadokz.ravno_god(),mesz,denz,data->nomdokz.ravno_filtr(),"0",gettext("УКР"));
  
  iceb_sql_zapis(strsql,1,0,data->window);

  /*Исправляем остальные проводки*/

  sprintf(strsql,"update Prov \
set \
oper='%s',\
nomd='%s',\
datd='%04d-%02d-%02d',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%02d-%02d' and nomd='%s' and \
pod=%s and kto='%s'",
  data->vidkom.ravno_filtr(),data->nomdok.ravno_filtr(),gd,md,dd,iceb_getuid(data->window),vrem,
  data->datadokz.ravno_god(),mesz,denz,data->nomdokz.ravno_filtr(),"0",gettext("УКР"));

  iceb_sql_zapis(strsql,1,0,data->window);

  if(iceb_u_SRAV(data->kontrz.ravno(),data->kontr.ravno(),0) != 0)
   {
    /*Исправляем код контрагента в проводках*/
    sprintf(strsql,"update Prov \
set \
kodkon='%s' \
where datd='%d-%d-%d' and nomd='%s' and \
pod=%s and kto='%s' and kodkon='%s'",
    data->kontr.ravno_filtr(),
    gd,md,dd,data->nomdokz.ravno_filtr(),"0",gettext("УКР"),data->kontrz.ravno_filtr());
  
    iceb_sql_zapis(strsql,1,0,data->window);
   }
 }


return(0);

}

