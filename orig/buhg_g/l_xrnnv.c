/*$Id: l_xrnnv.c,v 1.16 2011-08-29 07:13:43 sasa Exp $*/
/*14.04.2010	07.05.2008	Белых А.И.	l_xrnnv.c
Реестр выданных налоговых накладных
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
#include  "buhg_g.h"
#include "xrnn_poiw.h"
enum
{
 COL_DATAD,
 COL_DNZ,
 COL_NOMDOK,
 COL_VIDDOK,
 COL_SUMADOK,
 COL_NDS,
 COL_PODSYS,
 COL_KOD_OP,
 COL_INN,
 COL_NAIM_KONTR,
 COL_KOMENT,
 COL_DATA_VREM,
 COL_KTO,  
 COL_NOMZAP,
 NUM_COLUMNS
};

enum
{
 FK2,
 FK3,
 SFK3,
 FK4,
 FK5,
 FK6,
 FK7,
 FK8,
 FK9,
 FK10,
 SFK2,
 KOL_F_KL
};

class  l_xrnnv_data
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
  
  class xrnn_poiw rkp;
  class iceb_u_str datap;
  
  class iceb_u_str datan;
  class iceb_u_str datak;
  
  int nom_v_zap; /*Номер выбранной записи*/
  int dnz; /*Дневной номер записи*/
  class iceb_u_str data_v_zap; /*Дата выбранной записи*/
  class iceb_u_str nnn_tv; /*номер налоговой накладной только что введённый*/
  
  class iceb_u_str zapros;    
  //Конструктор
  l_xrnnv_data()
   {
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    datan.new_plus("");
    datak.new_plus("");
    nom_v_zap=0;
    data_v_zap.plus("");
    nnn_tv.plus("");
    datap.plus("");
    dnz=0;
   }      

 };

gboolean   l_xrnnv_key_press(GtkWidget *widget,GdkEventKey *event,class l_xrnnv_data *data);
void l_xrnnv_vibor(GtkTreeSelection *selection,class l_xrnnv_data *data);
void l_xrnnv_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_xrnnv_data *data);
void  l_xrnnv_knopka(GtkWidget *widget,class l_xrnnv_data *data);
void l_xrnnv_add_columns (GtkTreeView *treeview);
int l_xrnnv_udzap(class l_xrnnv_data *data);
int  l_xrnnv_prov_row(SQL_str row,class l_xrnnv_data *data);
void l_xrnnv_rasp(class l_xrnnv_data *data);
void l_xrnnv_create_list (class l_xrnnv_data *data);
int l_xrnnv_pz(class  l_xrnnv_data *data);

int l_xrnnv_v(class iceb_u_str *nom_nal_nak,int nom_zap,GtkWidget *wpredok);
int l_xrnnv_p(class xrnn_poiw *rek_poi,GtkWidget *wpredok);
int l_xrnn_udgrz(class l_xrnnv_data *data);
void l_xrnnv_nn_xml(int nom_zap,GtkWidget *wpredok);


extern char *organ;
extern SQL_baza  bd;
extern char      *name_system;
extern class iceb_rnfxml_data rek_zag_nn;
extern iceb_u_str kat_for_nal_nak; //Каталог для налоговых накладных

void l_xrnnv(short dnp,short mnp,short gnp,GtkWidget *wpredok)
{
l_xrnnv_data data;
char bros[512];
GdkColor color;
data.rkp.metka_poi=0;
data.rkp.metka_sort=0;
if(dnp != 0)
 {
  sprintf(bros,"%d.%d.%d",dnp,mnp,gnp);
  data.datan.new_plus(bros);
  data.datap.new_plus(bros);
  data.rkp.datan.new_plus(bros);
 }
 
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Реестр выданных налоговых накладных"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_xrnnv_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE,1);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Реестр выданных налоговых накладных"));
//gdk_color_parse("green",&color);
//gtk_widget_modify_fg(data.label_kolstr,GTK_STATE_NORMAL,&color);


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE,1);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");
gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_poisk,GTK_STATE_NORMAL,&color);

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE,1);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE,1);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE,1);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(l_xrnnv_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(l_xrnnv_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(l_xrnnv_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(l_xrnnv_knopka),&data);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Удалить все записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(l_xrnnv_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE,1);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(l_xrnnv_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);


sprintf(bros,"F6 %s",gettext("Импорт"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(l_xrnnv_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Импорт документов"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F7 %s",gettext("Нал.нак."));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(l_xrnnv_knopka),&data);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Получение налоговой накладной в формате xml"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"F8 %s",gettext("Переставить"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]), "clicked",GTK_SIGNAL_FUNC(l_xrnnv_knopka),&data);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Перестaвить запись выше/ниже в дневном списке"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_widget_show(data.knopka[FK8]);


sprintf(bros,"F9 %s",gettext("Сортировать"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK9]), "clicked",GTK_SIGNAL_FUNC(l_xrnnv_knopka),&data);
tooltips[FK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK9],data.knopka[FK9],gettext("Включть/выключить сортировку документов по дневным номерам записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK9]),(gpointer)FK9);
gtk_widget_show(data.knopka[FK9]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE,1);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(l_xrnnv_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_xrnnv_create_list(&data);

gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_xrnnv_create_list (class l_xrnnv_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

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

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_xrnnv_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_xrnnv_vibor),data);

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
G_TYPE_INT,
G_TYPE_INT);

class iceb_u_str sort("");

if(data->rkp.metka_sort == 0)
 sort.new_plus(" order by datd asc,nnni asc");
else
 sort.new_plus(" order by datd asc,dnz asc");

sprintf(strsql,"select * from Reenn where datd >= '%s'",data->datan.ravno_sqldata());


if(data->datan.getdlinna() > 1 && data->datak.getdlinna() > 1)
  sprintf(strsql,"select * from Reenn where datd >= '%s' and datd <= '%s'",data->datan.ravno_sqldata(),data->datak.ravno_sqldata());

strcat(strsql,sort.ravno());

data->zapros.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
double sumad=0.;
double sumands=0.;
double isumad=0.;
double isumands=0.;
float kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  if(l_xrnnv_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->nnn_tv.ravno(),row[1],0) == 0)
    data->snanomer=data->kolzap;

  //Дата документа
  ss[COL_DATAD].new_plus(iceb_u_datzap(row[1]));
  
  //Номер документа
  ss[COL_NOMDOK].new_plus(iceb_u_toutf(row[2]));

  sumad=atof(row[3]);
  sumands=atof(row[4]);
  isumad+=sumad;
  isumands+=sumands;
  
  //Сумма по документа
  ss[COL_SUMADOK].new_plus(sumad);
  //Сумма НДС
  ss[COL_NDS].new_plus(sumands);
  
  //Коментарий
  ss[COL_KOMENT].new_plus(iceb_u_toutf(row[11]));
  
  
  //ПОДСИСТЕМА
  ss[COL_PODSYS].new_plus("");
  if(atoi(row[8]) == 1)
    ss[COL_PODSYS].new_plus(gettext("МУ"));
  if(atoi(row[8]) == 2)
    ss[COL_PODSYS].new_plus(gettext("УСЛ"));
  if(atoi(row[8]) == 3)
    ss[COL_PODSYS].new_plus(gettext("УОС"));
  if(atoi(row[8]) == 4)
    ss[COL_PODSYS].new_plus(gettext("УКР"));
  if(atoi(row[8]) == 5)
    ss[COL_PODSYS].new_plus(gettext("ГК"));

  //код операции
  ss[COL_KOD_OP].new_plus(iceb_u_toutf(row[15]));
  //индивидуальний налоговый номер
  ss[COL_INN].new_plus(iceb_u_toutf(row[6]));
  //Наименование контрагента
  ss[COL_NAIM_KONTR].new_plus(iceb_u_toutf(row[7]));
  

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[13])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[12],0,data->window));

  /*Вид документа*/
  ss[COL_VIDDOK].new_plus(iceb_u_toutf(row[16]));

  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_DATAD,ss[COL_DATAD].ravno(),
  COL_NOMDOK,ss[COL_NOMDOK].ravno(),
  COL_VIDDOK,ss[COL_VIDDOK].ravno(),
  COL_SUMADOK,ss[COL_SUMADOK].ravno(),
  COL_NDS,ss[COL_NDS].ravno(),
  COL_PODSYS,ss[COL_PODSYS].ravno(),
  COL_KOD_OP,ss[COL_KOD_OP].ravno(),
  COL_INN,ss[COL_INN].ravno(),
  COL_KOMENT,ss[COL_KOMENT].ravno(),
  COL_NAIM_KONTR,ss[COL_NAIM_KONTR].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  COL_DNZ,row[18],
  COL_NOMZAP,atoi(row[0]),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

data->nnn_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_xrnnv_add_columns (GTK_TREE_VIEW (data->treeview));


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
zagolov.plus(gettext("Реестр выданных налоговых накладных"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);
if(data->rkp.metka_sort == 0)
  zagolov.ps_plus(gettext("Сортировка по номерам накладных"));
else
  zagolov.ps_plus(gettext("Сортировка по дневной нумерации записей"));
  
//sprintf(strsql,"\n%.2f+%.2f=%.2f",isumad-isumands,isumands,isumad);

sprintf(strsql,"\n%s:%s %s:%.2f+%.2f=%.2f",
gettext("Дата начала"),
data->datan.ravno(),
gettext("Сумма по документам"),
isumad-isumands,isumands,isumad);


zagolov.plus(strsql);
gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

if(data->rkp.metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->rkp.datan.ravno(),gettext("Дата начала"));
  iceb_str_poisk(&zagolov,data->rkp.datak.ravno(),gettext("Дата конца"));
  iceb_str_poisk(&zagolov,data->rkp.nnn.ravno(),gettext("Номер налоговой накладной"));
  iceb_str_poisk(&zagolov,data->rkp.inn.ravno(),gettext("Інд.под.номер"));
  iceb_str_poisk(&zagolov,data->rkp.naim_kontr.ravno(),gettext("Наименование контрагента"));
  iceb_str_poisk(&zagolov,data->rkp.nomdok.ravno(),gettext("Номер документа"));
  iceb_str_poisk(&zagolov,data->rkp.koment.ravno(),gettext("Коментарий"));
  iceb_str_poisk(&zagolov,data->rkp.kodgr.ravno(),gettext("Код группы"));
  iceb_str_poisk(&zagolov,data->rkp.podsistema.ravno(),gettext("Подсистема"));
  iceb_str_poisk(&zagolov,data->rkp.kodop.ravno(),gettext("Код операции"));
  iceb_str_poisk(&zagolov,data->rkp.datavnn.ravno(),gettext("Дата выписки налоговой накладной"));
  iceb_str_poisk(&zagolov,data->rkp.viddok.ravno(),gettext("Вид документа"));
  

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

void l_xrnnv_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата"), renderer,"text", COL_DATAD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Н/з"), renderer,"text", COL_DNZ,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Ном.док."), renderer,"text", COL_NOMDOK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Вид д-а"), renderer,"text", COL_VIDDOK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Сумма с НДС"), renderer,"text", COL_SUMADOK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Сумма НДС"), renderer,"text", COL_NDS,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Подсистема"), renderer,"text", COL_PODSYS,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код операции"), renderer,"text", COL_KOD_OP,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Инд.н.н."), renderer,"text", COL_INN,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование контрагента"), renderer,"text", COL_NAIM_KONTR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Коментарий"), renderer,"text", COL_KOMENT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,"text", COL_KTO,NULL);

}

/****************************/
/*Выбор строки*/
/**********************/

void l_xrnnv_vibor(GtkTreeSelection *selection,class l_xrnnv_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;
gchar *datazap;
gchar *dnz;
gint nom_v_zap;
gint nomer;


gtk_tree_model_get(model,&iter,COL_DATAD,&datazap,COL_DNZ,&dnz,COL_NOMZAP,&nom_v_zap,NUM_COLUMNS,&nomer,-1);

data->nom_v_zap=nom_v_zap;
data->data_v_zap.new_plus(datazap);
data->snanomer=nomer;
data->dnz=atoi(dnz);

g_free(datazap);
g_free(dnz);
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_xrnnv_knopka(GtkWidget *widget,class l_xrnnv_data *data)
{
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_xrnnv_knopka knop=%d\n",knop);
data->kl_shift=0;
switch (knop)
 {
  case FK2:
    if(l_xrnnv_v(&data->nnn_tv,0,data->window) == 0)
      l_xrnnv_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    if(l_xrnnv_v(&data->nnn_tv,data->nom_v_zap,data->window) == 0)
      l_xrnnv_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(l_xrnnv_udzap(data) == 0)
      l_xrnnv_create_list(data);
    return;  
  
  case SFK3:
    if(l_xrnn_udgrz(data) == 0)
      l_xrnnv_create_list(data);
    return;     

  case FK4:
    if(l_xrnnv_p(&data->rkp,data->window) == 0)
     {
      data->datan.new_plus(data->rkp.datan.ravno());
      data->datak.new_plus(data->rkp.datak.ravno());   
     }
    else
     {
      data->datan.new_plus(data->datap.ravno());
      data->datak.new_plus("");   
    
     }
    l_xrnnv_create_list(data);
    return;  

  case FK5:
    l_xrnnv_rasp(data);
    return;  

  case FK6:
    if(rnn_imp(2,data->window) == 0)
      l_xrnnv_create_list(data);
    return;  

  case FK7:
    l_xrnnv_nn_xml(data->nom_v_zap,data->window);
    return;  
  
  case FK8:
    if(l_xrnnv_pz(data) == 0)
      l_xrnnv_create_list(data);

    return;  

  case FK9:
    data->rkp.metka_sort++;

    if(data->rkp.metka_sort >= 2)
     data->rkp.metka_sort=0;

    l_xrnnv_create_list(data);

    return;  

  case FK10:
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_xrnnv_key_press(GtkWidget *widget,GdkEventKey *event,class l_xrnnv_data *data)
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

  case GDK_F6:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
    return(TRUE);

  case GDK_F7:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK7]),"clicked");
    return(TRUE);

  case GDK_F8:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK8]),"clicked");
    return(TRUE);

  case GDK_F9:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK9]),"clicked");
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
void l_xrnnv_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_xrnnv_data *data)
{

gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");



}

/*****************************/
/*Удаление записи            */
/*****************************/

int l_xrnnv_udzap(class l_xrnnv_data *data)
{
if(iceb_pbpds(data->data_v_zap.ravno(),data->window) != 0)
 return(1);

if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
 return(1);

char strsql[512];

sprintf(strsql,"delete from Reenn where nz=%d",data->nom_v_zap);

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


return(0);
}
/****************************/
/*Проверка записей          */
/*****************************/

int  l_xrnnv_prov_row(SQL_str row,class l_xrnnv_data *data)
{
if(data->rkp.metka_poi == 0)
 return(0);
//Полное сравнение
if(iceb_u_proverka(data->rkp.nnn.ravno(),row[2],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->rkp.inn.ravno(),row[6],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->rkp.nomdok.ravno(),row[10],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->rkp.viddok.ravno(),row[16],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->rkp.kodgr.ravno(),row[14],0,0) != 0)
 return(1);

if(data->rkp.podsistema.getdlinna() > 1)
 {
  char metka_pod[20];
  memset(metka_pod,'\0',sizeof(metka_pod));

  if(row[8][0] == '1')
   strcpy(metka_pod,gettext("МУ"));
  if(row[8][0] == '2')
   strcpy(metka_pod,gettext("УСЛ"));
  if(row[8][0] == '3')
   strcpy(metka_pod,gettext("УОС"));
  if(row[8][0] == '4')
   strcpy(metka_pod,gettext("УКР"));
  if(iceb_u_proverka(data->rkp.podsistema.ravno(),metka_pod,0,0) != 0)
   return(1); 
 }





if(iceb_u_proverka(data->rkp.kodop.ravno(),row[20],0,0) != 0)
 return(1);

//Поиск образца в строке
if(data->rkp.naim_kontr.ravno()[0] != '\0')
 if(iceb_u_strstrm(row[12],data->rkp.naim_kontr.ravno()) == 0)
   return(1);
//Поиск образца в строке
if(data->rkp.koment.ravno()[0] != '\0')
 if(iceb_u_strstrm(row[12],data->rkp.koment.ravno()) == 0)
  return(1);

if(data->rkp.datavnn.getdlinna() > 1)
 {
  short d,m,g;
  iceb_u_rsdat(&d,&m,&g,row[21],2);
  short d1,m1,g1;
  iceb_u_rsdat(&d1,&m1,&g1,data->rkp.datavnn.ravno(),1);
  if(iceb_u_sravmydat(d,m,g,d1,m1,g1) != 0)
   return(1);
 }   
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_xrnnv_rasp(class l_xrnnv_data *data)
{
data->rkp.pr_ras=2;
if(xrnn_rasp_r(&data->rkp,data->window) != 0)
 return;

iceb_rabfil(&data->rkp.imaf,&data->rkp.naim,"",0,data->window);

}
/*********************/
/*удаление нескольких записей*/
/*****************************/
int l_xrnn_udgrz(class l_xrnnv_data *data)
{



if(iceb_menu_danet(gettext("Удалить все записи? Вы уверены?"),2,data->window) != 1)
 return(1);



short dt,mt,gt;

iceb_u_poltekdat(&dt,&mt,&gt);


short dk=0;
short mk=0;
short gk=0;
iceb_u_rsdat(&dk,&mk,&gk,data->rkp.datak.ravno(),1);

if(iceb_parol(0,data->window) != 0)
 return(1);

class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

SQL_str row;
class SQLCURSOR cur;
float kolstr1=0;
char strsql[200];
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

short d=0;
short m=0;
short g=0;
short mz=0,gz=0;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(l_xrnnv_prov_row(row,data) != 0)
   continue;
  iceb_u_rsdat(&d,&m,&g,row[1],2);
  if(mz != m || gz != g)
   {
    if(iceb_pbpds(m,g,data->window) != 0)
     return(0);
    mz=m;
    gz=g;
   }
  sprintf(strsql,"delete from Reenn where nz=%s",row[0]);
  iceb_sql_zapis(strsql,1,0,data->window);

 } 

return(0);
}
/***********************************/
/*Перестановка записей*/
/*************************************/
int l_xrnnv_pz(class l_xrnnv_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
int dnz=0,dnz1=0;
short dz=0,mz=0,gz=0;
int unomz=0;
if(data->rkp.metka_sort == 0)
 {
  iceb_menu_soob(gettext("Включите сортировку по дневным номерам записей!"),data->window);
  return(1);
 }
class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


//zagolovok.plus(organ,40);
zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Переместить вверх"));//0
punkt_m.plus(gettext("Переместить вниз"));//1

int metka_op=0;
metka_op=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,data->window);

switch(metka_op)
 {
  case -1:
    return(1);
 }

sprintf(strsql,"select datd,dnz from Reenn where nz=%d",data->nom_v_zap);
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  iceb_menu_soob(gettext("Не найдена запись которую хотим переместить!"),data->window);
  return(1);
 }

dnz=atoi(row[1]);
iceb_u_rsdat(&dz,&mz,&gz,row[0],2);

if(metka_op == 0) /*вверх*/
 {
  if(dnz == 0)
   {
    iceb_menu_soob(gettext("Выше некуда!"),data->window);
    return(1);
   }  
  sprintf(strsql,"select nz,dnz from Reenn where datd='%04d-%02d-%02d' and dnz < %d order by dnz desc limit 1",gz,mz,dz,dnz);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    iceb_menu_soob(gettext("Выше нет записей!"),data->window);
    return(1);
   }
  unomz=atoi(row[0]);
  dnz1=atoi(row[1]);      
  /*меняем записи местами*/
  sprintf(strsql,"update Reenn set dnz=%d where nz=%d",dnz1,data->nom_v_zap);
  iceb_sql_zapis(strsql,1,0,data->window);
  sprintf(strsql,"update Reenn set dnz=%d where nz=%d",dnz,unomz);
  iceb_sql_zapis(strsql,1,0,data->window);

 }

if(metka_op == 1) /*вниз*/
 {
  sprintf(strsql,"select nz,dnz from Reenn where datd='%04d-%02d-%02d' and dnz > %d order by dnz asc limit 1",gz,mz,dz,dnz);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    iceb_menu_soob(gettext("Ниже нет записей!"),data->window);
    return(1);
   }
  unomz=atoi(row[0]);
  dnz1=atoi(row[1]);      

  /*меняем записи местами*/
  sprintf(strsql,"update Reenn set dnz=%d where nz=%d",dnz1,data->nom_v_zap);
  iceb_sql_zapis(strsql,1,0,data->window);
  sprintf(strsql,"update Reenn set dnz=%d where nz=%d",dnz,unomz);
  iceb_sql_zapis(strsql,1,0,data->window);

 }
return(0); 
}
/***********************************************/
/*Получение налоговой накладной в формате xml*/
/**********************************************/
void l_xrnnv_nn_xml(int nom_zap,GtkWidget *wpredok)
{
char strsql[512];
class SQLCURSOR cur;
SQL_str row;


sprintf(strsql,"select * from Reenn where nz=%d",nom_zap);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  iceb_menu_soob(gettext("Не нашли запись в реестре!"),wpredok);
  return;
 }
int metka_pods=atoi(row[8]);
if(metka_pods == 0)
 {
  iceb_menu_soob(gettext("Запись введена в реестр вручную!"),wpredok);
  return;
 }
if(metka_pods == 5)
 {
  iceb_menu_soob(gettext("Запись импортирована из подсистемы \"Главная книга\"!"),wpredok);
  return;
 }

int pnd=0;  /*порядковый номер документа*/
class iceb_u_str imaf_xml("");

iceb_rnfxml(&rek_zag_nn,wpredok); /*читаем данные для создания xml файлов*/

short dd,md,gd;
iceb_u_rsdat(&dd,&md,&gd,row[1],2);
kat_for_nal_nak.new_plus("."); //каталог для налоговых накладных

if(metka_pods == 1)
  xml_nn_matw(&pnd,2,dd,md,gd,row[10],row[2],md,gd,md,1,&imaf_xml,wpredok);
if(metka_pods == 2)
  xml_nn_uslw(&pnd,2,dd,md,gd,row[10],row[2],md,gd,md,1,&imaf_xml,wpredok);
if(metka_pods == 3)
  xml_nn_uosw(&pnd,2,dd,md,gd,row[10],row[2],md,gd,md,1,&imaf_xml,wpredok);

class iceb_u_spisok imaf;
class iceb_u_spisok naim;

imaf.plus(imaf_xml.ravno());
naim.plus(gettext("Налоговая накладная в xml формате"));

//iceb_t_rabfil(&oth,"");
iceb_rabfil(&imaf,&naim,"",0,wpredok);


}
