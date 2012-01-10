/*$Id: l_zar_dok_zap.c,v 1.21 2011-02-21 07:35:55 sasa Exp $*/
/*10.03.2009	27.10.2006	Белых А.И.	l_zar_dok_zap.c
Работа со списком записей в документе подсистеме "Заработная плата"
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
#include  "buhg_g.h"
#include  "l_zar_dok_zap.h"

enum
{
  COL_TABNOM,
  COL_FIO,
  COL_KOD_NAH,
  COL_SHET,
  COL_SUMA,
  COL_DNI,
  COL_VMES,
  COL_NAIM_KN,
  COL_KOMENT,
  COL_NOMZ,
  COL_PODR,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

enum
{
  SFK1,
  FK2,
  SFK2,
  FK3,
  SFK3,
  FK4,
  FK5,
  FK10,
  SFK10,
  KOL_F_KL
};

class  l_zar_dok_zap_data
 {
  public:

  l_zar_dok_zap_rek poisk;

  class iceb_u_str datad;
  class iceb_u_str nomdok;
  int prn;  
      
  class iceb_u_str tabnom_tv;  /*только что введённая запись*/
  class iceb_u_str kod_nah_tv;
  class iceb_u_str shet_tv;
  class iceb_u_str nomz_tv;
  class iceb_u_str vmes_tv;
      
  class iceb_u_str tabnom;
  class iceb_u_str kod_nah;
  class iceb_u_str shet;
  class iceb_u_str nomz;
  class iceb_u_str vmes;
  class iceb_u_str podr;
  
  class iceb_u_str zapros;
    
  GtkWidget *label_hap;
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  
  short     metka_rr;   //0-работа со списком 1-выбор

  //Конструктор
  l_zar_dok_zap_data()
   {
    
    snanomer=0;
    metka_rr=metka_voz=kl_shift=0;
    window=treeview=NULL;
    tabnom_tv.plus("");
    kod_nah_tv.plus("");
   }      
 };

gboolean   l_zar_dok_zap_key_press(GtkWidget *widget,GdkEventKey *event,class l_zar_dok_zap_data *data);
void l_zar_dok_zap_vibor(GtkTreeSelection *selection,class l_zar_dok_zap_data *data);
void l_zar_dok_zap_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_zar_dok_zap_data *data);
void  l_zar_dok_zap_knopka(GtkWidget *widget,class l_zar_dok_zap_data *data);
void l_zar_dok_zap_add_columns (GtkTreeView *treeview);
int l_zar_dok_zap_udzap(class l_zar_dok_zap_data *data);
int l_zar_dok_zap_ud(class l_zar_dok_zap_data *data);
int  l_zar_dok_zap_prov_row(SQL_str row,class l_zar_dok_zap_data *data);
void l_zar_dok_zap_rasp(class l_zar_dok_zap_data *data);
void l_zar_dok_zap_create_list (class l_zar_dok_zap_data *data);

int l_zar_dok_zap_v(const char *datad,const char *nomdok,int prn,class iceb_u_str *tabnom,class iceb_u_str *kod_nah,int podr,class iceb_u_str *shet,class iceb_u_str *nomz,class iceb_u_str *vmes,GtkWidget *wpredok);
int l_zar_dok_zap_p(int,class l_zar_dok_zap_rek *rek_poi,GtkWidget *wpredok);
void imp_zardokw(int prn,const char *datadok,const char *nomdok,GtkWidget *wpredok);


extern char *organ;
extern SQL_baza  bd;
extern char      *name_system;

int l_zar_dok_zap(short prn,const char *datad,const char *nomdok,GtkWidget *wpredok)
{
l_zar_dok_zap_data data;
char strsql[512];
GdkColor color;
data.poisk.clear_data();
data.nomdok.new_plus(nomdok);
data.datad.new_plus(datad);
data.prn=prn;

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);

if(data.prn == 1)
  sprintf(strsql,"%s %s",name_system,gettext("Список начислений"));
if(data.prn == 2)
  sprintf(strsql,"%s %s",name_system,gettext("Список удержаний"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_zar_dok_zap_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

sprintf(strsql,"%s:%s %s:%s",
gettext("Дата"),datad,gettext("Номер документа"),nomdok);

data.label_hap=gtk_label_new (iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (vbox2),data.label_hap,FALSE, FALSE, 0);

data.label_kolstr=gtk_label_new (gettext("Список начислений"));
//gdk_color_parse("green",&color);
//gtk_widget_modify_fg(data.label_kolstr,GTK_STATE_NORMAL,&color);


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show_all(vbox1);
gtk_widget_show_all(vbox2);

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

sprintf(strsql,"%sF1 %s",RFK,gettext("Шапка"));
data.knopka[SFK1]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK1],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK1]), "clicked",GTK_SIGNAL_FUNC(l_zar_dok_zap_knopka),&data);
tooltips[SFK1]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK1],data.knopka[SFK1],gettext("Работа с шапкой документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK1]),(gpointer)SFK1);
gtk_widget_show(data.knopka[SFK1]);

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(l_zar_dok_zap_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(l_zar_dok_zap_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);


sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(l_zar_dok_zap_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(strsql,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(l_zar_dok_zap_knopka),&data);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Удалить документ"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(strsql,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(l_zar_dok_zap_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(l_zar_dok_zap_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(l_zar_dok_zap_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);

sprintf(strsql,"%sF10 %s",RFK,gettext("Импорт"));
data.knopka[SFK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK10],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK10]), "clicked",GTK_SIGNAL_FUNC(l_zar_dok_zap_knopka),&data);
tooltips[SFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK10],data.knopka[SFK10],gettext("Импорт записей в документ из файла"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK10]),(gpointer)SFK10);
gtk_widget_show(data.knopka[SFK10]);

gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_zar_dok_zap_create_list(&data);

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
void l_zar_dok_zap_create_list (class l_zar_dok_zap_data *data)
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

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_zar_dok_zap_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_zar_dok_zap_vibor),data);

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
G_TYPE_INT);


sprintf(strsql,"select tabn,knah,suma,shet,nomz,podr,kom,ktoz,vrem,godn,mesn,kdn from Zarp  where datz = '%s' \
and nd='%s' and prn='%d' order by tabn asc",data->datad.ravno_sqldata(),data->nomdok.ravno(),data->prn);

data->zapros.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
double itogo=0.;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  if(l_zar_dok_zap_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->tabnom_tv.ravno(),row[0],0) == 0)
    data->snanomer=data->kolzap;

  //Табельный номер
  ss[COL_TABNOM].new_plus(iceb_u_toutf(row[0]));
  
  //читаем фамилию
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
    ss[COL_FIO].new_plus(iceb_u_toutf(row1[0]));
  else 
    ss[COL_FIO].new_plus("");
  
  ss[COL_KOD_NAH].new_plus(row[1]);

  sprintf(strsql,"%10.2f",atof(row[2]));
  ss[COL_SUMA].new_plus(strsql);
  itogo+=atof(row[2]);
  
  ss[COL_SHET].new_plus(iceb_u_toutf(row[3]));
  
  //Читаем код начисления
  if(data->prn == 1)
    sprintf(strsql,"select naik from Nash where kod=%s",row[1]);
  if(data->prn == 2)
    sprintf(strsql,"select naik from Uder where kod=%s",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   ss[COL_NAIM_KN].new_plus(iceb_u_toutf(row1[0]));      
  else
   ss[COL_NAIM_KN].new_plus("");

  //Номер записи
  ss[COL_NOMZ].new_plus(row[4]);   
  //Коментарий
  ss[COL_KOMENT].new_plus(iceb_u_toutf(row[6]));  
  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[8])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[7],0,data->window));

  /*В счёт какого месяца*/
  ss[COL_VMES].new_plus(row[10]);
  ss[COL_VMES].plus(".");
  ss[COL_VMES].plus(row[9]);
  /*Подразделение*/
  ss[COL_PODR].new_plus(row[5]);
    
  /*количество дней*/
  ss[COL_DNI].new_plus(row[11]);    
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_TABNOM,ss[COL_TABNOM].ravno(),
  COL_FIO,ss[COL_FIO].ravno(),
  COL_SHET,ss[COL_SHET].ravno(),
  COL_SUMA,ss[COL_SUMA].ravno(),
  COL_KOD_NAH,ss[COL_KOD_NAH].ravno(),
  COL_NOMZ,ss[COL_NOMZ].ravno(),
  COL_KOMENT,ss[COL_KOMENT].ravno(),
  COL_NAIM_KN,ss[COL_NAIM_KN].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  COL_VMES,ss[COL_VMES].ravno(),
  COL_PODR,ss[COL_PODR].ravno(),
  COL_DNI,ss[COL_DNI].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->tabnom_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_zar_dok_zap_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

sprintf(strsql,"%s:%s %s:%s",
gettext("Дата"),data->datad.ravno(),gettext("Номер документа"),data->nomdok.ravno());
gtk_label_set_text(GTK_LABEL(data->label_hap),iceb_u_toutf(strsql));

iceb_u_str stroka;
iceb_u_str zagolov;
if(data->prn == 1)
 zagolov.plus(gettext("Список начислений"));
if(data->prn == 2)
 zagolov.plus(gettext("Список удержаний"));

sprintf(strsql," %s:%d %s:%.2f",
gettext("Количество записей"),data->kolzap,
gettext("Сумма"),itogo);

zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

if(data->poisk.metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.tabnom.ravno(),gettext("Табельный номер"));
  iceb_str_poisk(&zagolov,data->poisk.fio.ravno(),gettext("Фамилия"));
  iceb_str_poisk(&zagolov,data->poisk.shet.ravno(),gettext("Счёт"));
  if(data->prn == 1)
    iceb_str_poisk(&zagolov,data->poisk.kod_nah.ravno(),gettext("Код начисления"));
  if(data->prn == 2)
    iceb_str_poisk(&zagolov,data->poisk.kod_nah.ravno(),gettext("Код удержания"));
  iceb_str_poisk(&zagolov,data->poisk.koment.ravno(),gettext("Коментарий"));
  

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

void l_zar_dok_zap_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Таб.N"), renderer,"text", COL_TABNOM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Фамилия Имя Отчество"), renderer,"text", COL_FIO,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Сумма"), renderer,"text", COL_SUMA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дни"), renderer,"text", COL_DNI,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Месяц"), renderer,"text", COL_VMES,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Счёт"), renderer,"text", COL_SHET,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Код"), renderer,"text", COL_KOD_NAH,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM_KN,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Коментарий"), renderer,"text", COL_KOMENT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,"N", renderer,"text", COL_NOMZ,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

}

/****************************/
/*Выбор строки*/
/**********************/

void l_zar_dok_zap_vibor(GtkTreeSelection *selection,class l_zar_dok_zap_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *tabnom;
gchar *kod_nah;
gchar *shet;
gchar *nomz;
gint  nomer;
gchar *vmes;
gchar *podr;

gtk_tree_model_get(model,&iter,
COL_TABNOM,&tabnom,
COL_KOD_NAH,&kod_nah,
COL_SHET,&shet,
COL_NOMZ,&nomz,
COL_VMES,&vmes,
COL_PODR,&podr,
NUM_COLUMNS,&nomer,-1);

data->tabnom.new_plus(iceb_u_fromutf(tabnom));
data->kod_nah.new_plus(iceb_u_fromutf(kod_nah));
data->shet.new_plus(iceb_u_fromutf(shet));
data->nomz.new_plus(nomz);
data->snanomer=nomer;
data->vmes.new_plus(vmes);
data->podr.new_plus(podr);

g_free(tabnom);
g_free(kod_nah);
g_free(shet);
g_free(nomz);
g_free(vmes);
g_free(podr);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_zar_dok_zap_knopka(GtkWidget *widget,class l_zar_dok_zap_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_zar_dok_zap_knopka knop=%d\n",knop);
data->kl_shift=0;
short d,m,g;
switch (knop)
 {
  case SFK1: /*Работа с шапкой документа*/

    if(l_zar_dok_hap(&data->datad,&data->nomdok,data->prn,data->window) == 0)
       l_zar_dok_zap_create_list(data);

    return;
    
  case FK2:
    data->tabnom_tv.new_plus("");
    data->shet_tv.new_plus("");
    data->nomz_tv.new_plus("");
    data->kod_nah_tv.new_plus("");
    data->vmes_tv.new_plus("");
    data->podr.new_plus("");
    
    if(l_zar_dok_zap_v(data->datad.ravno(),data->nomdok.ravno(),data->prn,
    &data->tabnom_tv,&data->kod_nah_tv,data->podr.ravno_atoi(),&data->shet_tv,&data->nomz_tv,&data->vmes_tv,data->window) == 0)
       l_zar_dok_zap_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;

    if(l_zar_dok_zap_v(data->datad.ravno(),data->nomdok.ravno(),data->prn,
    &data->tabnom,&data->kod_nah,data->podr.ravno_atoi(),&data->shet,&data->nomz,&data->vmes,data->window) == 0)
       l_zar_dok_zap_create_list(data);

    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    l_zar_dok_zap_udzap(data);
    l_zar_dok_zap_create_list(data);
    return;  
  
  case SFK3:

    if(iceb_menu_danet(gettext("Удалить документ ? Вы уверены ?"),2,data->window) != 1)
     return;
    if(l_zar_dok_zap_ud(data) == 0)
     {
      data->metka_voz=1;
      gtk_widget_destroy(data->window);
      return;
     }

    l_zar_dok_zap_create_list(data);
    return;  


  case FK4:
    l_zar_dok_zap_p(data->prn,&data->poisk,data->window);

    l_zar_dok_zap_create_list(data);

    return;  

  case FK5:
    l_zar_dok_zap_rasp(data);
    return;  

    
  case FK10:
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;

  case SFK10:
    iceb_u_rsdat(&d,&m,&g,data->datad.ravno(),1);
    
    if(l_nahud_prov_blok(m,g,0,data->window) != 0)
       return;;
    imp_zardokw(data->prn,data->datad.ravno(),data->nomdok.ravno(),data->window);
    l_zar_dok_zap_create_list(data);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zar_dok_zap_key_press(GtkWidget *widget,GdkEventKey *event,class l_zar_dok_zap_data *data)
{
iceb_u_str repl;

switch(event->keyval)
 {

  case GDK_F1:

    if(data->kl_shift == 0)
      ;
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK1]),"clicked");
    return(TRUE);

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

  
  case GDK_Escape:
  case GDK_F10:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK10]),"clicked");
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
void l_zar_dok_zap_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_zar_dok_zap_data *data)
{
//printf("l_zar_dok_zap_v_row\n");
//printf("l_zar_dok_zap_v_row корректировка\n");
if(data->metka_rr == 0)
 {
  gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
  return;
 }

data->metka_voz=0;

gtk_widget_destroy(data->window);


}

/****************************/
/*Проверка записей          */
/*****************************/

int  l_zar_dok_zap_prov_row(SQL_str row,class l_zar_dok_zap_data *data)
{
if(data->poisk.metka_poi == 0)
 return(0);


//Полное сравнение
if(iceb_u_proverka(data->poisk.tabnom.ravno(),row[0],0,0) != 0)
 return(1);

//Полное сравнение
if(iceb_u_proverka(data->poisk.kod_nah.ravno(),row[1],0,0) != 0)
 return(1);

//Полное сравнение
if(iceb_u_proverka(data->poisk.shet.ravno(),row[3],0,0) != 0)
 return(1);

if(data->poisk.fio.getdlinna() > 1)
 {
  SQL_str row1;
  class SQLCURSOR cur1;
  char strsql[512];
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   if(iceb_u_strstrm(row1[0],data->poisk.fio.ravno()) == 0)
    return(1);
 }
 
//Поиск образца в строке
if(data->poisk.koment.getdlinna() > 1 && iceb_u_strstrm(row[6],data->poisk.koment.ravno()) == 0)
 return(1);


   
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_zar_dok_zap_rasp(class l_zar_dok_zap_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);
char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;


if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return;
 }

sprintf(strsql,"zar_dokz%d.lst",getpid());

imaf.plus(strsql);
if(data->prn == 1)
  naimot.plus(gettext("Список начислений"));
if(data->prn == 2)
  naimot.plus(gettext("Список удержаний"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

fprintf(ff,"%s:%s\n%s:%s\n",
gettext("Дата документа"),data->datad.ravno(),
gettext("Номер документа"),data->nomdok.ravno());

iceb_u_zagolov(naimot.ravno(0),0,0,0,0,0,0,organ,ff);

if(data->poisk.metka_poi == 1)
 {
  if(data->poisk.tabnom.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Табельный номер"),data->poisk.tabnom.ravno());
  if(data->poisk.fio.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Фамилия"),data->poisk.fio.ravno());
  if(data->poisk.shet.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Счёт"),data->poisk.shet.ravno());

  if(data->prn == 1)
   if(data->poisk.kod_nah.getdlinna() > 1)
    fprintf(ff,"%s:%s\n",gettext("Код начисления"),data->poisk.kod_nah.ravno());
  if(data->prn == 2)
   if(data->poisk.kod_nah.getdlinna() > 1)
    fprintf(ff,"%s:%s\n",gettext("Код удержания"),data->poisk.kod_nah.ravno());

  if(data->poisk.koment.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Коментарий"),data->poisk.koment.ravno());
 }

 
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
 Т/н  |Фамилия Имя Отчество|  Сумма   |Код| Месяц |Дни|Наименование|Коментарий|Дата и время запис.| Кто записал\n"));
/*
123456 12345678901234567890 1234567890 123 1234567 123456789012 1234567890
*/
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------\n");

iceb_u_str s5;
iceb_u_str s6;
char fio[512];
char naim_kod[50];
SQL_str row1;
class SQLCURSOR cur1;
double itogo=0.;
int it_dn=0;
float kolstr1=0.;
class iceb_u_int kod_nah;
class iceb_u_double kod_nah_sum;
class iceb_u_int kod_nah_dni;
int nomer_nah=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(l_zar_dok_zap_prov_row(row,data) != 0)
    continue;
  memset(fio,'\0',sizeof(fio));
  memset(naim_kod,'\0',sizeof(naim_kod));
  
  //читаем фамилию
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   strncpy(fio,row1[0],sizeof(fio)-1);    

  if((nomer_nah=kod_nah.find(atoi(row[1]))) < 0)
    kod_nah.plus(row[1]);
  kod_nah_sum.plus(atof(row[2]),nomer_nah);
  kod_nah_dni.plus(atoi(row[11]),nomer_nah);
  
  if(data->prn == 1)
    sprintf(strsql,"select naik from Nash where kod=%s",row[1]);
  if(data->prn == 2)
    sprintf(strsql,"select naik from Uder where kod=%s",row[1]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   strncpy(naim_kod,row1[0],sizeof(naim_kod)-1);  

  //Дата и время записи
  s5.new_plus(iceb_u_vremzap(row[8]));

  //Кто записал
  s6.new_plus(iceb_kszap(row[7],1,data->window));

//sprintf(strsql,"select tabn,knah,suma,shet,nomz,podr,kom,ktoz,vrem from Zarp  where datz = '%s' 

  fprintf(ff,"%-6s %-*.*s %10.2f %-3s %02d.%04d %3s %-*.*s %-*.*s %s %s\n",
  row[0],
  iceb_u_kolbait(20,fio),iceb_u_kolbait(20,fio),fio,
  atof(row[2]),
  row[1],
  atoi(row[10]),atoi(row[9]),
  row[11],
  iceb_u_kolbait(12,naim_kod),iceb_u_kolbait(12,naim_kod),naim_kod,
  iceb_u_kolbait(10,row[6]),iceb_u_kolbait(10,row[6]),row[6],
  s5.ravno(),s6.ravno());

  if(iceb_u_strlen(fio) > 20)
   fprintf(ff,"%6s %s\n","",iceb_u_adrsimv(20,fio));

  itogo+=atof(row[2]);
  it_dn+=atoi(row[11]);
 }
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s %10.2f             %3d\n",iceb_u_kolbait(27,gettext("Итого")),gettext("Итого"),itogo,it_dn);

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);

fprintf(ff,"%s\n",gettext("Свод"));
fprintf(ff,"\
----------------------------------------\n");
fprintf(ff,"\
Код|    Наименование    |  Сумма   |Дни|\n");
fprintf(ff,"\
----------------------------------------\n");
for(int ii=0; ii < kod_nah.kolih(); ii++)
 {
  memset(naim_kod,'\0',sizeof(naim_kod));

  if(data->prn == 1)
    sprintf(strsql,"select naik from Nash where kod=%d",kod_nah.ravno(ii));
  if(data->prn == 2)
    sprintf(strsql,"select naik from Uder where kod=%d",kod_nah.ravno(ii));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   strncpy(naim_kod,row1[0],sizeof(naim_kod)-1);
  
  fprintf(ff,"%3d %-*.*s %10.2f %3d\n",
  kod_nah.ravno(ii),
  iceb_u_kolbait(20,naim_kod),iceb_u_kolbait(20,naim_kod),naim_kod,
  kod_nah_sum.ravno(ii),kod_nah_dni.ravno(ii));
 }
fprintf(ff,"\
----------------------------------------\n");
fprintf(ff,"%*.*s:%10.2f %3d\n",
iceb_u_kolbait(24,gettext("Итого")),
iceb_u_kolbait(24,gettext("Итого")),
gettext("Итого"),itogo,it_dn);


iceb_podpis(ff,data->window);
fclose(ff);

gdite.close();
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,"",0,data->window);

}
/*****************************/
/*Удаление записи            */
/*****************************/

int l_zar_dok_zap_udzap(class l_zar_dok_zap_data *data)
{
short dd,md,gd;
iceb_u_rsdat(&dd,&md,&gd,data->datad.ravno(),1);
short godz=gd;
short mesz=md;
iceb_u_rsdat1(&mesz,&godz,data->vmes.ravno());

class ZARP     zp;
zp.tabnom=data->tabnom.ravno_atoi();
zp.prn=data->prn;
zp.knu=data->kod_nah.ravno_atoi();
zp.dz=dd;
zp.mz=md;
zp.gz=gd;
zp.mesn=mesz; zp.godn=godz;
zp.nomz=data->nomz.ravno_atoi();
zp.podr=data->podr.ravno_atoi();
strncpy(zp.shet,data->shet.ravno(),sizeof(zp.shet)-1);

if(zarudnuw(&zp,data->window) != 0)
 return(1);

zarsocw(md,gd,data->tabnom.ravno_atoi(),NULL,data->window);

zaravprw(data->tabnom.ravno_atoi(),md,gd,NULL,data->window);
return(0);

}
/*************************/
/*Удалить документ*/
/**********************/
int l_zar_dok_zap_ud(class l_zar_dok_zap_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);


SQL_str row;
class SQLCURSOR cur;
int kolstr;
if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return(1);
 }

int voz=0;
float kolstr1=0.;
while(cur.read_cursor(&row) != NULL)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
//  strzag(LINES-1,0,kolstr,++kolstr1);
//  if(dirzarkz_uz(dd,md,gd,row[0],1,row[1],row[5],row[3],atoi(row[4])) != 0)
  if(l_zar_dok_zap_udzap(data) != 0)
   {
    if(kolstr1 == 1)
     {
      return(1);
     }    
    else
      voz++;
   }
 }

if(voz != 0) 
 return(voz); 

//Удаляем шапку документа
char strsql[512];
sprintf(strsql,"delete from Zardok where god=%d and nomd='%s'",data->datad.ravno_god(),data->nomdok.ravno());
return(iceb_sql_zapis(strsql,1,0,data->window));
}
