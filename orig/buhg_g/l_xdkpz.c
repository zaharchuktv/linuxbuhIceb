/*$Id: l_xdkpz.c,v 1.15 2011-02-21 07:35:54 sasa Exp $*/
/*25.04.2007	06.05.2006	Белых А.И.	l_xdkpz.c
Работа с подтверждающими записями для платёжных документов
*/
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include "buhg_g.h"
#include "dok4w.h"
enum
{
 FK2,
 SFK2,
 FK3,
 SFK3,
 FK4,
 FK5,
 FK10,
 KOL_F_KL
};

enum
{
 COL_DATAP,
 COL_SUMAP,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  xdkpz_data
 {
  public:
  GtkWidget *label_kolstr;
//  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать 
  int       kolzap;     //Количество записей

  iceb_u_str zapros;
  int metka_dok; //0-платёжка 1-требование
  char tablica[50];
      
  //реквизиты выбранной записи
  iceb_u_str data_pod;
  double     suma_pod;

  //Конструктор
  xdkpz_data()
   {
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    suma_pod=0.;
   }      
 };
gboolean   xdkpz_key_press(GtkWidget *widget,GdkEventKey *event,class xdkpz_data *data);
void  xdkpz_knopka(GtkWidget *widget,class xdkpz_data *data);
void xdkpz_create_list (class xdkpz_data *data);
void xdkpz_vibor(GtkTreeSelection *selection,class xdkpz_data *data);
void xdkpz_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class xdkpz_data *data);

int l_xdkpz_v(const char *tablica,const char *data_pod,double suma_pod,GtkWidget *wpredok);
void l_xdkpz_rz(class xdkpz_data *data);

extern SQL_baza	bd;
extern char *name_system;
extern char     *organ;
extern class REC rec;

void l_xdkpz(char *tablica,GtkWidget *wpredok)
{
xdkpz_data data;
char bros[512];
//GdkColor color;

if(iceb_u_SRAV(tablica,"Pltp",0) == 0)
 data.metka_dok=0;
if(iceb_u_SRAV(tablica,"Pltt",0) == 0)
 data.metka_dok=1;
strcpy(data.tablica,tablica);
 
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);

sprintf(bros,"%s %s",name_system,gettext("Список подтверждённых записей"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(xdkpz_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);


data.label_kolstr=gtk_label_new (gettext("Список подтверждённых записей"));

gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);


data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),-1,200);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(bros,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(xdkpz_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(xdkpz_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(xdkpz_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удалить выбранную запись"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(xdkpz_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Удалить все записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(xdkpz_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(xdkpz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

xdkpz_create_list(&data);
gtk_widget_show(data.window);

//if(metka == 0)
//  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


//printf("l_xdkpz end\n");

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  xdkpz_knopka(GtkWidget *widget,class xdkpz_data *data)
{
char strsql[512];
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:

    l_xdkpz_v(data->tablica,"",0,data->window);
    xdkpz_create_list(data);
    return;  

  case SFK2:
    l_xdkpz_v(data->tablica,data->data_pod.ravno(),data->suma_pod,data->window);
    xdkpz_create_list(data);
    return;  

  case FK3:
    
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
      return;    

    if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0)
     sprintf(strsql,"delete from Pzpd where tp=0 and \
datd='%04d-%02d-%02d' and datp='%s' and nomd='%s' and suma=%.2f",
     rec.gd,rec.md,rec.dd,data->data_pod.ravno_sqldata(),rec.nomdk.ravno(),data->suma_pod);

    if(iceb_u_SRAV(data->tablica,"Pltt",0) == 0)
       sprintf(strsql,"delete from Pzpd where tp=1 and \
datd='%04d-%02d-%02d' and datp='%s' and nomd='%s' and suma=%.2f",
       rec.gd,rec.md,rec.dd,data->data_pod.ravno_sqldata(),rec.nomdk.ravno(),data->suma_pod);

    iceb_sql_zapis(strsql,1,0,data->window);

    xdkpz_create_list(data);
    return;  

  case SFK3:
    if(iceb_menu_danet(gettext("Удалить все записи ? Вы уверены ?"),2,data->window) == 2)
      return;    

    if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0)
     sprintf(strsql,"delete from Pzpd where tp=0 and \
datd='%04d-%02d-%02d' and datp='%s' and nomd='%s'",
     rec.gd,rec.md,rec.dd,data->data_pod.ravno_sqldata(),rec.nomdk.ravno());

    if(iceb_u_SRAV(data->tablica,"Pltt",0) == 0)
       sprintf(strsql,"delete from Pzpd where tp=1 and \
datd='%04d-%02d-%02d' and datp='%s' and nomd='%s'",
       rec.gd,rec.md,rec.dd,data->data_pod.ravno_sqldata(),rec.nomdk.ravno());

    iceb_sql_zapis(strsql,1,0,data->window);

    xdkpz_create_list(data);
    return;  

  case FK5:
    l_xdkpz_rz(data);
    return;  

  
  case FK10:
//    printf("xdkpz_knopka F10\n");
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   xdkpz_key_press(GtkWidget *widget,GdkEventKey *event,class xdkpz_data *data)
{
iceb_u_str repl;
//printf("xdkpz_key_press keyval=%d state=%d\n",event->keyval,event->state);


switch(event->keyval)
 {
  case GDK_F2:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    if(data->kl_shift == 1)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
    return(TRUE);

  case GDK_F3:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    if(data->kl_shift == 1)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK3]),"clicked");
    return(TRUE);

  case GDK_F4:
//    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
    return(TRUE);

  case GDK_F5:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
    return(TRUE);

  case GDK_F11:
//    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK11]),"clicked");
    return(TRUE);

  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("xdkpz_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/*****************/
/*Создаем колонки*/
/*****************/

void xdkpz_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

//printf("xdkpz_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата под-ждения"), renderer,"text",COL_DATAP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Сумма"), renderer,"text", COL_SUMAP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

//printf("xdkpz_add_columns end\n");

}

/***********************************/
/*Создаем список для просмотра */
/***********************************/
void xdkpz_create_list (class xdkpz_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQL_str row;
char strsql[512];
//char bros[500];
int  kolstr=0;
iceb_u_str zagolov;

//printf("xdkpz_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(xdkpz_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(xdkpz_vibor),data);

gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (data->treeview), TRUE); //Устанавливаются полоски при отображении списка
gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);


model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING,
G_TYPE_STRING, 
G_TYPE_STRING,
G_TYPE_INT);

sprintf(strsql,"select datp,suma,ktoz,vrem from Pzpd where tp=%d and nomd='%s' and datd = '%d-%02d-%02d' \
order by datp asc",data->metka_dok,rec.nomdk.ravno(),rec.gd,rec.md,rec.dd);


data->zapros.new_plus(strsql);

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
  

  //Дата подтверждения
  ss[COL_DATAP].new_plus(iceb_u_sqldata(row[0]));
  
  //Сумма
  ss[COL_SUMAP].new_plus(row[1]);

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[3])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[2],0,data->window));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_DATAP,ss[COL_DATAP].ravno(),
  COL_SUMAP,ss[COL_SUMAP].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

xdkpz_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

zagolov.new_plus(gettext("Список подтверждённых записей"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);
sprintf(strsql,"%s:%02d.%02d.%d %s:%s",
gettext("Дата"),
rec.dd,rec.md,rec.gd,
gettext("Номер документа"),
rec.nomdk.ravno());

zagolov.ps_plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));

}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void xdkpz_vibor(GtkTreeSelection *selection,class xdkpz_data *data)
{
printf("xdkpz_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *data_pod;
gchar *suma_pod;

gint  nomer;


gtk_tree_model_get(model,&iter,COL_DATAP,&data_pod,COL_SUMAP,&suma_pod,NUM_COLUMNS,&nomer,-1);

data->data_pod.new_plus(data_pod);
data->suma_pod=atof(suma_pod);

data->snanomer=nomer;

g_free(data_pod);
g_free(suma_pod);

//printf("xdkpz_vibor-%s %d\n",data->kodv.ravno(),data->snanomer);

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void xdkpz_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class xdkpz_data *data)
{
//Взять на корректировку выбранную запись
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");


}
/********************************/
/*Распечатка подтверждающих записей*/
/***********************************/
void l_xdkpz_rz(class xdkpz_data *data)
{
SQL_str row;
SQLCURSOR cur;
FILE *ff;
char imaf_r[50];
int kolstr;

if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return;
 }
if(kolstr == 0)
 return;

sprintf(imaf_r,"xdkpz%d.lst",getpid());

if((ff=fopen(imaf_r,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_r,"",errno,data->window);
  return;
 }
iceb_u_startfil(ff);
iceb_u_zagolov(gettext("Список подтверждённых записей"),0,0,0,0,0,0,organ,ff);

if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0)
 fprintf(ff,"%s.\n",gettext("Платёжное поручение"));
if(iceb_u_SRAV(data->tablica,"Pltt",0) == 0)
 fprintf(ff,"%s.\n",gettext("Платёжное требование"));
 
fprintf(ff,"%s:%02d.%02d.%d %s:%s\n",gettext("Дата документа"),
rec.dd,rec.md,rec.gd,
gettext("Номер документа"),
rec.nomdk.ravno());

short d,m,g;

fprintf(ff,"\
-------------------------------------------------------------\n");

fprintf(ff,"\
Дата подт.|  Сумма   |Дата и время запис.| Кто записал\n");

fprintf(ff,"\
-------------------------------------------------------------\n");
double suma_it=0.;

iceb_u_str ss[2];

while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);

  //Дата и время записи
  ss[0].new_plus(iceb_u_vremzap(row[3]));

  //Кто записал
  ss[1].new_plus(iceb_kszap(row[2],1,data->window));
  
  fprintf(ff,"%02d.%02d.%d %10.2f %s %s\n",d,m,g,atof(row[1]),ss[0].ravno(),ss[1].ravno());
  suma_it+=atof(row[1]);
 }

fprintf(ff,"\
-------------------------------------------------------------\n");
fprintf(ff,"%10s %10.2f\n",gettext("Итого"),suma_it);

iceb_podpis(ff,data->window);
fclose(ff);

class iceb_u_spisok imaf;
class iceb_u_spisok naimf;
imaf.plus(imaf_r);
naimf.plus(gettext("Список подтверждённых записей"));
iceb_rabfil(&imaf,&naimf,NULL,0,data->window);
}

