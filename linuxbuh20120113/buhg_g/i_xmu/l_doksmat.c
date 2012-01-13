/*$Id: l_doksmat.c,v 1.39 2011-02-21 07:35:52 sasa Exp $*/
/*14.10.2009	22.06.2004	Белых А.И.	l_doksmat.c
Просмотр списка документов в материальном учёте
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <unistd.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include        "l_doksmat.h"
#include 	"zar_eks_ko.h"
enum
{
 FK2,
 FK4,
 FK5,
 FK6,
 FK9,
 FK10,
 KOL_F_KL
};

enum
{
 COL_METKAZ,
 COL_DATAD,
 COL_NOMDOK,
 COL_KONTR,
 COL_KODOP,
 COL_SKLAD,
 COL_NOMNALNAK,
 COL_NOMPD,  
 COL_NAIMKONTR,
 COL_NAIMKODOP,
 COL_NAIMSKLAD,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  doksmat_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *label_metka;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *treeview;
  GtkWidget *window;
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  GtkWidget *radiobutton[3];

  //Реквизиты выбранной записи
  iceb_u_str datav;
  iceb_u_str nomdokv;
  iceb_u_str skladv;
  int        tipzv;
     
  short dp,mp,gp;
  short metka_r;   //0-все документы 1-не подтвержденные 2-без проводок
  //реквизиты поискп
  class doksmat_poi rk;
  iceb_u_str zapros; //Запрос к базе донных
  
  doksmat_data()
   {
    metka_r=0;
    kl_shift=0;
    snanomer=0;
    treeview=NULL;
   }

 };
gboolean   doksmat_key_press(GtkWidget *widget,GdkEventKey *event,class doksmat_data *data);
void doksmat_vibor(GtkTreeSelection *selection,class doksmat_data *data);
void doksmat_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class doksmat_data *data);
void  doksmat_knopka(GtkWidget *widget,class doksmat_data *data);
void doksmat_add_columns (GtkTreeView *treeview);
void doksmat_create_list (class doksmat_data *data);
void l_dokmat_eko(class  doksmat_data *data);

void       doksmat_radio0(GtkWidget *,class doksmat_data *);
void       doksmat_radio1(GtkWidget *,class doksmat_data *);
void       doksmat_radio2(GtkWidget *,class doksmat_data *);

int l_doksmat_p(class doksmat_poi *,GtkWidget*);
int doksmat_prov_row(SQL_str row,class doksmat_poi *data,GtkWidget *wpredok);
void doksmat_ras(class doksmat_data *data);

extern SQL_baza	bd;
extern char *name_system;
extern short startgod; /*Стартовый год просмотров*/
extern char *organ;

void  l_doksmat(short dp,short mp,short gp,GtkWidget *wpredok)
{


doksmat_data data;
char bros[312];
GdkColor color;

data.dp=dp;
data.mp=mp;
data.gp=gp;

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
//gtk_widget_set_usize(GTK_WIDGET(data.window),1000,768);
//gtk_widget_set_size_request(GTK_WIDGET(data.window),1024,768);
//gtk_window_set_default_size(GTK_WINDOW(data.window),400,300);
//gtk_window_resize(GTK_WINDOW(data.window),1024,768);
//gtk_window_maximize(GTK_WINDOW(data.window));

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(bros,"%s %s (%s)",name_system,
gettext("Список документов"),
gettext("Материальный учёт"));

gtk_window_set_title (GTK_WINDOW (data.window),iceb_u_toutf(bros));
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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(doksmat_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

sprintf(bros,"%s (%s)",
gettext("Список документов"),
gettext("Материальный учёт"));
data.label_kolstr=gtk_label_new (iceb_u_toutf(bros));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_metka=gtk_label_new ("");
data.label_poisk=gtk_label_new ("");
gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_metka,GTK_STATE_NORMAL,&color);
gtk_widget_modify_fg(data.label_poisk,GTK_STATE_NORMAL,&color);

gtk_box_pack_start (GTK_BOX (vbox2),data.label_metka,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);
//gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);
gtk_widget_set_size_request(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

GtkWidget *hboxradio = gtk_hbox_new (TRUE, 0);

//Вставляем радиокнопки
GSList *group;

data.radiobutton[0]=gtk_radio_button_new_with_label(NULL,gettext("Все"));
//  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[0]),TRUE); //Устанавливем активной кнопку
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton[0]), "clicked",GTK_SIGNAL_FUNC(doksmat_radio0),&data);
//gtk_object_set_user_data(GTK_OBJECT(data.radiobutton0),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton[0]));

sprintf(bros,"%s \"?\"",gettext("Помеченные"));
data.radiobutton[1]=gtk_radio_button_new_with_label(group,bros);
//  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[1]),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data.radiobutton[1]), "clicked",GTK_SIGNAL_FUNC(doksmat_radio1),&data);
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[1], TRUE, TRUE, 0);

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton[1]));

sprintf(bros,"%s \"*\"",gettext("Помеченные"));
data.radiobutton[2]=gtk_radio_button_new_with_label(group,bros);
//  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[2]),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data.radiobutton[2]), "clicked",GTK_SIGNAL_FUNC(doksmat_radio2),&data);
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[2], TRUE, TRUE, 0);

gtk_box_pack_start(GTK_BOX (vbox2), hboxradio, FALSE,FALSE, 0);
gtk_widget_show_all(hboxradio);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(bros,"F2 %s",gettext("Просмотр"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(doksmat_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Просмотр выбранного документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(doksmat_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(doksmat_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Экспорт"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(doksmat_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Экспорт кассовых ордеров в подсистему \"Учёт кассовых оредров\""),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F9 %s",gettext("Оплата"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK9]), "clicked",GTK_SIGNAL_FUNC(doksmat_knopka),&data);
tooltips[FK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK9],data.knopka[FK9],gettext("Переключение режимов просмотра (только оплаченыые, только не оплаченные, все)"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK9]),(gpointer)FK9);
gtk_widget_show(data.knopka[FK9]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(doksmat_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

doksmat_create_list(&data);

gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

printf("l_doksmat end\n");
}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  doksmat_radio0(GtkWidget *widget,class doksmat_data *data)
{
//g_print("doksmat_radio0\n");

if(data->metka_r == 0)
  return;
data->metka_r=0;
doksmat_create_list(data);

}
void  doksmat_radio1(GtkWidget *widget,class doksmat_data *data)
{
//g_print("doksmat_radio1\n");
if(data->metka_r == 1)
  return;
data->metka_r=1;
doksmat_create_list(data);
}
void  doksmat_radio2(GtkWidget *widget,class doksmat_data *data)
{
//g_print("doksmat_radio1\n");
if(data->metka_r == 2)
  return;
data->metka_r=2;
doksmat_create_list(data);
}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void doksmat_create_list (class doksmat_data *data)
{

class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

class iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR curr;
char strsql[512];
int  kolstr=0;
SQL_str row;
SQL_str row1;
short ddd,mmm,ggg;
iceb_u_poltekdat(&ddd,&mmm,&ggg);
//GdkColor color;

//printf("doksmat_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(doksmat_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(doksmat_vibor),data);

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
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);

iceb_u_str zapros;

short dn=data->dp,mn=data->mp,gn=data->gp;
short dk=0,mk=0,gk=0;

if(data->rk.metka_poi == 1 && data->rk.datan.getdlinna() > 1)
 {
  iceb_rsdatp(&dn,&mn,&gn,data->rk.datan.ravno(),&dk,&mk,&gk,data->rk.datak.ravno(),data->window);
 }

if(gn == 0)
 gn=startgod;

if(gn == 0)
 gn=ggg;

sprintf(strsql,"select * from Dokummat where datd >= '%04d-01-01'",gn);

if(dn != 0)
 sprintf(strsql,"select * from Dokummat where datd >= '%04d-%02d-%02d'",gn,mn,dn);

if(dn != 0 && dk != 0)
  sprintf(strsql,"select * from Dokummat \
where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d'",
gn,mn,dn,gk,mk,dk);

data->zapros.new_plus(strsql);

if(data->metka_r == 1)
  data->zapros.plus(" and pod=0");

if(data->metka_r == 2)
  data->zapros.plus(" and pro=0");


data->zapros.plus(" order by datd asc");


if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
char kodkon[30];
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  if(doksmat_prov_row(row,&data->rk,data->window) != 0)
    continue;



  //Метка записи
  if(row[0][0] == '1')
   ss[COL_METKAZ].new_plus("+");
  else
   ss[COL_METKAZ].new_plus("-");

  if(row[7][0] == '0')
   ss[COL_METKAZ].plus("?");
  if(row[8][0] == '0')
   ss[COL_METKAZ].plus("*");
  if(row[14][0] == '0')
   ss[COL_METKAZ].plus("$");

  //Дата документа
  ss[COL_DATAD].new_plus(iceb_u_sqldata(row[1]));
  
  //Номер документа
  ss[COL_NOMDOK].new_plus(iceb_u_toutf(row[4]));

  //Контрагент
  ss[COL_KONTR].new_plus(iceb_u_toutf(row[3]));

  //Код операции
  ss[COL_KODOP].new_plus(iceb_u_toutf(row[6]));

  //Склад
  ss[COL_SKLAD].new_plus(iceb_u_toutf(row[2]));

  //Номер налоговой накладной
  ss[COL_NOMNALNAK].new_plus(iceb_u_toutf(row[5]));

  //Номер накладной парного документа
  ss[COL_NOMPD].new_plus(iceb_u_toutf(row[11]));

  //Узнаём наименование контрагента
  memset(kodkon,'\0',sizeof(kodkon));
  
  iceb_kodkon(kodkon,row[3]);  
  
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kodkon);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
    ss[COL_NAIMKONTR].new_plus(iceb_u_toutf(row1[0]));
  else    
    ss[COL_NAIMKONTR].new_plus("");

  //Узнаём наименование операции
  if(row[0][0] == '1')
    sprintf(strsql,"select naik from Prihod where kod='%s'",row[6]);
  else  
    sprintf(strsql,"select naik from Rashod where kod='%s'",row[6]);
  
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
    ss[COL_NAIMKODOP].new_plus(iceb_u_toutf(row1[0]));
  else    
    ss[COL_NAIMKODOP].new_plus("");

  //Узнаём наименование склада
   
  sprintf(strsql,"select naik from Sklad where kod=%s",row[2]);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
    ss[COL_NAIMSKLAD].new_plus(iceb_u_toutf(row1[0]));
  else    
    ss[COL_NAIMSKLAD].new_plus("");

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[10])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[9],0,data->window));
  

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_METKAZ,ss[COL_METKAZ].ravno(),
  COL_DATAD,ss[COL_DATAD].ravno(),
  COL_NOMDOK,ss[COL_NOMDOK].ravno(),
  COL_KONTR,ss[COL_KONTR].ravno(),
  COL_KODOP,ss[COL_KODOP].ravno(),
  COL_SKLAD,ss[COL_SKLAD].ravno(),
  COL_NOMNALNAK,ss[COL_NOMNALNAK].ravno(),
  COL_NOMPD,ss[COL_NOMPD].ravno(),
  COL_NAIMKONTR,ss[COL_NAIMKONTR].ravno(),
  COL_NAIMKODOP,ss[COL_NAIMKODOP].ravno(),
  COL_NAIMSKLAD,ss[COL_NAIMSKLAD].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

doksmat_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str stroka;
if(data->metka_r != 0)
 {
  if(data->metka_r == 1)
   stroka.new_plus(gettext("Просмотр только не подтверждённых документов"));
  if(data->metka_r == 2)
   stroka.new_plus(gettext("Просмотр только документов без проводок"));
  gtk_label_set_text(GTK_LABEL(data->label_metka),stroka.ravno_toutf());
  gtk_widget_show(data->label_metka);
 }
else 
 gtk_widget_hide(data->label_metka); 

iceb_u_str zagolov;
zagolov.plus(gettext("Список документов"));
zagolov.plus(" (");
zagolov.plus(gettext("Материальный учёт"));
zagolov.plus(")");

sprintf(strsql," %s:%d",gettext("Стартовый год"),startgod);
zagolov.plus(strsql);

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

if(data->rk.metka_poi == 1 || data->rk.metka_opl != 0)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->rk.nomdok.ravno(),gettext("Номер документа"));
  iceb_str_poisk(&zagolov,data->rk.datan.ravno(),gettext("Дата ночала"));
  iceb_str_poisk(&zagolov,data->rk.datak.ravno(),gettext("Дата конца"));
  iceb_str_poisk(&zagolov,data->rk.kontr.ravno(),gettext("Контрагент"));
  iceb_str_poisk(&zagolov,data->rk.kodop.ravno(),gettext("Код операции"));
  iceb_str_poisk(&zagolov,data->rk.skl.ravno(),gettext("Склад"));
  iceb_str_poisk(&zagolov,data->rk.dover.ravno(),gettext("Доручення"));
  iceb_str_poisk(&zagolov,data->rk.nomnalnak.ravno(),gettext("Номер нал.нак."));

  if(data->rk.pr == 1)
   {
    zagolov.ps_plus(gettext("Приход/расход"));
    zagolov.plus(": +");
   }
  if(data->rk.pr == 2)
   {
    zagolov.ps_plus(gettext("Приход/расход"));
    zagolov.plus(": -");
   }
  if(data->rk.metka_opl == 1)
   zagolov.ps_plus(gettext("Только оплаченные"));
  if(data->rk.metka_opl == 2)
   zagolov.ps_plus(gettext("Только неоплаченные"));

  gtk_label_set_text(GTK_LABEL(data->label_poisk),zagolov.ravno_toutf());
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 

gtk_widget_show(data->label_kolstr);

}

/*****************/
/*Создаем колонки*/
/*****************/

void doksmat_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Метка"),renderer,"text",COL_METKAZ,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_METKAZ);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Дата док-та"),renderer,"text",COL_DATAD,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_DATAD);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Номер документа"),renderer,"text",COL_NOMDOK,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_NOMDOK);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Контрагент"),renderer,"text",COL_KONTR,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_KONTR);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Операция"),renderer,"text",COL_KODOP,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_KODOP);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Склад"),renderer,"text",COL_SKLAD,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_SKLAD);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Номер нал.нак."),renderer,"text",COL_NOMNALNAK,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_NOMNALNAK);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Номер пар.док."),renderer,"text",COL_NOMPD,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_NOMPD);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Наименование контрагента"),renderer,"text",COL_NAIMKONTR,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_NAIMKONTR);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Наименование операции"),renderer,"text",COL_NAIMKODOP,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_NAIMKODOP);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Наименование склада"),renderer,"text",COL_NAIMSKLAD,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_NAIMSKLAD);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Дата и время записи"),renderer,"text",COL_DATA_VREM,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_DATA_VREM);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Кто записал"),renderer,"text",COL_KTO,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_KTO);
gtk_tree_view_append_column (treeview, column);


}

/****************************/
/*Выбор строки*/
/**********************/

void doksmat_vibor(GtkTreeSelection *selection,class doksmat_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *datad;
gchar *nomdok;
gchar *sklad;
gchar *tipz;

gint  nomer;


gtk_tree_model_get(model,&iter,COL_DATAD,&datad,COL_NOMDOK,&nomdok,COL_SKLAD,&sklad,
COL_METKAZ,&tipz,NUM_COLUMNS,&nomer,-1);

data->datav.new_plus(iceb_u_fromutf(datad));
data->nomdokv.new_plus(iceb_u_fromutf(nomdok));
data->skladv.new_plus(iceb_u_fromutf(sklad));

if(tipz[0] == '+')
 data->tipzv=1;
else
 data->tipzv=2;
 
data->snanomer=nomer;

g_free(datad);
g_free(nomdok);
g_free(sklad);
g_free(tipz);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  doksmat_knopka(GtkWidget *widget,class doksmat_data *data)
{
//char    bros[512];
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {

  case FK2:
    if(data->kolzap == 0)
     return;
    l_dokmat(&data->datav,data->tipzv,&data->skladv,&data->nomdokv,data->window);
    /*При выходе из документа устанвливаются метки на документ*/
    doksmat_create_list(data);
    return;  

  case FK4:
    l_doksmat_p(&data->rk,data->window);
    doksmat_create_list(data);
    return;  

  case FK5:
    if(data->kolzap == 0)
     return;
    doksmat_ras(data);
    return;  

  case FK6:
    l_dokmat_eko(data);
    return;  

  case FK9:
    data->rk.metka_opl++;
    if(data->rk.metka_opl > 2)
     data->rk.metka_opl=0;    
    doksmat_create_list(data);
    return;  
    
  case FK10:
//    printf("doksmat_knopka F10\n");
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   doksmat_key_press(GtkWidget *widget,GdkEventKey *event,class doksmat_data *data)
{
iceb_u_str repl;

switch(event->keyval)
 {


  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");

    return(TRUE);

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
    return(TRUE);

  case GDK_F5:
    return(TRUE);

  case GDK_F6:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
    return(TRUE);

  case GDK_F9:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK9]),"clicked");
    return(TRUE);

  
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("doksmat_key_press-Нажата клавиша Shift\n");

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
void doksmat_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class doksmat_data *data)
{
//data->metkazapisi=1;
//gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");

}

/******************************/
/*Проверка строки на условия поиска*/
/***********************************/

int doksmat_prov_row(SQL_str row,class doksmat_poi *data,GtkWidget *wpredok)
{
int metka_opl=atoi(row[14]);

if(data->metka_opl == 1 && metka_opl != 1)
  return(1);
if(data->metka_opl == 2 && metka_opl != 0)
  return(1);

if(data->metka_poi == 0)
 return(0);


//Поиск образца в строке
if(iceb_u_proverka(data->nomdok.ravno(),row[4],0,0) != 0)
  return(1);

if(data->pr == 1)
 if(row[0][0] != '1')
  return(1);

if(data->pr == 2)
 if(row[0][0] != '2')
  return(1);
 
if(data->kontr.getdlinna() > 1 || data->kodop.getdlinna() > 1 || data->skl.getdlinna() > 1)
 {
  SQL_str row1;
  SQLCURSOR cur;
  char strsql[512];
  sprintf(strsql,"select sklad,kontr,nomnn,kodop from Dokummat where datd='%s' and \
sklad=%s and nomd='%s' and tip=%s",row[1],row[2],row[4],row[0]);
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

   }
  else
   {
    //Поиск образца в строке
    if(iceb_u_proverka(data->skl.ravno(),row1[0],0,0) != 0)
      return(1);

    //Поиск образца в строке
    if(iceb_u_proverka(data->kontr.ravno(),row1[1],0,0) != 0)
      return(1);

    //Поиск образца в строке
    if(iceb_u_proverka(data->nomnalnak.ravno(),row1[2],0,0) != 0)
      return(1);

    //Поиск образца в строке
    if(iceb_u_proverka(data->kodop.ravno(),row1[3],0,0) != 0)
      return(1);
   }

 }

if(data->dover.getdlinna() > 1 )
 {
  SQL_str row1;
  SQLCURSOR cur;
  char strsql[512];
  short d,m,god;
  iceb_u_rsdat(&d,&m,&god,row[1],2);
  
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and \
sklad=%s and nomd='%s' and nomerz=1",god,row[2],row[4]);
  if(iceb_sql_readkey(strsql,&row1,&cur,wpredok) == 1)
   {
    char dover[30];
    iceb_u_polen(row1[0],dover,sizeof(dover),1,'#');
    //Поиск образца в строке
    if(iceb_u_proverka(data->dover.ravno(),dover,4,0) != 0)
      return(1);
   }
  else
   return(1);
 }
 
return(0);
}
/****************************/
/*Распечатка*/
/***********************/
void doksmat_ras(class doksmat_data *data)
{
iceb_clock sss(data->window);

SQLCURSOR cur;
SQLCURSOR curr;
int  kolstr=0;
SQL_str row;
SQL_str row1;
char strsql[512];

if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return;
 }
if(kolstr == 0)
 return;

char imaf[30];
FILE *ff;

sprintf(imaf,"doksm%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }
iceb_u_startfil(ff);
fprintf(ff,"\x1B\x4D"); /*12-знаков*/
fprintf(ff,"\x0F");  /*Ужатый режим*/

iceb_u_zagolov(gettext("Список документов"),0,0,0,0,0,0,organ,ff);
fprintf(ff,"\"+\"-%s\n",gettext("отмечены приходные документы"));
fprintf(ff,"\"-\"-%s\n",gettext("отмечены расходные документы"));
fprintf(ff,"\"?\"-%s\n",gettext("отмечены не подтверждённые документы"));
fprintf(ff,"\"*\"-%s\n",gettext("отмечены документы без проводок"));

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
Метка|  Дата    |Номер документа|      Контрагент    |    Операция        |        Склад       |Номер н.н.|Номер р.д.|Дата и время записи|Кто записал\n");
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------------------\n");
char kodkon[30];
iceb_u_str metka;
iceb_u_str datad;
iceb_u_str kontr;
iceb_u_str kodop;
iceb_u_str sklad;
iceb_u_str dataz;
iceb_u_str ktoz;

while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  if(doksmat_prov_row(row,&data->rk,data->window) != 0)
    continue;

  //Метка записи
  if(row[0][0] == '1')
   metka.new_plus("+");
  else
   metka.new_plus("-");

  if(row[7][0] == '0')
   metka.plus("?");
  if(row[8][0] == '0')
   metka.plus("*");

  //Дата документа
  datad.new_plus(iceb_u_sqldata(row[1]));
  

  //Контрагент
  kontr.new_plus(row[3]);
  kontr.plus(" ");

  //Узнаём наименование контрагента
  memset(kodkon,'\0',sizeof(kodkon));
  
  iceb_kodkon(kodkon,row[3]);  
  
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kodkon);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
    kontr.plus(row1[0]);


  //Код операции
  kodop.new_plus(row[6]);
  kodop.plus(" ");

  //Узнаём наименование операции
  if(row[0][0] == '1')
    sprintf(strsql,"select naik from Prihod where kod='%s'",row[6]);
  else  
    sprintf(strsql,"select naik from Rashod where kod='%s'",row[6]);
  
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
    kodop.plus(row1[0]);

  //Склад
  sklad.new_plus(row[2]);
  sklad.plus(" ");

  //Узнаём наименование склада
   
  sprintf(strsql,"select naik from Sklad where kod=%s",row[2]);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
    sklad.plus(row1[0]);

  //Дата и время записи
  dataz.new_plus(iceb_u_toutf(iceb_u_vremzap(row[10])));

  //Кто записал
  ktoz.new_plus(iceb_kszap(row[9],1,data->window));
  
  fprintf(ff,"%-5s %-10s %-15s %-20.20s %-20.20s %-20.20s %-10s %-10s %s %s\n",
  metka.ravno(),
  datad.ravno(),
  row[4],
  kontr.ravno(),
  kodop.ravno(),
  sklad.ravno(),
  row[5],
  row[11],
  dataz.ravno(),
  ktoz.ravno());
  
 }

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------------------\n");

iceb_podpis(ff,data->window);
fprintf(ff,"\x1B\x50"); /*10-знаков*/
fprintf(ff,"\x12");  /*Нормальный режим печати*/

fclose(ff);
iceb_u_spisok imafs;
iceb_u_spisok naim;
imafs.plus(imaf);
naim.plus(gettext("Список документов"));

iceb_rabfil(&imafs,&naim,"",0,data->window);

}



/*************************************/
/*Экспорт кассовых ордеров*/
/***************************************/
void l_doksmat_eko_r(class  doksmat_data *data,class zar_eks_ko_rek *rek)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

SQL_str row,row1;
class SQLCURSOR cur,cur1;
char strsql[512];
short vt=0;
char shetk[32];
char naimop[512];

//Проверяем код операции
if(rek->prn == 1)
 sprintf(strsql,"select naik,shetkor from Kasop1 where kod='%s'",rek->kodop.ravno());
if(rek->prn == 2)
 sprintf(strsql,"select naik,shetkor from Kasop2 where kod='%s'",rek->kodop.ravno());

if(iceb_sql_readkey(strsql,&row,&cur1,data->window) != 1)
 {
  sprintf(strsql,gettext("Не найден код операции %s !"),rek->kodop.ravno());
  iceb_menu_soob(strsql,data->window);
  return;
 }

memset(naimop,'\0',sizeof(naimop));
strncpy(naimop,row[0],sizeof(naimop)-1);
strncpy(shetk,row[1],sizeof(shetk)-1);



iceb_poldan("Код группы возвратная тара",strsql,"matnast.alx",data->window);
vt=atoi(strsql);

class iceb_u_spisok kod_kontr;
class iceb_u_spisok koment_kontr;
class iceb_u_double suma_kontr;


short		dko,mko,gko;
iceb_u_rsdat(&dko,&mko,&gko,rek->data.ravno(),1);
int kolih_kasord=0;
class iceb_u_str nomd("");
char koment[512];
double suma=0.;
double sumabn=0.;
double sumkor=0.;
short d,m,g;
int lnds=0;
double kolih=0.;
double suma_nds=0.;
double suma_dok=0.;
float pnds=0.;
int tipz_mat=0;
if(rek->prn == 1)
 tipz_mat=2;
else
 tipz_mat=1;
int nomer_kontr=0;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return;
 }
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(rek->prn == atoi(row[0])) /*Если приходный кассовый ордер то нам нужны расходные документы и наооборот*/
   continue;
  
 if(doksmat_prov_row(row,&data->rk,data->window) != 0)
    continue;
 //if(sravdok(row,poisk) != 0)
//    continue;
  
  iceb_u_rsdat(&d,&m,&g,row[1],2);

  pnds=atof(row[13]);

  /*Узнаем НДС документа*/
  lnds=0;
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' and sklad=%s and nomerz=11",g,row[4],row[3]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
    lnds=atoi(row1[0]);
  suma_nds=0.;
  if(lnds == 0)
   {
    /*Узнаем сумму НДС документа*/
    sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=6",g,row[4],row[3]);

    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
      suma_nds=atof(row1[0]);
   }

  sumzap1w(d,m,g,row[4],atoi(row[2]),&suma,&sumabn,lnds,vt,&sumkor,tipz_mat,&kolih,data->window);

  if(lnds == 0 && suma_nds == 0.)
    suma_nds=(suma+sumkor)*pnds/100.;
  
/*  printw("suma=%f sumabn=%f sumkor=%f\n",suma,sumabn,sumkor);*/
  suma_dok=suma+sumkor+suma_nds+sumabn;
    
  sprintf(koment,"%s:%.10g",gettext("Количество"),kolih);  


  if(rek->metka_r == 0)
   {
    kolih_kasord++;
    iceb_lock_tables("LOCK TABLES Kasord WRITE,icebuser READ");
    iceb_nomnak(gko,rek->kassa.ravno(),&nomd,rek->prn,2,1,data->window);
    zaphkorw(0,rek->kassa.ravno(),rek->prn,dko,mko,gko,nomd.ravno(),shetk,rek->kodop.ravno(),naimop,"","","","","",0,0,0,"","",data->window);

    printf("%s:%s\n",gettext("Кассовый ордер"),nomd.ravno());

    zapzkorw(rek->kassa.ravno(),rek->prn,dko,mko,gko,nomd.ravno(),row[4],suma_dok,"",koment,data->window);
   }
  else
   {
    if((nomer_kontr=kod_kontr.find_r(row[3])) < 0)
     {
      kod_kontr.plus(row[3]);
      koment_kontr.plus(koment);
     }

    suma_kontr.plus(suma_dok,nomer_kontr);


   }  

 }

if(rek->metka_r == 1)
 {
  kolih_kasord++;
  iceb_lock_tables("LOCK TABLES Kasord WRITE,icebuser READ");
  iceb_nomnak(gko,rek->kassa.ravno(),&nomd,rek->prn,2,1,data->window);
  zaphkorw(0,rek->kassa.ravno(),rek->prn,dko,mko,gko,nomd.ravno(),shetk,rek->kodop.ravno(),naimop,"","","","","",0,0,0,"","",data->window);
  printf("%s:%s\n",gettext("Кассовый ордер"),nomd.ravno());

  for(nomer_kontr=0; nomer_kontr < kod_kontr.kolih(); nomer_kontr++)
   {
    
    zapzkorw(rek->kassa.ravno(),rek->prn,dko,mko,gko,nomd.ravno(),kod_kontr.ravno(nomer_kontr),suma_kontr.ravno(nomer_kontr),"",koment_kontr.ravno(nomer_kontr),data->window);
   }
 }

sprintf(strsql,"%s:%d",gettext("Количество экспортированых кассовых ордеров"),kolih_kasord);
iceb_menu_soob(strsql,data->window);

}
/***********************************/
/*Експорт кассовых ордеров*/
/****************************/

void l_dokmat_eko(class  doksmat_data *data)
{
class zar_eks_ko_rek rek;

  
if(eks_ko_v(&rek,data->window) != 0)
 return;
l_doksmat_eko_r(data,&rek);

}
