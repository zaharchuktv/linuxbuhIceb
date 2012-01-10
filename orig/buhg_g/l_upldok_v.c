/*$Id: l_upldok_v.c,v 1.10 2011-02-21 07:35:54 sasa Exp $*/
/*22.06.2010	12.03.2008	Белых А.И.	l_upldok_v.c
Ввод и корректировка шапки документа для "Учёта путевых листов"
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
#include "l_upldok_rekh.h"

enum
 {
  E_DATA_DOK,
  E_KOD_POD,
  E_NOMDOK,
  E_KOD_AVT,
  E_KOD_VOD,
  E_PS_VIEZD,
  E_PS_VOZV,
  E_PROBEG_PO_GOR,
  E_NST_PO_GOR,
  E_VTPN_PO_GOR,
  E_VTF_PO_GOR,
  E_PROBEG_ZA_GOR,
  E_NST_ZA_GOR,
  E_VTPN_ZA_GOR,
  E_VTF_ZA_GOR,

  E_DATA_VIEZD,
  E_VREM_VIEZD,
  E_DATA_VOZV,
  E_VREM_VOZV,
  E_OSOB_OTM,
  E_NBSO,
  KOLENTER  
 };

enum
 {
  FK2,
  FK3,
  FK10,
  KOL_FK
 };


class l_upldok_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_vod;
  GtkWidget *label_pod;
  GtkWidget *label_avt;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str data_dokz;
  class iceb_u_str kod_podz;
  class iceb_u_str nomdokz;

  class l_upldok_rekh rk;
  
  l_upldok_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    rk.clear();
   }

  void read_rek()
   {
    rk.data_dok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_DOK]))));
    rk.kod_pod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_POD]))));
    rk.nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK]))));
    rk.kod_avt.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_AVT]))));
    rk.kod_vod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_VOD]))));
    rk.ps_viezd.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PS_VIEZD]))));
    rk.ps_vozv.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PS_VOZV]))));
    rk.probeg_po_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PROBEG_PO_GOR]))));
    rk.nst_po_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NST_PO_GOR]))));
    rk.vtpn_po_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VTPN_PO_GOR]))));
    rk.vtf_po_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VTF_PO_GOR]))));
    rk.probeg_za_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PROBEG_ZA_GOR]))));
    rk.nst_za_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NST_ZA_GOR]))));
    rk.vtpn_za_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VTPN_ZA_GOR]))));
    rk.vtf_za_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VTF_ZA_GOR]))));

    rk.data_viezd.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_VIEZD]))));
    rk.vrem_viezd.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VREM_VIEZD]))));

    rk.data_vozv.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_VOZV]))));
    rk.vrem_vozv.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VREM_VOZV]))));

    rk.osob_otm.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_OSOB_OTM]))));
    rk.nbso.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NBSO]))));

   }

 };


gboolean   l_upldok_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_upldok_v_data *data);
void  l_upldok_v_knopka(GtkWidget *widget,class l_upldok_v_data *data);
void    l_upldok_v_vvod(GtkWidget *widget,class l_upldok_v_data *data);
int l_upldok_zap(class l_upldok_v_data *data);

void  l_upldok_v_e_knopka(GtkWidget *widget,class l_upldok_v_data *data);
void l_upldok_v_avt(class l_upldok_v_data *data);
void uplnomdokw(short god,const char *podr,class iceb_u_str *nomdok,GtkWidget *wpredok);
void l_upldok_v_pod(class l_upldok_v_data *data);
int poippl(class iceb_u_str *data_dok,const char *kodavt,GtkWidget*);
int l_upldok_vs1(class l_upldok_rekh *rekhd,GtkWidget *wpredok);

extern SQL_baza  bd;
extern char      *name_system;

int l_upldok_v(class iceb_u_str *data_dok,class iceb_u_str *kod_pod,class iceb_u_str *nomdok,GtkWidget *wpredok)
{
class iceb_u_str naim_pod("");
class iceb_u_str naim_vod("");
class iceb_u_str naim_avt("");

class l_upldok_v_data data;
char strsql[512];
iceb_u_str kikz;
data.data_dokz.new_plus(data_dok->ravno());
data.kod_podz.new_plus(kod_pod->ravno());
data.nomdokz.new_plus(nomdok->ravno());

if(data.data_dokz.getdlinna() >  1)
 {
  sprintf(strsql,"select * from Upldok where god=%d and kp=%d and nomd='%s'",
  data.data_dokz.ravno_god(),data.kod_podz.ravno_atoi(),data.nomdokz.ravno());
  
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  data.rk.data_dok.new_plus(data_dok->ravno());
  data.rk.kod_pod.new_plus(row[1]);
  data.rk.nomdok.new_plus(row[2]);
  data.rk.kod_avt.new_plus(row[5]);
  data.rk.kod_vod.new_plus(row[6]);
  data.rk.ps_viezd.new_plus(row[7]);
  data.rk.ps_vozv.new_plus(row[8]);
  data.rk.probeg_po_gor.new_plus(row[11]);
  data.rk.nst_po_gor.new_plus(row[19]);
  data.rk.vtpn_po_gor.new_plus(row[9]);
  data.rk.vtf_po_gor.new_plus(row[10]);
  data.rk.probeg_za_gor.new_plus(row[24]);
  data.rk.nst_za_gor.new_plus(row[25]);
  data.rk.vtpn_za_gor.new_plus(row[22]);
  data.rk.vtf_za_gor.new_plus(row[23]);
  kikz.plus(iceb_kikz(row[20],row[21],wpredok));


  data.rk.ves_gruza.new_plus(row[26]);
  data.rk.tono_kil.new_plus(row[32]);
  data.rk.nzt_na_gruz.new_plus(row[27]);
  data.rk.ztf_na_gruz.new_plus(row[28]);
  data.rk.vr_dvig.new_plus(row[29]);
  data.rk.nzt_vr_dvig.new_plus(row[30]);
  data.rk.ztf_vr_dvig.new_plus(row[31]);
  
  data.rk.data_viezd.new_plus(iceb_u_datzap(row[12]));
  data.rk.vrem_viezd.new_plus(row[14]);
  data.rk.data_vozv.new_plus(iceb_u_datzap(row[13]));
  data.rk.vrem_vozv.new_plus(row[15]);
  data.rk.osob_otm.new_plus(row[18]);
  data.rk.nbso.new_plus(row[4]);
      
  sprintf(strsql,"select naik from Uplpodr where kod=%d",data.rk.kod_pod.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_pod.new_plus(row[0]);

  sprintf(strsql,"select naik from Uplouot where kod=%d",data.rk.kod_vod.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_vod.new_plus(row[0]);

  sprintf(strsql,"select naik from Uplavt where kod=%d",data.rk.kod_avt.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_avt.new_plus(row[0]);
   
 }
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.data_dokz.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",name_system,gettext("Ввод нового путевого листа"));
  label=gtk_label_new(gettext("Ввод нового путевого листа"));
 }
else
 {
  sprintf(strsql,"%s %s",name_system,gettext("Корректировка шапки путевого листа"));
  iceb_u_str repl;
  repl.plus(gettext("Корректировка шапки путевого листа"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno_toutf());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_upldok_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox = gtk_vbox_new (FALSE,1);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE,1);
GtkWidget *hboxknop = gtk_hbox_new (FALSE,1);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s (%s)",gettext("Дата"),gettext("д.м.г"));
data.knopka_enter[E_DATA_DOK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_DOK]), data.knopka_enter[E_DATA_DOK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_DOK]),"clicked",GTK_SIGNAL_FUNC(l_upldok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_DOK]),(gpointer)E_DATA_DOK);
tooltips_enter[E_DATA_DOK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_DOK],data.knopka_enter[E_DATA_DOK],gettext("Выбор даты"),NULL);

data.entry[E_DATA_DOK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_DOK]), data.entry[E_DATA_DOK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_DOK]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_DOK]),data.rk.data_dok.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_DOK]),(gpointer)E_DATA_DOK);


sprintf(strsql,"%s",gettext("Код подразделения"));
data.knopka_enter[E_KOD_POD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_POD]), data.knopka_enter[E_KOD_POD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_POD]),"clicked",GTK_SIGNAL_FUNC(l_upldok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_POD]),(gpointer)E_KOD_POD);
tooltips_enter[E_KOD_POD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_POD],data.knopka_enter[E_KOD_POD],gettext("Выбор даты"),NULL);

data.entry[E_KOD_POD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_POD]), data.entry[E_KOD_POD], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_POD]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_POD]),data.rk.kod_pod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_POD]),(gpointer)E_KOD_POD);

data.label_pod=gtk_label_new(naim_pod.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_POD]), data.label_pod, FALSE, FALSE,1);


sprintf(strsql,"%s",gettext("Номер документа"));
data.knopka_enter[E_NOMDOK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.knopka_enter[E_NOMDOK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_NOMDOK]),"clicked",GTK_SIGNAL_FUNC(l_upldok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_NOMDOK]),(gpointer)E_NOMDOK);
tooltips_enter[E_NOMDOK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_NOMDOK],data.knopka_enter[E_NOMDOK],gettext("Получение номера документа"),NULL);

data.entry[E_NOMDOK] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMDOK]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk.nomdok.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMDOK]),(gpointer)E_NOMDOK);


sprintf(strsql,"%s",gettext("Код автомобиля"));
data.knopka_enter[E_KOD_AVT]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_AVT]), data.knopka_enter[E_KOD_AVT], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_AVT]),"clicked",GTK_SIGNAL_FUNC(l_upldok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_AVT]),(gpointer)E_KOD_AVT);
tooltips_enter[E_KOD_AVT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_AVT],data.knopka_enter[E_KOD_AVT],gettext("Выбор автомобиля"),NULL);

data.entry[E_KOD_AVT] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_AVT]), data.entry[E_KOD_AVT], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_AVT]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_AVT]),data.rk.kod_avt.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_AVT]),(gpointer)E_KOD_AVT);

data.label_avt=gtk_label_new(naim_avt.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_AVT]), data.label_avt, FALSE, FALSE,1);



sprintf(strsql,"%s",gettext("Код водителя"));
data.knopka_enter[E_KOD_VOD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.knopka_enter[E_KOD_VOD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_VOD]),"clicked",GTK_SIGNAL_FUNC(l_upldok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_VOD]),(gpointer)E_KOD_VOD);
tooltips_enter[E_KOD_VOD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_VOD],data.knopka_enter[E_KOD_VOD],gettext("Выбор водителя"),NULL);

data.entry[E_KOD_VOD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.entry[E_KOD_VOD], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_VOD]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_VOD]),data.rk.kod_vod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_VOD]),(gpointer)E_KOD_VOD);

data.label_vod=gtk_label_new(naim_vod.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.label_vod, FALSE, FALSE,1);


label=gtk_label_new(gettext("Показания спидометра при выезде"));
gtk_box_pack_start (GTK_BOX (hbox[E_PS_VIEZD]), label, FALSE, FALSE,1);

data.entry[E_PS_VIEZD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_PS_VIEZD]), data.entry[E_PS_VIEZD], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PS_VIEZD]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PS_VIEZD]),data.rk.ps_viezd.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PS_VIEZD]),(gpointer)E_PS_VIEZD);


label=gtk_label_new(gettext("Показания спидометра при возвращении"));
gtk_box_pack_start (GTK_BOX (hbox[E_PS_VOZV]), label, FALSE, FALSE,1);

data.entry[E_PS_VOZV] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_PS_VOZV]), data.entry[E_PS_VOZV], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PS_VOZV]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PS_VOZV]),data.rk.ps_vozv.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PS_VOZV]),(gpointer)E_PS_VOZV);


label=gtk_label_new(gettext("Пробег по городу"));
gtk_box_pack_start (GTK_BOX (hbox[E_PROBEG_PO_GOR]), label, FALSE, FALSE,1);

data.entry[E_PROBEG_PO_GOR] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_PROBEG_PO_GOR]), data.entry[E_PROBEG_PO_GOR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PROBEG_PO_GOR]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PROBEG_PO_GOR]),data.rk.probeg_po_gor.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PROBEG_PO_GOR]),(gpointer)E_PROBEG_PO_GOR);


label=gtk_label_new(gettext("Норма списания топлива по городу"));
gtk_box_pack_start (GTK_BOX (hbox[E_NST_PO_GOR]), label, FALSE, FALSE,1);

data.entry[E_NST_PO_GOR] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_NST_PO_GOR]), data.entry[E_NST_PO_GOR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NST_PO_GOR]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NST_PO_GOR]),data.rk.nst_po_gor.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NST_PO_GOR]),(gpointer)E_NST_PO_GOR);


label=gtk_label_new(gettext("Затраты топлива по норме по городу"));
gtk_box_pack_start (GTK_BOX (hbox[E_VTPN_PO_GOR]), label, FALSE, FALSE,1);

data.entry[E_VTPN_PO_GOR] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_VTPN_PO_GOR]), data.entry[E_VTPN_PO_GOR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VTPN_PO_GOR]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VTPN_PO_GOR]),data.rk.vtpn_po_gor.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VTPN_PO_GOR]),(gpointer)E_VTPN_PO_GOR);


label=gtk_label_new(gettext("Затраты топлива фактические по городу"));
gtk_box_pack_start (GTK_BOX (hbox[E_VTF_PO_GOR]), label, FALSE, FALSE,1);

data.entry[E_VTF_PO_GOR] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_VTF_PO_GOR]), data.entry[E_VTF_PO_GOR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VTF_PO_GOR]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VTF_PO_GOR]),data.rk.vtf_po_gor.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VTF_PO_GOR]),(gpointer)E_VTF_PO_GOR);


label=gtk_label_new(gettext("Пробег за городом"));
gtk_box_pack_start (GTK_BOX (hbox[E_PROBEG_ZA_GOR]), label, FALSE, FALSE,1);

data.entry[E_PROBEG_ZA_GOR] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_PROBEG_ZA_GOR]), data.entry[E_PROBEG_ZA_GOR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PROBEG_ZA_GOR]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PROBEG_ZA_GOR]),data.rk.probeg_za_gor.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PROBEG_ZA_GOR]),(gpointer)E_PROBEG_ZA_GOR);


label=gtk_label_new(gettext("Норма списания топлива за городом"));
gtk_box_pack_start (GTK_BOX (hbox[E_NST_ZA_GOR]), label, FALSE, FALSE,1);

data.entry[E_NST_ZA_GOR] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_NST_ZA_GOR]), data.entry[E_NST_ZA_GOR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NST_ZA_GOR]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NST_ZA_GOR]),data.rk.nst_za_gor.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NST_ZA_GOR]),(gpointer)E_NST_ZA_GOR);


label=gtk_label_new(gettext("Затраты топлива по норме за городом"));
gtk_box_pack_start (GTK_BOX (hbox[E_VTPN_ZA_GOR]), label, FALSE, FALSE,1);

data.entry[E_VTPN_ZA_GOR] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_VTPN_ZA_GOR]), data.entry[E_VTPN_ZA_GOR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VTPN_ZA_GOR]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VTPN_ZA_GOR]),data.rk.vtpn_za_gor.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VTPN_ZA_GOR]),(gpointer)E_VTPN_ZA_GOR);


label=gtk_label_new(gettext("Затраты топлива фактические за городом"));
gtk_box_pack_start (GTK_BOX (hbox[E_VTF_ZA_GOR]), label, FALSE, FALSE,1);

data.entry[E_VTF_ZA_GOR] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_VTF_ZA_GOR]), data.entry[E_VTF_ZA_GOR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VTF_ZA_GOR]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VTF_ZA_GOR]),data.rk.vtf_za_gor.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VTF_ZA_GOR]),(gpointer)E_VTF_ZA_GOR);


sprintf(strsql,"%s (%s)",gettext("Дата выезда"),gettext("д.м.г"));
data.knopka_enter[E_DATA_VIEZD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VIEZD]), data.knopka_enter[E_DATA_VIEZD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_VIEZD]),"clicked",GTK_SIGNAL_FUNC(l_upldok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_VIEZD]),(gpointer)E_DATA_VIEZD);
tooltips_enter[E_DATA_VIEZD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_VIEZD],data.knopka_enter[E_DATA_VIEZD],gettext("Выбор даты"),NULL);

data.entry[E_DATA_VIEZD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VIEZD]), data.entry[E_DATA_VIEZD], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_VIEZD]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_VIEZD]),data.rk.data_viezd.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_VIEZD]),(gpointer)E_DATA_VIEZD);


sprintf(strsql,"%s (%s)",gettext("Время выезда"),gettext("ч.м.с"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VIEZD]), label, FALSE, FALSE,1);

data.entry[E_VREM_VIEZD] = gtk_entry_new_with_max_length (8);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VIEZD]), data.entry[E_VREM_VIEZD], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VREM_VIEZD]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VREM_VIEZD]),data.rk.vrem_viezd.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VREM_VIEZD]),(gpointer)E_VREM_VIEZD);


sprintf(strsql,"%s (%s)",gettext("Дата возвращения"),gettext("д.м.г"));
data.knopka_enter[E_DATA_VOZV]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VOZV]), data.knopka_enter[E_DATA_VOZV], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_VOZV]),"clicked",GTK_SIGNAL_FUNC(l_upldok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_VOZV]),(gpointer)E_DATA_VOZV);
tooltips_enter[E_DATA_VOZV]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_VOZV],data.knopka_enter[E_DATA_VOZV],gettext("Выбор даты"),NULL);

data.entry[E_DATA_VOZV] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VOZV]), data.entry[E_DATA_VOZV], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_VOZV]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_VOZV]),data.rk.data_vozv.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_VOZV]),(gpointer)E_DATA_VOZV);


sprintf(strsql,"%s (%s)",gettext("Время возвращения"),gettext("ч.м.с"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VOZV]), label, FALSE, FALSE,1);

data.entry[E_VREM_VOZV] = gtk_entry_new_with_max_length (8);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VOZV]), data.entry[E_VREM_VOZV], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VREM_VOZV]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VREM_VOZV]),data.rk.vrem_vozv.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VREM_VOZV]),(gpointer)E_VREM_VOZV);


sprintf(strsql,"%s",gettext("Особые отметки"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_OSOB_OTM]), label, FALSE, FALSE,1);

data.entry[E_OSOB_OTM] = gtk_entry_new_with_max_length (249);
gtk_box_pack_start (GTK_BOX (hbox[E_OSOB_OTM]), data.entry[E_OSOB_OTM], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_OSOB_OTM]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_OSOB_OTM]),data.rk.osob_otm.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_OSOB_OTM]),(gpointer)E_OSOB_OTM);


sprintf(strsql,"%s",gettext("Номер бланка строгой отчётности"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NBSO]), label, FALSE, FALSE,1);

data.entry[E_NBSO] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_NBSO]), data.entry[E_NBSO], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NBSO]), "activate",GTK_SIGNAL_FUNC(l_upldok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NBSO]),data.rk.nbso.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NBSO]),(gpointer)E_NBSO);


GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_upldok_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F3 %s",gettext("Дополнение"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Ввод и корректировка дополнительной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]),"clicked",GTK_SIGNAL_FUNC(l_upldok_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE,1);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_upldok_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0)
 {
  data_dok->new_plus(data.rk.data_dok.ravno());
  kod_pod->new_plus(data.rk.kod_pod.ravno());
  nomdok->new_plus(data.rk.nomdok.ravno());
 }
 
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_upldok_v_e_knopka(GtkWidget *widget,class l_upldok_v_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATA_DOK:

    if(iceb_calendar(&data->rk.data_dok,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_DOK]),data->rk.data_dok.ravno());
      
    return;  

  case E_DATA_VIEZD:

    if(iceb_calendar(&data->rk.data_viezd,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_VIEZD]),data->rk.data_viezd.ravno());
      
    return;  

  case E_DATA_VOZV:

    if(iceb_calendar(&data->rk.data_vozv,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_VOZV]),data->rk.data_vozv.ravno());
      
    return;  

  case E_KOD_POD:
    if(l_uplpod(1,&kod,&naim,data->window) == 0)
     {
      data->rk.kod_pod.new_plus(kod.ravno());
      l_upldok_v_pod(data);
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_POD]),data->rk.kod_pod.ravno());
      
    return;  

  case E_KOD_AVT:
    if(l_uplavt(1,&kod,&naim,data->window) == 0)
     {
      data->rk.kod_avt.new_plus(kod.ravno());
//      gtk_label_set_text(GTK_LABEL(data->label_avt),naim.ravno_toutf(20));
      l_upldok_v_avt(data);     
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_AVT]),data->rk.kod_avt.ravno());
      
    return;  


  case E_KOD_VOD:
    if(l_uplout(1,&kod,&naim,1,data->window) == 0)
     {
      data->rk.kod_vod.new_plus(kod.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_vod),naim.ravno_toutf(20));
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_VOD]),data->rk.kod_vod.ravno());
      
    return;  

  case E_NOMDOK:
    data->rk.data_dok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATA_DOK]))));
    data->rk.kod_pod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_KOD_POD]))));
    data->rk.nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_NOMDOK]))));

    if(data->rk.data_dok.getdlinna() <= 1)
     return;
     
    if(data->rk.kod_pod.getdlinna() <= 1)
     return;

    uplnomdokw(data->rk.data_dok.ravno_god(),data->rk.kod_pod.ravno(),&data->rk.nomdok,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk.nomdok.ravno_toutf());
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_upldok_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_upldok_v_data *data)
{
//char  bros[512];

//printf("l_upldok_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");

    return(TRUE);

  case GDK_F3:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");

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
void  l_upldok_v_knopka(GtkWidget *widget,class l_upldok_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_upldok_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_upldok_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  

  case FK3:
    l_upldok_vs1(&data->rk,data->window);
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

void    l_upldok_v_vvod(GtkWidget *widget,class l_upldok_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_upldok_v_vvod enter=%d\n",enter);
char strsql[512];
class iceb_u_str naim("");
SQL_str row;
class SQLCURSOR cur;
float bb=0.;
switch (enter)
 {
  case E_DATA_DOK:
    data->rk.data_dok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOD_POD:
    data->rk.kod_pod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(data->rk.kod_pod.getdlinna() > 1)
     {
      sprintf(strsql,"select naik from Uplpodr where kod=%d",data->rk.kod_pod.ravno_atoi());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       naim.new_plus(row[0]);

      gtk_label_set_text(GTK_LABEL(data->label_pod),naim.ravno_toutf(20));
     }
    break;

  case E_KOD_AVT:
    data->rk.kod_avt.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    l_upldok_v_avt(data);

    break;

  case E_KOD_VOD:
    data->rk.kod_vod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    sprintf(strsql,"select naik from Uplouot where kod=%d",data->rk.kod_vod.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     naim.new_plus(row[0]);

    gtk_label_set_text(GTK_LABEL(data->label_vod),naim.ravno_toutf(20));

    break;

  case E_PS_VIEZD:
    data->rk.ps_viezd.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_PS_VOZV:
    data->rk.ps_vozv.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    data->rk.ps_viezd.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_PS_VIEZD]))));
    data->rk.nst_po_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_NST_PO_GOR]))));
    if(data->rk.ps_viezd.ravno_atof() != 0.)
     {
      bb=data->rk.ps_vozv.ravno_atof()-data->rk.ps_viezd.ravno_atof();
      data->rk.probeg_po_gor.new_plus(bb);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_PROBEG_PO_GOR]),data->rk.probeg_po_gor.ravno_toutf());
     }
    if(data->rk.nst_po_gor.ravno_atof() != 0.)
     {
      data->rk.vtpn_po_gor.new_plus(data->rk.nst_po_gor.ravno_atof()/100.*bb);
      data->rk.vtf_po_gor.new_plus(data->rk.nst_po_gor.ravno_atof()/100.*bb);

      gtk_entry_set_text(GTK_ENTRY(data->entry[E_VTPN_PO_GOR]),data->rk.vtpn_po_gor.ravno_toutf());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_VTF_PO_GOR]),data->rk.vtf_po_gor.ravno_toutf());
      
     }
    break;

  case E_PROBEG_PO_GOR:
    data->rk.probeg_po_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    data->rk.vtpn_po_gor.new_plus(data->rk.nst_po_gor.ravno_atof()/100.*data->rk.probeg_po_gor.ravno_atof());
    data->rk.vtf_po_gor.new_plus(data->rk.nst_po_gor.ravno_atof()/100.*data->rk.probeg_po_gor.ravno_atof());

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VTPN_PO_GOR]),data->rk.vtpn_po_gor.ravno_toutf());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VTF_PO_GOR]),data->rk.vtf_po_gor.ravno_toutf());
    if(data->rk.probeg_za_gor.ravno_atof() == 0.)
     {
      data->rk.probeg_za_gor.new_plus(data->rk.ps_vozv.ravno_atof() - data->rk.ps_viezd.ravno_atof() - data->rk.probeg_po_gor.ravno_atof());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_PROBEG_ZA_GOR]),data->rk.probeg_za_gor.ravno_toutf());
      
      data->rk.vtpn_za_gor.new_plus(data->rk.probeg_za_gor.ravno_atof()*data->rk.nst_za_gor.ravno_atof()/100.);
      data->rk.vtf_za_gor.new_plus(data->rk.probeg_za_gor.ravno_atof()*data->rk.nst_za_gor.ravno_atof()/100.);

      gtk_entry_set_text(GTK_ENTRY(data->entry[E_VTPN_ZA_GOR]),data->rk.vtpn_za_gor.ravno_toutf());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_VTF_ZA_GOR]),data->rk.vtf_za_gor.ravno_toutf());
      
     }
    break;

  case E_NST_PO_GOR:
    data->rk.nst_po_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    data->rk.probeg_po_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_PROBEG_PO_GOR]))));
  
    data->rk.vtpn_po_gor.new_plus(data->rk.nst_po_gor.ravno_atof()/100. * data->rk.probeg_po_gor.ravno_atof());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VTPN_PO_GOR]),data->rk.vtpn_po_gor.ravno_toutf());

    data->rk.vtf_po_gor.new_plus(data->rk.nst_po_gor.ravno_atof()/100. * data->rk.probeg_po_gor.ravno_atof());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VTF_PO_GOR]),data->rk.vtf_po_gor.ravno_toutf());
    
    break;

  case E_VTPN_PO_GOR:
    data->rk.vtpn_po_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_VTF_PO_GOR:
    data->rk.vtf_po_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_PROBEG_ZA_GOR:
    data->rk.probeg_za_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    
    data->rk.vtpn_za_gor.new_plus(data->rk.probeg_za_gor.ravno_atof() * data->rk.nst_za_gor.ravno_atof()/100.);
    data->rk.vtf_za_gor.new_plus(data->rk.probeg_za_gor.ravno_atof() * data->rk.nst_za_gor.ravno_atof()/100.);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VTPN_ZA_GOR]),data->rk.vtpn_za_gor.ravno_toutf());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VTF_ZA_GOR]),data->rk.vtf_za_gor.ravno_toutf());
    if(data->rk.probeg_po_gor.ravno_atof() != 0.)    
     {
      data->rk.probeg_po_gor.new_plus(data->rk.ps_vozv.ravno_atof()-data->rk.ps_viezd.ravno_atof()-data->rk.probeg_za_gor.ravno_atof());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_PROBEG_PO_GOR]),data->rk.probeg_po_gor.ravno_toutf());
     }
    data->rk.vtpn_po_gor.new_plus(data->rk.nst_po_gor.ravno_atof()/100.*data->rk.probeg_po_gor.ravno_atof());
    data->rk.vtf_po_gor.new_plus(data->rk.nst_po_gor.ravno_atof()/100.*data->rk.probeg_po_gor.ravno_atof());

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VTPN_PO_GOR]),data->rk.vtpn_po_gor.ravno_toutf());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VTF_PO_GOR]),data->rk.vtf_po_gor.ravno_toutf());
         
    break;

  case E_NST_ZA_GOR:
    data->rk.nst_za_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    data->rk.probeg_za_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_PROBEG_ZA_GOR]))));
  
    data->rk.vtpn_za_gor.new_plus(data->rk.nst_za_gor.ravno_atof()/100. * data->rk.probeg_za_gor.ravno_atof());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VTPN_ZA_GOR]),data->rk.vtpn_za_gor.ravno_toutf());

    data->rk.vtf_za_gor.new_plus(data->rk.nst_za_gor.ravno_atof()/100. * data->rk.probeg_za_gor.ravno_atof());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VTF_ZA_GOR]),data->rk.vtf_za_gor.ravno_toutf());
    break;

  case E_VTPN_ZA_GOR:
    data->rk.vtpn_za_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_VTF_ZA_GOR:
    data->rk.vtf_za_gor.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int l_upldok_zap(class l_upldok_v_data *data)
{
char strsql[1024];
short dv=0,mv=0,gv=0;  

if(iceb_u_rsdat(&dv,&mv,&gv,data->rk.data_dok.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Неправильно ввели дату документа!"),data->window);
  return(1);
 }

if(l_upldok_pb(data->rk.data_dok.ravno(),data->window) != 0)
 return(1);

if(data->data_dokz.getdlinna() > 1)
 if(l_upldok_pb(data->data_dokz.ravno(),data->window) != 0)
  return(1);

if(data->rk.data_viezd.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Неправильно ввели дату выезда!"),data->window);
  return(1);
 }

if(data->rk.data_vozv.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Неправильно ввели дату возвращения!"),data->window);
  return(1);
 }

if(data->rk.vrem_vozv.prov_time() != 0)
 {
  iceb_menu_soob(gettext("Неправильно ввели время возвращения!"),data->window);
  return(1);
 }

if(data->rk.vrem_viezd.prov_time() != 0)
 {
  iceb_menu_soob(gettext("Неправильно ввели время выезда!"),data->window);
  return(1);
 }

if(data->rk.kod_pod.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели код подразделения!"),data->window);
  return(1);
 }
if(data->rk.kod_avt.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели код автомобиля!"),data->window);
  return(1);
 }

sprintf(strsql,"select naik from Uplpodr where kod=%d",data->rk.kod_pod.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден код подразделения"),  data->rk.kod_pod.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

if(iceb_u_SRAV(data->nomdokz.ravno(),data->rk.nomdok.ravno(),0) != 0)
 {
  sprintf(strsql,"select datd from Upldok where god=%d and kp=%d and nomd='%s'",
  gv,data->rk.kod_pod.ravno_atoi(),data->rk.nomdok.ravno());
  if(iceb_sql_readkey(strsql,data->window) > 0)
   {
    sprintf(strsql,gettext("С номером %s документ уже есть !"),
    data->rk.nomdok.ravno());
    iceb_menu_soob(strsql,data->window);

    return(1);
   }
 }

sprintf(strsql,"select naik from Uplavt where kod=%d",data->rk.kod_avt.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден код автомобиля"),
  data->rk.kod_avt.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }


sprintf(strsql,"select naik from Uplouot where kod=%d",data->rk.kod_vod.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден код водителя"),
  data->rk.kod_vod.ravno());

  iceb_menu_soob(strsql,data->window);
  return(1);
 }


time_t   vrem;
time(&vrem);
SQL_str row;
class SQLCURSOR curr;
if(data->data_dokz.getdlinna() <= 1) //Ввод нового документа
 {
  int nom_zap=0;
  //Определяем номер записи для документа
  sprintf(strsql,"select nz from Upldok where datd='%d-%d-%d' and ka=%d order by nz desc limit 1",
  gv,mv,dv,atoi(data->rk.kod_avt.ravno()));
  if(iceb_sql_readkey(strsql,&row,&curr,data->window) > 0)
   nom_zap=atoi(row[0])+1;

  sprintf(strsql,"insert into Upldok \
values ('%04d-%d-%d',%s,'%s',%d,'%s',%s,%s,%d,%d,%.3f,%.3f,%d,\
'%s','%s','%s','%s',0,0,'%s',%.3f,%d,%ld,\
%.3f,%.3f,%d,%.3f,%.2f,%.3f,%.3f,%.2f,%.3f,%.3f,%.3f,%d)",
  gv,mv,dv,
  data->rk.kod_pod.ravno(),
  data->rk.nomdok.ravno(),
  gv,
  data->rk.nbso.ravno_filtr(),
  data->rk.kod_avt.ravno(),
  data->rk.kod_vod.ravno(),
  data->rk.ps_viezd.ravno_atoi(),
  data->rk.ps_vozv.ravno_atoi(),
  data->rk.vtpn_po_gor.ravno_atof(),
  data->rk.vtf_po_gor.ravno_atof(),
  data->rk.probeg_po_gor.ravno_atoi(),
  data->rk.data_viezd.ravno_sqldata(),
  data->rk.data_vozv.ravno_sqldata(),
  
  data->rk.vrem_viezd.ravno_time(),
  data->rk.vrem_vozv.ravno_time(),
  
  data->rk.osob_otm.ravno_filtr(),
  data->rk.nst_po_gor.ravno_atof(),
  iceb_getuid(data->window),vrem,
  data->rk.vtpn_za_gor.ravno_atof(),
  data->rk.vtf_za_gor.ravno_atof(),
  data->rk.probeg_za_gor.ravno_atoi(),
  data->rk.nst_za_gor.ravno_atof(),
  data->rk.ves_gruza.ravno_atof(),
  data->rk.nzt_na_gruz.ravno_atof(),
  data->rk.ztf_na_gruz.ravno_atof(),
  data->rk.vr_dvig.ravno_atof(),
  data->rk.nzt_vr_dvig.ravno_atof(),
  data->rk.ztf_vr_dvig.ravno_atof(),
  data->rk.tono_kil.ravno_atof(),
  nom_zap);

    
 }
else  //Корректировка старого документа 
 {
  //исправляем записи в таблице списания топлива по счетам и объектам
  sprintf(strsql,"update Upldok2 set \
datd='%d-%d-%d',\
kp=%d,\
nomd='%s' \
where datd='%s' and kp=%d and nomd='%s'",
  gv,mv,dv,
  data->rk.kod_pod.ravno_atoi(),
  data->rk.nomdok.ravno(),
  data->data_dokz.ravno_sqldata(),data->kod_podz.ravno_atoi(),data->nomdokz.ravno());

  if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
   return(1);

  //Исправлям записи в документе
  
  sprintf(strsql,"update Upldok1 set \
datd='%d-%d-%d',\
kp=%d,\
nomd='%s',\
ka=%d,\
kv=%d \
where datd='%s' and kp=%d and nomd='%s'",
  gv,mv,dv,
  atoi(data->rk.kod_pod.ravno()),
  data->rk.nomdok.ravno(),
  data->rk.kod_avt.ravno_atoi(),
  data->rk.kod_vod.ravno_atoi(),
  data->data_dokz.ravno_sqldata(),data->kod_podz.ravno_atoi(),data->nomdokz.ravno());

  if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
   return(1);




  //Исправляем шапку документа

  sprintf(strsql,"update Upldok set \
datd='%d-%d-%d',\
kp=%d,\
nomd='%s',\
god=%d,\
nomb='%s',\
ka=%d,\
kv=%d,\
psv=%d,\
psz=%d,\
ztpn=%.3f,\
ztfa=%.3f,\
prob=%d,\
denn='%s',\
denk='%s',\
vremn='%s',\
vremk='%s',\
otmet='%s',\
nst=%.3f,\
ktoz=%d,\
vrem=%ld,\
ztpnzg=%.3f,\
ztfazg=%.3f,\
probzg=%d,\
nstzg=%.3f,\
vesg=%.2f,\
ztvsn=%.3f,\
ztvsf=%.3f,\
vrr=%.2f,\
ztvrn=%.3f,\
ztvrf=%.3f,\
tk=%.3f \
where god=%d and kp=%d and nomd='%s'",
  gv,mv,dv,
  data->rk.kod_pod.ravno_atoi(),
  data->rk.nomdok.ravno(),
  gv,
  data->rk.nbso.ravno_filtr(),
  data->rk.kod_avt.ravno_atoi(),
  data->rk.kod_vod.ravno_atoi(),
  data->rk.ps_viezd.ravno_atoi(),
  data->rk.ps_vozv.ravno_atoi(),
  data->rk.vtpn_po_gor.ravno_atof(),
  data->rk.vtf_po_gor.ravno_atof(),
  data->rk.probeg_po_gor.ravno_atoi(),
  data->rk.data_viezd.ravno_sqldata(),
  data->rk.data_vozv.ravno_sqldata(),
  data->rk.vrem_viezd.ravno_time(),
  data->rk.vrem_vozv.ravno_time(),
  data->rk.osob_otm.ravno_filtr(),
  data->rk.nst_po_gor.ravno_atof(),
  iceb_getuid(data->window),vrem,
  data->rk.vtpn_za_gor.ravno_atof(),
  data->rk.vtf_za_gor.ravno_atof(),
  data->rk.probeg_za_gor.ravno_atoi(),
  data->rk.nst_za_gor.ravno_atof(),
  data->rk.ves_gruza.ravno_atof(),
  data->rk.nzt_na_gruz.ravno_atof(),
  data->rk.ztf_na_gruz.ravno_atof(),
  data->rk.vr_dvig.ravno_atof(),
  data->rk.nzt_vr_dvig.ravno_atof(),
  data->rk.ztf_vr_dvig.ravno_atof(),
  data->rk.tono_kil.ravno_atof(),
  data->data_dokz.ravno_god(),data->kod_podz.ravno_atoi(),data->nomdokz.ravno());
  

  
 }

 
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


gtk_widget_hide(data->window);


return(0);

}
/**********************************/
/*установки после чтения кода автомобиля*/
/****************************************/
void l_upldok_v_avt(class l_upldok_v_data *data)
{
char strsql[512];
class iceb_u_str naim("");
short metka_nst=0;
SQL_str row;
class SQLCURSOR cur;
int kod_vod=0;

if(iceb_poldan("Летняя норма расхода топлива",strsql,"uplnast.alx",data->window) == 0)
 if(iceb_u_SRAV(strsql,"Вкл",1) == 0)
  metka_nst=1;

sprintf(strsql,"select * from Uplavt where kod=%d",data->rk.kod_avt.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  naim.new_plus(row[1]);
  naim.plus(" ");
  naim.plus(row[5]);
  
  if(data->rk.nzt_na_gruz.getdlinna() <= 1)
   data->rk.nzt_na_gruz.new_plus(row[10]);

  if(data->rk.nzt_vr_dvig.getdlinna() <= 1)
   data->rk.nzt_vr_dvig.new_plus(row[11]);
   
  kod_vod=atoi(row[4]);
  
  if(metka_nst == 0) /*зимняя*/
   {
    data->rk.nst_po_gor.new_plus(row[3]);
    data->rk.nst_za_gor.new_plus(row[9]);
   }
  else /*летняя*/
   {
    data->rk.nst_po_gor.new_plus(row[2]);
    data->rk.nst_za_gor.new_plus(row[8]);
   }
  gtk_entry_set_text(GTK_ENTRY(data->entry[E_NST_ZA_GOR]),data->rk.nst_za_gor.ravno_toutf());
  gtk_entry_set_text(GTK_ENTRY(data->entry[E_NST_PO_GOR]),data->rk.nst_po_gor.ravno_toutf());

  if(kod_vod != 0)
   {
    data->rk.kod_vod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_KOD_VOD]))));
    if(data->rk.kod_vod.getdlinna() <= 1)
     {     
      data->rk.kod_vod.new_plus(kod_vod);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_VOD]),data->rk.kod_vod.ravno_toutf());
      class iceb_u_str naim_vod("");   
      sprintf(strsql,"select naik from Uplouot where kod=%d",kod_vod);
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {
        naim_vod.new_plus(row[0]);
       }
      gtk_label_set_text(GTK_LABEL(data->label_vod),naim_vod.ravno_toutf(20));
     }
   }

  int psp=poippl(&data->rk.data_dok,data->rk.kod_avt.ravno(),data->window);
  if(psp != 0)
   {
    data->rk.ps_viezd.new_plus(psp);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PS_VIEZD]),data->rk.ps_viezd.ravno_toutf());
   }  
 }

gtk_label_set_text(GTK_LABEL(data->label_avt),naim.ravno_toutf(20));

}
/********************************************/
/*установки после чтения кода подразделения*/
/*******************************************/ 
void l_upldok_v_pod(class l_upldok_v_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str naim("");

sprintf(strsql,"select naik from Uplpodr where kod=%d",data->rk.kod_pod.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 naim.new_plus(row[0]);

gtk_label_set_text(GTK_LABEL(data->label_pod),naim.ravno_toutf(20));

data->rk.data_dok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATA_DOK]))));
data->rk.nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_NOMDOK]))));

if(data->rk.nomdok.getdlinna() > 1 )
 return;
if(data->rk.data_dok.getdlinna() <= 1)
 return;
 
if(data->rk.kod_pod.getdlinna() <= 1)
 return;

uplnomdokw(data->rk.data_dok.ravno_god(),data->rk.kod_pod.ravno(),&data->rk.nomdok,data->window);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk.nomdok.ravno_toutf());


}
/******************************************************************/
/*Поиск предыдущего путевого листа и возврат показания спидометра*/
/******************************************************************/

int poippl(class iceb_u_str *data_dok,const char *kodavt,GtkWidget *wpredok)
{
int	poksp=0;
char	strsql[512];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select psz from Upldok where datd <= '%s' and ka=%s and psz != 0 order by datd desc,nz desc limit 1",
data_dok->ravno_sqldata(),kodavt);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) > 0)
 poksp=atoi(row[0]);

return(poksp);

}
