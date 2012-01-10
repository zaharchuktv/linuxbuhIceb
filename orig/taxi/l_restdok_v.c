/*$Id: l_restdok_v.c,v 1.50 2011-02-21 07:36:20 sasa Exp $*/
/*06.11.2007	26.02.2004	Белых А.И.	l_restdok_v.c
Ввод количества и запись в документ
Если вернули 0-записали
             1-нет
*/
#include <stdlib.h>
#include <unistd.h>
#include "i_rest.h"
enum
{
  FK2,
  FK3,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KOLIH,
  E_SUMA,
//  E_EI,
  E_KOMENT,
  KOLENTER
 };


class  restdok_v_data
 {
  public:
  short dd,md,gd;
  iceb_u_str nomd;
  int   skl;
  int   kodm;
  int   podr;
  int   nz;
  
  iceb_u_str kolih;
  iceb_u_str suma;
  iceb_u_str koment;
  double     cena;
  iceb_u_str ei;
  float      ps; //Процент скидки для записи в документ
  double     saldo;  
  
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *label_ei;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  short     metkazapisi; //0-новая запись 1-корректировка
  short     metkaz;//0-товар 1-услуга
  short     metka_prov_saldo; //0-сальдо не проверять 1-проверять
  short metka_kontr_ost; //0-остаток не контролировать 1-контролировать
  double ostatok; //остаток по товару
  int       voz;  
  char imaf_prot[50]; //имя файла с протоколом расчёта остатка
    
  //Конструктор
  restdok_v_data()
   {
    memset(imaf_prot,'\0',sizeof(imaf_prot));
    voz=0;
    cena=0.;
    kl_shift=metkazapisi=0;
    window=NULL;
    koment.plus("");
    kolih.plus("");
    suma.plus("");
    ps=0.;
    saldo=0.;
    metka_prov_saldo=0;    
   }      
  void read_rek()
   {
    for(int i=0; i < KOLENTER ; i++)
     gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }
 };

gboolean   restdok_v_key_press(GtkWidget *widget,GdkEventKey *event,class restdok_v_data *data);
void  restdok_v_knopka(GtkWidget *widget,class restdok_v_data *data);
void  restdok_v_e_knopka(GtkWidget *widget,class restdok_v_data *data);
void    restdok_v_vvod(GtkWidget *widget,class restdok_v_data *data);
int restdok_pk(char *god,char *shet,GtkWidget*);
int   restdok_v_zapis(class restdok_v_data *data);

float l_restdok_v_pr_sk(int kod_gr);

extern SQL_baza	bd;
extern char *name_system;
extern uid_t  kod_operatora;
extern class iceb_get_dev_data config_dev;

int l_restdok_v(short dd,short md,short gd, //дата документа
const char *nomd, //номер документа
int skl, //склад для материалов, подразделение для услуг (с которого будет списваться)
int kodm, //Код материала/услуги
int podr, //код подразделения в которм выписан счёт
int metka, //0-новая запись 1-корректировка
int metkaz, //0-товар 1-услуга
int nz,     //номер записи
float ps_klienta, //процент скидки клиента
const char *kod_klienta, //Код клиента
GtkWidget *wpredok)
{
/*printf("l_restdok-start skl=%d kodm=%d metkaz=%d\n",skl,kodm,metkaz);*/

char strsql[512];
restdok_v_data data;
iceb_u_str nadpis;

//printf("l_restdok_v\n");
    
if(kod_klienta[0] != '\0')
 {

  iceb_u_str imaf_nastr("restnast.alx");
//  iceb_imafn("restnast.alx",&imaf_nastr);
  if(iceb_poldan("Контроль сальдо по клиенту при выписке счетов",strsql,imaf_nastr.ravno(),wpredok) == 0)
   if(iceb_u_SRAV("Вкл",strsql,1) == 0)
    {
     data.saldo=rest_sal_kl(kod_klienta,wpredok);

     if(data.saldo <= 0.)
      {
        sprintf(strsql,"%s:%s\n%s %.2f !\n%s",
        gettext("Код клиента"),
        kod_klienta,
        gettext("Отрицательный остаток"),
        data.saldo,
        gettext("Необходимо пополнение счёта !"));
        iceb_menu_soob(strsql,wpredok);
        return(1);
      }  
     data.metka_prov_saldo=1;
    }
 }
data.kl_shift=0;
data.metkazapisi=metka;
data.dd=dd;
data.md=md;
data.gd=gd;
data.nomd.plus(nomd);
data.skl=skl;
data.kodm=kodm;
data.metkaz=metkaz;   
data.nz=nz;   
data.podr=podr;
data.ps=ps_klienta;

SQL_str row;
SQLCURSOR cur;
char spisok_grup[1024];
memset(spisok_grup,'\0',sizeof(spisok_grup));
iceb_u_str imafil("restnast.alx");
iceb_u_str naim_ei;
naim_ei.plus("");

//iceb_imafn("restnast.alx",&imafil);

//читаем наименование товара
if(metkaz == 0)
 {
  sprintf(strsql,"Список групп товаров для склада %d",skl);
  iceb_poldan(strsql,spisok_grup,imafil.ravno(),wpredok);

  sprintf(strsql,"select naimat,ei,cenapr,kodgr from Material where kodm=%d",kodm);  
 }

if(metkaz == 1) //Код услуги
 {
  sprintf(strsql,"Список групп услуг для подразделения %d",skl);
  iceb_poldan(strsql,spisok_grup,imafil.ravno(),wpredok);

  sprintf(strsql,"select naius,ei,cena,kodgr from Uslugi  where kodus=%d",kodm);  

 }

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
 {
  iceb_u_str repl;
  if(metkaz == 0)
     sprintf(strsql,"%s %d !",gettext("Не найден код товара"),kodm);
  if(metkaz == 1) //Код услуги
     sprintf(strsql,"%s %d !",gettext("Не найден код услуги"),kodm);

  repl.plus(strsql);
  iceb_menu_soob(&repl,data.window);
  return(1);
 }

data.cena=atof(row[2]);
data.ei.new_plus(row[1]);

float proc_sk_zap=0.;
if(metkaz == 0)
 proc_sk_zap=l_restdok_v_pr_sk(atoi(row[3]));

if(proc_sk_zap > data.ps)
 data.ps=proc_sk_zap;

//код материалла может быть задан без выбора склада
if(iceb_u_proverka(spisok_grup,row[3],0,1) != 0)
 {
  if(metkaz == 0)
   {
    sprintf(strsql,"%s %d %s %d !",
    gettext("Не найден код товара"),
    kodm,
    gettext("на складе"),
    skl);
   }
  if(metkaz == 1) //Код услуги
   {
    sprintf(strsql,"%s %d %s %d !",
    gettext("Не найден код услуги "),
    kodm,
    gettext("для подразделения"),
    skl);
   }
  iceb_menu_soob(strsql,data.window);
  return(1);

 }

sprintf(strsql,"%d %s",kodm,row[0]);
nadpis.plus(strsql);

sprintf(strsql,"%s:%.2f",gettext("Процент скидки"),data.ps);
nadpis.ps_plus(strsql);


data.ostatok=0.;
if(metkaz == 0)
 {
  class iceb_u_str imafnast("restnast.alx");
//  iceb_imafn("restnast.alx",&imafnast);
  data.metka_kontr_ost=0;
  sprintf(data.imaf_prot,"konto%d.lst",getpid());

  if(iceb_poldan("Контроль остатка товара при выписке счетов",strsql,imafnast.ravno(),wpredok) == 0)
   if(iceb_u_SRAV("Вкл",strsql,1) == 0)
    {
     data.metka_kontr_ost=1;
     data.ostatok=osttov(kodm,skl,data.ei.ravno(),data.imaf_prot,wpredok);
    }
 }


if(metka == 1)
 {
  //узнаём количество, единицу измерения и цену 
  sprintf(strsql,"select ei,kolih,cena,koment from Restdok1 where datd='%d-%d-%d' and \
nomd='%s' and skl=%d and mz=%d and kodz=%d and nz=%d",
  data.gd,data.md,data.dd,data.nomd.ravno(),data.skl,data.metkaz,data.kodm,data.nz);
  if(sql_readkey(&bd,strsql,&row,&cur) < 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,data.window);
    return(1);
  
   }   
  data.ei.new_plus(row[0]);
  data.kolih.new_plus(row[1]);
  data.cena=atof(row[2]);
  data.koment.new_plus(row[3]);
  sprintf(strsql,"%.2f",atof(row[1])*atof(row[2]));
  data.suma.new_plus(strsql);
 }
else
 {
  data.kolih.new_plus("1");
  data.suma.new_plus(data.cena);
 }
sprintf(strsql,"%s: %.2f",gettext("Цена"),data.cena);
nadpis.ps_plus(strsql);

if(data.metka_kontr_ost == 1)
 {
  nadpis.ps_plus(gettext("Остаток"));
  nadpis.plus(":");
  nadpis.plus(data.ostatok);
 }

//читаем наименование единицы измерения
sprintf(strsql,"select naik from Edizmer where kod='%s'",
data.ei.ravno());  
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  naim_ei.new_plus(row[0]);
 }
nadpis.ps_plus(gettext("Единица измерения"));
nadpis.plus(":");
nadpis.plus(data.ei.ravno());
nadpis.plus("/");
nadpis.plus(naim_ei.ravno());


 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(config_dev.metka_screen == 1)
  gtk_widget_set_usize(data.window,-1,400);
if(data.metkazapisi == 0)
  sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));
if(data.metkazapisi == 1)
  sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));


gtk_window_set_title(GTK_WINDOW(data.window),strsql);
//gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_widget_destroyed),&data.window);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(restdok_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
 
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++) 
 hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

GtkWidget *label=gtk_label_new(nadpis.ravno_toutf());

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox),label);
for(int i=0; i < KOLENTER; i++) 
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

data.knopka_enter[E_KOLIH]=gtk_button_new_with_label(gettext("Количество"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), data.knopka_enter[E_KOLIH], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOLIH]),"clicked",GTK_SIGNAL_FUNC(restdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOLIH]),(gpointer)E_KOLIH);

data.entry[E_KOLIH] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), data.entry[E_KOLIH], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOLIH]), "activate",GTK_SIGNAL_FUNC(restdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH]),data.kolih.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOLIH]),(gpointer)E_KOLIH);

data.knopka_enter[E_SUMA]=gtk_button_new_with_label(gettext("Сумма"));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.knopka_enter[E_SUMA], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SUMA]),"clicked",GTK_SIGNAL_FUNC(restdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SUMA]),(gpointer)E_SUMA);

data.entry[E_SUMA] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMA]), "activate",GTK_SIGNAL_FUNC(restdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.suma.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMA]),(gpointer)E_SUMA);
/**************
data.knopka_enter[E_EI]=gtk_button_new_with_label(gettext("Единица измерения"));
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.knopka_enter[E_EI], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_EI]),"clicked",GTK_SIGNAL_FUNC(restdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_EI]),(gpointer)E_EI);

data.entry[E_EI] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_EI]), "activate",GTK_SIGNAL_FUNC(restdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.ei.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_EI]),(gpointer)E_EI);

data.label_ei=gtk_label_new(naim_ei.ravno_toutf());
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.label_ei, TRUE, TRUE, 0);
***********************/
data.knopka_enter[E_KOMENT]=gtk_button_new_with_label(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.knopka_enter[E_KOMENT], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOMENT]),"clicked",GTK_SIGNAL_FUNC(restdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOMENT]),(gpointer)E_KOMENT);

data.entry[E_KOMENT] = gtk_entry_new_with_max_length (100);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(restdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);



GtkTooltips *tooltips[KOL_F_KL];


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(restdok_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

if(metkaz == 0)
 {
  sprintf(strsql,"F3 %s",gettext("Протокол"));
  data.knopka[FK3]=gtk_button_new_with_label(strsql);
  tooltips[FK3]=gtk_tooltips_new();
  gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Просмотр протокола хода расчёта"),NULL);
  gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]),"clicked",GTK_SIGNAL_FUNC(restdok_v_knopka),&data);
  gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
  gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);
  if(data.metka_kontr_ost == 0)
    gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[FK3]),FALSE);//Недоступна
 }

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(restdok_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

unlink(data.imaf_prot);

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   restdok_v_key_press(GtkWidget *widget,GdkEventKey *event,class restdok_v_data *data)
{
//char  bros[300];

switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);

  case GDK_F3:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
//    printf("Нажата клавиша F10\n");
    return(TRUE);

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
void  restdok_v_knopka(GtkWidget *widget,class restdok_v_data *data)
{
//char bros[300];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("restdok_v_knopka knop=%d\n",knop);

switch ((gint)knop)
 {
  case FK2:
    data->read_rek();
    if(restdok_v_zapis(data) != 0)
      return;
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case FK3:
    if(data->metkaz == 1)
     return;
    iceb_prosf(data->imaf_prot,data->window);
    return;    

  case FK10:
    printf("Нажата кнопка F10\n");
    data->voz=1;
    gtk_widget_destroy(data->window);
    data->window=NULL;
    return;
 }
}

/*****************************/
/*Обработчик нажатия кнопок  ввода */
/*****************************/
void  restdok_v_e_knopka(GtkWidget *widget,class restdok_v_data *data)
{
SQLCURSOR cur;

iceb_u_str kod;
kod.plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case E_KOLIH:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_KOLIH]),"activate");

    iceb_mous_klav(gettext("Количество"),&data->kolih,20,0,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOLIH]),data->kolih.ravno_toutf());

    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_KOLIH]),"activate");
    return;  

  case E_SUMA:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_SUMA]),"activate");

    iceb_mous_klav(gettext("Сумма"),&data->suma,20,0,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA]),data->suma.ravno_toutf());

    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_SUMA]),"activate");
    return;  
/****************
  case E_EI:
    if(l_spisok(100,0,&kod,0,data->window) == 0)
     {
      data->ei.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),data->ei.ravno_toutf());

      sprintf(strsql,"select naik from Edizmer where kod='%s'",kod.ravno());

      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
        gtk_label_set_text(GTK_LABEL(data->label_ei),iceb_u_toutf(row[0]));
      else
        gtk_label_set_text(GTK_LABEL(data->label_ei)," ");

     }
    return;
*******************/    
  case E_KOMENT:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_KOMENT]),"activate");

    iceb_mous_klav(gettext("Коментарий"),&data->koment,99,1,0,0,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOMENT]),data->koment.ravno_toutf());

    return;  


 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    restdok_v_vvod(GtkWidget *widget,class restdok_v_data *data)
{
double suma;
char   bros[100];
//char strsql[300];
//SQL_str row;
SQLCURSOR cur;
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("restdok_v_vvod enter=%d\n",enter);
switch (enter)
 {
  case E_KOLIH:
    data->kolih.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    suma=atof(data->kolih.ravno())*data->cena;
    sprintf(bros,"%.2f",suma);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA]),bros);
        
    break;

  case E_SUMA:
    data->suma.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    suma=atof(data->suma.ravno())/data->cena;
    sprintf(bros,"%.2f",suma);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOLIH]),bros);
    break;
  case E_KOMENT:
    data->koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
/*************
  case E_EI:
    data->ei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    sprintf(strsql,"select naik from Edizmer where kod='%s'",data->ei.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
      gtk_label_set_text(GTK_LABEL(data->label_ei),iceb_u_toutf(row[0]));
    else
      gtk_label_set_text(GTK_LABEL(data->label_ei)," ");

    break;
***************/
 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/***************************/
/*Запись*/
/*************************/

int   restdok_v_zapis(class restdok_v_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;

time_t vrem;
time(&vrem);


if(data->metka_prov_saldo == 1)
 {
  double saldo=data->saldo-(data->kolih.ravno_atof()*data->cena);
  if(saldo <= 0.)
   {
    sprintf(strsql,"%s %.2f !\n%s",
    gettext("Отрицательный остаток"),
    saldo,
    gettext("Необходимо пополнение счёта !"));
    iceb_menu_soob(strsql,data->window);
    return(1);
   }  
 }

if(data->metka_kontr_ost == 1)
 {
  if(data->ostatok - data->kolih.ravno_atof() < -0.00001)
   {
    sprintf(strsql,"%s %f < %f  !\n%s",
    gettext("Отрицательный остаток"),
    data->ostatok,data->kolih.ravno_atof(),
    gettext("Расход невозможно сделать!"));
    iceb_menu_soob(strsql,data->window);
    return(1);
   
   }
 }

sprintf(strsql,"select naik from Edizmer where kod='%s'",data->ei.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найдено единицу измерения"),data->ei.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

if(data->metkazapisi == 0) 
 {

  return(zap_vrest_dok(data->dd,data->md,data->gd,data->nomd.ravno(),
   data->skl,data->metkaz,data->kodm,data->ei.ravno(),data->kolih.ravno_atof(),
   data->cena,data->koment.ravno(),kod_operatora,data->podr,data->ps));

 }

if(data->metkazapisi == 1)
 {
  sprintf(strsql,"update Restdok1 set \
kolih=%.10g,\
ei='%s',\
koment='%s' \
where datd='%d-%d-%d' and nomd='%s' and skl=%d and mz=%d and kodz=%d and nz=%d",
  data->kolih.ravno_atof(),
  data->ei.ravno(),
  data->koment.ravno(),
  data->gd,data->md,data->dd,
  data->nomd.ravno(),
  data->skl,
  data->metkaz,
  data->kodm,
  data->nz);

//printf("restdok_v_zapis %s\n",strsql);

  if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
   return(1);
  
 }



return(0);



}
/************************/
/*Определение процента скидки*/
/*****************************/
float l_restdok_v_pr_sk(int kod_gr)
{
char strsql[500];
char bros[100];
memset(bros,'\0',sizeof(bros));

iceb_u_str imaf_nast("restnast.alx");
//iceb_imafn("restnast.alx",&imaf_nast);
sprintf(strsql,"Скидки на товары для группы %d",kod_gr);

if(iceb_poldan(strsql,bros,imaf_nast.ravno(),NULL) != 0)
 return(0.);

printf("l_restdok_v_pr_sk bros=%s\n",bros);

int kolpol=iceb_u_pole2(bros,';');

if(kolpol == 0)
 return(0.);
time_t vrem;
struct tm *bf;

time(&vrem);
bf=localtime(&vrem);
char vrem_sk[50];
char proc_sk[50]; 
short has=0;
short min=0;
short sek=0;
float procent=0.;

printf("%d:%d:%d  kolpok=%d\n",bf->tm_hour,bf->tm_min,bf->tm_sec,kolpol);

for(int i=0; i < kolpol; i++)
 {
  iceb_u_polen(bros,strsql,sizeof(strsql),i+1,';');
  if(strsql[0] == '\0')
   continue;
  iceb_u_polen(strsql,vrem_sk,sizeof(vrem_sk),1,',');

  
  iceb_u_rstime(&has,&min,&sek,vrem_sk);
  printf("%d:%d:%d\n",has,min,sek);

  if(iceb_u_sravtime(has,min,sek,bf->tm_hour,bf->tm_min,bf->tm_sec) < 0)
   continue;
   
  iceb_u_polen(strsql,proc_sk,sizeof(proc_sk),2,',');
  procent=atof(proc_sk);  
  break;  

 }
printf("procent=%f\n",procent);

return(procent);
}
