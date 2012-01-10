/*$Id: l_restdok2_vmu.c,v 1.10 2011-02-21 07:36:20 sasa Exp $*/
/*06.11.2007	21.03.2006	Белых А.И.	l_restdok2_vmu.c
Создание списка материаллов/услуг
*/


#include <stdlib.h>
#include "i_rest.h"
#include "l_restdok2.h"


enum
{
 COL_MU_KOD,
 COL_MU_OSTATOK,
 COL_MU_NAIM,
 COL_MU_CENA,
 COL_MU_EI,
 NUM_MU_COL
};

void resdok2_vmu_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class restdok2_data *data);

void resdok2_vmu_vibor(GtkTreeSelection *selection,class restdok2_data *data);
void resdok2_vmu_add_columns(GtkTreeView *treeview,class  restdok2_data*);

extern SQL_baza bd;

void l_restdok2_vmu(class restdok2_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
char strsql[512];
int  kolstr=0;
SQLCURSOR cur;
SQL_str   row;

//printf("l_restdok2_vmu\n");

data->kl_shift=0; //0-отжата 1-нажата  

//gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

if(data->spisok_mu.treeview != NULL)
  gtk_widget_destroy(data->spisok_mu.treeview);

data->spisok_mu.treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->spisok_mu.sw),data->spisok_mu.treeview);

g_signal_connect(data->spisok_mu.treeview,"row_activated",G_CALLBACK(resdok2_vmu_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->spisok_mu.treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(resdok2_vmu_vibor),data);

gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (data->spisok_mu.treeview), TRUE); //Устанавливаются полоски при отображении списка
gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->spisok_mu.treeview)),GTK_SELECTION_SINGLE);



model = gtk_list_store_new (NUM_MU_COL+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);

//printf("data->metka_mat_usl=%d\n",data->metka_mat_usl);

//gtk_list_store_clear(model);
if(data->metka_mat_usl == 0)
  sprintf(strsql,"select kodm,naimat,ei,cenapr from Material where kodgr=%d \
  and cenapr != 0. and ei != '' order by naimat asc",data->kod_grupv);
if(data->metka_mat_usl == 1)
  sprintf(strsql,"select kodus,naius,ei,cena from Uslugi where kodgr=%d \
  and cena != 0. and ei != '' order by naius asc",data->kod_grupv);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

class iceb_u_str ss[NUM_MU_COL];

double ostatok=0.;
data->spisok_mu.kolzap=0;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  ostatok=0.; 
  if(data->metka_mat_usl == 0 && data->metka_kontr_ost == 1)
    ostatok=osttov(atoi(row[0]),data->skladv.ravno_atoi(),row[2],"",data->window);

  //остаток
  ss[COL_MU_OSTATOK].new_plus(ostatok);
  
  //код
  ss[COL_MU_KOD].new_plus(row[0]);
  
  //наименование
  ss[COL_MU_NAIM].new_plus(iceb_u_toutf(row[1]));

  //единица измерения
  ss[COL_MU_EI].new_plus(iceb_u_toutf(row[2]));


  //цена
  sprintf(strsql,"%.2f",atof(row[3]));  
  ss[COL_MU_CENA].new_plus(strsql);

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_MU_KOD,ss[COL_MU_KOD].ravno(),
  COL_MU_OSTATOK,ss[COL_MU_OSTATOK].ravno(),
  COL_MU_NAIM,ss[COL_MU_NAIM].ravno(),
  COL_MU_EI,ss[COL_MU_EI].ravno(),
  COL_MU_CENA,ss[COL_MU_CENA].ravno(),
  NUM_MU_COL,data->spisok_mu.kolzap,
  -1);

  data->spisok_mu.kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data->spisok_mu. treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

resdok2_vmu_add_columns (GTK_TREE_VIEW (data->spisok_mu.treeview),data);



gtk_widget_show (data->spisok_mu.treeview);
gtk_widget_show (data->spisok_mu.sw);

if(data->spisok_mu.kolzap > 0)
  l_restdok2_setstr(&data->spisok_mu); //стать на нужную строку


if(data->metka_mat_usl == 1)
   gtk_widget_set_sensitive(GTK_WIDGET(data->knopka_mu[MU_FK_OSTATOK]),FALSE);//Недоступна
else
   gtk_widget_set_sensitive(GTK_WIDGET(data->knopka_mu[MU_FK_OSTATOK]),TRUE);//доступна
}

/*****************/
/*Создаем колонки*/
/*****************/

void resdok2_vmu_add_columns(GtkTreeView *treeview,class  restdok2_data *data)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

//printf("resdok2_vmu_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
if(data->metka_mat_usl == 1)
  g_object_set(G_OBJECT(renderer),"foreground","blue",NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_MU_KOD,NULL);

if(data->metka_mat_usl == 0)
if(data->metka_kontr_ost == 1)
 {
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
  -1,gettext("Остаток"), renderer,"text", COL_MU_OSTATOK,NULL);
 }


renderer = gtk_cell_renderer_text_new ();
if(data->metka_mat_usl == 1)
  g_object_set(G_OBJECT(renderer),"foreground","blue",NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,\
gettext("Наименование"), renderer,"text", COL_MU_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
if(data->metka_mat_usl == 1)
  g_object_set(G_OBJECT(renderer),"foreground","blue",NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,\
gettext("Цена"), renderer,"text", COL_MU_CENA,NULL);

renderer = gtk_cell_renderer_text_new ();
if(data->metka_mat_usl == 1)
  g_object_set(G_OBJECT(renderer),"foreground","blue",NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,\
gettext("Е/и"), renderer,"text", COL_MU_EI,NULL);



//printf("resdok2_vmu_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void resdok2_vmu_vibor(GtkTreeSelection *selection,class restdok2_data *data)
{
//printf("resdok2_vmu_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_MU_KOD,&kod,NUM_MU_COL,&nomer,-1);


data->kod_matus=atoi(kod);
data->spisok_mu.snanomer=nomer;

g_free(kod);

//printf("data->kod_matus-%d  data->skladv=%s\n",data->kod_matus,data->skladv.ravno());

}

/****************************/
/*Выбор строки*/
/**********************/
void resdok2_vmu_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class restdok2_data *data)
{
//printf("resdok2_vmu_v_row\n");
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka_mu[MU_FK_VIBOR]),"clicked");
}


