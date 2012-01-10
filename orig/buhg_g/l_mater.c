/*$Id: l_mater.c,v 1.56 2011-04-21 05:32:48 sasa Exp $*/
/*12.11.2009	05.05.2004	Белых А.И.	l_mater.c
Работа со списком материалов
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
#include        "buhg_g.h"
#include        "l_mater.h"
#include        "l_mater_dv.h"

enum
{
 FK2,
 SFK2,
 FK3,
 FK4,
 SFK4,
 FK5,
 SFK5,
 FK6,
 FK7,
 SFK7,
 FK8,
 SFK8,
 FK9,
 FK10,
 KOL_F_KL
};


enum
{
 COL_KOD,
 COL_OST,
 COL_NAIM,
 COL_EI,
 COL_CENA,
 COL_KRAT,
 COL_FAS,  
 COL_ARTIK,
 COL_HTRIX,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  mater_data
 {
  public:
  class mater_rek poisk;

  iceb_u_str kodv; //Код записи на которой стоим
  iceb_u_str naimv;

  class iceb_u_str kodzap; //Только что записанный код 
  class iceb_u_str zapros;
  
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *label_sklad;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       metka_voz;  //0-выбрали 1-нет  
  short     metka_rr;   //0-работа со списком 1-выбор
  short     metka_ost;  //0-без остатка 1-с остатком реальным 2-с остатками по документам
  short metka_sort;     /*0-сортировка по наименованиям 1-по кодам материалла*/
  
  int skl;
  int skl_poi;
  int kolzap;
  short metka_skl;
  short metkazapisi;
  //Конструктор
  mater_data()
   {
    metka_sort=0;
    snanomer=0;
    metka_skl=skl_poi=skl=metka_ost=metka_rr=metka_voz=kl_shift=metkazapisi=0;
    window=treeview=NULL;
    
   }      
 };

gboolean   mater_key_press(GtkWidget *widget,GdkEventKey *event,class mater_data *data);
void mater_vibor(GtkTreeSelection *selection,class mater_data *data);
void mater_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class mater_data *data);
void  mater_knopka(GtkWidget *widget,class mater_data *data);
void mater_add_columns (GtkTreeView *treeview,short,short);
int l_mater_v(class iceb_u_str *kodmat,GtkWidget *wpredok);
void l_mater_p(class  mater_rek *data_poi,GtkWidget *wpredok);
void mater_udzap(class mater_data *data);
int  mater_prov_row(SQL_str row,class mater_data *data);
void mater_rasp(class mater_data *data);
int  mater_mvru(GtkWidget*);
void  mater_udnz(GtkWidget *wpredok);
void  l_mater_zkm(const char *kodm,GtkWidget *wpredok);
void mater_create_list (class mater_data *data);

int  l_kartmat(const char *kodm,iceb_u_str *skl,iceb_u_str *n_kart,GtkWidget *wpredok);
int l_mater_dv(class mater_dv_data *datar,GtkWidget *wpredok);
void            dvmatw(class mater_dv_data*,int kodm,GtkWidget *wpredok);
int  mater_prc(class mater_data *data);
int l_mater_vmost(GtkWidget *wpredok);

extern char *organ;
extern short metka_pros_mat_s_ost; //0-всё показывать 1-только материалы с остатками

int   l_mater(int metka_rr, //0-ввод и корек. 1-выбор
class iceb_u_str *kod,
class iceb_u_str *naim,
int skl, // склад
int metka_pros_mat, //0-всё 1-смотреть переменную metka_pros_mat_s_ost
GtkWidget *wpredok)
{

class mater_data data;
char bros[512];
GdkColor color;
data.poisk.clear_zero();

if(metka_pros_mat == 1)
  data.metka_ost=data.poisk.metka_mat_s_ost=metka_pros_mat_s_ost;

data.metka_rr=metka_rr;
data.skl_poi=data.skl=skl;
if(data.skl_poi != 0)
 data.metka_skl=1;
 
if(naim->getdlinna() > 1)
 {
  data.poisk.metka_poi=1;
  data.poisk.naim.new_plus(naim->ravno());
 }
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Список материаллов"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(mater_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Список материалов"));
//gdk_color_parse("green",&color);
//gtk_widget_modify_fg(data.label_kolstr,GTK_STATE_NORMAL,&color);


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_sklad=gtk_label_new ("");
data.label_poisk=gtk_label_new ("");
gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_poisk,GTK_STATE_NORMAL,&color);
gtk_widget_modify_fg(data.label_sklad,GTK_STATE_NORMAL,&color);

gtk_box_pack_start (GTK_BOX (vbox2),data.label_sklad,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_widget_show (data.sw);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(mater_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(mater_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(mater_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(mater_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"%sF4 %s",RFK,gettext("Склад"));
data.knopka[SFK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK4]), "clicked",GTK_SIGNAL_FUNC(mater_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK4],TRUE,TRUE, 0);
tooltips[SFK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK4],data.knopka[SFK4],gettext("Показать только материалы которые есть на складе"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK4]),(gpointer)SFK4);
gtk_widget_show(data.knopka[SFK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(mater_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"%sF5 %s",RFK,gettext("Сортировка"));
data.knopka[SFK5]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK5]), "clicked",GTK_SIGNAL_FUNC(mater_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK5],TRUE,TRUE, 0);
tooltips[SFK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK5],data.knopka[SFK5],gettext("Включить/выключить сортировку записей по кодам материалов"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK5]),(gpointer)SFK5);
gtk_widget_show(data.knopka[SFK5]);

sprintf(bros,"F6 %s",gettext("Замена кода"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(mater_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Замена одного кода на другой с удалением первого"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F7 %s",gettext("Остаток"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(mater_knopka),&data);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Включить/выключить показ записей с остатками"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"%sF7 %s",RFK,gettext("С остатками"));
data.knopka[SFK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK7],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK7]), "clicked",GTK_SIGNAL_FUNC(mater_knopka),&data);
tooltips[SFK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK7],data.knopka[SFK7],gettext("Включить/выключить показ записей имеющих остатки"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK7]),(gpointer)SFK7);
gtk_widget_show(data.knopka[SFK7]);

sprintf(bros,"F8 %s",gettext("Карточка"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]), "clicked",GTK_SIGNAL_FUNC(mater_knopka),&data);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Просмотр списка карточек по коду материалла"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_widget_show(data.knopka[FK8]);

sprintf(bros,"%sF8 %s",RFK,gettext("Движение"));
data.knopka[SFK8]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK8],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK8]), "clicked",GTK_SIGNAL_FUNC(mater_knopka),&data);
tooltips[SFK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK8],data.knopka[SFK8],gettext("Распечатать движение материалла по документам"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK8]),(gpointer)SFK8);
gtk_widget_show(data.knopka[SFK8]);

sprintf(bros,"F9 %s",gettext("Переоценка"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK9]), "clicked",GTK_SIGNAL_FUNC(mater_knopka),&data);
tooltips[FK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK9],data.knopka[FK9],gettext("Уменьшение или увеличение цен на заданный процент"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK9]),(gpointer)FK9);
gtk_widget_show(data.knopka[FK9]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(mater_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
//gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));


mater_create_list(&data);

gtk_widget_grab_focus(data.knopka[FK10]);
gtk_widget_show(data.window);

if(metka_rr == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();

if(data.metka_voz == 0 )
 {
  kod->new_plus(data.kodv.ravno());
  naim->new_plus(data.naimv.ravno());
 }

printf("l_mater end\n");
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.metka_voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void mater_create_list (class mater_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

class iceb_clock skur(data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;
short	ddd,mmm,ggg;
iceb_u_poltekdat(&ddd,&mmm,&ggg);

/*printf("mater_create_list %d\n",data->snanomer);*/
data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);
gtk_widget_show (data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(mater_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(mater_vibor),data);

gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (data->treeview), TRUE); //Устанавливаются полоски при отображении списка
gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_INT, 
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

iceb_refresh();
if(data->metka_sort == 0)
 sprintf(strsql,"select * from Material order by naimat asc");
else
 sprintf(strsql,"select * from Material order by kodm asc");
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
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  if(mater_prov_row(row,data) != 0)
    continue;

  if(iceb_u_SRAV(data->kodzap.ravno(),row[0],0) == 0)
    data->snanomer=data->kolzap;

  //Код
  ss[COL_KOD].new_plus(iceb_u_toutf(row[0]));

  //остаток
  if(data->metka_ost != 0 || data->poisk.metka_mat_s_ost == 1)
   {
    sprintf(strsql,"%.10g",data->poisk.kolih);
    ss[COL_OST].new_plus(strsql);
   }
  else
    ss[COL_OST].new_plus(" ");
    
  //Наименование
  ss[COL_NAIM].new_plus(iceb_u_toutf(row[2]));

  //Единица измерения
  ss[COL_EI].new_plus(iceb_u_toutf(row[4]));

  //Цена
  sprintf(strsql,"%.10g",atof(row[6]));  
  ss[COL_CENA].new_plus(strsql);

  //Кратность
  sprintf(strsql,"%.10g",atof(row[7]));  
  ss[COL_KRAT].new_plus(strsql);

  //Фасовка
  sprintf(strsql,"%.10g",atof(row[8]));  
  ss[COL_FAS].new_plus(strsql);

  //артикул
  ss[COL_ARTIK].new_plus(iceb_u_toutf(row[14]));

  //Штрих-код
  ss[COL_HTRIX].new_plus(iceb_u_toutf(row[3]));

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[11])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[10],0,data->window));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD,ss[COL_KOD].ravno_atoi(),
  COL_OST,ss[COL_OST].ravno(),
  COL_NAIM,ss[COL_NAIM].ravno(),
  COL_EI,ss[COL_EI].ravno(),
  COL_CENA,ss[COL_CENA].ravno(),
  COL_KRAT,ss[COL_KRAT].ravno(),
  COL_FAS,ss[COL_FAS].ravno(),
  COL_ARTIK,ss[COL_ARTIK].ravno(),
  COL_HTRIX,ss[COL_HTRIX].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

mater_add_columns (GTK_TREE_VIEW (data->treeview),data->metka_ost,data->poisk.metka_mat_s_ost);


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK8]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK8]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK8]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK8]),TRUE);//Доступна
 }

data->kodzap.new_plus("");


iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Список материалов"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);
if(data->poisk.metka_mat_s_ost == 1)
 zagolov.ps_plus(gettext("Показываем записи только с остатками !"));
 
gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

if(data->metka_skl == 1)
 {
  sprintf(strsql," %s:%d",gettext("Склад"),data->skl_poi);
  zagolov.new_plus(strsql);
  gtk_label_set_text(GTK_LABEL(data->label_sklad),zagolov.ravno_toutf());
  gtk_widget_show(data->label_sklad);
 }
else
  gtk_widget_hide(data->label_sklad); 


if(data->poisk.metka_poi == 1)
 {
//  printf("Формирование заголовка с реквизитами поиска.\n");
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.kod.ravno(),gettext("Код"));
  iceb_str_poisk(&zagolov,data->poisk.naim.ravno(),gettext("Наименование"));
  iceb_str_poisk(&zagolov,data->poisk.grupa.ravno(),gettext("Группа"));
  iceb_str_poisk(&zagolov,data->poisk.ei.ravno(),gettext("Единица измерения"));
  iceb_str_poisk(&zagolov,data->poisk.krat.ravno(),gettext("Кратность"));
  iceb_str_poisk(&zagolov,data->poisk.kodtar.ravno(),gettext("Код тары"));
  iceb_str_poisk(&zagolov,data->poisk.artik.ravno(),gettext("Артикул"));
  iceb_str_poisk(&zagolov,data->poisk.htrix.ravno(),gettext("Штрих-код"));
  iceb_str_poisk(&zagolov,data->poisk.kriost.ravno(),gettext("Критичний залишок"));
  iceb_str_poisk(&zagolov,data->poisk.fas.ravno(),gettext("Фасовка"));
  iceb_str_poisk(&zagolov,data->poisk.cena.ravno(),gettext("Цена"));
  
  if(data->poisk.mcena == 1)
    iceb_str_poisk(&zagolov,gettext("С НДC"),gettext("Цена введена"));
  if(data->poisk.mcena == 2)
    iceb_str_poisk(&zagolov,gettext("Без НДC"),gettext("Цена введена"));

  gtk_label_set_text(GTK_LABEL(data->label_poisk),zagolov.ravno_toutf());
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 

gtk_widget_show(data->label_kolstr);

//printf("Конец\n");

}

/*****************/
/*Создаем колонки*/
/*****************/

void mater_add_columns(GtkTreeView *treeview,short metka_ost,short metka_mat_s_ost)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;

//printf("mater_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Код"),renderer,"text",COL_KOD,NULL);
gtk_tree_view_column_set_sort_column_id (column, COL_KOD);
gtk_tree_view_append_column (treeview, column);



renderer = gtk_cell_renderer_text_new ();

if(metka_ost != 0 || metka_mat_s_ost == 1)
 {
  column = gtk_tree_view_column_new_with_attributes (gettext("Остаток"),renderer,"text",COL_OST,NULL);
  gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
  gtk_tree_view_column_set_sort_column_id (column, COL_OST);
  gtk_tree_view_append_column (treeview, column);
 }

column = gtk_tree_view_column_new_with_attributes (gettext("Наименование"),renderer,"text",COL_NAIM,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NAIM);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Е/и"),renderer,"text",COL_EI,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_EI);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Цена"),renderer,"text",COL_CENA,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_CENA);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кратность"), renderer,"text", COL_KRAT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Фасовка"), renderer,"text", COL_FAS,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Артикул"), renderer,"text", COL_ARTIK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Штрих-код"), renderer,"text", COL_HTRIX,NULL);

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

void mater_vibor(GtkTreeSelection *selection,class mater_data *data)
{
//printf("mater_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

//gchar *kod;
gint kod;
gchar *naim;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_KOD,&kod,COL_NAIM,&naim,NUM_COLUMNS,&nomer,-1);

//data->kodv.new_plus(iceb_u_fromutf(kod));
data->kodv.new_plus(kod);
data->naimv.new_plus(iceb_u_fromutf(naim));
data->snanomer=nomer;

//g_free(kod);
g_free(naim);

//printf("mater_vibor%s %s %d\n",data->kodv.ravno(),data->naimv.ravno(),nomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  mater_knopka(GtkWidget *widget,class mater_data *data)
{
mater_dv_data datar;
iceb_u_str repl;
iceb_u_str skl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case FK2:
    data->metkazapisi=0;

//    printf("mater_knopka F2 metkazapisi=%d\n",data->metkazapisi);
    data->kodzap.new_plus("");
    if(l_mater_v(&data->kodzap,data->window) == 0)
      mater_create_list(data);
    return;  

  case SFK2:
    data->metkazapisi=1;
    data->kodzap.new_plus(data->kodv.ravno());
    if(l_mater_v(&data->kodzap,data->window) == 0)
      mater_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    int nomer;
    if((nomer=mater_mvru(data->window)) == 0)
      mater_udzap(data);
  
    if(nomer == 1)
     {
      mater_udnz(data->window);
     }
    mater_create_list(data);
    return;  


  case FK4:
    l_mater_p(&data->poisk,data->window);
    mater_create_list(data);
    return;  

  case SFK4:

      data->metka_skl++;
      
      if(data->metka_skl > 1)
       data->metka_skl=0;

    if(data->skl == 0 && data->metka_skl == 1)
     {
      iceb_u_str repl;
      iceb_u_str vvod;
      repl.plus(gettext("Введите код склада"));
      if(iceb_menu_vvod1(&repl,&vvod,20,data->window) != 0)
       {
        mater_create_list(data);
        return;
       }
      data->skl_poi=vvod.ravno_atoi();
     }



    mater_create_list(data);
    return;  

  case FK5:
    if(data->kolzap == 0)
      return;
    mater_rasp(data);
    return;  

  case SFK5:
    data->metka_sort++;
    if(data->metka_sort > 1)
     data->metka_sort=0;
    mater_create_list(data);
    return;  

  case FK6:
    l_mater_zkm(data->kodv.ravno(),data->window);
    mater_create_list(data);
    return;  
  
  case FK7:
/**********
    data->metka_ost++;
    if(data->metka_ost == 2)
     data->metka_ost=0;
***************/
    data->metka_ost=l_mater_vmost(data->window);

    mater_create_list(data);
    return;  


  case SFK7:
    if(data->metka_ost == 0)
     {
      iceb_menu_soob(gettext("Не включен режим показа с остатками (F7)"),data->window);
      return;
     }
    data->poisk.metka_mat_s_ost++;
    if(data->poisk.metka_mat_s_ost > 1)
     data->poisk.metka_mat_s_ost=0;

    mater_create_list(data);
    return;  
     
  case FK8:
    if(data->kolzap == 0)
      return;
    l_kartmat(data->kodv.ravno(),&skl,&repl,NULL);
    mater_create_list(data);
    return;  

  case SFK8:
    if(data->kolzap == 0)
      return;
    if(l_mater_dv(&datar,data->window) == 0)
      dvmatw(&datar,data->kodv.ravno_atoi(),data->window);
    return;  

  case FK9:
    if(data->kolzap == 0)
      return;

   if(mater_prc(data) == 0)
    mater_create_list(data);

   return;  
   
  case FK10:
//    printf("mater_knopka F10\n");
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   mater_key_press(GtkWidget *widget,GdkEventKey *event,class mater_data *data)
{
iceb_u_str repl;
//printf("mater_key_press keyval=%d state=%d\n",event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_KP_Add: //Нажата клавиша "+" на дополнительной клавиатуре
  case GDK_F2:

    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");

    return(TRUE);
   
  case GDK_F3:
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    return(TRUE);

  case GDK_F4:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK4]),"clicked");
    return(TRUE);

  case GDK_F5:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK5]),"clicked");
    return(TRUE);

  case GDK_F6:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
    return(TRUE);

  case GDK_F7:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK7]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK7]),"clicked");
    return(TRUE);

  case GDK_F8:

    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK8]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK8]),"clicked");

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
    printf("mater_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  case GDK_KP_0:
    printf("Нажата клавиша 000.\n");
//    data->metka_voz=GDK_KP_0;
    data->metka_voz=0;
    gtk_widget_destroy(data->window);
    return(TRUE);

  case GDK_KP_Insert:
//    data->metka_voz=GDK_KP_0;
    data->metka_voz=0;
    gtk_widget_destroy(data->window);
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
void mater_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class mater_data *data)
{
//printf("mater_v_row\n");
//printf("mater_v_row корректировка\n");
if(data->metka_rr == 0)
 {
  gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
  return;
 }

data->metka_voz=0;

gtk_widget_destroy(data->window);


}

/*****************************/
/*Удаление записи            */
/*****************************/

void mater_udzap(class mater_data *data)
{

iceb_u_str sp;
sp.plus(gettext("Удалить запись ? Вы уверены ?"));
if(iceb_menu_danet(&sp,2,data->window) == 2)
 return;

if(mater_pvu(1,data->kodv.ravno(),data->window) != 0)
 return;

char strsql[512];

sprintf(strsql,"delete from Material where kodm=%s",
data->kodv.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;

}
/****************************/
/*Проверка записей          */
/*****************************/

int  mater_prov_row(SQL_str row,class mater_data *data)
{

if(data->metka_skl == 1 && data->skl_poi != 0)
 {
  char strsql[512];
  sprintf(strsql,"select kodm from Kart where sklad=%d and kodm=%s limit 1",data->skl_poi,row[0]);
  if(sql_readkey(&bd,strsql) <= 0)
    return(5);  

 }

if(data->metka_ost == 1)
  data->poisk.kolih=ostkar1w(atoi(row[0]),data->skl_poi,data->poisk.dt,data->poisk.mt,data->poisk.gt,data->window);
if(data->metka_ost == 2)
  data->poisk.kolih=ostdok1w(data->poisk.dt,data->poisk.mt,data->poisk.gt,data->skl_poi,atoi(row[0]));
//printf("metka_mat_s_ost=%d kolih=%f\n",data->poisk.metka_mat_s_ost,data->poisk.kolih);

if(data->poisk.metka_mat_s_ost == 1)
 if(data->poisk.kolih < 0.0000001)
  return(1);

if(data->poisk.metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->poisk.kod.ravno(),row[0],0,0) != 0)
 return(1);


//Поиск образца в строке
if(data->poisk.naim.getdlinna() > 1)
 if(iceb_u_strstrm(row[2],data->poisk.naim.ravno()) == 0)
  return(1);

//Поиск образца в строке
if(iceb_u_proverka(data->poisk.grupa.ravno(),row[1],0,0) != 0)
  return(1);

//Поиск образца в строке
if(iceb_u_proverka(data->poisk.ei.ravno(),row[4],0,0) != 0)
  return(1);

//Поиск образца в строке
if(data->poisk.artik.getdlinna() > 1)
 if(iceb_u_strstrm(row[14],data->poisk.artik.ravno()) == 0)
  return(1);

//Поиск образца в строке
if(data->poisk.htrix.getdlinna() > 1)
 if(iceb_u_strstrm(row[3],data->poisk.htrix.ravno()) == 0)
  return(1);

if(data->poisk.cena.getdlinna() > 1)
 if(atof(data->poisk.cena.ravno()) != atof(row[6]))
  return(1);

if(data->poisk.krat.getdlinna() > 1)
 if(atof(data->poisk.krat.ravno()) != atof(row[7]))
  return(1);

//Полное сравнение
if(iceb_u_proverka(data->poisk.kodtar.ravno(),row[9],0,0) != 0)
 return(1);

if(data->poisk.fas.getdlinna() > 1)
 if(atof(data->poisk.fas.ravno()) != atof(row[8]))
  return(1);

if(data->poisk.kriost.getdlinna() > 1)
 if(atof(data->poisk.kriost.ravno()) != atof(row[5]))
  return(1);

if(data->poisk.mcena == 1) //0-не проверять 1-искать с НДС 2-искать без НДС
 if(atoi(row[12]) != 0)
  return(1);

if(data->poisk.mcena == 2) //0-не проверять 1-искать с НДС 2-искать без НДС
 if(atoi(row[12]) != 1)
  return(1);

  
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void mater_rasp(class mater_data *data)
{
char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;

if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"mater%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список материалов"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_u_zagolov(gettext("Список материалов"),0,0,0,0,0,0,organ,ff);

fprintf(ff,"\
------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Код |          Наименование        |Ед.изм.|  Цена    |Дата и время запис.| Кто записал\n"));
/*
1234 123456789012345678901234567890 1234567 1234567890
*/
fprintf(ff,"\
------------------------------------------------------------------------------------------\n");



//int i=0;

iceb_u_str s5;
iceb_u_str s6;


cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  if(mater_prov_row(row,data) != 0)
    continue;

  //Дата и время записи
  s5.new_plus(iceb_u_vremzap(row[11]));

  //Кто записал
  s6.new_plus(iceb_kszap(row[10],1,data->window));

  fprintf(ff,"%-4s %-*.*s %-*s %10.10g %s %s\n",
  row[0],
  iceb_u_kolbait(30,row[2]),iceb_u_kolbait(30,row[2]),row[2],
  iceb_u_kolbait(7,row[4]),row[4],
  atof(row[6]),s5.ravno(),s6.ravno());

  for(int ii=30; ii < iceb_u_strlen(row[2]); ii+=30)
   fprintf(ff,"%4s %-*.*s\n",
   "",
   iceb_u_kolbait(30,iceb_u_adrsimv(ii,row[2])),
   iceb_u_kolbait(30,iceb_u_adrsimv(ii,row[2])),
   iceb_u_adrsimv(ii,row[2]));
 }
fprintf(ff,"\
------------------------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);

iceb_rabfil(&imaf,&naimot,"",0,data->window);

}
/*****************************/
/*Проверить возможность удаления*/
/********************************/
//Если вернули 0-можно удалять 1-нет
int  mater_pvu(int met,const char *kod,GtkWidget *wpredok)
{
char strsql[512];
int kolstr=0;
SQLCURSOR cur;


sprintf(strsql,"select kodm from Kart where kodm=%d limit 1",atoi(kod));

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
 }
else
 {   
   
  if(kolstr != 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Kart",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }    
  }

sprintf(strsql,"select kodm from Dokummat1 where kodm=%s limit 1",kod);
 /*
 printw("\nstrsql=%s\n",strsql);
 OSTANOV();
 */
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
 }


if(kolstr != 0)
 {
  if(met != 0)
   {
    iceb_u_str soob;
    sprintf(strsql,"%s Dokummat1",gettext("Удалить невозможно ! Есть записи в таблице"));
    soob.plus(strsql);
    iceb_menu_soob(&soob,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select kodi from Specif where kodi=%s or kodd=%s and kz=0 limit 1",kod,kod);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
 }
else
 {   
  
  if(kolstr != 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Specif",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }    
 }

sprintf(strsql,"select metka,kodzap from Usldokum1 where \
metka=0 and kodzap=%s limit 1",kod);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
 }
else
 {   
  
  if(kolstr != 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Usldokum1",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }    
 }

sprintf(strsql,"select kodm from Roznica where kodm='%s' limit 1",kod);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
 
if(kolstr != 0)
 {
  if(met != 0)
   {
    iceb_u_str soob;
    sprintf(strsql,"%s Roznica",gettext("Удалить невозможно ! Есть записи в таблице"));
    soob.plus(strsql);
    iceb_menu_soob(&soob,wpredok);
   }
  return(1);
 }


sprintf(strsql,"select kodm from Uplmt where kodm=%s limit 1",kod);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return(1);
   }
 }
else
 {   
  
  if(kolstr != 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Uplmt",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }
 }

sprintf(strsql,"select mz from Restdok1 where mz=0 and kodz=%s limit 1",kod);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return(1);
   }
 }
else
 {   
  
  if(kolstr != 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Restdok1",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }
 }


return(0);

}
/************************/
/*меню выбора режима удаления*/
/*****************************/
int  mater_mvru(GtkWidget *wpredok)
{
//char bros[512];

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Удаление выбранной записи"));
punkt_m.plus(gettext("Удалить неиспользуемые коды материалов"));


int nomer=0;
return(iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok));

}
/***********************************/
/*переоценка цены*/
/**********************************/

int  mater_prc(class mater_data *data)
{


iceb_u_str repl;
iceb_u_str procent;

repl.plus(gettext("Введите процент"));
repl.plus(" (+/-)");

if(iceb_menu_vvod1(&repl,&procent,20,data->window) != 0)
 return(1);

iceb_clock sss(data->window);

SQLCURSOR cur;
SQL_str row;
int kolstr;
char strsql[512];

sprintf(strsql,"select * from Material order by naimat asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }
if(kolstr == 0)
 return(1);
 
double krat=procent.ravno_atof();
double cena;
time_t vrem;
time(&vrem);

while(cur.read_cursor(&row) != 0)
 {

  if(mater_prov_row(row,data) != 0)
    continue;
 
    
  cena=atof(row[6]);
  if(cena == 0.)
    continue;

  if(krat >= 0.)        
   cena=cena+cena*krat/100;
  else
   cena=cena+cena*krat/(100-krat);
   
  cena=iceb_u_okrug(cena,0.01);
           
  sprintf(strsql,"update Material \
set \
cenapr=%.2f,\
ktoz=%d,\
vrem=%ld \
where kodm=%s",cena,iceb_getuid(data->window),vrem,row[0]);

  iceb_sql_zapis(strsql,0,0,data->window);

 }


return(0);

}
/**********************************/
/*выбор остатка*/
/*****************************/
int l_mater_vmost(GtkWidget *wpredok)
{
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Реальный остаток на карточках"));//0
punkt_m.plus(gettext("Остаток по всем документам (в том числе и по неподтверждённым)"));//1


int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);

switch(nomer)
 {
  case 0:
   return(1);
  case 1:
   return(2);

  default:
   return(0);   
 }
return(0);

}
