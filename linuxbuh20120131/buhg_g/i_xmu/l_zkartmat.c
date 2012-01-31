/*$Id: l_zkartmat.c,v 1.56 2011-02-21 07:35:55 sasa Exp $*/
/*19.05.2010	19.05.2004	Белых А.И.	l_zkartmat.c
Промотр списка записей в карточке определенного материалла
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "l_zkartmat.h"

enum
{
 FK2,
 FK3,
 FK4,
 FK5,
 FK6,
 FK7,
 FK8,
 FK10,
 KOL_F_KL
};

enum
{
 COL_DATAP,
 COL_DATADOK,
 COL_NOMDOK,
 COL_KODOP,
 COL_KONTR,
 COL_KOLIH,
 COL_PR,  
 COL_CENA,
 COL_DOPNAIM,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  zkartmat_data
 {
  public:

  iceb_u_str datapv;
  iceb_u_str datadv;
  iceb_u_str nomdokv;
  iceb_u_str tipv;
  double     kolihv;
  iceb_u_str kodopv;


  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *label_ost;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  
  short     metka_dpros; //0-от стартового года до текущей даты 1-за текущий месяц
    
  rek_kartmat rk; //Реквизиты карточки материалла
  
  short      metka_viv_ost; //0-Выдать остатки по карточкам 1-по документам
  short      dn,mn,gn;
  short      dk,mk,gk; //дата на которую вычисляем остаток
  zkartmat_poi poi;
  iceb_u_str zapros;
  iceb_u_str zagolov;
  iceb_u_str zagolov_poi;
  short dpn,mpn,gpn;
          
  //Конструктор
  zkartmat_data()
   {
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
    metka_viv_ost=0;
    metka_dpros=0;
   }      
 };

gboolean   zkartmat_key_press(GtkWidget *widget,GdkEventKey *event,class zkartmat_data *data);
void zkartmat_vibor(GtkTreeSelection *selection,class zkartmat_data *data);
void zkartmat_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class zkartmat_data *data);
void  zkartmat_knopka(GtkWidget *widget,class zkartmat_data *data);
void zkartmat_add_columns (GtkTreeView *treeview);
void zkartmat_create_list(class zkartmat_data *data);

int zkartmat_prow_row(SQL_str row,class zkartmat_poi *data,GtkWidget *wpredok);

int l_zkartmat_p(class zkartmat_poi *datap,GtkWidget *wpredok);
void vivostw(int,short dn,short mn,short gn,short dk,short mk,short gk,iceb_u_str *ost_str,class ostatok *data);
void  zkartmat_ras(class zkartmat_data *data);
int zkartmat_uz(int metka_u,class zkartmat_data *data);
int l_zkartmat_v(class rek_kartmat *data_kar,GtkWidget *wpredok);
void  zkartmat_sliqnie(const char*,const char*,const char*,GtkWidget*);

int  l_zkartmat1(const char *skl,const char *n_kart,GtkWidget *wpredok);

extern SQL_baza	bd;
extern char *name_system;
extern short	startgod; /*Стартовый год просмотров*/
extern short	startgod; /*Стартовый год просмотров*/
extern char     *organ;
extern double	okrcn;  /*Округление цены*/
extern iceb_u_str shrift_ravnohir;

int  l_zkartmat(int skl,int n_kart,GtkWidget *wpredok)
 {
  char sklad[40];
  char nomkar[40];
  sprintf(sklad,"%d",skl);
  sprintf(nomkar,"%d",n_kart);
  return(l_zkartmat1(sklad,nomkar,wpredok));
 }
int  l_zkartmat(const char *skl,const char *n_kart,GtkWidget *wpredok)
 {
  return(l_zkartmat1(skl,n_kart,wpredok));
 }

int  l_zkartmat1(const char *skl,const char *n_kart,GtkWidget *wpredok)
{
zkartmat_data data;
char strsql[512];
char bros[512];
GdkColor color;
SQL_str row;
SQLCURSOR cur;
iceb_u_str soob;
short ddd,mmm,ggg;
iceb_u_poltekdat(&ddd,&mmm,&ggg);


//Чтаем наименование склада
sprintf(strsql,"select naik from Sklad where kod=%s",skl);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найден код склада"));
  repl.plus(" ");
  repl.plus(skl);
  repl.plus(" !");
  iceb_menu_soob(&repl,wpredok);
  data.rk.naimskl.new_plus("");
 }
else
  data.rk.naimskl.new_plus(row[0]);

//читаем реквизиты карточки
sprintf(strsql,"select * from Kart where sklad=%s and nomk=%s",skl,n_kart);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найдена карточка"));
  repl.plus(" ");
  repl.plus(n_kart);
  repl.ps_plus(gettext("Склад"));
  repl.plus(skl);
  iceb_menu_soob(&repl,wpredok);
  return(1);
   
 }
data.rk.kodm.plus(row[2]);
data.rk.skl.plus(skl);
data.rk.n_kart.plus(n_kart);
data.rk.shet.plus(row[5]);
data.rk.cena.plus(row[6]);
data.rk.ei.plus(row[4]);
data.rk.nds.plus(row[9]);
data.rk.krat.plus(row[8]);
data.rk.cenapr.plus(row[7]);
data.rk.innom.plus(row[15]);
data.rk.rnd.plus(row[16]);
data.rk.nomzak.plus(row[17]);
data.rk.mnds.plus(row[3]);
data.rk.fas.plus(row[10]);
data.rk.kodtar.plus(row[11]);
short d,m,g;
if(iceb_u_rsdat(&d,&m,&g,row[14],2) == 0)
 {
  sprintf(strsql,"%d.%d.%d",d,m,g);
  data.rk.datv.plus(strsql);
 }
else
  data.rk.datv.plus("");

data.rk.data_kon_is.plus_sqldata(row[18]);

data.dn=1;
data.mn=1;
data.gn=startgod;
if(startgod == 0)
  data.gn=ggg;
  
data.dk=ddd;
data.mk=mmm;
data.gk=ggg;

sprintf(strsql,"%d.%d.%d",data.dn,data.mn,data.gn);
data.poi.datan.new_plus(strsql);

sprintf(strsql,"%d.%d.%d",data.dk,data.mk,data.gk);
data.poi.datak.new_plus(strsql);

//Читаем наименование материалла
sprintf(strsql,"select kodgr,naimat from Material where kodm=%s",data.rk.kodm.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  data.rk.kodgr.plus(row[0]);
  data.rk.naimat.plus(row[1]);
 }
else 
 {
  data.rk.kodgr.plus("");
  data.rk.naimat.plus("");
 }

//читаем наименование группы
sprintf(strsql,"select naik from Grup where kod=%s",data.rk.kodgr.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 data.rk.naimgr.plus(row[0]);
else 
 data.rk.naimgr.plus("");

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Просмотр карточки"));

gtk_window_set_title (GTK_WINDOW (data.window),bros);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zkartmat_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

soob.new_plus(gettext("Просмотр карточки"));
soob.ps_plus(data.rk.kodm.ravno());
soob.plus(" ");
soob.plus(data.rk.naimat.ravno());

data.label_kolstr=gtk_label_new (soob.ravno_toutf());


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");
data.label_ost=gtk_label_new ("");

PangoFontDescription *font_pango=pango_font_description_from_string(shrift_ravnohir.ravno());
gtk_widget_modify_font(GTK_WIDGET(data.label_ost),font_pango);
pango_font_description_free(font_pango);


gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_poisk,GTK_STATE_NORMAL,&color);

gtk_box_pack_start (GTK_BOX (vbox2),data.label_ost,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),-1,200);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];


sprintf(bros,"F2 %s",gettext("Корректировать"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(zkartmat_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Корректировка реквизитов карточки материалла"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(zkartmat_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(zkartmat_knopka),&data);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(zkartmat_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Смена даты"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(zkartmat_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Включение/выключение просмотра записей за текущий месяц"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F7 %s",gettext("Остаток"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(zkartmat_knopka),&data);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Показать остатки по карточке или по записям в документах"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"F8 %s",gettext("Слияние"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]), "clicked",GTK_SIGNAL_FUNC(zkartmat_knopka),&data);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Слияние записей из нужной карточки в текущюю с удалением первой"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_widget_show(data.knopka[FK8]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(zkartmat_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

zkartmat_create_list(&data);

//gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));
gtk_widget_show(data.window);


gtk_main();


printf("l_zkartmat end\n");
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.metka_voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void zkartmat_create_list (class zkartmat_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

class iceb_clock sss(data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[1024];
int  kolstr=0;
SQL_str row;
SQL_str row1;
short dpk,mpk,gpk;
iceb_u_str zagolov;

//printf("zkartmat_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(zkartmat_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(zkartmat_vibor),data);

gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (data->treeview), TRUE); //Устанавливаются полоски при отображении списка
gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);


if(data->poi.metka_poi == 0)
 {
  data->gpn=data->gn;
  data->mpn=data->mn;
  data->dpn=data->dn;

  gpk=data->gk;
  mpk=data->mk;
  dpk=data->dk;
 }
if(data->poi.metka_poi == 1)
 {
  iceb_rsdatp(&data->dpn,&data->mpn,&data->gpn,data->poi.datan.ravno(),&dpk,&mpk,&gpk,data->poi.datak.ravno(),data->window);
 }

ostatok data_ost;
ostatok data_ostdok;

if(data->metka_viv_ost == 0)
 {
  ostkarw(data->dpn,data->mpn,data->gpn,dpk,mpk,gpk,data->rk.skl.ravno(),data->rk.n_kart.ravno(),&data_ost);
  vivostw(0,data->dpn,data->mpn,data->gpn,dpk,mpk,gpk,&zagolov,&data_ost);
 }
if(data->metka_viv_ost == 1)
 {
  ostdokw(data->dpn,data->mpn,data->gpn,dpk,mpk,gpk,data->rk.skl.ravno(),data->rk.n_kart.ravno(),&data_ostdok);
  vivostw(1,data->dpn,data->mpn,data->gpn,dpk,mpk,gpk,&zagolov,&data_ostdok);
 }
     
gtk_label_set_text(GTK_LABEL(data->label_ost),zagolov.ravno_toutf());
gtk_widget_show(data->label_ost);

sprintf(strsql,"select * from Zkart where sklad=%s and nomk=%s and \
datdp >= '%04d-%02d-%02d' and datdp <= '%04d-%02d-%02d' order by datdp asc",
data->rk.skl.ravno(),data->rk.n_kart.ravno(),data->gpn,data->mpn,data->dpn,gpk,mpk,dpk);

data->zapros.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
float kolstr1=0.;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  if( zkartmat_prow_row(row,&data->poi,data->window) != 0)
    continue;



  //Дата подтверждения
  ss[COL_DATAP].new_plus(iceb_u_sqldata(row[3]));

  //Дата документа
  ss[COL_DATADOK].new_plus(iceb_u_sqldata(row[4]));

  //Номер документа
  ss[COL_NOMDOK].new_plus(iceb_u_toutf(row[2]));

  //читаем шапку документа
  sprintf(strsql,"select kodop,kontr from Dokummat where datd='%s' and sklad=%s and nomd='%s' \
and tip=%s",
  row[4],data->rk.skl.ravno(),row[2],row[5]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) > 0)
   {
    //Код операции
    ss[COL_KODOP].new_plus(iceb_u_toutf(row1[0]));

    //Код контрагента
    ss[COL_KONTR].new_plus(iceb_u_toutf(row1[1]));
   }
  else
   { 
    iceb_u_str repl;
    repl.plus(gettext("Не найден документ"));
    repl.plus(" !!!");
    repl.ps_plus(row[4]);
    repl.plus(" ");
    repl.plus(data->rk.skl.ravno());
    repl.plus(" ");
    repl.plus(row[2]);
    repl.plus(" ");
    repl.plus(row[5]);
    iceb_menu_soob(&repl,data->window);
    
    //Код операции
    ss[COL_KODOP].new_plus(iceb_u_toutf(" "));

    //Код контрагента
    ss[COL_KONTR].new_plus(iceb_u_toutf(" "));
   }
  //количество
  sprintf(strsql,"%.10g",atof(row[6]));
  ss[COL_KOLIH].new_plus(strsql);

  //приход/расход
  if(row[5][0] == '1')
    ss[COL_PR].new_plus("+");
  else
    ss[COL_PR].new_plus("-");

  //цена
  sprintf(strsql,"%.10g",atof(row[7]));
  ss[COL_CENA].new_plus(strsql);
    
  ss[COL_DOPNAIM].new_plus("");

  sprintf(strsql,"select dnaim from Dokummat1 where sklad=%s and \
nomd='%s' and kodm=%s and nomkar=%s and datd='%s'",
  data->rk.skl.ravno(),row[2],data->rk.kodm.ravno(),row[1],row[4]);


  if(sql_readkey(&bd,strsql,&row1,&cur1) > 0)
    ss[COL_DOPNAIM].new_plus(iceb_u_toutf(row1[0]));
  

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[9])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[8],0,data->window));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_DATAP,ss[COL_DATAP].ravno(),
  COL_DATADOK,ss[COL_DATADOK].ravno(),
  COL_NOMDOK,ss[COL_NOMDOK].ravno(),
  COL_KODOP,ss[COL_KODOP].ravno(),
  COL_KONTR,ss[COL_KONTR].ravno(),
  COL_KOLIH,ss[COL_KOLIH].ravno(),
  COL_PR,ss[COL_PR].ravno(),
  COL_CENA,ss[COL_CENA].ravno(),
  COL_DOPNAIM,ss[COL_DOPNAIM].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

zkartmat_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str stroka;

data->zagolov.new_plus(gettext("Склад"));
data->zagolov.plus(" N");
data->zagolov.plus(data->rk.skl.ravno());
data->zagolov.plus(" ");
data->zagolov.plus(data->rk.naimskl.ravno());

data->zagolov.ps_plus(gettext("Просмотр карточки"));
data->zagolov.plus(" N");
data->zagolov.plus(data->rk.n_kart.ravno());

//data->zagolov.plus(" ");

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
data->zagolov.plus(strsql);

if(data->poi.metka_poi == 0)
 {
   gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна

  data->zagolov.plus(" ");
  data->zagolov.plus(gettext("Просмотр"));
  data->zagolov.plus(" ");

  sprintf(strsql,"%d.%d.%d",data->dn,data->mn,data->gn);
  data->zagolov.plus(strsql);
  data->zagolov.plus(" => ");
  sprintf(strsql,"%d.%d.%d",data->dk,data->mk,data->gk);
  data->zagolov.plus(strsql);
 }

data->zagolov.ps_plus(data->rk.kodm.ravno());
data->zagolov.plus(" ");
data->zagolov.plus(data->rk.naimat.ravno());
if(data->rk.data_kon_is.getdlinna() > 1)
 {
  data->zagolov.ps_plus(gettext("Конечная дата использования"));
  data->zagolov.plus(":");
  data->zagolov.plus(data->rk.data_kon_is.ravno());
  
 }
data->zagolov.ps_plus(gettext("Группа"));
data->zagolov.plus(" N");
data->zagolov.plus(data->rk.kodgr.ravno());
data->zagolov.plus(" ");
data->zagolov.plus(data->rk.naimgr.ravno());

sprintf(strsql,"%s:%s",gettext("Счет"),data->rk.shet.ravno());
data->zagolov.ps_plus(strsql);

sprintf(strsql," %s:%.10g",gettext("Цена"),data->rk.cena.ravno_atof());
data->zagolov.plus(strsql);

sprintf(strsql," %s:%s",gettext("Е/и"),data->rk.ei.ravno());
data->zagolov.plus(strsql);

sprintf(strsql," %s:%s",gettext("НДС"),data->rk.nds.ravno());
data->zagolov.plus(strsql);

sprintf(strsql," %s:%s",gettext("Кратность"),data->rk.krat.ravno());
data->zagolov.plus(strsql);

if(data->rk.nomzak.getdlinna() > 1)
 {
  sprintf(strsql," %s:%s",gettext("Номер заказа"),data->rk.nomzak.ravno());
  data->zagolov.plus(strsql);
 }

if(data->rk.innom.getdlinna() > 1)
 {
  if(data->rk.rnd.getdlinna() > 1)
   sprintf(strsql," %s:%s",gettext("Серия"),data->rk.innom.ravno());
  else
   sprintf(strsql," %s:%s",gettext("Инв-ный номер"),data->rk.innom.ravno());
  data->zagolov.plus(strsql);
 }

if(data->rk.rnd.getdlinna() > 1)
 {
  sprintf(strsql," %s:%s",gettext("Рег.N"),data->rk.rnd.ravno());
  data->zagolov.plus(strsql);
 }

gtk_label_set_text(GTK_LABEL(data->label_kolstr),data->zagolov.ravno_toutf());


if(data->poi.metka_poi == 1 )
 {

   //При поиске это не работает
   gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
// printf("Формирование заголовка с реквизитами поиска.\n");
  
  data->zagolov_poi.new_plus(gettext("Поиск"));
  data->zagolov_poi.plus(" !!!");

  iceb_str_poisk(&data->zagolov_poi,data->poi.datan.ravno(),gettext("Дата начала"));
  iceb_str_poisk(&data->zagolov_poi,data->poi.datak.ravno(),gettext("Дата конца"));
  iceb_str_poisk(&data->zagolov_poi,data->poi.kontr.ravno(),gettext("Код контрагента"));
  iceb_str_poisk(&data->zagolov_poi,data->poi.kodop.ravno(),gettext("Код операции"));
  iceb_str_poisk(&data->zagolov_poi,data->poi.nomdok.ravno(),gettext("Номер документа"));
  
  if(data->poi.pri_ras == 1)
   {
    data->zagolov_poi.ps_plus(gettext("Приход/расход"));
    data->zagolov_poi.plus(": +");
   }
  if(data->poi.pri_ras == 2)
   {
    data->zagolov_poi.ps_plus(gettext("Приход/расход"));
    data->zagolov_poi.plus(": -");
   }
   
  gtk_label_set_text(GTK_LABEL(data->label_poisk),data->zagolov_poi.ravno_toutf());
  gtk_widget_show(data->label_poisk);
 }
else
 {
  gtk_widget_hide(data->label_poisk); 
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
 }

gtk_widget_show(data->label_kolstr);


}

/*****************/
/*Создаем колонки*/
/*****************/

void zkartmat_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

printf("zkartmat_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата подт-ния"), renderer,"text", COL_DATAP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата док-та"), renderer,"text", COL_DATADOK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Номер документа"), renderer,"text", COL_NOMDOK,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Операция"), renderer,"text", COL_KODOP,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Контрагент"), renderer,"text", COL_KONTR,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Количество"), renderer,"text", COL_KOLIH,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,"+\\-", renderer,"text", COL_PR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Цена"), renderer,"text", COL_CENA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дополнительное наименование"), renderer,
"text", COL_DOPNAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

printf("zkartmat_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void zkartmat_vibor(GtkTreeSelection *selection,class zkartmat_data *data)
{
printf("zkartmat_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gchar *naim;
gchar *tip;
gint  nomer;
gchar *kolih;
gchar *datadv;
gchar *kodop;

gtk_tree_model_get(model,&iter,COL_DATAP,&kod,COL_NOMDOK,&naim,COL_KOLIH,&kolih,
COL_PR,&tip,COL_DATADOK,&datadv,COL_KODOP,&kodop,NUM_COLUMNS,&nomer,-1);

data->datapv.new_plus(kod);
data->kodopv.new_plus(iceb_u_fromutf(kodop));
data->datadv.new_plus(datadv);
data->nomdokv.new_plus(iceb_u_fromutf(naim));
data->snanomer=nomer;
data->tipv.new_plus(tip);
data->kolihv=atof(kolih);

g_free(kod);
g_free(tip);
g_free(naim);
g_free(datadv);
g_free(kolih);
g_free(kodop);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zkartmat_knopka(GtkWidget *widget,class zkartmat_data *data)
{
iceb_u_str repl;
iceb_u_str vdata;
short ddd,mmm,ggg;
iceb_u_poltekdat(&ddd,&mmm,&ggg);

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("zkartmat_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case FK2:
    l_zkartmat_v(&data->rk,data->window);
    zkartmat_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    zkartmat_uz(0,data);
    zkartmat_create_list(data);
    return;  

  case FK4:
    l_zkartmat_p(&data->poi,data->window);
    zkartmat_create_list(data);
    return;  

  case FK5:
    zkartmat_ras(data);
    return;  

  case FK6:
    if(data->poi.metka_poi == 1)
     return;
     
    data->metka_dpros++;
    if(data->metka_dpros == 2)
     data->metka_dpros=0;

    if(data->metka_dpros == 0)
     {
      data->dn=1;
      data->mn=1;
      data->gn=startgod;
      if(startgod == 0)
      data->gn=ggg;
     }
    if(data->metka_dpros == 1)
     {
      data->dn=1;
      data->mn=mmm;
      data->gn=ggg;
     }

    zkartmat_create_list(data);
    return;  

  case FK7:
    data->metka_viv_ost++;
    if(data->metka_viv_ost == 2)
     data->metka_viv_ost=0;
    zkartmat_create_list(data);

    return;  

  case FK8:
    zkartmat_sliqnie(data->rk.skl.ravno(),data->rk.n_kart.ravno(),data->rk.kodm.ravno(),data->window);
    zkartmat_create_list(data);
    return;  

    
  case FK10:
//    printf("zkartmat_knopka F10\n");
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zkartmat_key_press(GtkWidget *widget,GdkEventKey *event,class zkartmat_data *data)
{
iceb_u_str repl;
//printf("zkartmat_key_press keyval=%d state=%d\n",event->keyval,event->state);

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

  case GDK_F6:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
    return(TRUE);

  case GDK_F7:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK7]),"clicked");
    return(TRUE);
  
  case GDK_F8:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK8]),"clicked");
    return(TRUE);

  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("zkartmat_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/****************************/
/*Выбор строки*/
/**********************/
void zkartmat_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class zkartmat_data *data)
{
//printf("zkartmat_v_row\n");
//printf("zkartmat_v_row корректировка\n");

//gtk_widget_destroy(data->window);

//data->metka_voz=0;

}
/*****************************/
/*Удаление записи*/
/*****************************/
//Если вернули 0-удалили 1-нет
int zkartmat_uz(int metka_u, //0-удаление с проверкой остатка 1-без проверки
class zkartmat_data *data)
{
short d,m,g;
iceb_u_str stroka;

iceb_u_rsdat(&d,&m,&g,data->datapv.ravno(),1);

if(iceb_pbpds(m,g,data->window) != 0)
  return(1);
ostatok ostk;



if(data->tipv.ravno()[0] == '+')
 if(metka_u == 0)   
  {
    ostkarw(data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,data->rk.skl.ravno(),data->rk.n_kart.ravno(),&ostk);
    if(ostk.ostg[3]-data->kolihv < -0.0000000001)
     {
      iceb_u_str repl;
      repl.plus(gettext("Отрицательный остаток ! Удаление невозможно !"));
      iceb_menu_soob(&repl,data->window);
      return(1);
     }
  }
iceb_u_spisok stroka1;

stroka.new_plus(gettext("Внимание"));
stroka.plus(" !!!");
stroka1.plus(stroka.ravno());

stroka1.plus(gettext("Документ будет помечен, как не подтвержденный и без проводок !"));
stroka1.plus(gettext("Удалить запись ? Вы уверены ?"));

if(iceb_menu_danet(&stroka1,2,data->window) == 2)
 return(1);

/*Удаляем запись в карточке*/
char strsql[512];

sprintf(strsql,"delete from Zkart where sklad=%s and nomk=%s \
and nomd='%s' and datdp='%d-%02d-%02d'",data->rk.skl.ravno(),data->rk.n_kart.ravno(),
data->nomdokv.ravno(),g,m,d);
/*
printf("\nstrsql=%s\n",strsql);
   */

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR)
  {
   stroka.new_plus(gettext("У вас нет полномочий для выполнения этой операции !"));
   iceb_menu_soob(&stroka,data->window);
   return(1); 
  }
 else
  iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
 }

iceb_u_rsdat(&d,&m,&g,data->datadv.ravno(),1);



podvdokw(d,m,g,data->nomdokv.ravno(),data->rk.skl.ravno_atoi(),data->window);

if(data->tipv.ravno()[0] == '+')
if(data->kodopv.getdlinna() > 1)
 {
  SQL_str row1;
  SQLCURSOR cur;
  sprintf(strsql,"select vido from Prihod where kod='%s'",
  data->kodopv.ravno());
  if(sql_readkey(&bd,strsql,&row1,&cur) != 1)
   {
    stroka.new_plus(gettext("Не найден код операции"));
    stroka.plus(" ");
    stroka.plus(data->kodopv.ravno());
    stroka.plus(" !");
    iceb_menu_soob(&stroka,data->window);
    return(0);    
   }
 /*Если операция внутреннего перемещения или изменения
   стоимости то проверять проводки не надо для прихода*/
  if(row1[0][0] != '0')
    return(0);    
 }


if(iceb_u_SRAV(data->nomdokv.ravno(),"000",0) != 0 )
  prosprw(3,data->rk.skl.ravno_atoi(),d,m,g,data->nomdokv.ravno(),
  data->tipv.ravno_atoi(),0,0,"",data->kodopv.ravno(),NULL,data->window);


return(0); 

}
/******************************/
/*Проверка строки на условия поиска*/
/***********************************/

int zkartmat_prow_row(SQL_str row,class zkartmat_poi *data,GtkWidget *wpredok)
{


if(data->metka_poi == 0)
 return(0);


//Поиск образца в строке
if(iceb_u_proverka(data->nomdok.ravno(),row[2],0,0) != 0)
  return(1);

if(data->pri_ras == 1)
 if(row[5][0] != '1')
  return(1);

if(data->pri_ras == 2)
 if(row[5][0] != '2')
  return(1);
 
if(data->kontr.getdlinna() > 1 || data->kodop.getdlinna() > 1)
 {
  SQL_str row1;
  SQLCURSOR cur;
  char strsql[512];
  sprintf(strsql,"select kontr,kodop from Dokummat where datd='%s' and \
sklad=%s and nomd='%s'",row[4],row[0],row[2]);
  if(iceb_sql_readkey(strsql,&row1,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена шапка документа !"));
    repl.ps_plus(row[4]);
    repl.plus(" ");
    repl.plus(row[0]);
    repl.plus(" ");
    repl.plus(row[2]);
    
    iceb_menu_soob(&repl,wpredok);

    return(1);    
   }
  //Поиск образца в строке
  if(iceb_u_proverka(data->kontr.ravno(),row1[0],0,0) != 0)
    return(1);

  //Поиск образца в строке
  if(iceb_u_proverka(data->kodop.ravno(),row1[1],0,0) != 0)
    return(1);


 }
return(0);
}
/***********************************/
/*Формирование строки с остатками*/
/*********************************/

void vivostw(int metka,short dn,short mn,short gn,
short dk,short mk,short gk,
iceb_u_str *ost_str,class ostatok *data)
{
char stroka[1024];
char str1[512];
char str2[512];

short godn=startgod;
if(godn == 0)
 godn=gn;
if(gn < startgod)
  godn=gn; 

if(metka == 0)
  ost_str->new_plus(gettext("Остаток на карточке"));
if(metka == 1)
  ost_str->new_plus(gettext("Остаток по документам"));
ost_str->plus(":");

sprintf(str1,"%s %02d.%02d.%d",gettext("Остаток на"),1,1,godn);
sprintf(str2,"%s %02d.%02d.%d",gettext("Остаток на"),dk,mk,gk);

sprintf(stroka,"\
%-*s: %10.10g %10.2f %02d.%02d.%d %10.10g %10.2f\n\
%-*s: %10.10g %10.2f %10s %10.10g %10.2f\n\
%-*s: %10.10g %10.2f %10s %10.10g %10.2f\n\
%-*s: %10.10g %10.2f %02d.%02d.%d %10.10g %10.2f",
iceb_u_kolbait(30,str1),str1,
data->ostg[0],data->ostgc[0],dn,mn,gn,data->ostm[0],data->ostmc[0],
iceb_u_kolbait(30,gettext("Приход за период")),gettext("Приход за период"),
data->ostg[1],data->ostgc[1]," ",data->ostm[1],data->ostmc[1],
iceb_u_kolbait(30,gettext("Расход за период")),gettext("Расход за период"),
data->ostg[2],data->ostgc[2]," ",data->ostm[2],data->ostmc[2],
iceb_u_kolbait(30,str2),str2,
data->ostg[3],data->ostgc[3],dk,mk,gk,data->ostm[3],data->ostmc[3]);

ost_str->ps_plus(stroka);


}
/*******************************/
/*Распечатка записей*/
/*****************************/
void  zkartmat_ras(class zkartmat_data *data)
{
char strsql[512];
SQLCURSOR       cur;
SQLCURSOR       cur1;
SQL_str         row,row1;
char            imaf[56];
FILE            *f1;
int             kolstr;

if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
if(kolstr == 0)
 return;
 
memset(imaf,'\0',sizeof(imaf));
sprintf(imaf,"k%d.lst",getpid());
if((f1 = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }

iceb_u_zagolov(data->zagolov.ravno(),0,0,0,0,0,0,organ,f1);

if(data->zagolov_poi.getdlinna() > 1)
  fprintf(f1,"\n%s\n",data->zagolov_poi.ravno());

fprintf(f1,"\n\
--------------------------------------------------------------------------------------------------------\n");
fprintf(f1,gettext("Дата подт.|Дата док. | Ном. доку. |Операц.  |Код конт.|Наименование контра.|  Количество |     Цена    |\n"));
fprintf(f1,"\
--------------------------------------------------------------------------------------------------------\n");


double it=0.;
char kor[32];
char kop[32];
char bros[512];
double kolih=0.;
short d,m,g;
short dp,mp,gp;
double cena=0.;

while(cur.read_cursor(&row) != 0)
 {

  if( zkartmat_prow_row(row,&data->poi,data->window) != 0)
    continue;

  /*Читаем код организации, код операции*/

  //читаем шапку документа
  sprintf(strsql,"select kodop,kontr from Dokummat where datd='%s' and sklad=%s and nomd='%s' \
and tip=%s",
  row[4],data->rk.skl.ravno(),row[2],row[5]);

  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    printf("Не нашли шапку документа !!!\n%s\n",strsql);
    continue;
   }
  int tp=atoi(row[5]);
  strcpy(kop,row1[0]);
  strcpy(kor,row1[1]);    

  memset(bros,'\0',sizeof(bros));
  if(iceb_u_atof(kor) == 0. && iceb_u_pole(kor,bros,2,'-') == 0 && kor[0] == '0')
    strncpy(bros,"00",sizeof(bros)-1);
  else
    strncpy(bros,kor,sizeof(bros)-1);
  
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",bros);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    printf("Не найден контрагент - %s\n",kor);
    bros[0]='\0';
   }
  else
    strcpy(bros,row1[0]);

  char br1[3];
  
  strcpy(br1,"+");
  if(tp == 2)
   {  
    strcpy(br1,"-");
    kolih*=(-1);
   }
  iceb_u_rsdat(&dp,&mp,&gp,row[3],2);
  iceb_u_rsdat(&d,&m,&g,row[4],2);

  //Пропускаем промежуточные стартовые остатки
  if(iceb_u_SRAV(row[2],"000",0) == 0)
   if(iceb_u_sravmydat(d,m,g,data->dpn,data->mpn,data->gpn) != 0)
     continue;

  kolih=atof(row[6]);  
  cena=atof(row[7]);  
  if(row[5][0] == '2')
    kolih*=-1;

  it+=kolih;
  
  fprintf(f1,"%02d.%02d.%d|%02d.%02d.%d|%-*s|%-*s %s|%-*s|%-*.*s|\
%13.13g|%13.10g\n",
  dp,mp,gp,
  d,m,g,
  iceb_u_kolbait(12,row[2]),row[2],
  iceb_u_kolbait(7,kop),kop,
  br1,
  iceb_u_kolbait(9,kor),kor,
  iceb_u_kolbait(20,bros),iceb_u_kolbait(20,bros),bros,
  kolih,cena);
/*
  fprintf(f1,"%02d.%02d.%d|%02d.%02d.%d|%-12s|%-7s %s|%-9s|%-20.20s|
%13.13g|%13.10g\n",
  mk10.dp,mk10.mp,mk10.gp,mk10.d,mk10.m,mk10.g,mk10.nn,mk1.kop,br1,
  mk1.kor,bros,mk10.kolih,mk10.cena);
*/
 }


if(fabs(it) < 0.0000000001)
  it=0.;

fprintf(f1,"\
--------------------------------------------------------------------------------------------------------\n\
%*s: %13.10g\n",iceb_u_kolbait(74,gettext("Итого")),gettext("Итого"),it);

iceb_podpis(f1,data->window);
fclose(f1);

iceb_u_spisok imafs;
iceb_u_spisok naimot;

imafs.plus(imaf);
naimot.plus(gettext("Распечатка записей карточки материалла"));

iceb_ustpeh(imafs.ravno(0),3,data->window);
iceb_rabfil(&imafs,&naimot,"",0,data->window);
 

}

/**********************************/
/*Переписывание записей из заказанной карточки в текущюю с удалением заказанной карточки*/
/**************************************************/

void  zkartmat_sliqnie(const char *skl,const char *n_kart,const char *kodm,GtkWidget *wpredok)
{
iceb_u_str nomkart;

iceb_u_spisok repl;
repl.plus(gettext("Вы вошли в режим слияния записей двух карточек.\n\
Записи из карточки номер которой вы введете, перепишутся в текущюю карточку.\n\
После этого карточка будет удалена. Эта операция возможна, если карточка\n\
была заведена в текущем отчетном периоде."));

repl.plus("");
repl.plus(gettext("Введите номер карточки"));
repl.plus("");

if(iceb_menu_vvod1(&repl,&nomkart,20,wpredok) != 0)
  return;

if(iceb_parol(0,wpredok) != 0)
 return;

SQLCURSOR cur;
SQL_str row;
SQLCURSOR cur1;
SQL_str row1;
char strsql[512];

/*Читаем заказанную карточку*/
sprintf(strsql,"select * from Kart where sklad=%s and nomk=%s",
skl,nomkart.ravno());

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,gettext("Нет на складе %s карточки N%s !"),skl,nomkart.ravno());
  repl.new_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  return;
 }

iceb_u_str strok;

strok.new_plus(gettext("Карточка"));
strok.plus(":");
strok.plus(row[1]);

strok.ps_plus(gettext("Материал"));
strok.plus(":");
strok.plus(row[2]);

//читаем наименование материалла
sprintf(strsql,"select naimat from Material where kodm=%s",row[2]);
if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
 {
  strok.plus(" ");
  strok.plus(row1[0]);
 } 

if(iceb_u_SRAV(kodm,row[2],0) != 0)
 {
  repl.new_plus(gettext("Это карточка с другим кодом материалла !"));
  iceb_menu_soob(&repl,wpredok);
  return;
 }

strok.ps_plus(gettext("Учётная цена"));
strok.plus(":");
strok.plus(row[6]);


strok.ps_plus(gettext("Единица измерения"));
strok.plus(":");
strok.plus(row[4]);

strok.ps_plus(gettext("Счёт учёта"));
strok.plus(":");
strok.plus(row[5]);

strok.ps_plus(gettext("Фасовка"));
strok.plus(":");
strok.plus(row[10]);

strok.ps_plus(gettext("Кратность"));
strok.plus(":");
strok.plus(row[8]);

strok.ps_plus(gettext("Код тары"));
strok.plus(":");
strok.plus(row[11]);

strok.ps_plus(gettext("НДС"));
strok.plus(":");
strok.plus_ps(row[9]);

repl.new_plus(strok.ravno());

strok.new_plus(gettext("Внимание"));
strok.plus(" !!!\n");

repl.plus(strok.ravno());

sprintf(strsql,gettext("Переписываем с удалением записей из карточки %s в карточку %s"),
nomkart.ravno(),n_kart);
repl.plus(strsql);

repl.plus(gettext("Всё правильно ? Вы уверены ?"));

if(iceb_menu_danet(&repl,2,wpredok) == 2)
 return;

int kolstr=0;

sprintf(strsql,"select * from Zkart where sklad=%s and nomk=%s \
order by datdp asc",skl,nomkart.ravno());
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr == 0)
  return;

iceb_clock skur(wpredok);

//читаем цену учета в той карточке куда переписываем
double cena=0.;
sprintf(strsql,"select cena from Kart where sklad=%s and nomk=%s",skl,n_kart);
if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
 {
  cena=atof(row1[0]);
  cena=iceb_u_okrug(cena,okrcn);
 }

 



short d,m,g;
short dd,md,gd;
double cenaz=0.;
char kor[56];
char kop[56];
short mpz=0;

while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[3],2);

  if(mpz == 0)
   {

    if(iceb_pblok(m,g,ICEB_PS_MU,wpredok) != 0)
     {
      sprintf(strsql,gettext("Карточка открыта %d.%dг., эта дата заблокирована !"),m,g);
      
      repl.new_plus(strsql);
      repl.new_plus(gettext("Слияние невозможно !"));
      iceb_menu_soob(&repl,wpredok);      
      return;
     }

   }
  mpz++;
  iceb_u_rsdat(&dd,&md,&gd,row[4],2);
  

  int tipz=atoi(row[5]);
  double kolih=atof(row[6]);

  cenaz=atof(row[7]);
  cenaz=iceb_u_okrug(cenaz,okrcn);
 
  if(tipz == 1)
    cenaz=cena;
      
  zapvkrw(d,m,g,row[2],atoi(skl),atoi(n_kart),dd,md,gd,tipz,kolih,cena,wpredok);

  
  memset(kop,'\0',sizeof(kop));
  memset(kor,'\0',sizeof(kor));
  sprintf(strsql,"select tip,kodop,kontr from Dokummat \
where datd='%s' and sklad=%s and nomd='%s' and tip=%d",
  row[4],skl,row[2],tipz);

  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    strok.new_plus(gettext("Не найдено документ"));
    strok.plus(" !!!");
    strok.ps_plus(row[4]);
    strok.plus(" ");
    strok.plus(skl);
    strok.plus(" ");
    strok.plus(n_kart);
    strok.plus(" ");
    strok.plus(row[2]);
    iceb_menu_soob(&strok,wpredok);    
    continue;
   }
  strncpy(kor,row1[2],sizeof(kor)-1);
  strncpy(kop,row1[1],sizeof(kop)-1);

  printf("%02d.%02d.%d %02d.%02d.%d %-*s %-*s %-*s %10.10g %.6g\n",
  d,m,g,dd,md,gd,
  iceb_u_kolbait(8,row[2]),row[2],
  iceb_u_kolbait(3,kop),kop,
  iceb_u_kolbait(8,kor),kor,
  kolih,atof(row[7]));
    
   /*Помечаем что проводки надо проверить*/
   if(tipz == 1 )
    {
     if(iceb_u_SRAV(row[2],"000",0) == 0 || iceb_u_SRAV(kop,"ОСТ",0) == 0 )
        goto vp1;
     
     sprintf(strsql,"select vido from Prihod where kod='%s'",kop);
     if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
      {
       strok.new_plus(strsql);
       sprintf(strsql,"Не найдена операция %s !!!",kop);
       strok.ps_plus(strsql);
       iceb_menu_soob(&strok,wpredok);    
      }
     else
      if(atoi(row1[0]) != 0)
        goto vp1;
    }
   sprintf(strsql,"update Dokummat \
set \
pro=0 \
where datd='%s' and sklad=%s and nomd='%s' and tip=%d",
   row[4],skl,row[2],tipz);

   if(sql_zap(&bd,strsql) != 0)
    {
     iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
     break;
    }
      

vp1:;



   /*Исправляем запись в накладной*/

   sprintf(strsql,"update Dokummat1 \
set \
nomkar=%s, \
cena=%.10g, \
ktoi=%d \
where datd='%d-%d-%d' and sklad=%s and kodm=%s and nomkar=%d and \
tipz=%d",
   n_kart,cenaz,iceb_getuid(wpredok),
   gd,md,dd,skl,kodm,nomkart.ravno_atoi(),tipz);

   if(sql_zap(&bd,strsql) != 0)
    {
     iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
     break;
    }


 } 

/*Удаляем записи в карточке*/
sprintf(strsql,"delete from Zkart where sklad=%s and nomk=%s",
skl,nomkart.ravno());

if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,wpredok);

/*Удаляем саму карточку*/
sprintf(strsql,"delete from Kart where sklad=%s and nomk=%s",
skl,nomkart.ravno());

if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,wpredok);


}
