/*$Id: l_zar_dok.c,v 1.19 2011-02-21 07:35:54 sasa Exp $*/
/*12.11.2009	24.10.2006	Белых А.И.	l_zar_dok.c
Работа со списком документов
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
#include  "buhg_g.h"
#include  "l_zar_dok.h"

enum
{
  COL_METKA_ZAP,
  COL_DATAD,
  COL_NOMD,
  COL_SUMA,
  COL_PODR,
  COL_KOMENT,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

enum
{
  FK2,
//  FK3,
  FK4,
  FK5,
  FK10,
  SFK2,
  KOL_F_KL
};

class  l_zar_dok_data
 {
  public:

  l_zar_dok_rek poisk;

  class iceb_u_str datad;
  class iceb_u_str nomdok;
  short prn;
    
  class iceb_u_str datad_tv;
  class iceb_u_str nomdok_tv;

  short dn,mn,gn;
  short dk,mk,gk;
  short dz,mz,gz;
  class iceb_u_str zapros;
    
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  
  //Конструктор
  l_zar_dok_data()
   {
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
    datad_tv.new_plus("");
    nomdok_tv.new_plus("");
    dk=mk=gk=dn=mn=gn=0;
   }      
 };

gboolean   l_zar_dok_key_press(GtkWidget *widget,GdkEventKey *event,class l_zar_dok_data *data);
void l_zar_dok_vibor(GtkTreeSelection *selection,class l_zar_dok_data *data);
void l_zar_dok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_zar_dok_data *data);
void  l_zar_dok_knopka(GtkWidget *widget,class l_zar_dok_data *data);
void l_zar_dok_add_columns (GtkTreeView *treeview);
int  l_zar_dok_prov_row(SQL_str row,class l_zar_dok_data *data);
void l_zar_dok_rasp(class l_zar_dok_data *data);
void l_zar_dok_create_list (class l_zar_dok_data *data);

int l_zar_dok_p(class l_zar_dok_rek *rek_poi,GtkWidget *wpredok);


extern char *organ;
extern SQL_baza  bd;
extern char      *name_system;
extern short	startgodz; /*Стаpтовый год для расчета сальдо*/

int l_zar_dok(GtkWidget *wpredok)
{
l_zar_dok_data data;
char bros[512];
GdkColor color;

data.poisk.clear_data();

data.dz=data.dn=1;
data.mz=data.mn=1;
data.gn=startgodz;
if(data.gn == 0)
 {
  short d,m;
  iceb_u_poltekdat(&d,&m,&data.gn);
 }
data.gz=data.gn;

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Список документов"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_zar_dok_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Список документов"));
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

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(l_zar_dok_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Просмотр"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(l_zar_dok_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Просмотр выбранного документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);

/**********
sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(l_zar_dok_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);
**************/

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(l_zar_dok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(l_zar_dok_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(l_zar_dok_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_zar_dok_create_list(&data);

gtk_widget_show(data.window);

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
void l_zar_dok_create_list (class l_zar_dok_data *data)
{
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur,cur1;
char strsql[512];
int  kolstr=0;
SQL_str row,row1;
//GdkColor color;

data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_zar_dok_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_zar_dok_vibor),data);

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


if(data->dk != 0)
  sprintf(strsql,"select * from Zardok where datd >='%04d-%02d-%02d' and \
datd <= '%04d-%02d-%02d' order by datd,nomd asc",data->gn,data->mn,data->dn,data->gk,data->mk,data->dk);
else
  sprintf(strsql,"select * from Zardok where datd >='%04d-%02d-%02d' order by datd,nomd asc",data->gn,data->mn,data->dn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
data->zapros.new_plus(strsql);
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
double suma_dok=0.;
while(cur.read_cursor(&row) != 0)
 {
  /*printf("%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[7]);*/
  
  if(l_zar_dok_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->nomdok_tv.ravno(),row[2],0) == 0 && iceb_u_SRAV(data->datad.ravno(),iceb_u_datzap(row[1]),0) == 0)
    data->snanomer=data->kolzap;

  suma_dok=0.;
  //Узнаём сумму по документа
  sprintf(strsql,"select SUM(suma) from Zarp where datz='%s' and nd='%s'",row[1],row[2]);
  
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    if(row1[0] != NULL)
     suma_dok=atof(row1[0]);
   }
  ss[COL_SUMA].new_plus(suma_dok);

  //Дата
  ss[COL_DATAD].new_plus(iceb_u_datzap(row[1]));
  
  //Номер документа
  ss[COL_NOMD].new_plus(iceb_u_toutf(row[2]));

  //Подразделение
  ss[COL_PODR].new_plus(iceb_u_toutf(row[3]));
  sprintf(strsql,"select naik from Podr where kod=%s",row[3]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    ss[COL_PODR].plus(" ");
    ss[COL_PODR].plus(iceb_u_toutf(row1[0]));
   }

  //Коментарий
  ss[COL_KOMENT].new_plus(iceb_u_toutf(row[4]));
  
  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[6])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[5],0,data->window));

  /*Метка прихода/расхода*/
  if(row[7][0] == '2')
    ss[COL_METKA_ZAP].new_plus("-");
  else
    ss[COL_METKA_ZAP].new_plus("+");
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_DATAD,ss[COL_DATAD].ravno(),
  COL_NOMD,ss[COL_NOMD].ravno(),
  COL_PODR,ss[COL_PODR].ravno(),
  COL_KOMENT,ss[COL_KOMENT].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  COL_METKA_ZAP,ss[COL_METKA_ZAP].ravno(),
  COL_SUMA,ss[COL_SUMA].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->datad_tv.new_plus("");
data->nomdok_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_zar_dok_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
 {
//  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Список документов"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

if(data->poisk.metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.datan.ravno(),gettext("Дата начала"));
  iceb_str_poisk(&zagolov,data->poisk.datak.ravno(),gettext("Дата конца"));
  iceb_str_poisk(&zagolov,data->poisk.podr.ravno(),gettext("Подразделение"));
  iceb_str_poisk(&zagolov,data->poisk.koment.ravno(),gettext("Коментарий"));
  iceb_str_poisk(&zagolov,data->poisk.nomdok.ravno(),gettext("Номер документа"));
  

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

void l_zar_dok_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,"M", renderer,"text", COL_METKA_ZAP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата док-та"), renderer,"text", COL_DATAD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Ном.док."), renderer,"text", COL_NOMD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Сумма"), renderer,"text", COL_SUMA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Подразделение"), renderer,"text", COL_PODR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Коментарий"), renderer,"text", COL_KOMENT,NULL);

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

void l_zar_dok_vibor(GtkTreeSelection *selection,class l_zar_dok_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *datd;
gchar *nomd;
gint  nomer;
gchar *metka_zap;

gtk_tree_model_get(model,&iter,
COL_DATAD,&datd,
COL_NOMD,&nomd,
COL_METKA_ZAP,&metka_zap,
NUM_COLUMNS,&nomer,-1);

data->datad.new_plus(iceb_u_fromutf(datd));
data->nomdok.new_plus(iceb_u_fromutf(nomd));
data->snanomer=nomer;

if(metka_zap[0] == '+')
 data->prn=1;
else
 data->prn=2;
 
g_free(datd);
g_free(nomd);
g_free(metka_zap);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_zar_dok_knopka(GtkWidget *widget,class l_zar_dok_data *data)
{
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_zar_dok_knopka knop=%d\n",knop);
data->kl_shift=0;
switch (knop)
 {
  case FK2:
    if((data->prn=l_zar_dok_prn(data->window)) < 1)
      return;
    
    data->datad_tv.new_plus("");
    data->nomdok_tv.new_plus("");
    if(l_zar_dok_hap(&data->datad_tv,&data->nomdok_tv,data->prn,data->window) == 0)
      l_zar_dok_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    l_zar_dok_zap(data->prn,data->datad.ravno(),data->nomdok.ravno(),data->window);

    l_zar_dok_create_list(data);
    return;  
/****************
  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    l_zar_dok_udzap(data);
    l_zar_dok_create_list(data);
    return;  
  
****************/

  case FK4:
    if(l_zar_dok_p(&data->poisk,data->window)  == 0)
     {
      if(data->poisk.datan.getdlinna() > 1)
       iceb_u_rsdat(&data->dn,&data->mn,&data->gn,data->poisk.datan.ravno(),1);
      if(data->poisk.datak.getdlinna() > 1)
       iceb_u_rsdat(&data->dk,&data->mk,&data->gk,data->poisk.datak.ravno(),1);
     }
    else
     {
      data->dn=data->dz;
      data->mn=data->mz;
      data->gn=data->gz;
      data->dk=data->mk=data->gk=0;
     }
    l_zar_dok_create_list(data);

    return;  

  case FK5:
    l_zar_dok_rasp(data);
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

gboolean   l_zar_dok_key_press(GtkWidget *widget,GdkEventKey *event,class l_zar_dok_data *data)
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
/*****   
  case GDK_F3:
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    return(TRUE);
************/
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
void l_zar_dok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_zar_dok_data *data)
{
//printf("l_zar_dok_v_row\n");
//printf("l_zar_dok_v_row корректировка\n");
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");


}

/****************************/
/*Проверка записей          */
/*****************************/

int  l_zar_dok_prov_row(SQL_str row,class l_zar_dok_data *data)
{
if(data->poisk.metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->poisk.podr.ravno(),row[3],0,0) != 0)
 return(1);

//Полное сравнение
if(iceb_u_proverka(data->poisk.nomdok.ravno(),row[2],0,0) != 0)
 return(1);


//Поиск образца в строке
if(data->poisk.koment.ravno()[0] != '\0' && iceb_u_strstrm(row[4],data->poisk.koment.ravno()) == 0)
 return(1);


   
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_zar_dok_rasp(class l_zar_dok_data *data)
{
char strsql[512];
SQL_str row,row1;
FILE *ff;
SQLCURSOR cur,cur1;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;


if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return;
 }

sprintf(strsql,"grupp%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список документов"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_u_zagolov(gettext("Список документов"),data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,organ,ff);

if(data->poisk.metka_poi == 1)
 {

  if(data->poisk.podr.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Подразделение"),data->poisk.podr.ravno());
  if(data->poisk.koment.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Коментарий"),data->poisk.koment.ravno());

 }

fprintf(ff,"\
----------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
   Дата   |Номер д.|   Подразделение    |Дата и время запис.| Кто записал\n"));
/*
1234567890 12345678 12345678901234567890
*/
fprintf(ff,"\
----------------------------------------------------------------------------------\n");

iceb_u_str s5;
iceb_u_str s6;


char naim[512];
int kolzap=0;
while(cur.read_cursor(&row) != 0)
 {
  if(l_zar_dok_prov_row(row,data) != 0)
    continue;
  
  strcpy(naim,row[3]);

  sprintf(strsql,"select naik from Podr where kod=%s",row[3]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    strcat(naim," ");
    strcat(naim,row1[0]);
   }
  
     
  //Дата и время записи
  s5.new_plus(iceb_u_vremzap(row[6]));

  //Кто записал
  s6.new_plus(iceb_kszap(row[5],1,data->window));

  fprintf(ff,"%10s %-*s %-*.*s %s %s\n",
  iceb_u_datzap(row[1]),
  iceb_u_kolbait(8,row[2]),row[2],
  iceb_u_kolbait(20,naim),iceb_u_kolbait(20,naim),naim,
  s5.ravno(),s6.ravno());
  kolzap++;
 }
fprintf(ff,"\
----------------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolzap);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,"",0,data->window);

}
/****************************/
/*Выбор прихода/расхода*/
/*************************/
int l_zar_dok_prn(GtkWidget *wpredok)
{
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Ввод начислений"));//0
punkt_m.plus(gettext("Ввод удержаний"));//1


int nomer=0;
return(iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok)+1);


}
