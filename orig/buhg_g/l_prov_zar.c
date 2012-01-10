/*$Id: l_prov_zar.c,v 1.20 2011-03-15 09:28:17 sasa Exp $*/
/*27.06.2010	07.09.2006	Белых А.И.	l_prov_zar.c
Работа с проводками для подсистемы "Зароботная плата"
*/
#include <errno.h>
#include <stdlib.h>
#include        "buhg_g.h"
#include <unistd.h>
enum
{
 FK2,
 SFK2,
 FK3,
 SFK3,
 FK4,
 SFK4,
 FK5,
 SFK5,
 FK6,
 FK8,
 FK10,
 KOL_F_KL
};

enum
{
 COL_DATA,
 COL_SHET,
 COL_SHET_KOR,
 COL_KONTR,
 COL_DEBET,
 COL_KREDIT,
 COL_KOMENT,
 COL_DATA_VREM,
 COL_KTO,  
 COL_KEKV,
//неотображаемые данные
 COL_VREM,
 COL_KTOZ,
 COL_VAL,
 NUM_COLUMNS
};

class prov_zar_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *radiobutton[3];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  int       voz;

  iceb_u_str zapros;
  char metkasys[24];
  
  //Реквизиты документа
  int tabnom;
  short dd,md,gd; //Дата документа
  int podr;  
  char nomdok[56];
  char imaf_prot[56];  

  class iceb_u_str fio;

  int metka_r;
  int metka_avp;
  
  //Выбранная проводка
  iceb_u_str datap_v;
  iceb_u_str shet_v;
  iceb_u_str shet_kor_v;
  iceb_u_str kontr_v;
  double     debet_v;
  double     kredit_v;
  iceb_u_str koment_v;
  time_t     vrem_v;
  int        ktoz_v;
  int        val_v;
  double     suma_deb;
  double     suma_kre;  
  int        kekv;
  float pnds; //Процент НДС на момент создания документа

  prov_zar_data()
   {
    kekv=0;
    dd=1;
    podr=0;
    fio.plus("");
    metka_r=0;
    treeview=NULL;
    snanomer=0;
    kolzap=0;
    voz=0;
    kl_shift=0;
    metka_avp=0;
   }

 };
gboolean   prov_zar_key_press(GtkWidget *widget,GdkEventKey *event,class prov_zar_data *data);
void prov_zar_vibor(GtkTreeSelection *selection,class prov_zar_data *data);
void prov_zar_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class prov_zar_data *data);
void  prov_zar_knopka(GtkWidget *widget,class prov_zar_data *data);
void prov_zar_add_columns (GtkTreeView *treeview);
void prov_zar_create_list (class prov_zar_data *data);

void       prov_zar_radio0(GtkWidget *,class prov_zar_data *);
void       prov_zar_radio1(GtkWidget *,class prov_zar_data *);
void       prov_zar_radio2(GtkWidget *,class prov_zar_data *);

void prov_zar_sapka_menu(class prov_zar_data *data);
int prov_zar_prpvblok(class prov_zar_data *data);


extern SQL_baza  bd;
extern char      *name_system;
extern iceb_u_str shrift_ravnohir;

void l_prov_zar(int tabnom,
short md,short gd,
GtkWidget *wpredok)
{
class prov_zar_data data;
char strsql[512];
SQL_str row;
SQLCURSOR cur;

data.tabnom=tabnom;
data.md=md;
data.gd=gd;
sprintf(data.nomdok,"%d-%d",md,tabnom);
sprintf(data.imaf_prot,"zarpr%d.lst",getpid());

iceb_u_dpm(&data.dd,&data.md,&data.gd,5); //Установить день на последний день месяца

strcpy(data.metkasys,gettext("ЗП"));

sprintf(strsql,"select fio from Kartb where tabn=%d",data.tabnom);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 data.fio.new_plus(row[0]);


data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,
gettext("Работа с проводками (расчёт зарплаты)"));

gtk_window_set_title (GTK_WINDOW (data.window),iceb_u_toutf(strsql));
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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(prov_zar_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);


gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show_all(hbox);

data.label_kolstr=gtk_label_new ("");

iceb_u_str stroka;

prov_zar_sapka_menu(&data);



gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);


data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

GtkWidget *hboxradio = gtk_hbox_new (TRUE, 0);

//Вставляем радиокнопки
GSList *group;

data.radiobutton[0]=gtk_radio_button_new_with_label(NULL,gettext("Дебет"));
//  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[0]),TRUE); //Устанавливем активной кнопку
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton[0]), "clicked",GTK_SIGNAL_FUNC(prov_zar_radio0),&data);
//gtk_object_set_user_data(GTK_OBJECT(data.radiobutton0),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton[0]));

sprintf(strsql,"%s",gettext("Кредит"));
data.radiobutton[1]=gtk_radio_button_new_with_label(group,strsql);
//  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[1]),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data.radiobutton[1]), "clicked",GTK_SIGNAL_FUNC(prov_zar_radio1),&data);
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[1], TRUE, TRUE, 0);

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton[1]));

sprintf(strsql,"%s",gettext("Все"));
data.radiobutton[2]=gtk_radio_button_new_with_label(group,strsql);
//  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[2]),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data.radiobutton[2]), "clicked",GTK_SIGNAL_FUNC(prov_zar_radio2),&data);
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[2], TRUE, TRUE, 0);

gtk_box_pack_start(GTK_BOX (vbox2), hboxradio, FALSE,FALSE, 0);
gtk_widget_show_all(hboxradio);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(prov_zar_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой проводки"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(strsql);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(prov_zar_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);

sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(prov_zar_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удалить выбранную запись"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(strsql,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(strsql);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(prov_zar_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Удалить все проводки"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);


sprintf(strsql,"F4 %s",gettext("Проводки"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(prov_zar_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Выполнить все нужные проводки"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(strsql,"SF4 %s",gettext("Протокол"));
data.knopka[SFK4]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK4],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK4]), "clicked",GTK_SIGNAL_FUNC(prov_zar_knopka),&data);
tooltips[SFK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK4],data.knopka[SFK4],gettext("Протокол хода автоматического выполнения проводок"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK4]),(gpointer)SFK4);
gtk_widget_show(data.knopka[SFK4]);

sprintf(strsql,"F5 %s",gettext("Настройка"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(prov_zar_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Настройка автоматического выполнения проводок"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"SF5 %s",gettext("Распечатать"));
data.knopka[SFK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK5]), "clicked",GTK_SIGNAL_FUNC(prov_zar_knopka),&data);
tooltips[SFK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK5],data.knopka[SFK5],gettext("Распечатать проводки"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK5]),(gpointer)SFK5);
gtk_widget_show(data.knopka[SFK5]);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(prov_zar_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);

gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

prov_zar_create_list(&data);

//gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));
gtk_widget_show(data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void prov_zar_create_list (class prov_zar_data *data)
{
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;
//GdkColor color;

//printf("prov_zar_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(prov_zar_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(prov_zar_vibor),data);

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
G_TYPE_LONG, 
G_TYPE_INT,  
G_TYPE_INT,
G_TYPE_INT);

iceb_u_str zapros;

if(data->metka_r == 2)
  sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre,komen,ktoi,vrem,val,kekv \
from Prov where kto='%s' and pod=%d and nomd='%s' and \
datd >= '%04d-%02d-1' and datd <= '%04d-%02d-%d' order by datp asc",
  data->metkasys,data->podr,data->nomdok,data->gd,data->md,data->gd,data->md,31);

if(data->metka_r == 0)
  sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre,komen,ktoi,vrem,val,kekv \
from Prov where kto='%s' and pod=%d and nomd='%s' and \
datd >= '%04d-%02d-1' and datd <= '%04d-%02d-%d' and deb <> 0. order by datp asc",
  data->metkasys,data->podr,data->nomdok,data->gd,data->md,data->gd,data->md,31);

if(data->metka_r == 1)
  sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre,komen,ktoi,vrem,val,kekv \
from Prov where kto='%s' and pod=%d and nomd='%s' and \
datd >= '%04d-%02d-1' and datd <= '%04d-%02d-%d' and kre <> 0. order by datp asc",
  data->metkasys,data->podr,data->nomdok,data->gd,data->md,data->gd,data->md,31);

data->zapros.new_plus(strsql);


if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
data->suma_deb=0.;
data->suma_kre=0.;

while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  //Дата
  ss[COL_DATA].new_plus(iceb_u_sqldata(row[0]));
  
  //Счёт
  ss[COL_SHET].new_plus(iceb_u_toutf(row[1]));

  //Счёт корреспондент
  ss[COL_SHET_KOR].new_plus(iceb_u_toutf(row[2]));

  //Контрагент
  ss[COL_KONTR].new_plus(iceb_u_toutf(row[3]));

  //Дебет
  ss[COL_DEBET].new_plus(iceb_u_toutf(row[4]));
  data->suma_deb+=atof(row[4]);
  //Кредит
  ss[COL_KREDIT].new_plus(iceb_u_toutf(row[5]));
  data->suma_kre+=atof(row[5]);
  
  //Коментарий
  ss[COL_KOMENT].new_plus(iceb_u_toutf(row[6]));

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[8])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[7],0,data->window));
  
  if(atoi(row[10]) != 0)
    ss[COL_KEKV].new_plus(row[10]);
  else  
    ss[COL_KEKV].new_plus("");

  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_DATA,ss[COL_DATA].ravno(),
  COL_KEKV,ss[COL_KEKV].ravno(),
  COL_SHET,ss[COL_SHET].ravno(),
  COL_SHET_KOR,ss[COL_SHET_KOR].ravno(),
  COL_KONTR,ss[COL_KONTR].ravno(),
  COL_DEBET,ss[COL_DEBET].ravno(),
  COL_KREDIT,ss[COL_KREDIT].ravno(),
  COL_KOMENT,ss[COL_KOMENT].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  COL_VREM,atol(row[8]),
  COL_KTOZ,atoi(row[7]),
  COL_VAL,atoi(row[9]),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

prov_zar_add_columns (GTK_TREE_VIEW (data->treeview));

if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
 }
if(data->metka_avp == 0)
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK4]),FALSE);//Недоступна
else 
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK4]),TRUE);//Доступна

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

prov_zar_sapka_menu(data);

gtk_widget_show(data->label_kolstr);


}

/*****************/
/*Создаем колонки*/
/*****************/

void prov_zar_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

//printf("prov_zar_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата"), renderer,"text", COL_DATA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Счёт"), renderer,"text",COL_SHET,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Счёт кор."), renderer,"text", COL_SHET_KOR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кон-нт"), renderer,"text", COL_KONTR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дебет"), renderer,"text", COL_DEBET,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кредит"), renderer,"text", COL_KREDIT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("КЭКР"), renderer,"text", COL_KEKV,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Коментарий"), renderer,"text", COL_KOMENT,NULL);

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

void prov_zar_vibor(GtkTreeSelection *selection,class prov_zar_data *data)
{
//printf("prov_zar_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *shet_d;
gchar *shet_k;
gchar *datap;
gchar *debet;
gchar *kredit;
gchar *koment;
gchar *kontr;
gint ktoz;
glong vrem;
gint val;
gint nomer;
gchar *kekv;

gtk_tree_model_get(model,&iter,
COL_SHET,&shet_d,
COL_SHET_KOR,&shet_k,
COL_DATA,&datap,
COL_DEBET,&debet,
COL_KREDIT,&kredit,
COL_KONTR,&kontr,
COL_KOMENT,&koment,
COL_KTOZ,&ktoz,
COL_VREM,&vrem,
COL_VAL,&val,
COL_KEKV,&kekv,
NUM_COLUMNS,&nomer,-1);


data->shet_v.new_plus(iceb_u_fromutf(shet_d));
data->shet_kor_v.new_plus(iceb_u_fromutf(shet_k));
data->kontr_v.new_plus(iceb_u_fromutf(kontr));
data->datap_v.new_plus(iceb_u_fromutf(datap));
data->koment_v.new_plus(iceb_u_fromutf(koment));
data->debet_v=atof(debet);
data->kredit_v=atof(kredit);
data->ktoz_v=ktoz;
data->vrem_v=vrem;
data->val_v=val;
data->snanomer=nomer;
data->kekv=atoi(kekv);

g_free(shet_d);
g_free(shet_k);
g_free(datap);
g_free(debet);
g_free(kredit);
g_free(kontr);
g_free(koment);
g_free(kekv);
/*
printf("%s %s %s %s %.2f %.2f %s %d\n",
data->datap_v.ravno(),
data->shet_v.ravno(),
data->shet_kor_v.ravno(),
data->kontr_v.ravno(),
data->debet_v,
data->kredit_v,
data->koment_v.ravno(),
data->snanomer);
*/
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  prov_zar_knopka(GtkWidget *widget,class prov_zar_data *data)
{
iceb_u_str shetd;
iceb_u_str shetk;
iceb_u_str suma;
FILE *ff_prot=NULL;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {

  case GDK_plus: //это почемуто не работает Нажата клавиша плюс на основной клавиатуре
  case GDK_KP_Add: //Нажата клавиша "+" на дополнительной клавиатуре
  case FK2:
    if(prov_zar_prpvblok(data) != 0)
     return;
    if(iceb_vprov(data->metkasys,"","","","","",data->nomdok,"",data->podr,
    data->dd,data->md,data->gd,"",data->vrem_v,data->ktoz_v,"",
    2,0,0,0,data->window) != 0)
         return;  
    
    prov_zar_create_list(data);

    return;  

  case SFK2:
    if(data->kolzap == 0)
     return;
    if(prov_zar_prpvblok(data) != 0)
     return;
    shetd.new_plus(data->shet_v.ravno());
    shetk.new_plus(data->shet_kor_v.ravno());
    suma.new_plus(data->debet_v);
    if(data->debet_v == 0)
     {
      shetd.new_plus(data->shet_kor_v.ravno());
      shetk.new_plus(data->shet_v.ravno());
      suma.new_plus(data->kredit_v);
     }
    
    if(iceb_vprov(data->metkasys,shetd.ravno(),shetk.ravno(),
    suma.ravno(),data->datap_v.ravno(),data->koment_v.ravno(),
    data->nomdok,"",data->podr,
    data->dd,data->md,data->gd,"",data->vrem_v,data->ktoz_v,"",
    2,0,data->val_v,data->kekv,data->window) != 0)
         return;  
    
    prov_zar_create_list(data);

    return;  

  case FK3:
    if(data->kolzap == 0)
     return;
    if(prov_zar_prpvblok(data) != 0)
     return;
    shetd.new_plus(gettext("Удалить запись ? Вы уверены ?"));
    if(iceb_menu_danet(&shetd,2,data->window) != 1)
     return;

    iceb_udprov(data->val_v,data->datap_v.ravno(),data->shet_v.ravno_filtr(),
    data->shet_kor_v.ravno_filtr(),data->vrem_v,
    data->debet_v,data->kredit_v,data->koment_v.ravno_filtr(),2,data->window);

    prov_zar_create_list(data);
    
    return;  

  case SFK3:
    if(data->kolzap == 0)
     return;
  
    if(prov_zar_prpvblok(data) != 0)
     return;

    shetd.new_plus(gettext("Удалить все проводки ? Вы уверены ?"));
    if(iceb_menu_danet(&shetd,2,data->window) != 1)
     return;

    if(iceb_udprgr(data->metkasys,data->dd,data->md,data->gd,data->nomdok,data->podr,0,data->window) != 0)
     return;

    prov_zar_create_list(data);
    return;  
    
  
  case FK4:
    if(prov_zar_prpvblok(data) != 0)
     return;
    if((ff_prot = fopen(data->imaf_prot,"w")) == NULL)
     {
      iceb_er_op_fil(data->imaf_prot,"",errno,data->window);
      ff_prot=NULL;
     }

    zaravprw(data->tabnom,data->md,data->gd,ff_prot,data->window);

    if(ff_prot != NULL)
     {
      data->metka_avp=1;
      fclose(ff_prot);
     }
    prov_zar_create_list(data);
    return;  

  case SFK4:
    iceb_prosf(data->imaf_prot,data->window);
    return;  
  
  case FK5:
    zarnvpw(data->window);
    return;  

  case SFK5:
    iceb_raspprov(data->zapros.ravno(),data->dd,data->md,data->gd,data->nomdok,2,data->window);
    return;  
  

  case FK6:
    return;  
  
  

    
  case FK10:
    unlink(data->imaf_prot);
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   prov_zar_key_press(GtkWidget *widget,GdkEventKey *event,class prov_zar_data *data)
{
iceb_u_str repl;
printf("prov_zar_key_press keyval=%d state=%d\n",
event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_F2:
    if(data->kl_shift == 1) //Нажата
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);

  case GDK_F3:
    if(data->kl_shift == 1) //Нажата
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK3]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    return(TRUE);

  case GDK_F4:
    if(data->kl_shift == 1) //Нажата
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK4]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
  
    return(TRUE);

  case GDK_F5:
    if(data->kl_shift == 1) //Нажата
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK5]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
    
    return(TRUE);

  case GDK_F6:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
    return(TRUE);

//  case GDK_F9:
//    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK9]),"clicked");
//    return(TRUE);

  case GDK_F8:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK8]),"clicked");
    return(TRUE);

  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("prov_zar_key_press-Нажата клавиша Shift\n");

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
void prov_zar_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class prov_zar_data *data)
{
printf("prov_zar_v_row\n");
//data->metkazapisi=1;
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
}

/***************/
/*Шапка меню*/
/***************/

void prov_zar_sapka_menu(class prov_zar_data *data)
{
iceb_u_str stroka;

stroka.new_plus(gettext("Работа с проводками (расчёт зарплаты)"));

stroka.ps_plus(data->tabnom);

stroka.plus(" ");
stroka.plus(data->fio.ravno());

stroka.ps_plus(gettext("Дата"));
stroka.plus(":");
stroka.plus(data->md);
stroka.plus(".");
stroka.plus(data->gd);



char strsql[512];
sprintf(strsql," %s:%d",gettext("Количество записей"),
data->kolzap);
stroka.plus(strsql);



stroka.ps_plus(gettext("Сумма выполненных проводок"));
stroka.plus(":");
stroka.plus(data->suma_deb);
stroka.plus("/");
stroka.plus(data->suma_kre);


gtk_label_set_text(GTK_LABEL(data->label_kolstr),stroka.ravno_toutf());

}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  prov_zar_radio0(GtkWidget *widget,class prov_zar_data *data)
{
//g_print("prov_zar_radio0\n");

if(data->metka_r == 0)
  return;
data->metka_r=0;
prov_zar_create_list(data);

}
void  prov_zar_radio1(GtkWidget *widget,class prov_zar_data *data)
{
//g_print("prov_zar_radio1\n");
if(data->metka_r == 1)
  return;
data->metka_r=1;
prov_zar_create_list(data);
}
void  prov_zar_radio2(GtkWidget *widget,class prov_zar_data *data)
{
//g_print("prov_zar_radio1\n");
if(data->metka_r == 2)
  return;
data->metka_r=2;
prov_zar_create_list(data);
}
/**************************/
/*Проверка блокировки*/
/************************/

int prov_zar_prpvblok(class prov_zar_data *data)
{
//printf("prov_zar_prpvblok\n");
if(iceb_pbpds(data->md,data->gd,data->window) != 0)
  return(1);

if(data->kolzap == 0)
 return(0);


short d1,m1,g1;
iceb_u_rsdat(&d1,&m1,&g1,data->datap_v.ravno(),1);
if(iceb_pvglkni(m1,g1,data->window) != 0)
 return(1);

return(0);
}
