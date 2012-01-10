/*$Id: l_nahud.c,v 1.43 2011-08-29 07:13:43 sasa Exp $*/
/*08.08.2011	20.09.2006	Белых А.И.	l_nahud.c
Работа с начислениями/удержаниями
*/
#include <pwd.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "buhg_g.h"
#include "l_nahud.h"

enum
{
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
 SFK8,
 FK9,
 SFK9,
 FK10,
 KOL_F_KL
};

enum
{
 COL_TABNOM,
 COL_FIO,
 COL_DEN,
 COL_MES_GOD,
 COL_SUMA,
 COL_SHET,
 COL_NOM_ZAP,
 COL_PODR,
 COL_KOMENT,
 COL_KDATA,
 COL_NOMDOK,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  l_nahud_data
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
  
  class l_nahud_rek poi;  
  
  short prn;
  int kod_nah_ud;
  short mp,gp;
  
  short metka_sort; //0-сортировка по табельным номерам 1-сортировка по фамилиям
  short metka0; //0-все записи 1-только с ненулевым значением
  char naim_knu[512];
  
  class iceb_u_str tabnom_tv; //Только что введённый табельный номер
  
  //Реквизиты выбранной строки
  int tabnom;
  short den;
  short knah;
  short god,mes; //дата в счёт которой сумма
  int podr;
  class iceb_u_str shet;
  int nom_zap;
  class iceb_u_str nom_dok;
          
  class iceb_u_str shet_zap; //Запомненный счёт для ввода записей
  class iceb_u_str zapros; //запрос к базе данных
  class iceb_u_str zapros1; /*запрос без сортировки*/  

  //Конструктор
  l_nahud_data()
   {
    tabnom_tv.plus("");
    shet_zap.plus("");  
    metka_sort=metka0=0;
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    memset(naim_knu,'\0',sizeof(naim_knu));
   }      
 };

gboolean   l_nahud_key_press(GtkWidget *widget,GdkEventKey *event,class l_nahud_data *data);
void l_nahud_vibor(GtkTreeSelection *selection,class l_nahud_data *data);
void l_nahud_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class l_nahud_data *data);
void  l_nahud_knopka(GtkWidget *widget,class l_nahud_data *data);
void l_nahud_add_columns (GtkTreeView *treeview);
void l_nahud_create_list(class l_nahud_data *data);

int l_nahud_vnz(class l_nahud_data *data);
int l_nahud_sfk3(class l_nahud_data *data);
int l_nahud_p(class l_nahud_rek *datap,GtkWidget *wpredok);
void l_nahud_rasp(class l_nahud_data *data);
int l_nahud_vz(int prn,int kod_nah_ud,short mp,short gp,GtkWidget *wpredok);
void l_nahud_sf6(class l_nahud_data *data);
void imp_nu(int prn,int,short,short,GtkWidget *wpredok);
void imp_kr(int prn,int kod_nah_ud,short mz,short gz,GtkWidget *wpredok);
void l_nahud_f7(class l_nahud_data *data);
void zvb_pibw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok);
void zvb_nadraw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok);
void zvb_ukrsocw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok);
void zvb_ukrsibw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok);
void zvb_kreditpromw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok);
void zvb_ukrpromw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok);
void zvb_ukreksimw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok);
void zvb_hreshatikw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok);
void zvb_ibank2uaw(short prn,const char *zapros,class l_nahud_rek *poisk,const char *kod00,GtkWidget *wpredok);
void zvb_alfabankw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok);
void zvb_ukrgazw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok);
void zvb_promekonomw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok);
void zar_eks_ko(int prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok);
int zar_sm_shet(const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok);
void l_nahud_srvz(class  l_nahud_data *data);

extern SQL_baza	bd;
extern char *name_system;
extern char     *organ;
extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern float dnei,hasov; //Количество рабочих дней и часов в определённом месяце. Читает программа redkalw.c перед началом расчёта начислений/удержаний
extern int      kol_strok_na_liste;
extern char     shrpz[16]; //счет расчетов по зарплате
 
void l_nahud(int prn, //1-просмотр начислений 2-удержаний
int kod_nah_ud,
short mp,short gp,
GtkWidget *wpredok)
{
char strsql[512];
class  l_nahud_data data;
SQL_str row;
class SQLCURSOR cur;
data.prn=prn;
data.kod_nah_ud=kod_nah_ud;
data.mp=mp;
data.gp=gp;
data.poi.mes_god_pros.new_plus(mp);
data.poi.mes_god_pros.plus(".");
data.poi.mes_god_pros.plus(gp);

//читаем наименование начисления/удержания
if(data.prn == 1)
 sprintf(strsql,"select naik from Nash where kod=%d",data.kod_nah_ud);
if(data.prn == 2)
 sprintf(strsql,"select naik from Uder where kod=%d",data.kod_nah_ud);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 strncpy(data.naim_knu,row[0],sizeof(data.naim_knu)-1); 


data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


if(data.prn == 1)
  sprintf(strsql,"%s %s",name_system,gettext("Начисления"));
if(data.prn == 2)
  sprintf(strsql,"%s %s",name_system,gettext("Удержания"));

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

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_nahud_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

if(data.prn == 1)
  data.label_kolstr=gtk_label_new (gettext("Начисления"));
if(data.prn == 2)
  data.label_kolstr=gtk_label_new (gettext("Удержания"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");
GdkColor color;
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


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(l_nahud_knopka),&data);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод новой записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK2]), "clicked",GTK_SIGNAL_FUNC(l_nahud_knopka),&data);
tooltips[SFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK2],data.knopka[SFK2],gettext("Корректировка выбранной записи"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK2]),(gpointer)SFK2);
gtk_widget_show(data.knopka[SFK2]);

sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(l_nahud_knopka),&data);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удалить выбранную запись"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(strsql,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK3]), "clicked",GTK_SIGNAL_FUNC(l_nahud_knopka),&data);
tooltips[SFK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK3],data.knopka[SFK3],gettext("Удаление всех записей или записей с нулевыми суммами"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK3]),(gpointer)SFK3);
gtk_widget_show(data.knopka[SFK3]);

sprintf(strsql,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(l_nahud_knopka),&data);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Поиск нужных записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(l_nahud_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка записей"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"%sF5 %s",RFK,gettext("Сортування"));
data.knopka[SFK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK5]), "clicked",GTK_SIGNAL_FUNC(l_nahud_knopka),&data);
tooltips[SFK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK5],data.knopka[SFK5],gettext("Включить/выключить сортировку по фамилиям"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK5]),(gpointer)SFK5);
gtk_widget_show(data.knopka[SFK5]);

sprintf(strsql,"F6 %s",gettext("Ввести"));
data.knopka[FK6]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(l_nahud_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Ввод начисления/удержания всем"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(strsql,"%sF6 %s",RFK,gettext("Импорт"));
data.knopka[SFK6]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK6]), "clicked",GTK_SIGNAL_FUNC(l_nahud_knopka),&data);
tooltips[SFK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK6],data.knopka[SFK6],gettext("Импорт начислений/удержаний из файла или из подсистемы \"Учёт командировочных расходов\""),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK6]),(gpointer)SFK6);
gtk_widget_show(data.knopka[SFK6]);

sprintf(strsql,"F7 %s",gettext("Банк"));
data.knopka[FK7]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(l_nahud_knopka),&data);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Экспорт данных для передачи в банк"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);

sprintf(strsql,"%sF7 %s",RFK,gettext("Ордера"));
data.knopka[SFK7]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK7],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK7]), "clicked",GTK_SIGNAL_FUNC(l_nahud_knopka),&data);
tooltips[SFK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK7],data.knopka[SFK7],gettext("Экспорт кассовых ордеров в подсистему \"Учёт кассовых ордеров.\""),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK7]),(gpointer)SFK7);
gtk_widget_show(data.knopka[SFK7]);

sprintf(strsql,"F8 %s",gettext("Сумма"));
data.knopka[FK8]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]), "clicked",GTK_SIGNAL_FUNC(l_nahud_knopka),&data);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Включить/выключить показ записей только с ненулевой суммой"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_widget_show(data.knopka[FK8]);

sprintf(strsql,"%sF8 %s",RFK,gettext("Сменить"));
data.knopka[SFK8]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK8],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK8]), "clicked",GTK_SIGNAL_FUNC(l_nahud_knopka),&data);
tooltips[SFK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK8],data.knopka[SFK8],gettext("Смена реквизитов во всех записях"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK8]),(gpointer)SFK8);
gtk_widget_show(data.knopka[SFK8]);

sprintf(strsql,"F9 %s",gettext("Смена даты"));
data.knopka[FK9]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK9]), "clicked",GTK_SIGNAL_FUNC(l_nahud_knopka),&data);
tooltips[FK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK9],data.knopka[FK9],gettext("Уменьшить дату просмотра на месяц"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK9]),(gpointer)FK9);
gtk_widget_show(data.knopka[FK9]);

sprintf(strsql,"%sF9 %s",RFK,gettext("Смена даты"));
data.knopka[SFK9]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK9],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK9]), "clicked",GTK_SIGNAL_FUNC(l_nahud_knopka),&data);
tooltips[SFK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK9],data.knopka[SFK9],gettext("Увеличить дату просмотра на месяц"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK9]),(gpointer)SFK9);
gtk_widget_show(data.knopka[SFK9]);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(l_nahud_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_nahud_create_list(&data);

gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


//printf("l_l_nahud end\n");
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


return;

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_nahud_knopka(GtkWidget *widget,class l_nahud_data *data)
{
class ZARP zp;
short d=1;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_nahud_knopka knop=%d\n",knop);
data->kl_shift=0;

switch (knop)
 {
  case FK2: //Ввод новой записи
    if(l_nahud_prov_blok(data->mp,data->gp,0,data->window) != 0)
     return;
    l_nahud_vnz(data);
    l_nahud_create_list(data); //Всегда делаем новый просмотр
    return;  

  case SFK2: //Коригування запису
    if(data->kolzap == 0)
     return;
    if(l_nahud_prov_blok(data->mp,data->gp,data->tabnom,data->window) != 0)
     return;

    if(l_nahud_t_v(data->tabnom,data->den,data->mp,data->gp,data->prn,data->kod_nah_ud,data->mes,data->god,
    data->podr,data->shet.ravno(),data->nom_zap,data->window) == 0)
     {
      zarsocw(data->mp,data->gp,data->tabnom,NULL,data->window); //запускать и для удержаний так как может быть добавлено удержание при наличии которого считается и отчисления в соц. фонд
      zaravprw(data->tabnom,data->mp,data->gp,NULL,data->window);
      l_nahud_create_list(data);
     }

    
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(l_nahud_prov_blok(data->mp,data->gp,data->tabnom,data->window) != 0)
     return;


    if(data->nom_dok.getdlinna() > 1)
     {
      iceb_menu_soob(gettext("Записи сделанные из документов, удаляются только в документах !"),data->window);
      return;
     }
  
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
     return;

    
    zp.tabnom=data->tabnom;
    zp.prn=data->prn;
    zp.knu=data->kod_nah_ud;
    zp.dz=data->den;
    zp.mz=data->mp;
    zp.gz=data->gp;
    zp.godn=data->god;
    zp.mesn=data->mes;
    zp.nomz=data->nom_zap;
    zp.podr=data->podr;
    strncpy(zp.shet,data->shet.ravno(),sizeof(zp.shet)-1);
    
    zarudnuw(&zp,data->window);

    l_nahud_create_list(data);
    return;  

  case SFK3:

    if(data->kolzap == 0)
      return;
    if(l_nahud_prov_blok(data->mp,data->gp,0,data->window) != 0)
     return;

    if(l_nahud_sfk3(data) == 0)
      l_nahud_create_list(data);
    return;  

  case FK4: //Поиск
    if(l_nahud_p(&data->poi,data->window) == 0)
     iceb_u_rsdat1(&data->mp,&data->gp,data->poi.mes_god_pros.ravno());
     
    l_nahud_create_list(data);
    return;  

  case FK5:
    l_nahud_rasp(data);
    return;  

  case SFK5:
    data->metka_sort++;
    if(data->metka_sort >= 2)
     data->metka_sort=0;
    l_nahud_create_list(data);
    return;  

  case FK6:
    if(l_nahud_prov_blok(data->mp,data->gp,0,data->window) != 0)
     return;
    if(l_nahud_vz(data->prn,data->kod_nah_ud,data->mp,data->gp,data->window) == 0)
      l_nahud_create_list(data);
    return;  

  case SFK6:
    if(l_nahud_prov_blok(data->mp,data->gp,0,data->window) != 0)
     return;

    l_nahud_sf6(data);

    l_nahud_create_list(data);
    return;  

  case FK7:
    l_nahud_f7(data);
    return;  

  case SFK7: //Экспорт кассовых ордеров в подсистему Учёт кассовых ордеров.
    zar_eks_ko(data->prn,data->zapros.ravno(),&data->poi,data->window);
    return;  


  case FK8: //Включить/выключить просмотр записей с нулевыми суммами
    data->metka0++;
    if(data->metka0 == 2)
     data->metka0=0;
    l_nahud_create_list(data);
     
    return;  

  case SFK8: // Корректировка номера счёта
    l_nahud_srvz(data);
    return;  

  case FK9:
    iceb_u_dpm(&d,&data->mp,&data->gp,4);    
    l_nahud_create_list(data);
    return;  

  case SFK9:
    iceb_u_dpm(&d,&data->mp,&data->gp,3);    
    l_nahud_create_list(data);
    return;  
    
  case FK10: 
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_nahud_key_press(GtkWidget *widget,GdkEventKey *event,class l_nahud_data *data)
{
iceb_u_str repl;
//printf("l_nahud_key_press keyval=%d state=%d\n",event->keyval,event->state);

switch(event->keyval)
 {
  case GDK_F2:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    if(data->kl_shift == 1)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
    return(TRUE);

  case GDK_F3:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    if(data->kl_shift == 1)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK3]),"clicked");
    return(TRUE);

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
    return(TRUE);

  case GDK_plus: //это почемуто не работает Нажата клавиша плюс на основной клавиатуре
  case GDK_KP_Add: //Нажата клавиша "+" на дополнительной клавиатуре
  case GDK_F5:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
    if(data->kl_shift == 1)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK5]),"clicked");
    return(TRUE);
  
  case GDK_F6:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
    if(data->kl_shift == 1)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK6]),"clicked");
    return(TRUE);
  
  case GDK_F7:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK7]),"clicked");
    if(data->kl_shift == 1)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK7]),"clicked");
    return(TRUE);

  case GDK_F8:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK8]),"clicked");
    if(data->kl_shift == 1)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK8]),"clicked");
    return(TRUE);

  case GDK_F9:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK9]),"clicked");
    if(data->kl_shift == 1)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK9]),"clicked");
    return(TRUE);

  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("l_nahud_key_press-Нажата клавиша Shift\n");

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
void l_nahud_create_list (class l_nahud_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
int  kolstr=0;
SQL_str row;
iceb_u_str zagolov;

//printf("l_nahud_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_nahud_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_nahud_vibor),data);

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

if(data->metka0 == 0)
  sprintf(strsql,"select Zarp.*, Kartb.fio from Zarp, Kartb where datz >='%04d-%02d-01' and \
datz <= '%04d-%02d-31' and prn='%d' and knah=%d and Kartb.tabn=Zarp.tabn",
  data->gp,data->mp,data->gp,data->mp,data->prn,data->kod_nah_ud);


if(data->metka0 == 1)
  sprintf(strsql,"select Zarp.*, Kartb.fio from Zarp, Kartb where datz >='%04d-%02d-01' and \
datz <= '%04d-%02d-31' and prn='%d' and knah=%d and Kartb.tabn=Zarp.tabn and suma <> 0.",
  data->gp,data->mp,data->gp,data->mp,data->prn,data->kod_nah_ud);

data->zapros1.new_plus(strsql);

if(data->metka_sort == 0)
 strcat(strsql," order by tabn,datz,nomz asc");
if(data->metka_sort == 1)
 strcat(strsql," order by fio,datz,nomz asc");

data->zapros.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
float kolstr1=0.;
short d,m,g;

SQL_str row1;
double suma_i=0.;
double suma=0.;
double sumaib=0.;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(l_nahud_prov_str(row,&data->poi,data->window) != 0)
   continue;

  if(iceb_u_SRAV(data->tabnom_tv.ravno(),row[1],0) == 0)
    data->snanomer=data->kolzap;
     
  //Табельный номер
  ss[COL_TABNOM].new_plus(row[1]);
  
  //фамилия
  ss[COL_FIO].new_plus(iceb_u_toutf(row[15]));
    

   
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  ss[COL_DEN].new_plus(d);
  
  //Дата и время записи
  ss[COL_DATA_VREM].new_plus(iceb_u_toutf(iceb_u_vremzap(row[11])));

  //Кто записал
  ss[COL_KTO].new_plus(iceb_kszap(row[10],0,data->window));
  
  //В счёт какого месяца
  sprintf(strsql,"%02d.%d",atoi(row[7]),atoi(row[6]));
  ss[COL_MES_GOD].new_plus(iceb_u_toutf(strsql));

  //Сумма
  suma=atof(row[4]);
  sprintf(strsql,"%8.2f",suma);
  ss[COL_SUMA].new_plus(strsql);
  suma_i+=suma;

  if(data->prn == 1 && shetb != NULL)
    if(iceb_u_proverka(shetb,row[5],0,1) == 0)
       sumaib+=suma; 
  if(data->prn == 2 && shetbu != NULL)
    if(iceb_u_proverka(shetbu,row[5],0,1) == 0)
       sumaib+=suma; 
  
  //Номер записи
  ss[COL_NOM_ZAP].new_plus(row[9]);

  //Подразделение     
  ss[COL_PODR].new_plus(row[13]);

  //Коментарий
  ss[COL_KOMENT].new_plus(iceb_u_toutf(row[12]));

  //Номер документа
  ss[COL_NOMDOK].new_plus(iceb_u_toutf(row[14]));

  //Счёт
  ss[COL_SHET].new_plus(iceb_u_toutf(row[5]));


  sprintf(strsql,"select datd from Zarn1 where tabn=%d and prn='%d' and knah=%s",
  data->tabnom,data->prn,row[3]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
   {
    d=m=g=0;
   }
  else
    iceb_u_rsdat(&d,&m,&g,row1[0],2);
  
  ss[COL_KDATA].new_plus("");
  if(d != 0)
   {
    sprintf(strsql,"%02d.%02d.%d",d,m,g);
    ss[COL_KDATA].new_plus(strsql);
   }
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_TABNOM,ss[COL_TABNOM].ravno(),
  COL_FIO,ss[COL_FIO].ravno(),
  COL_SUMA,ss[COL_SUMA].ravno(),
  COL_NOM_ZAP,ss[COL_NOM_ZAP].ravno(),
  COL_PODR,ss[COL_PODR].ravno(),
  COL_KOMENT,ss[COL_KOMENT].ravno(),
  COL_MES_GOD,ss[COL_MES_GOD].ravno(),
  COL_NOMDOK,ss[COL_NOMDOK].ravno(),
  COL_DEN,ss[COL_DEN].ravno(),
  COL_KDATA,ss[COL_KDATA].ravno(),
  COL_SHET,ss[COL_SHET].ravno(),
  COL_DATA_VREM,ss[COL_DATA_VREM].ravno(),
  COL_KTO,ss[COL_KTO].ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->tabnom_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data->treeview),GTK_TREE_MODEL (model));

g_object_unref(GTK_TREE_MODEL (model));

l_nahud_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK7]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK8]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK7]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK8]),TRUE);//Доступна
 }


gtk_widget_show (data->treeview);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

if(data->prn == 1)
 zagolov.new_plus(gettext("Начисления"));
if(data->prn == 2)
 zagolov.new_plus(gettext("Удержания"));
 
zagolov.plus(". ");
zagolov.plus(gettext("Дата"));
zagolov.plus(":");
zagolov.plus(data->mp);
zagolov.plus(".");
zagolov.plus(data->gp);
zagolov.plus(gettext("г."));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);
zagolov.plus(" ");
zagolov.plus(gettext("Сумма всех записей"));
zagolov.plus(":");
zagolov.plus(suma_i);

if(data->metka0 == 1)
 zagolov.ps_plus(gettext("Записи только с ненулевой суммой"));

zagolov.ps_plus(data->kod_nah_ud);
zagolov.plus(" ");
zagolov.plus(data->naim_knu);

if(shetb != NULL)
 if(data->prn == 1)
  {
   sprintf(strsql,"/%.2f/%.2f",sumaib,suma_i-sumaib);
   zagolov.plus(strsql);
  }
if(shetbu != NULL)
 if(data->prn == 2)
  {
   sprintf(strsql,"/%.2f/%.2f",sumaib,suma_i-sumaib);
   zagolov.plus(strsql);
  }

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno_toutf());

gtk_widget_show(data->label_kolstr);


if(data->poi.metka_poi == 1 )
 {

// printf("Формирование заголовка с реквизитами поиска.\n");
  iceb_u_str strpoi;  

  strpoi.new_plus(gettext("Поиск"));
  strpoi.plus(" !!!");

  iceb_str_poisk(&strpoi,data->poi.podr.ravno(),gettext("Код подразделения"));
  iceb_str_poisk(&strpoi,data->poi.tabnom.ravno(),gettext("Табельный номер"));
  iceb_str_poisk(&strpoi,data->poi.mes_god_pros.ravno(),gettext("Дата просмотра"));
  iceb_str_poisk(&strpoi,data->poi.den_n_pros.ravno(),gettext("День начала"));
  iceb_str_poisk(&strpoi,data->poi.den_k_pros.ravno(),gettext("День конца"));
  iceb_str_poisk(&strpoi,data->poi.fio.ravno(),gettext("Фамилия"));
  iceb_str_poisk(&strpoi,data->poi.kateg.ravno(),gettext("Категория"));
  iceb_str_poisk(&strpoi,data->poi.shet.ravno(),gettext("Счёт"));
  iceb_str_poisk(&strpoi,data->poi.vkm.ravno(),gettext("В счёт какого месяца"));
  iceb_str_poisk(&strpoi,data->poi.koment.ravno(),gettext("Коментарий"));
  iceb_str_poisk(&strpoi,data->poi.grup_podr.ravno(),gettext("Код группы подразделения"));
  iceb_str_poisk(&strpoi,data->poi.mnks.ravno(),gettext("Карт-счёт"));
  iceb_str_poisk(&strpoi,data->poi.kodbanka.ravno(),gettext("Код банка"));

  gtk_label_set_text(GTK_LABEL(data->label_poisk),strpoi.ravno_toutf());
  gtk_widget_show(data->label_poisk);
 }
else
 {
  gtk_widget_hide(data->label_poisk); 
 }


gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));

}
/*****************/
/*Создаем колонки*/
/*****************/

void l_nahud_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

//printf("l_nahud_add_columns\n");
renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Т/н"), renderer,"text", COL_TABNOM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Фамилия Имя Отчество"), renderer,"text", COL_FIO,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("День"), renderer,"text", COL_DEN,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("В счёт"), renderer,"text", COL_MES_GOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Сумма"), renderer,"text", COL_SUMA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Счёт"), renderer,"text", COL_SHET,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,"N", renderer,"text", COL_NOM_ZAP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Подразделение"), renderer,"text", COL_PODR,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Коментарий"), renderer,"text", COL_KOMENT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Действует до"), renderer,"text", COL_KDATA,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Ном.док."), renderer,"text", COL_NOMDOK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

//printf("l_nahud_add_columns end\n");

}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void l_nahud_vibor(GtkTreeSelection *selection,class l_nahud_data *data)
{
//printf("l_nahud_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;
gchar *tabnom;
gchar *den;
gchar *mes_god;
gchar *podr;
gchar *shet;
gchar *nom_zap;
gchar *nom_dok;
gint  nomer;

gtk_tree_model_get(model,&iter,COL_TABNOM,&tabnom,COL_DEN,&den,COL_MES_GOD,&mes_god,
COL_PODR,&podr,COL_SHET,&shet,COL_NOM_ZAP,&nom_zap,COL_NOMDOK,&nom_dok,NUM_COLUMNS,&nomer,-1);

data->tabnom=atoi(tabnom);
data->den=atoi(den);

short m=0,g=0;
iceb_u_rsdat1(&m,&g,mes_god);
data->mes=m;
data->god=g;

data->podr=atoi(podr);
data->shet.new_plus(shet);
data->nom_zap=atoi(nom_zap);
data->nom_dok.new_plus(nom_dok);


data->snanomer=nomer;

g_free(tabnom);
g_free(den);
g_free(mes_god);
g_free(podr);
g_free(shet);
g_free(nom_zap);
g_free(nom_dok);

//printf("l_nahud_vibor-%s %d\n",data->kodv.ravno(),data->snanomer);

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void l_nahud_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_nahud_data *data)
{
//printf("l_nahud_v_row\n");
//printf("l_nahud_v_row корректировка\n");

gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");

}

/**********************/
/*Проверка блокировок*/
/*********************/
int l_nahud_prov_blok(short mp,short gp,int tabn,GtkWidget *wpredok)
{
char		strsql[512];

if(iceb_pbpds(mp,gp,wpredok) != 0)
 return(1);

if(iceb_pvglkni(mp,gp,wpredok) != 0)
 return(1);

if(tabn != 0)
 {
  SQL_str row;
  SQLCURSOR cur;
  //Проверяем блокировку карточки
  sprintf(strsql,"select blok from Zarn where god=%d and mes=%d and \
tabn=%d",gp,mp,tabn);
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   if(atoi(row[0]) != 0)
    {
     struct  passwd  *ktoz; /*Кто записал*/
     int blok=atoi(row[0]);
     if((ktoz=getpwuid(blok)) != NULL)
       sprintf(strsql,"%s %d %s",gettext("Карточка заблокирована !"),blok,ktoz->pw_gecos);
     else
       sprintf(strsql,gettext("Заблокировано неизвестным логином %d !"),blok);
     iceb_menu_soob(strsql,wpredok);
     return(1);
    }

 }
return(0);

}
/****************************/
/*Ввод новой записи*/
/*****************************/
int l_nahud_vnz(class l_nahud_data *data)
{

if(vvod_tabnom(&data->tabnom_tv,data->window) != 0)
 return(1);


SQL_str row;
class SQLCURSOR cur;
char strsql[512];
short metka_zarn=0;
//Проверяем блокировку карточки
 sprintf(strsql,"select shet,blok from Zarn where god=%d and mes=%d and \
tabn=%d",data->gp,data->mp,data->tabnom_tv.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
   if(atoi(row[1]) != 0)
    {
     struct  passwd  *ktoz; /*Кто записал*/
     int blok=atoi(row[0]);
     if((ktoz=getpwuid(blok)) != NULL)
       sprintf(strsql,"%s %d %s",gettext("Карточка заблокирована !"),blok,ktoz->pw_gecos);
     else
       sprintf(strsql,gettext("Заблокировано неизвестным логином %d !"),blok);
     iceb_menu_soob(strsql,data->window);
     return(1);
    }
  metka_zarn=1;
 }
/*Читаем карточку*/
sprintf(strsql,"select fio,datn,datk,shet,podr,kodg,kateg,\
sovm,zvan,lgoti,dolg from Kartb where tabn=%s",data->tabnom_tv.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),data->tabnom_tv.ravno_atoi());
  return(1);
 }

short dn,mn,gn;
short du,mu,gu;

iceb_u_rsdat(&dn,&mn,&gn,row[1],2);
iceb_u_rsdat(&du,&mu,&gu,row[2],2);
char shetkar[30];
strncpy(shetkar,row[3],sizeof(shetkar)-1);
int podr=atoi(row[4]);
int kateg=atoi(row[6]);
int sovm=atoi(row[7]);
int zvan=atoi(row[8]);
char lgot[50];
memset(lgot,'\0',sizeof(lgot));
strncpy(lgot,row[9],sizeof(lgot)-1);
char dolg[512];
memset(dolg,'\0',sizeof(dolg));
strncpy(dolg,row[10],sizeof(dolg)-1);

 
    /*Проверяем записан ли стандартный набор*/
provzzagw(data->mp,data->gp,data->tabnom_tv.ravno_atoi(),data->prn,podr,data->window);
    

if(l_nahud_t_v(data->tabnom_tv.ravno_atoi(),0,data->mp,data->gp,data->prn,data->kod_nah_ud,0,0,podr,data->shet_zap.ravno(),0,data->window) == 0)
 {
  zarsocw(data->mp,data->gp,data->tabnom_tv.ravno_atoi(),NULL,data->window); //запускать и для удержаний так как может быть добавлено удержание при наличии которого считается и отчисления в соц. фонд
  zaravprw(data->tabnom_tv.ravno_atoi(),data->mp,data->gp,NULL,data->window);
  if(metka_zarn == 0) 
   zapzarnw(data->mp,data->gp,podr,data->tabnom_tv.ravno_atoi(),kateg,sovm,zvan,shetkar,lgot,dn,mn,gn,du,mu,gu,0,dolg,data->window);

  //Проверяем есть ли не нулевые
  sprintf(strsql,"select datz from Zarp where datz >= '%04d-%d-01' and \
datz <= '%04d-%d-31' and tabn=%d and prn='%d' and knah=%d and suma <> 0.",
  data->gp,data->mp,data->gp,data->mp,data->tabnom_tv.ravno_atoi(),data->prn,data->kod_nah_ud);
//  printf("l_nahud-проверяем есть ли записи с ненулевыми суммами-%s\n",strsql);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
   {      
    //Удаляем нулевые начисления/удержания
    sprintf(strsql,"delete from Zarp where datz >= '%04d-%d-01' and \
datz <= '%04d-%d-31' and tabn=%d and prn='%d' and knah=%d and suma=0.",
    data->gp,data->mp,data->gp,data->mp,data->tabnom_tv.ravno_atoi(),data->prn,data->kod_nah_ud);
//    printf("l_nahud-удаляем записи с нулевыми суммами-%s\n",strsql);
    iceb_sql_zapis(strsql,0,0,data->window);
   }
  
 }
else 
 return(1);
 
return(0);

}
/*************************/
/*Удаление группы записей*/
/*************************/
int l_nahud_sfk3(class l_nahud_data *data)
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Удалить все записи с нулевыми суммами"));//0
punkt_m.plus(gettext("Удалить все записи"));//1


int nomer=0;
if((nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window)) < 0)
 return(1);
 
char strsql[512];

if(nomer == 0)
 {
  sprintf(strsql,"delete from Zarp where datz>='%04d-%d-01' and \
datz <= '%04d-%d-31' and prn='%d' and knah=%d and suma=0.",
data->gp,data->mp,data->gp,data->mp,data->prn,data->kod_nah_ud);
  
  return(iceb_sql_zapis(strsql,0,0,data->window));
     
 }

if(nomer == 1)
 {
  class iceb_gdite_data gdite;
  iceb_gdite(&gdite,0,data->window);
  SQL_str row,row1;
  class SQLCURSOR cur,cur1;
  int kolstr=0;
    
  if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    return(1);
   }
  int kolzapnar=0;
  int tabn=0;
  short dz,mz,gz;
  class ZARP zp;
  float kolstr1=0.;
  while(cur.read_cursor(&row) != 0)
   {
    iceb_pbar(gdite.bar,kolstr,++kolstr1);
    if(l_nahud_prov_str(row,&data->poi,data->window) != 0)
     continue;
    if(row[14][0] != '\0')
     {
      kolzapnar++;
      continue;
     }
    tabn=atoi(row[1]);

    //Проверяем блокировку карточки
    sprintf(strsql,"select blok from Zarn where god=%d and mes=%d and \
  tabn=%d",data->gp,data->mp,tabn);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
     if(atoi(row1[0]) != 0)
      {
       struct  passwd  *ktoz; /*Кто записал*/
       class iceb_u_str repl;
       
       int blok=atoi(row[0]);
       if((ktoz=getpwuid(blok)) != NULL)
         sprintf(strsql,"%s %d %s",gettext("Карточка заблокирована !"),blok,ktoz->pw_gecos);
       else
         sprintf(strsql,gettext("Заблокировано неизвестным логином %d !"),blok);
       repl.plus(strsql);

       sprintf(strsql,"%s %d",gettext("Табельный номер"),tabn);
       repl.ps_plus(strsql);
              
       iceb_menu_soob(strsql,data->window);
       continue;
      }
     }
/**********
    rsdat(&dz,&mz,&gz,row[0],2);
    knah=atoi(row[3]);
    godn=atoi(row[6]);
    mesn=atoi(row[7]);
    strcpy(shetnuz,row[5]);
    nomz=atoi(row[9]);
    podr=atoi(row[13]);
      
    zarudnu(gz,mz,dz,tabn,prn,knah,godn,mesn,podr,shetnuz,nomz);
***************/
    iceb_u_rsdat(&dz,&mz,&gz,row[0],2);

    zp.tabnom=tabn;
    zp.prn=data->prn;
    zp.knu=data->kod_nah_ud;
    zp.dz=dz;
    zp.mz=mz;
    zp.gz=gz;
    zp.godn=atoi(row[6]);
    zp.mesn=atoi(row[7]);
    zp.nomz=atoi(row[9]);
    zp.podr=atoi(row[13]);
    strncpy(zp.shet,row[5],sizeof(zp.shet)-1);
    
    zarudnuw(&zp,data->window);

    zarsocw(data->mp,data->gp,tabn,NULL,data->window); //запускать и для удержаний так как может быть добавлено удержание при наличии которого считается и отчисления в соц. фонд
    zaravprw(tabn,data->mp,data->gp,NULL,data->window);

   }
  if(kolzapnar != 0)
    iceb_menu_soob(gettext("Записи сделанные из документов, удаляются только в документах !"),data->window);
  return(0);        
 }
  
return(1);

}
/*********************************/
/*проверка строки на условия поиска*/
/***********************************/
int l_nahud_prov_str(SQL_str row,class l_nahud_rek *data,GtkWidget *wpredok)
{

if(data->metka_poi == 0)
 return(0);

if(iceb_u_proverka(data->shet.ravno(),row[5],0,0) != 0)
  return(1);

if(iceb_u_proverka(data->tabnom.ravno(),row[1],0,0) != 0)
  return(1);

short d=0,m=0,g=0;
if(data->den_n_pros.ravno_atoi() != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(d < data->den_n_pros.ravno_atoi())
    return(1);
 }

if(data->den_k_pros.ravno_atoi() != 0 )
 {
  if(d == 0)
   iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(d > data->den_k_pros.ravno_atoi())
    return(1);
 }

if(data->vkm.getdlinna() > 1)
 {
  iceb_u_rsdat1(&m,&g,data->vkm.ravno());
  
  if(atoi(row[7]) != m || atoi(row[6]) != g)
    return(1);
 }

char strsql[512];
class SQLCURSOR cur;
class iceb_u_str fio("");
class iceb_u_str podr("");
class iceb_u_str kateg("");
class iceb_u_str bankshet("");

/*Читаем фамилию*/
sprintf(strsql,"select fio,podr,kateg,bankshet,kb from Kartb where tabn=%s",row[1]);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  fio.new_plus(row[0]);
  podr.new_plus(row[1]);
  kateg.new_plus(row[2]);
  bankshet.new_plus(row[3]);

  if(iceb_u_proverka(data->kodbanka.ravno(),row[4],0,0) != 0)
    return(1);

 }


if(data->mnks.ravno_pr() == 1)
 if(bankshet.getdlinna() <= 1)
  return(1);

if(data->mnks.ravno_pr() == 2)
 if(bankshet.getdlinna() > 1)
  return(1);

  
if(iceb_u_proverka(data->podr.ravno(),podr.ravno(),0,0) != 0)
  return(1);




if(iceb_u_proverka(data->kateg.ravno(),kateg.ravno(),0,0) != 0)
  return(1);


if(data->fio.getdlinna() > 1)
  if(iceb_u_strstrm(fio.ravno(),data->fio.ravno()) == 0)
     return(1);

if(data->koment.getdlinna() > 1)
  if(iceb_u_strstrm(row[12],data->koment.ravno()) == 0)
     return(1);




//if(poisk->sumado != 0. && poisk->sumado < atof(row[4]))
//  return(1);

if(data->grup_podr.getdlinna() > 1)
 {
  sprintf(strsql,"select grup from Podr where kod=%s",podr.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   if(iceb_u_proverka(data->grup_podr.ravno(),row[0],0,0) != 0)
     return(1);
 }

return(0);

}
/***************************************/
/*Распечатка*/
/***************************************/
void l_nahud_rasp(class l_nahud_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);
char strsql[512];
SQL_str row,row1;
class SQLCURSOR cur,cur1;

int kolstr=0;
class iceb_u_str zapr(data->zapros1.ravno());
if(data->metka_sort == 0)
 zapr.plus(" order by podr,tabn,datz,nomz asc");
else
 zapr.plus(" order by podr,Kartb.fio,datz,nomz asc");

if((kolstr=cur.make_cursor(&bd,zapr.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return;
 }
char naipod[40];
char naived[20];
memset(naipod,'\0',sizeof(naipod));
memset(naived,'\0',sizeof(naived));

/*Читаем наименование*/
if(data->prn == 1)
 sprintf(strsql,"select naik from Nash where kod=%d",data->kod_nah_ud);
if(data->prn == 2)
 sprintf(strsql,"select naik from Uder where kod=%d",data->kod_nah_ud);

char naim[512];
memset(naim,'\0',sizeof(naim));
if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
 strncpy(naim,row1[0],sizeof(naim)-1);

char imaf[56];
FILE *ff;
sprintf(imaf,"rasnu%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }

class iceb_u_double SUMPODR;
class iceb_u_int KOLLPODR;
char imafved[50];
FILE *ffved;
sprintf(imafved,"rasved%d.lst",getpid());
if((ffved = fopen(imafved,"w")) == NULL)
 {
  iceb_er_op_fil(imafved,"",errno,data->window);
  return;
 }
char imafpodr[50];
FILE *ffpodr;
sprintf(imafpodr,"rpodr%d.lst",getpid());
if((ffpodr = fopen(imafpodr,"w")) == NULL)
 {
  iceb_er_op_fil(imafpodr,"",errno,data->window);
  return;
 }

sapreestpw(data->mp,data->gp,0,ffpodr);
struct  tm      *bf;
time_t tmm;
time(&tmm);
bf=localtime(&tmm);

char nairas[100];

if(data->prn == 1)
 sprintf(nairas,gettext("Распечатка начислений"));
if(data->prn == 2)
 sprintf(nairas,gettext("Распечатка удержаний"));

fprintf(ff,"%s %s %d.%d%s\n\
%s %d.%d.%d%s %s: %d:%d\n",
nairas,
gettext("за"),
data->mp,data->gp,
gettext("г."),

gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

if(data->prn == 1)
 fprintf(ff,gettext("Начисление"));
if(data->prn == 2)
 fprintf(ff,gettext("Удержание"));
fprintf(ff,":%d %s\n",data->kod_nah_ud,naim);

if(data->poi.metka_poi == 1)
 {
  if(data->poi.podr.getdlinna() > 1)
   fprintf(ff,"%s: %s\n",gettext("Подразделение"),data->poi.podr.ravno());
  if(data->poi.tabnom.getdlinna() > 1)
   fprintf(ff,"%s: %s\n",gettext("Табельный номер"),data->poi.tabnom.ravno());
  if(data->poi.fio.getdlinna() > 1)
   fprintf(ff,"%s: %s\n",gettext("Фамилия"),data->poi.fio.ravno());
  if(data->poi.den_n_pros.getdlinna() > 1)
   fprintf(ff,"%s: %s\n",gettext("День начала"),data->poi.den_n_pros.ravno());
  if(data->poi.den_k_pros.getdlinna() > 1)
   fprintf(ff,"%s: %s\n",gettext("День конца"),data->poi.den_k_pros.ravno());
  if(data->poi.kateg.getdlinna() > 1)
   fprintf(ff,"%s: %s\n",gettext("Категория"),data->poi.kateg.ravno());
  if(data->poi.shet.getdlinna() > 1)
   fprintf(ff,"%s: %s\n",gettext("Счёт"),data->poi.shet.ravno());
  if(data->poi.vkm.getdlinna() > 1)
   fprintf(ff,"%s: %s\n",gettext("В счёт какого месяца"),data->poi.vkm.ravno());
  if(data->poi.koment.getdlinna() > 1)
   fprintf(ff,"%s: %s\n",gettext("Коментарий"),data->poi.koment.ravno());
  if(data->poi.grup_podr.getdlinna() > 1)
   fprintf(ff,"%s: %s\n",gettext("Код группы подразделения"),data->poi.grup_podr.ravno());
  if(data->poi.mnks.getdlinna() > 1)
   fprintf(ff,"%s: %s\n",gettext("Карт-счёт"),data->poi.mnks.ravno());
  
 }
fprintf(ff,"\
-----------------------------------------------------------------------------\n");
fprintf(ff,gettext(" Т/н |       Фамилия      |День| Счет | Дата  |Под.|Н/з|  Сумма   |Коментарий\n"));
fprintf(ff,"\
-----------------------------------------------------------------------------\n");

double sumai=0.;
double itogo=0.;
double itogoo=0.;
float kolstr1=0.;
char podrz[30];

memset(podrz,'\0',sizeof(podrz));
int nomer_ved=0;
//int kol_na_liste=0;
short metkasap=0;
int kollist=0;
char famil[50];
char imq[30];
char otch[30];
short d,m,g;
double sum=0.;
int kolstrok=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
//  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(l_nahud_prov_str(row,&data->poi,data->window) != 0)
     continue;
  
  if(iceb_u_SRAV(podrz,row[13],0) != 0)
   {
    if(podrz[0] != '\0' && metkasap != 0)    
     {
      fprintf(ffpodr,"%3s %-*.*s %10.2f\n",
      podrz,
      iceb_u_kolbait(40,naipod),iceb_u_kolbait(40,naipod),naipod,
      itogo);
      
      fprintf(ff,"\
-----------------------------------------------------------------------------\n");
      fprintf(ff,"%*s:%10.2f\n",
      iceb_u_kolbait(55,gettext("Итого по подразделению")),gettext("Итого по подразделению"),itogo);

      itogw(ffved,itogo,data->window);

      SUMPODR.plus(itogo,-1);
      KOLLPODR.plus(kollist,-1);
      fprintf(ffved,"\f");
      itogo=0.;
      nomer_ved=0;
     }

    strcpy(podrz,row[13]);
    metkasap=0;
   }

   
  iceb_u_rsdat(&d,&m,&g,row[0],2);

  sum=atof(row[4]);
  if(sum == 0.)
    continue;
  if(data->prn == 2)
    sum*=-1;
  
  iceb_u_polen(row[15],famil,sizeof(famil),1,' ');
  iceb_u_polen(row[15],imq,sizeof(imq),2,' ');
  iceb_u_polen(row[15],otch,sizeof(otch),3,' ');
  sprintf(strsql,"%s %*.*s. %*.*s.",
  famil,
  iceb_u_kolbait(1,imq),
  iceb_u_kolbait(1,imq),
  imq,
  iceb_u_kolbait(1,otch),
  iceb_u_kolbait(1,otch),
  otch);
  
  fprintf(ff,"%-5s|%-*.*s|%4d|%-*s|%02d.%d|%4s|%3s|%10.2f|%s\n",
  row[1],
  iceb_u_kolbait(20,strsql),iceb_u_kolbait(20,strsql),strsql,
  d,
  iceb_u_kolbait(6,row[5]),row[5],
  atoi(row[7]),atoi(row[6]),row[13],row[9],
  sum,row[12]);

  if(metkasap == 0)
   {
    //Узнаем наименование подразделения
    memset(naipod,'\0',sizeof(naipod));
    sprintf(strsql,"select naik from Podr where kod=%s",row[13]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
      strncpy(naipod,row1[0],sizeof(naipod)-1);

    kolstrok=0;
    if(zarstfn53w(naipod,data->mp,data->gp,shrpz,"",&kolstrok,ffved) != 0)
      return;
    sspp1w(ffved,1,&kolstrok);
    kollist=1;
   }

  kolstrok+=2;

  if(kolstrok > kol_strok_na_liste)
   {
    kollist++;
    kolstrok=2;
    sspp1w(ffved,kollist,&kolstrok);
   }

  zarstvedw(&nomer_ved,atoi(row[1]),row[15],sum,ffved);

  metkasap++;
  sumai+=sum;
  itogo+=sum; 
  itogoo+=sum; 

 }
fprintf(ff,"\
-----------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(55,gettext("Итого по подразделению")),gettext("Итого по подразделению"),itogo);
fprintf(ff,"\
-----------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(55,gettext("Итого")),gettext("Итого"),sumai);

if(metkasap != 0)    
 {
  itogw(ffved,itogo,data->window);
  SUMPODR.plus(itogo,-1);
  KOLLPODR.plus(kollist,-1);
 }
fprintf(ffpodr,"%3s %-*.*s %10.2f\n",
podrz,
iceb_u_kolbait(40,naipod),iceb_u_kolbait(40,naipod),naipod,
itogo);

fprintf(ffpodr,"\
--------------------------------------------------------\n");
fprintf(ffpodr,"%44s %10.2f\n",gettext("Итого"),itogoo);


iceb_podpis(ffpodr,data->window);

fclose(ffpodr);

iceb_podpis(ff,data->window);
fclose(ff);
fclose(ffved);

sumprvedw(&SUMPODR,&KOLLPODR,imafved);



class iceb_u_spisok imafr;
class iceb_u_spisok naimf;
imafr.plus(imaf);
imafr.plus(imafved);
imafr.plus(imafpodr);

naimf.plus(nairas);
naimf.plus(gettext("Ведомость на выдачу зароботной платы"));
naimf.plus(gettext("Реестр платёжных ведомостей по подразделениям"));

for(int nomer=0; nomer < imafr.kolih(); nomer++)
 iceb_ustpeh(imafr.ravno(nomer),1,data->window);

iceb_rabfil(&imafr,&naimf,"",0,data->window);
}
/*****************************/
/*Импорт начилений/удержаний*/
/*****************************/

void l_nahud_sf6(class l_nahud_data *data)
{


iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Импорт из файла"));//0
punkt_m.plus(gettext("Импорт из подсистемы \"Учёт кассовых ордеров.\""));//1


int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);

if(nomer == 0)
 imp_nu(data->prn,data->kod_nah_ud,data->mp,data->gp,data->window);
if(nomer == 1)
 imp_kr(data->prn,data->kod_nah_ud,data->mp,data->gp,data->window);


}

/*****************************/
/*Выгрузка для зачисления на карт-счета*/
/*****************************/

void l_nahud_f7(class l_nahud_data *data)
{
int kod_banka=0;
class iceb_u_str kod00("00");
int nomer=0;
if((nomer=zarbanksw(&kod00,&kod_banka,data->window)) < 0)
 return;
switch(nomer)
 {
  case 1:
    zvb_pibw(data->prn,data->zapros.ravno(),&data->poi,data->window);
    break;
  case 2:
    zvb_nadraw(data->prn,data->zapros.ravno(),&data->poi,data->window);
    break;
  case 3:
    zvb_ukrsocw(data->prn,data->zapros.ravno(),&data->poi,data->window);
    break;
  case 4:
    zvb_ukrsibw(data->prn,data->zapros.ravno(),&data->poi,data->window);
    break;
  case 5:
    zvb_kreditpromw(data->prn,data->zapros.ravno(),&data->poi,data->window);
    break;
  case 6:
    zvb_ukrpromw(data->prn,data->zapros.ravno(),&data->poi,data->window);
    break;
  case 7:
    zvb_ukreksimw(data->prn,data->zapros.ravno(),&data->poi,data->window);
    break;
  case 8:
    zvb_hreshatikw(data->prn,data->zapros.ravno(),&data->poi,data->window);
    break;
  case 9:
    zvb_ibank2uaw(data->prn,data->zapros.ravno(),&data->poi,kod00.ravno(),data->window);
    break;
  case 10:
    zvb_alfabankw(data->prn,data->zapros.ravno(),&data->poi,data->window);
    break;
  case 11:
    zvb_ukrgazw(data->prn,data->zapros.ravno(),&data->poi,data->window);
    break;
  case 12:
    zvb_promekonomw(data->prn,data->zapros.ravno(),&data->poi,data->window);
    break;
 }

}
/*********************************/
/*Смена дня*/
/*********************************/
int l_nahud_sd(class  l_nahud_data *data)
{

class iceb_u_str den("");
if(iceb_menu_vvod1(gettext("Введите день на который нужно заменить"),&den,3,data->window) != 0)
 return(1);


 
short dn=den.ravno_atoi();
char strsql[512];
short d,m,g;

sprintf(strsql,"%d.%d.%d",dn,data->mp,data->gp);
if(iceb_u_rsdat(&d,&m,&g,strsql,1) != 0)
 {
  
   iceb_menu_soob(gettext("Не правильно ввели день!"),data->window);
   return(1);
 }
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;
//cur->poz_cursor(0);
if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return(1);
 }
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);
float kolstr1=0;
int nom_zap=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(l_nahud_prov_str(row,&data->poi,data->window) != 0)
   continue;
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  //проверяем блокировку карточки
  sprintf(strsql,"select blok from Zarn where tabn=%s and god=%d and mes=%d",row[1],g,m);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   if(atoi(row1[0]) != 0)
    {
     sprintf(strsql,"%s !%s: %s %s\n",gettext("Карточка заблокирована"),
     gettext("Табельный номер"),row[1],row[15]);
     iceb_menu_soob(strsql,data->window);
     continue;
    }
  nom_zap=atoi(row[9]);
  for(;nom_zap < 1000 ;nom_zap++)
   {
    
    sprintf(strsql,"update Zarp set datz='%04d-%02d-%02d', nomz=%d where datz='%s' and tabn=%s and prn=%s and \
knah=%s and godn=%s and mesn=%s and podr=%s and nomz=%s",
    g,m,dn,nom_zap,row[0],row[1],row[2],row[3],row[6],row[7],row[13],row[9]);
    
    if(iceb_sql_zapis(strsql,1,1,data->window) == 0)
      break;

   }
  zaravprw(atoi(row[1]),data->mp,data->gp,NULL,data->window);

 }
return(0);
}

/*********************************/
/*Смена в счет какого месяца*/
/*********************************/
int l_nahud_svkm(class  l_nahud_data *data)
{

class iceb_u_str vkm("");
if(iceb_menu_vvod1(gettext("Введите в счёт какого месяца (мм.гггг)"),&vkm,3,data->window) != 0)
 return(1);
 
char strsql[512];
short d,m,g;
short mz,gz;
if(iceb_u_rsdat1(&mz,&gz,vkm.ravno()) != 0)
 {
   iceb_menu_soob(gettext("Не правильно ввели дату!"),data->window);
   return(1);
  
 }
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return(1);
 }
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);
float kolstr1=0;
int nom_zap=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(l_nahud_prov_str(row,&data->poi,data->window) != 0)
   continue;
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  //проверяем блокировку карточки
  sprintf(strsql,"select blok from Zarn where tabn=%s and god=%d and mes=%d",row[1],g,m);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   if(atoi(row1[0]) != 0)
    {
     sprintf(strsql,"%s !%s: %s %s\n",gettext("Карточка заблокирована"),
     gettext("Табельный номер"),row[1],row[15]);
     iceb_menu_soob(strsql,data->window);
     continue;
    }
  nom_zap=atoi(row[9]);
  for(;nom_zap < 1000 ;nom_zap++)
   {
    
    sprintf(strsql,"update Zarp set godn=%d,mesn=%d, nomz=%d where datz='%s' and tabn=%s and prn=%s and \
knah=%s and godn=%s and mesn=%s and podr=%s and nomz=%s",
    gz,mz,nom_zap,row[0],row[1],row[2],row[3],row[6],row[7],row[13],row[9]);
    
    if(iceb_sql_zapis(strsql,1,1,data->window) == 0)
      break;

   }
 }
return(0);
}



/*******************************************/
/*Смена реквизитов в записях*/
/*******************************************/

void l_nahud_srvz(class  l_nahud_data *data)
{
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Виберите нужное"));

zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Сменить счёт"));//0
punkt_m.plus(gettext("Сменить день записи"));//1
punkt_m.plus(gettext("Сменить реквизит \"В счёт какого месяца\""));//2

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);

  if(l_nahud_prov_blok(data->mp,data->gp,0,data->window) != 0)
     return;

  switch (nomer)
   {
    case -1:
      return;

    case 0:
    
      if(zar_sm_shet(data->zapros.ravno(),&data->poi,data->window) == 0)
        l_nahud_create_list(data);
      return;

    case 1:
      if(l_nahud_sd(data) == 0)
        l_nahud_create_list(data);
      return;

    case 2:
      if(l_nahud_svkm(data) == 0)
        l_nahud_create_list(data);
      return;




   }
   
 }

}
