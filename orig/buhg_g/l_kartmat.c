/*$Id: l_kartmat.c,v 1.30 2011-02-21 07:35:52 sasa Exp $*/
/*11.11.2008	19.05.2004	Белых А.И.	l_kartmat.c
Промотр списка карточек определенного материалла
*/
#include <stdlib.h>
#include "buhg_g.h"

enum
{
 FK3,
 FK6,
 FK7,
 FK10,
 KOL_F_KL
};

enum
{
 COL_SKL,
 COL_NKART,
 COL_SHET,
 COL_EI,
 COL_NDS,
 COL_CENA,
 COL_OST,  
 COL_OSTDOK,
 COL_KRAT,
 COL_DATA_KON_IS,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  kartmat_data
 {
  public:

  iceb_u_str sklv;
  iceb_u_str n_kartv;


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
  
  iceb_u_str kodm; //Код материалла карточки которого ищем
  iceb_u_str naimat;
  short      metka_poi;
  short      dos,mos,gos; //дата на которую вычисляем остаток
    
  //Конструктор
  kartmat_data()
   {
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
    metka_poi=0;
   }      
 };

gboolean   kartmat_key_press(GtkWidget *widget,GdkEventKey *event,class kartmat_data *data);
void kartmat_vibor(GtkTreeSelection *selection,class kartmat_data *data);
void kartmat_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class kartmat_data *data);
void  kartmat_knopka(GtkWidget *widget,class kartmat_data *data);
void kartmat_add_columns (GtkTreeView *treeview);
void kartmat_create_list(class kartmat_data *data);

extern SQL_baza	bd;
extern char *name_system;
extern short ddd,mmm,ggg;


int  l_kartmat(const char *kodm,iceb_u_str *skl,iceb_u_str *n_kart,GtkWidget *wpredok)
{
kartmat_data data;
char strsql[512];
char bros[512];
GdkColor color;
SQL_str row;
SQLCURSOR cur;
iceb_u_str soob;
short ddd,mmm,ggg;
iceb_u_poltekdat(&ddd,&mmm,&ggg);

data.kodm.plus(kodm);
data.dos=ddd;
data.mos=mmm;
data.gos=ggg;

//Читаем наименование материалла
sprintf(strsql,"select naimat from Material where kodm=%s",kodm);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 data.naimat.plus(row[0]);
else 
 data.naimat.plus("");



data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Список карточек"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(kartmat_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

soob.new_plus(gettext("Список карточек"));
soob.ps_plus(data.kodm.ravno());
soob.plus(" ");
soob.plus(data.naimat.ravno());

data.label_kolstr=gtk_label_new (soob.ravno_toutf());


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");
gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_poisk,GTK_STATE_NORMAL,&color);

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),600,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];


sprintf(bros,"F3 %s",gettext("Просмотр"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(kartmat_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Просмотр карточки материалла"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F6 %s",gettext("Смена даты"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(kartmat_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Смена даты получения остатка"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F7 %s",gettext("Остаток"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(kartmat_knopka),&data);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Показать карточки с остатками"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(kartmat_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

kartmat_create_list(&data);

gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();

if(data.metka_voz == 0)
 {
  skl->new_plus(data.sklv.ravno());
  n_kart->new_plus(data.n_kartv.ravno());
 }

printf("l_kartmat end\n");
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.metka_voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void kartmat_create_list (class kartmat_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

iceb_clock skur(data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;

//printf("kartmat_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

//gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(kartmat_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(kartmat_vibor),data);

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
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);


sprintf(strsql,"select * from Kart where kodm=%s order by sklad,nomk desc",data->kodm.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
double ost=0.;
double ostdok=0.;
double iost=0.;
double iostdok=0.;
ostatok data_ost;
ostatok data_ostdok;
short d,m,g;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  //остаток
  ostkarw(1,1,data->gos,data->dos,data->mos,data->gos,row[0],row[1],&data_ost);
  ost=data_ost.ostg[3];
  if(data->metka_poi == 1)
   if(ost == 0.)
     continue;
     
  iost+=ost;
  sprintf(strsql,"%10.10g",ost);
  ss[COL_OST].new_plus(strsql);


  //Единица измерения
  ss[COL_EI].new_plus(iceb_u_toutf(row[4]));

  //Счёт
  ss[COL_SHET].new_plus(iceb_u_toutf(row[5]));
  
  //Цена
  sprintf(strsql,"%10.10g",atof(row[6]));
  ss[COL_CENA].new_plus(strsql);
  

  //остаток по документам
  ostdokw(1,1,data->gos,data->dos,data->mos,data->gos,row[0],row[1],&data_ostdok);
  ostdok=data_ostdok.ostg[3];
  iostdok+=ostdok;
  sprintf(strsql,"%10.10g",ostdok);
  ss[COL_OSTDOK].new_plus(strsql);

  iceb_u_rsdat(&d,&m,&g,row[18],2);
  sprintf(strsql,"%02d.%02d.%d",d,m,g);
  if(d != 0)
    ss[COL_DATA_KON_IS].new_plus(strsql);
  else
    ss[COL_DATA_KON_IS].new_plus("");    

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[13])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[12],0,data->window));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_SKL,row[0],
  COL_NKART,row[1],
  COL_SHET,ss[COL_SHET].ravno(),
  COL_EI,ss[COL_EI].ravno(),
  COL_NDS,row[9],
  COL_CENA,ss[COL_CENA].ravno(),
  COL_OST,ss[COL_OST].ravno(),
  COL_OSTDOK,ss[COL_OSTDOK].ravno(),
  COL_KRAT,ss[COL_KRAT].ravno(),
  COL_DATA_KON_IS,ss[COL_DATA_KON_IS].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

kartmat_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Список карточек"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);
zagolov.ps_plus(data->kodm.ravno());
zagolov.plus(" ");
zagolov.plus(data->naimat.ravno());
zagolov.ps_plus(gettext("Остаток на"));
sprintf(strsql,"%d.%d.%d",data->dos,data->mos,data->gos);
zagolov.plus(" ");
zagolov.plus(strsql);

zagolov.ps_plus(gettext("Остаток на карточках"));
zagolov.plus(":");
zagolov.plus(iost);

zagolov.plus(" ");
zagolov.plus(gettext("Остаток по документам"));
zagolov.plus(":");
zagolov.plus(iostdok);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

if(data->metka_poi == 1)
 {
  printf("Формирование заголовка с реквизитами поиска.\n");
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");
  zagolov.ps_plus(gettext("Только карточки с ненулевым остатком"));

  gtk_label_set_text(GTK_LABEL(data->label_poisk),zagolov.ravno_toutf());
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 

gtk_widget_show(data->label_kolstr);

//gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));


}

/*****************/
/*Создаем колонки*/
/*****************/

void kartmat_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

printf("kartmat_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Склад"), renderer,"text", COL_SKL,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Карточка"), renderer,"text", COL_NKART,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Счёт"), renderer,"text", COL_SHET,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Е/и"), renderer,"text", COL_EI,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("НДС"), renderer,"text", COL_NDS,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Цена"), renderer,"text", COL_CENA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Остаток"), renderer,"text", COL_OST,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Остаток"), renderer,"text", COL_OSTDOK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Кон.дата исп."), renderer,"text", COL_DATA_KON_IS,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Кратность"), renderer,"text", COL_KRAT,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);
printf("kartmat_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void kartmat_vibor(GtkTreeSelection *selection,class kartmat_data *data)
{
printf("kartmat_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gchar *naim;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_SKL,&kod,COL_NKART,&naim,NUM_COLUMNS,&nomer,-1);

data->sklv.new_plus(iceb_u_fromutf(kod));
data->n_kartv.new_plus(iceb_u_fromutf(naim));
data->snanomer=nomer;

g_free(kod);
g_free(naim);

//printf("%s %s %d\n",data->kodv.ravno(),data->naimv.ravno(),nomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  kartmat_knopka(GtkWidget *widget,class kartmat_data *data)
{
//char    bros[512];
iceb_u_str repl;
iceb_u_str vdata;
short d,m,g;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("kartmat_knopka knop=%d\n",knop);


switch (knop)
 {

  case FK3:
    if(data->kolzap == 0)
      return;
    l_zkartmat(data->sklv.ravno(),data->n_kartv.ravno(),data->window);
    return;  

  case FK6:
    if(data->kolzap == 0)
      return;
    repl.new_plus(gettext("Введите дату"));
    if(iceb_menu_vvod1(&repl,&vdata,11,data->window) != 0)
      return;  
    
    if(iceb_u_rsdat(&d,&m,&g,vdata.ravno(),1) != 0)
     {
      repl.new_plus(gettext("Не правильно введена дата !"));
      iceb_menu_soob(&repl,data->window);
      return;
     }
    data->dos=d;
    data->mos=m;
    data->gos=g;
    kartmat_create_list(data);
    
    return;  

  case FK7:
    data->metka_poi++;
    if(data->metka_poi == 2)
     data->metka_poi=0;
    kartmat_create_list(data);
     
    return;  

    
  case FK10:
//    printf("kartmat_knopka F10\n");
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kartmat_key_press(GtkWidget *widget,GdkEventKey *event,class kartmat_data *data)
{
iceb_u_str repl;
//printf("kartmat_key_press keyval=%d state=%d\n",event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_F3:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    return(TRUE);

  case GDK_F6:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
    return(TRUE);

  case GDK_F7:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK7]),"clicked");
    return(TRUE);

  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("kartmat_key_press-Нажата клавиша Shift\n");

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
void kartmat_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class kartmat_data *data)
{
//printf("kartmat_v_row\n");
//printf("kartmat_v_row корректировка\n");

gtk_widget_destroy(data->window);

data->metka_voz=0;

}
