/*$Id: vdnuw.c,v 1.30 2011-08-29 07:13:44 sasa Exp $*/
/*16.10.2010	22.08.2005	Белых А.И.	vdnuw.c
Работа с шапкой документа на услуги
*/
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "buhg_g.h"
#include "vdnuw.h"

enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KONTR,
  E_DATAD,
  E_PODR,
  E_NOMDOK,
  E_KODOP,
  E_USL_PROD,
  E_FOR_OPL,
  E_DATA_OPL,
  E_OSNOV,
  E_NOMDOK_POS,
  E_DATA_VNP,
  E_NOM_NAL_NAK,
  E_DATA_POL_NAL_NAK,
  E_DOVER,
  E_DATA_DOVER,
  E_SHEREZ,  
  KOLENTER  
 };

class vdnuw_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_naim_kontr;
  GtkWidget *label_naim_podr;
  GtkWidget *label_naim_kodop;
  GtkWidget *label_naim_for_opl;
  short kl_shift;
  short voz;      //0-ввели 1 нет
  GtkWidget *radiobutton[4];

  class vdnuw_rek rk;
  
  iceb_u_str kodk; //Код записи которую корректируют
  
  vdnuw_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR]))));
    rk.datad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAD]))));
    rk.podrv.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PODR]))));
    rk.nomdokv.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK]))));
    rk.kodop.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KODOP]))));
    rk.usl_prod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_USL_PROD]))));
    rk.for_opl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_FOR_OPL]))));
    rk.nom_nal_nak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOM_NAL_NAK]))));
    rk.data_opl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_OPL]))));
    rk.osnov.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_OSNOV]))));
    rk.nomdok_pos.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK_POS]))));
    rk.data_vnp.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_VNP])))); 
    rk.data_pol_nal_nak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_POL_NAL_NAK]))));    
    rk.dover.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DOVER])))); 
    rk.data_dover.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_DOVER]))));   
    rk.sherez.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHEREZ]))));
   }
 };

gboolean   vdnuw_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class vdnuw_v_data *data);
void    vdnuw_v_v_vvod(GtkWidget *widget,class vdnuw_v_data *data);
void  vdnuw_v_v_knopka(GtkWidget *widget,class vdnuw_v_data *data);
int vdnuw_zap(class vdnuw_v_data *data);

void  vdnuw_radio0(GtkWidget *widget,class vdnuw_v_data *data);
void  vdnuw_radio1(GtkWidget *widget,class vdnuw_v_data *data);
void  vdnuw_radio2(GtkWidget *widget,class vdnuw_v_data *data);
void  vdnuw_radio3(GtkWidget *widget,class vdnuw_v_data *data);



void  vdnuw_v_e_knopka(GtkWidget *widget,class vdnuw_v_data *data);

extern char *name_system;
extern SQL_baza bd;
extern char	*shrt;    /*Счета розничной торговли*/
extern double	okrcn;  /*Округление цены*/
extern char *imabaz;

int vdnuw(short *dd,short *md,short *gd,
int tipz,
iceb_u_str *nomdok,
int *podr,
GtkWidget *wpredok)
{
vdnuw_v_data data;
char strsql[512];
iceb_u_str kikz;
SQL_str row;
SQLCURSOR cur;
iceb_u_str naim_kontr;
naim_kontr.plus("");
iceb_u_str naim_kodop;
naim_kodop.plus("");
iceb_u_str naim_for_opl;
naim_for_opl.plus("");
iceb_u_str naim_podr;
naim_podr.plus("");

float pnds=0.;
if(*dd != 0)
 pnds=iceb_pnds(*dd,*md,*gd,wpredok);
else
 {
  pnds=iceb_pnds(wpredok);
 } 
data.rk.ds=*dd;
data.rk.ms=*md;
data.rk.gs=*gd;
data.rk.tipz=tipz;
data.rk.podrs=*podr;
if(*podr != 0)
 data.rk.podrv.plus(*podr);
data.rk.nomdoks.new_plus(nomdok->ravno());
data.rk.nomdokv.new_plus(nomdok->ravno());
data.rk.datad.new_plus_data(*dd,*md,*gd);
if(*dd != 0)
 {
  sprintf(strsql,"select kontr,nomnn,kodop,nds,forop,datop,uslpr,nomdp,\
dover,datdov,sherez,ktoi,vrem,osnov,datpnn,datdp from Usldokum where datd='%04d-%02d-%02d' \
and podr=%d and nomd='%s' and tp=%d",
  data.rk.gs,data.rk.ms,data.rk.ds,data.rk.podrs,data.rk.nomdoks.ravno(),data.rk.tipz);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  data.rk.kontr.new_plus(row[0]);
  data.rk.kontrz.new_plus(row[0]);
  data.rk.nom_nal_nak.new_plus(row[1]);
  data.rk.nom_nal_nakz.new_plus(row[1]);
  data.rk.kodop.new_plus(row[2]);
  data.rk.lnds=atoi(row[3]);
  data.rk.for_opl.new_plus(row[4]);
  data.rk.data_opl.new_plus_sqldata(row[5]);
  data.rk.usl_prod.new_plus(row[6]);
  data.rk.nomdok_pos.new_plus(row[7]);
  data.rk.dover.new_plus(row[8]);
  data.rk.data_dover.new_plus_sqldata(row[9]);
  data.rk.sherez.new_plus(row[10]);
  data.rk.data_vnp.new_plus_sqldata(row[15]);  

  kikz.plus(iceb_kikz(row[11],row[12],wpredok));

  data.rk.osnov.new_plus(row[13]);
  data.rk.data_pol_nal_nak.new_plus_sqldata(row[14]);

  //Узнаём наименование контрагента
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data.rk.kontr.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_kontr.new_plus(row[0]);

  //Узнаём наименование операции
  if(tipz == 1)
    sprintf(strsql,"select naik from Usloper1 where kod='%s'",data.rk.kodop.ravno());
  if(tipz == 2)
    sprintf(strsql,"select naik from Usloper2 where kod='%s'",data.rk.kodop.ravno());

  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_kodop.new_plus(row[0]);

  //Узнаём наименование подразделения
  sprintf(strsql,"select naik from Uslpodr where kod=%d",data.rk.podrs);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_podr.new_plus(row[0]);

  //Узнаём наименование формы оплаты
  if(data.rk.for_opl.getdlinna() > 1)
   {
    sprintf(strsql,"select naik from Foroplat where kod='%s'",data.rk.for_opl.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim_for_opl.new_plus(row[0]);
   }    
 }
else
 {
  short dt,mt,gt;
  iceb_u_poltekdat(&dt,&mt,&gt);
  sprintf(strsql,"%d.%d.%d",dt,mt,gt);
  data.rk.datad.new_plus(strsql);
 }
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(*dd != 0)
 sprintf(strsql,"%s %s",name_system,gettext("Просмотр шапки документа"));
else
 sprintf(strsql,"%s %s",name_system,gettext("Ввод нового документа"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(vdnuw_v_v_key_press),&data);

iceb_u_str zagolov;
if(*dd == 0)
 zagolov.new_plus(gettext("Ввод нового документа"));
else
 {
  zagolov.new_plus(gettext("Просмотр шапки документа"));
  zagolov.ps_plus(kikz.ravno());
 }
if(tipz == 1)
   zagolov.ps_plus(gettext("Приход"));
if(tipz == 2)
   zagolov.ps_plus(gettext("Расход"));

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

//Вставляем радиокнопки
GSList *group;
sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));

data.radiobutton[0]=gtk_radio_button_new_with_label(NULL,iceb_u_toutf(strsql));
if(data.rk.lnds == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[0]),TRUE); //Устанавливем активной кнопку
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton[0]), "clicked",GTK_SIGNAL_FUNC(vdnuw_radio0),&data);
//gtk_object_set_user_data(GTK_OBJECT(data.radiobutton0),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton[0]));

data.radiobutton[1]=gtk_radio_button_new_with_label(group,gettext("0%% НДС реализация на територии Украины"));
if(data.rk.lnds == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[1]),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data.radiobutton[1]), "clicked",GTK_SIGNAL_FUNC(vdnuw_radio1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton[1], TRUE, TRUE, 0);

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton[0]));

data.radiobutton[2]=gtk_radio_button_new_with_label(group,gettext("0%% НДС экспорт"));
if(data.rk.lnds == 2)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[2]),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data.radiobutton[2]), "clicked",GTK_SIGNAL_FUNC(vdnuw_radio2),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton[2], TRUE, TRUE, 0);


group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton[0]));

data.radiobutton[3]=gtk_radio_button_new_with_label(group,gettext("Освобождено от НДС"));
if(data.rk.lnds == 3)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[3]),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data.radiobutton[3]), "clicked",GTK_SIGNAL_FUNC(vdnuw_radio3),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton[3], TRUE, TRUE, 0);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KONTR]),"clicked",GTK_SIGNAL_FUNC(vdnuw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KONTR]),(gpointer)E_KONTR);
tooltips_enter[E_KONTR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KONTR],data.knopka_enter[E_KONTR],gettext("Выбор контрагента"),NULL);

data.entry[E_KONTR] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KONTR]), "activate",GTK_SIGNAL_FUNC(vdnuw_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rk.kontr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KONTR]),(gpointer)E_KONTR);

data.label_naim_kontr=gtk_label_new(naim_kontr.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.label_naim_kontr,TRUE, TRUE, 0);


sprintf(strsql,"%s",gettext("Дата"));
data.knopka_enter[E_DATAD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.knopka_enter[E_DATAD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAD]),"clicked",GTK_SIGNAL_FUNC(vdnuw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAD]),(gpointer)E_DATAD);
tooltips_enter[E_DATAD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAD],data.knopka_enter[E_DATAD],gettext("Выбор даты"),NULL);

data.entry[E_DATAD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.entry[E_DATAD],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAD]), "activate",GTK_SIGNAL_FUNC(vdnuw_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAD]),data.rk.datad.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAD]),(gpointer)E_DATAD);


sprintf(strsql,"%s",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODR]),"clicked",GTK_SIGNAL_FUNC(vdnuw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODR]),(gpointer)E_PODR);
tooltips_enter[E_PODR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_PODR],data.knopka_enter[E_PODR],gettext("Выбор подрозделения"),NULL);

data.entry[E_PODR] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODR]), "activate",GTK_SIGNAL_FUNC(vdnuw_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk.podrv.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODR]),(gpointer)E_PODR);

data.label_naim_podr=gtk_label_new(naim_podr.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.label_naim_podr,TRUE, TRUE, 0);


sprintf(strsql,"%s",gettext("Номер документа"));
data.knopka_enter[E_NOMDOK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.knopka_enter[E_NOMDOK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_NOMDOK]),"clicked",GTK_SIGNAL_FUNC(vdnuw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_NOMDOK]),(gpointer)E_NOMDOK);
tooltips_enter[E_NOMDOK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_NOMDOK],data.knopka_enter[E_NOMDOK],gettext("Получить номера документа"),NULL);

data.entry[E_NOMDOK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMDOK]), "activate",GTK_SIGNAL_FUNC(vdnuw_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk.nomdoks.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMDOK]),(gpointer)E_NOMDOK);


sprintf(strsql,"%s",gettext("Код операции"));
data.knopka_enter[E_KODOP]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.knopka_enter[E_KODOP], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODOP]),"clicked",GTK_SIGNAL_FUNC(vdnuw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODOP]),(gpointer)E_KODOP);
tooltips_enter[E_KODOP]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODOP],data.knopka_enter[E_KODOP],gettext("Выбор операции"),NULL);

data.entry[E_KODOP] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODOP]), "activate",GTK_SIGNAL_FUNC(vdnuw_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.rk.kodop.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODOP]),(gpointer)E_KODOP);

data.label_naim_kodop=gtk_label_new(naim_kodop.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.label_naim_kodop,TRUE, TRUE, 0);


sprintf(strsql,"%s",gettext("Вид договора"));
data.knopka_enter[E_USL_PROD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_USL_PROD]), data.knopka_enter[E_USL_PROD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_USL_PROD]),"clicked",GTK_SIGNAL_FUNC(vdnuw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_USL_PROD]),(gpointer)E_USL_PROD);
tooltips_enter[E_USL_PROD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_USL_PROD],data.knopka_enter[E_USL_PROD],gettext("Выбор договора из списка договоров"),NULL);

data.entry[E_USL_PROD] = gtk_entry_new_with_max_length (79);
gtk_box_pack_start (GTK_BOX (hbox[E_USL_PROD]), data.entry[E_USL_PROD],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_USL_PROD]), "activate",GTK_SIGNAL_FUNC(vdnuw_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_USL_PROD]),data.rk.usl_prod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_USL_PROD]),(gpointer)E_USL_PROD);


sprintf(strsql,"%s",gettext("Форма оплаты"));
data.knopka_enter[E_FOR_OPL]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_FOR_OPL]), data.knopka_enter[E_FOR_OPL], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_FOR_OPL]),"clicked",GTK_SIGNAL_FUNC(vdnuw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_FOR_OPL]),(gpointer)E_FOR_OPL);
tooltips_enter[E_FOR_OPL]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_FOR_OPL],data.knopka_enter[E_FOR_OPL],gettext("Выбор формы оплаты"),NULL);

data.entry[E_FOR_OPL] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_FOR_OPL]), data.entry[E_FOR_OPL],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_FOR_OPL]), "activate",GTK_SIGNAL_FUNC(vdnuw_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FOR_OPL]),data.rk.for_opl.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_FOR_OPL]),(gpointer)E_FOR_OPL);

data.label_naim_for_opl=gtk_label_new(naim_for_opl.ravno_toutf(20));
gtk_box_pack_start (GTK_BOX (hbox[E_FOR_OPL]), data.label_naim_for_opl,TRUE, TRUE, 0);


sprintf(strsql,"%s",gettext("Дата оплаты"));
data.knopka_enter[E_DATA_OPL]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_OPL]), data.knopka_enter[E_DATA_OPL], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_OPL]),"clicked",GTK_SIGNAL_FUNC(vdnuw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_OPL]),(gpointer)E_DATA_OPL);
tooltips_enter[E_DATA_OPL]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_OPL],data.knopka_enter[E_DATA_OPL],gettext("Выбор даты"),NULL);

data.entry[E_DATA_OPL] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_OPL]), data.entry[E_DATA_OPL],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_OPL]), "activate",GTK_SIGNAL_FUNC(vdnuw_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_OPL]),data.rk.data_opl.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_OPL]),(gpointer)E_DATA_OPL);

sprintf(strsql,"%s",gettext("Основание"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_OSNOV] = gtk_entry_new_with_max_length (79);
gtk_box_pack_start (GTK_BOX (hbox[E_OSNOV]), label,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_OSNOV]), data.entry[E_OSNOV],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_OSNOV]), "activate",GTK_SIGNAL_FUNC(vdnuw_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_OSNOV]),data.rk.osnov.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_OSNOV]),(gpointer)E_OSNOV);

sprintf(strsql,"%s",gettext("Номер накладной поставщика"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK_POS]), label,FALSE, FALSE, 0);
data.entry[E_NOMDOK_POS] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK_POS]), data.entry[E_NOMDOK_POS],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMDOK_POS]), "activate",GTK_SIGNAL_FUNC(vdnuw_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK_POS]),data.rk.nomdok_pos.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMDOK_POS]),(gpointer)E_NOMDOK_POS);

sprintf(strsql,"%s",gettext("Дата виписки накладной поставщика"));
data.knopka_enter[E_DATA_VNP]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VNP]), data.knopka_enter[E_DATA_VNP], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_VNP]),"clicked",GTK_SIGNAL_FUNC(vdnuw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_VNP]),(gpointer)E_DATA_VNP);
tooltips_enter[E_DATA_VNP]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_VNP],data.knopka_enter[E_DATA_VNP],gettext("Выбор даты"),NULL);

data.entry[E_DATA_VNP] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VNP]), data.entry[E_DATA_VNP],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_VNP]), "activate",GTK_SIGNAL_FUNC(vdnuw_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_VNP]),data.rk.data_vnp.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_VNP]),(gpointer)E_DATA_VNP);

if(tipz == 1)
 {
  sprintf(strsql,"%s",gettext("Номер налоговой накладной"));
  label=gtk_label_new(iceb_u_toutf(strsql));
  gtk_box_pack_start (GTK_BOX (hbox[E_NOM_NAL_NAK]), label,FALSE, FALSE, 0);
 }

if(tipz == 2)
 {
  sprintf(strsql,"%s",gettext("Номер налоговой накладной"));
  data.knopka_enter[E_NOM_NAL_NAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
  gtk_box_pack_start (GTK_BOX (hbox[E_NOM_NAL_NAK]), data.knopka_enter[E_NOM_NAL_NAK], FALSE, FALSE, 0);
  gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_NOM_NAL_NAK]),"clicked",GTK_SIGNAL_FUNC(vdnuw_v_e_knopka),&data);
  gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_NOM_NAL_NAK]),(gpointer)E_NOM_NAL_NAK);
  tooltips_enter[E_NOM_NAL_NAK]=gtk_tooltips_new();
  gtk_tooltips_set_tip(tooltips_enter[E_NOM_NAL_NAK],data.knopka_enter[E_NOM_NAL_NAK],gettext("Получить номера документа"),NULL);
 }

data.entry[E_NOM_NAL_NAK] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOM_NAL_NAK]), data.entry[E_NOM_NAL_NAK],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOM_NAL_NAK]), "activate",GTK_SIGNAL_FUNC(vdnuw_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOM_NAL_NAK]),data.rk.nom_nal_nak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOM_NAL_NAK]),(gpointer)E_NOM_NAL_NAK);

if(tipz == 1)
  sprintf(strsql,"%s",gettext("Дата получения налоговой накладной"));
if(tipz == 2)
  sprintf(strsql,"%s",gettext("Дата выдачи налоговой накладной"));
data.knopka_enter[E_DATA_POL_NAL_NAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_POL_NAL_NAK]), data.knopka_enter[E_DATA_POL_NAL_NAK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_POL_NAL_NAK]),"clicked",GTK_SIGNAL_FUNC(vdnuw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_POL_NAL_NAK]),(gpointer)E_DATA_POL_NAL_NAK);
tooltips_enter[E_DATA_POL_NAL_NAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_POL_NAL_NAK],data.knopka_enter[E_DATA_POL_NAL_NAK],gettext("Выбор даты"),NULL);

data.entry[E_DATA_POL_NAL_NAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_POL_NAL_NAK]), data.entry[E_DATA_POL_NAL_NAK],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_POL_NAL_NAK]), "activate",GTK_SIGNAL_FUNC(vdnuw_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_POL_NAL_NAK]),data.rk.data_pol_nal_nak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_POL_NAL_NAK]),(gpointer)E_DATA_POL_NAL_NAK);

sprintf(strsql,"%s",gettext("Доверенность"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_DOVER] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_DOVER]), label,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DOVER]), data.entry[E_DOVER],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DOVER]), "activate",GTK_SIGNAL_FUNC(vdnuw_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DOVER]),data.rk.dover.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DOVER]),(gpointer)E_DOVER);


sprintf(strsql,"%s",gettext("Дата выдачи доверенности"));
data.knopka_enter[E_DATA_DOVER]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_DOVER]), data.knopka_enter[E_DATA_DOVER], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_DOVER]),"clicked",GTK_SIGNAL_FUNC(vdnuw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_DOVER]),(gpointer)E_DATA_DOVER);
tooltips_enter[E_DATA_DOVER]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_DOVER],data.knopka_enter[E_DATA_DOVER],gettext("Выбор даты"),NULL);

data.entry[E_DATA_DOVER] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_DOVER]), data.entry[E_DATA_DOVER],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_DOVER]), "activate",GTK_SIGNAL_FUNC(vdnuw_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_DOVER]),data.rk.data_dover.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_DOVER]),(gpointer)E_DATA_DOVER);

sprintf(strsql,"%s",gettext("Через кого"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_SHEREZ] = gtk_entry_new_with_max_length (39);
gtk_box_pack_start (GTK_BOX (hbox[E_SHEREZ]), label,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SHEREZ]), data.entry[E_SHEREZ],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHEREZ]), "activate",GTK_SIGNAL_FUNC(vdnuw_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHEREZ]),data.rk.sherez.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHEREZ]),(gpointer)E_SHEREZ);




GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введённой в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(vdnuw_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(vdnuw_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 {
  iceb_u_rsdat(dd,md,gd,data.rk.datad.ravno(),1);
  nomdok->new_plus(data.rk.nomdokv.ravno());
  *podr=data.rk.podrv.ravno_atoi();
 }
return(data.voz);
}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  vdnuw_radio0(GtkWidget *widget,class vdnuw_v_data *data)
{
//g_print("vdnuw_radio0\n");
data->rk.lnds=0;

}
void  vdnuw_radio1(GtkWidget *widget,class vdnuw_v_data *data)
{
//g_print("vdnuw_radio1\n");
data->rk.lnds=1;
}
void  vdnuw_radio2(GtkWidget *widget,class vdnuw_v_data *data)
{
//g_print("vdnuw_radio1\n");
data->rk.lnds=2;
}
void  vdnuw_radio3(GtkWidget *widget,class vdnuw_v_data *data)
{
//g_print("vdnuw_radio1\n");
data->rk.lnds=3;
}

/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  vdnuw_v_e_knopka(GtkWidget *widget,class vdnuw_v_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");
char bros[512];
short dd,md,gd;


int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case E_DATAD:

    if(iceb_calendar(&data->rk.datad,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAD]),data->rk.datad.ravno());
      
    return;  

  case E_DATA_VNP:

    if(iceb_calendar(&data->rk.data_vnp,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_VNP]),data->rk.data_vnp.ravno());
      
    return;  

  case E_DATA_OPL:

    if(iceb_calendar(&data->rk.data_opl,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_OPL]),data->rk.data_opl.ravno());
      
    return;  

  case E_DATA_DOVER:

    if(iceb_calendar(&data->rk.data_dover,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_DOVER]),data->rk.data_dover.ravno());
      
    return;  

  case E_DATA_POL_NAL_NAK:

    if(iceb_calendar(&data->rk.data_pol_nal_nak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_POL_NAL_NAK]),data->rk.data_pol_nal_nak.ravno());

    if(data->rk.tipz == 1)
     {
      data->read_rek();

      iceb_pnnnp(data->rk.data_pol_nal_nak.ravno(),data->rk.data_vnp.ravno(),data->rk.kontr.ravno(), data->rk.nom_nal_nak.ravno(),
      data->rk.datad.ravno(),data->rk.nomdokv.ravno(),2,data->window);
     }
      
    return;  

  case E_KONTR:

    if(iceb_poi_kontr(&data->rk.kontr,&naim,data->window) == 0)
     {
      gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),naim.ravno_toutf(20));
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rk.kontr.ravno_toutf());
      gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_KONTR]),"activate");
     }

/**********
    if(iceb_vibrek(1,"Kontragent",&data->rk.kontr,&naim,data->window) == 0)
       gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),naim.ravno_toutf(20));
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rk.kontr.ravno_toutf());
*****************/
    return;

  case E_PODR:
    if(l_uslpodr(1,&kod,&naim,data->window) == 0)
     {
      data->rk.podrv.new_plus(kod.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_naim_podr),naim.ravno_toutf(20));
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk.podrv.ravno_toutf());
    return;

  case E_FOR_OPL:
    if(iceb_vibrek(1,"Foroplat",&data->rk.for_opl,&naim,data->window) == 0)
       gtk_label_set_text(GTK_LABEL(data->label_naim_for_opl),naim.ravno_toutf(20));
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_FOR_OPL]),data->rk.for_opl.ravno_toutf());
    return;

  case E_USL_PROD:
    data->rk.kontr.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_KONTR])));
    data->rk.usl_prod.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_USL_PROD])));
    if(data->rk.usl_prod.getdlinna() > 1)
     return;
    if(data->rk.kontr.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не ввели код контрагента!"),data->window);
      return;
     }
    
    
    if(iceb_getuslp_m(data->rk.kontr.ravno(),&data->rk.usl_prod,data->window) == 0)
     {
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_USL_PROD]),data->rk.usl_prod.ravno());
     }      
    return;  

//    gtk_entry_set_text(GTK_ENTRY(data->entry[E_FOR_OPL]),data->rk.for_opl.ravno_toutf());
    return;

  case E_KODOP:
 
    if(data->rk.tipz == 1)
     if(l_uslopp(1,&kod,&naim,data->window) == 0)
      {
       gtk_label_set_text(GTK_LABEL(data->label_naim_kodop),naim.ravno_toutf(20));
       data->rk.kodop.new_plus(kod.ravno());
      }

    if(data->rk.tipz == 2)
     if(l_uslopr(1,&kod,&naim,data->window) == 0)
      {
       gtk_label_set_text(GTK_LABEL(data->label_naim_kodop),naim.ravno_toutf(20));
       data->rk.kodop.new_plus(kod.ravno());
      }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->rk.kodop.ravno_toutf());
    return;

  case E_NOMDOK:
    data->rk.datad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATAD]))));
    data->rk.podrv.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_PODR]))));
    if(data->rk.datad.getdlinna() > 1 && data->rk.podrv.getdlinna() > 1)
     {
      iceb_nomnak(data->rk.datad.ravno(),data->rk.podrv.ravno(),&data->rk.nomdokv,data->rk.tipz,1,0,data->window);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk.nomdokv.ravno_toutf());
     }
    return;

  case E_NOM_NAL_NAK:
//    printf("дата=%s\n",data->rk.datad.ravno());
    if(data->rk.datad.getdlinna() <= 1)
      return;
    iceb_u_rsdat(&dd,&md,&gd,data->rk.datad.ravno(),1);
    sprintf(bros,"%d",iceb_nomnalnak(md,gd,data->window));
    data->rk.nom_nal_nak.new_plus(bros);    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOM_NAL_NAK]),data->rk.nom_nal_nak.ravno_toutf());

    return;

 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vdnuw_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class vdnuw_v_data *data)
{

//printf("vdnuw_v_v_key_press\n");
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
void  vdnuw_v_v_knopka(GtkWidget *widget,class vdnuw_v_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(vdnuw_zap(data) != 0)
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

void    vdnuw_v_v_vvod(GtkWidget *widget,class vdnuw_v_data *data)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
class iceb_u_str naimkod("");
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("vdnuw_v_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KONTR:
    data->rk.kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(data->rk.kontr.getdlinna() > 1)
     {
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->rk.kontr.ravno());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
        naimkod.new_plus(row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),naimkod.ravno_toutf(20));
     }
    break;

  case E_PODR:
    data->rk.podrv.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(data->rk.podrv.getdlinna() > 1)
     {
      //Узнаём наименование подразделения
      sprintf(strsql,"select naik from Uslpodr where kod=%d",data->rk.podrv.ravno_atoi());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       naimkod.new_plus(row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naim_podr),naimkod.ravno_toutf(20));
     }
    break;

  case E_KODOP:
    data->rk.kodop.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(data->rk.kodop.getdlinna() > 1)
     {
      //Узнаём наименование операции
      if(data->rk.tipz == 1)
        sprintf(strsql,"select naik from Usloper1 where kod='%s'",data->rk.kodop.ravno());
      if(data->rk.tipz == 2)
        sprintf(strsql,"select naik from Usloper2 where kod='%s'",data->rk.kodop.ravno());

      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       naimkod.new_plus(row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naim_kodop),naimkod.ravno_toutf(20));
     }
    break;

  case E_FOR_OPL:
    data->rk.for_opl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAD:
    data->rk.datad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATA_VNP:
    data->rk.data_vnp.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATA_DOVER:
    data->rk.data_dover.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHEREZ:
    data->rk.sherez.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOMDOK:
    data->rk.nomdokv.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_USL_PROD:
    data->rk.usl_prod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOM_NAL_NAK:
    data->rk.nom_nal_nak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    if(data->rk.tipz == 1)
     {
      data->read_rek();

      iceb_pnnnp(data->rk.data_pol_nal_nak.ravno(),data->rk.data_vnp.ravno(),data->rk.kontr.ravno(), data->rk.nom_nal_nak.ravno(),
      data->rk.datad.ravno(),data->rk.nomdokv.ravno(),2,data->window);
     }

    break;

  case E_DATA_POL_NAL_NAK:
    data->rk.data_pol_nal_nak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    if(data->rk.tipz == 1)
     {
      data->read_rek();

      iceb_pnnnp(data->rk.data_pol_nal_nak.ravno(),data->rk.data_vnp.ravno(),data->rk.kontr.ravno(), data->rk.nom_nal_nak.ravno(),
      data->rk.datad.ravno(),data->rk.nomdokv.ravno(),2,data->window);
     }

    break;

  case E_DOVER:
    data->rk.dover.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATA_OPL:
    data->rk.data_opl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_OSNOV:
    data->rk.osnov.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOMDOK_POS:
    data->rk.nomdok_pos.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int vdnuw_zap(class vdnuw_v_data *data)
{
char strsql[1024];
time_t   vrem;
short prov=0;
SQL_str row;
class SQLCURSOR cur;

data->read_rek(); //Читаем реквизиты меню


if(data->rk.kontr.getdlinna() <= 1 || data->rk.datad.getdlinna() <= 1 ||\
data->rk.podrv.getdlinna() <= 1 || data->rk.nomdokv.getdlinna() <= 1 || \
data->rk.kodop.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введены все обязательные реквизиты !"),data->window);
  return(1);
 }

if(data->rk.nom_nal_nak.getdlinna() > 1 && data->rk.data_pol_nal_nak.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена дата получения налоговой накладной !"),data->window);
  return(1);
 }

if(data->rk.data_pol_nal_nak.getdlinna() > 1)
if(data->rk.data_pol_nal_nak.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Не верно введена дата получения налоговой накладной !"),data->window);
  return(1);
 }

short denv=0,mesv=0,godv=0;
if(data->rk.data_vnp.getdlinna() > 1)
if(iceb_u_rsdat(&denv,&mesv,&godv,data->rk.data_vnp.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Неправильно ввели дату виписки !"),data->window);
  return(1);
 }


if(data->rk.ds != 0)
 {
  if(iceb_pbpds(data->rk.ms,data->rk.gs,data->window) != 0)
    return(1);
 }


if(iceb_pbpds(data->rk.datad.ravno(),data->window) != 0)
 {
  sprintf(strsql,gettext("Дата %s г. заблокирована !"),data->rk.datad.ravno_mes_god());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
short dd,md,gd;
if(iceb_u_rsdat(&dd,&md,&gd,data->rk.datad.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата документа !"),data->window);
  return(1);
 }

//Проверяем код контрагента
sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",data->rk.kontr.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не нашли код контрагента"),data->rk.kontr.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
//Проверяем форму оплаты
if(data->rk.for_opl.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Foroplat where kod='%s'",data->rk.for_opl.ravno());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не нашли код формы оплаты"),data->rk.for_opl.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }
//Проверяем код подразделения
sprintf(strsql,"select kod from Uslpodr where kod=%s",data->rk.podrv.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не нашли код подразделения"),data->rk.podrv.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

//Проверяем код операции
if(data->rk.tipz == 1)
 sprintf(strsql,"select kod from Usloper1 where kod='%s'",data->rk.kodop.ravno());
if(data->rk.tipz == 2)
 sprintf(strsql,"select kod from Usloper2 where kod='%s'",data->rk.kodop.ravno());

if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не нашли код операции"),data->rk.kodop.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

//Проверяем правильность ввода даты выдачи доверенности
if(data->rk.data_dover.getdlinna() > 1)
 {
  short ddov,mdov,gdov;
  if(iceb_u_rsdat(&ddov,&mdov,&gdov,data->rk.data_dover.ravno(),1) != 0)
   {
    iceb_menu_soob(gettext("Неправильно введена дата выдачи доверенности !"),data->window);
    return(1);
   }
 }
int metka_opl=0; 
//Узнаём нужно ли для этой операции делать проводки
if(data->rk.tipz == 1)
 sprintf(strsql,"select prov,vido from Usloper1 where kod='%s'",data->rk.kodop.ravno());
if(data->rk.tipz == 2)
 sprintf(strsql,"select prov,vido from Usloper2 where kod='%s'",data->rk.kodop.ravno());

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 if(row[0][0] == '1')
  prov=1;

if(atoi(row[1]) != 0)
 metka_opl=1;
time(&vrem);
float nds=iceb_pnds(data->rk.datad.ravno(),data->window);
if(data->rk.ds == 0) //Запись шапки нового документа
 {
  if(data->rk.tipz == 2 && data->rk.nom_nal_nak.getdlinna() > 1) 
   if(iceb_provnomnnak(md,gd,data->rk.nom_nal_nak.ravno(),data->window) != 0)
    return(1);
  class iceb_lock_tables kk("LOCK TABLES Usldokum WRITE,icebuser READ"); //Блокируем таблицу на запись
  //Проверяем может уже такой документ уже есть
  sprintf(strsql,"select tp from Usldokum where datd >= '%04d-%02d-%02d' and \
datd <= '%04d-%02d-%02d' and podr=%d and \
nomd='%s' and tp=%d",
  gd,1,1,
  gd,12,31,
  data->rk.podrv.ravno_atoi(),
  data->rk.nomdokv.ravno(),
  data->rk.tipz);
  if(iceb_sql_readkey(strsql,data->window) > 0)
   {
    sprintf(strsql,gettext("Документ с номером %s уже есть !"),data->rk.nomdokv.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
/************
  char bros[512];
  memset(bros,'\0',sizeof(bros));
  iceb_poldan("Н.Д.С.",bros,"matnast.alx",data->window);
  float nds=atof(bros);
***************/

  sprintf(strsql,"insert into Usldokum \
values (%d,'%s',%s,'%s','%s','%s','%s',%d,'%s','%s','%s',\
%.2f,%d,%d,%d,%d,'%s','%04d-%d-%d','%s','%s','%s',%d,%ld,'%s',\
'%s',0.,%.2f,%d)",
  data->rk.tipz,
  data->rk.datad.ravno_sqldata(),
  data->rk.podrv.ravno(),
  data->rk.kontr.ravno(),
  data->rk.nomdokv.ravno(),
  data->rk.nom_nal_nak.ravno(),
  data->rk.kodop.ravno(),
  data->rk.lnds,
  data->rk.for_opl.ravno(),
  data->rk.data_opl.ravno_sqldata(),
  data->rk.usl_prod.ravno(),
  0.,
  0,
  prov,
  0,
  0,
  data->rk.nomdok_pos.ravno(),
  godv,mesv,denv,
  data->rk.dover.ravno(),
  data->rk.data_dover.ravno_sqldata(),
  data->rk.sherez.ravno(),
  iceb_getuid(data->window),
  vrem,
  data->rk.osnov.ravno(),
  data->rk.data_pol_nal_nak.ravno(),
  nds,
  metka_opl);

  if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
   return(1);
  
 }
else
 {
  if(data->rk.tipz == 2) 
  if(iceb_u_SRAV(data->rk.nom_nal_nak.ravno(),data->rk.nom_nal_nakz.ravno(),0) != 0)
   if(iceb_provnomnnak(md,gd,data->rk.nom_nal_nak.ravno(),data->window) != 0)
    return(1);

  if(md != data->rk.ms || gd != data->rk.gs);
   if(iceb_pvkdd(6,data->rk.ds,data->rk.ms,data->rk.gs,md,gd,data->rk.podrs,data->rk.nomdoks.ravno(),data->rk.tipz,data->window) != 0)
    return(1);


  sprintf(strsql,"update Usldokum set \
datd='%s',\
podr=%s,\
kontr='%s',\
nomd='%s',\
nomnn='%s',\
kodop='%s',\
nds=%d,\
forop='%s',\
datop='%s',\
uslpr='%s',\
ktoi=%d,\
vrem=%ld,\
osnov='%s',\
nomdp='%s',\
pro=%d,\
datpnn='%s',\
dover='%s',\
datdov='%s',\
datdp='%04d-%02d-%02d',\
sherez='%s', \
pn=%.2f \
where tp=%d and datd='%04d-%d-%d' and podr=%d and nomd='%s'",
  data->rk.datad.ravno_sqldata(),
  data->rk.podrv.ravno(),
  data->rk.kontr.ravno(),
  data->rk.nomdokv.ravno(),
  data->rk.nom_nal_nak.ravno(),
  data->rk.kodop.ravno(),
  data->rk.lnds,
  data->rk.for_opl.ravno(),
  data->rk.data_opl.ravno_sqldata(),
  data->rk.usl_prod.ravno(),
  iceb_getuid(data->window),
  vrem,
  data->rk.osnov.ravno(),
  data->rk.nomdok_pos.ravno(),
  prov,
  data->rk.data_pol_nal_nak.ravno(),
  data->rk.dover.ravno(),
  data->rk.data_dover.ravno_sqldata(),
  godv,mesv,denv,
  data->rk.sherez.ravno(),
  nds,
  data->rk.tipz,data->rk.gs,data->rk.ms,data->rk.ds,data->rk.podrs,data->rk.nomdoks.ravno());

  if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
   return(1);


  sprintf(strsql,"update Usldokum1 set \
datd='%s',\
podr=%s,\
nomd='%s'\
where tp=%d and datd='%04d-%d-%d' and podr=%d and nomd='%s'",
  data->rk.datad.ravno_sqldata(),data->rk.podrv.ravno(),data->rk.nomdokv.ravno(),
  data->rk.tipz,data->rk.gs,data->rk.ms,data->rk.ds,data->rk.podrs,data->rk.nomdoks.ravno());

  iceb_sql_zapis(strsql,0,0,data->window);

  /*Исправляем подтверждающие записи в которых дата подтверждения
   совпадает с датой документа*/

  sprintf(strsql,"update Usldokum2 set \
datd='%s',\
datp='%04d-%02d-%02d',\
podr=%s,\
nomd='%s'\
where tp=%d and datd='%04d-%d-%d' and datp='%d-%d-%d' and podr=%d and \
nomd='%s'", 
  data->rk.datad.ravno_sqldata(),
  gd,md,dd,
  data->rk.podrv.ravno(),
  data->rk.nomdokv.ravno(),
  data->rk.tipz,
  data->rk.gs,data->rk.ms,data->rk.ds,
  data->rk.gs,data->rk.ms,data->rk.ds,
  data->rk.podrs,
  data->rk.nomdoks.ravno());


  iceb_sql_zapis(strsql,0,0,data->window);

  /*Исправляем все остальные подтверждающие записи*/
  sprintf(strsql,"update Usldokum2 set \
datd='%s',\
podr=%s,\
nomd='%s'\
where tp=%d and datd='%04d-%d-%d' and podr=%d and nomd='%s'",
  data->rk.datad.ravno_sqldata(),
  data->rk.podrv.ravno(),
  data->rk.nomdokv.ravno(),
  data->rk.tipz,
  data->rk.gs,data->rk.ms,data->rk.ds,
  data->rk.podrs,
  data->rk.nomdoks.ravno());

  iceb_sql_zapis(strsql,0,0,data->window);

  /*Исправляем проводки в которых дата проводки совпадает с
  датой документа*/
  sprintf(strsql,"update Prov \
set \
oper='%s',\
nomd='%s',\
datp='%s',\
datd='%04d-%02d-%02d',\
pod=%s,\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%d-%d' and datp='%04d-%d-%d' and nomd='%s' and \
pod=%d and kto='%s'",
  data->rk.kodop.ravno(),
  data->rk.nomdokv.ravno(),
  data->rk.datad.ravno_sqldata(),
  gd,md,dd,
  data->rk.podrv.ravno(),
  iceb_getuid(data->window),
  vrem,
  data->rk.gs,data->rk.ms,data->rk.ds,
  data->rk.gs,data->rk.ms,data->rk.ds,
  data->rk.nomdoks.ravno(),
  data->rk.podrs,
  gettext("УСЛ"));

  iceb_sql_zapis(strsql,0,0,data->window);
  /*Исправляем остальные проводки*/

  sprintf(strsql,"update Prov \
set \
oper='%s',\
nomd='%s',\
datd='%s',\
pod=%s,\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%02d-%02d' and nomd='%s' and \
pod=%d and kto='%s'",
  data->rk.kodop.ravno(),
  data->rk.nomdokv.ravno(),
  data->rk.datad.ravno_sqldata(),
  data->rk.podrv.ravno(),
  iceb_getuid(data->window),vrem,
  data->rk.gs,data->rk.ms,data->rk.ds,
  data->rk.nomdoks.ravno(),
  data->rk.podrs,
  gettext("УСЛ"));

  iceb_sql_zapis(strsql,0,0,data->window);

  if(iceb_u_SRAV(data->rk.kontrz.ravno(),data->rk.kontr.ravno(),0) != 0)
   {
    /*Исправляем код контрагента в проводках*/
    sprintf(strsql,"update Prov \
set \
kodkon='%s' \
where datd='%04d-%d-%d' and nomd='%s' and \
pod=%d and kto='%s' and kodkon='%s'",
    data->rk.kontr.ravno(),
    data->rk.gs,data->rk.ms,data->rk.ds,
    data->rk.nomdokv.ravno(),
    data->rk.podrs,
    gettext("УСЛ"),
    data->rk.kontrz.ravno());

    iceb_sql_zapis(strsql,0,0,data->window);

   }
 }

//printf("kontr_zap-strsql=%s\n",strsql);
//if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
// return(1);

//gtk_widget_hide(data->window);


return(0);

}
