/*$Id: admin_icebuser_v1.c,v 1.4 2011-02-21 07:35:50 sasa Exp $*/
/*12.11.2009	21.06.2006	Белых А.И.	admin_icebuser_v1.c
Работа со списком категорий
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
//zs
//#include  "buhg_g.h"
#include  "../headers/buhg_g.h"
//ze

enum
{
  COL_NAIMPS,
  COL_RNR,
  COL_RNB,
  COL_RNN,
  NUM_COLUMNS
};

enum
{
  FK2,
  FK10,
  KOL_F_KL
};

class  admin_icebuser_v1_data
 {
  public:


  iceb_u_str kodv;
  iceb_u_str naimv;
  iceb_u_str kod_kat_tv; //только что введённая единица измерения
  
  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       metka_voz;  //0-выбрали 1-нет  
  int nom_op;
  int dostup[ICEB_PS_UD][3];
  class iceb_u_str fioop;
  //Конструктор
  admin_icebuser_v1_data()
   {
    metka_voz=kl_shift=0;
    window=treeview=NULL;
    kod_kat_tv.plus("");
   }      
 };

gboolean   admin_icebuser_v1_key_press(GtkWidget *widget,GdkEventKey *event,class admin_icebuser_v1_data *data);
//void admin_icebuser_v1_vibor(GtkTreeSelection *selection,class admin_icebuser_v1_data *data);
void  admin_icebuser_v1_knopka(GtkWidget *widget,class admin_icebuser_v1_data *data);
void admin_icebuser_v1_add_columns (GtkTreeView *treeview);
void admin_icebuser_v1_create_list (class admin_icebuser_v1_data *data);

int admin_icebuser_v1_v(class iceb_u_str *kod_gr,GtkWidget *wpredok);
int admin_icebuser_v1_p(class admin_icebuser_v1_rek *rek_poi,GtkWidget *wpredok);


extern char *organ;
extern SQL_baza  bd;
extern char      *name_system;

int   admin_icebuser_v1(int nom_op,GtkWidget *wpredok)
{
admin_icebuser_v1_data data;
char bros[312];
data.nom_op=nom_op;
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
sprintf(strsql,"select * from icebuser where un=%d",data.nom_op);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  iceb_menu_soob(gettext("Не найдена запись для корректировки!"),wpredok);
  return(1);
 }
data.fioop.new_plus(row[1]);
int nomrow=6;
for(int nom=0; nom < ICEB_PS_UD; nom++)
 {
  data.dostup[nom][0]=atoi(row[nomrow]);
  data.dostup[nom][1]=atoi(row[nomrow+1]);
  data.dostup[nom][2]=atoi(row[nomrow+2]);

  nomrow+=3;
/**********
  printf("%d %d %d\n",
  data.dostup[nom][0],
  data.dostup[nom][1],
  data.dostup[nom][2]);
***********/
 }

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

gtk_window_set_default_size (GTK_WINDOW  (data.window),600,-1);


sprintf(bros,"%s %s",name_system,gettext("Разрешения"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(admin_icebuser_v1_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Список категорий работников"));
//gdk_color_parse("green",&color);
//gtk_widget_modify_fg(data.label_kolstr,GTK_STATE_NORMAL,&color);


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);


data.sw = gtk_scrolled_window_new (NULL, NULL);
gtk_widget_set_usize(GTK_WIDGET(data.sw),400,300);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(bros,"F2 %s",gettext("Сохранить"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(admin_icebuser_v1_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Сохранить настройки"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(admin_icebuser_v1_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

admin_icebuser_v1_create_list(&data);

gtk_widget_show(data.window);
//  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.metka_voz);

}





/***********************************/
/*Создаем список для просмотра */
/***********************************/
void admin_icebuser_v1_create_list (class admin_icebuser_v1_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

//g_signal_connect(data->treeview,"row_activated",G_CALLBACK(admin_icebuser_v1_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
//g_signal_connect(selection,"changed",G_CALLBACK(admin_icebuser_v1_vibor),data);

gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (data->treeview), TRUE); //Устанавливаются полоски при отображении списка
gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS, 
G_TYPE_STRING, 
G_TYPE_BOOLEAN,
G_TYPE_BOOLEAN,
G_TYPE_BOOLEAN);



class iceb_u_str naimps("");
gboolean metka1=TRUE;
gboolean metka2=TRUE;
gboolean metka3=TRUE;
//for(int nom=1; nom <= ICEB_PS_UD; nom++)
for(int nom=1; nom <= ICEB_PS_UD; nom++)
 {
  switch(nom)
   {
    case ICEB_PS_GK:
       naimps.new_plus(gettext("Главная книга"));     

       if(data->dostup[ICEB_PS_GK-1][0] == 0)
        metka1=TRUE;
       else
        metka1=FALSE;


       if(data->dostup[ICEB_PS_GK-1][1] == 0)
        metka2=TRUE;
       else
        metka2=FALSE;

       if(data->dostup[ICEB_PS_GK-1][2] == 0)
        metka3=TRUE;
       else
        metka3=FALSE;

       break;     

    case ICEB_PS_MU:
       naimps.new_plus(gettext("Материальный учёт"));     
       if(data->dostup[ICEB_PS_MU-1][0] == 0)
        metka1=TRUE;
       else
        metka1=FALSE;


       if(data->dostup[ICEB_PS_MU-1][1] == 0)
        metka2=TRUE;
       else
        metka2=FALSE;

       if(data->dostup[ICEB_PS_MU-1][2] == 0)
        metka3=TRUE;
       else
        metka3=FALSE;
       break;     

    case ICEB_PS_PD:
       naimps.new_plus(gettext("Материальный учёт"));     
       if(data->dostup[ICEB_PS_PD-1][0] == 0)
        metka1=TRUE;
       else
        metka1=FALSE;


       if(data->dostup[ICEB_PS_PD-1][1] == 0)
        metka2=TRUE;
       else
        metka2=FALSE;

       if(data->dostup[ICEB_PS_PD-1][2] == 0)
        metka3=TRUE;
       else
        metka3=FALSE;
       break;     

    case ICEB_PS_ZP:
       naimps.new_plus(gettext("Заработная плата"));     
       if(data->dostup[ICEB_PS_ZP-1][0] == 0)
        metka1=TRUE;
       else
        metka1=FALSE;


       if(data->dostup[ICEB_PS_ZP-1][1] == 0)
        metka2=TRUE;
       else
        metka2=FALSE;

       if(data->dostup[ICEB_PS_ZP-1][2] == 0)
        metka3=TRUE;
       else
        metka3=FALSE;
       break;     

    case ICEB_PS_UOS:
       naimps.new_plus(gettext("Учёт основных средств"));     
       if(data->dostup[ICEB_PS_UOS-1][0] == 0)
        metka1=TRUE;
       else
        metka1=FALSE;


       if(data->dostup[ICEB_PS_UOS-1][1] == 0)
        metka2=TRUE;
       else
        metka2=FALSE;

       if(data->dostup[ICEB_PS_UOS-1][2] == 0)
        metka3=TRUE;
       else
        metka3=FALSE;
       break;     

    case ICEB_PS_UU:
       naimps.new_plus(gettext("Учёт услуг"));     
       if(data->dostup[ICEB_PS_UU-1][0] == 0)
        metka1=TRUE;
       else
        metka1=FALSE;


       if(data->dostup[ICEB_PS_UU-1][1] == 0)
        metka2=TRUE;
       else
        metka2=FALSE;

       if(data->dostup[ICEB_PS_UU-1][2] == 0)
        metka3=TRUE;
       else
        metka3=FALSE;
       break;     
     

    case ICEB_PS_UKO:
       naimps.new_plus(gettext("Учёт кассовых оредров"));     
       if(data->dostup[ICEB_PS_UKO-1][0] == 0)
        metka1=TRUE;
       else
        metka1=FALSE;


       if(data->dostup[ICEB_PS_UKO-1][1] == 0)
        metka2=TRUE;
       else
        metka2=FALSE;

       if(data->dostup[ICEB_PS_UKO-1][2] == 0)
        metka3=TRUE;
       else
        metka3=FALSE;
       break;     

    case ICEB_PS_UKR:
       naimps.new_plus(gettext("Учёт командировочных расходов"));     
       if(data->dostup[ICEB_PS_UKR-1][0] == 0)
        metka1=TRUE;
       else
        metka1=FALSE;


       if(data->dostup[ICEB_PS_UKR-1][1] == 0)
        metka2=TRUE;
       else
        metka2=FALSE;

       if(data->dostup[ICEB_PS_UKR-1][2] == 0)
        metka3=TRUE;
       else
        metka3=FALSE;
       break;     

    case ICEB_PS_UPL:
       naimps.new_plus(gettext("Учёт путевых листов"));     
       if(data->dostup[ICEB_PS_UPL-1][0] == 0)
        metka1=TRUE;
       else
        metka1=FALSE;


       if(data->dostup[ICEB_PS_UPL-1][1] == 0)
        metka2=TRUE;
       else
        metka2=FALSE;

       if(data->dostup[ICEB_PS_UPL-1][2] == 0)
        metka3=TRUE;
       else
        metka3=FALSE;
       break;     

    case ICEB_PS_RNN:
       naimps.new_plus(gettext("Учёт путевых листов"));     
       if(data->dostup[ICEB_PS_RNN-1][0] == 0)
        metka1=TRUE;
       else
        metka1=FALSE;


       if(data->dostup[ICEB_PS_RNN-1][1] == 0)
        metka2=TRUE;
       else
        metka2=FALSE;

       if(data->dostup[ICEB_PS_RNN-1][2] == 0)
        metka3=TRUE;
       else
        metka3=FALSE;
       break;     

    case ICEB_PS_UD:
       naimps.new_plus(gettext("Учёт доверенностей"));     
       if(data->dostup[ICEB_PS_UD-1][0] == 0)
        metka1=TRUE;
       else
        metka1=FALSE;


       if(data->dostup[ICEB_PS_UD-1][1] == 0)
        metka2=TRUE;
       else
        metka2=FALSE;

       if(data->dostup[ICEB_PS_UD-1][2] == 0)
        metka3=TRUE;
       else
        metka3=FALSE;
       break;     
   }
/*************
  printf("*%d %d %d\n",
  data->dostup[nom][0],
  data->dostup[nom][1],
  data->dostup[nom][2]);
*************/  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_NAIMPS,naimps.ravno(),
  COL_RNR,metka1,
  COL_RNB,metka2,
  COL_RNN,metka3,
  -1);

 }
data->kod_kat_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

admin_icebuser_v1_add_columns (GTK_TREE_VIEW (data->treeview));



gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(data->nom_op);
zagolov.plus(" ",data->fioop.ravno());


gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());


gtk_widget_show(data->label_kolstr);

}
/**********************************/
/*                               */
/**********************************/
static void admin_icebuser_fixed_toggled (GtkCellRendererToggle *cell,
	       gchar                 *path_str,
	       gpointer               data)
{
  GtkTreeModel *model = (GtkTreeModel *)data;
  GtkTreeIter  iter;
  GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
  gboolean fixed;

  int colonka = GPOINTER_TO_INT(g_object_get_data (G_OBJECT (cell), "column"));
//  printf("colonka=%d\n",colonka);
  /* get toggled iter */
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_model_get (model, &iter, colonka, &fixed, -1);

  /* do something with the value */
  fixed ^= 1;

  /* set new value */
  gtk_list_store_set (GTK_LIST_STORE (model), &iter, colonka, fixed, -1);

  /* clean up */
  gtk_tree_path_free (path);
}

/*****************/
/*Создаем колонки*/
/*****************/

void admin_icebuser_v1_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Наименование подсистемы"),renderer,"text",COL_NAIMPS,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_NAIMPS);
gtk_tree_view_append_column (treeview, column);

GtkTreeModel *model = gtk_tree_view_get_model (treeview);

/* column for fixed toggles */
renderer = gtk_cell_renderer_toggle_new ();
g_signal_connect (renderer, "toggled", G_CALLBACK (admin_icebuser_fixed_toggled), model);
column = gtk_tree_view_column_new_with_attributes (gettext("Сохранить"),renderer,"active", COL_RNR,NULL);
gtk_tree_view_append_column (treeview, column);
gtk_object_set_user_data(GTK_OBJECT(column),(gpointer)COL_RNR);
g_object_set_data (G_OBJECT (renderer), "column", (gint *)COL_RNR);

/* column for fixed toggles */
renderer = gtk_cell_renderer_toggle_new ();
g_signal_connect (renderer, "toggled", G_CALLBACK (admin_icebuser_fixed_toggled), model);
column = gtk_tree_view_column_new_with_attributes (gettext("Блокировка"),renderer,"active", COL_RNB,NULL);
gtk_tree_view_append_column (treeview, column);
g_object_set_data (G_OBJECT (renderer), "column", (gint *)COL_RNB);

/* column for fixed toggles */
renderer = gtk_cell_renderer_toggle_new ();
g_signal_connect (renderer, "toggled", G_CALLBACK (admin_icebuser_fixed_toggled), model);
column = gtk_tree_view_column_new_with_attributes (gettext("Настройка"),renderer,"active", COL_RNN,NULL);
gtk_tree_view_append_column (treeview, column);
g_object_set_data (G_OBJECT (renderer), "column", (gint *)COL_RNN);


}
/*************************************/
/*Чтение настроек*/
/***********************************/

void admin_icebuser_readn(class  admin_icebuser_v1_data *data)
{
GtkTreeIter iter;
GtkTreeModel *list_store = gtk_tree_view_get_model (GTK_TREE_VIEW(data->treeview));
/*Получаем первый итератор в списке*/
gboolean valid=gtk_tree_model_get_iter_first(list_store,&iter);
int nomer=0;
while(valid)
 {
  gboolean  fixed1=FALSE;
  gboolean  fixed2=FALSE;
  gboolean  fixed3=FALSE;

  gtk_tree_model_get(list_store,&iter,COL_RNR,&fixed1,COL_RNB,&fixed2,COL_RNN,&fixed3,-1);

  if(fixed1 == TRUE) 
   data->dostup[nomer][0]=0;
  else
   data->dostup[nomer][0]=1;

  if(fixed2 == TRUE) 
   data->dostup[nomer][1]=0;
  else
   data->dostup[nomer][1]=1;

  if(fixed3 == TRUE) 
   data->dostup[nomer][2]=0;
  else
   data->dostup[nomer][2]=1;

  valid=gtk_tree_model_iter_next(list_store,&iter);

  nomer++;
 }
char strsql[1024];
struct   passwd  *ktor=getpwuid(getuid());

sprintf(strsql,"update icebuser set \
logz='%s',\
vrem=%ld,\
gkd=%d,\
gkb=%d,\
gkn=%d,\
mud=%d,\
mub=%d,\
mun=%d,\
pdd=%d,\
pdb=%d,\
pdn=%d,\
zpd=%d,\
zpb=%d,\
zpn=%d,\
osd=%d,\
osb=%d,\
osn=%d,\
uud=%d,\
uub=%d,\
uun=%d,\
kod=%d,\
kob=%d,\
kon=%d,\
krd=%d,\
krb=%d,\
krn=%d,\
pld=%d,\
plb=%d,\
pln=%d,\
nnd=%d,\
nnb=%d,\
nnn=%d,\
udd=%d,\
udb=%d,\
udn=%d \
where un=%d",
ktor->pw_name,
time(NULL),

data->dostup[0][0],
data->dostup[0][1],
data->dostup[0][2],

data->dostup[1][0],
data->dostup[1][1],
data->dostup[1][2],

data->dostup[2][0],
data->dostup[2][1],
data->dostup[2][2],

data->dostup[3][0],
data->dostup[3][1],
data->dostup[3][2],

data->dostup[4][0],
data->dostup[4][1],
data->dostup[4][2],

data->dostup[5][0],
data->dostup[5][1],
data->dostup[5][2],

data->dostup[6][0],
data->dostup[6][1],
data->dostup[6][2],

data->dostup[7][0],
data->dostup[7][1],
data->dostup[7][2],

data->dostup[8][0],
data->dostup[8][1],
data->dostup[8][2],

data->dostup[9][0],
data->dostup[9][1],
data->dostup[9][2],

data->dostup[10][0],
data->dostup[10][1],
data->dostup[10][2],

data->nom_op);
//printf("strsql=%s\n",strsql);

iceb_sql_zapis(strsql,1,0,data->window);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  admin_icebuser_v1_knopka(GtkWidget *widget,class admin_icebuser_v1_data *data)
{
iceb_u_str repl;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("admin_icebuser_v1_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    admin_icebuser_readn(data);
    data->metka_voz=0;
    gtk_widget_destroy(data->window);
    return;  

    
  case FK10:
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   admin_icebuser_v1_key_press(GtkWidget *widget,GdkEventKey *event,class admin_icebuser_v1_data *data)
{
iceb_u_str repl;

switch(event->keyval)
 {

  case GDK_F2:

      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");

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
//    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
