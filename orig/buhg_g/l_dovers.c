/*$Id: l_dovers.c,v 1.12 2011-02-21 07:35:52 sasa Exp $*/
/*22.06.2010	21.04.2009	Белых А.И.	l_dovers.c
Работа со списком доверенностей
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
#include  "buhg_g.h"
#include "l_dovers.h"
enum
{
  COL_METKA,
  COL_DATAD,
  COL_NOMERD,
  COL_KOMU,
  COL_POST,
  COL_OTMISP,
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
  KOL_F_KL
};

class  l_dovers_data
 {
  public:


  
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  

  short dn,mn,gn;
  class l_dovers_poi rp;  

  class iceb_u_str datav;
  class iceb_u_str nomerv;
  class iceb_u_str zapros;  
  //Конструктор
  l_dovers_data()
   {
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
    dn=mn=gn=0;  
   }      
 };

gboolean   l_dovers_key_press(GtkWidget *widget,GdkEventKey *event,class l_dovers_data *data);
void l_dovers_vibor(GtkTreeSelection *selection,class l_dovers_data *data);
void l_dovers_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_dovers_data *data);
void  l_dovers_knopka(GtkWidget *widget,class l_dovers_data *data);
void l_dovers_add_columns (GtkTreeView *treeview);
void l_dovers_create_list (class l_dovers_data *data);

int  l_dovers_prov_row(SQL_str row,class  l_dovers_data *data);
int l_dovers_p(class l_dovers_poi *rek_poi,GtkWidget *wpredok);
void l_dovers_rr(class  l_dovers_data *data);


extern char *organ;
extern SQL_baza  bd;
extern char      *name_system;

int l_dovers(short dp,short mp,short gp,GtkWidget *wpredok)
{
l_dovers_data data;
char bros[512];
GdkColor color;
short startgodud=0;

if(iceb_poldan("Стартовый год",bros,"nastud.alx",wpredok) == 0)
 startgodud=atoi(bros);

if(dp == 0 && startgodud != 0)
 {
  dp=1; mp=1; gp=startgodud;
 }
if(dp != 0)
 {
  data.dn=dp;
  data.mn=mp;
  data.gn=gp;
  
 } 

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Список доверенностей"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_dovers_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Список доверенностей"));
//gdk_color_parse("green",&color);
//gtk_widget_modify_fg(data.label_kolstr,GTK_STATE_NORMAL,&color);


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");
gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_poisk,GTK_STATE_NORMAL,&color);

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];


sprintf(bros,"F2 %s",gettext("Просмотр"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(l_dovers_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Просмотр выбранного документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"F3 %s",gettext("Использование"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(l_dovers_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Переключение просмотра списка документов. Только неиспользованные, только использованные, все"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);


sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(l_dovers_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(l_dovers_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка списка документов"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(l_dovers_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_dovers_create_list(&data);

gtk_widget_show(data.window);
//if(metka_rr == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.metka_voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_dovers_create_list (class l_dovers_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);
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

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_dovers_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_dovers_vibor),data);

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
G_TYPE_INT);


sprintf(strsql,"select * from Uddok where datd >='%d-%d-%d'",data->gn,data->mn,data->dn);

if(data->rp.metka_poi == 1)
 {
  if(data->rp.datan.getdlinna() > 1)
   sprintf(strsql,"select * from Uddok where datd >='%s'",
   data->rp.datan.ravno_sqldata());
   
  if(data->rp.datan.getdlinna() > 1 && data->rp.datak.getdlinna() > 1)
   sprintf(strsql,"select * from Uddok where datd >='%s' and datd <= '%s'",
   data->rp.datan.ravno_sqldata(),
   data->rp.datak.ravno_sqldata());
 }

data->zapros.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

class iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(l_dovers_prov_row(row,data) != 0)
   continue;
  ss[COL_METKA].new_plus("");
  if(row[6][0] == '\0')
   ss[COL_METKA].new_plus("*");
  ss[COL_DATAD].new_plus(iceb_u_datzap(row[0]));
  ss[COL_NOMERD].new_plus(iceb_u_toutf(row[1]));
  ss[COL_KOMU].new_plus(iceb_u_toutf(row[3]));
  ss[COL_POST].new_plus(iceb_u_toutf(row[4]));
  ss[COL_OTMISP].new_plus(iceb_u_toutf(row[6]));
  

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[15])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[14],0,data->window));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_METKA,ss[COL_METKA].ravno(),
  COL_DATAD,ss[COL_DATAD].ravno(),
  COL_NOMERD,ss[COL_NOMERD].ravno(),
  COL_KOMU,ss[COL_KOMU].ravno(),
  COL_POST,ss[COL_POST].ravno(),
  COL_OTMISP,ss[COL_OTMISP].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_dovers_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


iceb_u_str stroka;
iceb_u_str zagolov;
if(data->rp.metka_isp == 0)
  zagolov.plus(gettext("Список доверенностей"));
if(data->rp.metka_isp == 1)
  zagolov.plus(gettext("Список только неиспользованных документов"));
if(data->rp.metka_isp == 2)
  zagolov.plus(gettext("Список только использованных документов"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());


gtk_widget_show(data->label_kolstr);

if(data->rp.metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->rp.datan.ravno(),gettext("Дата начала"));
  iceb_str_poisk(&zagolov,data->rp.datak.ravno(),gettext("Дата конца"));
  iceb_str_poisk(&zagolov,data->rp.nomdov.ravno(),gettext("Ном.док."));
  iceb_str_poisk(&zagolov,data->rp.komu.ravno(),gettext("Кому"));
  iceb_str_poisk(&zagolov,data->rp.post.ravno(),gettext("Поставщик"));
  

  gtk_label_set_text(GTK_LABEL(data->label_poisk),zagolov.ravno_toutf());
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 

}

/*****************/
/*Создаем колонки*/
/*****************/

void l_dovers_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("M",renderer,"text",COL_METKA,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_METKA);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Дата д-та"),renderer,"text",COL_DATAD,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DATAD);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Ном.док."),renderer,"text",COL_NOMERD,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NOMERD);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Кому"),renderer,"text",COL_KOMU,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOMU);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Поставщик"),renderer,"text",COL_POST,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_POST);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Отметка использования"),renderer,"text",COL_OTMISP,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_OTMISP);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Дата и время записи"),renderer,"text",COL_DATA_VREM,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DATA_VREM);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Кто записал"),renderer,"text",COL_KTO,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KTO);
gtk_tree_view_append_column (treeview, column);
}

/****************************/
/*Выбор строки*/
/**********************/

void l_dovers_vibor(GtkTreeSelection *selection,class l_dovers_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *datav;
gchar *nomerv;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_DATAD,&datav,COL_NOMERD,&nomerv,NUM_COLUMNS,&nomer,-1);

data->datav.new_plus(datav);
data->nomerv.new_plus(iceb_u_fromutf(nomerv));
data->snanomer=nomer;

g_free(datav);
g_free(nomerv);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_dovers_knopka(GtkWidget *widget,class l_dovers_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_dovers_knopka knop=%d\n",knop);

switch (knop)
 {

  case FK2:
    l_dover(data->datav.ravno(),data->nomerv.ravno(),data->window);
    l_dovers_create_list(data);
    return;  

  case FK3:
    data->rp.metka_isp++;
    if(data->rp.metka_isp >= 3)
     data->rp.metka_isp=0;    
    l_dovers_create_list(data);
    return;  
  case FK4:
    l_dovers_p(&data->rp,data->window);
    l_dovers_create_list(data);
    return;  

  case FK5:
    l_dovers_rr(data);
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

gboolean   l_dovers_key_press(GtkWidget *widget,GdkEventKey *event,class l_dovers_data *data)
{
iceb_u_str repl;

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
void l_dovers_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_dovers_data *data)
{
//printf("l_dovers_v_row\n");
//printf("l_dovers_v_row корректировка\n");
  gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");


}

/****************************/
/*Проверка записей          */
/*****************************/

int  l_dovers_prov_row(SQL_str row,class  l_dovers_data *data)
{
if(data->rp.metka_isp == 1) /*только не использованые*/
 if(row[6][0] != '\0')
  return(1);

if(data->rp.metka_isp == 2) /*Только использованые*/
 if(row[6][0] == '\0')
  return(1);

if(data->rp.metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->rp.nomdov.ravno(),row[1],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->rp.komu.ravno(),row[3],4,0) != 0)
 return(1);


if(iceb_u_proverka(data->rp.post.ravno(),row[4],4,0) != 0)
 return(1);



   
return(0);
}
/*******************************/
/*Распечатка реестра доверенностей*/
/**********************************/
void l_dovers_rr(class  l_dovers_data *data)
{
SQL_str row;
FILE *ff;
char imaf[54];
char datainom[108];
sprintf(imaf,"rdov%d.lst",getpid());
int kolstr=0;
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return;
 }


if((ff=fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }
iceb_u_startfil(ff);
iceb_u_startfil(ff);
fprintf(ff,"\x1B\x4D"); /*12-знаков*/

iceb_u_zagolov(gettext("Реестр доверенностей"),data->rp.datan.ravno(),data->rp.datak.ravno(),organ,ff);
if(data->rp.komu.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Кому"),data->rp.komu.ravno());
if(data->rp.post.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Поставщик"),data->rp.post.ravno());
if(data->rp.nomdov.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Номер документа"),data->rp.nomdov.ravno());
 
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
Дата док. |Номер док.|Действ. до|   Кому выдана      |    Поставщик       |Дата и номер наряда |Омтетка об использовании\n");
/*
1234567890 1234567890 1234567890 12345678901234567890 12345678901234567890 12345678901234567890 1234567890123456789012345678901 
*/

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------\n");

while(cur.read_cursor(&row) != 0)
 {
  if(l_dovers_prov_row(row,data) != 0)
   continue;
  sprintf(datainom,"%s %s",iceb_u_datzap(row[7]),row[8]);  
  fprintf(ff,"%-10s %-*s %-10s %-*.*s %-*.*s %-*.*s %-30.30s\n",
  iceb_u_datzap(row[0]),
  iceb_u_kolbait(10,row[1]),row[1],
  iceb_u_datzap(row[2]),
  iceb_u_kolbait(20,row[3]),iceb_u_kolbait(20,row[3]),row[3],
  iceb_u_kolbait(20,row[4]),iceb_u_kolbait(20,row[4]),row[4],
  iceb_u_kolbait(20,datainom),iceb_u_kolbait(20,datainom),datainom,
  row[6]);  
 }
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------\n");

iceb_podpis(ff,data->window);
fclose(ff);

class iceb_u_spisok imafr;
class iceb_u_spisok naimf;
imafr.plus(imaf);
naimf.plus(gettext("Реестр доверенностей"));
iceb_rabfil(&imafr,&naimf,"",0,data->window);

}
