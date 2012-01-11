/*$Id: vkartm.c,v 1.28 2011-02-21 07:35:58 sasa Exp $*/
/*18.02.2010	11.07.2004	Белых А.И.	vkartm.c
Ввод новой карточки материалла
*/

#include <stdlib.h>
#include "../headers/buhg_g.h"

enum
{
  FK2,
  FK4,
  FK5,
  FK6,
  FK7,
  FK10,
  KOL_F_KL
};

enum
 {
  E_SHET,
  E_CENA,
  E_KOLIH,
  E_EI,
  E_NKART,
  E_KRAT,
  E_FAS,
  E_KODTAR,
  E_NDS,
  E_DATA_KON_IS,
  E_SHET_POL,
  E_NOMZ,
  E_CENAP,
//обязательные для чтения реквизиты
  E_SUMABNDS,
  E_CENASNDS,
  KOLENTER  
 };

class vkartm_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_shet;
  GtkWidget *hbox_cenap;

  short kl_shift;
  short voz;      //0-записано 1 нет
    
  short dd,md,gd;
  iceb_u_str nomdok;
  int tipz;
  int skl;
  iceb_u_str kodm;
  double cenadok;
  double kolihdok;
  float pnds;
  
  class iceb_u_str shet;
  class iceb_u_str shet_pol;
  class iceb_u_str cena;
  class iceb_u_str kolih;
  class iceb_u_str nkart;  
  class iceb_u_str krat;
  class iceb_u_str fas;
  class iceb_u_str kodtar;
  class iceb_u_str cenap;
  class iceb_u_str ei;
  class iceb_u_str nds;
  class iceb_u_str sumabnds;
  class iceb_u_str cenasnds;
  class iceb_u_str nomz;
  class iceb_u_str innom;
  class iceb_u_str regnom;
  class iceb_u_str data_kon_is;
  short		denv,mesv,godv; //Дата начала эксплуатации для малоценки
    
  short metka_f6;    //если 1-нажата клавиша F6
  short metkanz;
  short metkacp;
  vkartm_data() //Конструктор
   {
    metka_f6=0;
    denv=mesv=godv=0; //Дата начала эксплуатации для малоценки
    metkacp=0;
    metkanz=0;
    kl_shift=0;
    voz=1;
    kodm.new_plus("");
    shet.new_plus("");
    shet_pol.new_plus("");
    nkart.new_plus("");
    krat.new_plus("");
    fas.new_plus("");
    kodtar .new_plus("");
    kolih.new_plus("");
    cena.new_plus("");
    cenap.new_plus("");
    ei.new_plus("");
    nds.new_plus("");
    sumabnds.new_plus("");
    cenasnds.new_plus("");
    nomz.new_plus("");
    innom.new_plus("");
    regnom.new_plus("");
    data_kon_is.plus("");
    pnds=0.;
   }

  void read_rek()
   {

    shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET]))));
    shet_pol.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET_POL]))));
    cena.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_CENA]))));
    kolih.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOLIH]))));
    nkart.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NKART]))));
    krat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KRAT]))));
    fas.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_FAS]))));
    kodtar.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KODTAR]))));
    nds.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NDS]))));
    ei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_EI]))));
    data_kon_is.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_KON_IS]))));
    if(metkacp == 1)
      cenap.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_CENAP]))));
    if(metkanz == 1)
      nomz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMZ]))));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      if(metkanz == 0 && i == E_NOMZ)
        continue;
      if(metkacp == 0 && i == E_CENAP)
        continue;
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
     }
   }
 };

gboolean   vkartm_key_press(GtkWidget *widget,GdkEventKey *event,class vkartm_data *data);
void    vkartm_vvod(GtkWidget *widget,class vkartm_data *data);
void  vkartm_knopka(GtkWidget *widget,class vkartm_data *data);
int vkartm_zap(class vkartm_data *data);
void  vkartm_e_knopka(GtkWidget *widget,class vkartm_data *data);

int vmat_mc(iceb_u_str *innom,short *dn,short *mn,short *gn,GtkWidget*);
int vmat_medp(iceb_u_str *innom,short *dn,short *mn,short *gn,iceb_u_str*,GtkWidget*);

extern char     *mtsh;  /*Перечень материальных счетов*/
extern char	*shrt;    /*Счета розничной торговли*/
extern char *name_system;
extern SQL_baza bd;
extern double	okrcn;  /*Округление цены*/
extern double   okrg1;  /*Округление 1*/

int vkartm(short dd,short md,short gd,const char *nomdok,int tipz,int skl,int kodm,
double cenadok, //Цена в документе Для привязки записи к карточке
double kolihdok, //Кoличество в документе Для привязки записи к карточке
GtkWidget *wpredok)
{
vkartm_data data;
char strsql[512];
iceb_u_str naim;
SQL_str    row;
SQLCURSOR  cur;

data.dd=dd;
data.md=md;
data.gd=gd;
data.skl=skl;
data.tipz=tipz;
data.nomdok.new_plus(nomdok);
data.cenadok=cenadok;
data.kolihdok=kolihdok;

//Узнаём наименование слода
iceb_u_str naimskl;
naimskl.new_plus("");
sprintf(strsql,"select naik from Sklad where kod=%d",skl);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naimskl.new_plus(row[0]);
 
//Узнаём код операции
iceb_u_str kodop;
kodop.plus("");
sprintf(strsql,"select kodop,pn from Dokummat where datd='%d-%d-%d' and sklad=%d and nomd='%s' \
and tip=%d",gd,md,dd,skl,nomdok,tipz);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 { 
  kodop.new_plus(row[0]);
  data.pnds=atof(row[1]);
 }
//Унаём НДС документа
int lnds=0;
sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' \
and nomerz=11",gd,skl,nomdok);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 lnds=atoi(row[0]);

if(iceb_poldan("Коды операций прихода из производства",strsql,"matnast.alx",wpredok) == 0)
 if(iceb_u_proverka(strsql,kodop.ravno(),0,1) == 0)
   data.metkanz=1;



naim.new_plus("");
//Читаем наименование материалла
sprintf(strsql,"select kodm,naimat,ei,cenapr,krat,fasv,kodt,kodgr from Material where kodm=%d",kodm);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найден код материалла"));
  repl.plus(" ");
  repl.plus(kodm);
  repl.plus(" !");
  iceb_menu_soob(&repl,wpredok);
  return(1); 
 }
naim.new_plus(row[1]);
data.kodm.new_plus(row[0]);
data.ei.new_plus(row[2]);

if(cenadok != 0.)
 {
  sprintf(strsql,"%.10g",cenadok);
  data.cena.new_plus(strsql);
 }
if(kolihdok > 0.)
 {
  sprintf(strsql,"%.10g",kolihdok);
  data.kolih.new_plus(strsql);
 }
sprintf(strsql,"%d",nomkrw(skl,wpredok));
data.nkart.new_plus(strsql);
data.krat.new_plus(row[4]);
data.fas.new_plus(row[5]);
data.kodtar.new_plus(row[6]);

double nds=0.;
if(lnds == 0)
 {
  nds=data.pnds;
  sprintf(strsql,"%.5g",data.pnds);
  data.nds.new_plus(strsql);
 }
//Узнаём наименование группы
iceb_u_str naimgr;
int kodgr=atoi(row[7]);
naimgr.new_plus("");
sprintf(strsql,"select naik from Grup where kod=%d",kodgr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naimgr.new_plus(row[0]);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Ввод новой карточки материалла"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(vkartm_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
iceb_u_str repl;
repl.new_plus(gettext("Ввод новой карточки материалла"));
repl.plus("\n");
repl.plus(kodm);
repl.plus(" ");
repl.plus(naim.ravno());

repl.ps_plus(gettext("Склад"));
repl.plus(": ");
repl.plus(skl);
repl.plus(" ");
repl.plus(naimskl.ravno());

repl.ps_plus(gettext("Код группы"));
repl.plus(": ");
repl.plus(kodgr);
repl.plus(" ");
repl.plus(naimgr.ravno());

GtkWidget *label=gtk_label_new(repl.ravno_toutf());
data.label_shet=gtk_label_new("");

GtkWidget *hbox0 = gtk_hbox_new (FALSE, 5);

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *vboxknop = gtk_vbox_new (FALSE, 1);
//GtkWidget *vboxknop = gtk_vbox_new (TRUE, 1);

gtk_container_add (GTK_CONTAINER (data.window), hbox0);


gtk_box_pack_start (GTK_BOX (hbox0), vboxknop, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox0), vbox, TRUE, TRUE, 0);


gtk_box_pack_start (GTK_BOX (vbox),label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox),data.label_shet, FALSE, FALSE, 0);
//gtk_container_add (GTK_CONTAINER (vbox), label);
//gtk_container_add (GTK_CONTAINER (vbox), data.label_shet);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Счёт учёта"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(vkartm_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(vkartm_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

sprintf(strsql,"%s",gettext("Цена без НДС"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), label, FALSE, FALSE, 0);

data.entry[E_CENA] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), data.entry[E_CENA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_CENA]), "activate",GTK_SIGNAL_FUNC(vkartm_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA]),data.cena.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_CENA]),(gpointer)E_CENA);

sprintf(strsql,"%s",gettext("Количество"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_KOLIH] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), data.entry[E_KOLIH], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOLIH]), "activate",GTK_SIGNAL_FUNC(vkartm_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH]),data.kolih.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOLIH]),(gpointer)E_KOLIH);

sprintf(strsql,"%s",gettext("Единица измерения"));
data.knopka_enter[E_EI]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.knopka_enter[E_EI], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_EI]),"clicked",GTK_SIGNAL_FUNC(vkartm_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_EI]),(gpointer)E_EI);
tooltips_enter[E_EI]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_EI],data.knopka_enter[E_EI],gettext("Выбор единицы измерения"),NULL);

data.entry[E_EI] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_EI]), "activate",GTK_SIGNAL_FUNC(vkartm_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.ei.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_EI]),(gpointer)E_EI);

sprintf(strsql,"%s",gettext("Номер карточки"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_NKART] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_NKART]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NKART]), data.entry[E_NKART], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NKART]), "activate",GTK_SIGNAL_FUNC(vkartm_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NKART]),data.nkart.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NKART]),(gpointer)E_NKART);

sprintf(strsql,"%s",gettext("Кратность"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_KRAT] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KRAT]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KRAT]), data.entry[E_KRAT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KRAT]), "activate",GTK_SIGNAL_FUNC(vkartm_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KRAT]),data.krat.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KRAT]),(gpointer)E_KRAT);

sprintf(strsql,"%s",gettext("Фасовка"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_FAS] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_FAS]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_FAS]), data.entry[E_FAS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_FAS]), "activate",GTK_SIGNAL_FUNC(vkartm_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FAS]),data.fas.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_FAS]),(gpointer)E_FAS);

sprintf(strsql,"%s",gettext("Код тары"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_KODTAR] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KODTAR]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KODTAR]), data.entry[E_KODTAR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODTAR]), "activate",GTK_SIGNAL_FUNC(vkartm_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODTAR]),data.kodtar.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODTAR]),(gpointer)E_KODTAR);

sprintf(strsql,"%s",gettext("НДС"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_NDS] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_NDS]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NDS]), data.entry[E_NDS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NDS]), "activate",GTK_SIGNAL_FUNC(vkartm_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NDS]),data.nds.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NDS]),(gpointer)E_NDS);

sprintf(strsql,"%s",gettext("Дата конечного использования"));
data.knopka_enter[E_DATA_KON_IS]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_KON_IS]), data.knopka_enter[E_DATA_KON_IS], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_KON_IS]),"clicked",GTK_SIGNAL_FUNC(vkartm_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_KON_IS]),(gpointer)E_DATA_KON_IS);
tooltips_enter[E_DATA_KON_IS]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_KON_IS],data.knopka_enter[E_DATA_KON_IS],gettext("Выбор даты"),NULL);

data.entry[E_DATA_KON_IS] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_KON_IS]), data.entry[E_DATA_KON_IS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_KON_IS]), "activate",GTK_SIGNAL_FUNC(vkartm_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_KON_IS]),data.data_kon_is.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_KON_IS]),(gpointer)E_DATA_KON_IS);

sprintf(strsql,"%s",gettext("Счёт получения"));
data.knopka_enter[E_SHET_POL]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_POL]), data.knopka_enter[E_SHET_POL], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET_POL]),"clicked",GTK_SIGNAL_FUNC(vkartm_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET_POL]),(gpointer)E_SHET_POL);
tooltips_enter[E_SHET_POL]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET_POL],data.knopka_enter[E_SHET_POL],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET_POL] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_POL]), data.entry[E_SHET_POL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET_POL]), "activate",GTK_SIGNAL_FUNC(vkartm_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET_POL]),data.shet_pol.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET_POL]),(gpointer)E_SHET_POL);

if(data.metkanz == 1)
 {
  sprintf(strsql,"%s",gettext("Номер заказа"));
  label=gtk_label_new(iceb_u_toutf(strsql));
  data.entry[E_NOMZ] = gtk_entry_new_with_max_length (20);
  gtk_box_pack_start (GTK_BOX (hbox[E_NOMZ]), label, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox[E_NOMZ]), data.entry[E_NOMZ], TRUE, TRUE, 0);
  gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMZ]), "activate",GTK_SIGNAL_FUNC(vkartm_vvod),&data);
  gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMZ]),data.nomz.ravno());
  gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMZ]),(gpointer)E_NOMZ);
 }

sprintf(strsql,"%s",gettext("Цена продажи"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_CENAP] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENAP]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_CENAP]), data.entry[E_CENAP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_CENAP]), "activate",GTK_SIGNAL_FUNC(vkartm_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENAP]),data.cenap.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_CENAP]),(gpointer)E_CENAP);
data.hbox_cenap=hbox[E_CENAP];

sprintf(strsql,"%s",gettext("Сумма без НДС"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_SUMABNDS] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMABNDS]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMABNDS]), data.entry[E_SUMABNDS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMABNDS]), "activate",GTK_SIGNAL_FUNC(vkartm_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMABNDS]),data.sumabnds.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMABNDS]),(gpointer)E_SUMABNDS);


sprintf(strsql,"%s",gettext("Цена с НДС"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_CENASNDS]), label, FALSE, FALSE, 0);

data.entry[E_CENASNDS] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENASNDS]), data.entry[E_CENASNDS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_CENASNDS]), "activate",GTK_SIGNAL_FUNC(vkartm_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENASNDS]),data.cenasnds.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_CENASNDS]),(gpointer)E_CENASNDS);



GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введённой в меню информации."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(vkartm_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(vboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(vkartm_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(vboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F5 %s",gettext("Номер карточки"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Получить новый номер карточки."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]),"clicked",GTK_SIGNAL_FUNC(vkartm_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_box_pack_start(GTK_BOX(vboxknop), data.knopka[FK5], TRUE, TRUE, 0);

sprintf(strsql,"F6 %s",gettext("Малоценка"));
data.knopka[FK6]=gtk_button_new_with_label(strsql);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Записать как малоценку."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]),"clicked",GTK_SIGNAL_FUNC(vkartm_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_box_pack_start(GTK_BOX(vboxknop), data.knopka[FK6], TRUE, TRUE, 0);

sprintf(strsql,"F7 %s",gettext("Медпрепарат"));
data.knopka[FK7]=gtk_button_new_with_label(strsql);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Записать как медпрепарат."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]),"clicked",GTK_SIGNAL_FUNC(vkartm_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_box_pack_start(GTK_BOX(vboxknop), data.knopka[FK7], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(vkartm_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(vboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_widget_hide(data.hbox_cenap);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
return(data.voz);


}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  vkartm_e_knopka(GtkWidget *widget,class vkartm_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATA_KON_IS:

    if(iceb_calendar(&data->data_kon_is,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_KON_IS]),data->data_kon_is.ravno());
      
    return;  
   
  case E_SHET:

    if(iceb_vibrek(1,"Plansh",&data->shet,data->window) == 0)
     {
      class OPSHET reksh;
      class iceb_u_str repl;
      iceb_prsh1(data->shet.ravno(),&reksh,data->window);
      repl.new_plus(gettext("Наименование счёта"));
      repl.plus(": ");
      repl.plus(reksh.naim.ravno());
              
      gtk_label_set_text(GTK_LABEL(data->label_shet),repl.ravno_toutf());
     }
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet.ravno_toutf());

    return;  

  case E_EI:

    iceb_vibrek(1,"Edizmer",&data->ei,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),data->ei.ravno_toutf());

    return;  

  case E_SHET_POL:

    iceb_vibrek(1,"Plansh",&data->shet_pol,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet_pol.ravno_toutf());

    return;  


 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vkartm_key_press(GtkWidget *widget,GdkEventKey *event,class vkartm_data *data)
{

//printf("vkartm_key_press\n");
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
void  vkartm_knopka(GtkWidget *widget,class vkartm_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    
    data->read_rek(); //Читаем реквизиты меню
    if(vkartm_zap(data) != 0)
      return;
    if(data->nkart.ravno_atoi() < 0)
     {
      iceb_menu_soob(gettext("Номер карточки не может быть отрицательным !"),data->window);
      return;
     }
    data->voz=0;        
    gtk_widget_destroy(data->window);
    data->window=NULL;
    return;  


  case FK4:
    data->clear_rek();
    return;

  case FK6:
    data->read_rek(); //Читаем реквизиты меню

    if(vmat_mc(&data->innom,&data->denv,&data->mesv,&data->godv,data->window) != 0)
      return;
    
    data->metka_f6=1;
    if(vkartm_zap(data) != 0)
      return;
    data->voz=0;        
    
    gtk_widget_destroy(data->window);
    data->window=NULL;
    return;  

  case FK7:
    data->read_rek(); //Читаем реквизиты меню
    if(vmat_medp(&data->innom,&data->denv,&data->mesv,&data->godv,&data->regnom,data->window) != 0)
      return;

    if(vkartm_zap(data) != 0)
      return;
    data->voz=0;        
    gtk_widget_destroy(data->window);
    data->window=NULL;
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

void    vkartm_vvod(GtkWidget *widget,class vkartm_data *data)
{
double bb=0.;
double cena=0.;
char strsql[512];
class OPSHET reksh;
iceb_u_str repl;
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
switch (enter)
 {
  case E_SHET:
    data->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(iceb_prsh1(data->shet.ravno(),&reksh,data->window) == 0)
     {
      repl.new_plus(gettext("Наименование счёта"));
      repl.plus(": ");
      repl.plus(reksh.naim.ravno());
            
      gtk_label_set_text(GTK_LABEL(data->label_shet),repl.ravno_toutf());
     }
    else  
     gtk_label_set_text(GTK_LABEL(data->label_shet),"");

    /*Проверяем не является ли счет счетом розничной торговли*/
    if(shrt != NULL && data->metkanz == 0)
     {
      if(iceb_u_proverka(shrt,data->shet.ravno(),0,1) != 0)
       {
        data->metkacp=0;
        data->cenap.new_plus("");
        gtk_widget_hide(data->hbox_cenap);
       }
      else
       {
        data->metkacp=1;
        gtk_widget_show_all(data->hbox_cenap);
       }
     }

    break;

  case E_NKART:
    data->nkart.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    
    break;

  case E_KRAT:
    data->krat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_FAS:
    data->fas.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATA_KON_IS:
    data->data_kon_is.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHET_POL:
    data->shet_pol.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KODTAR:
    data->kodtar.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


  case E_KOLIH:
    data->kolih.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(data->cena.getdlinna() > 1)
     {
      bb=data->cena.ravno_atof()*data->kolih.ravno_atof();
      bb=iceb_u_okrug(bb,okrg1);
      sprintf(strsql,"%.10g",bb);
      data->sumabnds.new_plus(strsql);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMABNDS]),data->sumabnds.ravno());
     }

    break;

  case E_CENA:
    data->cena.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    bb=data->cena.ravno_atof();
    cena=bb=iceb_u_okrug(bb,okrcn);
    sprintf(strsql,"%.10g",bb);

    bb=bb+bb*data->pnds/100.;
    bb=iceb_u_okrug(bb,okrg1);

    sprintf(strsql,"%.10g",bb);
    data->cenasnds.new_plus(strsql);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENASNDS]),data->cenasnds.ravno());


    bb=cena*data->kolih.ravno_atof();
    bb=iceb_u_okrug(bb,okrg1);
    sprintf(strsql,"%.10g",bb);
    data->sumabnds.new_plus(strsql);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMABNDS]),data->sumabnds.ravno());
    
    break;

  case E_CENASNDS:
    data->cenasnds.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    bb=data->cenasnds.ravno_atof();

    bb=bb-bb*data->pnds/(100.+data->pnds);
    bb=iceb_u_okrug(bb,okrcn);
    sprintf(strsql,"%.10g",bb);
    data->cena.new_plus(strsql);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENA]),data->cena.ravno());

    bb=bb*data->kolih.ravno_atof();
    bb=iceb_u_okrug(bb,okrg1);
    sprintf(strsql,"%.10g",bb);
    data->sumabnds.new_plus(strsql);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMABNDS]),data->sumabnds.ravno());

    break;


  case E_EI:
    data->ei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NDS:
    data->nds.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOMZ:
    data->nomz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_CENAP:
    data->nomz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


  case E_SUMABNDS:
    data->sumabnds.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    
    if(data->sumabnds.ravno_atof() != 0. && data->kolih.ravno_atof() != 0.)
     {
      bb=data->sumabnds.ravno_atof()/data->kolih.ravno_atof();
      bb=iceb_u_okrug(bb,okrcn);
      sprintf(strsql,"%.10g",bb);
      data->cena.new_plus(strsql);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENA]),data->cena.ravno());

      bb=bb+bb*data->pnds/100.;
      bb=iceb_u_okrug(bb,okrg1);

      sprintf(strsql,"%.10g",bb);
      data->cenasnds.new_plus(strsql);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENASNDS]),data->cenasnds.ravno());
     }
    break;


 }

if(enter == E_NOMZ && data->metkanz == 0)
 enter+=1;

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/**************************/
/*запись введенной информации*/
/*****************************/

int vkartm_zap(class vkartm_data *data)
{
short		msnk=0; /*0-создать новую карточку и привязать к ней
			1 - привязать к найденной*/

if(data->nkart.getdlinna() <= 1 || data->shet.getdlinna() <= 1 || data->ei.getdlinna() <= 1 \
 || data->kolih.getdlinna() <= 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не введены все обязательные реквизиты !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 }

/*Проверяем не является ли счет счетом розничной торговли*/
if(shrt != NULL && data->metkanz == 0)
 {
  data->metkacp=1;
  if(iceb_u_proverka(shrt,data->shet.ravno(),0,1) != 0)
   {
    data->metkacp=0;
    data->cenap.new_plus("");
   }
 }    

if(data->metkacp == 1 && data->metkanz == 0 && data->cenap.getdlinna() <= 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не введена цена продажи !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 }

class OPSHET shetv;
if(iceb_prsh1(data->shet.ravno(),&shetv,data->window) != 0)
  return(1);

if(data->shet_pol.getdlinna() > 1)
 if(iceb_prsh1(data->shet_pol.ravno(),&shetv,data->window) != 0)
  return(1);

if(iceb_u_proverka(mtsh,data->shet.ravno(),1,1) != 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Счёт не материальный !"));
  repl.ps_plus(data->shet.ravno());
  iceb_menu_soob(&repl,data->window);
  return(1);
 }

short deng=0,mesg=0,godg=0;
if(data->data_kon_is.getdlinna() > 1)
 if(iceb_u_rsdat(&deng,&mesg,&godg,data->data_kon_is.ravno(),1) != 0)
  {
   iceb_menu_soob(gettext("Неправильно введена конечная дата использования !"),data->window);
   return(1);
  }

char strsql[1024];
SQL_str  row;
SQLCURSOR cur;

/*Проверяем может карточка с таким номером уже есть на складе*/
sprintf(strsql,"select nomk from Kart where sklad=%d and nomk=%d",
data->skl,data->nkart.ravno_atoi());
if(sql_readkey(&bd,strsql,&row,&cur) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Карточка с таким номером уже есть на складе !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 }    
 
double cena=data->cena.ravno_atof();
cena=iceb_u_okrug(cena,okrcn);
int      mnds=0;
if(data->nds.ravno()[0] == '+')
  mnds=1;

double nds=iceb_u_okrug(data->nds.ravno_atof(),0.01);
iceb_u_str nk1;
 
if(data->nomz.getdlinna() <= 1 && data->innom.getdlinna() <= 1)
 {     
  /*Проверяем может карточка с введенными уникальными реквизитами уже есть на складе*/
  sprintf(strsql,"select nomk from Kart where kodm=%d and sklad=%d \
and shetu='%s' and cena=%.10g and ei='%s' and nds=%.10g and cenap=%.10g \
and krat=%.10g and mnds=%d and dat_god='%s'",
  data->kodm.ravno_atoi(),data->skl,data->shet.ravno(),cena,data->ei.ravno(),nds,
  data->cenap.ravno_atof(),data->krat.ravno_atof(),mnds,data->data_kon_is.ravno_sqldata());
  if(sql_readkey(&bd,strsql,&row,&cur) > 0)
   {
    nk1.new_plus(row[0]);
    /*Проверяем есть ли уже такая запись в документе*/
    sprintf(strsql,"select datd from Dokummat1 where sklad=%d and \
nomd='%s' and kodm=%d and nomkar=%d and datd='%d-%d-%d'",
    data->skl,data->nomdok.ravno(),data->kodm.ravno_atoi(),nk1.ravno_atoi(),data->gd,data->md,data->dd);

    if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
     {
      iceb_u_str repl;
      repl.plus(gettext("Такая запись в документе уже есть !"));
      iceb_menu_soob(&repl,data->window);
      return(1);
     }

    iceb_u_str repl;
    repl.plus(gettext("Карточка с такими реквизитами уже есть !"));
    repl.ps_plus(gettext("Карточка"));
    repl.plus(" N");
    repl.plus(nk1.ravno());
    repl.ps_plus(gettext("Привязываем запись к этой карточке."));
    iceb_menu_soob(&repl,data->window);

    data->nkart.new_plus(nk1.ravno());
    msnk=1;
   }    
 }    

if(data->kolihdok != 0.)
 {
  /*Удяляем старую запись в документе*/
  sprintf(strsql,"delete from Dokummat1 where datd='%04d.%d.%d' and \
sklad=%d and nomd='%s' and kodm=%d and nomkar=0",
  data->gd,data->md,data->dd,data->skl,data->nomdok.ravno(),data->kodm.ravno_atoi());
  if(sql_zap(&bd,strsql) != 0)
    iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
         
 }      

if(msnk == 1)
   zapvdokw(data->dd,data->md,data->gd,data->skl,data->nkart.ravno_atoi(),
   data->kodm.ravno_atoi(),data->nomdok.ravno(),data->kolih.ravno_atof(),cena,data->ei.ravno(),
   nds,mnds,0,1,0,data->shet_pol.ravno(),data->nomz.ravno(),data->window);
else    
 {
  if(data->innom.getdlinna() > 1 && data->metka_f6 == 1)
   if(iceb_provinnom(data->innom.ravno(),data->window) != 0)
    return(1);

  if(zapkarw(data->skl,data->nkart.ravno_atoi(),data->kodm.ravno_atoi(),
  cena,data->ei.ravno(),data->shet.ravno(),data->krat.ravno_atof(),
  data->fas.ravno_atof(),data->kodtar.ravno_atoi(),data->nds.ravno_atof(),mnds,
  data->cenap.ravno_atof(),data->denv,data->mesv,data->godv,data->innom.ravno(),data->regnom.ravno(),data->nomz.ravno(),
  deng,mesg,godg,0,data->window) == 0)
     zapvdokw(data->dd,data->md,data->gd,data->skl,data->nkart.ravno_atoi(),
     data->kodm.ravno_atoi(),data->nomdok.ravno(),data->kolih.ravno_atof(),cena,data->ei.ravno(),
     nds,mnds,0,1,0,data->shet_pol.ravno(),data->nomz.ravno(),data->window);
  else
    return(1);
 }    

return(0);
}
