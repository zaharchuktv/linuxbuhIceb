/*$Id: l_uplsal.c,v 1.12 2011-02-21 07:35:54 sasa Exp $*/
/*12.11.2009	07.03.2006	Белых А.И.	l_uplsal.c
Работа со списком сальдо по топливу
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
#include  "buhg_g.h"
#include  "l_uplsal.h"

enum
{
  COL_GOD,
  COL_KOD_AVT,
  COL_KOD_VOD,
  COL_KOD_TOP,
  COL_SALDO,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

enum
{
  FK2,
  FK3,
  FK4,
  FK5,
  FK10,
  SFK2,
  KOL_F_KL
};

class  l_uplsal_data
 {
  public:

  l_uplsal_rek poisk;

  iceb_u_str godv;
  iceb_u_str kod_avtv;
  iceb_u_str kod_vodv;
  iceb_u_str kod_topv;
  
  iceb_u_str god_tv;
  iceb_u_str kod_avt_tv;
  iceb_u_str kod_vod_tv;
  iceb_u_str kod_top_tv;
  
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  short     metka_poi;  //0-без поиска 1-с поиском

  //Конструктор
  l_uplsal_data()
   {
    snanomer=0;
    kl_shift=metka_poi=0;
    window=treeview=NULL;
    god_tv.plus("");
    kod_avt_tv.plus("");
    kod_vod_tv.plus("");
    kod_top_tv.plus("");
   }      
 };

gboolean   l_uplsal_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplsal_data *data);
void l_uplsal_vibor(GtkTreeSelection *selection,class l_uplsal_data *data);
void l_uplsal_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_uplsal_data *data);
void  l_uplsal_knopka(GtkWidget *widget,class l_uplsal_data *data);
void l_uplsal_add_columns (GtkTreeView *treeview);
void l_uplsal_udzap(class l_uplsal_data *data);
int  l_uplsal_prov_row(SQL_str row,class l_uplsal_data *data);
void l_uplsal_rasp(class l_uplsal_data *data);
void l_uplsal_create_list (class l_uplsal_data *data);

int l_uplsal_v(class iceb_u_str *god,class iceb_u_str *kod_avt,class iceb_u_str *kod_vod,class iceb_u_str *kod_top,GtkWidget *wpredok);
int l_uplsal_p(class l_uplsal_rek *rek_poi,GtkWidget *wpredok);


extern char *organ;
extern SQL_baza  bd;
extern char      *name_system;

void   l_uplsal(GtkWidget *wpredok)
{
l_uplsal_data data;
char bros[512];
GdkColor color;

data.poisk.clear_data();

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Сальдо по автомобилям, водителям и кодам топлива"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_uplsal_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE,1);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Сальдо по автомобилям, водителям и кодам топлива"));
//gdk_color_parse("green",&color);
//gtk_widget_modify_fg(data.label_kolstr,GTK_STATE_NORMAL,&color);


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE,1);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");
gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_poisk,GTK_STATE_NORMAL,&color);

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE,1);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE,1);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE,1);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(l_uplsal_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(l_uplsal_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(l_uplsal_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(l_uplsal_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE,1);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(l_uplsal_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE,1);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(l_uplsal_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_uplsal_create_list(&data);

gtk_widget_show(data.window);
gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return;

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_uplsal_create_list (class l_uplsal_data *data)
{
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;
//GdkColor color;

data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_uplsal_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_uplsal_vibor),data);

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
G_TYPE_INT);


sprintf(strsql,"select * from Uplsal order by god,ka,kv,kmt asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
SQL_str row1;
class SQLCURSOR cur1;
class iceb_u_str naim("");
double itogo=0.;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  if(l_uplsal_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->god_tv.ravno(),row[0],0) == 0 && \
     iceb_u_SRAV(data->kod_avt_tv.ravno(),row[1],0) == 0 && \
     iceb_u_SRAV(data->kod_vod_tv.ravno(),row[2],0) == 0 && \
     iceb_u_SRAV(data->kod_top_tv.ravno(),row[3],0) == 0)
         data->snanomer=data->kolzap;

  //Год
  ss[COL_GOD].new_plus(iceb_u_toutf(row[0]));
  
  //КОД автомобиля
  naim.new_plus("");
  sprintf(strsql,"select naik from Uplavt where kod=%d",atoi(row[1]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim.new_plus(row1[0]);
  sprintf(strsql,"%s %s",row[1],naim.ravno_toutf());
  ss[COL_KOD_AVT].new_plus(strsql);

  //Код водителя
  naim.new_plus("");
  sprintf(strsql,"select naik from Uplouot where kod=%d",atoi(row[2]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim.new_plus(row1[0]);
  sprintf(strsql,"%s %s",row[2],naim.ravno_toutf());
  ss[COL_KOD_VOD].new_plus(strsql);

  //Код топлива
  ss[COL_KOD_TOP].new_plus(iceb_u_toutf(row[3]));

  //Сальдо
  ss[COL_SALDO].new_plus(iceb_u_toutf(row[4]));

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[6])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[5],0,data->window));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_GOD,ss[COL_GOD].ravno(),
  COL_KOD_AVT,ss[COL_KOD_AVT].ravno(),
  COL_KOD_VOD,ss[COL_KOD_VOD].ravno(),
  COL_KOD_TOP,ss[COL_KOD_TOP].ravno(),
  COL_SALDO,ss[COL_SALDO].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);
  itogo+=atof(row[4]);
  data->kolzap++;
 }
data->god_tv.new_plus("");
data->kod_avt_tv.new_plus("");
data->kod_vod_tv.new_plus("");
data->kod_top_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_uplsal_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Сальдо по автомобилям, водителям и кодам топлива"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

sprintf(strsql," %s:%.10g",gettext("Сумма"),itogo);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

if(data->metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.god.ravno(),gettext("Год"));
  iceb_str_poisk(&zagolov,data->poisk.kod_vod.ravno(),gettext("Код водителя"));
  iceb_str_poisk(&zagolov,data->poisk.kod_top.ravno(),gettext("Код топлива"));
  iceb_str_poisk(&zagolov,data->poisk.kod_avt.ravno(),gettext("Код автомобиля"));
  

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

void l_uplsal_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;


renderer = gtk_cell_renderer_text_new ();
//  g_object_set(G_OBJECT(renderer),"foreground","red",NULL);

//g_object_set(G_OBJECT(renderer),"background","black",NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Год"), renderer,"text", COL_GOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код автомобиля"), renderer,"text", COL_KOD_AVT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код водителя"), renderer,"text", COL_KOD_VOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код топлива"), renderer,"text", COL_KOD_TOP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Сальдо"), renderer,"text", COL_SALDO,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

}

/****************************/
/*Выбор строки*/
/**********************/

void l_uplsal_vibor(GtkTreeSelection *selection,class l_uplsal_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *god;
gchar *kod_avt;
gchar *kod_vod;
gchar *kod_top;
gint  nomer;


gtk_tree_model_get(model,&iter,
COL_GOD,&god,
COL_KOD_AVT,&kod_avt,
COL_KOD_VOD,&kod_vod,
COL_KOD_TOP,&kod_top,
NUM_COLUMNS,&nomer,-1);

data->godv.new_plus(iceb_u_fromutf(god));

if(iceb_u_pole(kod_avt,&data->kod_avtv,1,' ') != 0)
 data->kod_avtv.new_plus(kod_avt);

if(iceb_u_pole(kod_vod,&data->kod_vodv,1,' ') != 0)
 data->kod_vodv.new_plus(kod_vod);
 
data->kod_topv.new_plus(iceb_u_fromutf(kod_top));
data->snanomer=nomer;

g_free(god);
g_free(kod_avt);
g_free(kod_vod);
g_free(kod_top);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_uplsal_knopka(GtkWidget *widget,class l_uplsal_data *data)
{
//char    bros[512];
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uplsal_knopka knop=%d\n",knop);
data->kl_shift=0;
switch (knop)
 {
  case FK2:
    if(l_uplsal_v(&data->god_tv,&data->kod_avt_tv,&data->kod_vod_tv,&data->kod_top_tv,data->window) == 0)
      l_uplsal_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    if(l_uplsal_v(&data->godv,&data->kod_avtv,&data->kod_vodv,&data->kod_topv,data->window) == 0)
      l_uplsal_create_list(data);

    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    l_uplsal_udzap(data);
    l_uplsal_create_list(data);
    return;  
  

  case FK4:
    if(l_uplsal_p(&data->poisk,data->window) == 0)
     data->metka_poi=1;
    else
     data->metka_poi=0;
    l_uplsal_create_list(data);
    return;  

  case FK5:
    l_uplsal_rasp(data);
    return;  

    
  case FK10:
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uplsal_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplsal_data *data)
{
iceb_u_str repl;

switch(event->keyval)
 {

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
void l_uplsal_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_uplsal_data *data)
{

gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");


}

/*****************************/
/*Удаление записи            */
/*****************************/

void l_uplsal_udzap(class l_uplsal_data *data)
{

if(iceb_pbpds(1,data->godv.ravno_atoi(),data->window) != 0)
 return;

if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;


char strsql[512];

sprintf(strsql,"delete from Uplsal where god=%d and ka=%d and kv=%d and kmt='%s'",
data->godv.ravno_atoi(),
data->kod_avtv.ravno_atoi(),
data->kod_vodv.ravno_atoi(),
data->kod_topv.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/****************************/
/*Проверка записей          */
/*****************************/

int  l_uplsal_prov_row(SQL_str row,class l_uplsal_data *data)
{
if(data->metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->poisk.god.ravno(),row[0],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->poisk.kod_avt.ravno(),row[1],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->poisk.kod_top.ravno(),row[3],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->poisk.kod_vod.ravno(),row[2],0,0) != 0)
 return(1);


   
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_uplsal_rasp(class l_uplsal_data *data)
{
char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;

sprintf(strsql,"select * from Uplsal order by god,ka,kv,kmt asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"uplsal%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Сальдо по автомобилям, водителям и кодам топлива"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_u_zagolov(gettext("Сальдо по автомобилям, водителям и кодам топлива"),0,0,0,0,0,0,organ,ff);

if(data->metka_poi == 1)
 {


  if(data->poisk.god.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Год"),data->poisk.god.ravno());

  if(data->poisk.kod_vod.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Код водителя"),data->poisk.kod_vod.ravno());

  if(data->poisk.kod_avt.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Код автомобиля"),data->poisk.kod_avt.ravno());

  if(data->poisk.kod_vod.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Код водителя"),data->poisk.kod_vod.ravno());
 }

fprintf(ff,"\
--------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Год |     Автомобиль     |      Водитель      |Топливо|  Сальдо  |Дата и время запис.|Кто записал\n"));
//   12345678901234567890 12345678901234567890 1234567 1234567890
fprintf(ff,"\
--------------------------------------------------------------------------------------------------\n");

iceb_u_str s5;
iceb_u_str s6;

class iceb_u_str naim_avt("");
class iceb_u_str naim_vod("");
SQL_str row1;
class SQLCURSOR cur1;
double itogo=0.;
while(cur.read_cursor(&row) != 0)
 {
  if(l_uplsal_prov_row(row,data) != 0)
    continue;

  //КОД автомобиля
  naim_avt.new_plus(row[1]);
  sprintf(strsql,"select naik from Uplavt where kod=%d",atoi(row[1]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    naim_avt.plus(" ");
    naim_avt.plus(row1[0]);
   }
  //Код водителя
  naim_vod.new_plus(row[2]);
  sprintf(strsql,"select naik from Uplouot where kod=%d",atoi(row[2]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    naim_vod.plus(" ");
    naim_vod.plus(row1[0]);
   }

  //Дата и время записи
  s5.new_plus(iceb_u_vremzap(row[6]));

  //Кто записал
  s6.new_plus(iceb_kszap(row[5],1,data->window));

  fprintf(ff,"%-4s %-*.*s %-*.*s %-*s %10s %s %s\n",
  row[0],
  iceb_u_kolbait(20,naim_avt.ravno()),iceb_u_kolbait(20,naim_avt.ravno()),naim_avt.ravno(),
  iceb_u_kolbait(20,naim_vod.ravno()),iceb_u_kolbait(20,naim_vod.ravno()),naim_vod.ravno(),
  iceb_u_kolbait(7,row[3]),row[3],
  row[4],s5.ravno(),s6.ravno());
  itogo+=atof(row[4]);
 }
fprintf(ff,"\
--------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(54,gettext("Итого")),gettext("Итого"),itogo);

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,"",0,data->window);

}
