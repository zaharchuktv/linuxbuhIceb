/*$Id: vkasord2.c,v 1.19 2011-02-21 07:35:58 sasa Exp $*/
/*09.06.2010	24.01.2006	Белых А.И.	vkasord2.c
Ввод и корректировка шапки для расходного кассового ордера
*/
#include "buhg_g.h"
#include <unistd.h>

enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KASSA,
  E_DATAD,
  E_NOMDOK,
  E_KODOP,
  E_SHET,
  E_OSNOV,
  E_DOPOL,
  E_FIO,
  E_DOKUM,
  E_NOMER_BL,
  E_NOMVED,
  KOLENTER  
 };

class vkasord2_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *label_naim_shet;
  GtkWidget *label_naim_kodop;
  GtkWidget *label_naim_kassa;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  //Ключевые реквизиты корректируемого документа
  class iceb_u_str datad_k;
  class iceb_u_str nomdok_k;
  class iceb_u_str kassa_k;
  class iceb_u_str nomved_k;
  int tipz; //не корректируемый реквизит  
  
  //Реквизиты шапки документа
  class iceb_u_str kassa;
  class iceb_u_str datad;
  class iceb_u_str nomdok;
  class iceb_u_str kodop;
  class iceb_u_str shet;
  class iceb_u_str osnov;
  class iceb_u_str dopol;
  class iceb_u_str fio;
  class iceb_u_str dokum; //номер документа удостоверяющего личность получившего деньги
  class iceb_u_str nomer_bl;  
  class iceb_u_str nomer_blz;  /*Номер бланка запомненный до корректировки*/
  class iceb_u_str nomved; /*Номер ведомости на получение средств*/  
  vkasord2_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_data();
    nomer_blz.plus("");
    nomved_k.new_plus("");
   }

  void read_rek()
   {
    kassa.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KASSA]))));
    datad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAD]))));
    nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK]))));
    kodop.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KODOP]))));
    shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET]))));
    osnov.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_OSNOV]))));
    dopol.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DOPOL]))));
    fio.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_FIO]))));
    dokum.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DOKUM]))));
    nomer_bl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMER_BL]))));
    nomved.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMVED]))));
//    for(int i=0; i < KOLENTER; i++)
//      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }
  void clear_data()
   {
    kassa.new_plus("");
    datad.new_plus("");
    nomdok.new_plus("");
    kodop.new_plus("");
    shet.new_plus("");
    osnov.new_plus("");
    dopol.new_plus("");
    fio.new_plus("");
    dokum.new_plus("");
    nomer_bl.new_plus("");
    nomved.new_plus("");
   }
 };

gboolean   vkasord2_v_key_press(GtkWidget *widget,GdkEventKey *event,class vkasord2_data *data);
void    vkasord2_v_vvod(GtkWidget *widget,class vkasord2_data *data);
void  vkasord2_v_knopka(GtkWidget *widget,class vkasord2_data *data);
void vkasord2_kkvt(char *skod,char *nkod,GtkWidget *wpredok);
int vkasord2_pk(char *kod,GtkWidget *wpredok);

void  vkasord2_v_e_knopka(GtkWidget *widget,class vkasord2_data *data);
int vkasord2_zap(class vkasord2_data *data);
int vkasord2_ved(short dd,short md,short gd,const char *nomvs,const char *nomvn,const char *nomord,const char *koment,GtkWidget *wpredok);

extern char *name_system;
extern SQL_baza bd;
extern char	*shrt;    /*Счета розничной торговли*/
extern double	okrcn;  /*Округление цены*/

int vkasord2(iceb_u_str *datad, //дата документа
iceb_u_str *nomdok,
iceb_u_str *kassa,
GtkWidget *wpredok)
{
static class iceb_u_str datadok_zap("");
static class iceb_u_str kodkas_zap("");

class vkasord2_data data;
char strsql[512];
if(datadok_zap.getdlinna() <= 1)
 {
  short dt,mt,gt;
  iceb_u_poltekdat(&dt,&mt,&gt);
  sprintf(strsql,"%02d.%02d.%04d",dt,mt,gt);
  datadok_zap.plus(strsql);
 }

data.datad_k.plus(datad->ravno());
data.nomdok_k.plus(nomdok->ravno());
data.kassa_k.plus(kassa->ravno());
data.datad.plus(datad->ravno());
data.nomdok.plus(nomdok->ravno());
data.kassa.plus(kassa->ravno());
data.tipz=2; //меню для работы только с расходными документами
iceb_u_str kikz;
iceb_u_str naim_shet;
iceb_u_str naim_kodop;
iceb_u_str naim_kassa;
naim_shet.plus("");
naim_kodop.plus("");
naim_kassa.plus("");

if(data.nomdok_k.getdlinna() > 1) //корректировка шапки документа
 {
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select * from Kasord where nomd='%s' and kassa=%d and \
tp=%d and god=%d",data.nomdok_k.ravno(),data.kassa_k.ravno_atoi(),data.tipz,data.datad_k.ravno_god());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  data.shet.new_plus(row[4]);
  data.kodop.new_plus(row[5]);
  data.osnov.new_plus(row[8]);
  data.dopol.new_plus(row[9]);
  data.fio.new_plus(row[10]);
  data.dokum.new_plus(row[11]);
  data.nomer_bl.new_plus(row[15]);
  data.nomer_blz.new_plus(row[15]);
  data.nomved.new_plus(row[16]);
  data.nomved_k.new_plus(row[16]);
  kikz.plus(iceb_kikz(row[13],row[14],wpredok));

  //Узнаём наименование кассы
  sprintf(strsql,"select naik from Kas where kod=%d",data.kassa.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim_kassa.new_plus(row[0]);
  
  //Узнаём наименование счёта
  if(data.shet.getdlinna() > 1)
   {
    sprintf(strsql,"select nais from Plansh where ns='%s'",data.shet.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim_shet.new_plus(row[0]);
   }

  //Узнаём наименование операции
  if(data.kodop.getdlinna() > 1)
   {
    sprintf(strsql,"select naik from Kasop2 where kod='%s'",data.kodop.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim_kodop.new_plus(row[0]);
   }
  
 }
else
 {
  data.kassa.plus(kodkas_zap.ravno());
  data.datad.plus(datadok_zap.ravno());
  if(data.kassa.getdlinna() > 1 && data.datad.getdlinna() > 1)
     iceb_nomnak(data.datad.ravno_god(),data.kassa.ravno(),&data.nomdok,data.tipz,2,1,wpredok);

 }



data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.nomdok.getdlinna() > 1)
 sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи."));
else
 sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи."));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(vkasord2_v_key_press),&data);

iceb_u_str zagolov;
if(data.nomdok.getdlinna() <= 1)
 zagolov.new_plus(gettext("Ввод нового ордера на расход."));
else
 {
  zagolov.new_plus(gettext("Корректировка шапки расходного документа."));
  zagolov.ps_plus(kikz.ravno());
 }

GtkWidget *label=gtk_label_new(zagolov.ravno_toutf());

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

GtkWidget *separator1=gtk_hseparator_new();
gtk_container_add (GTK_CONTAINER (vbox), separator1);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];


sprintf(strsql,"%s",gettext("Код кассы"));
data.knopka_enter[E_KASSA]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KASSA]), data.knopka_enter[E_KASSA], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KASSA]),"clicked",GTK_SIGNAL_FUNC(vkasord2_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KASSA]),(gpointer)E_KASSA);
tooltips_enter[E_KASSA]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KASSA],data.knopka_enter[E_KASSA],gettext("Выбор кассы."),NULL);

data.entry[E_KASSA] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_KASSA]), data.entry[E_KASSA],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KASSA]), "activate",GTK_SIGNAL_FUNC(vkasord2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KASSA]),data.kassa.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KASSA]),(gpointer)E_KASSA);

data.label_naim_kassa=gtk_label_new(naim_kassa.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KASSA]), data.label_naim_kassa,TRUE, TRUE, 0);


sprintf(strsql,"%s",gettext("Дата"));
data.knopka_enter[E_DATAD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.knopka_enter[E_DATAD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAD]),"clicked",GTK_SIGNAL_FUNC(vkasord2_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAD]),(gpointer)E_DATAD);
tooltips_enter[E_DATAD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAD],data.knopka_enter[E_DATAD],gettext("Выбор даты."),NULL);

data.entry[E_DATAD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.entry[E_DATAD],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAD]), "activate",GTK_SIGNAL_FUNC(vkasord2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAD]),data.datad.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAD]),(gpointer)E_DATAD);

sprintf(strsql,"%s",gettext("Номер кассового ордера"));
data.knopka_enter[E_NOMDOK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.knopka_enter[E_NOMDOK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_NOMDOK]),"clicked",GTK_SIGNAL_FUNC(vkasord2_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_NOMDOK]),(gpointer)E_NOMDOK);
tooltips_enter[E_NOMDOK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_NOMDOK],data.knopka_enter[E_NOMDOK],gettext("Получение номера документа."),NULL);

data.entry[E_NOMDOK] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMDOK]), "activate",GTK_SIGNAL_FUNC(vkasord2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.nomdok.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMDOK]),(gpointer)E_NOMDOK);

sprintf(strsql,"%s",gettext("Счёт кассы"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(vkasord2_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов."),NULL);

data.entry[E_SHET] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(vkasord2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

data.label_naim_shet=gtk_label_new(naim_shet.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.label_naim_shet,TRUE, TRUE, 0);


sprintf(strsql,"%s",gettext("Код операции"));
data.knopka_enter[E_KODOP]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.knopka_enter[E_KODOP], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODOP]),"clicked",GTK_SIGNAL_FUNC(vkasord2_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODOP]),(gpointer)E_KODOP);
tooltips_enter[E_KODOP]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODOP],data.knopka_enter[E_KODOP],gettext("Выбор операции"),NULL);

data.entry[E_KODOP] = gtk_entry_new_with_max_length (9);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODOP]), "activate",GTK_SIGNAL_FUNC(vkasord2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.kodop.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODOP]),(gpointer)E_KODOP);

data.label_naim_kodop=gtk_label_new(naim_kodop.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.label_naim_kodop,TRUE, TRUE, 0);


sprintf(strsql,"%s",gettext("Основание"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_OSNOV] = gtk_entry_new_with_max_length (249);
gtk_box_pack_start (GTK_BOX (hbox[E_OSNOV]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_OSNOV]), data.entry[E_OSNOV],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_OSNOV]), "activate",GTK_SIGNAL_FUNC(vkasord2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_OSNOV]),data.osnov.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_OSNOV]),(gpointer)E_OSNOV);

sprintf(strsql,"%s",gettext("Дополнение"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_DOPOL] = gtk_entry_new_with_max_length (79);
gtk_box_pack_start (GTK_BOX (hbox[E_DOPOL]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DOPOL]), data.entry[E_DOPOL],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DOPOL]), "activate",GTK_SIGNAL_FUNC(vkasord2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DOPOL]),data.dopol.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DOPOL]),(gpointer)E_DOPOL);

sprintf(strsql,"%s",gettext("Фамилия"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_FIO] = gtk_entry_new_with_max_length (59);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_FIO]), "activate",GTK_SIGNAL_FUNC(vkasord2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.fio.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_FIO]),(gpointer)E_FIO);

sprintf(strsql,"%s",gettext("Документ"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_DOKUM] = gtk_entry_new_with_max_length (59);
gtk_box_pack_start (GTK_BOX (hbox[E_DOKUM]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DOKUM]), data.entry[E_DOKUM],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DOKUM]), "activate",GTK_SIGNAL_FUNC(vkasord2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DOKUM]),data.dokum.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DOKUM]),(gpointer)E_DOKUM);

sprintf(strsql,"%s",gettext("Номер бланка"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_NOMER_BL] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_BL]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_BL]), data.entry[E_NOMER_BL],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMER_BL]), "activate",GTK_SIGNAL_FUNC(vkasord2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMER_BL]),data.nomer_bl.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMER_BL]),(gpointer)E_NOMER_BL);


sprintf(strsql,"%s",gettext("Номер ведомости"));
data.knopka_enter[E_NOMVED]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMVED]), data.knopka_enter[E_NOMVED], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_NOMVED]),"clicked",GTK_SIGNAL_FUNC(vkasord2_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_NOMVED]),(gpointer)E_NOMVED);
tooltips_enter[E_NOMVED]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_NOMVED],data.knopka_enter[E_NOMVED],gettext("Получение номера ведомости"),NULL);

data.entry[E_NOMVED] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMVED]), data.entry[E_NOMVED],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMVED]), "activate",GTK_SIGNAL_FUNC(vkasord2_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMVED]),data.nomved.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMVED]),(gpointer)E_NOMVED);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введённой в меню информации."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(vkasord2_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(vkasord2_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 {
  datad->new_plus(data.datad.ravno());
  nomdok->new_plus(data.nomdok.ravno());
  kassa->new_plus(data.kassa.ravno());

  datadok_zap.new_plus(data.datad.ravno());
  kodkas_zap.new_plus(data.kassa.ravno());
 }
return(data.voz);
}

/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  vkasord2_v_e_knopka(GtkWidget *widget,class vkasord2_data *data)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
short god=0;
switch (knop)
 {
  case E_NOMDOK:

     //Читаем кассу
     data->kassa.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_KASSA]))));
     //Читаем дату
     data->datad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATAD]))));
     
     iceb_nomnak(data->datad.ravno_god(),data->kassa.ravno(),&data->nomdok,data->tipz,2,1,data->window);
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->nomdok.ravno_toutf());
      
    return;  

  case E_DATAD:

    if(iceb_calendar(&data->datad,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAD]),data->datad.ravno());

    if(data->datad.getdlinna() > 1 && data->kassa.getdlinna() > 1 && data->nomdok.getdlinna() <= 1)
     {
      iceb_nomnak(data->datad.ravno_god(),data->kassa.ravno(),&data->nomdok,data->tipz,2,1,data->window);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->nomdok.ravno_toutf());
     }      
    return;  

  case E_SHET:
    if(iceb_vibrek(1,"Plansh",&data->shet,&naim,data->window) == 0)
       gtk_label_set_text(GTK_LABEL(data->label_naim_shet),naim.ravno_toutf(20));
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet.ravno_toutf());
    return;

  case E_KASSA:
    if(l_spis_kas(1,&kod,&naim,data->window) == 0)
     {
      gtk_label_set_text(GTK_LABEL(data->label_naim_kassa),naim.ravno_toutf(20));
      data->kassa.new_plus(kod.ravno());
     }    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KASSA]),data->kassa.ravno_toutf());
    return;

  case E_KODOP:

     if(l_kasopr(1,&kod,&naim,data->window) == 0)
      {
       gtk_label_set_text(GTK_LABEL(data->label_naim_kodop),naim.ravno_toutf(20));
       data->kodop.new_plus(kod.ravno());
       sprintf(strsql,"select shetkor from Kasop2 where kod='%s'",kod.ravno());
       if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
        {
         gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),iceb_u_toutf(row[0]));

         sprintf(strsql,"select nais from Plansh where ns='%s'",row[0]);
         if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
           {
            sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
            gtk_label_set_text(GTK_LABEL(data->label_naim_shet),iceb_u_toutf(strsql));
           }    
        }
      }


    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->kodop.ravno_toutf());
    return;

  case E_NOMVED:
     data->datad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATAD]))));
     god=data->datad.ravno_god();
     if(god == 0)
       god=iceb_u_poltekgod();

    data->nomved.new_plus(l_kasvedz_v_maxnd(god,data->window));
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMVED]),data->nomved.ravno_toutf());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vkasord2_v_key_press(GtkWidget *widget,GdkEventKey *event,class vkasord2_data *data)
{

//printf("vkasord2_v_key_press\n");
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
void  vkasord2_v_knopka(GtkWidget *widget,class vkasord2_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(vkasord2_zap(data) != 0)
      return;

    gtk_widget_destroy(data->window);
    data->window=NULL;

    data->voz=0;
    return;  

    
  case FK10:
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    vkasord2_v_vvod(GtkWidget *widget,class vkasord2_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("vkasord2_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_FIO:
    data->fio.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOMDOK:
    data->nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DOKUM:
    data->dokum.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOMER_BL:
    data->nomer_bl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_OSNOV:
    data->osnov.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DOPOL:
    data->dopol.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAD:
    data->datad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(data->datad.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Не правильно введена дата!"),data->window);
      break;
     }    
    /*Читаем номер документа*/ 
    data->nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_NOMDOK]))));
    if(data->nomdok.getdlinna() <= 1)
     {
      /*Читаем кассу*/
      data->kassa.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_KASSA]))));

      if(data->kassa.getdlinna() > 1 && data->datad.getdlinna() > 1)
       {
        iceb_nomnak(data->datad.ravno_god(),data->kassa.ravno(),&data->nomdok,data->tipz,2,1,data->window);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->nomdok.ravno_toutf());
       }
     }
    break;

  case E_SHET:

    data->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    sprintf(strsql,"select nais from Plansh where ns='%s'",data->shet.ravno());

    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naim_shet),iceb_u_toutf(strsql));
     }    
    break;

  case E_KASSA:

    data->kassa.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    sprintf(strsql,"select naik from Kasop2 where kod='%s'",data->kassa.ravno());


    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naim_kassa),iceb_u_toutf(strsql));
     }    
    break;

  case E_KODOP:

    data->kodop.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    sprintf(strsql,"select naik,shetkor from Kasop2 where kod='%s'",data->kodop.ravno());

    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naim_kodop),iceb_u_toutf(strsql));
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),iceb_u_toutf(row[1]));

      sprintf(strsql,"select nais from Plansh where ns='%s'",row[1]);

      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {
        sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
        gtk_label_set_text(GTK_LABEL(data->label_naim_shet),iceb_u_toutf(strsql));
       }    

     }    
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/**************************/
/*Запись шапкм кассового ордера*/
/*******************************/
int vkasord2_zap(class vkasord2_data *data)
{
data->read_rek(); //читаем все реквизиты из меню

if(data->datad.getdlinna() <= 1 || data->kassa.getdlinna() <= 1 || \
data->nomdok.getdlinna() <= 1 || data->kodop.getdlinna() <= 1 || 
data->shet.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введены все обязательные реквизиты !"),data->window);
  return(1);
 }

short d,m,g;

if(iceb_u_rsdat(&d,&m,&g,data->datad.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата документа !"),data->window);
  return(1);
 }

if(pbkasw(m,g,data->window) != 0)
 return(1);

char strsql[512];

//Проверяем код кассы
sprintf(strsql,"select kod from Kas where kod=%d",data->kassa.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код кассы"),data->kassa.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

//проверяем код операции
sprintf(strsql,"select kod from Kasop2 where kod='%s'",data->kodop.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код операции"),data->kodop.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
//проверяем счёт
class OPSHET opsh;

if(data->shet.getdlinna() > 1)
 if(iceb_prsh1(data->shet.ravno(),&opsh,data->window) != 0)
  return(1);

SQL_str row;
class SQLCURSOR cur;
/*Проверяем номер бланка*/
if(data->nomer_bl.getdlinna() > 1)
 if(iceb_u_SRAV(data->nomer_bl.ravno(),data->nomer_blz.ravno(),0) != 0)
  {
   sprintf(strsql,"select nomd from Kasord where nb='%s'",data->nomer_bl.ravno());
   if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
    {
     sprintf(strsql,gettext("Номер бланка %s уже есть у документа %s !"),data->nomer_bl.ravno(),row[0]);
     iceb_menu_soob(strsql,data->window);
     return(1);     
    } 

  }

int kodv=0;
int mkor=0;
if(data->nomdok_k.getdlinna() > 1)
  mkor=1;

if(vkasord2_ved(d,m,g,data->nomved_k.ravno(),data->nomved.ravno(),data->nomdok.ravno(),data->osnov.ravno_filtr(),data->window) != 0)
 return(1);
 
short dz,mz,gz;
iceb_u_rsdat(&dz,&mz,&gz,data->datad_k.ravno(),1);
  
if((kodv=zaphkorw(mkor,
data->kassa.ravno(),
data->tipz,
d,m,g,
data->nomdok.ravno_filtr(),
data->shet.ravno_filtr(),
data->kodop.ravno_filtr(),
data->osnov.ravno_filtr(),
data->dopol.ravno_filtr(),
data->fio.ravno_filtr(),
data->dokum.ravno_filtr(),
data->kassa_k.ravno(),data->nomdok_k.ravno(),dz,mz,gz,data->nomer_bl.ravno(),data->nomved.ravno_filtr(),data->window)) != 0)
 {
  if(kodv == 1)
   {
    sprintf(strsql,gettext("С номером %s документ уже есть !"),data->nomdok.ravno());
    iceb_menu_soob(strsql,data->window);
   }
  return(1);
 }
return(0);

}
/*******************************/
/*Ведомость*/
/***************************/
int vkasord2_ved(short dd,short md,short gd,const char *nomvs,const char *nomvn,const char *nomord,const char *koment,GtkWidget *wpredok)
{
char strsql[512];
if(iceb_u_SRAV(nomvs,nomvn,0) != 0)
 {
  if(nomvn[0] != '\0')
   {
    sprintf(strsql,"select nomv from Kasnomved where god=%d and nomv='%s'",gd,nomvn);
    if(iceb_sql_readkey(strsql,wpredok) > 0)
     {
        sprintf(strsql,gettext("Номер ведомости %s уже есть!"),nomvn);
        iceb_menu_soob(strsql,wpredok);
        return(1);
     }
   }


 }
if(nomvs[0] != '\0')
 {
   /*Удаляем старую запись*/
   sprintf(strsql,"delete from Kasnomved where god=%d and nomv='%s'",gd,nomvs);
   iceb_sql_zapis(strsql,1,0,wpredok);
 }
if(nomvn[0] != '\0')
 {
  sprintf(strsql,"insert into Kasnomved values('%04d-%02d-%02d',%d,'%s',0.,'%s',%d,%ld,'%s')",
  gd,md,dd,gd,nomvn,koment,iceb_getuid(wpredok),time(NULL),nomord);

  return(iceb_sql_zapis(strsql,1,0,wpredok));
 }

return(0);
}

