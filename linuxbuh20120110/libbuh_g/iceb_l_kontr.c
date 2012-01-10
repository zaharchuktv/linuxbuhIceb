/*$Id: iceb_l_kontr.c,v 1.68 2011-02-21 07:36:07 sasa Exp $*/
/*10.10.2010	24.11.2003	Белых А.И.	iceb_l_kontr.c
Работа с общим списком контрагентов
Если вернули 0- выбрали контрагента
             1- нет
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
//#include        <pwd.h>
#include        "iceb_libbuh.h"
#include        "iceb_l_kontr.h"
enum
{
  COL_KONTR,
  COL_NAIMKONTR,
  COL_TELEF,
  COL_ADRES,
  COL_SHET,
  COL_MFO,
  COL_BANK,
  COL_ADRESB,
  COL_KOD,
  COL_NALNOM,
  COL_NSPDV,
  COL_GRUPA,
  COL_DOG,
  COL_RNHP,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

enum
{
  FK2,
  FK3,
  FK4,
  FK5,
  FK6,
  FK7,
  FK8,
  FK9,
  FK10,
  SFK2,
  SFK3,
  KOL_F_KL
};

class kontr_udnz_data
 {
  public:
  SQLCURSOR cur;
  FILE *ff;
  GtkWidget *window;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  gfloat kolstr1; //Количество прочитанных строк
  int    kolstr;  //Количество строк в курсоре
  int koludk;


  //Конструктор
  kontr_udnz_data()
   {
    koludk=0;
    kolstr1=0.;
    ff=NULL;
   }
 };


class kontr_data
 {
  public:
  

  class kontr_rek poisk;
  short metka_sort; //0-по кодам контрагента 1-по наименованию
    
  //Выбранный код контрагента
  class iceb_u_str shetsk;  //Счет списка контрагентов
  class iceb_u_str kodv; //выбранный код контрагента
  class iceb_u_str kontr_v; //только что введённый код контрагента
  class iceb_u_str naim;
      
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или 0
  int       kolzap;     //Количество записей
  short     metka_rr;   //0-ввод и корректировка 1-чистый выбор
  int       metka_voz;  //0-выбрано 1-не выбрано
      
  //Конструктор
  kontr_data()
   {
    shetsk.plus("");
    metka_voz=1;
    metka_rr=0;
    metka_sort=0;
    snanomer=0;
    kolzap=kl_shift=0;
    window=treeview=NULL;
   }  

 };


extern SQL_baza	bd;
extern char *name_system;


gboolean   kontr_key_press(GtkWidget *,GdkEventKey *,class kontr_data *);
void       kontr_knopka(GtkWidget *,class kontr_data *);
void       kontr_v_row(GtkTreeView *,GtkTreePath *,GtkTreeViewColumn *,class kontr_data *);
void       kontr_udzap(class kontr_data *data);
int        kontr_provvudzap(const char *kontr,int met,GtkWidget*);
void       kontr_vibor(GtkTreeSelection *,class kontr_data *);
void       kontr_udnz(GtkWidget*);
gint       kontr_udnz11(class kontr_udnz_data *data);
void       kontr_rasspis(class kontr_data *data);
void       kontr_snkod(class kontr_data *data);
void       kontr_sm_kod(const char *kodkon,GtkWidget *);

void       kontr_create_list (class kontr_data *);

extern char *organ;

int iceb_l_kontr(int metka_rr,//0-ввод и корректировка 1-чистый выбор
iceb_u_str *kod,
iceb_u_str *naikod,
GtkWidget  *wpredok)
{
kontr_data data;
char bros[512];
GdkColor color;

data.metka_rr=metka_rr;
if(naikod->getdlinna() > 1)
 {
  data.poisk.metka_poi=1;
  data.poisk.naimkon.plus(naikod->ravno());
 }
//printf("l_kontr-data.metka_rr=%d data.metka_poi=%d\n",data.metka_rr,data.metka_poi);
 
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Контрагенты"));

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
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(kontr_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(vbox1);
gtk_widget_show(vbox2);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Общий список контрагентов"));
//gdk_color_parse("green",&color);
//gtk_widget_modify_fg(data.label_kolstr,GTK_STATE_NORMAL,&color);


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);
gtk_widget_show (data.label_kolstr);

data.label_poisk=gtk_label_new (gettext("Ждите !"));
gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_poisk,GTK_STATE_NORMAL,&color);

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);
gtk_widget_show (data.label_poisk);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_widget_show (data.sw);


//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(kontr_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(kontr_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной запси"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(kontr_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной запси"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"%sF3 %s",RFK,gettext("Удал.неисп"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(kontr_knopka),&data);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Удаление неиспользованных контрагентов"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(kontr_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(kontr_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Най.код"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(kontr_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Найти запись по коду контрагента"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F7 %s",gettext("Сальдо"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(kontr_knopka),&data);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Получить сальдо по выбранному контрагенту"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"F8 %s",gettext("Сме.код"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]), "clicked",GTK_SIGNAL_FUNC(kontr_knopka),&data);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Сменить код контрагента на другой с удалением записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_widget_show(data.knopka[FK8]);

sprintf(bros,"F9 %s",gettext("Сортировка"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK9]), "clicked",GTK_SIGNAL_FUNC(kontr_knopka),&data);
tooltips[FK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK9],data.knopka[FK9],gettext("Сменить порядок сортировки записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK9]),(gpointer)FK9);
gtk_widget_show(data.knopka[FK9]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(kontr_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
//gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

kontr_create_list (&data);

gtk_widget_show(data.window);
if(data.metka_rr == 0)
 gtk_window_maximize(GTK_WINDOW(data.window));



//gtk_window_fullscreen(GTK_WINDOW(data.window));

//gtk_widget_hide (data.label_poisk);

gtk_main();

if(data.metka_voz == 0)
 {
  kod->new_plus(data.kodv.ravno());
  naikod->new_plus(data.naim.ravno());
 }

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.metka_voz);

}

/***********************************/
/*Создаем список для просмотра */
/***********************************/
void kontr_create_list (class kontr_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

class iceb_clock kk(data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;

//printf("kontr_create_list %d\n",data->snanomer);

//gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
//iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);
gtk_widget_show (data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(kontr_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(kontr_vibor),data);

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
G_TYPE_STRING,
G_TYPE_INT);


if(data->metka_sort == 0)
  sprintf(strsql,"select * from Kontragent order by kodkon asc");
else
  sprintf(strsql,"select * from Kontragent order by naikon asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str s1;
iceb_u_str s2;
iceb_u_str s3;
iceb_u_str s4;
iceb_u_str s5;
iceb_u_str s6;
iceb_u_str s8;
iceb_u_str s9;
iceb_u_str s10;
iceb_u_str s11;
iceb_u_str s12;
iceb_u_str s13;
iceb_u_str s14;
iceb_u_str s15;
iceb_u_str s16;

data->kolzap=0;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s\n",row[0],row[1]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  if(kontr_prov_row(row,&data->poisk) != 0)
    continue;


  if(iceb_u_SRAV(data->kontr_v.ravno(),row[0],0) == 0)
    data->snanomer=data->kolzap;

  //Код контрагента
  s1.new_plus(iceb_u_toutf(row[0]));
  
  //Наименование контрагента
  s2.new_plus(iceb_u_toutf(row[1]));

  //Телефон
  s3.new_plus(iceb_u_toutf(row[10]));

  //Адрес контрагента
  s4.new_plus(iceb_u_toutf(row[3]));

  //Счет
  s5.new_plus(iceb_u_toutf(row[7]));

  //МФО
  s6.new_plus(iceb_u_toutf(row[6]));

  //Банк
  iceb_u_str s7;
  s7.new_plus(iceb_u_toutf(row[2]));

  //Адрес банка
  s8.new_plus(iceb_u_toutf(row[4]));

  //Код ЕДРПОУ
  s9.new_plus(iceb_u_toutf(row[5]));

  //Налоговый номер
  s10.new_plus(iceb_u_toutf(row[8]));

  //Номер свидетельства плательщика ПДВ
  s11.new_plus(iceb_u_toutf(row[9]));

  //Группа
  s12.new_plus(iceb_u_toutf(row[11]));

  //Договор
  s13.new_plus(iceb_u_toutf(row[14]));

  //Регистрациооный номер частного предпринимателя
  s14.new_plus(iceb_u_toutf(row[15]));

  //Дата и время записи
  s15.new_plus(iceb_u_toutf(iceb_u_vremzap(row[13])));

  //Кто записал
  s16.new_plus(iceb_kszap(row[12],0,data->window));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KONTR,s1.ravno(),
  COL_NAIMKONTR,s2.ravno(),
  COL_TELEF,s3.ravno(),
  COL_ADRES,s4.ravno(),
  COL_SHET,s5.ravno(),
  COL_MFO,s6.ravno(),
  COL_BANK,s7.ravno(),
  COL_ADRESB,s8.ravno(),
  COL_KOD,s9.ravno(),
  COL_NALNOM,s10.ravno(),
  COL_NSPDV,s11.ravno(),
  COL_GRUPA,s12.ravno(),
  COL_DOG,s13.ravno(),
  COL_RNHP,s14.ravno(),
  COL_DATA_VREM,s15.ravno(),
  COL_KTO,s16.ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->kontr_v.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

kontr_add_columns (GTK_TREE_VIEW (data->treeview),data->shetsk.ravno());


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK8]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK8]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
 }

//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data->treeview,GTK_STATE_NORMAL,&color);


//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Общий список контрагентов"));

sprintf(strsql," %s:%d",gettext("Количество контрагентов"),data->kolzap);
zagolov.plus(strsql);
gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

if(data->poisk.metka_poi == 1)
 {
//  printf("Формирование заголовка с реквизитами поиска.\n");
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.kodkontr.ravno(),gettext("Код контрагента"));
  iceb_str_poisk(&zagolov,data->poisk.naimkon.ravno(),gettext("Наименование контрагента"));
  iceb_str_poisk(&zagolov,data->poisk.naimp.ravno(),gettext("Полное наименование"));
  iceb_str_poisk(&zagolov,data->poisk.shnds.ravno(),gettext("Спец счёт с НДС"));
  iceb_str_poisk(&zagolov,data->poisk.naimban.ravno(),gettext("Наименование банка"));
  iceb_str_poisk(&zagolov,data->poisk.adresb.ravno(),gettext("Город банка"));
  iceb_str_poisk(&zagolov,data->poisk.kod.ravno(),gettext("Код ЕГРПУ"));
  iceb_str_poisk(&zagolov,data->poisk.mfo.ravno(),gettext("МФО"));
  iceb_str_poisk(&zagolov,data->poisk.nomsh.ravno(),gettext("Номер счета"));
  iceb_str_poisk(&zagolov,data->poisk.innn.ravno(),gettext("Инд.нал.номер"));
  iceb_str_poisk(&zagolov,data->poisk.nspnds.ravno(),gettext("Ном.свид.пл.НДС"));
  iceb_str_poisk(&zagolov,data->poisk.telef.ravno(),gettext("Телефон"));
  iceb_str_poisk(&zagolov,data->poisk.grup.ravno(),gettext("Группа"));
  iceb_str_poisk(&zagolov,data->poisk.dogov.ravno(),gettext("Договор"));
  iceb_str_poisk(&zagolov,data->poisk.regnom.ravno(),gettext("Гегист.ном.ч/п"));
  



  gtk_label_set_text(GTK_LABEL(data->label_poisk),zagolov.ravno_toutf());
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 


//gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));
//printf("kontr_create_list - end\n");


}

/*****************/
/*Создаем колонки*/
/*****************/

void kontr_add_columns (GtkTreeView *treeview,const char *shet)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;

renderer = gtk_cell_renderer_text_new ();
if(shet[0] == '\0')
  g_object_set(G_OBJECT(renderer),"foreground","red",NULL);
else
  g_object_set(G_OBJECT(renderer),"foreground","blue",NULL);

column = gtk_tree_view_column_new_with_attributes (gettext("Код"),renderer, "text",COL_KONTR,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KONTR);
gtk_tree_view_append_column (treeview, column);



renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Наименование"),renderer, "text",COL_NAIMKONTR,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NAIMKONTR); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Телефон"),renderer, "text",COL_TELEF,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_TELEF); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Адрес"),renderer, "text",COL_ADRES,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_ADRES); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Счет"),renderer, "text",COL_SHET,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_SHET); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("МФО"),renderer, "text",COL_MFO,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_MFO); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Банк"),renderer, "text",COL_BANK,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_BANK); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Город банка"),renderer, "text",COL_ADRESB,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_ADRESB); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Код ЕГРПОУ"),renderer, "text",COL_KOD,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOD); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Инд.нал.номер"),renderer, "text",COL_NALNOM,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NALNOM); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Ном.свид.пл.НДС"),renderer, "text",COL_NSPDV,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NSPDV); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);



renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Группа"),renderer, "text",COL_GRUPA,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_GRUPA); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Договор"),renderer, "text",COL_DOG,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DOG); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Рег.ном.ч/п"),renderer, "text",COL_RNHP,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_RNHP); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();

column = gtk_tree_view_column_new_with_attributes (gettext("Дата и время записи"),renderer, "text",COL_DATA_VREM,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DATA_VREM); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Кто записал"),renderer, "text",COL_KTO,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешить изменять размер колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KTO); /*Разрешить сортировку по колонке*/
gtk_tree_view_append_column (treeview, column);

}
/****************************/
/*Выбор строки*/
/**********************/

void kontr_vibor(GtkTreeSelection *selection,class kontr_data *data)
{
//printf("kontr_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gchar *naim;
gint  nomer;
gtk_tree_model_get(model,&iter,0,&kod,1,&naim,16,&nomer,-1);

data->kodv.new_plus(iceb_u_fromutf(kod));
data->naim.new_plus(iceb_u_fromutf(naim));
data->snanomer=nomer;

g_free(kod);
g_free(naim);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  kontr_knopka(GtkWidget *widget,class kontr_data *data)
{
//char    bros[512];
iceb_u_str kod;
kod.plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("kontr_knopka knop=%d\n",knop);
data->kl_shift=0;

switch (knop)
 {
  case FK2:

    if(iceb_l_kontrv(&kod,"",data->window) == 0)
     {
      data->kontr_v.new_plus(kod.ravno());
      kontr_create_list (data);
     }
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    if(iceb_l_kontrv(&data->kodv,"",data->window) == 0)
     {
      data->kontr_v.new_plus(data->kodv.ravno());
      kontr_create_list (data);
     }

    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    kontr_udzap(data);
    return;  

  case SFK3:
    if(data->kolzap == 0)
      return;

    if(iceb_menu_danet(gettext("Удалить не используемые контрагеты ? Вы уверены ?"),2,data->window) == 2)
      return;

    kontr_udnz(data->window);
    kontr_create_list(data);
    return;  

  case FK4:
    if(iceb_l_kontrp(&data->poisk,data->window) == 0)
     data->poisk.metka_poi=1;
    else
     data->poisk.metka_poi=0;
    kontr_create_list(data);
    return;  

  case FK5:
//    gtk_widget_hide(data->window);
    kontr_rasspis(data);
//    gtk_widget_show(data->window);
    return;  

  case FK6:
    kontr_snkod(data);
    return;  

  case FK7:
   iceb_salorok(data->kodv.ravno(),data->window);
   return;

  case FK8:
   kontr_sm_kod(data->kodv.ravno(),data->window);
   kontr_create_list (data);
   return;
   
  case FK9:
    data->metka_sort++;
    if(data->metka_sort > 1)
      data->metka_sort=0;
      
    kontr_create_list (data);
    return;
    
  case FK10:
//    printf("kontr_knopka F10\n");
    gtk_widget_destroy(data->window);
    data->metka_voz=1;
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kontr_key_press(GtkWidget *widget,GdkEventKey *event,class kontr_data *data)
{
iceb_u_str repl;
//printf("kontr_key_press keyval=%d state=%d\n",event->keyval,event->state);
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
//    printf("kontr_key_press-Нажата клавиша Shift\n");

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
void kontr_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class kontr_data *data)
{
//printf("kontr_v_row data->metka_rr=%d\n",data->metka_rr);
if(data->metka_rr == 0)
 {
  gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
  return;
 }

//printf("kontr_v_row выход\n");

gtk_widget_destroy(data->window);

data->metka_voz=0;

}
/***********************/
/*Удаление записи      */
/***********************/
void   kontr_udzap(class kontr_data *data)
{
char  strsql[312];

 
iceb_u_str sp;
sp.plus(gettext("Удалить запись ? Вы уверены ?"));
if(iceb_menu_danet(&sp,2,data->window) == 2)
 return;

if(kontr_provvudzap(data->kodv.ravno(),1,data->window) != 0)
  return;

sprintf(strsql,"delete from Kontragent where kodkon='%s'",data->kodv.ravno());
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;

sprintf(strsql,"delete from Skontr where kodkon='%s'",data->kodv.ravno());
iceb_sql_zapis(strsql,0,0,data->window);


kontr_create_list(data);

}

/********************************/
/*Проверка возможности удаления записи*/
/***************************************/

int kontr_provvudzap(const char *kontr,int met,GtkWidget *wpredok) //0-молча 1-сообщения
{
char strsql[512];

sprintf(strsql,"select kodkon from Saldo where kodkon='%s' limit 1",
kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    iceb_u_str soob;
    sprintf(strsql,"%s Saldo",gettext("Удалить невозможно ! Есть записи в таблице"));
    soob.plus(strsql);
    iceb_menu_soob(&soob,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select kodkon from Prov where kodkon='%s'",
kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    iceb_u_str soob;
    sprintf(strsql,"%s Prov",gettext("Удалить невозможно ! Есть записи в таблице"));
    soob.plus(strsql);
    iceb_menu_soob(&soob,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select kontr from Dokummat where kontr='%s' limit 1",
kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    iceb_u_str soob;
    sprintf(strsql,"%s Dokummat",gettext("Удалить невозможно ! Есть записи в таблице"));
    soob.plus(strsql);
    iceb_menu_soob(&soob,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select kontr from Usldokum where kontr='%s' limit 1",
kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  if(met != 0)
   {
    iceb_u_str soob;
    sprintf(strsql,"%s Usldokum",gettext("Удалить невозможно ! Есть записи в таблице"));
    soob.plus(strsql);
    iceb_menu_soob(&soob,wpredok);
   }
  return(1);
 }    

sprintf(strsql,"select kontr from Uosdok where kontr='%s' limit 1",
kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Uosdok",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }    


sprintf(strsql,"select polu from Pltp where polu like '%s#%%' limit 1",kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Pltp",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }    

sprintf(strsql,"select polu from Pltt where polu like '%s#%%' limit 1",kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Pltt",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }    

sprintf(strsql,"select polu from Tpltt where polu like '%s#%%' limit 1",kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Tpltt",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }    

sprintf(strsql,"select polu from Tpltp where polu like '%s#%%' limit 1",
kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Tpltp",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }    


memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"select kontr from Kasord1 where kontr='%s' limit 1",kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Kasord1",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }    


memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"select kont from Ukrdok where kont='%s' limit 1",kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
   {
    if(met != 0)
     {
      iceb_u_str soob;
      sprintf(strsql,"%s Ukrdok",gettext("Удалить невозможно ! Есть записи в таблице"));
      soob.plus(strsql);
      iceb_menu_soob(&soob,wpredok);
     }
    return(1);
   }    


memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"select kontr from Uplouot where kontr='%s' limit 1",kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  
  if(met != 0)
   {
    iceb_u_str soob;
    sprintf(strsql,"%s Uplouot",gettext("Удалить невозможно ! Есть записи в таблице"));
    soob.plus(strsql);
    iceb_menu_soob(&soob,wpredok);
   }
  return(1);
 }

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"select kontr from Ukrdok1 where kontr='%s' or kdrnn='%s' limit 1",
kontr,kontr);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  
  if(met != 0)
   {
    iceb_u_str soob;
    sprintf(strsql,"%s Ukrdok1",gettext("Удалить невозможно ! Есть записи в таблице"));
    soob.plus(strsql);
    iceb_menu_soob(&soob,wpredok);
   }
  return(1);
 }


return(0);
}

/*************************************/
/*Удаление не используемых кодов контрагентов*/
/*********************************************/

void  kontr_udnz(GtkWidget *wpredok)
{
char strsql[512];
kontr_udnz_data data;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_deletable(GTK_WINDOW(data.window),FALSE); /*Выключить кнопку close в рамке окна*/
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Удаление контрагентов, которые не используются"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

//gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

GtkWidget *label=gtk_label_new(gettext("Ждите !!!"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

label=gtk_label_new(gettext("Удаление контрагентов, которые не используются."));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();
data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
//gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);

data.bar=gtk_progress_bar_new();

gtk_progress_bar_set_bar_style(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_CONTINUOUS);

gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_LEFT_TO_RIGHT);

gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна
gtk_widget_show_all(data.window);

gtk_idle_add((GtkFunction)kontr_udnz11,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

}
/**********************************/
/**********************************/
gint   kontr_udnz11(class kontr_udnz_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur1;
GtkTextIter iter;

//printf("kontr_udnz11 %f\n",data->kolstr1);

if(data->kolstr1 == 0)
 {
  sprintf(strsql,"select * from Kontragent");
  if((data->kolstr=data->cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    return(FALSE);
   }
  
  class iceb_u_str imaf;

  iceb_imafn("Kontragent.dat",&imaf);
//  printf("Имя файла:%s\n",imaf);
  
  if((data->ff = fopen(imaf.ravno(),"a")) == NULL)
   {
    iceb_er_op_fil(imaf.ravno(),"",errno,data->window);
    return(FALSE);
   }

  time_t tmm;
  struct  tm      *bf;

  time(&tmm);
  bf=localtime(&tmm);


  fprintf(data->ff,"\n%s\n",gettext("Список удаленных контрагентов"));
  fprintf(data->ff,"%s: %s\n",gettext("Логин"),iceb_u_getlogin());
  fprintf(data->ff,"%s: %s ",gettext("Имя"),iceb_getfioop(data->window));
  fprintf(data->ff,"%s %d.%d.%d%s %s: %d:%d\n",
  gettext("Дата"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("г."),
  gettext("Время"),
  bf->tm_hour,bf->tm_min);
 }
iceb_u_str stroka;
while(data->cur.read_cursor(&row) != 0)
 {
//  printf("%-5s %-40.40s ",row[0],row[1]);

  sprintf(strsql,"%s ",iceb_u_toutf(row[0]));    
  stroka.new_plus(strsql);
  sprintf(strsql,"%s\n",iceb_u_toutf(row[1]));    
  stroka.plus(strsql);
  
//  g_print("%s",stroka.ravno());
    
  gtk_text_buffer_get_iter_at_offset(data->buffer,&iter,-1);
  gtk_text_buffer_insert(data->buffer,&iter,stroka.ravno(),-1);
  gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(data->view),&iter,0.0,TRUE,0.,1.);
  
  iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

  if(kontr_provvudzap(row[0],0,data->window) != 0)
   {
//    printf("используется\n");
    return(TRUE);
   }

//  printf("удаляем\n");

  fprintf(data->ff,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],
  row[7],row[8],row[9],row[10],row[11],row[12],row[13],row[14]);

  sprintf(strsql,"delete from Skontr where kodkon='%s'",row[0]);
  if(iceb_sql_zapis(strsql,1,1,data->window) != 0)
   continue;
  
  sprintf(strsql,"delete from Kontragent where kodkon='%s'",row[0]);
  iceb_sql_zapis(strsql,1,1,data->window);

  sprintf(strsql,"delete from Kontragent1 where kodkon='%s'",row[0]);
  iceb_sql_zapis(strsql,1,1,data->window);

  data->koludk++;

  return(TRUE);
  
 }

fclose(data->ff);

printf("%s: %d\n",gettext("Количество удаленных контрагентов"),data->koludk);
//if(koludk != 0)
//  printf("%s:\n%s\n",gettext("Список удаленных контрагентов выгружено в файл"),imaf);

char imatab[30];

strcpy(imatab,"Kontragent");
//printf(gettext("Оптимизация таблицы %s !\n"),imatab);


int		metkazap=0;

strcpy(strsql,"select VERSION()");
sql_readkey(&bd,strsql,&row,&cur1);
if(iceb_u_SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  
//printf("VERSION:%s\n",row[0]);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table %s",imatab);

if(metkazap == 1)
 {
  if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql,data->window);
 }
else
  sql_readkey(&bd,strsql);

//printf(gettext("Оптимизацию таблицы закончено.\n"));

strcpy(imatab,"Skontr");
//printf(gettext("Оптимизация таблицы %s !\n"),imatab);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table %s",imatab);

if(metkazap == 1)
 {
  if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql,data->window);
 }
else
  sql_readkey(&bd,strsql);

strcpy(imatab,"Kontragent1");
//printf(gettext("Оптимизация таблицы %s !\n"),imatab);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table %s",imatab);

if(metkazap == 1)
 {
  if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql,data->window);
 }
else
  sql_readkey(&bd,strsql);



//printf(gettext("Оптимизацию таблицы закончено.\n"));
gtk_widget_destroy(data->window);

return(FALSE);
}
/*****************************************/
/*Распечатка списка контрагентов         */
/*****************************************/
void  kontr_rasspis(class kontr_data *data)
{
char imaf[56];
FILE *ff;
int kolstr=0;
char strsql[512];
SQLCURSOR cur;
SQL_str row;

sprintf(strsql,"select * from Kontragent order by kodkon asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

if(kolstr == 0)
  return;
  
sprintf(imaf,"Kontragent.lst");
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }


time_t tmm;
struct tm *bf;

time(&tmm);
bf=localtime(&tmm);


fprintf(ff,"%s\n\n%s\n\
%s %d.%d.%d%s %s: %d:%d\n",
organ,
gettext("Список контрагентов"),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Код |           Наименование  контрагента    |   Наименование банка         |  Адрес контрагента           |\
  Адрес банка              |  Код     |   MФО    | Номер счета   |Нал. номер     |Н.пл. НДС |Телефон   |Группа|Рег.н.ЧП| Договор\n"));
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
int kl=0;
while(cur.read_cursor(&row) != 0)
 {
   if(kontr_prov_row(row,&data->poisk) != 0)
    continue;
  kl++;
  fprintf(ff,"%-*s %-*.*s %-*.*s %-*.*s %-*.*s %-10s %-10s\
 %-15s %-15s %-10s %-10.10s %-6s %-8s %s\n",
  iceb_u_kolbait(5,row[0]),row[0],
  iceb_u_kolbait(40,row[1]),iceb_u_kolbait(40,row[1]),row[1],
  iceb_u_kolbait(30,row[2]),iceb_u_kolbait(30,row[2]),row[2],
  iceb_u_kolbait(30,row[3]),iceb_u_kolbait(30,row[3]),row[3],
  iceb_u_kolbait(30,row[4]),iceb_u_kolbait(30,row[4]),row[4],
  row[5],
  row[6],
  row[7],
  row[8],
  row[9],row[10],row[11],row[15],row[14]);
 }
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
%s - %d\n",gettext("Итого"),kl);

iceb_podpis(ff,data->window);

fclose(ff);

iceb_u_spisok fil;
iceb_u_spisok naim;
fil.plus(imaf);
naim.plus(gettext("Список контрагентов"));
iceb_ustpeh(imaf,3,data->window);
iceb_rabfil(&fil,&naim,"",0,data->window);
}
/**************************************/
/*Стать на код контрагента           */
/*************************************/

void    kontr_snkod(class kontr_data *data)
{
//iceb_u_str zapros;
char    kodkon[32];

memset(kodkon,'\0',sizeof(kodkon));

//zapros.plus(gettext("Введите нужный код контрагента"));
if(iceb_menu_vvod1(gettext("Введите нужный код контрагента"),kodkon,sizeof(kodkon),data->window) != 0)
  return;

if(kodkon[0] == '\0')
  return;

char strsql[512];

sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",kodkon);
printf("%s\n",strsql);
if(sql_readkey(&bd,strsql) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Нет такого кода в списке контрагентов !"));
  iceb_menu_soob(&repl,data->window);
  return;
 }


data->kontr_v.new_plus(kodkon);
kontr_create_list(data);

}
/*************************************/
/*Проверка записей на реквизиты поиска*/
/**************************************/

int  kontr_prov_row(SQL_str row,class kontr_rek *data)
{
if(data->metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->kodkontr.ravno(),row[0],0,0) != 0)
 return(1);

//Поиск образца в строке
if(data->naimkon.ravno()[0] != '\0' && iceb_u_strstrm(row[1],data->naimkon.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->naimp.ravno()[0] != '\0' && iceb_u_strstrm(row[16],data->naimp.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->naimban.ravno()[0] != '\0' && iceb_u_strstrm(row[2],data->naimban.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->adres.ravno()[0] != '\0' && iceb_u_strstrm(row[3],data->adres.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->adresb.ravno()[0] != '\0' && iceb_u_strstrm(row[4],data->adresb.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->kod.ravno()[0] != '\0' && iceb_u_strstrm(row[5],data->kod.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->mfo.ravno()[0] != '\0' && iceb_u_strstrm(row[6],data->mfo.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->nomsh.ravno()[0] != '\0' && iceb_u_strstrm(row[7],data->nomsh.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->innn.ravno()[0] != '\0' && iceb_u_strstrm(row[8],data->innn.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->nspnds.ravno()[0] != '\0' && iceb_u_strstrm(row[9],data->nspnds.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->telef.ravno()[0] != '\0' && iceb_u_strstrm(row[10],data->telef.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->grup.ravno()[0] != '\0' && iceb_u_strstrm(row[11],data->grup.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->dogov.ravno()[0] != '\0' && iceb_u_strstrm(row[14],data->dogov.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->regnom.ravno()[0] != '\0' && iceb_u_strstrm(row[15],data->regnom.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->shnds.ravno()[0] != '\0' && iceb_u_strstrm(row[17],data->shnds.ravno()) == 0)
 return(1);

return(0);
}
/************************************/
/*Смена кода контрагента с удалением*/
/************************************/
void kontr_sm_kod(const char *kodkons,GtkWidget *wpredok)
{

iceb_u_str repl;
repl.plus(gettext("Заменить код контрагента"));
repl.plus(" ");
repl.plus(kodkons);
repl.plus(" ");
repl.plus(gettext("Вы уверены ?"));
if(iceb_menu_danet(&repl,2,wpredok) == 2)
  return;

char kodkonn[32];
memset(kodkonn,'\0',sizeof(kodkonn));

repl.new_plus(gettext("Введите код контрагента на который нужно заменить"));
if(iceb_menu_vvod1(repl.ravno(),kodkonn,sizeof(kodkonn),wpredok) != 0)
  return;

//Проверяем есть ли этот код в списке контрагентов
char strsql[512];
//SQL_str row;

sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",kodkonn);
if(sql_readkey(&bd,strsql) != 1)
 {
  repl.new_plus(gettext("Нет такого кода в списке контрагентов !"));
  iceb_menu_soob(&repl,wpredok);
  return;
 }

if(iceb_parol(0,wpredok) != 0)
  return;

//Выполняем коректировку 
kontr_korkod(kodkons,kodkonn,wpredok);

//Проверяем возможность удаления
if(kontr_provvudzap(kodkons,1,wpredok) != 0)
  return;

//Удаляем из списка счетов
sprintf(strsql,"delete from Skontr where kodkon='%s'",kodkons);
iceb_sql_zapis(strsql,1,1,wpredok);

//Удаляем из списка конрагентов
sprintf(strsql,"delete from Kontragent where kodkon='%s'",kodkons);
iceb_sql_zapis(strsql,1,1,wpredok);

}
