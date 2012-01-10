/*$Id: l_uosgrnu.c,v 1.15 2011-05-06 08:37:47 sasa Exp $*/
/*12.11.2009	18.10.2007	Белых А.И.	l_uosgrnu.c
Работа со списком групп налогового учёта в учёте основных средств
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
#include  "iceb_libbuh.h"
#include  "l_uosgrnu.h"

enum
{
  COL_KOD,
  COL_NAIM,
  COL_PROC,
  COL_TA,
  COL_AR,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

enum
{
  FK2,
  FK3,
  SFK3,
  FK4,
  FK5,
  FK10,
  SFK2,
  KOL_F_KL
};

class  l_uosgrnu_data
 {
  public:

  l_uosgrnu_rek poisk;

  iceb_u_str kodv;
  iceb_u_str naimv;
  iceb_u_str kod_podr_tv; //только что введённая единица измерения
  
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  int     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  short     metka_poi;  //0-без поиска 1-с поиском
  int       metka_voz;  //0-выбрали 1-нет  
  short     metka_rr;   //0-работа со списком 1-выбор
  //Конструктор
  l_uosgrnu_data()
   {
    snanomer=0;
    metka_rr=metka_voz=kl_shift=metka_poi=0;
    window=treeview=NULL;
    kod_podr_tv.plus("");
   }      
 };

gboolean   l_uosgrnu_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosgrnu_data *data);
void l_uosgrnu_vibor(GtkTreeSelection *selection,class l_uosgrnu_data *data);
void l_uosgrnu_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_uosgrnu_data *data);
void  l_uosgrnu_knopka(GtkWidget *widget,class l_uosgrnu_data *data);
void l_uosgrnu_add_columns (GtkTreeView *treeview);
void l_uosgrnu_udzap(class l_uosgrnu_data *data);
int  l_uosgrnu_prov_row(SQL_str row,class l_uosgrnu_data *data);
void l_uosgrnu_rasp(class l_uosgrnu_data *data);
int  l_uosgrnu_pvu(int,const char *kod,GtkWidget *wpredok);
void l_uosgrnu_create_list (class l_uosgrnu_data *data);

int l_uosgrnu_v(class iceb_u_str *kod_gr,GtkWidget *wpredok);
int l_uosgrnu_p(class l_uosgrnu_rek *rek_poi,GtkWidget *wpredok);
void l_gruppdok_unk(class l_uosgrnu_data *data);


extern char *organ;
extern SQL_baza  bd;
extern char      *name_system;

int   l_uosgrnu(int metka_rr, //0-ввод и корек. 1-выбор
iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok)
{
l_uosgrnu_data data;
char bros[512];
GdkColor color;
data.poisk.clear_data();
data.metka_rr=metka_rr;

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);


sprintf(bros,"%s %s",name_system,gettext("Список групп налогового учёта"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_uosgrnu_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new(gettext("Список групп налогового учёта"));


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
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(l_uosgrnu_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(l_uosgrnu_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(l_uosgrnu_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(l_uosgrnu_knopka),&data);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Удалить неиспользуемые коды"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(l_uosgrnu_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(l_uosgrnu_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(l_uosgrnu_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_uosgrnu_create_list(&data);

gtk_widget_show(data.window);

if(metka_rr == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));


gtk_main();

if(data.metka_voz == 0)
 {
  kod->new_plus(data.kodv.ravno());
  naim->new_plus(data.naimv.ravno());
 }

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.metka_voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_uosgrnu_create_list (class l_uosgrnu_data *data)
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

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_uosgrnu_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_uosgrnu_vibor),data);

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


sprintf(strsql,"select * from Uosgrup order by kod asc");

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
  
  if(l_uosgrnu_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->kod_podr_tv.ravno(),row[0],0) == 0)
    data->snanomer=data->kolzap;

  //Код
  ss[COL_KOD].new_plus(iceb_u_toutf(row[0]));
  
  //Наименование
  ss[COL_NAIM].new_plus(iceb_u_toutf(row[1]));

  //Процент амортизации
  ss[COL_PROC].new_plus(row[2]);

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[4])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[3],0,data->window));
  
  if(atoi(row[5]) == 0)
   ss[COL_TA].new_plus(gettext("Пообъектно"));  
  else
   ss[COL_TA].new_plus(gettext("В целом по группе"));  

  if(atoi(row[6]) == 0)
   ss[COL_AR].new_plus(gettext("От начальной балансовой стоимости"));  
  else
   ss[COL_AR].new_plus(gettext("От остаточной балансовой стоимости"));  

  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD,ss[COL_KOD].ravno(),
  COL_NAIM,ss[COL_NAIM].ravno(),
  COL_PROC,ss[COL_PROC].ravno(),
  COL_TA,ss[COL_TA].ravno(),
  COL_AR,ss[COL_AR].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->kod_podr_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_uosgrnu_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Список групп налогового учёта"));

sprintf(strsql,". %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

if(data->metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.kod.ravno(),gettext("Код"));
  iceb_str_poisk(&zagolov,data->poisk.naim.ravno(),gettext("Наименование"));
  

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

void l_uosgrnu_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_KOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Процент"), renderer,"text", COL_PROC,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Тип амортизации"), renderer,"text",COL_TA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Алгоритм амортизации"), renderer,"text",COL_AR,NULL);



renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

}

/****************************/
/*Выбор строки*/
/**********************/

void l_uosgrnu_vibor(GtkTreeSelection *selection,class l_uosgrnu_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gchar *naim;
gint  nomer;


gtk_tree_model_get(model,&iter,0,&kod,1,&naim,NUM_COLUMNS,&nomer,-1);

data->kodv.new_plus(iceb_u_fromutf(kod));
data->naimv.new_plus(iceb_u_fromutf(naim));
data->snanomer=nomer;

g_free(kod);
g_free(naim);

//printf("%s %s %d\n",data->kodv.ravno(),data->naimv.ravno(),nomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_uosgrnu_knopka(GtkWidget *widget,class l_uosgrnu_data *data)
{
//char    bros[512];
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uosgrnu_knopka knop=%d\n",knop);
data->kl_shift=0; //0-отжата 1-нажата  

switch (knop)
 {
  case FK2:
    l_uosgrnu_v(&data->kod_podr_tv,data->window);

    l_uosgrnu_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    l_uosgrnu_v(&data->kodv,data->window);

    l_uosgrnu_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    l_uosgrnu_udzap(data);
    l_uosgrnu_create_list(data);
    return;  
  
  case SFK3:
    if(data->kolzap == 0)
      return;

    if(iceb_menu_danet(gettext("Удалить неиспользуемые коды ? Вы уверены ?"),2,data->window) != 1)
     return;

    l_gruppdok_unk(data);
    l_uosgrnu_create_list(data);
    return;  


  case FK4:
    if(l_uosgrnu_p(&data->poisk,data->window) == 0)
     data->metka_poi=1;
    else
     data->metka_poi=0;
    l_uosgrnu_create_list(data);

    return;  

  case FK5:
    l_uosgrnu_rasp(data);
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

gboolean   l_uosgrnu_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosgrnu_data *data)
{
iceb_u_str repl;
//printf("l_uosgrnu_key_press keyval=%d state=%d\n",event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_F2:

    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");

    return(TRUE);
   
  case GDK_F3:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK3]),"clicked");
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
void l_uosgrnu_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_uosgrnu_data *data)
{
//printf("l_uosgrnu_v_row\n");
//printf("l_uosgrnu_v_row корректировка\n");
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

void l_uosgrnu_udzap(class l_uosgrnu_data *data)
{

if(l_uosgrnu_pvu(1,data->kodv.ravno(),data->window) != 0)
 return;

char strsql[512];

sprintf(strsql,"delete from Uosgrup where kod='%s'",data->kodv.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/****************************/
/*Проверка записей          */
/*****************************/

int  l_uosgrnu_prov_row(SQL_str row,class l_uosgrnu_data *data)
{
if(data->metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->poisk.kod.ravno(),row[0],0,0) != 0)
 return(1);


//Поиск образца в строке
if(data->poisk.naim.ravno()[0] != '\0' && iceb_u_strstrm(row[1],data->poisk.naim.ravno()) == 0)
 return(1);


   
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_uosgrnu_rasp(class l_uosgrnu_data *data)
{
char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;

sprintf(strsql,"select * from Uosgrup order by kod asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"ugrnu%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список групп налогового учёта"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_u_zagolov(gettext("Список групп налогового учёта"),0,0,0,0,0,0,organ,ff);

if(data->metka_poi == 1)
 {
  if(data->poisk.kod.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Код"),data->poisk.kod.ravno());
  if(data->poisk.naim.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Наименование"),data->poisk.naim.ravno());
 }


fprintf(ff,"\
----------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код | Наименование группы          |Год.н.а.|Тип амортизации|Алгоритм амортизации|\n"));

/*
1234 123456789012345678901234567890 12345678 123456789012345 12345678901234567890
*/

fprintf(ff,"\
----------------------------------------------------------------------------------\n");

char tipam[50];
char alam[50];
while(cur.read_cursor(&row) != 0)
 {
  if(atoi(row[5]) == 0)
   strcpy(tipam,gettext("Пообъектная"));
  else
   strcpy(tipam,gettext("В целом по группе"));

  if(atoi(row[6]) == 0)
   strncpy(alam,gettext("От начальной балансовой стоимости"),sizeof(alam)-1);  
  else
   strncpy(alam,gettext("От остаточной балансовой стоимости"),sizeof(alam)-1);  

  fprintf(ff,"%-4s %-*.*s %8s %-*.*s %-*.*s\n",
  row[0],
  iceb_u_kolbait(30,row[1]),iceb_u_kolbait(30,row[1]),row[1],
  row[2],
  iceb_u_kolbait(15,tipam),iceb_u_kolbait(15,tipam),tipam,
  iceb_u_kolbait(20,alam),iceb_u_kolbait(20,alam),alam);
 }
fprintf(ff,"\
----------------------------------------------------------------------------------\n");

iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,"",0,data->window);

}
/*****************************/
/*Проверить возможность удаления*/
/********************************/
//Если вернули 0-можно удалять 1-нет
int  l_uosgrnu_pvu(int met,const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select hna from Uosinp where hna='%s' limit 1",kod);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    iceb_u_str soob;
    sprintf(strsql,"%s Uosinp",gettext("Удалить невозможно ! Есть записи в таблице"));
    soob.plus(strsql);
    iceb_menu_soob(&soob,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select hna from Uosamor where hna='%s' limit 1",kod);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    iceb_u_str soob;
    sprintf(strsql,"%s Uosamor",gettext("Удалить невозможно ! Есть записи в таблице"));
    soob.plus(strsql);
    iceb_menu_soob(&soob,wpredok);
   }
  return(1);
 }    


return(0);

}
/**********************************/
/*Удаление неиспользуемых кодов групп*/
/**************************************/
void l_gruppdok_unk(class l_uosgrnu_data *data)
{
SQL_str row;
SQLCURSOR cur;

char strsql[512];
sprintf(strsql,"select kod from Uosgrup");
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
int koludzap=0;

while(cur.read_cursor(&row) != 0)
 {
  if(l_uosgrnu_pvu(0,row[0],data->window) != 0)
   continue;
  sprintf(strsql,"delete from Uosgrup where kod=%d",atoi(row[0]));
  if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
   break;
  koludzap++;
 }
sprintf(strsql,"%s:%d",gettext("Количество удалённых записей"),koludzap);
iceb_menu_soob(strsql,data->window);

}

