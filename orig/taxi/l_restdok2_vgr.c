/*$Id: l_restdok2_vgr.c,v 1.12 2011-02-21 07:36:20 sasa Exp $*/
/*15.05.2006	21.03.2006	Белых А.И.	l_restdok2_vgr.c
Подпрограмма создания списка групп для выбора материалла/услуги
*/
#include <stdlib.h>
#include "i_rest.h"
#include "l_restdok2.h"
enum
{
 COL_GR_KOD,
 COL_GR_NAIM,
 NUM_GR_COL
};

void l_restdok2_gr_add_columns(GtkTreeView *treeview,short);
void l_restdok2_gr_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class restdok2_data *data);
void l_restdok2_gr_vibor(GtkTreeSelection *selection,class restdok2_data *data);


extern SQL_baza bd;

void l_restdok2_vgr(class restdok2_data *data)
{
//printf("l_restdok2_vgr\n");

int skl_pod=0; //код склада или подразделения
if(data->metka_mat_usl == 0)
  skl_pod=data->skladv.ravno_atoi();
if(data->metka_mat_usl == 1)
  skl_pod=data->podrv.ravno_atoi();
if(skl_pod == 0)
 {
  if(data->metka_mat_usl == 0)
   iceb_menu_soob(gettext("Не выбран склад !"),data->window);  
  if(data->metka_mat_usl == 1)
   iceb_menu_soob(gettext("Не выбрано подразделение !"),data->window);  
  return;
 }

GtkListStore *model=NULL;
GtkTreeIter iter;
data->kl_shift=0; //0-отжата 1-нажата  

//gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

if(data->spisok_gr.treeview != NULL)
  gtk_widget_destroy(data->spisok_gr.treeview);

data->spisok_gr.treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->spisok_gr.sw), data->spisok_gr.treeview);

g_signal_connect(data->spisok_gr.treeview,"row_activated",G_CALLBACK(l_restdok2_gr_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->spisok_gr.treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
//g_signal_connect(selection,"changed",G_CALLBACK(l_restdok2_gr_vibor),data);
g_signal_connect(selection,"changed",G_CALLBACK(l_restdok2_gr_vibor),data);

gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (data->spisok_gr.treeview), TRUE); //Устанавливаются полоски при отображении списка
gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->spisok_gr.treeview)),GTK_SELECTION_SINGLE);


model = gtk_list_store_new (NUM_GR_COL+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);

data->spisok_gr.kolzap=0;
char spisok[2048];
memset(spisok,'\0',sizeof(spisok));
iceb_u_str imaf_nastr("restnast.alx");
//iceb_imafn("restnast.alx",&imaf_nastr);

if(data->metka_mat_usl == 0)
 {
  char bros[512];
  sprintf(bros,"Список групп товаров для склада %d",skl_pod);
  iceb_poldan(bros,spisok,imaf_nastr.ravno(),data->window);
 }

if(data->metka_mat_usl == 1)
 {
  char bros[512];
  sprintf(bros,"Список групп услуг для подразделения %d",skl_pod);
  iceb_poldan(bros,spisok,imaf_nastr.ravno(),data->window);
 }

//printf("spisok=%s\n",spisok);

int kolih=iceb_u_pole2(spisok,',');

if(kolih == 0 && spisok[0] != '\0')
  kolih=1;


char  kod[40];
iceb_u_str naim;
char strsql[512];
SQL_str row;
SQLCURSOR cur;

for(int i=1; i <= kolih; i++)
 {
  naim.new_plus("");
  memset(kod,'\0',sizeof(kod));
  if(kolih > 1)
    iceb_u_polen(spisok,kod,sizeof(kod),i,',');
  else
   strncpy(kod,spisok,sizeof(kod)-1);
   

  if(data->metka_mat_usl == 0) //список групп для склада
    sprintf(strsql,"select naik from Grup where kod=%s",kod);



  if(data->metka_mat_usl == 1) //список групп услуг
   sprintf(strsql,"select naik from Uslgrup where kod=%s",kod);
//  printf("strsql=%s\n",strsql);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    naim.new_plus(row[0]);        
    
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_GR_KOD,kod,
  COL_GR_NAIM,naim.ravno_toutf(),
  NUM_GR_COL,data->spisok_gr.kolzap,
  -1);

  data->spisok_gr.kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data->spisok_gr.treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_restdok2_gr_add_columns (GTK_TREE_VIEW (data->spisok_gr.treeview),data->metka_mat_usl);



gtk_widget_show (data->spisok_gr.treeview);
gtk_widget_show (data->spisok_gr.sw);


if(data->spisok_gr.kolzap > 0)
 {
  GtkTreePath *path=gtk_tree_path_new_from_string("0");

  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->spisok_gr.treeview),path,NULL,FALSE,0,0);


  gtk_tree_view_set_cursor(GTK_TREE_VIEW (data->spisok_gr.treeview),path,NULL,FALSE);
  gtk_tree_path_free(path);


 }

gtk_widget_show_all(data->gor_panel);

}
/*****************/
/*Создаем колонки*/
/*****************/

void l_restdok2_gr_add_columns(GtkTreeView *treeview,short metka)
{
GtkCellRenderer *renderer;

//printf("spisok_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
if(metka == 1)
  g_object_set(G_OBJECT(renderer),"foreground","blue",NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_GR_KOD,NULL);

renderer = gtk_cell_renderer_text_new ();

if(metka == 1)
  g_object_set(G_OBJECT(renderer),"foreground","blue",NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,\
gettext("Наименование"), renderer,"text", COL_GR_NAIM,NULL);


//printf("spisok_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/
void l_restdok2_gr_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class restdok2_data *data)
{
//printf("spisok_v_row\n");
// gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
}
/****************************/
/*Выбор строки*/
/**********************/

void l_restdok2_gr_vibor(GtkTreeSelection *selection,class restdok2_data *data)
{
//printf("spisok_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gint  nomer;


gtk_tree_model_get(model,&iter,0,&kod,NUM_GR_COL,&nomer,-1);


data->kod_grupv=atoi(kod);
data->spisok_gr.snanomer=nomer;

g_free(kod);

//формирование списка материалов или услуг
l_restdok2_vmu(data);

}
