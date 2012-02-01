/*$Id: iceb_vprov.c,v 1.24 2011-02-21 07:36:08 sasa Exp $*/
/*12.11.2008	18.08.2004	Белых А.И.	iceb_vprov.c
Ввод и корректировка проводок
*/
#include <stdlib.h>
#include <ctype.h>
#include "iceb_libbuh.h"
enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_SH_DEBET,
  E_SH_KREDIT,
  E_SUMA,
  E_DATAP,
  E_KOMENT,
  E_KEKV,
  KOLENTER  
 };

class iceb_vprov_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет
  //новые данные
  iceb_u_str sh_debet;
  iceb_u_str sh_kredit;
  iceb_u_str suma;
  iceb_u_str datap;
  iceb_u_str koment;
  iceb_u_str kontr_sh;
  iceb_u_str kontr_shk;
  class iceb_u_str kekv;
  
  //Старые данные
  iceb_u_str sh_debet_z;
  iceb_u_str sh_kredit_z;
  double     suma_z;
  iceb_u_str datap_z;//Дата корректируемой проводки
  iceb_u_str koment_z;
  time_t     vrem_z;
  int        val_z;

  short dd,md,gd;   //Дата документа к котораму делаются проводки
  int        sklad;
  iceb_u_str nomdok;
  iceb_u_str kodop;
  int tipz;
  iceb_u_str metkasys;
  short      kolprovz;
  class iceb_u_str mtsh;
  int        metka_prov;              

  iceb_vprov_data() //Конструктор
   {
    metka_prov=0;
    kl_shift=0;
    voz=1;
    sh_debet.new_plus("");
    sh_kredit.new_plus("");
    suma.new_plus("");
    datap.new_plus("");
    koment.new_plus("");
    kolprovz=2;  
   }

  
  void read_rek()
   {
    metka_prov=1;
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
    metka_prov=0;
   }
  
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }
 };

void    iceb_vprov_v_vvod(GtkWidget *widget,class iceb_vprov_data *data);
void  iceb_vprov_v_knopka(GtkWidget *widget,class iceb_vprov_data *data);
void   iceb_vprov_rekviz(class iceb_vprov_data *data);
gboolean   iceb_vprov_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_vprov_data *data);

int iceb_vprov_zap(class iceb_vprov_data *data);
short pmsh(const char *sh,const char *nn,int skk,int d,int m,int g,const char *mtsh,GtkWidget *wpredok);
int iceb_vprov_prsh(iceb_vprov_data *data,int metka);
void  iceb_vprov_v_e_knopka(GtkWidget *widget,class iceb_vprov_data *data);

extern char *name_system;
extern SQL_baza bd;
 
int iceb_vprov(const char *metkasys,
const char *shet_d,
const char *shet_k,
const char *suma,
const char *data_prov, //Дата проводки
const char *koment,
const char *nomdok, //номер документа
const char *kodop, //Код операции
int sklad,   //Код подразделения
short dd,short md,short gd, //Дата документа
const char *kontr, //Код контрагента для корректируемой проводки если он есть
time_t vremz, //Время записи корректируемой проводки
int ktozap,  //Кто записал
const char *mtsh, //Перечень материальных счетов
int pods, //1-МУ 2-ЗП 3-УОС 4-ПЛТ 5-ТРЕ 6-УСЛ 7-КО 8-УКР
int tipz, //1-приход 2-расход
int val,//0-балансовый -1 внебалансовый
int kekv, //код экономической классификации расходов
GtkWidget *wpredok)
{
iceb_vprov_data data;
char strsql[400];
iceb_u_str naim_kodop;
naim_kodop.plus("");
SQL_str row;
SQLCURSOR cur;

data.sh_debet.new_plus(shet_d);
data.sh_kredit.new_plus(shet_k);
data.suma.new_plus(suma);
data.datap.new_plus(data_prov);
data.koment.new_plus(koment);

data.sh_debet_z.new_plus(shet_d);
data.sh_kredit_z.new_plus(shet_k);
data.suma_z=atof(suma);
data.datap_z.new_plus(data_prov);
data.koment_z.new_plus(koment);
data.vrem_z=vremz;
data.dd=dd;
data.md=md;
data.gd=gd;
data.tipz=tipz;
data.sklad=sklad;
data.nomdok.plus(nomdok);
data.kodop.plus(kodop);
data.metkasys.plus(metkasys);
data.val_z=val;
if(kekv != 0)
 data.kekv.plus(kekv);
else
 data.kekv.plus("");
 
if(val == -1)
 data.kolprovz=1;
data.mtsh.plus(mtsh);

if(pods == 1)
 {
  if(tipz == 1)
    sprintf(strsql,"select naik from Prihod where kod='%s'",kodop);  
  if(tipz == 2)
    sprintf(strsql,"select naik from Rashod where kod='%s'",kodop);  

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   naim_kodop.new_plus(row[0]);
 }

if(pods == 3)
 {
  if(tipz == 1)
    sprintf(strsql,"select naik from Uospri where kod='%s'",kodop);  
  if(tipz == 2)
    sprintf(strsql,"select naik from Uosras where kod='%s'",kodop);  

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   naim_kodop.new_plus(row[0]);
 }


if(pods == 4)
 {
  sprintf(strsql,"select naik from Opltp where kod='%s'",kodop);  
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   naim_kodop.new_plus(row[0]);
 }
if(pods == 5)
 {
  sprintf(strsql,"select naik from Opltt where kod='%s'",kodop);  
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   naim_kodop.new_plus(row[0]);
 }

if(pods == 6)
 {
  if(tipz == 1)
    sprintf(strsql,"select naik from Usloper1 where kod='%s'",kodop);  
  if(tipz == 2)
    sprintf(strsql,"select naik from Usloper2 where kod='%s'",kodop);  

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   naim_kodop.new_plus(row[0]);
 }

if(pods == 7)
 {
  if(tipz == 1)
    sprintf(strsql,"select naik from Kasop1 where kod='%s'",kodop);  
  if(tipz == 2)
    sprintf(strsql,"select naik from Kasop2 where kod='%s'",kodop);  

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   naim_kodop.new_plus(row[0]);
 }

if(pods == 8)
 {
  sprintf(strsql,"select naik from Ukrvkr where kod='%s'",kodop);  
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   naim_kodop.new_plus(row[0]);
 }


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(shet_d[0] == '\0')
  sprintf(strsql,"%s %s",name_system,gettext("Ввод новой проводки."));
else
  sprintf(strsql,"%s %s",name_system,gettext("Корректировка проводки."));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_vprov_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
iceb_u_str repl;
if(shet_d[0] == '\0')
  repl.new_plus(gettext("Ввод новой проводки."));
else
  repl.new_plus(gettext("Корректировка проводки."));

repl.ps_plus(gettext("Операция"));
repl.plus(":");
repl.plus(kodop);
repl.plus("/");
repl.plus(naim_kodop.ravno());

GtkWidget *label=gtk_label_new(repl.ravno_toutf());

/*************
repl.plus(" ");
repl.plus(data.naim_kodop.ravno());
repl.plus(" (");
if(mvnp == 0)
  repl.plus(gettext("Внешняя"));
if(mvnp == 1)
  repl.plus(gettext("Внутренняя"));
if(mvnp == 2)
  repl.plus(gettext("Изменения стоимости"));
repl.plus(")");

if(kikz.getdlinna() > 1)
 repl.ps_plus(kikz.ravno());
****************/ 

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

//sprintf(strsql,"%s",gettext("Счёт дебета"));
//label=gtk_label_new(iceb_u_toutf(strsql));
//gtk_box_pack_start (GTK_BOX (hbox[E_SH_DEBET]), label, FALSE, FALSE, 0);

sprintf(strsql,"%s",gettext("Счёт дебета"));
data.knopka_enter[E_SH_DEBET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SH_DEBET]), data.knopka_enter[E_SH_DEBET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SH_DEBET]),"clicked",GTK_SIGNAL_FUNC(iceb_vprov_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SH_DEBET]),(gpointer)E_SH_DEBET);
tooltips_enter[E_SH_DEBET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SH_DEBET],data.knopka_enter[E_SH_DEBET],gettext("Выбор счёта в плане счетов."),NULL);

data.entry[E_SH_DEBET] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SH_DEBET]), data.entry[E_SH_DEBET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SH_DEBET]), "activate",GTK_SIGNAL_FUNC(iceb_vprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SH_DEBET]),data.sh_debet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SH_DEBET]),(gpointer)E_SH_DEBET);

//sprintf(strsql,"%s",gettext("Счёт кредита"));
//label=gtk_label_new(iceb_u_toutf(strsql));
//gtk_box_pack_start (GTK_BOX (hbox[E_SH_KREDIT]), label, FALSE, FALSE, 0);

sprintf(strsql,"%s",gettext("Счёт кредита"));
data.knopka_enter[E_SH_KREDIT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SH_KREDIT]), data.knopka_enter[E_SH_KREDIT], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SH_KREDIT]),"clicked",GTK_SIGNAL_FUNC(iceb_vprov_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SH_KREDIT]),(gpointer)E_SH_KREDIT);
tooltips_enter[E_SH_KREDIT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SH_KREDIT],data.knopka_enter[E_SH_KREDIT],gettext("Выбор счёта в плане счетов."),NULL);

data.entry[E_SH_KREDIT] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SH_KREDIT]), data.entry[E_SH_KREDIT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SH_KREDIT]), "activate",GTK_SIGNAL_FUNC(iceb_vprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SH_KREDIT]),data.sh_kredit.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SH_KREDIT]),(gpointer)E_SH_KREDIT);

sprintf(strsql,"%s",gettext("Сумма"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_SUMA] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMA]), "activate",GTK_SIGNAL_FUNC(iceb_vprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.suma.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMA]),(gpointer)E_SUMA);

//sprintf(strsql,"%s",gettext("Дата проводки (д.м.г)"));
//label=gtk_label_new(iceb_u_toutf(strsql));
//gtk_box_pack_start (GTK_BOX (hbox[E_DATAP]), label, FALSE, FALSE, 0);

sprintf(strsql,"%s",gettext("Дата проводки (д.м.г)"));
data.knopka_enter[E_DATAP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAP]), data.knopka_enter[E_DATAP], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAP]),"clicked",GTK_SIGNAL_FUNC(iceb_vprov_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAP]),(gpointer)E_DATAP);
tooltips_enter[E_DATAP]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAP],data.knopka_enter[E_DATAP],gettext("Выбор даты."),NULL);

data.entry[E_DATAP] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAP]), data.entry[E_DATAP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAP]), "activate",GTK_SIGNAL_FUNC(iceb_vprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAP]),data.datap.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAP]),(gpointer)E_DATAP);


sprintf(strsql,"%s",gettext("Коментарий"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(iceb_vprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);

sprintf(strsql,"%s",gettext("Код экономической классификации затрат"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KEKV]), label, FALSE, FALSE, 0);

data.entry[E_KEKV] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox[E_KEKV]), data.entry[E_KEKV], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KEKV]), "activate",GTK_SIGNAL_FUNC(iceb_vprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KEKV]),data.kekv.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KEKV]),(gpointer)E_KEKV);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Записать"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Записать введенную в меню информацию."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(iceb_vprov_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(iceb_vprov_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(iceb_vprov_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));







return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  iceb_vprov_v_e_knopka(GtkWidget *widget,class iceb_vprov_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.new_plus("");
naim.new_plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
/*g_print("iceb_vprov_v_e_knopka knop=%d\n",*knop);*/

switch (knop)
 {
  case E_DATAP:

    if(iceb_calendar(&data->datap,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAP]),data->datap.ravno());
      
    return;  

   
  case E_SH_DEBET:

    iceb_vibrek(1,"Plansh",&data->sh_debet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SH_DEBET]),data->sh_debet.ravno_toutf());

    return;  

  case E_SH_KREDIT:

    iceb_vibrek(1,"Plansh",&data->sh_kredit,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SH_KREDIT]),data->sh_kredit.ravno_toutf());

    return;  


 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_vprov_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_vprov_data *data)
{

printf("iceb_vprov_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);


  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_vprov_v_knopka(GtkWidget *widget,class iceb_vprov_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
/*g_print("iceb_vprov_v_knopka knop=%d\n",*knop);*/

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    if(iceb_vprov_zap(data) != 0)
     return;
    
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case FK4:
    data->clear_rek();
    return;

  case FK10:
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    iceb_vprov_v_vvod(GtkWidget *widget,class iceb_vprov_data *data)
{
iceb_u_str repl;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (enter)
 {
  case E_SH_DEBET:
    data->sh_debet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    if(data->metka_prov == 0)
      iceb_vprov_prsh(data,0);
    
    break;
  case E_SH_KREDIT:
    data->sh_kredit.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    if(data->metka_prov == 0)
      iceb_vprov_prsh(data,1);
    break;
  case E_DATAP:
    data->datap.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_SUMA:
    data->suma.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KOMENT:
    data->koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KEKV:
    data->kekv.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
 }

enter++;
  
if(enter >= KOLENTER)
 enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}

/*********************/
/*Запись введенной информации*/
/*************************/

int iceb_vprov_zap(class iceb_vprov_data *data)
{
iceb_u_str repl;

//if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' && VV->VVOD_SPISOK_return_data(1)[0] == '\0')
if(data->sh_debet.getdlinna() <= 1 && data->sh_kredit.getdlinna() <= 1)
 {
  repl.new_plus(gettext("Не введены все обязательные реквизиты !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 }


int val=0; int kolprov=2;
if(data->sh_debet.getdlinna() <= 1 || data->sh_kredit.getdlinna() <= 1)
 {
  val=-1; kolprov=1;
 }

if(data->suma.getdlinna() <= 1 || data->datap.getdlinna() <= 1)
 {
  repl.new_plus(gettext("Не введены все обязательные реквизиты !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 }


if(data->suma.ravno_atof() == 0.)
 {
  repl.new_plus(gettext("Сумма равна нолю !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 }


if(data->datap.prov_dat() != 0)
 {
  repl.new_plus(gettext("Не верно введена дата !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 }

if(iceb_pvglkni(data->datap_z.ravno(),data->window) != 0)
 {
  repl.new_plus(gettext("Дата заблокована !"));
  repl.ps_plus(data->datap_z.ravno());
  iceb_menu_soob(&repl,data->window);
  return(1);
 }
short dp,mp,gp;
if(iceb_u_rsdat(&dp,&mp,&gp,data->datap.ravno(),1) != 0)
 {
  repl.new_plus(gettext("Не верно введена дата проводки !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 
 }
if(iceb_pvglkni(data->datap.ravno(),data->window) != 0)
 {
  repl.new_plus(gettext("Дата заблокована !"));
  repl.ps_plus(data->datap.ravno());
  iceb_menu_soob(&repl,data->window);
  return(1);
 }

OPSHET shetv;

if(val == 0 || data->sh_debet.getdlinna() > 1)
 if(iceb_prsh1(data->sh_debet.ravno(),&shetv,data->window) != 0)
   return(1);
char strsql[512];
if(val == 0 && shetv.stat == 1)
 {
  sprintf(strsql,gettext("Счет %s внебалансовый !"),data->sh_debet.ravno());
  repl.new_plus(strsql);
  iceb_menu_soob(&repl,data->window);
  return(1);
 }       

/*Для счетов с развернутым сальдо*/
short msrs=0;
if(shetv.saldo == 3)
 msrs=1;

memset(&shetv,'\0',sizeof(shetv));
if(val == 0 || data->sh_kredit.getdlinna() > 1)
 if(iceb_prsh1(data->sh_kredit.ravno(),&shetv,data->window) != 0)
  return(1);
  
if(val == 0 && shetv.stat == 1)
 {
  sprintf(strsql,gettext("Счет %s внебалансовый !"),data->sh_kredit.ravno());
  repl.new_plus(strsql);
  iceb_menu_soob(&repl,data->window);
  return(1);
 }       


/*Для счетов с развернутым сальдо*/
short msrs1=0;
if(shetv.saldo == 3)
 msrs1=1;

/*Код контрагента для главного счета*/
iceb_u_str kod;
iceb_u_str naim;
//memset(dkk,'\0',sizeof(dkk));
//memset(kodo,'\0',sizeof(kodo));
short metka_c=0;
if(msrs == 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Введите код контрагента для счета"));
  repl.plus(" ");
  repl.plus(data->sh_debet.ravno());

  while(metka_c == 0)
   {
  
    if(iceb_menu_vvod1(&repl,&kod,20,data->window) != 0)
      return(1);

    if(isdigit(kod.ravno()[0]) == 0) //Введен символ а не число
     {

      naim.new_plus(kod.ravno());
      kod.new_plus("");
      
      if(iceb_l_kontrsh(1,data->sh_debet.ravno(),&kod,&naim,data->window) != 0)
        continue;
      
     }  

    //Проверяем есть ли такой код контрагента
    sprintf(strsql,"select ns from Skontr where ns='%s' and kodkon='%s'",
    data->sh_debet.ravno_filtr(),kod.ravno_filtr());
    if(sql_readkey(&bd,strsql) != 1)
     {
      iceb_u_str repl;
      repl.plus(gettext("Не нашли код контрагента"));
      repl.plus(" ");
      repl.plus(kod.ravno());
      repl.plus(" ");
      repl.plus(gettext("в списке счета"));
      repl.plus(" ");
      repl.plus(data->sh_debet.ravno());
      repl.plus(" !");

      iceb_menu_soob(&repl,data->window);
      continue;
     }

    data->kontr_sh.new_plus(kod.ravno());  
    break;
   } 
 }

/*Код контрагента для субсчета*/

if(msrs1 == 1)
 {
  kod.new_plus("");
  
  iceb_u_str repl;
  repl.plus(gettext("Введите код контрагента для счета"));
  repl.plus(" ");
  repl.plus(data->sh_kredit.ravno());

  while(metka_c == 0)
   {
  
    if(iceb_menu_vvod1(&repl,&kod,20,data->window) != 0)
      return(1);

    if(isdigit(kod.ravno()[0]) == 0) //Введен символ а не число
     {

      naim.new_plus(kod.ravno());
      kod.new_plus("");
      
      if(iceb_l_kontrsh(1,data->sh_kredit.ravno(),&kod,&naim,data->window) != 0)
        continue;
      
     }  

    //Проверяем есть ли такой код контрагента
    sprintf(strsql,"select ns from Skontr where ns='%s' and kodkon='%s'",
    data->sh_kredit.ravno_filtr(),kod.ravno_filtr());
    if(sql_readkey(&bd,strsql) != 1)
     {
      iceb_u_str repl;
      repl.plus(gettext("Не нашли код контрагента"));
      repl.plus(" ");
      repl.plus(kod.ravno());
      repl.plus(" ");
      repl.plus(gettext("в списке счета"));
      repl.plus(" ");
      repl.plus(data->sh_kredit.ravno());
      repl.plus(" !");

      iceb_menu_soob(&repl,data->window);
      continue;
     }

    data->kontr_shk.new_plus(kod.ravno());  
    break;
   } 

 }
 double deb=data->suma.ravno_atof();
 double kre=0.;
 time_t vrem;
 time(&vrem);
 if(val == 0)
  if(iceb_zapprov(val,gp,mp,dp,
   data->sh_debet.ravno_filtr(),
   data->sh_kredit.ravno_filtr(),
   data->kontr_sh.ravno_filtr(),
   data->kontr_shk.ravno_filtr(),
   data->metkasys.ravno_filtr(),
   data->nomdok.ravno_filtr(),
   data->kodop.ravno_filtr(),
   deb,kre,
   data->koment.ravno_filtr(),
   kolprov,
   data->sklad,vrem,
   data->gd,data->md,data->dd,
   data->tipz,
   data->kekv.ravno_atoi(),
   data->window) != 0)
      return(1);

 if(val == -1)
  {
   if(data->sh_kredit.getdlinna() > 1)
    {
//     if(zapprov(val,g1,m1,d1,VV->VVOD_SPISOK_return_data(1),VV->VVOD_SPISOK_return_data(0),kodo,kodo1,kto,nn,kop,kre,deb,VV->VVOD_SPISOK_return_data(4),kolprov,skk,vrem,g,m,d) != 0)

     if(iceb_zapprov(val,gp,mp,dp,
       data->sh_kredit.ravno_filtr(),
       data->sh_debet.ravno_filtr(),
       data->kontr_sh.ravno_filtr(),
       data->kontr_shk.ravno_filtr(),
       data->metkasys.ravno_filtr(),
       data->nomdok.ravno_filtr(),
       data->kodop.ravno_filtr(),
       deb,kre,
       data->koment.ravno_filtr(),
       kolprov,
       data->sklad,vrem,
       data->gd,data->md,data->dd,
       data->tipz,
       data->kekv.ravno_atoi(),
       data->window) != 0)

     return(1);
    }         
   else
//    if(zapprov(val,g1,m1,d1,VV->VVOD_SPISOK_return_data(0),VV->VVOD_SPISOK_return_data(1),kodo,kodo1,kto,nn,kop,deb,kre,VV->VVOD_SPISOK_return_data(4),kolprov,skk,vrem,g,m,d) != 0)
    if(iceb_zapprov(val,gp,mp,dp,
     data->sh_debet.ravno_filtr(),
     data->sh_kredit.ravno_filtr(),
     data->kontr_sh.ravno_filtr(),
     data->kontr_shk.ravno_filtr(),
     data->metkasys.ravno_filtr(),
     data->nomdok.ravno_filtr(),
     data->kodop.ravno_filtr(),
     deb,kre,
     data->koment.ravno_filtr(),
     kolprov,
     data->sklad,vrem,
     data->gd,data->md,data->dd,
     data->tipz,
     data->kekv.ravno_atoi(),
     data->window) != 0)
        return(1);
  }     
 if(data->sh_debet_z.getdlinna() > 1)
  { 
   short d1,m1,g1;
   iceb_u_rsdat(&d1,&m1,&g1,data->datap_z.ravno(),1);
   deb=data->suma_z;
   kre=0.;
   iceb_udprov(data->val_z,g1,m1,d1,data->sh_debet.ravno_filtr(),
   data->sh_kredit_z.ravno_filtr(),data->vrem_z,deb,kre,data->koment_z.ravno_filtr(),
   data->kolprovz,data->window);
  }

return(0);

}
/**************************************/
/*Проверка корректности ввода счёта*/
/************************************/

int iceb_vprov_prsh(iceb_vprov_data *data,int metka)
{
iceb_u_str shet;
if(metka == 0)
 shet.plus(data->sh_debet.ravno());
if(metka == 1)
 shet.plus(data->sh_kredit.ravno());
 
OPSHET shetv;

if(iceb_prsh1(shet.ravno(),&shetv,data->window) != 0)
  return(1);
  
if(data->mtsh.getdlinna() > 1)
 if(pmsh(shet.ravno(),data->nomdok.ravno(),data->sklad,data->dd,data->md,data->gd,data->mtsh.ravno(),data->window) != 0)
  {
   iceb_u_spisok repl;
   char strsql[512];
   sprintf(strsql,gettext("Счет %s материальный и по нему нет записей в документе !"),shet.ravno());
   repl.plus(strsql);
   repl.plus(gettext("Все верно ? Вы уверены ?"));
   if(iceb_menu_danet(&repl,2,data->window) == 2)
    {
     if(metka == 0)
      {
       data->sh_debet.new_plus("");
       gtk_entry_set_text(GTK_ENTRY(data->entry[E_SH_DEBET]),data->sh_debet.ravno_toutf());
      }
     if(metka == 1)
      {
       data->sh_kredit.new_plus("");
       gtk_entry_set_text(GTK_ENTRY(data->entry[E_SH_DEBET]),data->sh_kredit.ravno_toutf());
      }
     return(1);
    }

  }
return(0);

}

/*****************/
/*Проверка счета */
/*****************/
/*Если счет материальный и нет записи материала
в накладной с этим счетом то счет введен ошибочно
Если вернули 0 - счет правильный
	     1 - нет
	     */


short pmsh(const char *sh,  //Счет для проверки
const char *nn,int skk,int d,int m,int g,
const char *mtsh,GtkWidget *wpredok)
{

char		strsql[512];
short           voz;
int		i;
int		nk;
long		kolstr;
SQL_str         row,row1;
voz=0;

if(mtsh == NULL)
  return(0);

if(iceb_u_pole1(mtsh,sh,',',1,&i) == 0)
  voz=1;

if(voz == 0)
 return(0);


//printf("sh-%s nn-%s skk-%d %d.%d.%d %s\n",
//sh,nn,skk,d,m,g,mtsh);

sprintf(strsql,"select nomkar from Dokummat1 where datd='%d-%02d-%02d' \
and sklad=%d and nomd='%s'",
g,m,d,skk,nn);
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }

if(kolstr == 0)
 {
  return(1);
 }

voz = 1;

while(cur.read_cursor(&row) != 0)
 {
  nk=atoi(row[0]);
  /*Читаем карточку материала*/
  if(nk != 0)
   {
    sprintf(strsql,"select shetu from Kart where sklad=%d and nomk=%d",
    skk,nk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      printf(gettext("Не нашли карточки %d на складе %d !\n"),
      nk,skk);
     }
   }
//  printf("sh=%s row1=%s\n",sh,row1[0]);
  
  if(iceb_u_SRAV(sh,row1[0],1) == 0)
   {
    voz = 0;
    break;
   }

 }
return(voz);

}
