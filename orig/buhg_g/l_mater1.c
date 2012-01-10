/*$Id: l_mater1.c,v 1.19 2011-02-21 07:35:53 sasa Exp $*/
/*12.11.2008	05.05.2004	Белых А.И.	l_mater1.c
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
// FK3,
 FK4,
// SFK4,
 FK5,
// FK6,
 FK7,
// FK8,
// FK9,
 FK10,
// SFK2,
// SFK8,
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

class  mater1_data
 {
  public:
  mater_rek poisk;

  iceb_u_str kodv; //Код записи на которой стоим
  iceb_u_str naimv;
  iceb_u_str ei;
  double     cenav; 
  iceb_u_str kodzap; //Только что записанный код 

  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *label_sklad;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int     snanomer;   //номер записи на которую надостать
  int       metka_voz;  //0-выбрали 1-нет  
  short     metka_ost;  //0-без остатка 1-с остатком
  short     metka_cen; //0-c нулевой ценой не выбирать 1-выбирать
//  int skl;
  int skl_poi;
  int kolzap;
  short metka_skl;
  //Конструктор
  mater1_data()
   {
    snanomer=0;
    metka_skl=skl_poi=metka_ost=metka_voz=kl_shift=0;
    window=treeview=NULL;
    
   }      
 };

gboolean   mater1_key_press(GtkWidget *widget,GdkEventKey *event,class mater1_data *data);
void mater1_vibor(GtkTreeSelection *selection,class mater1_data *data);
void mater1_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class mater1_data *data);
void  mater1_knopka(GtkWidget *widget,class mater1_data *data);
void mater1_add_columns (GtkTreeView *treeview,short);
void l_mater_p(class  mater_rek *data_poi,GtkWidget *wpredok);
void mater1_rasp(class mater1_data *data);
int  mater1_prov_row(SQL_str row,class mater1_data *data);
void  l_mater1_zkm(char *kodm,GtkWidget *wpredok);
void mater1_create_list (class mater1_data *data);

int l_mater_dv(class mater_dv_data *datar,GtkWidget *wpredok);
void            dvmatw(class mater_dv_data*,int kodm,GtkWidget *wpredok);

extern char *organ;

int   l_mater1(iceb_u_str *kod,iceb_u_str *naim,
int skl_poi, // склад
const char *grup,
int metka_skl, //0-все 1-только материалы карточки которых есть на складе
int metka_cen, //0-с нулевой ценой не выбирать 1-выбирать с любой ценой
GtkWidget *wpredok)
{
printf("l_mater1-%s\n",naim->ravno());

class mater1_data data;
char bros[512];
GdkColor color;

data.poisk.clear_zero();

data.skl_poi=skl_poi;
data.metka_skl=metka_skl;
data.poisk.grupa.new_plus(grup);
data.metka_cen=metka_cen;

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(mater1_key_press),&data);
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

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(mater1_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);


sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(mater1_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(mater1_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);


sprintf(bros,"F7 %s",gettext("Остаток"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(mater1_knopka),&data);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Включить/выключить показ записей с остатками"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(mater1_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));


mater1_create_list(&data);

gtk_widget_grab_focus(data.knopka[FK10]);
gtk_widget_show(data.window);

// gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();

if(data.metka_voz == 0 )
 {
  kod->new_plus(data.kodv.ravno());
  naim->new_plus(data.naimv.ravno());
 }

printf("l_mater1 end\n");
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.metka_voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void mater1_create_list (class mater1_data *data)
{
iceb_clock skur(data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;
short	ddd,mmm,ggg;
iceb_u_poltekdat(&ddd,&mmm,&ggg);

//printf("mater1_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(mater1_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(mater1_vibor),data);

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

sprintf(strsql,"select * from Material order by naimat asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
//double ostatok;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  if(mater1_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->kodzap.ravno(),row[0],0) == 0)
    data->snanomer=data->kolzap;

  //Код
  ss[COL_KOD].new_plus(iceb_u_toutf(row[0]));

  //остаток
  if(data->metka_ost == 1)
   {
    sprintf(strsql,"%10.10g",ostdok1w(ddd,mmm,ggg,data->skl_poi,atoi(row[0])));
    ss[COL_OST].new_plus(strsql);
   }
  else
    ss[COL_OST].new_plus(" ");
    
  //Наименование
  ss[COL_NAIM].new_plus(iceb_u_toutf(row[2]));

  //Единица измерения
  ss[COL_EI].new_plus(iceb_u_toutf(row[4]));

  //Цена
  sprintf(strsql,"%10.10g",atof(row[6]));  
  ss[COL_CENA].new_plus(strsql);

  //Кратность
  sprintf(strsql,"%10.10g",atof(row[7]));  
  ss[COL_KRAT].new_plus(strsql);

  //Фасовка
  sprintf(strsql,"%10.10g",atof(row[8]));  
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
  COL_KOD,ss[COL_KOD].ravno(),
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

mater1_add_columns (GTK_TREE_VIEW (data->treeview),data->metka_ost);


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),TRUE);//Доступна
 }

data->kodzap.new_plus("");

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Список материалов"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);
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

}

/*****************/
/*Создаем колонки*/
/*****************/

void mater1_add_columns(GtkTreeView *treeview,short metka_ost)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

printf("mater1_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
//  g_object_set(G_OBJECT(renderer),"foreground","red",NULL);

//g_object_set(G_OBJECT(renderer),"background","black",NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_KOD,NULL);

renderer = gtk_cell_renderer_text_new ();
//g_object_set(G_OBJECT(renderer),"foreground","white",NULL);
//g_object_set(G_OBJECT(renderer),"background","black",NULL);

if(metka_ost == 1)
 {
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
  -1,gettext("Остаток"), renderer,"text", COL_OST,NULL);
 }
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("E/и"), renderer,"text", COL_EI,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Цена"), renderer,"text", COL_CENA,NULL);

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
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);
printf("mater1_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void mater1_vibor(GtkTreeSelection *selection,class mater1_data *data)
{
//printf("mater1_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gchar *naim;
gchar *cena;
gchar *ei;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_KOD,&kod,COL_NAIM,&naim,COL_CENA,&cena,COL_EI,&ei,NUM_COLUMNS,&nomer,-1);

data->kodv.new_plus(iceb_u_fromutf(kod));
data->naimv.new_plus(iceb_u_fromutf(naim));
data->cenav=atof(cena);
data->ei.new_plus(ei);
data->snanomer=nomer;

g_free(kod);
g_free(naim);
g_free(cena);
g_free(ei);

//printf("mater1_vibor%s %s %d\n",data->kodv.ravno(),data->naimv.ravno(),nomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  mater1_knopka(GtkWidget *widget,class mater1_data *data)
{
//mater1_dv_data datar;
iceb_u_str repl;
iceb_u_str skl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case FK2:
    if(data->metka_cen == 0)
     {
      if(data->cenav < 0.009)
       {
        iceb_menu_soob(gettext("Не введена цена на товар !"),data->window);
        return;
       }
      if(data->ei.getdlinna() <= 1)
       {
        iceb_menu_soob(gettext("Не введена единица измерения !"),data->window);
        return;
       }
     }


    data->metka_voz=0;
    gtk_widget_destroy(data->window);
    return;  


  case FK4:
    l_mater_p(&data->poisk,data->window);
    mater1_create_list(data);
    return;  


  case FK5:
    if(data->kolzap == 0)
      return;
    mater1_rasp(data);
    return;  

  case FK7:
    data->metka_ost++;
    if(data->metka_ost == 2)
     data->metka_ost=0;
    mater1_create_list(data);
    return;  


   
  case FK10:
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   mater1_key_press(GtkWidget *widget,GdkEventKey *event,class mater1_data *data)
{
iceb_u_str repl;
//printf("mater1_key_press keyval=%d state=%d\n",event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_F2:

    return(TRUE);
   
  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
    return(TRUE);

  case GDK_F5:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
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
    printf("mater1_key_press-Нажата клавиша Shift\n");

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
/*Выбор строки по двойному клику*/
/**********************/
void mater1_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class mater1_data *data)
{
 gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");

}

/****************************/
/*Проверка записей          */
/*****************************/

int  mater1_prov_row(SQL_str row,class mater1_data *data)
{

if(data->metka_skl == 1 && data->skl_poi != 0)
 {
  char strsql[512];
  sprintf(strsql,"select kodm from Kart where sklad=%d and kodm=%s limit 1",data->skl_poi,row[0]);
  if(sql_readkey(&bd,strsql) <= 0)
    return(5);  

 }

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
void mater1_rasp(class mater1_data *data)
{
char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;

sprintf(strsql,"select * from Material order by naimat asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
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
iceb_u_startfil(ff);

iceb_u_zagolov(gettext("Список материалов"),0,0,0,0,0,0,organ,ff);

fprintf(ff,"\
------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Код |Наименование        |Дата и время запис.| Кто записал\n"));

fprintf(ff,"\
------------------------------------------------------------------------\n");



//int i=0;

iceb_u_str s5;
iceb_u_str s6;


cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  if(mater1_prov_row(row,data) != 0)
    continue;

  //Дата и время записи
  s5.new_plus(iceb_u_vremzap(row[11]));

  //Кто записал
  s6.new_plus(iceb_kszap(row[10],1,data->window));

  fprintf(ff,"%-4s %-20.20s %s %s\n",
  row[0],row[2],s5.ravno(),s6.ravno());


 }
fprintf(ff,"\
------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);

iceb_rabfil(&imaf,&naimot,"",0,data->window);

}
