/*$Id: l_uplspdok.c,v 1.8 2011-02-21 07:35:54 sasa Exp $*/
/*11.03.2008	11.03.2008	Белых А.И.	l_uplspdok.c
Просмотр списка документов
*/

#include <stdlib.h>
#include <unistd.h>
#include "buhg_g.h"
#include "l_uplspdok.h"
enum
{
 FK2,
 FK4,
 FK5,
 FK10,
 KOL_F_KL
};

enum
{
 COL_METKAZ,
 COL_DATAD,
 COL_KOD_POD,
 COL_NOMDOK,
 COL_KOD_VOD,
 COL_KOD_AVT,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  uplspdok_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать 
  int       kolzap;     //Количество записей

  class uplspdok_rek poi;
  iceb_u_str zapros;

  short ds,ms,gs; //Стартовая дата просмотра
  
  short dn,mn,gn;
  short dk,mk,gk;
  
  class iceb_u_str datav;
  class iceb_u_str nomdokv;
  class iceb_u_str kod_podv;
  
  //Конструктор
  uplspdok_data()
   {
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    dn=mn=gn=dk=mk=gk=0;
   }      
 };

gboolean   uplspdok_key_press(GtkWidget *widget,GdkEventKey *event,class uplspdok_data *data);
void uplspdok_vibor(GtkTreeSelection *selection,class uplspdok_data *data);
void uplspdok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class uplspdok_data *data);
void  uplspdok_knopka(GtkWidget *widget,class uplspdok_data *data);
void uplspdok_add_columns (GtkTreeView *treeview);
void uplspdok_create_list(class uplspdok_data *data);
int  uplspdok_prov_row(SQL_str row,class uplspdok_data *data);

int l_uplspdok_p(class uplspdok_rek *datap,GtkWidget *wpredok);

extern SQL_baza	bd;
extern char *name_system;
extern char     *organ;
extern short startgodupl;
 
void l_uplspdok(short ds,short ms,short gs, //Дата начала просмотра
GtkWidget *wpredok)
{
class uplspdok_data data;
char bros[512];
GdkColor color;


data.ds=data.dn=ds;
data.ms=data.mn=ms;
data.gs=data.gn=gs;
if(data.gn == 0)
 data.gn=startgodupl;
  
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Список путевых листов"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uplspdok_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Список путевых листов"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");
/************
PangoFontDescription *font_pango=pango_font_description_from_string(shrift_ravnohir.ravno());
gtk_widget_modify_font(GTK_WIDGET(data.label_ost),font_pango);
pango_font_description_free(font_pango);
***************/

gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_poisk,GTK_STATE_NORMAL,&color);

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),-1,200);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];


sprintf(bros,"F2 ?");
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(uplspdok_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Включение/выключение показа только документов, для которых не выполнено по объектное списание топлива"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);


sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(uplspdok_knopka),&data);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Просмотр"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(uplspdok_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Просмотр выбранного документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(uplspdok_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

uplspdok_create_list(&data);
gtk_widget_show(data.window);

//if(metka == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


//printf("l_uplspdok end\n");

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uplspdok_knopka(GtkWidget *widget,class uplspdok_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    if(data->poi.metka_pros == 1)
      data->poi.metka_pros=0;
    else
      data->poi.metka_pros=1;  
     
    uplspdok_create_list(data);
    return;  




  case FK4:
    l_uplspdok_p(&data->poi,data->window);
    
    if(data->poi.metka_poi == 1)
     {
      if(data->poi.datan.getdlinna() > 1)
       iceb_rsdatp(&data->dn,&data->mn,&data->gn,data->poi.datan.ravno(),\
       &data->dk,&data->mk,&data->gk,data->poi.datak.ravno(),data->window);
     }
    else
     {
      data->dn=data->ds;
      data->mn=data->ms;
      data->gn=data->gs;
      
     }
    uplspdok_create_list(data);
    return;  

  case FK5:
    if(l_upldok(data->datav.ravno(),data->nomdokv.ravno(),data->kod_podv.ravno(),data->window) != 0)
      uplspdok_create_list(data);
    return;  

    
  case FK10:
//    printf("uplspdok_knopka F10\n");
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uplspdok_key_press(GtkWidget *widget,GdkEventKey *event,class uplspdok_data *data)
{
iceb_u_str repl;
//printf("uplspdok_key_press keyval=%d state=%d\n",event->keyval,event->state);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch(event->keyval)
 {
  case GDK_F2:
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
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
    printf("uplspdok_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void uplspdok_create_list (class uplspdok_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
char bros[500];
int  kolstr=0;
SQL_str row;
iceb_u_str zagolov;

//printf("uplspdok_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(uplspdok_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(uplspdok_vibor),data);

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

sprintf(bros,"select datd,kp,nomd,ka,kv,mt,ktoz,vrem from Upldok");

sprintf(strsql,"%s where datd >= '%04d-01-01' order by datd asc",bros,data->gn);

if(data->dn != 0)
  sprintf(strsql,"%s where datd >= '%04d-%02d-%02d' order by datd asc",bros,data->gn,data->mn,data->dn);

if(data->dn != 0 && data->dk != 0)
  sprintf(strsql,"%s where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' \
order by datd asc",bros,data->gn,data->mn,data->dn,data->gk,data->mk,data->dk);

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
iceb_refresh();
class iceb_u_str naim("");
SQL_str row1;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
   if(uplspdok_prov_row(row,data) != 0)
    continue;

  //Метка записи

  ss[COL_METKAZ].new_plus("");

  if(row[5][0] == '0')
   ss[COL_METKAZ].plus("?");

  //Дата документа
  ss[COL_DATAD].new_plus(iceb_u_sqldata(row[0]));
  
  //Код подразделения
  naim.new_plus("");
  sprintf(strsql,"select naik from Uplpodr where kod=%d",atoi(row[1]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim.new_plus(row1[0]);  
  sprintf(strsql,"%s %s",row[1],naim.ravno_toutf());
  ss[COL_KOD_POD].new_plus(strsql);

  //Номер документа
  ss[COL_NOMDOK].new_plus(iceb_u_toutf(row[2]));

  //код водителя
  naim.new_plus("");
  sprintf(strsql,"select naik from Uplouot where kod=%d",atoi(row[4]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim.new_plus(row1[0]);  
  sprintf(strsql,"%s %s",row[4],naim.ravno_toutf());
  ss[COL_KOD_VOD].new_plus(strsql);

  //Код автомобиля
  naim.new_plus("");
  sprintf(strsql,"select naik from Uplavt where kod=%d",atoi(row[3]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim.new_plus(row1[0]);  
  sprintf(strsql,"%s %s",row[3],naim.ravno_toutf());
  ss[COL_KOD_AVT].new_plus(strsql);

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[7])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[6],0,data->window));


  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_METKAZ,ss[COL_METKAZ].ravno(),
  COL_DATAD,ss[COL_DATAD].ravno(),
  COL_NOMDOK,ss[COL_NOMDOK].ravno(),
  COL_KOD_VOD,ss[COL_KOD_VOD].ravno(),
  COL_KOD_AVT,ss[COL_KOD_AVT].ravno(),
  COL_KOD_POD,ss[COL_KOD_POD].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

uplspdok_add_columns (GTK_TREE_VIEW (data->treeview));


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

if(data->kolzap > 0)
 {
  if(data->snanomer  > data->kolzap-1)
     data->snanomer= data->kolzap-1;
  if(data->snanomer  < 0)
     data->snanomer=0;
      
  printf("poz=%d\n",data->snanomer);

  sprintf(strsql,"%d",data->snanomer);
  GtkTreePath *path=gtk_tree_path_new_from_string(strsql);

//  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,TRUE,0,0);
  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE,0,0);

  gtk_tree_view_set_cursor(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE);
  gtk_tree_path_free(path);
//  data->snanomer=-2;
 }

iceb_u_str strpoi;  
strpoi.plus("");

if(data->poi.metka_pros == 1)
 strpoi.new_plus(gettext("Просмотр только неподтверждённых документов"));
if(data->poi.metka_pros == 2)
 strpoi.new_plus(gettext("Просмотр только документов без проводок"));
 
if(data->poi.metka_poi == 1 )
 {

// printf("Формирование заголовка с реквизитами поиска.\n");
  if(data->poi.metka_pros != 0)      
   strpoi.ps_plus(gettext("Поиск"));
  else
   strpoi.new_plus(gettext("Поиск"));
  strpoi.plus(" !!!");

  iceb_str_poisk(&strpoi,data->poi.nomdok.ravno(),gettext("Номер документа"));
  iceb_str_poisk(&strpoi,data->poi.datan.ravno(),gettext("Дата начала"));
  iceb_str_poisk(&strpoi,data->poi.datak.ravno(),gettext("Дата конца"));
  iceb_str_poisk(&strpoi,data->poi.kod_pod.ravno(),gettext("Код подразделения"));
  iceb_str_poisk(&strpoi,data->poi.kod_vod.ravno(),gettext("Код водителя"));
  iceb_str_poisk(&strpoi,data->poi.kod_avt.ravno(),gettext("Код автомобиля"));
    
 }

if(strpoi.getdlinna() > 1)
 {
  gtk_label_set_text(GTK_LABEL(data->label_poisk),strpoi.ravno_toutf());
  gtk_widget_show(data->label_poisk);
 }
else
  gtk_widget_hide(data->label_poisk); 

zagolov.new_plus(gettext("Список путевых листов"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));

}
/*****************/
/*Создаем колонки*/
/*****************/

void uplspdok_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

//printf("uplspdok_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Метка"), renderer,"text", COL_METKAZ,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата док-та"), renderer,"text",COL_DATAD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Номер документа"), renderer,"text", COL_NOMDOK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Подразделение"), renderer,"text", COL_KOD_POD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Водитель"), renderer,"text", COL_KOD_VOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Автомобиль"), renderer,"text", COL_KOD_AVT,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

//printf("uplspdok_add_columns end\n");

}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void uplspdok_vibor(GtkTreeSelection *selection,class uplspdok_data *data)
{
//printf("uplspdok_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *datad;
gchar *nomdok;
gchar *kod_pod;

gint  nomer;


gtk_tree_model_get(model,&iter,
COL_DATAD,&datad,
COL_NOMDOK,&nomdok,
COL_KOD_POD,&kod_pod,
NUM_COLUMNS,&nomer,-1);

data->datav.new_plus(datad);
data->nomdokv.new_plus(iceb_u_fromutf(nomdok));
if(iceb_u_polen(kod_pod,&data->kod_podv,1,' ') != 0)
  data->kod_podv.new_plus(iceb_u_fromutf(kod_pod)); 
data->snanomer=nomer;

g_free(datad);
g_free(nomdok);
g_free(kod_pod);

//printf("uplspdok_vibor-%s %s %d\n",data->datav.ravno(),data->nomdokv.ravno(),data->snanomer);

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void uplspdok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class uplspdok_data *data)
{
//просмотр выбранного документа
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");


}
/****************************/
/*Проверка записей          */
/*****************************/

int  uplspdok_prov_row(SQL_str row,class uplspdok_data *data)
{

  
int pod=atoi(row[5]);
if(data->poi.metka_pros == 1 && pod == 1)
   return(1);


if(data->poi.metka_poi == 0)
 return(0);
 
//sprintf(bros,"select datd,kp,nomd,ka,kv,mt,ktoz,vrem from Upldok");
if(iceb_u_proverka(data->poi.kod_pod.ravno(),row[1],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->poi.kod_vod.ravno(),row[4],0,0) != 0)
 return(1);
if(iceb_u_proverka(data->poi.kod_avt.ravno(),row[3],0,0) != 0)
 return(1);



if(data->poi.nomdok.getdlinna() > 1)
 if(iceb_u_SRAV(data->poi.nomdok.ravno(),row[2],1) != 0)
   return(1);


return(0);
}
