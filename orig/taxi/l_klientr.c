/*$Id: l_klientr.c,v 1.63 2011-02-21 07:36:20 sasa Exp $*/
/*14.11.2008	01.03.2004	Белых А.И.	l_klientr.c
Работа со списком клиентов
Если вернули 0-документ не удален
             1-удален
*/
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "i_rest.h"
#include "l_klientr.h"


void klientr_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class klientr_data *data);
gboolean   klientr_key_press(GtkWidget *widget,GdkEventKey *event,class klientr_data *data);
void  klientr_knopka(GtkWidget *widget,class klientr_data *data);
void klientr_add_columns(GtkTreeView *treeview);
void klientr_vibor(GtkTreeSelection *selection,class klientr_data *data);
void klientr_udzap(class klientr_data *data);
int l_klientr_v(iceb_u_str *,int,GtkWidget*);
int l_klientr_p(class klient_rek_data *data,GtkWidget*);
void	klient_ras(class klientr_data *data);
int   klientr_prov_row(SQL_str row,class klientr_data *data);
int klientr_vvod_proc(const char *kodkl,GtkWidget *wpredok);
int l_klientr_bk(GtkWidget *wpredok);
void l_klient_impk(GtkWidget *wpredok);
int l_klient_imp(const char *imaf,GtkWidget *wpredok);
int l_klientr_bl(class iceb_u_str*,class iceb_u_str*,GtkWidget *wpredok);

void  l_klientr_radio0(GtkWidget *widget,class  klientr_data *data);
void  l_klientr_radio1(GtkWidget *widget,class  klientr_data *data);
void  l_klientr_radio2(GtkWidget *widget,class  klientr_data *data);

void l_klientr_vg(class  klientr_data*);

extern SQL_baza	bd;
extern char *name_system;

int  l_klientr(int metka, //0-работа со списком 1-выбор из списка
iceb_u_str *kodkl,
GtkWidget *wpredok)
{
klientr_data data;
char strsql[300];
GdkColor color;
SQLCURSOR cur;
data.metka_rr=metka;

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(strsql,"%s %s",name_system,gettext("Список клиентов"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(klientr_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Список клиентов"));
//gdk_color_parse("green",&color);
//gtk_widget_modify_fg(data.label_kolstr,GTK_STATE_NORMAL,&color);


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.labelpoi=gtk_label_new ("");
gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.labelpoi,GTK_STATE_NORMAL,&color);

gtk_box_pack_start (GTK_BOX (vbox2),data.labelpoi,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

GtkWidget *separator1=gtk_hseparator_new();
gtk_box_pack_start (GTK_BOX (vbox2),separator1, TRUE, TRUE, 2);

GtkWidget *hbox_radio = gtk_hbox_new (FALSE, 1);
gtk_box_pack_start (GTK_BOX (vbox2),hbox_radio, FALSE, FALSE, 2);
gtk_widget_show(hbox_radio);


//Вставляем радиокнопки
/*************
data.radiobutton[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Разблокированные"));
data.radiobutton[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton[0]),gettext("Заблокированные"));
data.radiobutton[2]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton[1]),gettext("Все"));

gtk_box_pack_start (GTK_BOX (hbox_radio), data.radiobutton[0], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (hbox_radio), data.radiobutton[1], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (hbox_radio), data.radiobutton[2], TRUE, TRUE, 1);

gtk_widget_show(data.radiobutton[0]);
gtk_widget_show(data.radiobutton[1]);
gtk_widget_show(data.radiobutton[2]);
**************/
//Вставляем радиокнопки
GSList *group;

data.radiobutton[0]=gtk_radio_button_new_with_label(NULL,gettext("Разблокированные"));
gtk_box_pack_start (GTK_BOX (hbox_radio),data.radiobutton[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton[0]), "clicked",GTK_SIGNAL_FUNC(l_klientr_radio0),&data);

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton[0]));

data.radiobutton[1]=gtk_radio_button_new_with_label(group,gettext("Заблокированные"));
gtk_box_pack_start (GTK_BOX (hbox_radio),data.radiobutton[1], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton[1]), "clicked",GTK_SIGNAL_FUNC(l_klientr_radio1),&data);

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton[1]));

data.radiobutton[2]=gtk_radio_button_new_with_label(group,gettext("Все"));
gtk_box_pack_start (GTK_BOX (hbox_radio),data.radiobutton[2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton[2]), "clicked",GTK_SIGNAL_FUNC(l_klientr_radio2),&data);


gtk_widget_show_all(hbox_radio);



//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(klientr_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[SFK2]),FALSE);//Недоступна
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(klientr_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной запси"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);


sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[FK3]),FALSE);//Недоступна
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(klientr_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(strsql,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(klientr_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);


sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(klientr_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"F6 %s",gettext("Процент"));
data.knopka[FK6]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(klientr_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Ввод процента скидки для выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(strsql,"F7 %s",gettext("Группа"));
data.knopka[FK7]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(klientr_knopka),&data);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Ввод клиентов в группу"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);

sprintf(strsql,"F8 %s",gettext("Блокирование"));
data.knopka[FK8]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]), "clicked",GTK_SIGNAL_FUNC(klientr_knopka),&data);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Блокировка неактивных карточек"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_widget_show(data.knopka[FK8]);

sprintf(strsql,"F9 %s",gettext("Импорт"));
data.knopka[FK9]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK9]), "clicked",GTK_SIGNAL_FUNC(klientr_knopka),&data);
tooltips[FK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK9],data.knopka[FK9],gettext("Импорт карточек из файла"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK9]),(gpointer)FK9);
gtk_widget_show(data.knopka[FK9]);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(klientr_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);



gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

klientr_create_list(&data);

//gtk_window_fullscreen(GTK_WINDOW(data.window));
gtk_widget_show(data.window);

if(metka == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));

gtk_main();
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

if(data.voz == 0)
  kodkl->new_plus(data.kodv.ravno());
  
printf("l_klientr end\n");

return(data.voz);

}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  l_klientr_radio0(GtkWidget *widget,class  klientr_data *data)
{
if(data->metka_bl == 0)
 return;
data->metka_bl=0;
//читаем значение радиокнопок

//if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->radiobutton[2])) == TRUE)
//   data->metka_bl=2;

klientr_create_list(data);

}

void  l_klientr_radio1(GtkWidget *widget,class  klientr_data *data)
{
if(data->metka_bl == 1)
 return;
data->metka_bl=1;

klientr_create_list(data);

}

void  l_klientr_radio2(GtkWidget *widget,class  klientr_data *data)
{
if(data->metka_bl == 2)
 return;
data->metka_bl=2;

klientr_create_list(data);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void klientr_create_list (class klientr_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[300];
int  kolstr=0;
SQL_str row;
//SQL_str row1;
//GdkColor color;
//double suma=0.;
//double isuma=0.;

//printf("klientr_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(klientr_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(klientr_vibor),data);

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

if(data->metka_bl == 2)
 sprintf(strsql,"select * from Taxiklient order by fio asc");
if(data->metka_bl == 0)
 sprintf(strsql,"select * from Taxiklient where mb=0 order by fio asc");
if(data->metka_bl == 1)
 sprintf(strsql,"select * from Taxiklient where mb=1 order by fio asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
float kolstr1=0.;
short d,m,g;
char data_vk[20];
char data_d[20];

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  if(klientr_prov_row(row,data) != 0)
      continue;


  //Код
  ss[COL_KOD].new_plus(iceb_u_toutf(row[0]));
  //Метка блокировки
  if(atoi(row[14]) == 0)
   ss[COL_MB].new_plus("");
  else  
   ss[COL_MB].new_plus("*");
  //фамилия
  ss[COL_FIO].new_plus(iceb_u_toutf(row[1]));

  //адрес
  ss[COL_ADRES].new_plus(iceb_u_toutf(row[2]));

  //телефон
  ss[COL_TELEFON].new_plus(iceb_u_toutf(row[3]));

  //Дата и время записи
  ss[COL_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[5])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_u_kszap(row[4],0));

  //Процент
  ss[COL_PROCENT].new_plus(iceb_u_toutf(row[6]));

  //Группа
  ss[COL_GRUPA].new_plus(iceb_u_toutf(row[7]));

  //Список подразделений доступ в которые запрещён
  ss[COL_SP_PODR].new_plus(iceb_u_toutf(row[8]));

  if(row[10][0] != '0')
   {   
    iceb_u_polen(row[10],data_d,sizeof(data_d),1,' ');
    iceb_u_rsdat(&d,&m,&g,data_d,2);
    iceb_u_polen(row[10],data_vk,sizeof(data_vk),2,' ');
    sprintf(strsql,"%02d.%02d.%d %s",d,m,g,data_vk);
    ss[COL_DVK].new_plus(strsql);
   }
  else
    ss[COL_DVK].new_plus("");
     
  if(atoi(row[9]) == 0)
    ss[COL_KVV].new_plus(gettext("Включено"));
  else      
    ss[COL_KVV].new_plus(gettext("Выключено"));
  gtk_list_store_append (model, &iter);
  
  if(row[11][0] == '0')
    ss[COL_POL].new_plus(gettext("Мужчина"));
  else
    ss[COL_POL].new_plus(gettext("Женщина"));

  //день рождения клиента
  if(row[12][0] != '0')
   ss[COL_DEN_ROG].new_plus(iceb_u_sqldata(row[12]));
  else
   ss[COL_DEN_ROG].new_plus("");
  
  ss[COL_KOMENT].new_plus(iceb_u_toutf(row[13]));
    
  gtk_list_store_set (model, &iter,
  COL_KOD,ss[COL_KOD].ravno(),
  COL_MB,ss[COL_MB].ravno(),
  COL_POL,ss[COL_POL].ravno(),
  COL_DEN_ROG,ss[COL_DEN_ROG].ravno(),
  COL_FIO,ss[COL_FIO].ravno(),
  COL_PROCENT,ss[COL_PROCENT].ravno(),
  COL_GRUPA,ss[COL_GRUPA].ravno(),
  COL_SP_PODR,ss[COL_SP_PODR].ravno(),
  COL_KOMENT,ss[COL_KOMENT].ravno(),
  COL_ADRES,ss[COL_ADRES].ravno(),
  COL_TELEFON,ss[COL_TELEFON].ravno(),
  COL_DVK,ss[COL_DVK].ravno(),
  COL_KVV,ss[COL_KVV].ravno(),
  COL_VREM,ss[COL_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

klientr_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
 }


gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str stroka;
iceb_u_str zagolov;

sprintf(strsql,"%s  %s:%d",
gettext("Список клиентов"),
gettext("Количество записей"),
data->kolzap);

zagolov.plus(strsql);


gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());


gtk_widget_show(data->label_kolstr);

if(data->metkapoi == 1)
 {
  iceb_u_str spis;
  sprintf(strsql,"%s !!!",gettext("Поиск"));
  spis.plus(strsql);
  iceb_str_poisk(&spis,data->poi.kod.ravno(),gettext("Код"));
  iceb_str_poisk(&spis,data->poi.fio.ravno(),gettext("Фамилия клиента"));
  iceb_str_poisk(&spis,data->poi.adres.ravno(),gettext("Адрес"));
  iceb_str_poisk(&spis,data->poi.telef.ravno(),gettext("Телефон"));
  iceb_str_poisk(&spis,data->poi.pr_sk.ravno(),gettext("Процент скидки"));
  iceb_str_poisk(&spis,data->poi.kodgr.ravno(),gettext("Группа"));
  iceb_str_poisk(&spis,data->poi.koment.ravno(),gettext("Коментарий"));

  gtk_label_set_text(GTK_LABEL(data->labelpoi),spis.ravno_toutf());
      
  gtk_widget_show(data->labelpoi); //Показываем

 }
else
 {
  gtk_widget_hide(data->labelpoi); //Показываем
 }


gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));

//printf("klientr_create_list end\n");

}

/*****************/
/*Создаем колонки*/
/*****************/

void klientr_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

//printf("klientr_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_KOD,NULL);

renderer = gtk_cell_renderer_text_new ();
//g_object_set(G_OBJECT(renderer),"foreground","white",NULL);
//g_object_set(G_OBJECT(renderer),"background","black",NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,"MB", renderer,"text", COL_MB,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Фамилия клиента"), renderer,"text", COL_FIO,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Пол"), renderer,"text", COL_POL,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Процент"), renderer,"text", COL_PROCENT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Группа"), renderer,"text", COL_GRUPA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Подразделение"), renderer,"text", COL_SP_PODR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Адрес"), renderer,"text", COL_ADRES,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Коментарий"), renderer,"text", COL_KOMENT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Телефон"), renderer,"text", COL_TELEFON,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата возврата"), renderer,"text", COL_DVK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата рождения"), renderer,"text", COL_DEN_ROG,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Контроль"), renderer,"text", COL_KVV,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,"text", COL_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,"text", COL_KTO,NULL);

//printf("klientr_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void klientr_vibor(GtkTreeSelection *selection,class klientr_data *data)
{
printf("klientr_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_KOD,&kod,NUM_COLUMNS,&nomer,-1);

data->kodv.new_plus(iceb_u_fromutf(kod));
data->nomervsp=nomer;

g_free(kod);

//printf("%s %d\n",data->kodv.ravno(),nomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  klientr_knopka(GtkWidget *widget,class klientr_data *data)
{
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("klientr_knopka knop=%d\n",knop);
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch ((gint)knop)
 {
  case FK2:
    data->kodkl.new_plus("");
    if(l_klientr_v(&data->kodkl,1,data->window) == 0)
      klientr_create_list(data);
    
    return;  

  case SFK2:

    data->kodkl.new_plus(data->kodv.ravno());
    if(l_klientr_v(&data->kodkl,0,data->window) == 0)
      klientr_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    klientr_udzap(data);
    return;  

  case FK4:
    data->metkapoi=l_klientr_p(&data->poi,data->window);
    klientr_create_list(data);
    return;  

  case FK5:
    klient_ras(data);
    return;  

  case FK6:
    if(data->kolzap == 0)
      return;
    if(klientr_vvod_proc(data->kodv.ravno(),data->window) != 0)
      return;

    klientr_create_list(data);
    return;  

  case FK7:

    l_klientr_vg(data);

    klientr_create_list(data);
    return;  

  case FK8:

    if(l_klientr_bk(data->window) == 0)
      klientr_create_list(data);
    return;  

  case FK9:
    l_klient_impk(data->window); 
    klientr_create_list(data);
    return;  

    
  case FK10:
//    printf("klientr_knopka F10\n");
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   klientr_key_press(GtkWidget *widget,GdkEventKey *event,class klientr_data *data)
{
iceb_u_str repl;
printf("klientr_key_press keyval=%d state=%d\n",
event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_F2:
    if(data->kl_shift == 0)   
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    if(data->kl_shift == 1)   
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
    return(TRUE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("klientr_key_press-Нажата клавиша Shift\n");

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
void klientr_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class klientr_data *data)
{
//printf("klientr_v_row корректировка %d\n",data->metka_rr);
data->snanomer=data->nomervsp;
if(data->metka_rr == 0)
  gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
else
 {
  gtk_widget_destroy(data->window);
  data->voz=0;
 }
}


/****************************/
/*Удалить запись*/
/****************************/

void klientr_udzap(class klientr_data *data)
{

if(iceb_klient_pvu(data->kodv.ravno(),data->window) != 0)
  return;


char strsql[300];

iceb_u_str repl;

repl.plus(gettext("Удалить запись ? Вы уверены ?"));
if(iceb_menu_danet(&repl,2,data->window) == 2)
  return;


sprintf(strsql,"delete from Taxiklient where kod='%s'",
data->kodv.ravno_filtr());
printf("klientr_v_udzap %s\n",strsql);

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
  return;

klientr_create_list(data);

}
/****************************************/
/*Распечатка списка клиентов            */
/****************************************/

void	klient_ras(class klientr_data *data)
{
char		strsql[300];
SQL_str		row;
int		kolstr=0;
FILE		*ff;
char		imaf[40];
SQLCURSOR cur;
//printf("rasklient\n");
sprintf(strsql,"select * from Taxiklient order by kod asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
sprintf(imaf,"ps%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }

iceb_u_startfil(ff);
fprintf(ff,"\x0F"); //Ужатый режим
iceb_u_zagolov("Список постоянных клиентов",0,0,0,0,0,0,"",ff);

if(data->metkapoi == 1)
 {
  if(data->poi.kod.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Код"),data->poi.kod.ravno());
  if(data->poi.fio.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Фамилия клиента"),data->poi.fio.ravno());
  if(data->poi.adres.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Адрес"),data->poi.adres.ravno());
  if(data->poi.telef.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Телефон"),data->poi.telef.ravno());
  if(data->poi.pr_sk.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Процент скидки"),data->poi.pr_sk.ravno());
  if(data->poi.kodgr.getdlinna() > 1)
   fprintf(ff,"%s:%s\n",gettext("Группа"),data->poi.kodgr.ravno());
 } 


fprintf(ff,"\
---------------------------------------------------------------------------------------\n\
   Код    | Фамилия Имя Отчество         |Дата рожд.|  Телефон   |   Адрес \n\
---------------------------------------------------------------------------------------\n");
int kolkl=0;                                                   
char den_rog[20];
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
   if(klientr_prov_row(row,data) != 0)
      continue;
  kolkl++;
  if(row[12][0] != '0')
    sprintf(den_rog,"%s",iceb_u_sqldata(row[12]));
  else
    memset(den_rog,'\0',sizeof(den_rog));
  fprintf(ff,"%-10s %-30s %10s %-12s %s\n",row[0],row[1],den_rog,row[3],row[2]);

 }
fprintf(ff,"\
-----------------------------------------------------------------------------\n");
fprintf(ff,"%s:%d\n",gettext("Коичество клиентов"),kolkl);

fprintf(ff,"\x12"); //Нормальные буквы
iceb_podpis(ff,data->window);
fclose(ff);

iceb_u_spisok fil;
iceb_u_spisok nazv;

fil.plus(imaf);
nazv.plus(gettext("Список клиентов"));

//printf("rasklient-fine\n");

iceb_rabfil(&fil,&nazv,"",0,data->window);


}
/**********************************/
/*Проверка записи на условия поиска*/
/************************************/

int   klientr_prov_row(SQL_str row,class klientr_data *data)
{
//printf("lklient_prov_row\n");

if(data->metkapoi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->poi.kod.ravno(),row[0],0,0) != 0)
 return(1);

//Поиск образца в строке
if(data->poi.fio.ravno()[0] != '\0' && iceb_u_strstrm(row[1],data->poi.fio.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->poi.koment.ravno()[0] != '\0' && iceb_u_strstrm(row[13],data->poi.koment.ravno()) == 0)
 return(1);

if(data->poi.adres.ravno()[0] != '\0' && iceb_u_strstrm(row[2],data->poi.adres.ravno()) == 0)
 return(1);

if(data->poi.telef.ravno()[0] != '\0' && iceb_u_strstrm(row[3],data->poi.telef.ravno()) == 0)
 return(1);
if(data->poi.pr_sk.getdlinna() > 1)
 if(data->poi.pr_sk.ravno_atof() != atof(row[6]))
  return(1);

if(iceb_u_proverka(data->poi.kodgr.ravno(),row[7],0,0) != 0)
   return(1);
   
return(0);
}

/***************************/
/*Ввод процента скидки для выбранной записи*/
/*******************************/

int klientr_vvod_proc(const char *kodkl,GtkWidget *wpredok)
{

class iceb_u_str imaf("restnast.alx");
//iceb_imafn("restnast.alx",&imaf);
char strsql[1024];
iceb_u_str repl;

if(iceb_poldan("Список логинов, которым разрешено вводить процент скидки",strsql,imaf.ravno(),wpredok) != 0)
 {
  repl.new_plus(gettext("Вам запрещено вводить процент скидки !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }
struct  passwd  *ktor; /*Кто работает*/
ktor=getpwuid(getuid());
if(iceb_u_proverka(strsql,ktor->pw_name,0,1) != 0)
 {
  repl.new_plus(gettext("Вам запрещено вводить процент скидки !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }


//узнаём фамилию клиента
char naimkl[512];
memset(naimkl,'\0',sizeof(naimkl));
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select fio from Taxiklient where kod='%s'",kodkl);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 strncpy(naimkl,row[0],sizeof(naimkl)-1);

repl.new_plus(gettext("Введите процент скидки"));
repl.ps_plus(kodkl);
repl.plus(" ");
repl.plus(naimkl);

char procent[20];
memset(procent,'\0',sizeof(procent));
if(iceb_menu_vvod1(repl.ravno(),procent,sizeof(procent),wpredok) != 0)
   return(1);

time_t vrem;
time(&vrem);

sprintf(strsql,"update Taxiklient \
set \
ktoi=%d,\
vrem=%ld,\
ps=%.2f \
where kod='%s'",
iceb_getuid(wpredok),
vrem,
atof(procent),kodkl);

if(iceb_sql_zapis(strsql,0,0,wpredok) != 0)
 return(1);


return(0);
}
/*************************************/
/*Блокировка неактивных карточек*/
/***********************************/
int l_klientr_bk(GtkWidget *wpredok)
{


class iceb_u_str denmesgod("");
class iceb_u_str kodgr("");

if(l_klientr_bl(&denmesgod,&kodgr,wpredok) != 0)
 return(1);

short den=0,mes=0,god=0;
if(iceb_u_rsdat(&den,&mes,&god,denmesgod.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата!"),wpredok);
  return(1);
 }

if(iceb_parol(0,wpredok) != 0)
 return(1);

class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);

int kolstr=0;

class SQLCURSOR cur;
SQL_str row;
char strsql[500];
sprintf(strsql,"select kod,grup from Taxiklient where mb=0");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

int kolih_zk=0; //количество заблокированных карточек
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);

  if(iceb_u_proverka(kodgr.ravno(),row[1],0,0) == 0)
   continue;

  sprintf(strsql,"select dv from Restvv where kk='%s' and dv >= '%04d-%02d-%02d 00:00:00' limit 1",row[0],god,mes,den);

  if(iceb_sql_readkey(strsql,wpredok) != 0)
   continue;

  //Проверяем был ли хоть один вход/выход. Если карточка не использовалась, блокировать её не надо
  sprintf(strsql,"select dv from Restvv where kk='%s' limit 1",row[0]);

  if(iceb_sql_readkey(strsql,wpredok) == 0) //новая карточка
   continue;
  

  kolih_zk++;
  sprintf(strsql,"update Taxiklient set mb=1 where kod='%s'",row[0]);

  iceb_sql_zapis(strsql,1,0,wpredok);    
 }

gdite.close();
sprintf(strsql,"%s:%d",gettext("Количество заблокированых карточек"),kolih_zk);
iceb_menu_soob(strsql,wpredok);

return(0);
}

/***********************************/
/*Импорт карточек из файла*/
/********************************/

void l_klient_impk(GtkWidget *wpredok)
{

static iceb_u_str imaf("");

class iceb_u_str naim;
naim.plus(gettext("Импорт карточек из файла"));
 
if(iceb_menu_import(&imaf,naim.ravno(),"",wpredok) != 0)
 return;

l_klient_imp(imaf.ravno(),wpredok);




}
