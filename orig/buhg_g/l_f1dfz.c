/*$Id: l_f1dfz.c,v 1.25 2011-08-29 07:13:43 sasa Exp $*/
/*07.10.2008	22.12.2006	Белых А.И.	l_f1dfz.c
Работа с записями в документе формы 1ДФ
Возвращаем 0-вышли из документа
           1-удалили документ
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
#include  "buhg_g.h"
#include "l_f1dfz.h"

enum
{
  COL_INN,
  COL_FIO,
  COL_NAH_DOH,
  COL_NAH_NAL,
  COL_VIP_DOH,
  COL_VIP_NAL,
  COL_OZ_DOH,
  COL_DATA_PNR,
  COL_DATA_USR,
  COL_LGOTA,
  COL_VID_DOK,
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
  FK6,
  FK8,
  FK10,
  KOL_F_KL
};

class  l_f1dfz_data
 {
  public:

  class iceb_u_str nomd; //номер документа с которым работаем
  int metka_oth; //0-отчётный 1-новый отчётный 2-уточняющий
  int kvrt; //номер квартала
    
  //реквизиты выбранной строки
  class iceb_u_str inn_v;
  class iceb_u_str priz_v;
  class iceb_u_str lgota_v;
  class iceb_u_str pr_v;
  //Реквизиты только что введённой строки
  class iceb_u_str inn_tv;
  class iceb_u_str priz_tv;
  class iceb_u_str lgota_tv;
  class iceb_u_str pr_tv;

  
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  int voz;

  class l_f1dfz_rek rk;

  //Конструктор
  l_f1dfz_data()
   {
    voz=0;
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    clear_tv();
   }      
  void clear_tv()
   {
    inn_tv.new_plus("");
    priz_tv.new_plus("");
    lgota_tv.new_plus("");
    pr_tv.new_plus("");
   }
 };

gboolean   l_f1dfz_key_press(GtkWidget *widget,GdkEventKey *event,class l_f1dfz_data *data);
void l_f1dfz_vibor(GtkTreeSelection *selection,class l_f1dfz_data *data);
void l_f1dfz_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_f1dfz_data *data);
void  l_f1dfz_knopka(GtkWidget *widget,class l_f1dfz_data *data);
void l_f1dfz_add_columns (GtkTreeView *treeview);
void l_f1dfz_create_list (class l_f1dfz_data *data);

int l_f1dfz_v(const char *nomdok,class iceb_u_str *inn,class iceb_u_str *priz,class iceb_u_str *lgota,class iceb_u_str *pr,GtkWidget *wpredok);
int l_f1dfz1_v(const char *nomdok,class iceb_u_str *inn,class iceb_u_str *priz,class iceb_u_str *lgota,class iceb_u_str *pr,GtkWidget *wpredok);
int l_f1dfz_p(class l_f1dfz_rek *rek,GtkWidget *wpredok);
int l_f1dfz_prov(SQL_str row,class l_f1dfz_rek *rk,GtkWidget *wpredok);
void l_f1dfz_read_sh(class l_f1dfz_data *data);

int rasf1df(const char *nomd,int metka_oth,class iceb_u_spisok*,class iceb_u_spisok*,GtkWidget *wpredok);
void zagrf1df(const char *nomd,int kolst,int kvrt,GtkWidget *wpredok);

extern char *organ;
extern SQL_baza  bd;
extern char      *name_system;

int  l_f1dfz(const char *nomd,GtkWidget *wpredok)
{
class l_f1dfz_data data;
char strsql[512];
GdkColor color;
data.nomd.new_plus(nomd);



data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(strsql,"%s %s",name_system,gettext("Форма 1ДФ"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_f1dfz_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Форма 1ДФ"));
//gdk_color_parse("green",&color);
//gtk_widget_modify_fg(data.label_kolstr,GTK_STATE_NORMAL,&color);


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

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
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK1], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK1]), "clicked",GTK_SIGNAL_FUNC(l_f1dfz_knopka),&data);
tooltips[SFK1]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK1],data.knopka[SFK1],gettext("Просмотр шапки документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK1]),(gpointer)SFK1);
gtk_widget_show(data.knopka[SFK1]);

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(l_f1dfz_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи для работника вашой организации"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(l_f1dfz_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);


sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(l_f1dfz_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(strsql,"%sF3 %s",RFK,gettext("Удаление"));
data.knopka[SFK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(l_f1dfz_knopka),&data);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Удаление документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(strsql,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(l_f1dfz_knopka),&data);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(l_f1dfz_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка формы 1ДФ и получение файлов электронной отчётности"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"F6 %s",gettext("Загрузка"));
data.knopka[FK6]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(l_f1dfz_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Загрузка формы 1ДФ из файла"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);


sprintf(strsql,"F8 %s",gettext("Запись"));
data.knopka[FK8]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]), "clicked",GTK_SIGNAL_FUNC(l_f1dfz_knopka),&data);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Ввод новой записи для постороннего человека"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_widget_show(data.knopka[FK8]);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(l_f1dfz_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_f1dfz_read_sh(&data); //перед чтением
l_f1dfz_create_list(&data);

gtk_widget_show(data.window);
//if(metka_rr == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_f1dfz_create_list (class l_f1dfz_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
class SQLCURSOR cur,cur1;
char strsql[512];
int  kolstr=0;
SQL_str row,row1;
//GdkColor color;

data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_f1dfz_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_f1dfz_vibor),data);

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


sprintf(strsql,"select * from F8dr1 where nomd='%s' order by inn asc,pr desc",data->nomd.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
float kolstr1=0.;
double suma_vip_doh=0.;
double suma_vip_nal=0.;
double suma_nah_doh=0.;
double suma_nah_nal=0.;
class iceb_u_spisok fiz_lico;

while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(l_f1dfz_prov(row,&data->rk,data->window) != 0)
   continue;
  

  if(iceb_u_SRAV(data->inn_tv.ravno(),row[1],0) == 0 && \
     iceb_u_SRAV(data->priz_tv.ravno(),row[4],0) == 0 && \
     iceb_u_SRAV(data->lgota_tv.ravno(),row[7],0) == 0 && \
     iceb_u_SRAV(data->pr_tv.ravno(),row[13],0) == 0 )
        data->snanomer=data->kolzap;

  //Идентификационный номер
  ss[COL_INN].new_plus(iceb_u_toutf(row[1]));
  if(fiz_lico.find(row[1]) < 0)
   fiz_lico.plus(row[1]);
   
  //Фамилия
  ss[COL_FIO].new_plus(iceb_u_toutf(row[10]));
  if(ss[COL_FIO].getdlinna() <= 1)
   {
    sprintf(strsql,"select fio from Kartb where inn='%s'",row[1]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
      ss[COL_FIO].new_plus(iceb_u_toutf(row1[0]));
   }
   
  //Сумма выплаченного дохода
  ss[COL_VIP_DOH].new_plus(iceb_u_toutf(row[2]));
  suma_vip_doh+=atof(row[2]);
  
  //Сумма выплаченного НАЛОГА
  ss[COL_VIP_NAL].new_plus(iceb_u_toutf(row[3]));
  suma_vip_nal+=atof(row[3]);
  
  //Сумма начисленного дохода
  ss[COL_NAH_DOH].new_plus(iceb_u_toutf(row[11]));
  suma_nah_doh+=atof(row[11]);
  
  //Сумма начисленного НАЛОГА
  ss[COL_NAH_NAL].new_plus(iceb_u_toutf(row[12]));
  suma_nah_nal+=atof(row[12]);
  
  //Признак дохода
  ss[COL_OZ_DOH].new_plus(iceb_u_toutf(row[4]));

  //Дата приёма на работу
   if(row[5][0] != '0')
     ss[COL_DATA_PNR].new_plus(iceb_u_sqldata(row[5]));
   else
     ss[COL_DATA_PNR].new_plus("");
  //Дата увольнения с работы
   if(row[6][0] != '0')
     ss[COL_DATA_USR].new_plus(iceb_u_sqldata(row[6]));
   else
     ss[COL_DATA_USR].new_plus("");
   
  //Льгота
   ss[COL_LGOTA].new_plus(row[7]);

  //Вид документа
   ss[COL_VID_DOK].new_plus(row[13]);
  
  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[9])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[8],0,data->window));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_INN,ss[COL_INN].ravno(),
  COL_FIO,ss[COL_FIO].ravno(),
  COL_VIP_DOH,ss[COL_VIP_DOH].ravno(),
  COL_VIP_NAL,ss[COL_VIP_NAL].ravno(),
  COL_NAH_DOH,ss[COL_NAH_DOH].ravno(),
  COL_NAH_NAL,ss[COL_NAH_NAL].ravno(),
  COL_OZ_DOH,ss[COL_OZ_DOH].ravno(),
  COL_DATA_PNR,ss[COL_DATA_PNR].ravno(),
  COL_DATA_USR,ss[COL_DATA_USR].ravno(),
  COL_LGOTA,ss[COL_LGOTA].ravno(),
  COL_VID_DOK,ss[COL_VID_DOK].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->clear_tv();
gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_f1dfz_add_columns (GTK_TREE_VIEW (data->treeview));


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
zagolov.plus(gettext("Форма 1ДФ"));
zagolov.plus(" ");
zagolov.plus(gettext("Номер документа"));
zagolov.plus(":");
zagolov.plus(data->nomd.ravno());
zagolov.plus(" ");
if(data->metka_oth == 0)
  zagolov.plus(gettext("Отчётный"));
if(data->metka_oth == 1)
  zagolov.plus(gettext("Новый отчётный"));
if(data->metka_oth == 2)
  zagolov.plus(gettext("Уточняющий"));

sprintf(strsql,"\n%s:%d %s:%d",
gettext("Количество записей"),data->kolzap,
gettext("Количество физических лиц"),fiz_lico.kolih());

zagolov.plus(strsql);

zagolov.ps_plus(gettext("Сумма выплаченного дохода"));
zagolov.plus(":");
zagolov.plus(suma_vip_doh);

zagolov.plus(" ");
zagolov.plus(gettext("Сумма выплаченного налога"));
zagolov.plus(":");
zagolov.plus(suma_vip_nal);

zagolov.ps_plus(gettext("Сумма начисленного дохода"));
zagolov.plus(":");
zagolov.plus(suma_nah_doh);

zagolov.plus(" ");
zagolov.plus(gettext("Сумма начисленного налога"));
zagolov.plus(":");
zagolov.plus(suma_nah_nal);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

if(data->rk.metka_poi == 0)
 {
  printf("Формирование заголовка с реквизитами поиска.\n");
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->rk.fio.ravno(),gettext("Фамилия"));
  iceb_str_poisk(&zagolov,data->rk.inn.ravno(),gettext("Инд.нал.номер"));
  iceb_str_poisk(&zagolov,data->rk.kod_doh.ravno(),gettext("Код дохода"));

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

void l_f1dfz_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Иден.номер"), renderer,"text", COL_INN,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Фамилия Имя Отчество"), renderer,"text", COL_FIO,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Вып-ный доход"), renderer,"text", COL_VIP_DOH,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Вып-ный налог"), renderer,"text", COL_VIP_NAL,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Нач-ный доход"), renderer,"text", COL_NAH_DOH,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Нач-ный налог"), renderer,"text", COL_NAH_NAL,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("П/д"), renderer,"text", COL_OZ_DOH,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата приёма"), renderer,"text", COL_DATA_PNR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата увольнения"), renderer,"text", COL_DATA_USR,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Льгота"), renderer,"text", COL_LGOTA,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("В/д"), renderer,"text", COL_VID_DOK,NULL);



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

void l_f1dfz_vibor(GtkTreeSelection *selection,class l_f1dfz_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;


gchar *inn;
gchar *priz;
gchar *lgota;
gchar *pr;
gint  nomer;


gtk_tree_model_get(model,&iter,
COL_INN,&inn,
COL_OZ_DOH,&priz,
COL_LGOTA,&lgota,
COL_VID_DOK,&pr,
NUM_COLUMNS,&nomer,-1);

data->inn_v.new_plus(inn);
data->priz_v.new_plus(priz);
data->lgota_v.new_plus(lgota);
data->pr_v.new_plus(pr);

data->snanomer=nomer;

g_free(inn);
g_free(priz);
g_free(lgota);
g_free(pr);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_f1dfz_knopka(GtkWidget *widget,class l_f1dfz_data *data)
{
char    strsql[512];
SQL_str row;
class SQLCURSOR cur;
iceb_u_str repl;
class iceb_u_spisok imaf;
class iceb_u_spisok naimf;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_f1dfz_knopka knop=%d\n",knop);
data->kl_shift=0;
switch (knop)
 {
  case SFK1:
    if(l_f1df_v(&data->nomd,data->window) == 0)
     {
      l_f1dfz_read_sh(data);
      l_f1dfz_create_list(data);
     }
  return;
    
  case FK2:
    if(l_f1dfz_v(data->nomd.ravno(),&data->inn_tv,&data->priz_tv,&data->lgota_tv,&data->pr_tv,data->window) == 0)
      l_f1dfz_create_list(data);
    return;  

  case SFK2:
    sprintf(strsql,"select fio from F8dr1 where nomd='%s' and inn='%s' and priz='%s' and lgota='%s' and pr=%d",
    data->nomd.ravno(),
    data->inn_v.ravno(),
    data->priz_v.ravno(),
    data->lgota_v.ravno(),
    data->pr_v.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
     {
      iceb_menu_soob(gettext("Не найдена запись для корректировки !"),data->window);
      return;
     }
    if(row[0][0] == '\0')
     {
      //корректировка тех кто работает в организации
      if(l_f1dfz_v(data->nomd.ravno(),&data->inn_v,&data->priz_v,&data->lgota_v,&data->pr_v,data->window) == 0)
        l_f1dfz_create_list(data);
     }
    else
     {
      //корректировка записей посторонних людей
      if(l_f1dfz1_v(data->nomd.ravno(),&data->inn_v,&data->priz_v,&data->lgota_v,&data->pr_v,data->window) == 0)
        l_f1dfz_create_list(data);
     }
    return;  
  
  case FK3:
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
      return;

    sprintf(strsql,"delete from F8dr1 where nomd='%s' and inn='%s' \
and priz='%s' and lgota='%s' and pr=%d",
    data->nomd.ravno(),
    data->inn_v.ravno(),
    data->priz_v.ravno(),
    data->lgota_v.ravno(),
    data->pr_v.ravno_atoi());

    iceb_sql_zapis(strsql,0,0,data->window);

    l_f1dfz_create_list(data);

    return;
  
  case SFK3:
    if(iceb_menu_danet(gettext("Удалить документ ? Вы уверены ?"),2,data->window) == 2)
      return;

    sprintf(strsql,"delete from F8dr1 where nomd='%s'",data->nomd.ravno());
    if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
     return;

    sprintf(strsql,"delete from F8dr where nomd='%s'",data->nomd.ravno());
    if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
     return;

    data->voz=1;
    gtk_widget_destroy(data->window);
    return;

  case FK4:
    l_f1dfz_p(&data->rk,data->window);
    l_f1dfz_create_list(data);
    return;

  case FK5:
    if(rasf1df(data->nomd.ravno(),data->metka_oth,&imaf,&naimf,data->window) == 0)
       iceb_rabfil(&imaf,&naimf,"",0,data->window);
    return;

  case FK6:
    zagrf1df(data->nomd.ravno(),data->kolzap,data->kvrt,data->window);
    l_f1dfz_create_list(data);
    return;

  case FK8:
    if(l_f1dfz1_v(data->nomd.ravno(),&data->inn_tv,&data->priz_tv,&data->lgota_tv,&data->pr_tv,data->window) == 0)
      l_f1dfz_create_list(data);
    return;  
    
  case FK10:
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_f1dfz_key_press(GtkWidget *widget,GdkEventKey *event,class l_f1dfz_data *data)
{
iceb_u_str repl;

switch(event->keyval)
 {

  case GDK_F1:

//    if(data->kl_shift == 0)
//      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK1]),"clicked");
//    else
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

  case GDK_F6:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
    return(TRUE);


  case GDK_F8:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK8]),"clicked");
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
void l_f1dfz_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_f1dfz_data *data)
{
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");

}

/************************/
/*Проверка записи*/
/*******************/

int l_f1dfz_prov(SQL_str row,class l_f1dfz_rek *rk,GtkWidget *wpredok)
{
if(rk->metka_poi == 1)
 return(0);

//Полное сравнение
if(iceb_u_proverka(rk->kod_doh.ravno(),row[4],0,0) != 0)
 return(1);

//Поиск образца в строке
if(rk->inn.ravno()[0] != '\0' && iceb_u_strstrm(row[1],rk->inn.ravno()) == 0)
 return(1);

if(rk->fio.getdlinna() > 1)
 {
  if(row[10][0] != '\0')
   {
    if(iceb_u_strstrm(row[10],rk->fio.ravno()) == 0)
     return(1);
   }
  else
   {
    char strsql[512];
    SQL_str row1;
    class SQLCURSOR cur;
    sprintf(strsql,"select fio from Kartb where inn='%s'",row[1]);
    if(iceb_sql_readkey(strsql,&row1,&cur,wpredok) == 1)
     if(iceb_u_strstrm(row1[0],rk->fio.ravno()) == 0)
      return(1);
     

   }  
 }
   
return(0);


}
/************************/
/*Чтение шапки документа*/
/************************/
void l_f1dfz_read_sh(class l_f1dfz_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select kvrt,vidd from F8dr where nomd='%s'",data->nomd.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  iceb_menu_soob(gettext("Не найдена шапка документа !"),data->window);
  return;
 }
data->kvrt=atoi(row[0]);
data->metka_oth=atoi(row[1]);

}
