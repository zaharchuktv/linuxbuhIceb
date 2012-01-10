/*$Id: l_restdok1.c,v 1.11 2011-02-21 07:36:20 sasa Exp $*/
/*20.03.2006	23.02.2004	Белых А.И.	l_restdok1.c
Просмотр документа при расчёте клиента на ценральной кассе
*/
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "i_rest.h"

enum
{
  FK5,
  FK10,
  KOL_F_KL
};

enum
{
 COL_SKL,
 COL_MZ,
 COL_KOD,
 COL_MR,
 COL_NAIM,
 COL_EI,
 COL_KOLIH,
 COL_CENA,
 COL_PROC_SK,
 COL_CENA_SK,
 COL_SUMA,
 COL_KOMENT,
 COL_VREM,
 COL_KTO,
 NUM_COLUMNS
};
 
class  restdok1_data
 {
  public:
 
  short       dd,md,gd; //дата документа
  iceb_u_str  nomdok; //номер документа
  

  iceb_u_str  nstol; // номер столика
  iceb_u_str  kodkl; //код клиента
  iceb_u_str  fio;
  iceb_u_str  koment;
  short       mo;    //0-не оплачено 1-оплачено
  int         podr;  //код подразделения в котороом выписывавается счёт
  iceb_u_str  naimpodr;
  time_t      vremz;
      
  int         kodn;  //код новой записи

  int         sklv;
  int         kodv;
  int         mzv;
  int         nz;  //номер записи
  
  GtkWidget *label_kolstr;
//  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  iceb_u_str skladv;
  iceb_u_str podrv;
  float      proc_sk; //Процент скидки
  
  //Конструктор
  restdok1_data()
   {
    skladv.new_plus("");
    kodn=0;
    snanomer=-2;
    kl_shift=0;
    window=NULL;
    treeview=NULL;
   }      
 };




gboolean   restdok1_key_press(GtkWidget *widget,GdkEventKey *event,class restdok1_data *data);
void  restdok1_knopka(GtkWidget *widget,class restdok1_data *data);
void restdok1_add_columns(GtkTreeView *treeview);
void restdok1_udzap(class restdok1_data *data);
int restdok1_uddok(short dd,short md,short gd,char *nomdok,GtkWidget*);
void restdok1_create_list (class restdok1_data *data);



extern short metka_crk; //0-расчёт клиентов по подразделениям 1-централизованный
extern short metka_skreen; //0-сенсорный экран не используется 1-используется
extern SQL_baza	bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;

void  l_restdok1(short god,iceb_u_str *nomdok,GtkWidget *wpredok)
{
restdok1_data data;
char strsql[300];
SQLCURSOR cur;
SQL_str   row;

data.gd=god;
data.nomdok.new_plus(nomdok->ravno());

//читаем реквизиты шапки документа
sprintf(strsql,"select * from Restdok where god=%d and nomd='%s'",
data.gd,data.nomdok.ravno());

if(iceb_sql_readkey(strsql,&row,&cur,NULL) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"%s %s %s %d",
  gettext("Не найден документ"),
  nomdok->ravno(),
  gettext("Год"),
  god);
  
  repl.plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  return;
 }

iceb_u_rsdat(&data.dd,&data.md,&data.gd,row[2],2); 
data.mo=atoi(row[3]);
data.nstol.new_plus(row[4]);
data.kodkl.new_plus(row[5]);
data.fio.new_plus(row[6]);
data.koment.new_plus(row[7]);
data.podr=atoi(row[11]);
data.naimpodr.new_plus("");
data.vremz=atol(row[12]);
data.proc_sk=atof(row[14]);

//Выставляем флаг
memset(strsql,'\0',sizeof(strsql));
if(data.kodkl.getdlinna() > 1)
  sprintf(strsql,"rest|%s|%s",imabaz,data.kodkl.ravno());

sprintf(strsql,"select naik from Restpod where kod=%d",data.podr);
if(iceb_sql_readkey(strsql,&row,&cur,NULL) == 1)
 data.naimpodr.new_plus(row[0]);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(strsql,"%s %s",name_system,gettext("Просмотр документа"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(restdok1_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Просмотр документа"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//gtk_widget_show_all(hboxv);
gtk_widget_show(vbox1);
gtk_widget_show(vbox2);



//Кнопки
GtkTooltips *tooltips[KOL_F_KL];


sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(restdok1_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(restdok1_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

restdok1_create_list(&data);

gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


return;

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void restdok1_create_list (class restdok1_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[300];
int  kolstr=0;
SQL_str row;
SQL_str row1;
//GdkColor color;
double suma=0.;
double isuma=0.;

printf("restdok1_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

//g_signal_connect(data->treeview,"row_activated",G_CALLBACK(restdok1_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
//g_signal_connect(selection,"changed",G_CALLBACK(restdok1_vibor),data);

gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (data->treeview), TRUE); //Устанавливаются полоски при отображении списка
gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+3,
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
G_TYPE_INT,
G_TYPE_INT);


sprintf(strsql,"select * from Restdok1 where datd='%d-%d-%d' and \
nomd='%s' order by mz asc,skl asc",
data->gd,data->md,data->dd,data->nomdok.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
double kolih=0.;
double cena=0.;
double cena_sk=0.;
//int nomerzap=0;
char metka_ras[5];
double suma_skidki=0.;
float proc_sk=0.;

while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
//  if(restdok1_prov_row(row,data) != 0)
//    continue;


  if(data->kodn == atoi(row[4]))
     data->snanomer=data->kolzap;

  //Склад
  ss[COL_SKL].new_plus(iceb_u_toutf(row[2]));
  
  //Метка записи
  if(row[3][0] == '0')
   ss[COL_MZ].new_plus(iceb_u_toutf("Т"));
  else
   ss[COL_MZ].new_plus(iceb_u_toutf("П"));
  
  //Код
  ss[COL_KOD].new_plus(iceb_u_toutf(row[4]));

  //узнаём наименование
  if(atoi(row[3]) == 0) //материал
   {
    
    sprintf(strsql,"select naimat from Material where kodm=%s",
    row[4]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     ss[COL_NAIM].new_plus(iceb_u_toutf(row1[0]));
    else
     ss[COL_NAIM].new_plus(iceb_u_toutf(" "));
            
   }
  else //услуга
   {
    
    sprintf(strsql,"select naius from Uslugi where kodus=%s",
    row[4]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     ss[COL_NAIM].new_plus(iceb_u_toutf(row1[0]));
    else
     ss[COL_NAIM].new_plus(iceb_u_toutf(" "));
            
   }
  
  //единица измерения
  ss[COL_EI].new_plus(iceb_u_toutf(row[5]));

  //количество
  kolih=atof(row[6]);
  sprintf(strsql,"%.10g",kolih);
  ss[COL_KOLIH].new_plus(strsql);

  // цена
  cena=atof(row[7]);
  sprintf(strsql,"%.10g",cena);
  ss[COL_CENA].new_plus(strsql);

  // процент скидки
  proc_sk=atof(row[16]);
  sprintf(strsql,"%.10g%%",proc_sk);
  ss[COL_PROC_SK].new_plus(strsql);

  //Цена со скидкой
  cena_sk=cena-cena*proc_sk/100.;  
  cena_sk=iceb_u_okrug(cena_sk,0.01);
  ss[COL_CENA_SK].new_plus(cena_sk);

  suma_skidki+=kolih*(cena-cena_sk);
  
  // сумма
  suma=kolih*cena_sk;
  isuma+=suma;
  sprintf(strsql,"%.2f",suma);  
  ss[COL_SUMA].new_plus(strsql);

  //коментарий
  ss[COL_KOMENT].new_plus(iceb_u_toutf(row[8]));

  //Дата и время записи
  ss[COL_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[11])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_u_kszap(row[10],0));
  
  memset(metka_ras,'\0',sizeof(metka_ras));
  if(row[14][0] == '1')
   metka_ras[0]='*';
   
  gtk_list_store_append (model, &iter);


   gtk_list_store_set (model, &iter,
   COL_SKL,ss[COL_SKL].ravno(),
   COL_MZ,ss[COL_MZ].ravno(),
   COL_KOD,ss[COL_KOD].ravno(),
   COL_MR,metka_ras,
   COL_NAIM,ss[COL_NAIM].ravno(),
   COL_EI,ss[COL_EI].ravno(),
   COL_KOLIH,ss[COL_KOLIH].ravno(),
   COL_CENA,ss[COL_CENA].ravno(),
   COL_PROC_SK,ss[COL_PROC_SK].ravno(),
   COL_CENA_SK,ss[COL_CENA_SK].ravno(),
   COL_SUMA,ss[COL_SUMA].ravno(),
   COL_KOMENT,ss[COL_KOMENT].ravno(),
   COL_VREM,ss[COL_VREM].ravno(),
   COL_KTO,ss[COL_KTO].ravno(),
   NUM_COLUMNS,data->kolzap,
   NUM_COLUMNS+1,atoi(row[3]),
   NUM_COLUMNS+2,atoi(row[13]),
   -1);

  data->kolzap++;
 }
data->kodn=0;
gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

restdok1_add_columns (GTK_TREE_VIEW (data->treeview));

  if(data->kolzap == 0)
   {
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
   }
  else
   {
//    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
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
//  data->snanomer=-2;
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

sprintf(strsql,"%s:%s %s %d.%d.%d %s:%d",
gettext("Счет"),
data->nomdok.ravno(),
gettext("от"),
data->dd,data->md,data->gd,
gettext("Количество записей"),
data->kolzap);

zagolov.plus(strsql);
printf("data->vremz=%ld\n",data->vremz);
sprintf(strsql,"%s:%s",gettext("Дата заказа"),iceb_u_vremzap(data->vremz));

zagolov.ps_plus(strsql);

sprintf(strsql,"%s: %d %s",
gettext("Подразделение"),
data->podr,data->naimpodr.ravno());
zagolov.ps_plus(strsql);

if(data->nstol.getdlinna() > 1)
 {
  zagolov.ps_plus(gettext("Номер столика"));
  zagolov.plus(":");
  zagolov.plus(data->nstol.ravno());
 }

if(data->kodkl.getdlinna() > 1)
 {
  zagolov.ps_plus(gettext("Код клиента"));
  zagolov.plus(":");
  zagolov.plus(data->kodkl.ravno());
 }

if(data->fio.getdlinna() > 1)
 {
  zagolov.ps_plus(gettext("Фамилия клиента"));
  zagolov.plus(":");
  zagolov.plus(data->fio.ravno());
 }


if(data->koment.getdlinna() > 1)
 {
  zagolov.ps_plus(gettext("Коментарий"));
  zagolov.plus(":");
  zagolov.plus(data->koment.ravno());
 }


if(data->proc_sk != 0.)
 {
  zagolov.ps_plus(gettext("Процент скидки"));
  zagolov.plus(":");
  zagolov.plus(data->proc_sk);
  suma_skidki=iceb_u_okrug(suma_skidki,0.01);
 }

if(suma_skidki == 0.)
  sprintf(strsql,"%s:%.2f",gettext("Сумма к оплате"),isuma);
else
  sprintf(strsql,"%s:%.2f %s:%.2f",
  gettext("Сумма к оплате"),isuma,
  gettext("Сумма скидки"),suma_skidki);

zagolov.ps_plus(strsql);
zagolov.plus("  ");
if(data->mo == 0)
  zagolov.plus(gettext("Счет не оплачено"));
if(data->mo == 1)
  zagolov.plus(gettext("Счет оплачено"));


gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());


gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));


}

/*****************/
/*Создаем колонки*/
/*****************/

void restdok1_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

printf("restdok1_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("П/д"), renderer,"text", COL_SKL,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,"M", renderer,"text", COL_MZ,NULL);

renderer = gtk_cell_renderer_text_new ();
//g_object_set(G_OBJECT(renderer),"foreground","white",NULL);
//g_object_set(G_OBJECT(renderer),"background","black",NULL);


gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_KOD,NULL);
renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
"*", renderer,"text", COL_MR,NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Наименование"), renderer,
"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Е/и"), renderer,
"text", COL_EI,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Количество"), renderer,
"text", COL_KOLIH,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Цена"), renderer,
"text", COL_CENA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("П/c"), renderer,"text", COL_PROC_SK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Цена со скидкой"), renderer,
"text", COL_CENA_SK,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Сумма"), renderer,
"text", COL_SUMA,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Коментарий"), renderer,
"text", COL_KOMENT,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

printf("restdok1_add_columns end\n");

}


/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  restdok1_knopka(GtkWidget *widget,class restdok1_data *data)
{
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("restdok1_knopka knop=%d\n",knop);
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch ((gint)knop)
 {


  case FK5:
    if(data->kolzap == 0)
      return;
    restdok_rshet(data->dd,data->md,data->gd,data->nomdok.ravno(),data->window);
    return;  

    
  case FK10:
//    printf("restdok1_knopka F10\n");
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   restdok1_key_press(GtkWidget *widget,GdkEventKey *event,class restdok1_data *data)
{
iceb_u_str repl;
printf("restdok1_key_press keyval=%d state=%d\n",
event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_F5:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
    return(TRUE);

  
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(TRUE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("restdok1_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
