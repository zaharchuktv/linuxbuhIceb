/*$Id: iceb_l_plansh.c,v 1.55 2011-02-21 07:36:07 sasa Exp $*/
/*12.06.2010	24.11.2003	Белых А.И.	iceb_l_plansh.c
Работа с планом счетов
Если вернули 0- выбрали контрагента
             1- нет
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
#include        "iceb_libbuh.h"
#include        "iceb_l_plansh.h"

enum
{
  FK2,
  SFK2,
  FK3,
  FK4,
  FK5,
  FK6,
  FK10,
  KOL_F_KL
};

enum
{
 COL_SHET,
 COL_NAIM,
 COL_VID,
 COL_SALDO,
 COL_STATUS,
 COL_TIP,
 COL_KSB,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};



class plansh_data
 {
  public:
  

  plansh_rek poisk;
  short metka_poi;  //0-не проверять 1-проверять
    
  //Выбранный счет
  class iceb_u_str shetv;  //выбранный счёт
  class iceb_u_str naimv;
  class iceb_u_str shet_tv; //Только что введённый счёт
           
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  short     metka_rr;   //0-ввод и корректировка 1-чистый выбор
  int       metka_voz;  //0-выбрано 1-не выбрано
    
  //Конструктор
  plansh_data()
   {
    metka_voz=1;
    metka_rr=0;
    metka_poi=0; //0-не проверять 1-проверять
    snanomer=0;
    kolzap=kl_shift=0;
    window=treeview=NULL;
   }  

 };

void plansh_create_list (class plansh_data *data);

int iceb_l_plansh_v(class iceb_u_str *shetk,GtkWidget *wpredok);
int iceb_l_plansh_p(class plansh_rek *rek_poi,GtkWidget *wpredok);
void       plansh_add_columns (GtkTreeView *treeview);
gboolean   plansh_key_press(GtkWidget *,GdkEventKey *,class plansh_data *);
void       plansh_knopka(GtkWidget *,class plansh_data *);
void       plansh_v_row(GtkTreeView *,GtkTreePath *,GtkTreeViewColumn *,class plansh_data *);
void       plansh_udzap(class plansh_data *data);
int        plansh_provvudzap(const char *plansh,int met,GtkWidget*);
void       plansh_vibor(GtkTreeSelection *,class plansh_data *);
void       plansh_udnz();
gint       plansh_udnz11(class plansh_udnz_data *data);
void       plansh_snkod(class plansh_data *data);
int        plansh_prov_row(SQL_str row,class plansh_data *data);
void       plansh_sm_kod(char *kodkon);
int 	udplsh(class plansh_data *);
int provudsh(const char *shet,int met,GtkWidget*);
void        rasplsh(class plansh_data *data);
int  plansh_vksb(class plansh_data *data);
int iceb_l_plansh_vksb(class iceb_u_str *kod_subbal,char *nadpis,GtkWidget *wpredok);

extern char *organ;

int         iceb_l_plansh(int metka_rr,//0-ввод и корректировка 1-чистый выбор
iceb_u_str *kod,
iceb_u_str *naim,
GtkWidget  *wpredok)
{
plansh_data data;
char bros[512];
GdkColor color;

data.metka_rr=metka_rr;
if(naim->getdlinna() > 1)
 {
  data.metka_poi=1;
  data.poisk.naim.plus(naim->ravno());
 }

//printf("l_plansh-data.metka_rr=%d data.metka_poi=%d\n",data.metka_rr,data.metka_poi);
 
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("План счетов"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(plansh_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(vbox1);
gtk_widget_show(vbox2);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("План счетов"));
//gdk_color_parse("green",&color);
//gtk_widget_modify_fg(data.label_kolstr,GTK_STATE_NORMAL,&color);


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

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

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(plansh_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[SFK2]),FALSE);//Недоступна
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(plansh_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной запси"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[FK3]),FALSE);//Недоступна
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(plansh_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной запси"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(plansh_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(plansh_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Код субб."));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(plansh_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Ввести для счёта (субсчёта) код суббаланса"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(plansh_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

plansh_create_list (&data);

gtk_widget_show(data.window);
if(metka_rr == 0)
 gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));
//gtk_widget_hide (data.label_poisk);

gtk_main();

if(data.metka_voz == 0)
 {
  kod->new_plus(data.shetv.ravno());
  naim->new_plus(data.naimv.ravno());
 }

printf("l_plansh end\n");
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.metka_voz);

}

/***********************************/
/*Создаем список для просмотра */
/***********************************/
void plansh_create_list (class plansh_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;
//GdkColor color;

//printf("plansh_create_list %d\n",data->snanomer);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->kl_shift=0;

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(plansh_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(plansh_vibor),data);

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
G_TYPE_INT);


sprintf(strsql,"select * from Plansh order by ns asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str s1;
iceb_u_str s2;
iceb_u_str s3;
iceb_u_str s4;
iceb_u_str s5;
iceb_u_str s6;
iceb_u_str s7;
iceb_u_str s8;
iceb_u_str s9;

data->kolzap=0;
int metka=0;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  if(plansh_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->shet_tv.ravno(),row[0],0) == 0)
    data->snanomer=data->kolzap;

  //Счет
  s1.new_plus(iceb_u_toutf(row[0]));
  
  //Наименование
  s2.new_plus(iceb_u_toutf(row[1]));

  //Вид счета счет/субсчет
  metka=atoi(row[3]);

  if(metka == 0)
   s3.new_plus(gettext("Счет"));
  if(metka == 1)
   s3.new_plus(gettext("Субрахунок"));

  //Сальдо свернутоЁ/развернутоЁ
  metka=atoi(row[6]);
  if(metka == 0)
   s4.new_plus(" ");
//   s4.new_plus(gettext("Свернутое"));
  if(metka == 3)
   s4.new_plus(gettext("Развернутое"));

  //Статус  балансовый/внебалансовый
  metka=atoi(row[7]);
  if(metka == 0)
    s5.new_plus(gettext("Балансовый"));
  if(metka == 1)
    s5.new_plus(gettext("Внебалансовый"));

  //Тип счета активный/пассивный/активно-пассивный
  metka=atoi(row[2]);
  if(metka == 0)
    s6.new_plus(gettext("Активный"));
  if(metka == 1)
    s6.new_plus(gettext("Пассивный"));
  if(metka == 2)
    s6.new_plus(gettext("Активно-пассивный"));

  s9.new_plus("");
  if(atoi(row[9]) != 0)
   s9.new_plus(row[9]);
   
  //Дата и время записи
  s7.new_plus(iceb_u_toutf(iceb_u_vremzap(row[5])));

  //Кто записал
  s8.new_plus(iceb_kszap(row[4],0,data->window));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_SHET,s1.ravno(),
  COL_NAIM,s2.ravno(),
  COL_VID,s3.ravno(),
  COL_SALDO,s4.ravno(),
  COL_STATUS,s5.ravno(),
  COL_TIP,s6.ravno(),
  COL_KSB,s9.ravno(),
  COL_DATA_VREM,s7.ravno(),
  COL_KTO,s8.ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->shet_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

plansh_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
 }

//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data->treeview,GTK_STATE_NORMAL,&color);

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("План счетов"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);
gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

//printf("data->metka_poi=%d\n",data->metka_poi);
if(data->metka_poi == 1)
 {
//  printf("Формирование заголвка с реквизитами поиска.\n");
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");
  iceb_str_poisk(&zagolov,data->poisk.shet.ravno(),gettext("Счёт"));
  iceb_str_poisk(&zagolov,data->poisk.naim.ravno(),gettext("Наименование счёта"));
  iceb_str_poisk(&zagolov,data->poisk.kod_subbal.ravno(),gettext("Код суббаланса"));
  
  gtk_label_set_text(GTK_LABEL(data->label_poisk),zagolov.ravno_toutf());
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));


}

/*****************/
/*Создаем колонки*/
/*****************/

void plansh_add_columns (GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Счет"), renderer,"text", COL_SHET,NULL);

renderer = gtk_cell_renderer_text_new ();
//g_object_set(G_OBJECT(renderer),"foreground","white",NULL);
//g_object_set(G_OBJECT(renderer),"background","black",NULL);


gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);
renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Вид счета"), renderer,
"text", COL_VID,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Сальдо"), renderer,
"text", COL_SALDO,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Статус"), renderer,
"text", COL_STATUS,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Тип счета"), renderer,
"text", COL_TIP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("К/с"), renderer,"text", COL_KSB,NULL);

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

void plansh_vibor(GtkTreeSelection *selection,class plansh_data *data)
{
//printf("plansh_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gchar *naim;
gint  nomer;
gtk_tree_model_get(model,&iter,0,&kod,1,&naim,NUM_COLUMNS,&nomer,-1);

data->shetv.new_plus(iceb_u_fromutf(kod));
data->naimv.new_plus(iceb_u_fromutf(naim));
data->snanomer=nomer;

g_free(kod);
g_free(naim);

//printf("%s %s %d\n",data->shetv.ravno(),data->naimv.ravno(),data->nomervsp);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  plansh_knopka(GtkWidget *widget,class plansh_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    if(iceb_l_plansh_v(&data->shet_tv,data->window) == 0)
      plansh_create_list (data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
     return;
    if(iceb_l_plansh_v(&data->shetv,data->window) == 0)
      plansh_create_list (data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(udplsh(data) == 0)
      plansh_create_list(data);
    return;  


  case FK4:
    if(iceb_l_plansh_p(&data->poisk,data->window) == 0)
     data->metka_poi=1;
    else
     data->metka_poi=0;
     
    plansh_create_list (data);
    return;  

  case FK5:
//    gtk_widget_hide(data->window);
    if(data->kolzap == 0)
      return;
    rasplsh(data);
//    gtk_widget_show(data->window);
    return;  

  case FK6:
    if(plansh_vksb(data) == 0)
     plansh_create_list(data);
    return;  
    
  case FK10:
//    printf("plansh_knopka F10\n");
    gtk_widget_destroy(data->window);
    data->metka_voz=1;
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   plansh_key_press(GtkWidget *widget,GdkEventKey *event,class plansh_data *data)
{
//printf("plansh_key_press keyval=%d state=%d\n",event->keyval,event->state);
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

  case GDK_F6:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
    return(TRUE);

  
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("plansh_key_press-Нажата клавиша Shift\n");

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
void plansh_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class plansh_data *data)
{
//printf("plansh_v_row data->metka_rr=%d\n",data->metka_rr);
if(data->metka_rr == 0)
 {
  gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
  return;
 }

//printf("plansh_v_row выход\n");

gtk_widget_destroy(data->window);

data->metka_voz=0;

}
/***********************/
/*Удаление записи      */
/***********************/
void   plansh_udzap(class plansh_data *data)
{
char  strsql[512];

 
iceb_u_str sp;
sp.plus(gettext("Удалить запись ? Вы уверены ?"));
if(iceb_menu_danet(&sp,2,data->window) == 2)
 return;

if(plansh_provvudzap(data->shetv.ravno(),1,data->window) != 0)
  return;

sprintf(strsql,"delete from Plansh where ns='%s'",data->shetv.ravno());
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;

plansh_create_list(data);

}

/********************************/
/*Проверка возможности удаления записи*/
/***************************************/

int plansh_provvudzap(const char *kod,int met,GtkWidget *wpredok) //0-молча 1-сообщения
{
char strsql[500];
int kolstr=0;
SQLCURSOR cur;


memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"select distinct ns from Saldo where ns='%s'",kod);
 //  printw("%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
 
if(kolstr != 0)
 {
  if(met != 0)
   {
    iceb_u_str soob;
    sprintf(strsql,"%s Saldo",gettext("Удалить невозможно ! Есть записи в таблице"));
    soob.plus(strsql);
    iceb_menu_soob(&soob,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre from Prov where sh='%s' or \
shk='%s'",kod,kod);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
 
if(kolstr != 0)
 {
  if(met != 0)
   {
    iceb_u_str soob;
    sprintf(strsql,"%s Prov",gettext("Удалить невозможно ! Есть записи в таблице"));
    soob.plus(strsql);
    iceb_menu_soob(&soob,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select distinct shetu from Kart where shetu='%s'",kod);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
 
 
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

 
sprintf(strsql,"select distinct shet from Nash where shet='%s'",kod);
 //   printw("%s\n",strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  {
   if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    {
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    }
  }
else
 {   
   
  if(kolstr != 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Nash",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }
 }


 sprintf(strsql,"select distinct shet from Uder where shet='%s'",kod);

 if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  {
   if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    {
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    }
  }
 else
  {   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       iceb_u_str soob;
       sprintf(strsql,"%s Uder",gettext("Удалить невозможно ! Есть записи в таблице"));
       soob.plus(strsql);
       iceb_menu_soob(&soob,wpredok);
      }
     return(1);
    }
  }

 sprintf(strsql,"select distinct shetu from Uosinp where shetu='%s'",kod);

 if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  {
   if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    {
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    }
  }
 else
  {   
   
   if(kolstr != 0)
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
  }

 sprintf(strsql,"select distinct shetu from Uslugi where shetu='%s'",kod);

 if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  {
   if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    {
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    }
  }
 else
  {   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       iceb_u_str soob;
       sprintf(strsql,"%s Uslugi",gettext("Удалить невозможно ! Есть записи в таблице"));
       soob.plus(strsql);
       iceb_menu_soob(&soob,wpredok);
      }
     return(1);
    }
  }

 sprintf(strsql,"select distinct shetu from Usldokum1 where shetu='%s'",kod);

 if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  {
   if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    {
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    }
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

 sprintf(strsql,"select distinct shetu from Usldokum2 where shetu='%s'",kod);

 if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  {
   if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    {
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    }
  }
 else
  {   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       iceb_u_str soob;
       sprintf(strsql,"%s Usldokum2",gettext("Удалить невозможно ! Есть записи в таблице"));
       soob.plus(strsql);
       iceb_menu_soob(&soob,wpredok);
      }
     return(1);
    }
  }

 sprintf(strsql,"select distinct shet from Zarsoc where shet='%s'",kod);

 if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  {
   if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    {
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    }
  }
 else
  {   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       iceb_u_str soob;
       sprintf(strsql,"%s Zarsoc",gettext("Удалить невозможно ! Есть записи в таблице"));
       soob.plus(strsql);
       iceb_menu_soob(&soob,wpredok);
      }
     return(1);
    }
  }

 sprintf(strsql,"select distinct shet from Zarsocz where shet='%s'",kod);

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
       sprintf(strsql,"%s Zarsocz",gettext("Удалить невозможно ! Есть записи в таблице"));
       soob.plus(strsql);
       iceb_menu_soob(&soob,wpredok);
      }
     return(1);
    }
  }


 sprintf(strsql,"select distinct shet from Kas where shet='%s'",kod);

 if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  {
   if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    {
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    }
  }
 else
  {   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       iceb_u_str soob;
       sprintf(strsql,"%s Kas",gettext("Удалить невозможно ! Есть записи в таблице"));
       soob.plus(strsql);
       iceb_menu_soob(&soob,wpredok);
      }
     return(1);
    }
  }


 sprintf(strsql,"select distinct shetkas from Kasop1 where shetkas='%s'",kod);

 if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  {
   if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    {
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    }
  }
 else
  {   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       iceb_u_str soob;
       sprintf(strsql,"%s Kasop1",gettext("Удалить невозможно ! Есть записи в таблице"));
       soob.plus(strsql);
       iceb_menu_soob(&soob,wpredok);
      }
     return(1);
    }
  }

 sprintf(strsql,"select distinct shetkor from Kasop1 where shetkor='%s'",kod);

 if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  {
   if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    {
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    }
  }
 else
  {   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       iceb_u_str soob;
       sprintf(strsql,"%s Kasop1",gettext("Удалить невозможно ! Есть записи в таблице"));
       soob.plus(strsql);
       iceb_menu_soob(&soob,wpredok);
      }
     return(1);
    }
  }

 sprintf(strsql,"select distinct shetkas from Kasop2 where shetkas='%s'",kod);

 if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  {
   if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    {
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    }
  }
 else
  {   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       iceb_u_str soob;
       sprintf(strsql,"%s Kasop2",gettext("Удалить невозможно ! Есть записи в таблице"));
       soob.plus(strsql);
       iceb_menu_soob(&soob,wpredok);
      }
     return(1);
    }
  }

 sprintf(strsql,"select distinct shetkor from Kasop2 where shetkor='%s'",kod);

 if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  {
   if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    {
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    }
  }
 else
  {   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       iceb_u_str soob;
       sprintf(strsql,"%s Kasop2",gettext("Удалить невозможно ! Есть записи в таблице"));
       soob.plus(strsql);
       iceb_menu_soob(&soob,wpredok);
      }
     return(1);
    }
  }


 sprintf(strsql,"select distinct shetk from Kasord where shetk='%s'",kod);

 if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  {
   if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    {
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    }
  }
 else
  {   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       iceb_u_str soob;
       sprintf(strsql,"%s Kasord",gettext("Удалить невозможно ! Есть записи в таблице"));
       soob.plus(strsql);
       iceb_menu_soob(&soob,wpredok);
      }
     return(1);
    }
  }


 sprintf(strsql,"select distinct shet from Ukrkras where shet='%s'",kod);

 if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  {
   if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    {
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    }
  }
 else
  {   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       iceb_u_str soob;
       sprintf(strsql,"%s Ukrkras",gettext("Удалить невозможно ! Есть записи в таблице"));
       soob.plus(strsql);
       iceb_menu_soob(&soob,wpredok);
      }
     return(1);
    }
  }


 sprintf(strsql,"select distinct shet from Ukrdok1 where shet='%s'",kod);

 if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  {
   if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    {
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    }
  }
 else
  {   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       iceb_u_str soob;
       sprintf(strsql,"%s Ukrdok1",gettext("Удалить невозможно ! Есть записи в таблице"));
       soob.plus(strsql);
       iceb_menu_soob(&soob,wpredok);
      }
     return(1);
    }
  }

 sprintf(strsql,"select distinct shet from Uplost where shet='%s'",kod);

 if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  {
   if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    {
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    }
  }
 else
  {   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       iceb_u_str soob;
       sprintf(strsql,"%s Uplost",gettext("Удалить невозможно ! Есть записи в таблице"));
       soob.plus(strsql);
       iceb_menu_soob(&soob,wpredok);
      }
     return(1);
    }
  }


 sprintf(strsql,"select distinct shet from Upldok2 where shet='%s'",kod);

 if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  {
   if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    {
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    }
  }
 else
  {   
   
   if(kolstr != 0)
    {
     if(met != 0)
      {
       iceb_u_str soob;
       sprintf(strsql,"%s Upldok2",gettext("Удалить невозможно ! Есть записи в таблице"));
       soob.plus(strsql);
       iceb_menu_soob(&soob,wpredok);
      }
     return(1);
    }
  }


return(0);
}


/*************************************/
/*Проверка записей на реквизиты поиска*/
/**************************************/

int  plansh_prov_row(SQL_str row,class plansh_data *data)
{
if(data->metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->poisk.shet.ravno(),row[0],0,0) != 0)
 return(1);

//Поиск образца в строке
if(data->poisk.naim.ravno()[0] != '\0' && iceb_u_strstrm(row[1],data->poisk.naim.ravno()) == 0)
 return(1);

if(iceb_u_proverka(data->poisk.kod_subbal.ravno(),row[9],0,0) != 0)
 return(1);
 
return(0);
}
/*******************************/
/*Удаление записи              */
/*******************************/

int udplsh(class plansh_data *data)
{

iceb_u_str SP;
SP.plus(gettext("Удалить запись ? Вы уверены ?"));

if(iceb_menu_danet(&SP,2,data->window) == 2)
 return(1);

class iceb_clock kk(data->window);


if(provudsh(data->shetv.ravno(),1,data->window) != 0)
  return(1);


char strsql[312];
sprintf(strsql,"delete from Plansh where ns='%s'",data->shetv.ravno());
iceb_sql_zapis(strsql,1,0,data->window);

return(0);

}
/*************************************/
/*Проверка возможности удаления счета*/
/*************************************/
int provudsh(const char *shet,int met,GtkWidget *wpredok)
{
char   strsql[512];

//printf("provudsh-%s\n",shet);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"select ns from Saldo where ns='%s' limit 1",shet);
//printf("%s\n",strsql);
if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    iceb_u_str soob;
    sprintf(strsql,"%s Saldo",gettext("Удалить невозможно ! Есть записи в таблице"));
    soob.plus(strsql);
    iceb_menu_soob(&soob,wpredok);
   }
  return(1);
 }

sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre from Prov where sh='%s' or \
shk='%s' limit 1",shet,shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    iceb_u_str soob;
    sprintf(strsql,"%s Prov",gettext("Удалить невозможно ! Есть записи в таблице"));
    soob.plus(strsql);
    iceb_menu_soob(&soob,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select shetu from Kart where shetu='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
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

   
sprintf(strsql,"select shet from Nash where shet='%s' limit 1",shet);
//   printw("%s\n",strsql);
if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Nash",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }


sprintf(strsql,"select shet from Uder where shet='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Uder",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shetu from Uosinp where shetu='%s' limit 1",shet);

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

sprintf(strsql,"select shetu from Uslugi where shetu='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Uslugi",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shetu from Usldokum1 where shetu='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
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
 
sprintf(strsql,"select shetu from Usldokum2 where shetu='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Usldokum2",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shet from Zarsoc where shet='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Zarsoc",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shet from Zarsocz where shet='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Zarsocz",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }


sprintf(strsql,"select shet from Kas where shet='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Kas",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shetkas from Kasop1 where shetkas='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Kasop1",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shetkor from Kasop1 where shetkor='%s' limit 1",shet);
if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Kasop1",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shetkas from Kasop2 where shetkas='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Kasop2",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shetkor from Kasop2 where shetkor='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Kasop2",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }


sprintf(strsql,"select shetk from Kasord where shetk='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Kasord",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }


sprintf(strsql,"select shet from Ukrkras where shet='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Ukrkras",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }


sprintf(strsql,"select shet from Ukrdok1 where shet='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Ukrdok1",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shet from Uplost where shet='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Uplost",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }


sprintf(strsql,"select shet from Upldok2 where shet='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Upldok2",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }

sprintf(strsql,"select shet from Opldok where shet='%s' limit 1",shet);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Opldok",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }

return(0);
 
}
/***********************************/
/*Распечатка плана счетов          */
/***********************************/

void rasplsh(class plansh_data *data)
{
char  strsql[512];
int   kolstr=0;
SQL_str  row;
SQLCURSOR cur;
FILE   *ff;
char    imaf[32];
char    bros[512];


//printf("rasplsh\n");
sprintf(strsql,"select * from Plansh order by ns asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
sprintf(imaf,"ps%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_u_str SP;
  sprintf(strsql,"%s: %d !",gettext("Ошибка открытия файла"),errno);
  SP.plus_ps(strsql);
  SP.plus(imaf);
  iceb_menu_soob(&SP,data->window);
  return;
 }

iceb_u_zagolov(gettext("План счетов"),0,0,0,0,0,0,organ,ff);

//class iceb_u_str imaf_n;
short	vplsh=0; /*0-двух уровневый план счетов 1-многоуровневый*/

//iceb_imafn("nastrb.alx",&imaf_n);
if(iceb_poldan("Многопорядковый план счетов",strsql,"nastrb.alx",data->window) == 0)
 if(iceb_u_SRAV("Вкл",strsql,1) == 0)
  vplsh=1;

if(vplsh == 0)
 fprintf(ff,"%s.\n",gettext("Двухпорядковый"));
if(vplsh == 1)
 fprintf(ff,"%s.\n",gettext("Многопорядковый"));

if(data->poisk.shet.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Счёт"),data->poisk.shet.ravno());
if(data->poisk.naim.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Наименование счёта"),data->poisk.naim.ravno());
if(data->poisk.kod_subbal.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код суббаланса"),data->poisk.kod_subbal.ravno());
 

  fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n");  
  fprintf(ff,gettext("\
Номер |   Тип    | Сальдо   |     Вид        |    Статус     |К/с| Наименование\n"));
  fprintf(ff,gettext("\
счета |  счета   |          |    счета       |     счета     |   |   счета\n"));

  fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n");  

short tipsh,vidsh;
char tip[112];
char msald[112];
char vid[112];
char ksb[112];

while(cur.read_cursor(&row) != 0)
 {
  if(plansh_prov_row(row,data) != 0)
    continue;

  tipsh=atoi(row[2]);
  vidsh=atoi(row[3]);

  memset(tip,'\0',sizeof(tip));
  if(tipsh == 0)
    strcpy(tip,gettext("Активный"));
  if(tipsh == 1)
    strcpy(tip,gettext("Пасивный"));
  if(tipsh == 2)
    strcpy(tip,gettext("Активно-пассивный"));

  memset(msald,'\0',sizeof(msald));
  if(row[6][0] == '3')
    strcpy(msald,gettext("Развернуто"));

  memset(vid,'\0',sizeof(vid));
  if(vidsh == 0)
    strcpy(vid,gettext("Счет"));
  if(vidsh == 1)
    strcpy(vid,gettext("Субсчет"));
  if(row[7][0] == '0')
    strcpy(bros,gettext("Балансовый"));
  if(row[7][0] == '1')
    strcpy(bros,gettext("Внебалансовый"));
  memset(ksb,'\0',sizeof(ksb));
  if(atoi(row[9]) != 0)
   strcpy(ksb,row[9]);

  fprintf(ff,"%-*s %-*s %-*.*s %-*.*s %-*s %-*s %s\n",
  iceb_u_kolbait(6,row[0]),row[0],
  iceb_u_kolbait(10,vid),vid,
  iceb_u_kolbait(10,msald),iceb_u_kolbait(10,msald),msald,
  iceb_u_kolbait(16,tip),iceb_u_kolbait(16,tip),tip,
  iceb_u_kolbait(15,bros),bros,
  iceb_u_kolbait(3,ksb),ksb,
  row[1]);

 }
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n");  
fprintf(ff,"%s: %d\n",gettext("Количество счетов"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);

iceb_u_spisok fil(0);
iceb_u_spisok nazv(0);

fil.plus(imaf);
nazv.plus(gettext("План счетов"));

iceb_ustpeh(imaf,3,data->window);

iceb_rabfil(&fil,&nazv,"",0,data->window);

}
/***************************/
/*Ввод и корректировка кода суббаланса*/
/**************************************/
int  plansh_vksb(class plansh_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select nais,ksb from Plansh where ns='%s'",data->shetv.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  iceb_menu_soob(gettext("Не найдено счёт в плане счетов !"),data->window);
  return(1);
 }
sprintf(strsql,"%s\n%s %s",gettext("Введите код суббаланса"),data->shetv.ravno(),row[0]);
class iceb_u_str kodsubbal("");
if(atoi(row[1]) != 0)
  kodsubbal.new_plus(row[1]);


if(iceb_l_plansh_vksb(&kodsubbal,strsql,data->window) != 0)
 return(1);  
//if(iceb_menu_vvod1(strsql,&kodsubbal,10,data->window) != 0)
// return(1);

if(kodsubbal.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Glksubbal where kod=%d",kodsubbal.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код суббаланса"),kodsubbal.ravno_atoi());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }

sprintf(strsql,"update Plansh set ksb=%d,vrem=%ld,ktoi=%d where ns='%s'",
kodsubbal.ravno_atoi(),
time(NULL),
iceb_getuid(data->window),
data->shetv.ravno());

iceb_sql_zapis(strsql,0,0,data->window);

return(0);
}

