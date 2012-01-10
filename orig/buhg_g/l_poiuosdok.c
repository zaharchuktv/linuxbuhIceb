/*$Id: l_poiuosdok.c,v 1.11 2011-02-21 07:35:53 sasa Exp $*/
/*15.01.2008	15.01.2008	Белых А.И.	l_poiuosdok.c
поиск документа по номеру документа в учёте основных средств
*/
#include <stdlib.h>
#include "buhg_g.h"

enum
{
 FK2,
 FK10,
 KOL_F_KL
};

enum
{
 COL_METKA,
 COL_DATADOK,
 COL_PODR,
 COL_NOMDOK,
 COL_KODOP,
 COL_KONTR,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  poiuosdok_data
 {
  public:

  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       kolzap;
  int       snanomer;  

  iceb_u_str nomdok_p; //Номер документа который ищем
  
  //реквизиты документа который выбрали для просмотра
  iceb_u_str datadok;
  iceb_u_str podr;
  iceb_u_str nomdok;
  int        tipz;
  
  //Конструктор
  poiuosdok_data()
   {
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
   }      
 };

gboolean   poiuosdok_key_press(GtkWidget *widget,GdkEventKey *event,class poiuosdok_data *data);
void poiuosdok_vibor(GtkTreeSelection *selection,class poiuosdok_data *data);
void poiuosdok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class poiuosdok_data *data);
void  poiuosdok_knopka(GtkWidget *widget,class poiuosdok_data *data);
void poiuosdok_add_columns (GtkTreeView *treeview);
void poiuosdok_create_list(class poiuosdok_data *data);


extern SQL_baza	bd;
extern char *name_system;

void  l_poiuosdok(const char *nomdok,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str soob;
class poiuosdok_data data;
data.nomdok_p.new_plus(nomdok);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Поиск документа по номеру"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(poiuosdok_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

soob.new_plus("");

data.label_kolstr=gtk_label_new (soob.ravno_toutf());


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];


sprintf(strsql,"F2 %s",gettext("Просмотр"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(poiuosdok_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Просмотр выбранного документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(poiuosdok_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

poiuosdok_create_list(&data);

gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));

gtk_main();


printf("l_poiuosdok end\n");
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void poiuosdok_create_list (class poiuosdok_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;

data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(poiuosdok_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(poiuosdok_vibor),data);

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


sprintf(strsql,"select datd,podr,nomd,kontr,kodop,podt,prov,tipz,ktoz,vrem from Uosdok where nomd='%s' \
order by datd desc",data->nomdok_p.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;


while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  //Дата  
  if(row[7][0] == '1')
   ss[COL_METKA].new_plus("+");
  if(row[7][0] == '2')
   ss[COL_METKA].new_plus("-");
  if(row[5][0] == '0')
   ss[COL_METKA].plus("?");
  if(row[6][0] == '0')
   ss[COL_METKA].plus("*");

  //подразделение
  ss[COL_PODR].new_plus(row[1]);

  //Дата документа
  ss[COL_DATADOK].new_plus(iceb_u_sqldata(row[0]));

  //Номер документа
  ss[COL_NOMDOK].new_plus(iceb_u_toutf(row[2]));

  //Код операции
  ss[COL_KODOP].new_plus(iceb_u_toutf(row[4]));


  //Код контрагента
  ss[COL_KONTR].new_plus(iceb_u_toutf(row[3]));
  

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[9])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[8],0,data->window));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_METKA,ss[COL_METKA].ravno(),
  COL_PODR,ss[COL_PODR].ravno(),
  COL_DATADOK,ss[COL_DATADOK].ravno(),
  COL_NOMDOK,ss[COL_NOMDOK].ravno(),
  COL_KODOP,ss[COL_KODOP].ravno(),
  COL_KONTR,ss[COL_KONTR].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

poiuosdok_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


iceb_u_str zagolov;
zagolov.plus(gettext("Поиск документа по номеру"));
zagolov.plus(" ");
zagolov.plus(data->nomdok_p.ravno());
zagolov.plus(" ");

zagolov.plus(gettext("Количество записей"));
zagolov.plus(":");
zagolov.plus(data->kolzap);


gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));


}

/*****************/
/*Создаем колонки*/
/*****************/

void poiuosdok_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

printf("poiuosdok_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,"M", renderer,"text", COL_METKA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата док-та"), renderer,"text", COL_DATADOK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Подразделение"), renderer,"text", COL_PODR,NULL);

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
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,"text", COL_KTO,NULL);

printf("poiuosdok_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void poiuosdok_vibor(GtkTreeSelection *selection,class poiuosdok_data *data)
{
printf("poiuosdok_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *metka;
gchar *datadok;
gchar *podr;
gchar *nomdok;
gint nomer;

gtk_tree_model_get(model,&iter,COL_METKA,&metka,
COL_DATADOK,&datadok,COL_PODR,&podr,COL_NOMDOK,&nomdok,NUM_COLUMNS,&nomer,-1);

data->datadok.new_plus(datadok);
data->podr.new_plus(podr);
data->nomdok.new_plus(nomdok);
data->snanomer=nomer;
if(metka[0] == '+')
 data->tipz=1;
if(metka[0] == '-')
 data->tipz=2;
 
g_free(metka);
g_free(datadok);
g_free(podr);
g_free(nomdok);

//printf("%s %s %s %d %s %f\n",data->datapv.ravno(),data->datadv.ravno(),data->nomdokv.ravno(),data->nomervsp,
//data->tipv.ravno(),data->kolihv);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  poiuosdok_knopka(GtkWidget *widget,class poiuosdok_data *data)
{
short dd,md,gd;
char strsql[512];
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("poiuosdok_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case FK2:
//    l_dokmat(&data->datadok,data->tipz,&data->sklad,&data->nomdok,data->window);
    iceb_u_rsdat(&dd,&md,&gd,data->datadok.ravno(),1);
//    l_usldok(dd,md,gd,data->tipz,data->podr.ravno_atoi(),data->nomdok.ravno(),data->window);
    sprintf(strsql,"%d.%d.%d",dd,md,gd);
    l_uosdok(strsql,data->nomdok.ravno(),data->window);
    poiuosdok_create_list(data);
    return;  


    
  case FK10:
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   poiuosdok_key_press(GtkWidget *widget,GdkEventKey *event,class poiuosdok_data *data)
{
iceb_u_str repl;
//printf("poiuosdok_key_press keyval=%d state=%d\n",event->keyval,event->state);

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
    printf("poiuosdok_key_press-Нажата клавиша Shift\n");

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
void poiuosdok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class poiuosdok_data *data)
{
//printf("poiuosdok_v_row\n");
//printf("poiuosdok_v_row корректировка\n");

//gtk_widget_destroy(data->window);

//data->metka_voz=0;
  gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");

}
