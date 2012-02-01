/*$Id: l_dokmat.c,v 1.135 2011-08-29 07:13:43 sasa Exp $*/
/*07.04.2010	25.06.2004	Белых А.И.	l_dokmat.c
Просмотр документа в материальном учёте.
Если вернули 0- вышли по F10
             1- удалили документ
*/

#include        <stdlib.h>
#include        <pwd.h>
#include        <math.h>
#include        <errno.h>
#include        <unistd.h>
#include        "../headers/buhg_g.h"
#include        "l_dokmat.h"
enum
{
 SFK1,
 FK2,
 SFK2,
 FK3,
 SFK3,
 FK4,
 FK5,
 SFK5,
 FK6,
 SFK6,
 FK7,
 SFK7,
 FK8,
 FK9,
 FK10,
 FK11,
 KOL_F_KL
};

enum
{
 COL_METKA,
 COL_KODMAT,
 COL_NOMKAR,
 COL_NAIM,
 COL_EI,
 COL_KOLIH,
 COL_CENA,
 COL_SUMMA,
 COL_KOLIHP,  
 COL_NOMZ,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  dokmat_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *treeview;
  GtkWidget *window;
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  int       voz;
  
  //Реквизиты выбранной записи
  iceb_u_str kodmat;
  iceb_u_str nomkar;
  int        nomervsp;
  iceb_u_str nomzak; //номер заказа у выбранной записи
  double     cenavdok;
  double     kolih_dok;
      
  iceb_u_str zapros;
  iceb_u_str naimsklad;
  dokmat_r_data rk;
  iceb_u_spisok imaf;

  double    kol_mat;    //Общее количество материалов по накладной
  double    suma; //Сумма по документа
  double    sumabn; //Сумма без НДС*/
  double    ves;  //Вес по накладной
  double    sumsnds; //Сумма с НДС
  int       metka_usl;          
  iceb_u_double    maskor;
  iceb_u_str naimkor;
  double    sumkopl;
  short     metka_pros_par_dok; //1-просмотр парного документа

  int       kodmat_v; //Код только что введенного материалла и для поиска материалла по коду материалла
    
  dokmat_data()
   {
    clear();
    kodmat_v=0;
   }
  void clear()
   {
    metka_pros_par_dok=0;
    sumkopl=0.;
    voz=0;
    kl_shift=0;
    snanomer=0;
    treeview=NULL;
    naimsklad.new_plus("");
    kol_mat=0.;
    naimkor.new_plus("");
   }
 };

gboolean   dokmat_key_press(GtkWidget *widget,GdkEventKey *event,class dokmat_data *data);
void dokmat_vibor(GtkTreeSelection *selection,class dokmat_data *data);
void dokmat_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class dokmat_data *data);
void  dokmat_knopka(GtkWidget *widget,class dokmat_data *data);
void dokmat_add_columns (GtkTreeView *treeview);
void dokmat_create_list (class dokmat_data *data);

int             readdokw(class dokmat_r_data *data,GtkWidget *wpredok);
short           prkor(short mp,short md,short gd,int blokpid,int loginrash,GtkWidget *wpredok);
void		deletdokum(iceb_u_spisok *imaf);
int	udpardok(short mdd,int tipz,short dd,short md,short gd,int skl,const char *nomdok,int skl1,const char *nomdokp,GtkWidget *wpredok);
int     vmatw(int tipz,short dd,short md,short gd,const char *nomdok,int skl,int*,float,GtkWidget *wpredok);
void dokmat_sapka_menu(class dokmat_data *data);
double		provkolw(short dd,short md,short gd,int skl,const char *nomd,GtkWidget *wpredok);
int dokmat_vihod(class dokmat_data *data);
int kzvmu2(short dd,short md,short gd,int skl,const char *nomdok,int kodm,int nk,int sklad1,const char *nomdokp,GtkWidget *wpredok);
int kzvmu1(short dd,short md,short gd,int skl,const char *nomdok,int kodm,int nk,int sklad1,const char *nomdokp,GtkWidget *wpredok);
void l_prov_mu(short dd,short md,short gd,int sklad,int sklad1,int tipz,const char *nomdok,const char *nomdokp,const char *kontr,const char *kodop,int lnds,double,float,GtkWidget *wpredok);
void dokmat_ras1(class dokmat_data *data);
void dokmat_ras2(class dokmat_data *data);
void dokmat_pros1(class dokmat_data *data);
void dokmat_pros2(class dokmat_data *data);
void   sozf(short dg,short mg,short gg,int skl,const char *nomdok,int tipz,int lnds,iceb_u_spisok *imaf,
double ves,short mvnp,const char *naimo,GtkWidget *wpredok);
void		rasdok2w(short dd,short md,short gd,int skl,const char *nomdok,short tipnn,GtkWidget *wpredok);
void            raspropw(short dd,short md,short gd,int skl,const char *nomdok,GtkWidget *wpredok);
void            rasmedprw(short dg,short mg,short gg,int skl,const char *nomdok,GtkWidget *wpredok);
void            cennikdw(short dg,short mg,short gg,int skl,const char *nomdok,GtkWidget *wpredok);
void            rasdok1w(short dg,short mg,short gg,int skl,const char *nomdok,short lnds,double ves,GtkWidget *wpredok);
void            rasactmuw(short dd,short md,short gd,int skl,const char *nomdok,GtkWidget *wpredok);
int   podzapm(short dd,short md,short gd,int tipz,int skl,const char *nomdok,int kodmat,int n_kart,double kolih_dok,double cena_dok,GtkWidget*);
void    podtdokw(short dd,short md,short gd,int skl,const char *nomdok,int tipz,int skl1,const char *nomdokp,GtkWidget *wpredok);
void dokmat_F8(class dokmat_data *data);
void dokmat_F9_prih(class dokmat_data *data);
void dokmat_F9_rash(class dokmat_data *data);
int  dokmat_pom_zap(class dokmat_data *data);
int copdokw(short dd,short md,short gd,const char *nomdok,int skl,int tipz,GtkWidget*);
void rozkorw(short dd,short md,short gd,int tipz,const char *nomdok,int skl,float,GtkWidget *wpredok);
void doocw(short dd,short md,short gd,int tipz,const char *nomdok,int skl,GtkWidget *wpredok);
int  perecenpw(short dd,short md,short gd,int tipz,const char *nomdok,int skl,GtkWidget *wpredok);
int  perecenrw(short dd,short md,short gd,int tipz,const char *nomdok,int skl,float,GtkWidget *wpredok);
void l_matusl(short dd,short md,short gd,const char *nomdok,int skl,float,GtkWidget *wpredok);
void l_dokmat_read_sap_dok(class dokmat_data *data,GtkWidget *wpredok);
void oplmuw(short d,short m,short g,int tipz,const char *nn,int skll,const char *kodop,GtkWidget *wpredok);


extern SQL_baza  bd;
extern char      *name_system;
extern double    okrg1;  /*Округление 1*/
extern char      *imabaz;
extern short	mborvd;    /*0-многопользовательская работа в документе разрешена 1- запрещена*/
short    mdd=0;  /*0-обычный документ 1-двойной*/
short    mvnp; /*0-внешняя 1 -внутреняя 2-изменение стоимости*/
extern short    srtnk; /*0-не включена 1-включена сортировка записей в накладной*/
extern short    obzap; /*0-не объединять записи 1-обединять*/


int l_dokmat(iceb_u_str *datad, //Дата документа
int tipz,
iceb_u_str *sklad,
iceb_u_str *nomdok,
GtkWidget *wpredok)
{
char strsql[512];
char bros[312];
GdkColor color;

dokmat_data data;

iceb_u_rsdat(&data.rk.dd,&data.rk.md,&data.rk.gd,datad->ravno(),1);
data.rk.nomdok.new_plus(nomdok->ravno());
data.rk.skk=sklad->ravno_atoi();
data.rk.tipz=tipz;

nazad:;


//readdokw(&data.rk,wpredok);
l_dokmat_read_sap_dok(&data,wpredok);
mdd=data.rk.mdd;
mvnp=data.rk.mvnp;

sprintf(strsql,"%s%d.%d.%d%d%s",imabaz,data.rk.dd,data.rk.md,data.rk.gd,data.rk.skk,data.rk.nomdok.ravno());
iceb_sql_flag flag_dok(strsql);

/*Делаем после чтения потому что там иногда определяется дата*/
if(mborvd == 1) /*Установить флаг работы с документом*/
 {

  if(flag_dok.flag_on() != 0)
   {
    iceb_u_str repl;
        
    repl.plus(gettext("С документом уже работает другой оператор !"));
    repl.ps_plus(gettext("Многопользовательская работа с документами заблокирована !"));
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
 }


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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(dokmat_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
//gtk_box_pack_start (GTK_BOX (hbox), vbox2, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new ("");
gtk_label_set_justify(GTK_LABEL(data.label_kolstr),GTK_JUSTIFY_LEFT);

iceb_u_str stroka;

dokmat_sapka_menu(&data);



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
gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);


//Кнопки
GtkTooltips *tooltips[KOL_F_KL];
sprintf(bros,"%sF1 %s",RFK,gettext("Шапка"));
data.knopka[SFK1]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK1]), "clicked",GTK_SIGNAL_FUNC(dokmat_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK1],TRUE,TRUE, 0);
tooltips[SFK1]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK1],data.knopka[SFK1],gettext("Просмотр шапки документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK1]),(gpointer)SFK1);
gtk_widget_show(data.knopka[SFK1]);

sprintf(bros,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(dokmat_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(dokmat_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(dokmat_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удалить выбранную запись"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(dokmat_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Удалить документ"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Проводки"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(dokmat_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Переход в режим работы с проводками для этого документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(dokmat_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка документов"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"%sF5 %s",RFK,gettext("Просмотр"));
data.knopka[SFK5]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK5]), "clicked",GTK_SIGNAL_FUNC(dokmat_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK5],TRUE,TRUE, 0);
tooltips[SFK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK5],data.knopka[SFK5],gettext("Просмотр распечатки документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK5]),(gpointer)SFK5);
gtk_widget_show(data.knopka[SFK5]);

sprintf(bros,"F6 %s",gettext("Карточка"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(dokmat_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Просмотр карточки материалла"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"%sF6 %s",RFK,gettext("Услуги"));
data.knopka[SFK6]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK6]), "clicked",GTK_SIGNAL_FUNC(dokmat_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK6],TRUE,TRUE, 0);
tooltips[SFK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK6],data.knopka[SFK6],gettext("Ввод и корректировка списка услуг"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK6]),(gpointer)SFK6);
gtk_widget_show(data.knopka[SFK6]);

sprintf(bros,"F7 %s",gettext("Подтверждение"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(dokmat_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Подтверждение записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"%sF7 %s",RFK,gettext("Подтверждение"));
data.knopka[SFK7]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK7]), "clicked",GTK_SIGNAL_FUNC(dokmat_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK7],TRUE,TRUE, 0);
tooltips[SFK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK7],data.knopka[SFK7],gettext("Подтверждение всех записей в документе"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK7]),(gpointer)SFK7);
gtk_widget_show(data.knopka[SFK7]);

sprintf(bros,"F8 %s",gettext("Меню"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]), "clicked",GTK_SIGNAL_FUNC(dokmat_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Получение меню для вибора нужного режима работы"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_widget_show(data.knopka[FK8]);

sprintf(bros,"F9 %s",gettext("Меню"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK9]), "clicked",GTK_SIGNAL_FUNC(dokmat_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
tooltips[FK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK9],data.knopka[FK9],gettext("Получение меню для выбора нужного режима работы"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK9]),(gpointer)FK9);
gtk_widget_show(data.knopka[FK9]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(dokmat_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);

if(mdd == 1)
 {
  sprintf(bros,"F11 %s",gettext("Парний док"));
  data.knopka[FK11]=gtk_button_new_with_label(bros);
  gtk_signal_connect(GTK_OBJECT(data.knopka[FK11]), "clicked",GTK_SIGNAL_FUNC(dokmat_knopka),&data);
  gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK11],TRUE,TRUE, 0);
  tooltips[FK11]=gtk_tooltips_new();
  gtk_tooltips_set_tip(tooltips[FK11],data.knopka[FK11],gettext("Перейти в парный документ"),NULL);
  gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK11]),(gpointer)FK11);
  gtk_widget_show(data.knopka[FK11]);
 }

gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

dokmat_create_list(&data);

gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

if(data.metka_pros_par_dok == 1)
 {
  if(data.rk.tipz == 1)
   data.rk.tipz=2;
  if(data.rk.tipz == 2)
   data.rk.tipz=1;
  data.rk.skk=data.rk.skl1;
  data.rk.nomdok.new_plus(data.rk.nomon.ravno());
  data.clear();
  goto nazad;
 }
 

//printf("l_dokmat end\n");


return(data.voz);

}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void dokmat_create_list (class dokmat_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

class iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR curr;
char strsql[512];
int  kolstr=0;
SQL_str row;
SQL_str row1;
//GdkColor color;

//printf("dokmat_create_list %d\n",data->snanomer);



data->kl_shift=0; //0-отжата 1-нажата  

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(dokmat_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(dokmat_vibor),data);

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
G_TYPE_INT);

iceb_u_str zapros;

sprintf(strsql,"select * from Dokummat1 where datd='%d-%d-%d' and \
sklad=%d and nomd='%s' and tipz=%d order by kodm asc",
data->rk.gd,data->rk.md,data->rk.dd,data->rk.skk,data->rk.nomdok.ravno(),data->rk.tipz);


data->zapros.new_plus(strsql);


if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
double kolihp=0.;
short d,m,g;
double suma;
data->kol_mat=0.;
data->suma=data->sumabn=data->ves=data->sumsnds=0.;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);

  if(data->kodmat_v == atoi(row[4]))
   data->snanomer=data->kolzap;
   
  sumdokw(row,&data->suma,&data->sumabn,&data->ves,data->rk.lnds,&data->sumsnds,data->rk.pnds);

  //Узнаём наименование материалла
  ss[COL_NAIM].new_plus(" ");  
  sprintf(strsql,"select naimat from Material where kodm=%s",row[4]);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найден код материалла"));
    repl.plus(" ");
    repl.plus(row[4]);
    repl.plus(" !");
    iceb_menu_soob(&repl,data->window);
   }
  else
   {
    ss[COL_NAIM].new_plus(iceb_u_toutf(row1[0]));  
    if(row[16][0] != '\0')
     {
      ss[COL_NAIM].plus("+");  
      ss[COL_NAIM].plus(iceb_u_toutf(row[16]));  
     }
   }  
  
  
  //Единица измерения
  ss[COL_EI].new_plus(iceb_u_toutf(row[7]));

  //Количество
  data->kol_mat+=atof(row[5]);
  sprintf(strsql,"%.7g",atof(row[5]));
  ss[COL_KOLIH].new_plus(strsql);

  //ЦЕНА
  sprintf(strsql,"%.7g",atof(row[6]));
  ss[COL_CENA].new_plus(strsql);
  

  suma=atof(row[5])*atof(row[6]);
  suma=iceb_u_okrug(suma,okrg1);
  
  //сумма
  sprintf(strsql,"%.2f",suma);
  ss[COL_SUMMA].new_plus(strsql);


  iceb_u_rsdat(&d,&m,&g,row[0],2);
  kolihp=readkolkw(data->rk.skk,atoi(row[3]),d,m,g,data->rk.nomdok.ravno(),data->window);  

  //Подтверждённое количество
  sprintf(strsql,"%.7g",kolihp);
  ss[COL_KOLIHP].new_plus(strsql);

  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[12])));

  //Кто записал
//  ss[COL_KTO].new_plus(iceb_kszap(row[11],0,data->window));
  ss[COL_KTO].new_plus(iceb_kszap(row[11],0,data->window));
  //Номер заказа
  ss[COL_NOMZ].new_plus(iceb_u_toutf(row[17]));
  
  //Метка записи
  memset(strsql,'\0',sizeof(strsql));
  if(row[8][0] == '1')
   strcat(strsql,"#");
  if(atof(row[9]) == 0.)
   strcat(strsql,"*");
  if(atoi(row[10]) == 1)
   strcat(strsql,"+");
  ss[COL_METKA].new_plus(strsql);
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_METKA,ss[COL_METKA].ravno(),
  COL_KODMAT,row[4],
  COL_NOMKAR,row[3],
  COL_NAIM,ss[COL_NAIM].ravno(),
  COL_EI,ss[COL_EI].ravno(),
  COL_KOLIH,ss[COL_KOLIH].ravno(),
  COL_CENA,ss[COL_CENA].ravno(),
  COL_SUMMA,ss[COL_SUMMA].ravno(),
  COL_KOLIHP,ss[COL_KOLIHP].ravno(),
  COL_NOMZ,ss[COL_NOMZ].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

data->kodmat_v=0;

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

dokmat_add_columns (GTK_TREE_VIEW (data->treeview));

data->metka_usl=sumdokwu(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),&data->suma,data->window);

if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  if(data->suma == 0.)
   {
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK5]),FALSE);//Недоступна
   }
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK7]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK6]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK7]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

dokmat_sapka_menu(data);

gtk_widget_show(data->label_kolstr);

}

/*****************/
/*Создаем колонки*/
/*****************/

void dokmat_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

//printf("dokmat_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
"M", renderer,"text", COL_METKA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Код"), renderer,"text", COL_KODMAT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Н.к."), renderer,"text",COL_NOMKAR,NULL);

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
gettext("Цена"), renderer,"text", COL_CENA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Сумма"), renderer,"text", COL_SUMMA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Под-но"), renderer,"text", COL_KOLIHP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Номер заказа"), renderer,"text", COL_NOMZ,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,"text", COL_KTO,NULL);
//printf("dokmat_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void dokmat_vibor(GtkTreeSelection *selection,class dokmat_data *data)
{
//printf("dokmat_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kodmat;
gchar *nomkar;
gint  nomer;
gchar *nomzak;
gchar *cenavdok;
gchar *kolih_dok;


gtk_tree_model_get(model,&iter,COL_KODMAT,&kodmat,COL_NOMKAR,&nomkar,
COL_CENA,&cenavdok,COL_KOLIH,&kolih_dok,COL_NOMZ,&nomzak,NUM_COLUMNS,&nomer,-1);

data->kodmat.new_plus(iceb_u_fromutf(kodmat));
data->nomkar.new_plus(iceb_u_fromutf(nomkar));
data->nomzak.new_plus(iceb_u_fromutf(nomzak));
data->cenavdok=atof(cenavdok);
data->kolih_dok=atof(kolih_dok);

data->nomervsp=nomer;
data->snanomer=nomer;

g_free(kodmat);
g_free(nomkar);
g_free(nomzak);
g_free(cenavdok);
g_free(kolih_dok);

//printf("%s %s %d\n",data->kodmat.ravno(),data->nomkar.ravno(),data->snanomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  dokmat_knopka(GtkWidget *widget,class dokmat_data *data)
{
iceb_u_str repl;
char strsql[512];
iceb_u_str sklad;
iceb_u_str datad;
iceb_u_str kod;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {

  case SFK1:

    sklad.new_plus(data->rk.skk);

    sprintf(strsql,"%d.%d.%d",data->rk.dd,data->rk.md,data->rk.gd);
    datad.new_plus(strsql);
//    printf("sklad=%s nomdok=%s datad=%s\n",sklad.ravno(),data->rk.nomdok.ravno(),datad.ravno());
    if(vdndw(data->rk.tipz,&datad,&sklad,&data->rk.nomdok,&kod,data->window) == 0)
     {
      iceb_u_rsdat(&data->rk.dd,&data->rk.md,&data->rk.gd,datad.ravno(),1);

      data->rk.skk=sklad.ravno_atoi();

      deletdokum(&data->imaf);
      
//      readdokw(&data->rk,data->window);
      l_dokmat_read_sap_dok(data,data->window);
      dokmat_sapka_menu(data);
     }
    return;  

  case FK2:
    if(prkor(0,data->rk.md,data->rk.gd,data->rk.blokpid,data->rk.loginrash,data->window) != 0)
      return;  

    if(vmatw(data->rk.tipz,data->rk.dd,data->rk.md,data->rk.gd,data->rk.nomdok.ravno(),
    data->rk.skk,&data->kodmat_v,data->rk.pnds,data->window) == 0)
     {
      dokmat_create_list(data);
      deletdokum(&data->imaf);
     }
    return;  

  case SFK2:

    if(prkor(0,data->rk.md,data->rk.gd,data->rk.blokpid,data->rk.loginrash,data->window) != 0)
      return;  
    
    if(data->rk.tipz == 2)
     if(kzvmu2(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),
     data->kodmat.ravno_atoi(),data->nomkar.ravno_atoi(),data->rk.skl1,data->rk.nomon.ravno(),
     data->window) == 0)
        dokmat_create_list(data);

    if(data->rk.tipz == 1)
     if(kzvmu1(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),
     data->kodmat.ravno_atoi(),data->nomkar.ravno_atoi(),data->rk.skl1,data->rk.nomon.ravno(),
     data->window) == 0)
        dokmat_create_list(data);

     deletdokum(&data->imaf);

    return;  

  case FK3:
    if(data->kolzap == 0)
     return;    
    if(prkor(0,data->rk.md,data->rk.gd,data->rk.blokpid,data->rk.loginrash,data->window) != 0)
      return;  
    
    repl.new_plus(gettext("Удалить запись ? Вы уверены ?"));
    
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return;    
    if(data->rk.mdd == 1) //Двойной документ
     {
      int tipzp=1;
      if(data->rk.tipz == 1)
       tipzp=2;

      if(matuddw(1,0,1,tipzp,data->rk.dd,data->rk.md,data->rk.gd,data->rk.nomon.ravno(),
      data->rk.skl1,data->nomkar.ravno_atoi(),data->kodmat.ravno_atoi(),data->window) != 0)
        return;
     }      

    matuddw(1,0,1,data->rk.tipz,data->rk.dd,data->rk.md,data->rk.gd,data->rk.nomdok.ravno(),
    data->rk.skk,data->nomkar.ravno_atoi(),data->kodmat.ravno_atoi(),data->window);

    deletdokum(&data->imaf);

    dokmat_create_list(data);
    
    return;  

  case SFK3:
    if(prkor(0,data->rk.md,data->rk.gd,data->rk.blokpid,data->rk.loginrash,data->window) != 0)
      return;  
  
    
    if(iceb_menu_danet(gettext("Удалить документ ? Вы уверены ?"),2,data->window) == 2)
      return;    

    deletdokum(&data->imaf);
    

    if(udpardok(data->rk.mdd,data->rk.tipz,data->rk.dd,data->rk.md,data->rk.gd,
    data->rk.skk,data->rk.nomdok.ravno(),data->rk.skl1,data->rk.nomon.ravno(),data->window) != 0)
      return;    
       
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;  
    
  
  case FK4:
    if(data->rk.metkaprov == 1 || iceb_u_SRAV(data->rk.nomdok.ravno(),"000",0) == 0)
     {
      iceb_menu_soob(gettext("Для этой операции проводки не делаются !"),data->window);
      return;
     }


    if(mvnp != 0 && data->rk.tipz == 1)
     {
      iceb_menu_soob(gettext("Для операций внутренних перемещений и изменения стоимости,\nпроводки делаются только для расходных документов !"),data->window);
      return;
     }    

    l_prov_mu(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,
    data->rk.skl1,
    data->rk.tipz,
    data->rk.nomdok.ravno(),
    data->rk.nomon.ravno(),
    data->rk.kpos.ravno(),data->rk.kprr.ravno(),data->rk.lnds,data->rk.sumkor,data->rk.pnds,data->window);
    return;  
  
  case FK5:
    
    if(data->kolzap == 0 && data->suma == 0.)
     return;    

    sozf(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),
    data->rk.tipz,data->rk.lnds,&data->imaf,data->ves,mvnp,data->rk.naimo.ravno(),data->window);

    if(data->rk.tipz == 1)
      dokmat_ras1(data);
    if(data->rk.tipz == 2)
      dokmat_ras2(data);
    return;  

  case SFK5:
    if(data->kolzap == 0 && data->suma == 0.)
     return;    

    sozf(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),
    data->rk.tipz,data->rk.lnds,&data->imaf,data->ves,mvnp,data->rk.naimo.ravno(),data->window);
  
    if(data->rk.tipz == 1)
      dokmat_pros1(data);
    if(data->rk.tipz == 2)
      dokmat_pros2(data);
    return;  

  case FK6:
    l_zkartmat(data->rk.skk,data->nomkar.ravno_atoi(),data->window);
    return;  

  case SFK6:
    l_matusl(data->rk.dd,data->rk.md,data->rk.gd,data->rk.nomdok.ravno(),data->rk.skk,data->rk.pnds,data->window);

    dokmat_create_list(data);

    return;  
  
  case FK7:
    if(data->kolzap == 0)
     return;    

    if(prkor(0,data->rk.md,data->rk.gd,data->rk.blokpid,data->rk.loginrash,data->window) != 0)
      return;  
    
    if(data->nomkar.ravno_atoi() == 0)
     {
      int voz;
      int n_kart;
      voz=l_kartmatv(data->rk.tipz,data->rk.dd,data->rk.md,data->rk.gd,data->rk.nomdok.ravno(),data->cenavdok,
      &data->nomzak,data->kodmat.ravno_atoi(),data->rk.skk,&n_kart,data->kolih_dok,1,data->rk.pnds,data->window);

      if(data->nomzak.getdlinna() > 1 && voz != 0)
       {
        iceb_u_str repl;
        repl.plus(gettext("Показать все карточки по этому материалу ?"));
        if(iceb_menu_danet(&repl,2,data->window) == 1)
         {
          iceb_u_str nomzak;
          nomzak.plus("");
          l_kartmatv(data->rk.tipz,data->rk.dd,data->rk.md,data->rk.gd,data->rk.nomdok.ravno(),data->cenavdok,
          &nomzak,data->kodmat.ravno_atoi(),data->rk.skk,&n_kart,data->kolih_dok,1,data->rk.pnds,data->window);
         }         
       }
      
      deletdokum(&data->imaf);
      dokmat_create_list(data);
      return;     
     }

    if(podzapm(data->rk.dd,data->rk.md,data->rk.gd,data->rk.tipz,data->rk.skk,data->rk.nomdok.ravno(),
    data->kodmat.ravno_atoi(),data->nomkar.ravno_atoi(),data->kolih_dok,data->cenavdok,data->window) == 0)
     {
      dokmat_create_list(data);
      if(data->rk.skl1 != 0 )
       {
        repl.new_plus(gettext("Не забудте подтвердить соответствующюю запись и в парном документе !"));
        iceb_menu_soob(&repl,data->window);
       }

     }


    return;  
  
  case SFK7:

    gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

    podtdokw(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),
    data->rk.tipz,data->rk.skl1,data->rk.nomon.ravno(),data->window);

    dokmat_create_list(data);
    return;  
  
  case FK8:
    dokmat_F8(data);
    return;  

  case FK9:
    if(data->rk.tipz == 1)
      dokmat_F9_prih(data);
    if(data->rk.tipz == 2)
      dokmat_F9_rash(data);
    return;  

    
  case FK10:
    if(dokmat_vihod(data) != 0)
     return;
    //data->voz=0; определяется в dokmat_vihod
    gtk_widget_destroy(data->window);
    return;

  case FK11:
    if(mdd == 0)
     return;
    if(iceb_menu_danet(gettext("Перейти в парный документ ? Вы уверены ?"),2,data->window) == 2)
       return;
    data->metka_pros_par_dok=1;    
    gtk_widget_destroy(data->window);
    return;  
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   dokmat_key_press(GtkWidget *widget,GdkEventKey *event,class dokmat_data *data)
{
iceb_u_str repl;
//printf("dokmat_key_press keyval=%d state=%d\n",
//event->keyval,event->state);

switch(event->keyval)
 {
  case GDK_F11:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK11]),"clicked");
    return(TRUE);

  case GDK_F1:
    if(data->kl_shift == 1) //Нажата
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK1]),"clicked");
//    else
//      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK1]),"clicked");
    return(TRUE);

  case GDK_KP_Add: //Нажата клавиша "+" на дополнительной клавиатуре
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
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
    return(TRUE);

  case GDK_F5:
    if(data->kl_shift == 1) //Нажата
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK5]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
    return(TRUE);

  case GDK_F6:
    if(data->kl_shift == 1) //Нажата
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK6]),"clicked");
    else
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
    return(TRUE);

  case GDK_F7:
    if(data->kl_shift == 1) //Нажата
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK7]),"clicked");
    else
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
    //printf("dokmat_key_press-Нажата клавиша Shift\n");

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
void dokmat_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class dokmat_data *data)
{
//printf("dokmat_v_row\n");
//data->metkazapisi=1;
gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
}

/************************************************/
/*Проверка возможности корректировать накладную*/
/************************************************/
/*Если вернули 0 - можно корректировать
	       1 - нет
*/

short           prkor(short mp, //0-все проверять 1-только блокировку накладной
short md,short gd,int blokpid,int loginrash,GtkWidget *wpredok)
{
struct  passwd  *ktoz; /*Кто записал*/
int		bbb;
char		strsql[512];
/*
printw("\nprkor=%d %d.%d %d\n",mp,md,gd,blokpid);
refresh();
*/
if(blokpid != 0 )
 {
  bbb=blokpid;
  
  if((ktoz=getpwuid(bbb)) != NULL)
     sprintf(strsql,"%s %d %s !",gettext("Документ заблокирован"),blokpid,ktoz->pw_gecos);
  else
     sprintf(strsql,"%s %d !",gettext("Заблокировано неизвестным логином"),bbb);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

if(mp == 1)
 return(0);

if(loginrash != 0 )
 {
  bbb=loginrash;
  iceb_u_str repl;
    
  repl.plus(gettext("Изменения не возможны ! Распечатан чек !"));
  if((ktoz=getpwuid(bbb)) != NULL)
     sprintf(strsql,"%d %s",loginrash,ktoz->pw_gecos);
  else
     sprintf(strsql,"%s %d !",gettext("Заблокировано неизвестным логином"),loginrash);
     
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

if(iceb_pbpds(md,gd,wpredok) != 0)
 return(1);

return(0);
}


/********************************/
/*Если файлов нет то создаем их*/
/********************************/
void sozf(short dg,short mg,short gg,int skl,const char *nomdok,
int tipz,int lnds,iceb_u_spisok *imaf,double ves,
short mvnp,  //0-внешняя 1 -внутреняя 2-изменение стоимости
const char *naimo, //Наименование контрагента
GtkWidget *wpredok)
{
FILE            *ff;

/*Создание накладной*/

if((ff = fopen(imaf->ravno(0),"r")) == NULL)
 {
  if(errno != ENOENT)
   iceb_er_op_fil(imaf->ravno(0),"",errno,NULL);

  if(errno == ENOENT)
   {


    if((ff = fopen(imaf->ravno(0),"w")) == NULL)
      iceb_er_op_fil(imaf->ravno(0),"",errno,NULL);
    iceb_u_startfil(ff);

    rasdokw(dg,mg,gg,skl,nomdok,imaf->ravno(0),lnds,0,ff,ves,wpredok);
    fclose(ff);
   }
 }
else
  fclose(ff);


/*Сделать анализ операции*/
if(mvnp != 0)
 return;

/*Создание счет-фактуры*/
/*****************************
if((ff = fopen(imaf->ravno(1),"r")) == NULL)
 {
  if(errno != ENOENT)
    iceb_er_op_fil(imaf->ravno(1),"",errno,NULL);

  if(errno == ENOENT)
   {  
    rasnalnw(dg,mg,gg,skl,nomdok,imaf->ravno(1),lnds,wpredok);
   }
 }
else
  fclose(ff);
****************************/
if(tipz == 1)
  return;

/*Создание счета на оплату*/
if((ff = fopen(imaf->ravno(2),"r")) == NULL)
 {
  if(errno != ENOENT)
   {
    iceb_er_op_fil(imaf->ravno(2),"",errno,NULL);
    return;
   }
  if(errno == ENOENT)
   {  
    if((ff = fopen(imaf->ravno(2),"w")) == NULL)
      iceb_er_op_fil(imaf->ravno(2),"",errno,NULL);
    iceb_u_startfil(ff);
    rasdokw(dg,mg,gg,skl,nomdok,imaf->ravno(2),lnds,1,ff,ves,wpredok);
    fclose(ff);
   }
 }
else
  fclose(ff);

}

/*****************************/
/*Создание счет-фактуры*/
/*****************************/
int l_dokmat_snn(const char *imaf,
short dg,short mg,short gg,
int skl,
const char *nomdok,
int lnds,
GtkWidget *wpredok)
{
FILE *ff;

/*Создание счет-фактуры*/
if((ff = fopen(imaf,"r")) == NULL)
 {
  if(errno != ENOENT)
   {
    iceb_er_op_fil(imaf,"",errno,NULL);
    return(1);
   }
  if(errno == ENOENT)
   {  
    return(rasnalnw(dg,mg,gg,skl,nomdok,imaf,lnds,wpredok));
   }
 }
else
  fclose(ff);
return(0);
}






/*********************************/
/*Удаление всех файлов документов*/
/**********************************/

void		deletdokum(iceb_u_spisok *imaf)
{
for(int i=0; i < imaf->kolih() ; i++)
  unlink(imaf->ravno(i));

}


/*****************************/
/*Создание файла акта приемки*/
/*****************************/
void sozfap(short dg,short mg,short gg,int skl,const char *nomdok,int lnds,const char *imaf,GtkWidget *wpredok)
{
FILE		*ff;
double		ves=0.;


if((ff = fopen(imaf,"r")) == NULL)
 {
  if(errno != ENOENT)
   {
    iceb_er_op_fil(imaf,"",errno,NULL);
    return;
   }
  if(errno == ENOENT)
   {


    if((ff = fopen(imaf,"w")) == NULL)
      iceb_er_op_fil(imaf,"",errno,NULL);
    iceb_u_startfil(ff);

    rasdokw(dg,mg,gg,skl,nomdok,imaf,lnds,2,ff,ves,wpredok);
    fclose(ff);
   }
 }
else
 fclose(ff);
}

/****************************/
/* Удаление ДОКУМЕНТА*/
/*****************************/
/*Если вернуди 0- порядок
               1- не удалили
*/

int udpardok(short mdd, //0-обычный документ 1-двойной
int tipz, //1-приход 2-расход
short dd,short md,short gd, //дата документа
int skl,const char *nomdok,
int skl1,
const char *nomdokp,
GtkWidget *wpredok)
{
class iceb_clock kk(wpredok);
if(mdd == 1) //Двойной документ
 {
  if(tipz == 2) //Расход
   {        
    if(provudvdokw(skl1,dd,md,gd,nomdokp,0,0,wpredok) != 0)
      return(1);

    //Удаляем в приходном
    if(matuddw(0,0,0,1,dd,md,gd,nomdokp,skl1,0,0,wpredok) != 0)
      return(1);

   }
  if(tipz == 1) //Приход
   {

    if(iceb_udprgr(gettext("МУ"),dd,md,gd,nomdokp,skl1,2,wpredok) != 0)
     return(1);
    //Удаляем в расходном
    if(matuddw(0,0,0,2,dd,md,gd,nomdokp,skl1,0,0,wpredok) != 0)
      return(1);
   }
 }

if(iceb_udprgr(gettext("МУ"),dd,md,gd,nomdok,skl,tipz,wpredok) != 0)
 return(1);

//Удаляем документ из которого удаляем
if(matuddw(0,0,0,tipz,dd,md,gd,nomdok,skl,0,0,wpredok) != 0)
  return(1);
return(0);
}

/************************/
/*Формирование шапки меню*/
/***************************/
void dokmat_sapka_menu(class dokmat_data *data)
{
char strsql[1024];

class iceb_u_str stroka;

stroka.new_plus(gettext("Работа с документом материального учёта"));
stroka.ps_plus(gettext("Склад"));
stroka.plus(":");
stroka.plus(data->rk.skk);
stroka.plus(" ");
stroka.plus(data->rk.naiskl.ravno());

stroka.plus(" ");
stroka.plus(gettext("Дата"));
stroka.plus(":");
stroka.plus(data->rk.dd);
stroka.plus(".");
stroka.plus(data->rk.md);
stroka.plus(".");
stroka.plus(data->rk.gd);

stroka.plus(" ");
stroka.plus(gettext("Номер документа"));
stroka.plus(":");
stroka.plus(data->rk.nomdok.ravno());


sprintf(strsql," %s:%d/%.10g",gettext("Количество записей"),
data->kolzap,data->kol_mat);
stroka.plus(strsql);


stroka.ps_plus(gettext("Контрагент"));
stroka.plus(":");
stroka.plus(data->rk.kpos.ravno());
stroka.plus("/");
stroka.plus(data->rk.naimo.ravno());

stroka.ps_plus(gettext("Операция"));
stroka.plus(":");
stroka.plus(data->rk.kprr.ravno());
stroka.plus("/");
stroka.plus(data->rk.naimpr.ravno());

stroka.plus(" ");
if(data->rk.tipz == 1)
 {
  stroka.plus("(");
  stroka.plus(gettext("Приход"));
  stroka.plus(")");
 }
if(data->rk.tipz == 2)
 {
  stroka.plus("(");
  stroka.plus(gettext("Расход"));
  stroka.plus(")");
 }
if(mdd == 1)
 { 
  stroka.plus(" ");
  stroka.plus(gettext("Двойной документ"));
  stroka.plus(" !!!");
 } 
 
if(data->rk.skl1 > 0)
 {
  stroka.ps_plus(gettext("Номер парного документа"));
  stroka.plus(":");
  stroka.plus(data->rk.nomon.ravno());
 }
if(data->naimkor.getdlinna() > 1)
 {
  stroka.ps_plus(gettext("Корректировка"));
  stroka.plus(":");
  stroka.plus(data->naimkor.ravno());
 }


/*Читаем заблокирован документ для выписки или нет*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and sklad=%d and nomd='%s' and nomerz=4",
data->rk.gd,data->rk.skk,data->rk.nomdok.ravno());

if(sql_readkey(&bd,strsql) == 1)
 {
  stroka.ps_plus(gettext("Документ заблокирован для программы выписки"));
 }


if(data->rk.loginrash != 0)
 {
  stroka.ps_plus(gettext("Кассовый чек распечатал"));
  stroka.plus(" ");
  stroka.plus(data->rk.loginrash);

 }


if(data->rk.blokpid != 0)
 {
  struct  passwd  *ktoz; /*Кто записал*/
  stroka.ps_plus(gettext("Заблокировал"));
  stroka.plus(" ");
  stroka.plus(data->rk.blokpid);
  stroka.plus(" ");
  if((ktoz=getpwuid(data->rk.blokpid)) != NULL)
    stroka.plus(ktoz->pw_gecos);
  else
    stroka.plus(gettext("Неизвестный логин"));
  
 }


if(data->metka_usl != 0)
 {
  stroka.ps_plus(gettext("Для документа введены услуги"));
 }


if(data->rk.pro == 0)
 {
  stroka.ps_plus(gettext("Проводки не сделаны"));
 }


if(data->maskor.kolih() != 0 && data->naimkor.getdlinna() > 1)
   data->rk.sumkor=makkorrw(data->suma+data->sumabn,&data->maskor);


double suma_bnds=iceb_u_okrug(data->suma,okrg1);
double bb=data->suma+(data->rk.sumkor);

double suma_nds=0.; //Сумма НДС
if(data->rk.tipz == 1 && data->rk.sumandspr != 0.)
  suma_nds=data->rk.sumandspr;
else
  suma_nds=bb*data->rk.pnds/100.;
bb=bb+suma_nds;
double suma_snds=iceb_u_okrug(bb,okrg1); //сумма с ндс

double suma_zap_bnds=iceb_u_okrug(data->sumabn,okrg1); //Сумма записей без ндс

float proc=0.;

if(data->rk.lnds == 0)
 {
  sprintf(strsql,"%s:%.2f",gettext("Сумма всех записей"),suma_bnds);

  stroka.ps_plus(strsql);
  
  if(data->rk.tipz == 1 && data->rk.sumandspr != 0.)
   {
    sprintf(strsql,"/%.2f/",suma_nds);
    stroka.plus(strsql);
   }
  else
   {
    sprintf(strsql,"/%.2f/",suma_nds);
    stroka.plus(strsql);
   }
  
  if(suma_zap_bnds > 0)
   {
    sprintf(strsql,"%s %.2f/",gettext("Без НДС"),suma_zap_bnds);
    stroka.plus(strsql);
   }
  if(data->rk.sumkor != 0)
   {
    if(data->rk.sumkor > 0.)
      proc=(data->rk.sumkor)*100./suma_bnds;
    if(data->rk.sumkor < 0.)
      proc=100.*(data->rk.sumkor)/(suma_bnds-(data->rk.sumkor)*-1);
    proc=iceb_u_okrug(proc,0.1);
    sprintf(strsql,"КС %.2f %.1f%% / ",data->rk.sumkor,proc);
    stroka.plus(strsql);
   }   
  data->sumkopl=suma_snds+suma_zap_bnds;
  sprintf(strsql,"%.2f(%.2f)",data->sumkopl,data->sumsnds);
  stroka.plus(strsql);

  if(data->ves > 0.09)
   {
    sprintf(strsql," %s:%.1f",gettext("Вес"),data->ves);
    stroka.plus(strsql);
   }
 }
else
 {

  data->sumkopl=suma_bnds+suma_zap_bnds;
  sprintf(strsql,"%s:%.2f(%.2f) %s",
  gettext("Сумма всех записей"),data->sumkopl,data->sumsnds,
  gettext("Без НДС"));

  stroka.ps_plus(strsql);

  if(data->ves != 0.)
   {
    sprintf(strsql," %s:%.1f",
    gettext("Вес"),data->ves);

    stroka.plus(strsql);
   }
  if(data->rk.sumkor != 0.)
   {
    proc=(data->rk.sumkor)*100./(suma_bnds+suma_zap_bnds);
    proc=iceb_u_okrug(proc,0.1);
    sprintf(strsql," КС %.2f %.1f%%",data->rk.sumkor,proc);

    stroka.plus(strsql);
    
    data->sumkopl+=data->rk.sumkor;
    sprintf(strsql,"/%.2f",data->sumkopl);

    stroka.plus(strsql);

   }   
 }

gtk_label_set_text(GTK_LABEL(data->label_kolstr),stroka.ravno_toutf());

/*Корректировка на сумму накладной*/
sprintf(strsql,"delete from Dokummat2 \
where god=%d and sklad=%d and nomd='%s' and nomerz=13",
data->rk.gd,data->rk.skk,data->rk.nomdok.ravno());

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR) //У оператора доступ только на чтение
   iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);

if(fabs(data->rk.sumkor) > 0.009)
 {
  sprintf(strsql,"insert into Dokummat2 \
values (%d,%d,'%s',%d,'%.2f')",
data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),13,data->rk.sumkor);
  if(sql_zap(&bd,strsql) != 0)
   iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,data->window);
 }   

}
/*******************************/
/*Завершение работы в документе*/
/*******************************/
//Eсли вернули 0 можно выходить из документа
int dokmat_vihod(class dokmat_data *data)
{
iceb_clock kk(data->window);
char strsql[512];
data->voz=0;
if(mdd == 1 && mvnp > 0) /*Двойной документ*/
 {
  double bb=provkolw(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skl1,data->rk.nomon.ravno(),data->window);
  //printf("bb=%f kol_mat=%f skl1=%d\n",bb,data->kol_mat,data->rk.skl1);
  if(fabs(bb-data->kol_mat) > 0.00000001)
   {
    iceb_u_str repl;              
    repl.plus(gettext("Другое количество материалла во встречном документе !"));
    
    sprintf(strsql,"%s:%d.%d.%d %s:%d %s:%s",
    gettext("Дата"),data->rk.dd,data->rk.md,data->rk.gd,
    gettext("Склад"),data->rk.skl1,
    gettext("Номер документа"),data->rk.nomon.ravno());
    repl.ps_plus(strsql);

    sprintf(strsql,"%.10g != %.10g",bb,data->kol_mat);
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,data->window);
   }
 } 

if(data->kolzap == 0 && data->suma == 0.)
 {
  iceb_u_str repl;
  repl.plus(gettext("Удалить документ ?"));

  if(iceb_menu_danet(&repl,2,data->window) == 1)
   {
    if(iceb_udprgr(gettext("МУ"),data->rk.dd,data->rk.md,data->rk.gd,
    data->rk.nomdok.ravno(),data->rk.skk,data->rk.tipz,data->window) != 0)
     return(1);

    if(udpardok(data->rk.mdd,data->rk.tipz,data->rk.dd,data->rk.md,data->rk.gd,
    data->rk.skk,data->rk.nomdok.ravno(),data->rk.skl1,data->rk.nomon.ravno(),data->window) != 0)
      return(1);    
    deletdokum(&data->imaf);
    data->voz=1;
    return(0);
   }
  return(0);
 }



podvdokw(data->rk.dd,data->rk.md,data->rk.gd,data->rk.nomdok.ravno(),data->rk.skk,data->window);

oplmuw(data->rk.dd,data->rk.md,data->rk.gd,data->rk.tipz,data->rk.nomdok.ravno(),data->rk.skk,data->rk.kprr.ravno(),data->window);

if(data->rk.metkaprov == 1)
  return(0);

if(iceb_u_SRAV(data->rk.nomdok.ravno(),"000",0) == 0)
  return(0);

/*Если операция внутренняя и накладная на приход
Проводки выполняются только для расходных накладных*/

if(mvnp != 0 && data->rk.tipz == 1)
  return(0);


prosprw(3,data->rk.skk,data->rk.dd,data->rk.md,data->rk.gd,data->rk.nomdok.ravno(),
data->rk.tipz,data->rk.lnds,data->rk.skl1,data->rk.nomon.ravno(),data->rk.kprr.ravno(),NULL,data->window);

if(data->rk.tipz == 2)
 {
  //Проверяем подтверждён документ или нет и сделаны к нему проводки или нет
  sprintf(strsql,"select pod,pro from Dokummat where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
  data->rk.gd,data->rk.md,data->rk.dd,data->rk.skk,data->rk.nomdok.ravno());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   {
    if(atoi(row[0]) == 1 && atoi(row[1]) == 0)
     {
      char spis_op[1024];
      memset(spis_op,'\0',sizeof(spis_op));
      if(iceb_poldan("Коды операций расходов для которых запрещен выход из документа без проводок",spis_op,"matnast.alx",data->window) == 0)
       if(iceb_u_proverka(spis_op,data->rk.kprr.ravno(),0,1) == 0)
        {
         iceb_menu_soob(gettext("Не сделаны проводки ! Выход из документа запрещён !"),data->window);         
         return(1);
        }
     }
   }

 }


return(0);

}
/*************************/
/*Распечатка для приходных документов*/
/************************/

void dokmat_ras1(class dokmat_data *data)
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;


titl.plus(gettext("Распечатка документов"));





punkt_m.plus(gettext("Распечатка накладной"));//0
punkt_m.plus(gettext("Распечатка накладной на мед-препараты"));//1
punkt_m.plus(gettext("Распечатка акта приемки"));//2
punkt_m.plus(gettext("Распечатка ценников для товаров"));//3
punkt_m.plus(gettext("Распечатка накладной с ценами учета/реализации"));//4
punkt_m.plus(gettext("Распечатка акта приемки-передачи"));//5
punkt_m.plus(gettext("Распечатка кассового ордера"));//6
punkt_m.plus(gettext("Удалить распечатки документов"));//7
punkt_m.plus(gettext("Распечатка счет-фактуры"));//8

zagolovok.new_plus(gettext("Выберите нужное"));

/***********
char stroka[1024];

memset(stroka,'\0',sizeof(stroka));
char *nameprinter = getenv("PRINTER");
if(nameprinter != NULL)
 sprintf(stroka,"%s:%-20.20s",gettext("Принтер"),nameprinter);
else
 sprintf(stroka,"%s:%-20.20s",gettext("Принтер"),gettext("По умолчанию"));

zagolovok.ps_plus(stroka);
*****************/
int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);
iceb_u_spisok imafils;
iceb_u_spisok naimf;
switch(nomer)
 {
  case 0:
    iceb_pehf(data->imaf.ravno(0),0,data->window);
    break;

  case 1:
    rasmedprw(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),data->window);
    break;

  case 2:
    sozfap(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),
    data->rk.lnds,data->imaf.ravno(4),data->window);


    imafils.plus(data->imaf.ravno(4));
    naimf.plus(gettext("Распечатка акта приемки"));

    iceb_rabfil(&imafils,&naimf,"",0,data->window);
    break;

  case 3:
    cennikdw(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),data->window);
    break;

  case 4:
    rasdok1w(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),data->rk.lnds,
    data->ves,data->window);
    break;

  case 5:
    rasactmuw(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),data->window);
    break;

  case 6:

    iceb_kasord1(data->imaf.ravno(3),"",data->rk.dd,data->rk.md,data->rk.gd,"","",
    data->sumkopl,data->rk.naimo.ravno(),data->rk.osnovanie.ravno(),"","",data->window);

    imafils.plus(data->imaf.ravno(3));
    naimf.plus(gettext("Распечатка кассового ордера"));

    iceb_rabfil(&imafils,&naimf,"",0,data->window);

  case 7:
    deletdokum(&data->imaf);
    break;

  case 8:
    if(mvnp != 0)
     {
      iceb_u_str repl;
      repl.plus(gettext("Для этой операции счет-фактура не печатается !"));
      iceb_menu_soob(&repl,data->window);
      break;
     } 
    if(l_dokmat_snn(data->imaf.ravno(1),data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),data->rk.lnds,data->window) != 0)
     break;
    iceb_pehf(data->imaf.ravno(1),0,data->window);

    break;
 }




}
/*************************************/
/*Распечатки для расходных документов*/
/*************************************/

void dokmat_ras2(class dokmat_data *data)
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатка документов"));




punkt_m.plus(gettext("Распечатка накладной"));//0
punkt_m.plus(gettext("Распечатка счет-фактуры"));//1
punkt_m.plus(gettext("Распечатка счёта"));//2
punkt_m.plus(gettext("Распечатка кассового ордера"));//3
punkt_m.plus(gettext("Распечатка счета в две колонки"));//4
punkt_m.plus(gettext("Распечатка пропуска"));//5
punkt_m.plus(gettext("Распечатка акта списания"));//6
punkt_m.plus(gettext("Распечатка накладной на мед-препараты"));//7
punkt_m.plus(gettext("Распечатка ценников для товаров"));//8
punkt_m.plus(gettext("Распечатка акта приемки"));//9
punkt_m.plus(gettext("Распечатка накладной с ценами учета/реализации"));//10
punkt_m.plus(gettext("Распечатка акта приемки-передачи"));//11
punkt_m.plus(gettext("Удалить распечатки документов"));//12

zagolovok.new_plus(gettext("Выберите нужное"));


/**************
char stroka[1024];
memset(stroka,'\0',sizeof(stroka));
char *nameprinter = getenv("PRINTER");
if(nameprinter != NULL)
 sprintf(stroka,"%s:%-20.20s",gettext("Принтер"),nameprinter);
else
 sprintf(stroka,"%s:%-20.20s",gettext("Принтер"),gettext("По умолчанию"));

zagolovok.ps_plus(stroka);
**************/
int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);

iceb_u_spisok imafils;
iceb_u_spisok naimf;
switch(nomer)
 {
  case 0:
    iceb_pehf(data->imaf.ravno(0),0,data->window);
    break;

  case 1:
    if(mvnp != 0)
     {
      iceb_u_str repl;
      repl.plus(gettext("Для этой операции счет-фактура не печатается !"));
      iceb_menu_soob(&repl,data->window);
      break;
     } 
    if(l_dokmat_snn(data->imaf.ravno(1),data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),data->rk.lnds,data->window) != 0)
     break;
    iceb_pehf(data->imaf.ravno(1),0,data->window);

    break;

  case 2:
    if(mvnp != 0)
     {
      iceb_u_str repl;
      repl.plus(gettext("Для этой операции счета не распечатываются !"));
      iceb_menu_soob(&repl,data->window);
      break;
     }
    iceb_pehf(data->imaf.ravno(2),0,data->window);

    break;

  case 3:
    
    iceb_kasord2(data->imaf.ravno(3),"",data->rk.dd,data->rk.md,data->rk.gd,"","",
    data->sumkopl,data->rk.naimo.ravno(),data->rk.osnovanie.ravno(),"","","",NULL,data->window);

    imafils.plus(data->imaf.ravno(3));
    naimf.plus(gettext("Распечатка кассового ордера"));

    iceb_rabfil(&imafils,&naimf,"",0,data->window);

//    iceb_pehf(data->imaf.ravno(3),1,data->window);
    break;

  case 4:
    rasdok2w(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),
    data->rk.lnds,data->window);
    break;

  case 5:
    raspropw(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),data->window);
    break;

  case 6:

    rasdokw(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),
    data->imaf.ravno(5),data->rk.lnds,3,NULL,data->ves,data->window);

    imafils.plus(data->imaf.ravno(5));
    naimf.plus(gettext("Распечатка акта списания"));

    iceb_rabfil(&imafils,&naimf,"",0,data->window);
    break;


  case 7:
    rasmedprw(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),data->window);
    break;

  case 8:
    cennikdw(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),data->window);
    break;

  case 9:

    sozfap(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),
    data->rk.lnds,data->imaf.ravno(4),data->window);

    imafils.plus(data->imaf.ravno(4));
    naimf.plus(gettext("Распечатка акта приемки"));

    iceb_rabfil(&imafils,&naimf,"",0,data->window);
    break;

  case 10:
    rasdok1w(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),data->rk.lnds,data->ves,data->window);
    break;

  case 11:
    rasactmuw(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),data->window);
    break;
    
  case 12:
    deletdokum(&data->imaf);
    break;
 
 }




}
/****************************/
/*Меню для клавиши F8*/
/********************************/
void dokmat_F8(class dokmat_data *data)
{
char strsql[524];
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Установить/снять блокировку даты"));//0
punkt_m.plus(gettext("Отметить запись"));//1
punkt_m.plus(gettext("Найти нужный код материалла"));//2
if(srtnk == 0)
  punkt_m.plus(gettext("Включить сортировку записей в распечатке"));//3
if(srtnk == 1)
  punkt_m.plus(gettext("Выключить сортировку записей в распечатке"));//3

if(data->rk.blokpid == 0)
  punkt_m.plus(gettext("Заблокировать документ"));//4
if(data->rk.blokpid != 0)
  punkt_m.plus(gettext("Разблокировать документ"));//4

punkt_m.plus(gettext("Снять подтверждение со всего документа"));//5

if(obzap == 0)
  punkt_m.plus(gettext("Включить объединение одинаковых мат-лов в распечатке"));//6
if(obzap == 1)
  punkt_m.plus(gettext("Выключить объединение одинаковых мат-лов в распечатке"));//6

punkt_m.plus(gettext("Ввести сумму или процент корректировки"));//7
punkt_m.plus(gettext("Копировать записи из других документов"));//8

if(data->rk.loginrash != 0)
  punkt_m.plus(gettext("Снятие метки распечатанного чека"));//9
if(data->rk.loginrash == 0)
  punkt_m.plus(gettext("Установка метки распечатанного чека"));//9

punkt_m.plus(gettext("Добавить к наименованию материалла"));//10

int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);

iceb_u_spisok imafils;
iceb_u_spisok naimf;
char dnaim[512];
SQL_str row;
SQLCURSOR cur;
switch(nomer)
 {

  case 0:
    iceb_f_redfil("blokmak.alx",0,NULL);
    break;
  
  case 1:

    if(dokmat_pom_zap(data) == 0)
      dokmat_create_list(data);
    
    break;

  case 2:
    
    titl.new_plus(gettext("Введите код материалла"));
    zagolovok.new_plus("");
    
    if(iceb_menu_vvod1(&titl,&zagolovok,20,data->window) != 0)
     return;
    data->kodmat_v=zagolovok.ravno_atoi();
    dokmat_create_list(data);
    break;

  case 3:

   deletdokum(&data->imaf);
   srtnk++;
   if(srtnk == 2)
      srtnk=0;
   break;

  case 4:
//    if(data->rk.blokpid != (int) getuid())
//     if(prkor(0,data->rk.md,data->rk.gd,data->rk.blokpid,data->rk.loginrash,data->window) != 0)

    if(data->rk.blokpid != 0 && data->rk.blokpid != iceb_getuid(data->window))
     if(iceb_parol(0,data->window) != 0)  
        return;

    if(data->rk.blokpid == 0)
     data->rk.blokpid=iceb_getuid(data->window);
    else
     data->rk.blokpid=0;

    memset(strsql,'\0',sizeof(strsql)); 
    sprintf(strsql,"delete from Dokummat2 \
where god=%d and sklad=%d and nomd='%s' and nomerz=12",
    data->rk.gd,data->rk.skk,data->rk.nomdok.ravno());

    if(sql_zap(&bd,strsql) != 0)
     iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);

    if(data->rk.blokpid != 0.)
     {
      sprintf(strsql,"insert into Dokummat2 \
values (%d,%d,'%s',%d,'%d')",
      data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),12,data->rk.blokpid);

      if(sql_zap(&bd,strsql) != 0)
       iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,data->window);
     }

    dokmat_create_list(data);
      
   break;

  case 5: //Снятие подтверждения со всех записей в документе
    if(data->rk.blokpid != iceb_getuid(data->window))
     if(prkor(0,data->rk.md,data->rk.gd,data->rk.blokpid,data->rk.loginrash,data->window) != 0)
        return;  

    sprintf(strsql,"delete from Zkart where datd='%d-%02d-%02d' \
and nomd='%s' and sklad=%d and tipz=%d",data->rk.gd,data->rk.md,data->rk.dd,data->rk.nomdok.ravno(),
    data->rk.skk,data->rk.tipz);
    if(sql_zap(&bd,strsql) != 0)
     iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);

    dokmat_create_list(data);

   break;

  case 6: //Включить/выключить объединение одинаковых материалов в распечатке накладной
   deletdokum(&data->imaf);
   obzap++;
   if(obzap == 2)
    obzap=0;
    
   break;

  case 7: //ввести сумму или процент корректировки

    titl.new_plus(gettext("Введите сумму или процент (%)"));
    zagolovok.new_plus("");
    if(iceb_menu_vvod1(&titl,&zagolovok,20,data->window) != 0)
      break;

    deletdokum(&data->imaf);
    data->rk.sumkor=zagolovok.ravno_atof();
    if(iceb_u_pole(zagolovok.ravno(),strsql,1,'%') == 0)
      data->rk.sumkor=(data->suma+data->sumabn)*data->rk.sumkor/100.;
    data->rk.sumkor=iceb_u_okrug(data->rk.sumkor,0.01);

    dokmat_create_list(data);

   break;
  
  case 8: //ввести сумму или процент корректировки
    if(copdokw(data->rk.dd,data->rk.md,data->rk.gd,data->rk.nomdok.ravno(),data->rk.skk,data->rk.tipz,data->window) == 0)
      dokmat_create_list(data);
    
    break;

  case 9: //Установка/снятие метки распечатанного чека

     if(iceb_parol(0,data->window) != 0)
      break;

     if(data->rk.loginrash != 0)        
      {
       sprintf(strsql,"delete from Dokummat2 where god=%d and \
nomd='%s' and sklad=%d and nomerz=%d",data->rk.gd,data->rk.nomdok.ravno(),data->rk.skk,5);
       data->rk.loginrash=0;
       //printf("%s\n",strsql);
       if(sql_zap(&bd,strsql) != 0)
         iceb_msql_error(&bd,"",strsql,data->window);
       dokmat_create_list(data);
       break; 
      }

     if(data->rk.loginrash == 0)        
      {
       sprintf(strsql,"insert into Dokummat2 \
values(%d,%d,'%s',%d,'%d')",data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),5,iceb_getuid(data->window));
       data->rk.loginrash=iceb_getuid(data->window);

       if(sql_zap(&bd,strsql) != 0)
         iceb_msql_error(&bd,"",strsql,data->window);
      }
     

     dokmat_create_list(data);

    break;

  case 10: //Добавить к наименованию материалла
    
    memset(dnaim,'\0',sizeof(dnaim));

    //Читаем дополнительное наименование в записи
    sprintf(strsql,"select dnaim from Dokummat1 where datd='%d-%02d-%02d' and sklad=%d and \
nomd='%s' and tipz=%d and kodm=%s and nomkar=%s",
    data->rk.gd,data->rk.md,data->rk.dd,data->rk.skk,data->rk.nomdok.ravno(),data->rk.tipz,
    data->kodmat.ravno(),data->nomkar.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
     {
      iceb_u_str repl;
      repl.plus(gettext("Не найдена запись для корректировки !"));
      repl.ps_plus(strsql);
      iceb_menu_soob(&repl,data->window);
      break;
     }
    
    strncpy(dnaim,row[0],sizeof(dnaim)-1);

    titl.new_plus(gettext("Введите дополнение к наименованию материалла"));

    if(iceb_menu_vvod1(&titl,dnaim,sizeof(dnaim),data->window) != 0)
     break;

    sqlfiltr(dnaim,sizeof(dnaim));

    sprintf(strsql,"update Dokummat1 set dnaim='%s' where \
datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%s and nomkar=%s",
    dnaim,data->rk.gd,data->rk.md,data->rk.dd,data->rk.skk,data->rk.nomdok.ravno(),
    data->kodmat.ravno(),data->nomkar.ravno());

    
    if(sql_zap(&bd,strsql) != 0)
     iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

    deletdokum(&data->imaf);
    dokmat_create_list(data);

    break;
   
 }

}
/****************************/
/*Меню для клавиши F9 приход*/
/********************************/
void dokmat_F9_prih(class dokmat_data *data)
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Оформить возврат"));//0
punkt_m.plus(gettext("Дооценить документ"));//1
punkt_m.plus(gettext("Ввести сумму НДС для документа"));//2
punkt_m.plus(gettext("Переценить документ на заданный процент"));//3

int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);
char bros[512];
char strsql[524];
switch(nomer)
 {
  case 0: //Оформить возврат
    rozkorw(data->rk.dd,data->rk.md,data->rk.gd,data->rk.tipz,data->rk.nomdok.ravno(),data->rk.skk,data->rk.pnds,data->window);
    break;

  case 1: //Дооценить документ
    doocw(data->rk.dd,data->rk.md,data->rk.gd,data->rk.tipz,data->rk.nomdok.ravno(),data->rk.skk,data->window);
    break;

  case 2: //Ввести сумму НДС для документа
    bros[0]='\0';
    if(data->rk.sumandspr != 0.)
      sprintf(bros,"%.2f",data->rk.sumandspr);

    if(iceb_menu_vvod1(gettext("Введите сумму НДС"),bros,20,data->window) != 0)
      break;

    data->rk.sumandspr=atof(bros);
    if(data->rk.sumandspr != 0.)
     sprintf(strsql,"replace into Dokummat2 values (%d,%d,'%s',6,'%s')",
     data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),bros);
    else
     sprintf(strsql,"delete from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=6",
     data->rk.gd,data->rk.skk,data->rk.nomdok.ravno());
     

    
    if(sql_zap(&bd,strsql) != 0)
     iceb_msql_error(&bd,"",strsql,data->window);

     deletdokum(&data->imaf);
     dokmat_create_list(data);
    break;

  case 3: //Переценить документ

    if(perecenpw(data->rk.dd,data->rk.md,data->rk.gd,data->rk.tipz,data->rk.nomdok.ravno(),data->rk.skk,data->window) != 0)
     break;

    deletdokum(&data->imaf);
    dokmat_create_list(data);
    break;


 }
}
/****************************/
/*Меню для клавиши F9 расход*/
/********************************/
void dokmat_F9_rash(class dokmat_data *data)
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Переценить документ на заданный процент"));//0
punkt_m.plus(gettext("Выбрать вид корректировки суммы по документу"));//1
punkt_m.plus(gettext("Распечатать остатки"));//2
punkt_m.plus(gettext("Снять привязку к карточкам"));//3
punkt_m.plus(gettext("Снять блокировку с документа для выписки"));//4

int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);

char strsql[524];
iceb_u_spisok naimf;

switch(nomer)
 {
  case 0: //Переценить документ
    if(perecenrw(data->rk.dd,data->rk.md,data->rk.gd,data->rk.tipz,data->rk.nomdok.ravno(),data->rk.skk,data->rk.pnds,data->window) != 0)
     break;

    deletdokum(&data->imaf);
    dokmat_create_list(data);
    break;

  case 1: //Установить вид скидки

    data->naimkor.new_plus("");
    punkt_m.free_class();
    if(makkorw(&punkt_m,1,&data->naimkor,&data->maskor,"makkor.alx") != 0)
     break;       

    if((nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,data->window)) < 0)
      break;
     
    data->naimkor.new_plus(punkt_m.ravno(nomer));
    makkorw(&punkt_m,2,&data->naimkor,&data->maskor,"makkor.alx");

    deletdokum(&data->imaf);
    dokmat_create_list(data);

    break;

  case 2: // Распечатать остатки
    rasoskrw(data->window);
    break;

  case 3: // Снять привязку к карточкам
    punkt_m.free_class();
    punkt_m.plus(gettext("Будут удалены все подтверждающие записи в карточках !"));
    punkt_m.plus(gettext("Снять привязку ? Вы уверены ?"));

    if(iceb_menu_danet(&punkt_m,2,data->window) == 2)
       break;
       
    sprintf(strsql,"delete from Zkart where datd='%d-%02d-%02d' \
and nomd='%s' and sklad=%d",data->rk.gd,data->rk.md,data->rk.dd,data->rk.nomdok.ravno(),data->rk.skk);

    if(sql_zap(&bd,strsql) != 0)
      iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);

    sprintf(strsql,"update Dokummat1 set nomkar=0 where \
datd='%d-%02d-%02d' and nomd='%s' and sklad=%d",
    data->rk.gd,data->rk.md,data->rk.dd,data->rk.nomdok.ravno(),data->rk.skk);

    if(sql_zap(&bd,strsql) != 0)
      iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,data->window);

    dokmat_create_list(data);

    break;

  case 4: // Снять блокировку с документа для выписки
     sprintf(strsql,"delete from Dokummat2 where god=%d and \
sklad=%d and nomd='%s' and nomerz=4",data->rk.gd,data->rk.skk,data->rk.nomdok.ravno());
    if(sql_zap(&bd,strsql) != 0)
      iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
    dokmat_create_list(data);
   break;
   

 }
}
/****************************/
/*Меню для пометки записей*/
/********************************/
int dokmat_pom_zap(class dokmat_data *data)
{

char strsql[512];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select nomkar,voztar,nds,mnds from Dokummat1 where \
datd='%d-%d-%d' and sklad=%d and nomd='%s' and kodm=%d and nomkar=%d",
data->rk.gd,data->rk.md,data->rk.dd,data->rk.skk,data->rk.nomdok.ravno(),
data->kodmat.ravno_atoi(),data->nomkar.ravno_atoi());
int i=0;
if((i=iceb_sql_readkey(strsql,&row,&cur,data->window)) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найдена запись для корректировки !"));
  repl.ps_plus(strsql);
  repl.plus("\n");
  repl.plus(i);
  iceb_menu_soob(&repl,data->window);
  return(1);
 }

int voztar=atoi(row[1]);
int nomer_kart=atoi(row[0]); 
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


if(voztar == 0)
   punkt_m.plus(gettext("Отметить как оборотную тару"));
if(voztar == 1)
   punkt_m.plus(gettext("Снять отметку оборотной тары"));

punkt_m.plus(gettext("Отметить с 0 проц. НДС"));

sprintf(strsql,gettext("Отметить с %.1f проц. НДС"),data->rk.pnds);
punkt_m.plus(strsql);

punkt_m.plus(gettext("Отметить с НДС в том числе"));
punkt_m.plus(gettext("Отметить без НДС в том числе"));

int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);

switch(nomer)
 {
  case -1:
    return(1);
  
  case 0:

     voztar++;
     if(voztar > 1)
       voztar=0;

    sprintf(strsql,"update Dokummat1 \
set voztar=%d where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' and \
kodm=%d and nomkar=%d",
       voztar,data->rk.gd,data->rk.md,data->rk.dd,data->rk.skk,data->rk.nomdok.ravno(),
       data->kodmat.ravno_atoi(),data->nomkar.ravno_atoi());
    
    iceb_sql_zapis(strsql,0,0,data->window);
    
    break;
 
  case 1:
    if(nomer_kart != 0)
     {
       iceb_u_str repl;
       repl.plus(gettext("Запись привязана к карточке"));
       repl.ps_plus(gettext("Корректировка не возможна !"));
       iceb_menu_soob(&repl,data->window);
       return(1);
     }
    
    sprintf(strsql,"update Dokummat1 \
set nds=%.10g where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' and \
kodm=%d and nomkar=%d",
    0.,data->rk.gd,data->rk.md,data->rk.dd,data->rk.skk,data->rk.nomdok.ravno(),
    data->kodmat.ravno_atoi(),data->nomkar.ravno_atoi());
    
    iceb_sql_zapis(strsql,0,0,data->window);
    
    break; 

  case 2:
    if(nomer_kart != 0)
     {
       iceb_u_str repl;
       repl.plus(gettext("Запись привязана к карточке"));
       repl.ps_plus(gettext("Корректировка не возможна !"));
       iceb_menu_soob(&repl,data->window);
       return(1);
     }
    
    sprintf(strsql,"update Dokummat1 \
set nds=%.10g where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' and \
kodm=%d and nomkar=%d",
    data->rk.pnds,data->rk.gd,data->rk.md,data->rk.dd,data->rk.skk,data->rk.nomdok.ravno(),
    data->kodmat.ravno_atoi(),data->nomkar.ravno_atoi());
    
    iceb_sql_zapis(strsql,0,0,data->window);
    
    break; 

  case 3:
    if(nomer_kart != 0)
     {
       iceb_u_str repl;
       repl.plus(gettext("Запись привязана к карточке"));
       repl.ps_plus(gettext("Корректировка не возможна !"));
       iceb_menu_soob(&repl,data->window);
       return(1);
     }
    
    sprintf(strsql,"update Dokummat1 \
set mnds=1 where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' and \
kodm=%d and nomkar=%d",
    data->rk.gd,data->rk.md,data->rk.dd,data->rk.skk,data->rk.nomdok.ravno(),
    data->kodmat.ravno_atoi(),data->nomkar.ravno_atoi());
    
    iceb_sql_zapis(strsql,0,0,data->window);
    
    break; 

  case 4:
    if(nomer_kart != 0)
     {
       iceb_u_str repl;
       repl.plus(gettext("Запись привязана к карточке"));
       repl.ps_plus(gettext("Корректировка не возможна !"));
       iceb_menu_soob(&repl,data->window);
       return(1);
     }
    
    sprintf(strsql,"update Dokummat1 \
set mnds=0 where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' and \
kodm=%d and nomkar=%d",
    data->rk.gd,data->rk.md,data->rk.dd,data->rk.skk,data->rk.nomdok.ravno(),
    data->kodmat.ravno_atoi(),data->nomkar.ravno_atoi());
    
    iceb_sql_zapis(strsql,0,0,data->window);
    
    break; 
 
 }
return(0);
}
/*************************/
/*Просмотр  для приходных документов*/
/************************/

void dokmat_pros1(class dokmat_data *data)
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Просмотр распечатки документов"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Редактирование накладной"));
punkt_m.plus(gettext("Редактирование акта приемки"));
punkt_m.plus(gettext("Просмотр накладной"));
punkt_m.plus(gettext("Просмотр акта приемки"));
punkt_m.plus(gettext("Просмотр счет-фактуры"));

int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);
iceb_u_spisok imafils;
iceb_u_spisok naimf;
switch(nomer)
 {
  case 0:
    iceb_f_redfil(data->imaf.ravno(0),1,NULL);

    break;

  case 1:
    sozfap(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),
    data->rk.lnds,data->imaf.ravno(4),data->window);

    iceb_f_redfil(data->imaf.ravno(4),1,NULL);

    break;

  case 2:
    iceb_prospp(data->imaf.ravno(0),data->window);
    break;

  case 3:
    sozfap(data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),
    data->rk.lnds,data->imaf.ravno(4),data->window);
    iceb_prospp(data->imaf.ravno(4),data->window);
    break;

  case 4:
    if(l_dokmat_snn(data->imaf.ravno(1),data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),data->rk.lnds,data->window) != 0)
     break;
    iceb_prospp(data->imaf.ravno(1),data->window);
    break;

 }
}
/*************************/
/*Просмотр  для расходных документов*/
/************************/

void dokmat_pros2(class dokmat_data *data)
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Просмотр распечатки документов"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Редактирование накладной"));
punkt_m.plus(gettext("Редактирование счет-фактуры"));
punkt_m.plus(gettext("Редактирование счета"));
punkt_m.plus(gettext("Просмотр накладной"));
punkt_m.plus(gettext("Просмотр счет-фактуры"));
punkt_m.plus(gettext("Просмотр счета"));

int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);
iceb_u_spisok imafils;
iceb_u_spisok naimf;
switch(nomer)
 {
  case 0:
    iceb_f_redfil(data->imaf.ravno(0),1,NULL);
    break;

  case 1:
    if(l_dokmat_snn(data->imaf.ravno(1),data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),data->rk.lnds,data->window) != 0)
     break;
    iceb_f_redfil(data->imaf.ravno(1),1,NULL);
    break;

  case 2:
    iceb_f_redfil(data->imaf.ravno(2),1,NULL);
    break;
  case 3:
    iceb_prospp(data->imaf.ravno(0),data->window);
    break;

  case 4:
    if(l_dokmat_snn(data->imaf.ravno(1),data->rk.dd,data->rk.md,data->rk.gd,data->rk.skk,data->rk.nomdok.ravno(),data->rk.lnds,data->window) != 0)
     break;
    iceb_prospp(data->imaf.ravno(1),data->window);
    break;

  case 5:
    iceb_prospp(data->imaf.ravno(2),data->window);
    break;
 }
}
/******************************************/
/*Чтение шапки документа и формирование именов файлов*/
/*****************************************************/
void l_dokmat_read_sap_dok(class dokmat_data *data,GtkWidget *wpredok)
{
//чтение реквизитов шапки документа
readdokw(&data->rk,wpredok);

char strsql[512];

data->imaf.free_class();

/*Имя файла накладной*/
if(data->rk.tipz == 1)
     sprintf(strsql,"np%ld.lst",data->rk.vrem_zap);
if(data->rk.tipz == 2)
     sprintf(strsql,"nr%ld.lst",data->rk.vrem_zap);

data->imaf.plus(strsql);//0

/*имя файла счет-фактуры*/
if(data->rk.tipz == 1)
     sprintf(strsql,"nnp%ld.lst",data->rk.vrem_zap);
if(data->rk.tipz == 2)
     sprintf(strsql,"nnr%ld.lst",data->rk.vrem_zap);

data->imaf.plus(strsql);//1

/*Имя файла со счетом на оплату*/
sprintf(strsql,"sh%ld.lst",data->rk.vrem_zap);
data->imaf.plus(strsql);//2

/*Имя файла с кассовым ордером*/
sprintf(strsql,"ko%ld.lst",data->rk.vrem_zap);
data->imaf.plus(strsql);//3

/*Имя файла с актом приемки*/
sprintf(strsql,"ap%ld.lst",data->rk.vrem_zap);
data->imaf.plus(strsql);//4

/*Имя файла с актом списания*/
sprintf(strsql,"acts%ld.lst",data->rk.vrem_zap);
data->imaf.plus(strsql);//5

}
