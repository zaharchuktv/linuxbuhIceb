/*$Id: l_srestdok.c,v 1.54 2011-02-21 07:36:20 sasa Exp $*/
/*08.11.2007	25.02.2004	Белых А.И.	l_srestdok.c
Работа с архивом документов
*/
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "i_rest.h"
#include "l_srestdok.h"
enum
{
  FK2,
//  FK3,
  FK4,
  FK5,
  FK10,
  KOL_F_KL
};

enum
{
 COL_DATAZ,
 COL_SUMA,
 COL_DATA,
 COL_NOMD,
 COL_PODR,
 COL_METKA,
 COL_STOL,
 COL_KODKL,
 COL_FAMIL,
 COL_KOMENT,
 COL_DATAO,
 COL_SPISANO,
 COL_VREM,
 COL_KTO,
 NUM_COLUMNS
};

class  srestdok_data
 {
  public:

  short dv,mv,gv; //выбранная дата
  iceb_u_str nomdokv; //выбранный номер документа
  int        nomervsp; //Номер записи в списке

  srestdok_r_data rk; //Реквизиты поиска документов
  
  GtkWidget *hboxradio;
  GtkWidget *radiobutton0;
  GtkWidget *radiobutton1;
  GtkWidget *radiobutton2;
  GtkWidget *radiobutton3;

  GtkWidget *radiobutton_op[2];

  short     metkarr;
  int metka_operatora; //0-показать все записи 1-показать записи выписанные работающим сейчас оператором
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *pwindow;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  short     metkazapisi; //0-новая запись 1-корректировка
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  
  //Конструктор
  srestdok_data()
   {
    metka_operatora=0;
    metkarr=0;    
    snanomer=0;
    kl_shift=metkazapisi=0;
//    vwindow=
    pwindow=window=treeview=NULL;
    rk.clear_data();
   }      



};

void srestdok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class srestdok_data *data);
gboolean   srestdok_key_press(GtkWidget *widget,GdkEventKey *event,class srestdok_data *data);
void  srestdok_knopka(GtkWidget *widget,class srestdok_data *data);
void srestdok_add_columns(GtkTreeView *treeview);
void srestdok_vibor(GtkTreeSelection *selection,class srestdok_data *data);
void l_srestdok_p(class srestdok_r_data *data,GtkWidget*);
int srestdok_prov_row(SQL_str row,class srestdok_data *data);
void srestdok_zapros(char *strsql,class srestdok_data *data);
void srestdok_ras(class srestdok_data *data);
void srestdok_create_list (class srestdok_data *data);

void       srestdok_radio0(GtkWidget *,class srestdok_data *);
void       srestdok_radio1(GtkWidget *,class srestdok_data *);
void       srestdok_radio2(GtkWidget *,class srestdok_data *);
void       srestdok_radio3(GtkWidget *,class srestdok_data *);

void  srestdok_radio_op0(GtkWidget *widget,class srestdok_data *data);
void  srestdok_radio_op1(GtkWidget *widget,class srestdok_data *data);

extern char *organ;
extern int kodpodr;
extern iceb_u_str naimpodr;
extern SQL_baza	bd;
extern char *name_system;
extern uid_t kod_operatora; /*Код работающего оператора*/


void  l_srestdok(GtkWidget *wpredok)
{
srestdok_data data;
char strsql[300];
GdkColor color;
SQLCURSOR cur;
//SQL_str   row;
time_t    vrem;
struct tm *bf;

time(&vrem);
bf=localtime(&vrem);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(strsql,"%s %s",name_system,gettext("Список счетов"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);


if(wpredok != NULL)
 {
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(srestdok_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Список счетов"));
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
gtk_widget_set_usize(GTK_WIDGET(data.sw),600,400);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

gtk_widget_show(data.sw);

//Вставляем радиокнопки
data.hboxradio = gtk_hbox_new(FALSE, 0);
gtk_box_pack_start(GTK_BOX(vbox2), data.hboxradio, FALSE, TRUE, 0);

GSList *group;

data.radiobutton0=gtk_radio_button_new_with_label(NULL,gettext("Неоплаченные"));
gtk_box_pack_start (GTK_BOX (data.hboxradio),data.radiobutton0, TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton0), "clicked",GTK_SIGNAL_FUNC(srestdok_radio0),&data);
//gtk_object_set_user_data(GTK_OBJECT(data.radiobutton0),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton0));


sprintf(strsql,"%s %d.%d.%d",
gettext("За"),
bf->tm_mday,
bf->tm_mon+1,
bf->tm_year+1900);

data.radiobutton1=gtk_radio_button_new_with_label(group,strsql);
gtk_box_pack_start (GTK_BOX (data.hboxradio),data.radiobutton1, TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton1), "clicked",GTK_SIGNAL_FUNC(srestdok_radio1),&data);

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton1));

data.radiobutton2=gtk_radio_button_new_with_label(group,gettext("Все"));
gtk_box_pack_start (GTK_BOX (data.hboxradio),data.radiobutton2, TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton2), "clicked",GTK_SIGNAL_FUNC(srestdok_radio2),&data);
if(kodpodr != 0)
 {
  group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton2));

  data.radiobutton3=gtk_radio_button_new_with_label(group,gettext("Все неоплаченные"));
  //gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton3),TRUE); //Устанавливем активной кнопку
  data.metkarr=3;
  gtk_signal_connect(GTK_OBJECT(data.radiobutton3), "clicked",GTK_SIGNAL_FUNC(srestdok_radio3),&data);
  gtk_box_pack_start (GTK_BOX (data.hboxradio),data.radiobutton3, TRUE, TRUE, 0);
 }

data.metkarr=0; //устанавливается именно в конце радиокнопок
 
GtkWidget *separator=gtk_vseparator_new();
gtk_box_pack_start (GTK_BOX (data.hboxradio),separator, TRUE, TRUE, 2);


GSList *group_op;

data.radiobutton_op[0]=gtk_radio_button_new_with_label(NULL,gettext("Все"));
gtk_box_pack_start (GTK_BOX (data.hboxradio),data.radiobutton_op[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton_op[0]), "clicked",GTK_SIGNAL_FUNC(srestdok_radio_op0),&data);

group_op=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton_op[0]));

sprintf(strsql,"%s %d",gettext("Оператор"),kod_operatora);
data.radiobutton_op[1]=gtk_radio_button_new_with_label(group_op,strsql);
gtk_box_pack_start (GTK_BOX (data.hboxradio),data.radiobutton_op[1], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton_op[1]), "clicked",GTK_SIGNAL_FUNC(srestdok_radio_op1),&data);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton_op[data.metka_operatora]),TRUE); //Устанавливем активной кнопку


gtk_widget_show_all(data.hboxradio);



//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Просмотр"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(srestdok_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Просмотр выбранного документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

/*******************************
sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[FK3]),FALSE);//Недоступна
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(srestdok_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удаление выбранной запси"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);
***************************/

sprintf(strsql,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(srestdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(srestdok_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(srestdok_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

srestdok_create_list(&data);

gtk_widget_show(data.window);
//gtk_widget_show_all(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();

//printf("l_srestdok end\n");


}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void srestdok_create_list (class srestdok_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
char strsql[300];
int  kolstr=0;
SQL_str row;
SQLCURSOR cur;
SQL_str row1;
SQLCURSOR cur1;
short d,m,g;

//printf("srestdok_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(srestdok_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(srestdok_vibor),data);

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
memset(strsql,'\0',sizeof(strsql));

srestdok_zapros(strsql,data);
printf("strsql=%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

//gtk_list_store_clear(model);


iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
float kolstr1=0.;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  /*printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);*/
  
  if(srestdok_prov_row(row,data) != 0)
    continue;
  
  //дата выписки счёта
  iceb_u_rsdat(&d,&m,&g,row[2],2);
  sprintf(strsql,"%02d.%02d.%d",d,m,g);
  ss[COL_DATA].new_plus(strsql);

  /*сумма по документу*/
  ss[COL_SUMA].new_plus(sumapsh(g,row[1],"",data->window));
  
  //номер документа
  ss[COL_NOMD].new_plus(iceb_u_toutf(row[1]));

  //метка
  if(atoi(row[3]) == 0)
     ss[COL_METKA].new_plus("*");
  else
     ss[COL_METKA].new_plus(" ");
  
  //Номер стола
  ss[COL_STOL].new_plus(iceb_u_toutf(row[4]));

  //Код клиента
  ss[COL_KODKL].new_plus(iceb_u_toutf(row[5]));

  //фамилия
  ss[COL_FAMIL].new_plus("");
  if(row[6][0] != '\0')
   ss[COL_FAMIL].new_plus(iceb_u_toutf(row[6]));
  else
   {
    //Узнаем в какой группе клиент
    sprintf(strsql,"select grup from Taxiklient where kod='%s'",row[5]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
      sprintf(strsql,"select naik from Grupklient where kod='%s'",row1[0]);
      if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
       ss[COL_FAMIL].new_plus(iceb_u_toutf(row1[0]));
     }
   }
  //коментарий
  ss[COL_KOMENT].new_plus(iceb_u_toutf(row[7]));

  //Дата и время записи
  ss[COL_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[10])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_u_kszap(row[9],0));

  //Дата и время заказа
  ss[COL_DATAZ].new_plus(iceb_u_toutf(iceb_u_vremzap(row[12])));

  //Дата и время оплаты
  ss[COL_DATAO].new_plus(iceb_u_toutf(iceb_u_vremzap(row[13])));
  
  //Метка списения документа
  if(atoi(row[8]) == 1)
   ss[COL_SPISANO].new_plus(gettext("Списано"));
  else
   ss[COL_SPISANO].new_plus("");
   
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_DATAZ,ss[COL_DATAZ].ravno(),
  COL_SUMA,ss[COL_SUMA].ravno(),
  COL_DATA,ss[COL_DATA].ravno(),
  COL_NOMD,ss[COL_NOMD].ravno(),
  COL_PODR,row[11],
  COL_METKA,ss[COL_METKA].ravno(),
  COL_STOL,ss[COL_STOL].ravno(),
  COL_KODKL,ss[COL_KODKL].ravno(),
  COL_FAMIL,ss[COL_FAMIL].ravno(),
  COL_KOMENT,ss[COL_KOMENT].ravno(),
  COL_DATAO,ss[COL_DATAO].ravno(),
  COL_SPISANO,ss[COL_SPISANO].ravno(),
  COL_VREM,ss[COL_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS, data->kolzap,
               -1);

  data->kolzap++;

 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

srestdok_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
 }


gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Список счетов"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

if(kodpodr != 0 && data->rk.metkapoi == 0)
 {
  sprintf(strsql,"%s: %d %s",
  gettext("Подразделение"),kodpodr,naimpodr.ravno());
  zagolov.ps_plus(strsql);
 }

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

gtk_widget_show(data->label_kolstr);

if(data->rk.metkapoi == 1)
 {
  iceb_u_str spis;
  sprintf(strsql,"%s !!!",gettext("Поиск"));
  spis.plus(strsql);

  if(data->rk.datan.ravno()[0] != '\0')
   {
    sprintf(strsql,"%s: %s",gettext("Дата начала"),data->rk.datan.ravno());
    spis.ps_plus(strsql);
   }
  if(data->rk.datak.ravno()[0] != '\0')
   {
    sprintf(strsql,"%s: %s",gettext("Дата конца"),data->rk.datak.ravno());
    spis.ps_plus(strsql);
   }
  if(data->rk.nomdokp.ravno()[0] != '\0')
   {
    sprintf(strsql,"%s: %s",gettext("Номер документа"),data->rk.nomdokp.ravno());
    spis.ps_plus(strsql);
   }

  if(data->rk.nomstol.getdlinna() > 1)
   {
    sprintf(strsql,"%s: %s",gettext("Номер столика"),data->rk.nomstol.ravno());
    spis.ps_plus(strsql);
   }    

  if(data->rk.fio.getdlinna() > 1)
   {
    sprintf(strsql,"%s: %s",gettext("Фамилия клиента"),data->rk.fio.ravno());
    spis.ps_plus(strsql);
   }    

  if(data->rk.kodkl.getdlinna() > 1)
   {
    sprintf(strsql,"%s: %s",gettext("Код клиента"),data->rk.kodkl.ravno());
    spis.ps_plus(strsql);
   }    

  if(data->rk.koment.getdlinna() > 1)
   {
    sprintf(strsql,"%s: %s",gettext("Коментарий"),data->rk.koment.ravno());
    spis.ps_plus(strsql);
   }    

  if(data->rk.podr.getdlinna() > 1)
   {
    sprintf(strsql,"%s: %s",gettext("Подразделение"),data->rk.podr.ravno());
    spis.ps_plus(strsql);
   }    

  gtk_label_set_text(GTK_LABEL(data->label_poisk),spis.ravno_toutf());
      
  gtk_widget_show(data->label_poisk); //Показываем
  gtk_widget_hide(data->hboxradio); //Скрываем

 }
else
 {
  gtk_widget_hide(data->label_poisk); //Скрываем
  gtk_widget_show_all(data->hboxradio); //Показываем
 }

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));


}

/*****************/
/*Создаем колонки*/
/*****************/

void srestdok_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

//printf("srestdok_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата заказа"), renderer,"text", COL_DATAZ,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Сумма"), renderer,"text", COL_SUMA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата док-та"), renderer,"text", COL_DATA,NULL);

renderer = gtk_cell_renderer_text_new ();
//g_object_set(G_OBJECT(renderer),"foreground","white",NULL);
//g_object_set(G_OBJECT(renderer),"background","black",NULL);


gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,\
gettext("Н/д"), renderer,"text", COL_NOMD,NULL);

renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("К/п"), renderer,"text", COL_PODR,NULL);

renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
"M", renderer,"text", COL_METKA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Н/с"), renderer,
"text", COL_STOL,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("К/к"), renderer,
"text", COL_KODKL,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Фамилия клиента"), renderer,
"text", COL_FAMIL,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Коментарий"), renderer,"text", COL_KOMENT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата оплати"), renderer,"text", COL_DATAO,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Метка списания"), renderer,"text", COL_SPISANO,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,"text", COL_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,"text", COL_KTO,NULL);

//printf("srestdok_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void srestdok_vibor(GtkTreeSelection *selection,class srestdok_data *data)
{
//printf("srestdok_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *datd;
gchar *nomd;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_DATA,&datd,COL_NOMD,&nomd,NUM_COLUMNS,&nomer,-1);

iceb_u_rsdat(&data->dv,&data->mv,&data->gv,datd,1);

data->nomdokv.new_plus(iceb_u_fromutf(nomd));
data->snanomer=data->nomervsp=nomer;

g_free(datd);
g_free(nomd);

//printf("%d.%d.%d %s %d\n",data->dv,data->mv,data->gv,data->nomdokv.ravno(),nomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  srestdok_knopka(GtkWidget *widget,class srestdok_data *data)
{
//iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("srestdok_knopka knop=%d\n",knop);
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch ((gint)knop)
 {
  case FK2:
    if(data->kolzap == 0)
      return;
    printf("srestdok_knopka F2 metkazapisi=%d\n",data->metkazapisi);
    int i;
    if((i=vip_shet(data->gv,&data->nomdokv,data->window)) != 0)
      srestdok_create_list(data);
    return;  

  case FK4:
    l_srestdok_p(&data->rk,data->window);
    srestdok_create_list(data);
    return;  

  case FK5:
    if(data->kolzap == 0)
      return;  
    srestdok_ras(data);
    return;  

    
  case FK10:
//    printf("srestdok_knopka F10\n");
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   srestdok_key_press(GtkWidget *widget,GdkEventKey *event,class srestdok_data *data)
{
iceb_u_str repl;
printf("srestdok_key_press keyval=%d state=%d\n",
event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);
   
  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
    return(TRUE);

  case GDK_F5:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
    return(TRUE);

  
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(TRUE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("srestdok_key_press-Нажата клавиша Shift\n");

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
void srestdok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class srestdok_data *data)
{
printf("srestdok_v_row\n");
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
}

/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  srestdok_radio0(GtkWidget *widget,class srestdok_data *data)
{
//g_print("srestdok_radio0\n");
/*
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("srestdok_radio knop=%s\n",knop);

data->metkarr=atoi(knop);
*/
if(data->metkarr == 0)
  return;
data->metkarr=0;

srestdok_create_list(data);

}
void  srestdok_radio1(GtkWidget *widget,class srestdok_data *data)
{
//g_print("srestdok_radio1\n");
if(data->metkarr == 1)
  return;
data->metkarr=1;
srestdok_create_list(data);
}

void  srestdok_radio2(GtkWidget *widget,class srestdok_data *data)
{
//g_print("srestdok_radio2\n");
if(data->metkarr == 2)
  return;

data->metkarr=2;
//printf("data->metkarr=%d\n",data->metkarr);
srestdok_create_list(data);

}
void  srestdok_radio3(GtkWidget *widget,class srestdok_data *data)
{
//g_print("srestdok_radio3\n");
if(data->metkarr == 3)
  return;

data->metkarr=3;
//printf("data->metkarr=%d\n",data->metkarr);
srestdok_create_list(data);

}

void  srestdok_radio_op0(GtkWidget *widget,class srestdok_data *data)
{
if(data->metka_operatora == 0)
  return;
data->metka_operatora=0;

srestdok_create_list(data);

}


void  srestdok_radio_op1(GtkWidget *widget,class srestdok_data *data)
{
if(data->metka_operatora == 1)
  return;
data->metka_operatora=1;

srestdok_create_list(data);

}

/*****************************/
/*Проверка на реквизиты поиска*/
/******************************/
int srestdok_prov_row(SQL_str row,class srestdok_data *data)
{
if(data->rk.metkapoi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->rk.nomdokp.ravno(),row[1],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->rk.podr.ravno(),row[11],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->rk.nomstol.ravno(),row[4],0,0) != 0)
 return(1);

if(iceb_u_proverka(data->rk.kodkl.ravno(),row[5],0,0) != 0)
 return(1);

//Поиск образца в строке
if(data->rk.fio.ravno()[0] != '\0' && iceb_u_strstrm(row[6],data->rk.fio.ravno()) == 0)
 return(1);

//Поиск образца в строке
if(data->rk.koment.ravno()[0] != '\0' && iceb_u_strstrm(row[7],data->rk.koment.ravno()) == 0)
 return(1);

return(0);

}

/****************************/
/*формирование запроса*/
/************************/
void srestdok_zapros(char *strsql,class srestdok_data *data)
{

if(data->rk.metkapoi == 0)
 {
  if(data->metkarr == 0)
   {
    if(kodpodr != 0)
     sprintf(strsql,"select * from Restdok where mo=0 and podr=%d",
     kodpodr);
    else
     sprintf(strsql,"select * from Restdok where mo=0");
    
   }
  if(data->metkarr == 1)
   {
    time_t    vrem;
    struct tm *bf;

    time(&vrem);
    bf=localtime(&vrem);

    if(kodpodr == 0)
     sprintf(strsql,"select * from Restdok where datd='%d-%d-%d'",
     bf->tm_year+1900,bf->tm_mon+1,bf->tm_mday);
    else
     sprintf(strsql,"select * from Restdok where datd='%d-%d-%d' and podr=%d",
     bf->tm_year+1900,bf->tm_mon+1,bf->tm_mday,kodpodr);
   }

  if(data->metkarr == 2)
    sprintf(strsql,"select * from Restdok");


  if(data->metkarr == 3)
   {
    sprintf(strsql,"select * from Restdok where mo=0");
   }
 
 }
else
 {
  if(data->rk.datan.getdlinna() > 1 && data->rk.datak.getdlinna() <= 1)
   sprintf(strsql,"select * from Restdok where datd >= '%s'",
   data->rk.datan.ravno_sqldata());

  if(data->rk.datan.getdlinna() <= 1 && data->rk.datak.getdlinna() > 1)
   sprintf(strsql,"select * from Restdok where datd <= '%s'",
   data->rk.datak.ravno_sqldata());

  if(data->rk.datan.getdlinna() > 1 && data->rk.datak.getdlinna() > 1)
   sprintf(strsql,"select * from Restdok where datd >= '%s' \
and datd <= '%s'",
   data->rk.datan.ravno_sqldata(),data->rk.datak.ravno_sqldata());

  if(data->rk.datan.getdlinna() <= 1 && data->rk.datak.getdlinna() <= 1)
   sprintf(strsql,"select * from Restdok");

 }
//printf("srestdok_zapros =%s /metkapoi=%d/metkarr=%d\n",strsql,data->rk.metkapoi,data->metkarr);
if(data->metka_operatora == 1)
 {
  char bros[100];
  if(iceb_u_strstrm(strsql,"where") == 1)
    sprintf(bros," and ktoi=%d",kod_operatora);
  else
    sprintf(bros," where ktoi=%d",kod_operatora);
  
  strcat(strsql,bros);
 } 
strcat(strsql," order by vremz desc");

}
/***********************/
/*Распечатка*/
/**********************/
void srestdok_ras(class srestdok_data *data)
{
gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
char strsql[300];
int  kolstr;
SQLCURSOR cur;
SQL_str   row;

srestdok_zapros(strsql,data);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
short dn,mn,gn;
short dk,mk,gk;
iceb_u_rsdat(&dn,&mn,&gn,data->rk.datan.ravno(),1);
iceb_u_rsdat(&dk,&mk,&gk,data->rk.datak.ravno(),1);

FILE *ff;
char imaf[30];
sprintf(imaf,"sheta%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }
iceb_u_startfil(ff);

iceb_u_zagolov(gettext("Список cчетов"),dn,mn,gn,dk,mk,gk,organ,ff);

if(data->rk.nomdokp.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Номер документа"),data->rk.nomdokp.ravno());
if(data->rk.kodkl.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Код клиента"),data->rk.kodkl.ravno());
if(data->rk.nomstol.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Номер столика"),data->rk.nomstol.ravno());
if(data->rk.fio.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Фамилия клиента"),data->rk.fio.ravno());
if(data->rk.koment.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Коментарий"),data->rk.koment.ravno());
if(data->rk.podr.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Подразделение"),data->rk.podr.ravno());
 

fprintf(ff,"\
--------------------------------------------------------------------\n");
fprintf(ff,"\
  Дата    |К/п|Номер док.|О|Код кл.|   Фамилия          |  Сумма   |\n");

fprintf(ff,"\
--------------------------------------------------------------------\n");

double sumadok=0.;
double itogo=0.;
short d,m,g;
char  metkaopl[2];
while(cur.read_cursor(&row) != 0)
 {
  if(srestdok_prov_row(row,data) != 0)
    continue;

  iceb_u_rsdat(&d,&m,&g,row[2],2);
  sumadok=sumapsh(g,row[1],"",data->window);
  itogo+=sumadok;
  memset(metkaopl,'\0',sizeof(metkaopl));
  if(atoi(row[3]) == 0)
    metkaopl[0]='*';
  else
    metkaopl[0]=' ';
      
  fprintf(ff,"%02d.%02d.%02d %-3s %-10s %s %-7s %-20.20s %10.2f\n",
  d,m,g,row[11],row[1],metkaopl,row[5],row[6],sumadok);
  
 }

fprintf(ff,"\
--------------------------------------------------------------------\n");
fprintf(ff,"%56s:%10.2f\n",gettext("Итого"),itogo);

fprintf(ff,"* %s\n",gettext("Звёздочкой отмечены неоплаченные счета"));

iceb_podpis(ff,data->window);
fclose(ff);

iceb_u_spisok fil;
iceb_u_spisok nazv;

fil.plus(imaf);
nazv.plus(gettext("Список cчетов"));

//printf("rasklient-fine\n");

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));

iceb_rabfil(&fil,&nazv,"",0,data->window);


}
