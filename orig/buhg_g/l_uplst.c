/*$Id: l_uplst.c,v 1.12 2011-02-21 07:35:54 sasa Exp $*/
/*12.11.2009	17.04.2008	Белых А.И.	l_uplst.c
Работа со списком объектов на которые списано топливо
*/
#include <math.h>
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
#include  "buhg_g.h"

enum
{
  COL_KOD_OB,
  COL_NAIM,
  COL_SHET,
  COL_KOD_TOP,
  COL_PR_PO_GOR,
  COL_ZT_PO_GOR,
  COL_HASI,
  COL_PR_ZA_GOR,
  COL_ZT_ZA_GOR,
  COL_ZT_NA_GRUZ,
  COL_ZT_NA_VREM,
  COL_TONO_KIL,
  COL_VREM_DVIG,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

enum
{
  FK2,
  SFK2,
  FK3,
  FK10,
  KOL_F_KL
};

class  l_uplst_data
 {
  public:

  short dd,md,gd;
  class iceb_u_str nomdok;
  int kod_pod;
  int kod_avt;
  int kod_vod;

  class iceb_u_str kod_topv;
  class iceb_u_str shetv;
  class iceb_u_str kod_obv;

  class iceb_u_str kod_top_tv;
  class iceb_u_str shet_tv;
  class iceb_u_str kod_ob_tv;
  
  class iceb_u_spisok KST; //Коды списанного в путевом листе топлива
  class iceb_u_double KOL;  //Количество списанного топлива
    
  GtkWidget *label_kolstr;
  GtkWidget *label_itog;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  
  //Конструктор
  l_uplst_data()
   {
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
    kod_top_tv.plus("");
    shet_tv.plus("");
    kod_ob_tv.plus("");
   }      
 };

gboolean   l_uplst_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplst_data *data);
void l_uplst_vibor(GtkTreeSelection *selection,class l_uplst_data *data);
void l_uplst_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_uplst_data *data);
void  l_uplst_knopka(GtkWidget *widget,class l_uplst_data *data);
void l_uplst_add_columns (GtkTreeView *treeview);
void l_uplst_udzap(class l_uplst_data *data);
void l_uplst_create_list (class l_uplst_data *data);

int l_uplst_v(short dd,short md,short gd,const char *nomdok,int kod_pod,int kod_vod,int kod_avt,const char *kod_top,const char *shet_sp,int kod_ob,GtkWidget *wpredok);


extern char *organ;
extern SQL_baza  bd;
extern char      *name_system;

void l_uplst(const char *data_dok,const char *nomdok,int kod_pod,GtkWidget *wpredok)
{
class l_uplst_data data;
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str naim_podr("");
class iceb_u_str naim_vod("");
class iceb_u_str naim_avt("");

iceb_u_rsdat(&data.dd,&data.md,&data.gd,data_dok,1);
data.nomdok.new_plus(nomdok);

sprintf(strsql,"select ka,kv,ztfa,prob,ztfazg,probzg,ztvsf,ztvrf from Upldok where god=%d and kp=%d and nomd='%s'",
data.gd,kod_pod,nomdok);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  iceb_menu_soob(gettext("Не нашли шапку документа!"),wpredok);
  return;
 }
data.kod_pod=kod_pod;
data.kod_vod=atoi(row[1]);
data.kod_avt=atoi(row[0]);

double ztf_po_gor=atof(row[2]);
int prob_po_gor=atoi(row[3]);

double ztf_za_gor=atof(row[4]);
int prob_za_gor=atoi(row[5]);
double ztf_na_gruz=atof(row[6]);
double ztf_na_vrem=atof(row[7]);

spst(data.dd,data.md,data.gd,nomdok,kod_pod,&data.KST,&data.KOL,wpredok);
class iceb_u_str top_ost;
for(int ii=0 ; ii < data.KST.kolih(); ii++)
 {
  sprintf(strsql,"\n%s:%.3f",data.KST.ravno(ii),data.KOL.ravno(ii));
  top_ost.plus(strsql);
 }

sprintf(strsql,"select naik from Uplpodr where kod=%d",data.kod_pod);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_podr.new_plus(row[0]);

sprintf(strsql,"select naik from Uplouot where kod=%d",data.kod_vod);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_vod.new_plus(row[0]);


sprintf(strsql,"select nomz,naik from Uplavt where kod=%d",data.kod_avt);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  naim_avt.new_plus(row[0]);
  naim_avt.new_plus(" ");
  naim_avt.new_plus(row[1]);
 }



data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);


sprintf(strsql,"%s %s",name_system,gettext("Список объектов на которые списано топливо"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_uplst_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
GtkWidget *hbox_hap = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox_hap = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE,1);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Список объектов на которые списано топливо"));
//gdk_color_parse("green",&color);
//gtk_widget_modify_fg(data.label_kolstr,GTK_STATE_NORMAL,&color);


gtk_box_pack_start (GTK_BOX (vbox2),hbox_hap,FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox_hap),vbox_hap,FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (vbox_hap),data.label_kolstr,FALSE, FALSE,1);

sprintf(strsql,"%s:%s %s:%d.%d.%d\n\
%s:%d %s\n\
%s:%d %s\n\
%s:%d %s",
gettext("Номер документа"),
data.nomdok.ravno(),
gettext("Дата"),
data.dd,data.md,data.gd,
gettext("Подразделение"),
data.kod_pod,naim_podr.ravno(),
gettext("Водитель"),
data.kod_vod,
naim_vod.ravno(),
gettext("Автомобиль"),
data.kod_avt,
naim_avt.ravno());

GtkWidget *label_hap=gtk_label_new (iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (vbox_hap),label_hap,FALSE, FALSE,1);


sprintf(strsql,"\
%s - %s:%d %s:%.10g\n\
%s - %s:%d %s:%.10g\n\
%s:%.10g %s:%.10g",
gettext("Списано по городу"),
gettext("Пробег"),
prob_po_gor,
gettext("Топливо"),
ztf_po_gor,
gettext("Списано за городом"),
gettext("Пробег"),
prob_za_gor,
gettext("Топливо"),
ztf_za_gor,
gettext("Расход на груз"),
ztf_na_gruz,
gettext("Расход на отработанное время"),
ztf_na_vrem);


GtkWidget *frame1=gtk_frame_new(gettext("Списано в путевом листе"));
gtk_box_pack_start (GTK_BOX (vbox_hap),frame1,FALSE, FALSE,1);

label_hap=gtk_label_new (iceb_u_toutf(strsql));
gtk_container_add(GTK_CONTAINER(frame1),label_hap);

GtkWidget *frame2=gtk_frame_new(gettext("Списано по объектам и счетам"));
gtk_box_pack_start (GTK_BOX (vbox_hap),frame2,FALSE, FALSE,1);


data.label_itog=gtk_label_new("");
gtk_container_add(GTK_CONTAINER(frame2),data.label_itog);



GtkWidget *frame3=gtk_frame_new(gettext("Коды топлива списанные в путевом листе"));
gtk_box_pack_end(GTK_BOX (hbox_hap),frame3,FALSE, FALSE,1);
if(top_ost.getdlinna() > 1)
  label_hap=gtk_label_new(top_ost.ravno_toutf());
else
  label_hap=gtk_label_new("");

gtk_container_add(GTK_CONTAINER(frame3),label_hap);
//gtk_box_pack_end(GTK_BOX (hbox_hap),label_hap,FALSE, FALSE,1);


gtk_widget_show(vbox1);
gtk_widget_show_all(vbox2);


data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE,1);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE,1);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(l_uplst_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(l_uplst_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);


sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(l_uplst_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE,1);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(l_uplst_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_uplst_create_list(&data);

gtk_widget_show(data.window);
gtk_window_maximize(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_uplst_create_list (class l_uplst_data *data)
{
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[1024];
int  kolstr=0;
SQL_str row;
//GdkColor color;

data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_uplst_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_uplst_vibor),data);

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
G_TYPE_STRING, 
G_TYPE_INT);


sprintf(strsql,"select * from Upldok2 where datd='%d-%d-%d' and kp=%d and nomd='%s' order by ko asc",
data->gd,data->md,data->dd,data->kod_pod,data->nomdok.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
SQL_str row1;
class SQLCURSOR cur1;
int pb_po_gor=0;
double zt_po_gor=0.;
int pb_za_gor=0;
double zt_za_gor=0.;
double otr_vrem=0.;
double zt_na_gruz=0.;
double zt_na_vrem=0.;
double tono_kil=0.;
double vrem_dvig=0.;

while(cur.read_cursor(&row) != 0)
 {
/*  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);*/
  


  if(iceb_u_SRAV(data->kod_ob_tv.ravno(),row[5],0) == 0 && \
     iceb_u_SRAV(data->shet_tv.ravno(),row[4],0) == 0 && \
     iceb_u_SRAV(data->kod_top_tv.ravno(),row[3],0) == 0)
        data->snanomer=data->kolzap;

  //Код
  ss[COL_KOD_OB].new_plus(iceb_u_toutf(row[5]));
  
  //Наименование
  sprintf(strsql,"select naik from Uplost where kod=%s",row[5]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
    ss[COL_NAIM].new_plus(iceb_u_toutf(row1[0]));
  else
    ss[COL_NAIM].new_plus("");
  
  //Счёт
  ss[COL_SHET].new_plus(iceb_u_toutf(row[4]));

  //Код топлива
  ss[COL_KOD_TOP].new_plus(iceb_u_toutf(row[3]));

  //Пробег по городу
  ss[COL_PR_PO_GOR].new_plus(row[6]);
  pb_po_gor+=atoi(row[6]);
  
  //Затраты топлива по городу
  ss[COL_ZT_PO_GOR].new_plus(row[7]);
  zt_po_gor+=atof(row[7]);
  
  //отработанное время
  ss[COL_HASI].new_plus(row[8]);
  otr_vrem+=atof(row[8]);
  
  //ПРОбег за городом
  ss[COL_PR_ZA_GOR].new_plus(row[13]);
  pb_za_gor+=atoi(row[13]);
  
  //Затраты топлива за городом
  ss[COL_ZT_ZA_GOR].new_plus(row[14]);
  zt_za_gor+=atof(row[14]);
  
  //Затраты на перевозку груза
  ss[COL_ZT_NA_GRUZ].new_plus(row[15]);
  zt_na_gruz+=atof(row[15]);
  
  //Затраты на отработанное время
  ss[COL_ZT_NA_VREM].new_plus(row[16]);
  zt_na_vrem+=atof(row[16]);
  
  //Тонокилометры
  ss[COL_TONO_KIL].new_plus(row[17]);
  tono_kil+=atof(row[17]);
  
  //время работы двигателя
  ss[COL_VREM_DVIG].new_plus(row[18]);
  vrem_dvig+=atof(row[18]);

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[12])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[11],0,data->window));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD_OB,ss[COL_KOD_OB].ravno(),
  COL_NAIM,ss[COL_NAIM].ravno(),
  COL_SHET,ss[COL_SHET].ravno(),
  COL_KOD_TOP,ss[COL_KOD_TOP].ravno(),
  COL_PR_PO_GOR,ss[COL_PR_PO_GOR].ravno(),
  COL_ZT_PO_GOR,ss[COL_ZT_PO_GOR].ravno(),
  COL_HASI,ss[COL_HASI].ravno(),
  COL_PR_ZA_GOR,ss[COL_PR_ZA_GOR].ravno(),
  COL_ZT_ZA_GOR,ss[COL_ZT_ZA_GOR].ravno(),
  COL_ZT_NA_GRUZ,ss[COL_ZT_NA_GRUZ].ravno(),
  COL_ZT_NA_VREM,ss[COL_ZT_NA_VREM].ravno(),
  COL_TONO_KIL,ss[COL_TONO_KIL].ravno(),
  COL_VREM_DVIG,ss[COL_VREM_DVIG].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->kod_top_tv.new_plus("");
data->shet_tv.new_plus("");
data->kod_ob_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_uplst_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Список объектов на которые списано топливо"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());


gtk_widget_show(data->label_kolstr);

sprintf(strsql,"\
%s - %s:%d %s:%.10g\n\
%s - %s:%d %s:%.10g\n\
%s:%.10g %s:%.10g\n\
%s:%.10g %s:%.10g\n\
%s:%.10g",
gettext("Списано по городу"),
gettext("Пробег"),
pb_po_gor,
gettext("Топливо"),
zt_po_gor,
gettext("Списано за городом"),
gettext("Пробег"),
pb_za_gor,
gettext("Топливо"),
zt_za_gor,
gettext("Расход на груз"),
zt_na_gruz,
gettext("Расход на отработанное время"),
zt_na_vrem,
gettext("Отработанное время"),
otr_vrem,
gettext("Тоннокилометры"),
tono_kil,
gettext("Отработанное время двигателем"),
vrem_dvig);

gtk_label_set_text(GTK_LABEL(data->label_itog),iceb_u_toutf(strsql));

}

/*****************/
/*Создаем колонки*/
/*****************/

void l_uplst_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;


renderer = gtk_cell_renderer_text_new ();
//  g_object_set(G_OBJECT(renderer),"foreground","red",NULL);

//g_object_set(G_OBJECT(renderer),"background","black",NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_KOD_OB,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Счёт"), renderer,"text", COL_SHET,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код топлива"), renderer,"text", COL_KOD_TOP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Пробег по городу"), renderer,"text", COL_PR_PO_GOR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Расход по городу"), renderer,"text", COL_ZT_PO_GOR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Отработанное время"), renderer,"text", COL_HASI,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Пробег за городом"), renderer,"text", COL_PR_ZA_GOR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Расход за городом"), renderer,"text", COL_ZT_ZA_GOR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Расход за груз"), renderer,"text", COL_ZT_NA_GRUZ,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Расход за отработанное\nдвигателем время"), renderer,"text", COL_ZT_NA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Тоннокилометры"), renderer,"text", COL_TONO_KIL,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Время отработанное\nдвигателем"), renderer,"text", COL_VREM_DVIG,NULL);



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

void l_uplst_vibor(GtkTreeSelection *selection,class l_uplst_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod_top;
gchar *shet;
gchar *kod_ob;
gint  nomer;


gtk_tree_model_get(model,&iter,
COL_KOD_OB,&kod_ob,
COL_KOD_TOP,&kod_top,
COL_SHET,&shet,
COL_KOD_TOP,&kod_top,
NUM_COLUMNS,&nomer,-1);

data->kod_obv.new_plus(iceb_u_fromutf(kod_ob));
data->kod_topv.new_plus(iceb_u_fromutf(kod_top));
data->shetv.new_plus(iceb_u_fromutf(shet));
data->snanomer=nomer;


g_free(kod_top);
g_free(kod_ob);
g_free(shet);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_uplst_knopka(GtkWidget *widget,class l_uplst_data *data)
{
//char    bros[512];
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uplst_knopka knop=%d\n",knop);
data->kl_shift=0;
switch (knop)
 {
  case FK2:
    if(l_uplst_v(data->dd,data->md,data->gd,data->nomdok.ravno(),data->kod_pod,
    data->kod_vod,data->kod_avt,data->kod_top_tv.ravno(),data->shet_tv.ravno(),
    data->kod_ob_tv.ravno_atoi(),data->window) == 0)
       l_uplst_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    if(l_uplst_v(data->dd,data->md,data->gd,data->nomdok.ravno(),data->kod_pod,
    data->kod_vod,data->kod_avt,data->kod_topv.ravno(),data->shetv.ravno(),
    data->kod_obv.ravno_atoi(),data->window) == 0)
       l_uplst_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    l_uplst_udzap(data);
    l_uplst_create_list(data);
    return;  
  

  case FK10:
    provpstw(data->dd,data->md,data->gd,data->nomdok.ravno(),data->kod_pod,data->window);
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uplst_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplst_data *data)
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
   
  case GDK_F3:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
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
void l_uplst_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_uplst_data *data)
{

gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");


}

/*****************************/
/*Удаление записи            */
/*****************************/

void l_uplst_udzap(class l_uplst_data *data)
{


char strsql[512];

sprintf(strsql,"delete from Upldok2 where datd='%04d-%02d-%02d' and kp=%d and nomd='%s' and kt='%s' and \
shet='%s' and ko=%d",
data->gd,data->md,data->dd,
data->kod_pod,
data->nomdok.ravno(),
data->kod_topv.ravno_filtr(),
data->shetv.ravno_filtr(),
data->kod_obv.ravno_atoi());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/*****************************************************/
/*Создание списка списанного в путевом листе топлива*/
/*****************************************************/
void spst(short dd,short md,short gd, //Дата
const char *nomd, //Номер документа
int podr, //Подразделение
class iceb_u_spisok *KOT, //Коды топлива
class iceb_u_double *KOL, //Количество топлива
GtkWidget *wpredok)
{
char	strsql[512];
int	kolstr=0;
SQL_str row;
int nomer=0;

sprintf(strsql,"select kodtp,kolih from Upldok1 where datd='%d-%d-%d' and kp=%d and nomd='%s' \
and tz=2",gd,md,dd,podr,nomd);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  if((nomer=KOT->find(row[0])) < 0)
     KOT->plus(row[0]);
  KOL->plus(atof(row[1]),nomer);
  
 }


}
