/*$Id: l_kartmatv.c,v 1.35 2011-02-21 07:35:52 sasa Exp $*/
/*11.11.2008	19.05.2004	Белых А.И.	l_kartmatv.c
Промотр списка карточек определенного материалла и выбор нужной карточки
Если вернули 0 - выбрали карточку
             1 - не выбрали
             2 - ввод без привязки к карточке
            -1 - ввод новой карточки
*/
#include <stdlib.h>
#include "buhg_g.h"

enum
{
 FK2,
 FK3,
 FK4,
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
 COL_NOMZ,  //Номер заказа
 COL_INNOM, //Инвентарный номер
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  kartmatv_data
 {
  public:

  iceb_u_str sklv;
  iceb_u_str n_kartv;
  iceb_u_str nomerzv; //Номер заказа

  int        nomervsp; //Номер записи в списке


  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  short     snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  
  
  iceb_u_str nomz;
  int        tipz;
  short      dd,md,gd;
  iceb_u_str nomdok;
  iceb_u_str kodm; //Код материалла карточки которого ищем
  int        skl;
  double     cenavdok;
  iceb_u_str naimat;
  short      metka_poi;
  short      dos,mos,gos; //дата на которую вычисляем остаток
  double     kolih_dok;
  int        metka_prk; //0-выбор карточки и запись в неё 1-выбор карточки для привязки к ней           [B
  float pnds;
  //Конструктор
  kartmatv_data()
   {
    snanomer=-2;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
    metka_poi=0;
   }      
 };

gboolean   kartmatv_key_press(GtkWidget *widget,GdkEventKey *event,class kartmatv_data *data);
void kartmatv_vibor(GtkTreeSelection *selection,class kartmatv_data *data);
void kartmatv_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class kartmatv_data *data);
void  kartmatv_knopka(GtkWidget *widget,class kartmatv_data *data);
void kartmatv_add_columns (GtkTreeView *treeview);
void kartmatv_create_list(class kartmatv_data *data);
int kartmatv_zap(class kartmatv_data *data);
void	vmatkolp(int skl,const char *nomdok,int kodm,int nk,short gd,short md,short dd,GtkWidget *wpredok);
int   prihodw(short dd,short md,short gd,int skl,const char *nomd,int km,int nk,double cena,const char *ei,int mnds,double ndsg,const char *nomz,GtkWidget *wpredok);
int   rashodw(short dd,short md,short gd,int skl,const char *nomdok,int nk,int kmm,double kolih,float,GtkWidget *wpredok);

int vkartm(short dd,short md,short gd,const char *nomdok,int tipz,int skl,int kodm,double cenadok,double kolihdok,GtkWidget *wpredok);

extern SQL_baza	bd;
extern char *name_system;


int  l_kartmatv(int tipz,
short dd,short md,short gd, //Дата документа
const char *nomdok, //Номер документа
double cenavdok,
iceb_u_str *nomz,
int kodm,//Если не равно нолю то это код материалла , который нужно привязать к карточке
int skl,int *n_kart,
double kolih_dok,
int metka_prk, //0-выбор карточки и запись в неё 1-выбор карточки для привязки к ней           
float pnds,
GtkWidget *wpredok)
{
kartmatv_data data;
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
data.nomz.new_plus(nomz->ravno());
data.tipz=tipz;
data.dd=dd;
data.md=md;
data.gd=gd;
data.nomdok.new_plus(nomdok);
data.cenavdok=cenavdok;
data.skl=skl;
data.kolih_dok=kolih_dok;
data.metka_prk=metka_prk;
data.pnds=pnds;

printf("%s\n",__FUNCTION__);

//Читаем наименование материалла
sprintf(strsql,"select naimat from Material where kodm=%d",kodm);
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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(kartmatv_key_press),&data);
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


sprintf(bros,"F2 %s",gettext("Новая карточка"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(kartmatv_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой карточки материалла"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"F3 %s",gettext("Просмотр"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(kartmatv_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Просмотр карточки материалла"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Без карточки"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(kartmatv_knopka),&data);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Ввод материалла без привязки к карточке"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F6 %s",gettext("Смена даты"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(kartmatv_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Смена даты получения остатка"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F7 %s",gettext("Остаток"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(kartmatv_knopka),&data);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Показать карточки с остатками"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(kartmatv_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

kartmatv_create_list(&data);

//gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));
gtk_widget_show(data.window);


gtk_main();

if(data.metka_voz == 0)
 {
//  *skl=data.sklv.ravno_int();
  *n_kart=data.n_kartv.ravno_atoi();
 }

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.metka_voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void kartmatv_create_list (class kartmatv_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

class iceb_clock skur(data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;
//GdkColor color;

data->kl_shift=0; //0-отжата 1-нажата  

//gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(kartmatv_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(kartmatv_vibor),data);

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
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);

if(data->nomz.getdlinna() <= 1)
 {
  sprintf(strsql,"select * from Kart where kodm=%d order by sklad,nomk desc",data->kodm.ravno_atoi());

  if(data->tipz == 1)
   sprintf(strsql,"select * from Kart where kodm=%d order by sklad,cena desc",data->kodm.ravno_atoi());
 }
else
 {
  sprintf(strsql,"select * from Kart where kodm=%d and nomz='%s' order by sklad,nomk desc",
  data->kodm.ravno_atoi(),data->nomz.ravno());
 }

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
  
  //Номер заказа
  ss[COL_NOMZ].new_plus(iceb_u_toutf(row[17]));
  
  //Инвентарный номер или серия для мед-прераратов
  ss[COL_INNOM].new_plus(iceb_u_toutf(row[15]));
  
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
  COL_NOMZ,ss[COL_NOMZ].ravno(),
  COL_DATA_KON_IS,ss[COL_DATA_KON_IS].ravno(),
  COL_INNOM,ss[COL_INNOM].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

kartmatv_add_columns (GTK_TREE_VIEW (data->treeview));

if(data->tipz == 2)
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна

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

if(data->snanomer > -2)
 {
  if(data->snanomer  > data->kolzap)
     data->snanomer= data->kolzap;
      
  printf("poz=%d\n",data->snanomer);

  sprintf(strsql,"%d",data->snanomer);
  GtkTreePath *path=gtk_tree_path_new_from_string(strsql);

//  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,TRUE,0,0);
  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE,0,0);

  gtk_tree_view_set_cursor(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE);
  gtk_tree_path_free(path);
  data->snanomer=-2;
 }
else
 {
  if(data->kolzap > 0)
   {
    GtkTreePath *path=gtk_tree_path_new_from_string("0");

    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE,0,0);

    gtk_tree_view_set_cursor(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE);
  //  gtk_tree_view_row_activated(GTK_TREE_VIEW (data->treeview),path,NULL);

    gtk_tree_path_free(path);
    data->snanomer=-2;
   }
 }

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

void kartmatv_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;


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
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Номер заказа"), renderer,"text", COL_NOMZ,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Инвен.номер/серия мед.преп."), renderer,"text", COL_INNOM,NULL);

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

void kartmatv_vibor(GtkTreeSelection *selection,class kartmatv_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gchar *naim;
gchar *nomz;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_SKL,&kod,COL_NKART,&naim,COL_NOMZ,&nomz,NUM_COLUMNS,&nomer,-1);

data->sklv.new_plus(iceb_u_fromutf(kod));
data->n_kartv.new_plus(iceb_u_fromutf(naim));
data->nomerzv.new_plus(iceb_u_fromutf(nomz));
data->nomervsp=nomer;

g_free(kod);
g_free(naim);
g_free(nomz);

//printf("%s %s %d\n",data->kodv.ravno(),data->naimv.ravno(),nomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  kartmatv_knopka(GtkWidget *widget,class kartmatv_data *data)
{
//char    bros[512];
iceb_u_str repl;
iceb_u_str vdata;
short d,m,g;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("kartmatv_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case FK2:
    if(data->tipz == 2)
      return;  

    if(vkartm(data->dd,data->md,data->gd,data->nomdok.ravno(),data->tipz,data->skl,
    data->kodm.ravno_atoi(),data->cenavdok,data->kolih_dok,data->window) != 0)
     return;
     
    data->metka_voz=0;
    gtk_widget_destroy(data->window);

    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    l_zkartmat(data->sklv.ravno(),data->n_kartv.ravno(),data->window);
    return;  

  case FK4: //ввод материалла без привязки к карточке
    data->metka_voz=2;
    gtk_widget_destroy(data->window);
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
    kartmatv_create_list(data);
    
    return;  

  case FK7:
    data->metka_poi++;
    if(data->metka_poi == 2)
     data->metka_poi=0;
    kartmatv_create_list(data);
     
    return;  

    
  case FK10:
//    printf("kartmatv_knopka F10\n");
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kartmatv_key_press(GtkWidget *widget,GdkEventKey *event,class kartmatv_data *data)
{
iceb_u_str repl;
//printf("kartmatv_key_press keyval=%d state=%d\n",event->keyval,event->state);

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
    printf("kartmatv_key_press-Нажата клавиша Shift\n");

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
void kartmatv_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class kartmatv_data *data)
{
//printf("kartmatv_v_row\n");
//printf("kartmatv_v_row корректировка\n");
if(kartmatv_zap(data) != 0)
 return;

data->metka_voz=0;

gtk_widget_destroy(data->window);


}
/*****************************/
/*Запись в карточку и документ*/
/*******************************/

int kartmatv_zap(class kartmatv_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;

printf("%s\n",__FUNCTION__);

//читаем реквизиты карточки
sprintf(strsql,"select * from Kart where sklad=%d and nomk=%d",
data->sklv.ravno_atoi(),
data->n_kartv.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найдена карточка"));
  repl.plus(" ");
  repl.plus(data->n_kartv.ravno());
  repl.plus(" ");
  repl.plus(gettext("Склад"));
  repl.plus(" ");
  repl.plus(data->sklv.ravno());
  iceb_menu_soob(&repl,data->window);
  return(1);
 }
double cenavkart=atof(row[6]);
iceb_u_str shetu;
shetu.plus(row[5]);
double ndsg=atof(row[9]);
iceb_u_str ei;
ei.plus(row[4]);
int mndsz=atoi(row[3]);
if(data->tipz == 1)
 {
  if(row[15][0] != '\0')
   {
    iceb_u_str repl;
    
    repl.plus(gettext("Карточка с инвентарным номером"));
    repl.ps_plus(gettext("Приход в карточку не возможен !"));
    iceb_menu_soob(&repl,data->window);
 
    return(1);  
   }

  if(data->kodm.ravno_atoi() != 0 && data->cenavdok != 0. && data->cenavdok != cenavkart)
   {
    iceb_u_str repl;
    
    repl.plus(gettext("Цена в карточке не равна цене в документе"));

    iceb_menu_soob(&repl,data->window);
    return(1);  
   }
 }

iceb_u_str kodop;
kodop.plus("");
iceb_u_str nomdokp;
nomdokp.plus("");
iceb_u_str kontr;
kontr.plus("");
iceb_u_str skl_dok;
skl_dok.plus("");

//читаем по документа
sprintf(strsql,"select kontr,kodop,nomon from Dokummat where datd='%d-%d-%d' and \
sklad=%d and nomd='%s' and tip=%d",
  data->dd,data->md,data->gd,data->sklv.ravno_atoi(),data->nomdok.ravno(),data->tipz);

if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  kontr.new_plus(row[0]);
  kodop.new_plus(row[1]);
  nomdokp.new_plus(row[2]);
 }

short vido=0;

if(data->tipz == 1 )
 {
  //читаем код операции по документа
  
  sprintf(strsql,"select vido from Prihod where kod='%s'",kodop.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
      vido=atoi(row[0]);
 }

if(data->tipz == 2 )
 {
  //читаем код операции по документа
  
    iceb_u_str sheta;
    sheta.plus("");
  
    //читаем операцию
    sprintf(strsql,"select vido,sheta from Rashod where kod='%s'",
    kodop.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      vido=atoi(row[0]);
      sheta.new_plus(row[1]);
     }      
  /*Проверяем соотвецтвие счета в выбранной карточке
  перечню счетов возможных при данном расходе*/
    if(iceb_u_proverka(sheta.ravno(),shetu.ravno(),0,0) != 0)
     {
      iceb_u_str repl;

      sprintf(strsql,"%s %s !",
      gettext("Этот материал числится на счете"),
      shetu.ravno());
      repl.plus(strsql);      

      sprintf(strsql,"%s %s !",
      gettext("Для данной операции можно:"),
      sheta.ravno());

      repl.ps_plus(strsql);      
      
      iceb_menu_soob(&repl,data->window);

      return(1);  
     }
 }

/*Проверяем есть ли такая запись*/

sprintf(strsql,"select datd from Dokummat1 where sklad=%d and \
nomd='%s' and kodm=%d and nomkar=%d and datd='%04d-%d-%d'",
data->sklv.ravno_atoi(),data->nomdok.ravno(),data->kodm.ravno_atoi(),
data->n_kartv.ravno_atoi(),data->gd,data->md,data->dd);
if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
 {
  if(nomdokp.getdlinna() > 1 && vido != 0)  
   {
    iceb_u_str repl;
    repl.plus(gettext("Такая запись в документе уже есть !"));
    iceb_menu_soob(&repl,data->window);
    return(1);  
   }
  iceb_u_str repl;
  repl.plus(gettext("Такая запись в документе уже есть !"));
  repl.ps_plus(gettext("Увеличить количество ?"));
    
  if(iceb_menu_danet(&repl,2,data->window) == 2)
    return(1);

  vmatkolp(data->sklv.ravno_atoi(),data->nomdok.ravno(),data->kodm.ravno_atoi(),
  data->n_kartv.ravno_atoi(),data->gd,data->md,data->dd,data->window);
  return(0);
 }


/*Если расход материалла или приход сторонние, то склад в выбранной
карточке должен быть такой как во входном меню
Если внутреннее перемещение:
расход - карточка должна быть с того склада что указано во входном
 меню. С нее будет проведен расход на склад который указан в коде
 организации принимающей расход через тире. Необходимо найти на этом
 складе карточку с такимиже реквизитами или завести новую.
приход - нужно найти карточку на складе с которого будет произведен расход
 (указан в коде организации через тире) найти на складе указанном
 во входном меню такую карточку или записать новую.
То есть во всех случаях кроме внутреннего перемещения-прихода
склад в выбранной карточке должен быть равен складу указанному во
входном меню.
*/
char bros[512];
if(data->tipz == 1 && kontr.ravno_atoi() != 0. && iceb_u_pole(kontr.ravno(),bros,2,'-') == 0)
 {
  if(data->skl == data->sklv.ravno_atoi())
   {
    iceb_u_str repl;

    sprintf(strsql,"%s %s !",gettext("Карточка должна быть не со склада"),data->sklv.ravno());
    
    iceb_menu_soob(strsql,data->window);
    return(1);  
   }
 }
else
 if(data->skl != data->sklv.ravno_atoi())
  {
   iceb_u_str repl;

   sprintf(strsql,"%s %s !",gettext("Эта карточка находиться на складе"),data->sklv.ravno());
   iceb_menu_soob(strsql,data->window);
   return(1);  
  }
//Читаем метку НДС документа
sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' \
and nomerz=11",data->gd,data->skl,data->nomdok.ravno());
int lnds=0;
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
  lnds=atoi(row[0]);
  
/*Документ с НДС а карточка без НДС*/
if(lnds == 0 && ndsg < 0.00001)
 {
  iceb_u_spisok repl;
  repl.plus(gettext("Документ c НДС, а карточка без НДС"));
  repl.plus(gettext("Все верно ? Вы уверены ?"));
  if(iceb_menu_danet(&repl,2,data->window) == 2)
   return(1);  
 }

if(lnds != 0 && ndsg > 0.0001)
 {
  iceb_u_spisok repl;

  repl.plus(gettext("Документ без НДС, а карточка с НДС"));
  repl.plus(gettext("Все верно ? Вы уверены ?"));

  if(iceb_menu_danet(&repl,2,data->window) == 2)
   return(1);  
 }


if(data->tipz == 1)
 {
  if(data->metka_prk == 1) //привязка к карточке
   {
    sprintf(strsql,"update Dokummat1 \
set \
nomkar=%d \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and \
nomkar=0 and tipz=1",data->n_kartv.ravno_atoi(),data->gd,data->md,data->dd,data->skl,data->nomdok.ravno(),data->kodm.ravno_atoi());
    if(sql_zap(&bd,strsql) != 0)
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

    return(0);
   }   

  if(prihodw(data->dd,data->md,data->gd,data->skl,data->nomdok.ravno(),
  data->kodm.ravno_atoi(),data->n_kartv.ravno_atoi(),cenavkart,ei.ravno(),
  mndsz,ndsg,data->nomz.ravno(),data->window) == 0)
   return(1);  
 }

if(data->tipz == 2)
 {

  if(data->metka_prk == 1) //привязка к карточке
   {
    sprintf(strsql,"update Dokummat1 \
set \
nomkar=%d \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and \
nomkar=0 and tipz=2",data->n_kartv.ravno_atoi(),data->gd,data->md,data->dd,data->skl,data->nomdok.ravno(),data->kodm.ravno_atoi());
    if(sql_zap(&bd,strsql) != 0)
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

    return(0);
   }   


  if(rashodw(data->dd,data->md,data->gd,data->skl,data->nomdok.ravno(),data->n_kartv.ravno_atoi(),
  data->kodm.ravno_atoi(),data->kolih_dok,data->pnds,data->window) != 0)
   return(1);  
 }

return(0);  
}
/*******************************/
/* Увеличение количества       */
/*******************************/

void	vmatkolp(int skl,const char *nomdok,int kodm,int nk,short gd,short md,short dd,GtkWidget *wpredok)
{
char		strsql[512];
iceb_u_str repl;

repl.plus(gettext("Введите количество"));
iceb_u_str kolih;

if(iceb_menu_vvod1(&repl,&kolih,20,wpredok) != 0)
   return;

sprintf(strsql,"update Dokummat1 set kolih=kolih+%f where sklad=%d and \
nomd='%s' and kodm=%d and nomkar=%d and datd='%04d-%d-%d'",
kolih.ravno_atof(),skl,nomdok,kodm,nk,gd,md,dd);

if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

}
