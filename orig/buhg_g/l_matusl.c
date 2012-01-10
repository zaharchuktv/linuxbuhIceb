/*$Id: l_matusl.c,v 1.26 2011-02-21 07:35:53 sasa Exp $*/
/*12.11.2009	01.11.2004	Белых А.И.	l_matusl.c
ввод и корректировка услуг в документ материального учёта
*/
#include        <stdlib.h>
#include        "buhg_g.h"

enum
{
 FK2,
 SFK2,
 FK3,
 FK10,
 KOL_F_KL
};

enum
{
 COL_KODU,
 COL_NAIM,
 COL_EI,
 COL_KOLIH,
 COL_CENA_SUMA,
 COL_SUMA,
 COL_PDV,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  matusl_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *treeview;
  GtkWidget *window;
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  int       voz;

  //Реквизиты документа
  
  short dd,md,gd;
  iceb_u_str nomdok;
  int skl;
  float pnds;

  //Реквизиты выбраной строки
  iceb_u_str naim_usl_v;
  iceb_u_str ei_v;
  iceb_u_str kolih_v;
  iceb_u_str cena_suma_v;
  iceb_u_str pdv_v;
  int nomer_zap;
      
  matusl_data()
   {
    nomer_zap=0;
    snanomer=0;
    voz=0;
    treeview=NULL;
    naim_usl_v.new_plus("");
    ei_v.new_plus("");
    kolih_v.new_plus("");
    cena_suma_v.new_plus("");
    pdv_v.new_plus("");
   }


 };

gboolean   matusl_key_press(GtkWidget *widget,GdkEventKey *event,class matusl_data *data);
void matusl_vibor(GtkTreeSelection *selection,class matusl_data *data);
void matusl_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class matusl_data *data);
void  matusl_knopka(GtkWidget *widget,class matusl_data *data);
void matusl_add_columns (GtkTreeView *treeview);
void matusl_create_list (class matusl_data *data);


int l_matusl_v(int metka,short dd,short md,short gd,const char *nomdok,int skl,float,int,GtkWidget *wpredok);
void  matusl_udzap(class matusl_data *data);

extern SQL_baza  bd;
extern char      *name_system;
extern double	okrcn;  /*Округление цены*/
extern double	okrg1;


void l_matusl(short dd,short md,short gd,const char *nomdok,int skl,float pnds,GtkWidget *wpredok)
{
class matusl_data data;
char bros[500];
data.dd=dd;
data.md=md;
data.gd=gd;
data.nomdok.new_plus(nomdok);
data.skl=skl;
data.pnds=pnds;

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(bros,"%s %s",name_system,
gettext("Работа с документом материального учёта"));

gtk_window_set_title (GTK_WINDOW (data.window),iceb_u_toutf(bros));
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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(matusl_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new ("");

iceb_u_str stroka;

gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);


data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(bros,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(matusl_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);


sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(matusl_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(matusl_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удалить выбранную запись"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(matusl_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);

gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

matusl_create_list(&data);

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
void matusl_create_list (class matusl_data *data)
{
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
int  kolstr=0;
SQL_str row,row1;

//printf("matusl_create_list %d\n",data->snanomer);


data->kl_shift=0; //0-отжата 1-нажата  

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(matusl_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(matusl_vibor),data);

gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (data->treeview), TRUE); //Устанавливаются полоски при отображении списка
gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);


model = gtk_list_store_new (NUM_COLUMNS+2, 
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


sprintf(strsql,"select * from Dokummat3 where datd='%d-%d-%d' and \
sklad=%d and nomd='%s'",
data->gd,data->md,data->dd,data->skl,data->nomdok.ravno());


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
double bb;
double suma=0.;
char naiusl[500];

while(cur.read_cursor(&row) != 0)
 {

//  printf("%s %s %s %s %s %s %s\n",row[0],row[2],row[3],row[4],row[5],row[6],row[7]);

  kolih=atof(row[4]);
  cena=atof(row[5]);
  cena=iceb_u_okrug(cena,okrcn);

  bb=kolih*cena;
  if(bb != 0.)
    bb=iceb_u_okrug(bb,okrg1);
  suma+=bb;  
    
  //Код услуги
  ss[COL_KODU].new_plus(row[10]);  
  
  memset(naiusl,'\0',sizeof(naiusl));
  if(atoi(row[10]) != 0)
   {
    sprintf(strsql,"select naius from Uslugi where kodus=%s",row[10]);
    printf("strsql=%s\n",strsql);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
       strncpy(naiusl,row1[0],sizeof(naiusl)-1);
   }  

  if(naiusl[0] == '\0')
   strncpy(naiusl,row[7],sizeof(naiusl)-1);  
  else
   {
    strcat(naiusl,"+");
    strcat(naiusl,row[7]);
   }
  //Наименование услуги
  ss[COL_NAIM].new_plus(iceb_u_toutf(naiusl));  
      
  //Количество
  sprintf(strsql,"%.10g",atof(row[4]));
  ss[COL_KOLIH].new_plus(strsql);  

  //Цена
  sprintf(strsql,"%.10g",atof(row[5]));
  ss[COL_CENA_SUMA].new_plus(strsql);  

  //Сумма по строке
  sprintf(strsql,"%.10g",bb);
  ss[COL_SUMA].new_plus(strsql);  

  //НДС
  sprintf(strsql,"%.10g",atof(row[6]));
  ss[COL_PDV].new_plus(strsql);  
  

  //Единица измерения
  ss[COL_EI].new_plus(iceb_u_toutf(row[3]));

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[9])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[8],0,data->window));

  gtk_list_store_append (model, &iter);
    
  gtk_list_store_set (model, &iter,
  COL_KODU,ss[COL_KODU].ravno(),
  COL_NAIM,ss[COL_NAIM].ravno(),
  COL_EI,ss[COL_EI].ravno(),
  COL_KOLIH,ss[COL_KOLIH].ravno(),
  COL_CENA_SUMA,ss[COL_CENA_SUMA].ravno(),
  COL_SUMA,ss[COL_SUMA].ravno(),
  COL_PDV,ss[COL_PDV].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  NUM_COLUMNS+1,atoi(row[11]),-1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

matusl_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str zagolov;
zagolov.new_plus(gettext("Ввод и корректировка услуг"));
zagolov.ps_plus(gettext("Документ"));
zagolov.plus(" ");
zagolov.plus(data->nomdok.ravno());
zagolov.plus(" ");
zagolov.plus(gettext("Дaта"));
sprintf(strsql," %d.%d.%d%s",data->dd,data->md,data->gd,gettext("г."));
zagolov.plus(strsql);
zagolov.plus(" ");
zagolov.plus(gettext("Сумма"));
zagolov.plus(":");
zagolov.plus(suma);
zagolov.plus(" ");
zagolov.plus(gettext("Количество записей"));
zagolov.plus(":");
zagolov.plus(data->kolzap);



gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

gtk_widget_show(data->label_kolstr);

printf("matusl_create - end\n");

}





/*****************/
/*Создаем колонки*/
/*****************/

void matusl_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

printf("matusl_add_columns\n");


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_KODU,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Е/и"), renderer,"text", COL_EI,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Количество"), renderer,"text", COL_KOLIH,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Цена"), renderer,"text", COL_CENA_SUMA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Сумма"), renderer,"text", COL_SUMA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("НДС"), renderer,"text", COL_PDV,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,"text", COL_KTO,NULL);

printf("matusl_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void matusl_vibor(GtkTreeSelection *selection,class matusl_data *data)
{
printf("matusl_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *naim;
gchar *ei;
gchar *kolih;
gchar *cena_suma;
gchar *pdv;
gint nomer;
gint nomer_zap;

gtk_tree_model_get(model,&iter,COL_NAIM,&naim,COL_EI,&ei,
COL_CENA_SUMA,&cena_suma,COL_KOLIH,&kolih,COL_PDV,&pdv,NUM_COLUMNS,&nomer,NUM_COLUMNS+1,&nomer_zap,-1);


data->naim_usl_v.new_plus(iceb_u_fromutf(naim));
data->ei_v.new_plus(iceb_u_fromutf(ei));
data->kolih_v.new_plus(iceb_u_fromutf(kolih));
data->cena_suma_v.new_plus(iceb_u_fromutf(cena_suma));
data->pdv_v.new_plus(pdv);

data->snanomer=nomer;
data->nomer_zap=nomer_zap;

g_free(naim);
g_free(ei);
g_free(kolih);
g_free(cena_suma);
g_free(pdv);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  matusl_knopka(GtkWidget *widget,class matusl_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {


  case FK2:

    l_matusl_v(0,data->dd,data->md,data->gd,data->nomdok.ravno(),
    data->skl,data->pnds,0,data->window);
    
    matusl_create_list(data);

    return;  

  case SFK2:

    if(data->kolzap == 0)
     return;

    l_matusl_v(1,data->dd,data->md,data->gd,data->nomdok.ravno(),
    data->skl,data->pnds,data->nomer_zap,data->window);

    matusl_create_list(data);

    return;  


  case FK3:
    if(data->kolzap == 0)
     return;

    matusl_udzap(data);

    matusl_create_list(data);

    return;  

    
  case FK10:
    gtk_widget_destroy(data->window);
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   matusl_key_press(GtkWidget *widget,GdkEventKey *event,class matusl_data *data)
{
iceb_u_str repl;
printf("matusl_key_press keyval=%d state=%d\n",
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
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    return(TRUE);


  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("matusl_key_press-Нажата клавиша Shift\n");

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
void matusl_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class matusl_data *data)
{
printf("matusl_v_row\n");
//data->metkazapisi=1;
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
}
/****************************/
/*Удаление записи*/
/****************************/

void  matusl_udzap(class matusl_data *data)
{


iceb_u_str repl;
repl.plus(gettext("Удалить запись ? Вы уверены ?"));
if(iceb_menu_danet(&repl,2,data->window) == 2)
 return;

char strsql[1024];

sprintf(strsql,"delete from Dokummat3 where nz=%d",data->nomer_zap);

iceb_sql_zapis(strsql,0,0,data->window);



}
