/*$Id: l_otrvr.c,v 1.17 2011-02-21 07:35:53 sasa Exp $*/
/*05.05.2009	19.09.2006	Белых А.И.	l_otrvr.c
Ввод и корректировка отработанного времени для всех табельных номеров
*/

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "buhg_g.h"
#include "l_otrvr.h"

enum
{
 FK2,
 SFK2,
 FK3,
 FK4,
 FK5,
 FK7,
 FK8,
 FK10,
 KOL_F_KL
};

enum
{
 COL_TABNOM,
 COL_FIO,
 COL_KOD,
 COL_NAIM,
 COL_R_DNI,
 COL_HASI,
 COL_K_DNI,
 COL_KOLIH_R_DNEI,
 COL_HAS_V_DEN,
 COL_DATAN,
 COL_DATAK,
 COL_KOMENT,
 COL_DATA_VREM,
 COL_KTO,  
//Не выводится на экран
 COL_NOM_ZAP,
 NUM_COLUMNS
};

class  l_otrvr_data
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

  class iceb_u_str zapros;
  class l_otrvr_rek rk;
  
  class iceb_u_str tabnom_tv; //Табельный номер только-что введённый
  class iceb_u_str kodtv;//Код толькочто введённой записи
    
  class iceb_u_str tabnomv; //Выбранный табельный номер
  class iceb_u_str kodv; //Выбранный код 
  int nom_zap;           //Номер только что выбранной записи
    
  short mp,gp;
  short metka_poi;  //0-без поиска 1-с поиском
  //Конструктор
  l_otrvr_data()
   {
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    nom_zap=0;
    metka_poi=0;
   }      
 };

gboolean   l_otrvr_key_press(GtkWidget *widget,GdkEventKey *event,class l_otrvr_data *data);
void l_otrvr_vibor(GtkTreeSelection *selection,class l_otrvr_data *data);
void l_otrvr_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class l_otrvr_data *data);
void  l_otrvr_knopka(GtkWidget *widget,class l_otrvr_data *data);
void l_otrvr_add_columns (GtkTreeView *treeview);
void l_otrvr_create_list(class l_otrvr_data *data);

void l_otrvr_rasp(class l_otrvr_data *data);
void l_otrvr_udzap(class l_otrvr_data *data);
int l_otrvr_v(class iceb_u_str *tabnom,short mp,short gp,class iceb_u_str *vid_tab,int nom_zap,GtkWidget *wpredok);
int l_otrvr_p(  class l_otrvr_rek *poi,GtkWidget *wpredok);
int l_otrvr_prov(SQL_str row,class l_otrvr_data *data);
void impotvrw(GtkWidget *wpredok);
int l_otrvr_zamkod(class l_otrvr_data *data);
int l_otrvr_zk(class iceb_u_str *skod,class iceb_u_str *nkod,GtkWidget *wpredok);

extern SQL_baza	bd;
extern char *name_system;
extern char     *organ;
 
void l_otrvr(short mp,short gp,GtkWidget *wpredok)
{
l_otrvr_data data;
char bros[512];
GdkColor color;


data.mp=mp;
data.gp=gp;


data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,gettext("Ввод и корректировка отработанного времени"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_otrvr_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Ввод и корректировка отработанного времени"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");

gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_poisk,GTK_STATE_NORMAL,&color);

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),-1,200);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_widget_show (data.sw);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];


sprintf(bros,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(l_otrvr_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(l_otrvr_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(l_otrvr_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удалить выбранную запись"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(l_otrvr_knopka),&data);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(l_otrvr_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F7 %s",gettext("Импорт"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(l_otrvr_knopka),&data);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Импорт отработанного времени"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"F8 %s",gettext("Замена кода"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]), "clicked",GTK_SIGNAL_FUNC(l_otrvr_knopka),&data);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Замена одного кода на другой"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_widget_show(data.knopka[FK8]);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(l_otrvr_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_otrvr_create_list(&data);
gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


//printf("l_otrvr end\n");
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_otrvr_knopka(GtkWidget *widget,class l_otrvr_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_otrvr_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case FK2:
    data->tabnom_tv.new_plus("");
    data->kodtv.new_plus("");
    if(l_otrvr_v(&data->tabnom_tv,data->mp,data->gp,&data->kodtv,data->nom_zap,data->window) == 0)
      l_otrvr_create_list(data);

    return;  

  case SFK2:

    if(l_otrvr_v(&data->tabnomv,data->mp,data->gp,&data->kodv,data->nom_zap,data->window) == 0)
     { 
      data->tabnom_tv.new_plus(data->tabnomv.ravno());
      l_otrvr_create_list(data);
     }  
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    l_otrvr_udzap(data);
    l_otrvr_create_list(data);
    return;  

  case FK4:

    if(l_otrvr_p(&data->rk,data->window) == 0)
     data->metka_poi=1;
    else
     data->metka_poi=0;
     
    l_otrvr_create_list(data);
    return;  


  case FK5:
    l_otrvr_rasp(data);
    return;  

  case FK7:
    impotvrw(data->window);
    l_otrvr_create_list(data);
   return;  

  case FK8:
    if(l_otrvr_zamkod(data) == 0)
      l_otrvr_create_list(data);
   return;  

    
  case FK10:
//    printf("l_otrvr_knopka F10\n");
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_otrvr_key_press(GtkWidget *widget,GdkEventKey *event,class l_otrvr_data *data)
{
//printf("l_otrvr_key_press keyval=%d state=%d\n",event->keyval,event->state);

switch(event->keyval)
 {
  case GDK_plus: //это почемуто не работает Нажата клавиша плюс на основной клавиатуре
  case GDK_KP_Add: //Нажата клавиша "+" на дополнительной клавиатуре
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

  case GDK_F7:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK7]),"clicked");
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
    printf("l_otrvr_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_otrvr_create_list (class l_otrvr_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
int  kolstr=0;
SQL_str row,row1;
iceb_u_str zagolov;

//printf("l_otrvr_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_otrvr_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_otrvr_vibor),data);

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
G_TYPE_INT,
G_TYPE_INT);

sprintf(strsql,"select tabn,kodt,dnei,has,kdnei,datn,datk,ktoz,vrem,kolrd,kolrh,kom,nomz from Ztab \
where god=%d and mes=%d order by tabn asc",data->gp,data->mp);

data->zapros.new_plus(strsql);

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
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(l_otrvr_prov(row,data) != 0)
   continue;
     
  if(iceb_u_SRAV(data->kodtv.ravno(),row[1],0) == 0)
   if(iceb_u_SRAV(data->tabnomv.ravno(),row[0],0) == 0)
    data->snanomer=data->kolzap;

  //Табельный номер
  ss[COL_TABNOM].new_plus(iceb_u_toutf(row[0]));
  
  //фамилия
  ss[COL_FIO].new_plus("");
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   ss[COL_FIO].new_plus(iceb_u_toutf(row1[0]));

  //Код 
  ss[COL_KOD].new_plus(iceb_u_toutf(row[1]));

  //Наименование
  ss[COL_NAIM].new_plus("");
  sprintf(strsql,"select naik from Tabel where kod=%s",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
    ss[COL_NAIM].new_plus(iceb_u_toutf(row1[0]));

  //Количество отработанных рабочих дней
  sprintf(strsql,"%.10g",atof(row[2]));
  ss[COL_R_DNI].new_plus(strsql);

  //Количество отработанных часов
  sprintf(strsql,"%.10g",atof(row[3]));
  ss[COL_HASI].new_plus(strsql);

  //Количество отработанных календарных дней
  sprintf(strsql,"%.10g",atof(row[4]));
  ss[COL_K_DNI].new_plus(strsql);

  //Количество рабочих дней в месяце
  sprintf(strsql,"%.10g",atof(row[9]));
  ss[COL_KOLIH_R_DNEI].new_plus(strsql);

  //Количество рабочих часов в день
  sprintf(strsql,"%.10g",atof(row[10]));
  ss[COL_HAS_V_DEN].new_plus(strsql);

  //Дата начала
  ss[COL_DATAN].new_plus(iceb_u_datzap(row[5]));

  //Дата конца
  ss[COL_DATAK].new_plus(iceb_u_datzap(row[6]));

  //Коментарий
  ss[COL_KOMENT].new_plus(iceb_u_toutf(row[11]));

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[8])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[7],0,data->window));



  gtk_list_store_append (model, &iter);


  gtk_list_store_set (model, &iter,
  COL_TABNOM,ss[COL_TABNOM].ravno(),
  COL_FIO,ss[COL_FIO].ravno(),
  COL_KOD,ss[COL_KOD].ravno(),
  COL_NAIM,ss[COL_NAIM].ravno(),
  COL_R_DNI,ss[COL_R_DNI].ravno(),
  COL_HASI,ss[COL_HASI].ravno(),
  COL_K_DNI,ss[COL_K_DNI].ravno(),
  COL_KOLIH_R_DNEI,ss[COL_KOLIH_R_DNEI].ravno(),
  COL_HAS_V_DEN,ss[COL_HAS_V_DEN].ravno(),
  COL_DATAN,ss[COL_DATAN].ravno(),
  COL_DATAK,ss[COL_DATAK].ravno(),
  COL_KOMENT,ss[COL_KOMENT].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->kodtv.new_plus("");
gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_otrvr_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

if(data->metka_poi == 1 )
 {

// printf("Формирование заголовка с реквизитами поиска.\n");
  iceb_u_str strpoi;  

  strpoi.new_plus(gettext("Поиск"));
  strpoi.plus(" !!!");

  iceb_str_poisk(&strpoi,data->rk.tabnom.ravno(),gettext("Табельный номер"));
  iceb_str_poisk(&strpoi,data->rk.kod_tab.ravno(),gettext("Код табеля"));
  iceb_str_poisk(&strpoi,data->rk.podr.ravno(),gettext("Подразделение"));
  iceb_str_poisk(&strpoi,data->rk.koment.ravno(),gettext("Коментарий"));

  gtk_label_set_text(GTK_LABEL(data->label_poisk),strpoi.ravno_toutf());
  gtk_widget_show(data->label_poisk);
 }
else
 {
  gtk_widget_hide(data->label_poisk); 
 }

zagolov.new_plus(gettext("Ввод и корректировка отработанного времени"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

zagolov.ps_plus(gettext("Дата"));
zagolov.plus(":");
zagolov.plus(data->mp);
zagolov.plus(".");
zagolov.plus(data->gp);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));

}
/*****************/
/*Создаем колонки*/
/*****************/

void l_otrvr_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Т/н"), renderer,"text", COL_TABNOM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Фамилия Имя Отчество"), renderer,"text", COL_FIO,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_KOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование вида табеля"), renderer,"text", COL_NAIM,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дни"), renderer,"text", COL_R_DNI,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Часы"), renderer,"text", COL_HASI,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Календарные\nдни"), renderer,"text", COL_K_DNI,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Кол.раб.\nдней в месяце"), renderer,"text", COL_KOLIH_R_DNEI,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Кол.раб.\nчасов в дне"), renderer,"text", COL_HAS_V_DEN,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата\nначала"), renderer,"text", COL_DATAN,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата\nконца"), renderer,"text", COL_DATAK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Коментарий"), renderer,"text", COL_KOMENT,NULL);

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
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void l_otrvr_vibor(GtkTreeSelection *selection,class l_otrvr_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *tabnom;
gchar *kod;
gint  nomer;
gint  nom_zap;

gtk_tree_model_get(model,&iter,COL_TABNOM,&tabnom,COL_KOD,&kod,COL_NOM_ZAP,&nom_zap,NUM_COLUMNS,&nomer,-1);

data->tabnomv.new_plus(tabnom);
data->kodv.new_plus(iceb_u_fromutf(kod));
data->snanomer=nomer;
data->nom_zap=nom_zap;

g_free(kod);
g_free(tabnom);

//printf("l_otrvr_vibor-%s %d\n",data->kodv.ravno(),data->snanomer);

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void l_otrvr_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_otrvr_data *data)
{
//printf("l_otrvr_v_row\n");
//printf("l_otrvr_v_row корректировка\n");

gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");


}
/*****************************/
/*Удаление записи            */
/*****************************/

void l_otrvr_udzap(class l_otrvr_data *data)
{

if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
 return;

char strsql[512];

sprintf(strsql,"delete from Ztab where tabn=%d and god=%d and mes=%d and kodt=%d and nomz=%d",
data->tabnomv.ravno_atoi(),data->gp,data->mp,
data->kodv.ravno_atoi(),data->nom_zap);

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;

}
/**********************************/
/*Распечатать список*/
/****************************/

void l_otrvr_rasp(class l_otrvr_data *data)
{
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

sprintf(strsql,"l_otrvr%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Отработанное время"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_u_zagolov(gettext("Отработанное время"),0,0,0,0,0,0,organ,ff);

fprintf(ff,"%s:%02d.%d\n",gettext("Дата"),data->mp,data->gp);

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
      |                    |Код |                    |Колич.|Колич.|Колич.|Колич.|Колич.|  Дата    |   Дата   |                   |\n\
 Т/н  |Фамилия Имя Отчество|табе|  Наименование      |отраб.|отраб.|кален.|рабоч.|рабоч.| начала   |  конца   |Дата и время запис.| Кто записал\n\
      |                    | ля |    табеля          |дней  |часов |дней  |дней в|часов |          |          |                   |\n\
      |                    |    |                    |      |      |      |месяце|в дне |          |          |                   |\n"));
//                                                    123456 123456 123456 123456 123456 1234567890 1234567890
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------------------\n");

iceb_u_str s5;
iceb_u_str s6;
SQL_str row1;
class SQLCURSOR cur1;

char naim[512];
char datan[20];
char datak[20];
float kol_otr_dnei;
float kol_otr_has;
float kol_kal_dnei;
float i_kol_otr_dnei=0.;
float i_kol_otr_has=0.;
float i_kol_kal_dnei=0.;
float kol_rab_dnei;
float kol_rab_has;
char fio[512];
while(cur.read_cursor(&row) != 0)
 {
  if(l_otrvr_prov(row,data) != 0)
   continue;

  memset(fio,'\0',sizeof(fio));
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   strncpy(fio,row1[0],sizeof(fio)-1);

  //Наименование
  memset(naim,'\0',sizeof(naim));
  
  sprintf(strsql,"select naik from Tabel where kod=%s",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   strncpy(naim,row1[0],sizeof(naim)-1);
  
  kol_otr_dnei=atof(row[2]);
  kol_otr_has=atof(row[3]);
  kol_kal_dnei=atof(row[4]);
  
  i_kol_otr_dnei+=kol_otr_dnei;
  i_kol_otr_has+=kol_otr_has;
  i_kol_kal_dnei+=kol_kal_dnei;

  kol_rab_dnei=atof(row[9]);
  kol_rab_has=atof(row[10]);
  
  strcpy(datan,iceb_u_datzap(row[5]));
  strcpy(datak,iceb_u_datzap(row[6]));
  
  //Дата и время записи
  s5.new_plus(iceb_u_vremzap(row[8]));

  //Кто записал
  s6.new_plus(iceb_kszap(row[7],1,data->window));

  fprintf(ff,"%-6s %-*.*s %-4s %-*.*s %6.6g %6.6g %6.6g %6.6g %6.6g %10s %10s %s %.*s\n",
  row[0],
  iceb_u_kolbait(20,fio),iceb_u_kolbait(20,fio),fio,
  row[1],
  iceb_u_kolbait(20,naim),iceb_u_kolbait(20,naim),naim,
  kol_otr_dnei,
  kol_otr_has,
  kol_kal_dnei,
  kol_rab_dnei,
  kol_rab_has,
  datan,
  datak,
  s5.ravno(),
  iceb_u_kolbait(20,s6.ravno()),s6.ravno());

  if(iceb_u_strlen(naim) > 20 || iceb_u_strlen(fio) > 20)
   {
    fprintf(ff,"%6s %-*.*s %4s %s\n",
    "",
    iceb_u_kolbait(20,iceb_u_adrsimv(20,fio)),
    iceb_u_kolbait(20,iceb_u_adrsimv(20,fio)),
    iceb_u_adrsimv(20,fio),
    "",
    iceb_u_adrsimv(20,naim));
   }
 }
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s %6.6g %6.6g %6.6g\n",
iceb_u_kolbait(53,gettext("Итого")),gettext("Итого"),
i_kol_otr_dnei,i_kol_otr_has,i_kol_kal_dnei);

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,"",0,data->window);

}
/*************************/
/*проверка записи на условия поиска*/
/***********************************/
int l_otrvr_prov(SQL_str row,class l_otrvr_data *data)
{
if(data->metka_poi == 0)
 return(0);
 
if(iceb_u_proverka(data->rk.tabnom.ravno(),row[0],0,0) != 0)
 return(1);
if(iceb_u_proverka(data->rk.kod_tab.ravno(),row[1],0,0) != 0)
 return(1);

if(data->rk.podr.getdlinna() > 1)
 {
  char strsql[512];
  SQL_str row1;
  class SQLCURSOR cur;
  sprintf(strsql,"select podr from Kartb where tabn=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur,data->window) == 1)
   if(iceb_u_proverka(data->rk.podr.ravno(),row1[0],0,0) != 0)
    return(1);
 }

return(0);

}
/*********************************************/
/*Замена одного кода на другой по всему списку*/
/*********************************************/
int l_otrvr_zamkod(class l_otrvr_data *data)
{
class iceb_u_str skod("");
class iceb_u_str nkod("");
char strsql[512];

if(l_otrvr_zk(&skod,&nkod,data->window) != 0)
 return(1);
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return(1);
 }

//sprintf(strsql,"select tabn,kodt,dnei,has,kdnei,datn,datk,ktoz,vrem,kolrd,kolrh,kom,nomz from Ztab 
//where god=%d and mes=%d order by tabn asc",data->gp,data->mp);

while(cur.read_cursor(&row) != 0)
 {
  if(l_otrvr_prov(row,data) != 0)
   continue;
  if(iceb_u_proverka(skod.ravno(),row[1],0,0) != 0)
   continue;

  sprintf(strsql,"update Ztab set kodt=%d where tabn=%s and god=%d and mes=%d and kodt=%s and nomz=%s",
  nkod.ravno_atoi(),row[0],data->gp,data->mp,row[1],row[12]);

  iceb_sql_zapis(strsql,0,0,data->window);
 }
return(0);
}
