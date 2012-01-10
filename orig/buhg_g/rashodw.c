/*$Id: rashodw.c,v 1.26 2011-04-14 16:09:36 sasa Exp $*/
/*08.04.2011	15.07.2004	Белых А.И.	rashodw.c
Ввод расхода в выбранную карточку
*/
#include <math.h>
#include <stdlib.h>
#include "buhg_g.h"

enum
{
  FK2,
  FK3,
  FK4,
  FK5,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KOLIH,
  E_CENA,
  E_SHET,
//*******************
  E_NOMKAR,
  E_EI,
  E_NDS,
  E_CENAP,
//обязательные для чтения реквизиты
  E_CENASNDS,
  E_SUMABNDS,
  E_KOLTAR,
  KOLENTER  
 };

class rashodw_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *label_shet;
  GtkWidget *label_ost;
  GtkWidget *hbox_cenap;

  short kl_shift;
  short voz;      //0-записано 1 нет
  
  //Реквизиты котрые вводятся в меню
  class iceb_u_str kolih;
  class iceb_u_str cena;
  class iceb_u_str cenap;
  class iceb_u_str shet;
  class iceb_u_str cenasnds;
  class iceb_u_str sumabnds;
  class iceb_u_str koltar;
  class iceb_u_str ei_par;  //Единица измерения которая будет в парном документе
  class iceb_u_str nomkar_par;
  class iceb_u_str nds_par;
    
  //Реквизиты полученные
  int kodm;
  short      dd,md,gd;
  iceb_u_str nomdok;
  int skl;
  int skl1;
  int n_kart;
  int kmm;
  double kolih_dok;
  iceb_u_str nomon;   
  float pnds;
  //**************
  iceb_u_str ei_kar; //Единица измерения в карточке
  iceb_u_str shet_kar; //Счёт учёта в карточке
  iceb_u_str nomz_kar; 
  float krat_kar;
  double cena_kar;
  double cenap_z; /*Запомненная цена продажи*/
  float nds_kar;
  float fas_kar;
  int   kodt_kar;
  short mnds_kar;
  short denv,mesv,godv;
  iceb_u_str rnd_kar; 
  iceb_u_str innom_kar;
  short di,mi,gi; //Дата конечного использования 
  
  short metka_ost; //0-остаток по карточкам 1-по документам
  
  rashodw_data()
   {
    metka_ost=0;
    voz=0;
    kl_shift=0;
    clear_rekv();
    nomon.new_plus("");
    di=mi=gi=0;
   }

  void clear_rekv()
   {
    kolih.new_plus("");
    cena.new_plus("");
    shet.new_plus("");
    cenasnds.new_plus("");
    sumabnds.new_plus("");
    koltar.new_plus("");
    ei_par.new_plus("");
    nomkar_par.new_plus("");
    nds_par.new_plus("");
    cenap.new_plus("");
   }

  void read_rek()
   {
    for(int i=0; i < 7; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }

  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    clear_rekv();
   }
 };
gboolean   rashodw_key_press(GtkWidget *widget,GdkEventKey *event,class rashodw_data *data);
void    rashodw_vvod(GtkWidget *widget,class rashodw_data *data);
void  rashodw_knopka(GtkWidget *widget,class rashodw_data *data);
void   rashodw_rekviz(class rashodw_data *data);

int  rashodw_zap(class rashodw_data *data);
int rashodw_provshetrt(class rashodw_data *data);

extern char *name_system;
extern SQL_baza bd;
extern double	okrcn;  /*Округление цены*/
extern double   okrg1;  /*Округление 1*/
extern short    mdd;  /*0-обычный документ 1-двойной*/
extern short	cnsnds; /*0-Цена материалла без ндс 1- с ндс*/
extern char	*shrt;    /*Счета розничной торговли*/

int rashodw(short dd,short md,short gd,int skl,const char *nomdok,
int nk, //Номер карточки
int kmm, 
double kolih,
float pnds,
GtkWidget *wpredok)
{

char  strsql[512];
class rashodw_data data;
SQL_str    row;
SQLCURSOR  cur;




data.skl=skl;
data.dd=dd;
data.md=md;
data.gd=gd;
data.kmm=kmm;
data.kolih_dok=kolih;
data.n_kart=nk;
data.nomdok.new_plus(nomdok);
data.pnds=pnds;


//читаем реквизиты карточки
sprintf(strsql,"select * from Kart where sklad=%d and nomk=%d",skl,nk);
if(iceb_sql_readkey(strsql,&row,&cur,data.window) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найдена карточка"));
  repl.plus(" ");
  repl.plus(nk);
  repl.plus(" ");
  repl.plus(gettext("Склад"));
  repl.plus(" ");
  repl.plus(skl);
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }
data.kodm=atoi(row[2]);
data.cena_kar=atof(row[6]);
data.nds_kar=atof(row[9]);
data.cenap.new_plus(row[7]);
data.cenap_z=atof(row[7]);
data.krat_kar=atof(row[8]);
data.fas_kar=atof(row[10]);
data.ei_par.new_plus(row[4]);
data.ei_kar.new_plus(row[4]);
data.shet_kar.new_plus(row[5]);
data.mnds_kar=atoi(row[3]);
data.nomz_kar.new_plus(row[17]);
data.innom_kar.new_plus(row[15]);
data.kodt_kar=atoi(row[11]);
data.rnd_kar.new_plus(row[16]);
iceb_u_rsdat(&data.denv,&data.mesv,&data.godv,row[14],2);
iceb_u_rsdat(&data.di,&data.mi,&data.gi,row[18],2);


//Узнаём наименование склада
iceb_u_str naimskl;
naimskl.new_plus("");
sprintf(strsql,"select naik from Sklad where kod=%d",skl);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naimskl.new_plus(row[0]);
 
//Узнаём код операции
iceb_u_str kontr;
iceb_u_str kodop;
kodop.plus("");
kontr.plus("");
sprintf(strsql,"select kontr,kodop,nomon from Dokummat where datd='%d-%d-%d' and sklad=%d and nomd='%s' \
and tip=%d",gd,md,dd,skl,nomdok,2);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  kontr.new_plus(row[0]);
  kodop.new_plus(row[1]);
  data.nomon.new_plus(row[2]);
 }
//узнаём вид операции
short    mvnp=0; /*0-внешняя 1 -внутреняя 2-изменение стоимости*/
sprintf(strsql,"select vido from Rashod where kod='%s'",kodop.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  mvnp=atoi(row[0]);

iceb_u_str naim;
naim.new_plus("");
//Читаем наименование материалла
sprintf(strsql,"select kodgr,naimat,cenapr,nds from Material where kodm=%d",data.kodm);
printf("strsql=%s\n",strsql);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найден код материалла"));
  repl.plus(" ");
  repl.plus(data.kodm);
  repl.plus(" !");
  iceb_menu_soob(&repl,wpredok);
  return(1); 
 }
int kodgr=atoi(row[0]);
double cena_mat=atof(row[2]);
naim.new_plus(row[1]);
double ndscen=atof(row[3]);

short kodopuc=0; //Метка операции списания по учетной цене
if(iceb_poldan("Коды операций расхода по учетным ценам",strsql,"matnast.alx",wpredok) == 0)
 if(iceb_u_proverka(strsql,kodop.ravno(),0,1) == 0)
   kodopuc=1;  //Списание по учетной цене

if(cnsnds == 1 && cena_mat != 0. && ndscen == 0.)
 {
  cena_mat=cena_mat-cena_mat*pnds/(100.+pnds);
  cena_mat=iceb_u_okrug(cena_mat,okrcn);
 }
double cena_menu=cena_mat;
if(cena_mat == 0. || mvnp == 1 || kodopuc == 1)
 {
  cena_menu=data.cena_kar;
 } 
data.skl1=0;
if(mdd == 1)
 {
  data.shet.new_plus(data.shet_kar.ravno());
  if(kontr.ravno_atof() == 0. && iceb_u_pole(kontr.ravno(),strsql,2,'-') == 0 && kontr.ravno()[0] == '0')
    data.skl1=atoi(strsql);
  sprintf(strsql,"%d",nomkrw(data.skl1,wpredok));
  data.nomkar_par.new_plus(strsql);
  if(data.mnds_kar == 1)
    sprintf(strsql,"+%.5g",data.nds_kar);
  else
    sprintf(strsql,"%.5g",data.nds_kar);
  data.nds_par.new_plus(strsql);  
 }
//Читаем наименование группы материалла
iceb_u_str naim_grup;
naim_grup.new_plus("");
sprintf(strsql,"select naik from Grup where kod=%d",kodgr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_grup.new_plus(row[0]);

//читаем наименование счёта
iceb_u_str naim_shet_kar;
naim_shet_kar.plus("");
sprintf(strsql,"select nais from Plansh where ns='%s'",data.shet_kar.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_shet_kar.new_plus(row[0]);
 
sprintf(strsql,"%.10g",cena_menu);
data.cena.new_plus(strsql);

double cenasnds=cena_menu+cena_menu*pnds/100.;
cenasnds=iceb_u_okrug(cenasnds,okrg1);
sprintf(strsql,"%.10g",cenasnds);
data.cenasnds.new_plus(strsql);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи в расходный документ"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rashodw_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
iceb_u_str repl;
repl.new_plus(gettext("Ввод новой записи в расходный документ"));

repl.ps_plus(gettext("Материал"));
repl.plus(": ");
repl.plus(data.kodm);
repl.plus(" ");
repl.plus(naim.ravno());

repl.ps_plus(gettext("Склад"));
repl.plus(": ");
repl.plus(skl);
repl.plus(" ");
repl.plus(naimskl.ravno());

repl.ps_plus(gettext("Группа"));
repl.plus(": ");
repl.plus(kodgr);
repl.plus(" ");
repl.plus(naim_grup.ravno());

repl.ps_plus(gettext("Счёт учёта"));
repl.plus(":");
repl.plus(data.shet_kar.ravno());
repl.plus(" ");
repl.plus(naim_shet_kar.ravno());

repl.ps_plus(gettext("Карточка"));
repl.plus(":");
repl.plus(data.n_kart);


repl.plus(" ");
repl.plus(gettext("Единица измерения"));
repl.plus(":");
repl.plus(data.ei_kar.ravno());

repl.plus(" ");
repl.plus(gettext("Цена"));
repl.plus(":");
repl.plus(data.cena_kar);

repl.plus(" ");
repl.plus(gettext("Фасовка"));
repl.plus(":");
repl.plus(data.fas_kar);

repl.plus(" ");
repl.plus(gettext("Кратность"));
repl.plus(":");
repl.plus(data.krat_kar);

repl.plus(" ");
if(data.mnds_kar == 1)
 sprintf(strsql,"%s:+%.6g",gettext("НДС"),data.nds_kar);
else
 sprintf(strsql,"%s:%.6g",gettext("НДС"),data.nds_kar);
repl.plus(strsql);

GtkWidget *label=gtk_label_new(repl.ravno_toutf());

data.label_shet=gtk_label_new("");
short dt,mt,gt; //Текущая дата
iceb_u_poltekdat(&dt,&mt,&gt);

sprintf(strsql,"%s:%d.%d.%d",gettext("Остаток на"),dt,mt,gt);
repl.new_plus(strsql);

//вычисляем остаток по карточке

class ostatok ostvkar;
ostkarw(1,1,gt,dt,mt,gt,skl,nk,&ostvkar);
sprintf(strsql,"%s:%.10g / %.2f",gettext("Остаток на карточке"),ostvkar.ostg[3],ostvkar.ostgc[3]);
repl.ps_plus(strsql);

ostdokw(1,1,gt,dt,mt,gt,skl,nk,&ostvkar);
sprintf(strsql,"%s:%.10g / %.2f",gettext("Остаток по документам"),ostvkar.ostg[3],ostvkar.ostgc[3]);
repl.ps_plus(strsql);

data.label_ost=gtk_label_new(repl.ravno_toutf());

GtkWidget *hbox0 = gtk_hbox_new (FALSE, 5);

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *vboxknop = gtk_vbox_new (FALSE, 1);

gtk_container_add (GTK_CONTAINER (data.window), hbox0);


gtk_box_pack_start (GTK_BOX (hbox0), vboxknop, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox0), vbox, TRUE, TRUE, 0);


gtk_box_pack_start (GTK_BOX (vbox),label, FALSE, FALSE, 0);
if(cena_mat == 0.)
 {
  label=gtk_label_new(gettext("Не введена цена продажи. Взята цена из карточки"));
  GdkColor color;

  gdk_color_parse("red",&color);
  gtk_widget_modify_fg(label,GTK_STATE_NORMAL,&color);
  gtk_box_pack_start (GTK_BOX (vbox),label, FALSE, FALSE, 0);
 }
gtk_box_pack_start (GTK_BOX (vbox),data.label_ost, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox),data.label_shet, FALSE, FALSE, 0);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);


sprintf(strsql,"%s",gettext("Счёт списания"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_SHET] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(rashodw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);
 
sprintf(strsql,"%s",gettext("Цена без НДС"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_CENA] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), data.entry[E_CENA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_CENA]), "activate",GTK_SIGNAL_FUNC(rashodw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA]),data.cena.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_CENA]),(gpointer)E_CENA);
 
sprintf(strsql,"%s",gettext("Количество"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_KOLIH] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), data.entry[E_KOLIH], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOLIH]), "activate",GTK_SIGNAL_FUNC(rashodw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH]),data.kolih.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOLIH]),(gpointer)E_KOLIH);
 

sprintf(strsql,"%s",gettext("Сумма без НДС"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_SUMABNDS] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMABNDS]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMABNDS]), data.entry[E_SUMABNDS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMABNDS]), "activate",GTK_SIGNAL_FUNC(rashodw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMABNDS]),data.sumabnds.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMABNDS]),(gpointer)E_SUMABNDS);
 
sprintf(strsql,"%s",gettext("Цена с НДС"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_CENASNDS] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENASNDS]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_CENASNDS]), data.entry[E_CENASNDS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_CENASNDS]), "activate",GTK_SIGNAL_FUNC(rashodw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENASNDS]),data.cenasnds.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_CENASNDS]),(gpointer)E_CENASNDS);
 
sprintf(strsql,"%s",gettext("Количество тары"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_KOLTAR] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLTAR]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLTAR]), data.entry[E_KOLTAR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOLTAR]), "activate",GTK_SIGNAL_FUNC(rashodw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLTAR]),data.koltar.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOLTAR]),(gpointer)E_KOLTAR);

sprintf(strsql,"%s",gettext("Продажная цена"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_CENAP] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENAP]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_CENAP]), data.entry[E_CENAP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_CENAP]), "activate",GTK_SIGNAL_FUNC(rashodw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENAP]),data.cenap.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_CENAP]),(gpointer)E_CENAP);
data.hbox_cenap=hbox[E_CENAP];


sprintf(strsql,"%s",gettext("Единица измерения"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_EI] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_EI]), "activate",GTK_SIGNAL_FUNC(rashodw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.ei_par.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_EI]),(gpointer)E_EI);

sprintf(strsql,"%s",gettext("Номер карточки"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_NOMKAR] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMKAR]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMKAR]), data.entry[E_NOMKAR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMKAR]), "activate",GTK_SIGNAL_FUNC(rashodw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMKAR]),data.nomkar_par.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMKAR]),(gpointer)E_NOMKAR);

sprintf(strsql,"%s",gettext("НДС"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_NDS] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_NDS]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NDS]), data.entry[E_NDS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NDS]), "activate",GTK_SIGNAL_FUNC(rashodw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NDS]),data.nds_par.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NDS]),(gpointer)E_NDS);




GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введённой в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(rashodw_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(vboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Реквизиты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Просмотр нужных реквизитов"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]),"clicked",GTK_SIGNAL_FUNC(rashodw_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_box_pack_start(GTK_BOX(vboxknop), data.knopka[FK3], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(rashodw_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(vboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F5 %s",gettext("Номер карточки"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Получить новый номер карточки"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]),"clicked",GTK_SIGNAL_FUNC(rashodw_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_box_pack_start(GTK_BOX(vboxknop), data.knopka[FK5], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(rashodw_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(vboxknop), data.knopka[FK10], TRUE, TRUE, 0);


gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all(data.window);

if(mdd == 0)
 {
  gtk_widget_hide(data.knopka[FK5]);
  gtk_widget_hide(hbox[E_NDS]);
  gtk_widget_hide(hbox[E_EI]);
  gtk_widget_hide(hbox[E_NOMKAR]);
 }

//Проверка счёта на принадлежность к счетам розничной торговли
rashodw_provshetrt(&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rashodw_key_press(GtkWidget *widget,GdkEventKey *event,class rashodw_data *data)
{

//printf("rashodw_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);

  case GDK_F3:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    return(TRUE);

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
void  rashodw_knopka(GtkWidget *widget,class rashodw_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;
char strsql[512];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(rashodw_zap(data) != 0)
      return;
    data->voz=0;        
    gtk_widget_destroy(data->window);
    data->window=NULL;
    return;  

  case FK3:
    rashodw_rekviz(data);
    return;

  case FK4:
    data->clear_rek();
    return;

  case FK5:
    if(mdd == 0)
      return;
    sprintf(strsql,"%d",nomkrw(data->skl1,data->window));
    data->nomkar_par.new_plus(strsql);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMKAR]),data->nomkar_par.ravno_toutf());
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

void    rashodw_vvod(GtkWidget *widget,class rashodw_data *data)
{
double bb=0.;
double cena=0.;
char strsql[512];
class OPSHET reksh;
iceb_u_str repl;
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//printf("rashodw_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SHET:
    data->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(data->shet.getdlinna() > 1)
     {
      if(iceb_prsh1(data->shet.ravno(),&reksh,data->window) == 0)
       {
        repl.new_plus(gettext("Наименование счёта"));
        repl.plus(": ");
        repl.plus(reksh.naim.ravno());
              
        gtk_label_set_text(GTK_LABEL(data->label_shet),repl.ravno_toutf());
       }
      else  
       gtk_label_set_text(GTK_LABEL(data->label_shet),"");

      //Проверка счёта на принадлежность к счетам рознтчной торговли
      rashodw_provshetrt(data);
     }
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
    if(data->krat_kar != 0. && data->kolih.ravno_atof() != 0)
     {
      bb=data->kolih.ravno_atof()/data->krat_kar;
      sprintf(strsql,"%.10g",bb);
      data->koltar.new_plus(strsql);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOLTAR]),data->koltar.ravno());
     }
     
    break;

  case E_KOLTAR:
    data->koltar.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(data->krat_kar != 0. && data->koltar.ravno_atof() != 0)
     {
      bb=data->koltar.ravno_atof()*data->krat_kar;
      sprintf(strsql,"%.10g",bb);
      data->kolih.new_plus(strsql);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOLIH]),data->kolih.ravno());
     
     }
    break;

  case E_CENAP:
    data->cenap.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_EI:
    data->ei_par.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOMKAR:
    data->nomkar_par.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NDS:
    data->nds_par.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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

/*
  case E_CENAP:
    data->nomz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
*/

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


enter+=1;

if(mdd == 0)
 {
  //Должны быть расположены в тойже последовательности как в меню

  if(enter == E_NOMKAR)
   enter+=1;

  if(enter == E_EI)
   enter+=1;
  
  if(enter == E_NDS)
   enter+=1;

 }

if(enter == E_CENAP  && rashodw_provshetrt(data) != 0)
  enter+=1;
  
if(enter >= KOLENTER)
enter=0;
//printf("rashodw_vvod end enter=%d\n",enter);

gtk_widget_grab_focus(data->entry[enter]);
 
}
/********************/
/*Работа с реквизитами*/
/***********************/
void   rashodw_rekviz(class rashodw_data *data)
{
short nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Реквизиты"));

zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Просмотр списка счетов"));
punkt_m.plus(gettext("Просмотр списка единиц измерения"));

nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);



iceb_u_str kod;
iceb_u_str naikod;
class OPSHET reksh;
switch (nomer)
 {
  case -1:
    return;
    
  case 0:
    if(iceb_vibrek(1,"Plansh",&data->shet,data->window) == 0)
     {
      if(iceb_prsh1(data->shet.ravno(),&reksh,data->window) == 0)
       {
        iceb_u_str repl;
        repl.new_plus(gettext("Наименование счёта"));
        repl.plus(": ");
        repl.plus(reksh.naim.ravno());

        gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet.ravno_toutf());
              
        gtk_label_set_text(GTK_LABEL(data->label_shet),repl.ravno_toutf());
       }
      else  
       gtk_label_set_text(GTK_LABEL(data->label_shet),"");
     }
    break;

  case 1:
    if(iceb_vibrek(1,"Edizmer",&data->ei_par,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),data->ei_par.ravno_toutf());
    break;

 }

}
/***************************************************************/
/*Проверка счёта на принадлежность к счетам розничной торговли*/
/***************************************************************/
//Если вернули 0-цена продажи должна вводиться 1-нет
int  rashodw_provshetrt(class rashodw_data *data)
{
int voz=1;

if(mdd == 1)
  if(iceb_u_proverka(shrt,data->shet.ravno(),0,1) == 0)
   {
    gtk_widget_show_all(data->hbox_cenap);
    voz=0;
   } 

if(iceb_u_proverka(shrt,data->shet.ravno(),0,1) != 0)
 {
  gtk_widget_hide(data->hbox_cenap);
  gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENAP]),"");
  data->cenap.new_plus("");
  voz=1;
 }
return(voz);

}
/***********************************/
/*Запись меню                      */
/************************************/
int  rashodw_zap(class rashodw_data *data)
{
iceb_u_str repl;
OPSHET shetv;

if(data->kolih.ravno_atof() == 0.)
 {
  repl.new_plus(gettext("Не введено количество"));
  repl.plus(" !");
  iceb_menu_soob(&repl,data->window);
  return(1);
 }

if(data->cena.getdlinna() <= 1)
 {
  repl.new_plus(gettext("Не введена цена !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 }

double cenav=0.;

if(rashodw_provshetrt(data) == 0 && data->cenap.ravno_atof() == 0.)
 {
  repl.new_plus(gettext("Не введена цена реализации !"));
  iceb_menu_soob(&repl,data->window);
  return(1);

  cenav=data->cena.ravno_atof();
  cenav=iceb_u_okrug(cenav,okrcn);
 }

if(mdd == 0) //Одинарный документ
 {
   /*Проверяем счет*/
  if(data->shet.getdlinna() > 1)
   {
    if(iceb_prsh1(data->shet.ravno(),&shetv,data->window) != 0)
     return(1);
   }
 }

if(mdd == 1) /*Двойной документ*/
 {
  if(data->ei_par.getdlinna() <= 1 || data->nds_par.getdlinna() <= 1 || data->nomkar_par.getdlinna() <= 1)
   {
    repl.new_plus(gettext("Не введены все обязательные реквизиты !"));
    iceb_menu_soob(&repl,data->window);
    return(1);
   }

   if(data->skl == data->skl1)
    if(data->cena_kar == data->cena.ravno_atof() && 
    iceb_u_SRAV(data->shet_kar.ravno(),data->shet.ravno(),0) == 0 &&
    iceb_u_SRAV(data->ei_kar.ravno(),data->ei_par.ravno(),0) == 0  && fabs(data->cenap_z-data->cenap.ravno_atof()) < 0.01)
      {

       repl.new_plus(gettext("Перемещение внутри склада. Не изменен ни один реквизит !"));
       iceb_menu_soob(&repl,data->window);
       return(1);
      }
   if(data->nomkar_par.ravno_atoi() < 0)
    {
     iceb_menu_soob(gettext("Номер карточки не может быть отрицательным !"),data->window);
     return(1);
    }
   /*Проверяем счет*/
   if(iceb_prsh1(data->shet.ravno(),&shetv,data->window) != 0)
     return(1);

 }


double cena=data->cena.ravno_atof();
cena=iceb_u_okrug(cena,okrcn);

class ostatok ost_kar;
class ostatok ost_dok;

short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

/*Остаток по карточкам*/
ostkarw(1,1,gt,31,mt,gt,data->skl,data->n_kart,&ost_kar);

/*Остаток по документам*/
ostdokw(1,1,gt,31,mt,gt,data->skl,data->n_kart,&ost_dok);

char strsql[512];
           
if(ost_dok.ostg[3] - data->kolih.ravno_atof() < -0.0001 || ost_kar.ostg[3]-data->kolih.ravno_atof() < -0.0001)
 {
  iceb_u_spisok repl_s;
  

  repl_s.plus(gettext("Расход больше остатка на карточке !"));

  sprintf(strsql,"%s:%.10g %s:%.10g",gettext("Остаток на карточке"),ost_kar.ostg[3],
  gettext("Остаток по документам"),ost_dok.ostg[3]);

  repl_s.plus(strsql);  

  memset(strsql,'\0',sizeof(strsql));
  if(iceb_poldan("Запрет привязки к карточкам с недостаточным остатком для списания",strsql,"matnast.alx",data->window) == 0)
   if(iceb_u_SRAV(strsql,"Вкл",1) == 0)
    {
     iceb_menu_soob(&repl_s,data->window);
     return(1);
    }
  
  repl_s.plus(gettext("Ввести расход ?"));

  if(iceb_menu_danet(&repl_s,2,data->window) == 2)
   return(1);
 }
SQL_str row;
SQLCURSOR cur;
float nds=data->nds_par.ravno_atof();
nds=iceb_u_okrug(nds,0.01);

/*Внутреннее перемещение*/
if(mdd == 1)
 {
  
  short mndsv=0;
  if(data->nds_par.ravno()[0] == '+')
    mndsv=1;
  int nkv=data->nomkar_par.ravno_atoi();

  double kolihpr=data->kolih.ravno_atof();

     
 /*Проверяем может в выходном документе уже есть этот материал
  с такойже ценой. Если есть то просто увеличиваем количество*/
  if(data->innom_kar.getdlinna() <= 1)
   {        
    sprintf(strsql,"select nomkar from Dokummat1 where \
datd='%04d-%d-%d' and sklad=%d and nomd='%s' and kodm=%d and cena=%.10g",
    data->gd,data->md,data->dd,data->skl1,data->nomon.ravno(),data->kodm,cena);
   
   if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    {
     int nomkarp=atoi(row[0]);
     //Проверяем совпадают ли счета учета
     sprintf(strsql,"select shetu from Kart where nomk=%d and \
sklad=%d",nomkarp,data->skl1);
     if(sql_readkey(&bd,strsql,&row,&cur) == 1)
      {
       if(iceb_u_SRAV(data->shet.ravno(),row[0],0) != 0)
         goto vpered;
      }

     repl.new_plus(gettext("В выходном документе уже есть этот материал !"));
     repl.ps_plus(gettext("Увеличиваем его количество"));
     iceb_menu_soob(&repl,data->window);
          
     sprintf(strsql,"update Dokummat1 set \
kolih=kolih+%.10g where datd='%04d-%d-%d' and sklad=%d and nomd='%s' and kodm=%d and cena=%.10g",
     data->kolih.ravno_atof(),data->gd,data->md,data->dd,data->skl1,data->nomon.ravno(),data->kodm,cena);
     if(sql_zap(&bd,strsql) != 0)
       iceb_msql_error(&bd,"rashodw-Корректировки записи !",strsql,data->window);

     /*Делаем запись в расходный документ*/
     zapvdokw(data->dd,data->md,data->gd,data->skl,data->n_kart,data->kodm,data->nomdok.ravno(),
     data->kolih.ravno_atof(),data->cena_kar,data->ei_kar.ravno(),data->nds_kar,data->mnds_kar,
     0,2,nomkarp,data->shet.ravno(),data->nomz_kar.ravno(),data->window);
     return(0);
    }
   }

vpered:;

  if(iceb_u_SRAV(data->ei_kar.ravno(),data->ei_par.ravno(),0) != 0 )
   {
    repl.new_plus(gettext("Изменена единица измерения"));
    repl.ps_plus(gettext("Введите количество для приходного документа"));
    memset(strsql,'\0',sizeof(strsql));
    iceb_u_str nkol;
    if(iceb_menu_vvod1(&repl,&nkol,20,data->window) != 0)
     return(1);
    kolihpr=nkol.ravno_atof();        
   }


  repl.new_plus(gettext("Внутреннее перемещение"));
  
  /*Проверяем есть ли карточка с такой ценой
  если есть узнаем ее номер, если нет определяем
  последний номер карточки по этому складу*/

  //Округляем также как при записи
  data->krat_kar=iceb_u_okrug(data->krat_kar,0.000001);

  if(data->innom_kar.getdlinna() > 1 || data->nomz_kar.getdlinna() > 1)
   goto zapkr;
   
  sprintf(strsql,"select nomk from Kart where kodm=%d and \
sklad=%d and shetu='%s' and cena=%.10g and ei='%s' and nds=%.6g \
and cenap=%.10g and krat=%.6g and mnds=%d and dat_god='%04d-%02d-%02d'",
  data->kodm,data->skl1,data->shet.ravno(),cena,data->ei_par.ravno(),nds,cenav,data->krat_kar,mndsv,
  data->gi,data->mi,data->di);


  if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
   {
    nkv=atoi(row[0]);
    sprintf(strsql,gettext("Такой материал есть в карточке %d, записываем туда"),nkv);
    repl.ps_plus(strsql);
   }
  else
   {
    /*Проверяем нет ли уже карточки с таким номером*/
    sprintf(strsql,"select nomk from Kart where sklad=%d \
and nomk=%d",data->skl1,data->nomkar_par.ravno_atoi());
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     {
      sprintf(strsql,gettext("Карточка %s есть на складе %d с другим материалом !"),
      data->nomkar_par.ravno(),data->skl1);
      repl.ps_plus(strsql);
      iceb_menu_soob(&repl,data->window);
      return(1);
     }

zapkr:;
    sprintf(strsql,gettext("Делаем запись в карточку %d на складе %d"),nkv,data->skl1);
    repl.ps_plus(strsql);
    
    if(zapkarw(data->skl1,nkv,data->kodm,cena,data->ei_par.ravno(),data->shet.ravno(),data->krat_kar,
    data->fas_kar,data->kodt_kar,nds,mndsv,cenav,data->denv,data->mesv,data->godv,data->innom_kar.ravno(),
    data->rnd_kar.ravno(),data->nomz_kar.ravno(),data->di,data->mi,data->gi,0,data->window) != 0)
      return(1);
   }

 repl.ps_plus(gettext("Номер парного документа"));
 repl.plus(":");
 repl.plus(data->nomon.ravno());
 
 iceb_menu_soob(&repl,data->window);
 
 /*Делаем запись в расходный документ*/
 zapvdokw(data->dd,data->md,data->gd,data->skl,data->n_kart,data->kodm,data->nomdok.ravno(),
 data->kolih.ravno_atof(),data->cena_kar,data->ei_kar.ravno(),data->nds_kar,data->mnds_kar,0,2,nkv,data->shet.ravno(),
 data->nomz_kar.ravno(),data->window);

 /*Делаем запись в приходный документ*/
  zapvdokw(data->dd,data->md,data->gd,data->skl1,nkv,data->kodm,data->nomon.ravno(),
  kolihpr,cena,data->ei_par.ravno(),nds,mndsv,0,1,data->n_kart,data->shet_kar.ravno(),data->nomz_kar.ravno(),data->window);
 }
else
 {
  zapvdokw(data->dd,data->md,data->gd,data->skl,data->n_kart,data->kodm,data->nomdok.ravno(),
  data->kolih.ravno_atof(),cena,data->ei_kar.ravno(),data->nds_kar,data->mnds_kar,0,2,0,data->shet.ravno(),
  data->nomz_kar.ravno(),data->window);
 }



return(0);

}
