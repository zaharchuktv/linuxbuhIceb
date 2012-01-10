/*$Id: lzaktaxiv.c,v 1.44 2011-02-21 07:36:21 sasa Exp $*/
/*10.07.2007	10.10.2003	Белых А.И.	lzaktaxiv.c
Ввод новой записи
*/
#include        <time.h>
#include        <stdlib.h>
#include        <unistd.h>
#include "taxi.h"
#include "lzaktaxi.h"

enum
{
 FK2,
 FK4,
 FK10,
 KOL_F_KL
};

enum
 {
  E_TELEF,
  E_KODKL,
  E_ADRES_POD,
  E_ADRES_POEZ,
  E_KOMENT,
  E_SUMA,
  E_KOLIH_PAS,
  E_FIO,
  E_DATA_ZAK,
  E_VREM_ZAK,
  KOLENTER
 };


class lzaktaxiv_data
 {
  public:
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *entry[KOLENTER];
  GtkWidget *windowkodzav;
  GtkWidget *windowvoditel;
  GtkWidget *label_saldo; //Вывод сальдо
  short     kl_shift; //0-отжата 1-нажата  
  int       voz;
  int      metkavz; //0-запись не выбрана 1-выбрана
  int      metkarr; //0-только не завершенные заказы 1-все
  int       metkazapisi; //0-новая запись 1-корректировка
  time_t vremnz;       
  class zaktaxi_rek zap_zaktaxi;
      
  //реквизиты для корректировки строки
  class iceb_u_str vrem;
  class iceb_u_str datz;
  class iceb_u_str vremz;
  
  lzaktaxiv_data()
   {
    voz=0;
    kl_shift=0;
   }
  void read_rek()
   {

    zap_zaktaxi.telef.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_TELEF]))));
    zap_zaktaxi.kodk.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KODKL]))));
    zap_zaktaxi.fio.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_FIO]))));
    zap_zaktaxi.kolp.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOLIH_PAS]))));
    zap_zaktaxi.adreso.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_ADRES_POD]))));
    zap_zaktaxi.adresk.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_ADRES_POEZ]))));
    zap_zaktaxi.koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT]))));
    zap_zaktaxi.suma.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SUMA]))));
    zap_zaktaxi.datvz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_ZAK]))));
    zap_zaktaxi.vremvz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VREM_ZAK]))));


    zap_zaktaxi.kodzav.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(windowkodzav)->entry))));
    zap_zaktaxi.kv.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(windowvoditel)->entry))));


//   gtk_signal_emit_by_name(GTK_OBJECT(GTK_COMBO(windowkodzav)->entry),"activate");
//   gtk_signal_emit_by_name(GTK_OBJECT(GTK_COMBO(windowvoditel)->entry),"activate");

   }
  
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    zap_zaktaxi.clear_data();
   }
 };

void get_telef(GtkWidget *,class lzaktaxiv_data *);
void clear_menu(class lzaktaxiv_data *);
gboolean   vzaktaxi_key_press(GtkWidget *widget,GdkEventKey *event,class lzaktaxiv_data *v_data);
void       kodkl_get(GtkWidget *,class lzaktaxiv_data*);
void       voditel_get(GtkWidget*,class lzaktaxiv_data*);
void       vzaktaxi_knopka(GtkWidget *,class lzaktaxiv_data *);
int    zzaktaxi(class lzaktaxiv_data *v_data);
void  kodzav_get(GtkWidget *widget,class lzaktaxiv_data *v_data);

extern SQL_baza	bd;
extern char *name_system;

void   lzaktaxiv(class lzaktaxi_data *gl_data)
{
class lzaktaxiv_data v_data;
v_data.zap_zaktaxi.clear_data();

char    strsql[300];
SQL_str row;
short d,m,g;
time_t vrem;
struct tm *bf;
int i=0;

v_data.metkavz=gl_data->metkavz;
v_data.metkarr=gl_data->metkarr;
v_data.metkazapisi=gl_data->metkazapisi;
 

time(&v_data.vremnz);

//v_data->zap_zaktaxi.clear_data();

v_data.zap_zaktaxi.suma.new_plus("1");

if(gl_data->metkazapisi == 1)
 {
  if(gl_data->metkavz == 0)
   return;

  v_data.vrem.new_plus(gl_data->rowv[16]);
  v_data.datz.new_plus(gl_data->rowv[8]);
  v_data.vremz.new_plus(gl_data->rowv[9]);

  SQLCURSOR cur;

  if(gl_data->rowv[0][0] != '\0')
   {
    sprintf(strsql,"select naik from Taxikzz where kod=%s",gl_data->rowv[0]);
    if(iceb_sql_readkey(strsql,&row,&cur,gl_data->window) == 1)
     {
      sprintf(strsql,"%s %s",gl_data->rowv[0],row[0]);
      v_data.zap_zaktaxi.kodzav.new_plus(strsql);

     }
   }


  v_data.zap_zaktaxi.kodk.new_plus(gl_data->rowv[1]);
  if(gl_data->rowv[2][0] != '\0')
   {
    sprintf(strsql,"select fio,gosn from Taxivod where kod='%s'",gl_data->rowv[2]);
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
     {    
      sprintf(strsql,"%s %s %s",gl_data->rowv[2],row[0],row[1]);
      v_data.zap_zaktaxi.kv.new_plus(strsql);
     }
   }


  v_data.zap_zaktaxi.fio.new_plus(gl_data->rowv[3]);
  v_data.zap_zaktaxi.telef.new_plus(gl_data->rowv[4]);
  v_data.zap_zaktaxi.kolp.new_plus(gl_data->rowv[5]);

  v_data.zap_zaktaxi.datvz.new_plus(iceb_u_datzap(gl_data->rowv[6]));

  v_data.zap_zaktaxi.vremvz.new_plus(gl_data->rowv[7]);
  v_data.zap_zaktaxi.adreso.new_plus(gl_data->rowv[10]);
  v_data.zap_zaktaxi.adresk.new_plus(gl_data->rowv[11]);
  v_data.zap_zaktaxi.koment.new_plus(gl_data->rowv[12]);
  v_data.zap_zaktaxi.suma.new_plus(gl_data->rowv[13]);
  
 }

v_data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
//gtk_window_set_position( GTK_WINDOW(v_data.window),ICEB_POS_CENTER);
//gtk_widget_set_usize(GTK_WIDGET(v_data.window), 600,180);
//if(v_data.metkazapisi == 1)

gdk_window_set_cursor(gl_data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
//Удерживать окно над породившем его окном всегда
gtk_window_set_transient_for(GTK_WINDOW(v_data.window),GTK_WINDOW(gl_data->window));

gtk_window_set_modal( GTK_WINDOW(v_data.window) ,TRUE ); 

if(gl_data->metkazapisi == 0)
  sprintf(strsql,"%s %s",NAME_SYSTEM,iceb_u_toutf("Ввод новой записи"));
if(gl_data->metkazapisi == 1)
  sprintf(strsql,"%s %s",NAME_SYSTEM,iceb_u_toutf("Корректировка записи"));
gtk_window_set_title(GTK_WINDOW(v_data.window),strsql);

gtk_signal_connect(GTK_OBJECT(v_data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(v_data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(v_data.window),"key_press_event",GTK_SIGNAL_FUNC(vzaktaxi_key_press),&v_data);
//gtk_signal_connect_after(GTK_OBJECT(v_data.window),"key_release_event",GTK_SIGNAL_FUNC(obr_klav_plansh_o),gl_data);

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox1 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox2 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox3 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox4 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox5 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox6 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox7 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox8 = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (v_data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), hbox1); //Адрес подачи
gtk_container_add (GTK_CONTAINER (vbox), hbox2); //Адрес поездки
gtk_container_add (GTK_CONTAINER (vbox), hbox3); //Код водителя
gtk_container_add (GTK_CONTAINER (vbox), hbox5); //Код завершения
gtk_container_add (GTK_CONTAINER (vbox), hbox); //Телефон, код клиента
gtk_container_add (GTK_CONTAINER (vbox), hbox4); //Коментарий
gtk_container_add (GTK_CONTAINER (vbox), hbox6); //Сумма, количество пассажиров
gtk_container_add (GTK_CONTAINER (vbox), hbox7); //Фамилия клиента
gtk_container_add (GTK_CONTAINER (vbox), hbox8); //Дата , время выполнения заказа
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


GtkWidget *label=gtk_label_new(iceb_u_toutf("Телефон"));
v_data.entry[E_TELEF] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), v_data.entry[E_TELEF], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (v_data.entry[E_TELEF]), "activate",GTK_SIGNAL_FUNC(get_telef),&v_data);
gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_TELEF]),v_data.zap_zaktaxi.telef.ravno_toutf());
//gtk_widget_set_usize(v_data.entry[E_TELEF],100,20);

label=gtk_label_new(iceb_u_toutf("Код клиента"));
v_data.entry[E_KODKL] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), v_data.entry[E_KODKL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (v_data.entry[E_KODKL]), "activate",GTK_SIGNAL_FUNC(kodkl_get),&v_data);
//gtk_widget_set_usize(v_data.entry[E_KODKL],60,20);
gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_KODKL]),v_data.zap_zaktaxi.kodk.ravno_toutf());


label=gtk_label_new(iceb_u_toutf("Адрес подачи"));
v_data.entry[E_ADRES_POD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), v_data.entry[E_ADRES_POD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (v_data.entry[E_ADRES_POD]), "activate",GTK_SIGNAL_FUNC(iceb_get_text_str),&v_data.zap_zaktaxi.adreso);
//gtk_widget_set_usize(v_data.entry[E_ADRES_POD],200,20);
gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_ADRES_POD]),v_data.zap_zaktaxi.adreso.ravno_toutf());


label=gtk_label_new(iceb_u_toutf("Адрес поездки"));
v_data.entry[E_ADRES_POEZ] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), v_data.entry[E_ADRES_POEZ], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (v_data.entry[E_ADRES_POEZ]), "activate",GTK_SIGNAL_FUNC(iceb_get_text_str),&v_data.zap_zaktaxi.adresk);
//gtk_widget_set_usize(v_data.entry[E_ADRES_POEZ],200,20);
gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_ADRES_POEZ]),v_data.zap_zaktaxi.adresk.ravno_toutf());



label=gtk_label_new(iceb_u_toutf("Код водителя"));
gtk_box_pack_start (GTK_BOX (hbox3), label, FALSE, FALSE, 0);

//Создаем выбор водителя
GList *glist=NULL;

SQLCURSOR cur;
sprintf(strsql,"select kod,fio,gosn from Taxivod where sm=0 order by fio asc");
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);

iceb_u_spisok VOD;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s %s %s",row[0],row[1],row[2]);
  VOD.plus(iceb_u_toutf(strsql));
 }
if(kolstr == 0)
 VOD.plus(iceb_u_toutf("Не введен список водителей !!!"));

v_data.windowvoditel=gtk_combo_new();


for(i=0 ; i < kolstr; i++)
  glist=g_list_append(glist,(void*)VOD.ravno(i));

gtk_combo_set_popdown_strings(GTK_COMBO(v_data.windowvoditel),glist);
gtk_combo_disable_activate(GTK_COMBO(v_data.windowvoditel));
gtk_box_pack_start (GTK_BOX (hbox3), v_data.windowvoditel, TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (GTK_COMBO(v_data.windowvoditel)->entry), "activate",GTK_SIGNAL_FUNC(voditel_get),&v_data);
gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(v_data.windowvoditel)->entry),v_data.zap_zaktaxi.kv.ravno_toutf());

label=gtk_label_new(iceb_u_toutf("Коментарий"));
v_data.entry[E_KOMENT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox4), v_data.entry[E_KOMENT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (v_data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(iceb_get_text_str),&v_data.zap_zaktaxi.koment);
//gtk_widget_set_usize(v_data.entry[E_KOMENT],200,20);
gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_KOMENT]),v_data.zap_zaktaxi.koment.ravno_toutf());


label=gtk_label_new(iceb_u_toutf("Код завершения"));
gtk_box_pack_start (GTK_BOX (hbox5), label, FALSE, FALSE, 0);
//Создаем меню кодов завершения

glist=NULL;

sprintf(strsql,"select kod,naik from Taxikzz order by kod asc");
kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);

iceb_u_spisok KZV;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s %s",row[0],row[1]);
  KZV.plus(iceb_u_toutf(strsql));
 }
if(kolstr == 0)
  KZV.plus(iceb_u_toutf("Не введен список кодов завершения !!!"));

v_data.windowkodzav=gtk_combo_new();


for(i=0 ; i < kolstr; i++)
  glist=g_list_append(glist,(void*)KZV.ravno(i));

gtk_combo_set_popdown_strings(GTK_COMBO(v_data.windowkodzav),glist);
gtk_combo_disable_activate(GTK_COMBO(v_data.windowkodzav));
gtk_box_pack_start (GTK_BOX (hbox5), v_data.windowkodzav, TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (GTK_COMBO(v_data.windowkodzav)->entry), "activate",GTK_SIGNAL_FUNC(kodzav_get),&v_data);
//Если не выводить то в меню будет первая строка из списка
//if(gl_data->metkazapisi == 1)
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(v_data.windowkodzav)->entry),v_data.zap_zaktaxi.kodzav.ravno_toutf());


label=gtk_label_new(iceb_u_toutf("Сумма"));
v_data.entry[E_SUMA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox6), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox6), v_data.entry[E_SUMA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (v_data.entry[E_SUMA]), "activate",GTK_SIGNAL_FUNC(iceb_get_text_str),&v_data.zap_zaktaxi.suma);
//gtk_widget_set_usize(v_data.entry[E_SUMA],40,20);
gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_SUMA]),v_data.zap_zaktaxi.suma.ravno_toutf());

double suma=0.;

if(v_data.zap_zaktaxi.kodk.getdlinna() > 1)
 suma=taxi_saldo(v_data.zap_zaktaxi.kodk.ravno());

sprintf(strsql,"Сальдо: %.2f ",suma);
v_data.label_saldo=gtk_label_new(iceb_u_toutf(strsql));

gtk_box_pack_start (GTK_BOX (hbox6), v_data.label_saldo, FALSE, FALSE, 0);

label=gtk_label_new(iceb_u_toutf("Количество пассажиров"));
v_data.entry[E_KOLIH_PAS] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox6), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox6), v_data.entry[E_KOLIH_PAS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (v_data.entry[E_KOLIH_PAS]), "activate",GTK_SIGNAL_FUNC(iceb_get_text_str),&v_data.zap_zaktaxi.kolp);
//gtk_widget_set_usize(v_data.entry[E_KOLIH_PAS],40,20);
gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_KOLIH_PAS]),v_data.zap_zaktaxi.kolp.ravno_toutf());


/********************************/

label=gtk_label_new(iceb_u_toutf("ФИО"));
v_data.entry[E_FIO] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox7), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox7), v_data.entry[E_FIO], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (v_data.entry[E_FIO]), "activate",GTK_SIGNAL_FUNC(iceb_get_text_str),&v_data.zap_zaktaxi.fio);
gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_FIO]),v_data.zap_zaktaxi.fio.ravno_toutf());
//gtk_widget_set_usize(v_data.entry[E_FIO],200,20);


label=gtk_label_new(iceb_u_toutf("Дата вып.заказа"));
v_data.entry[E_DATA_ZAK] = gtk_entry_new_with_max_length (11);
gtk_box_pack_start (GTK_BOX (hbox8), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox8), v_data.entry[E_DATA_ZAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (v_data.entry[E_DATA_ZAK]), "activate",GTK_SIGNAL_FUNC(iceb_get_text_str),&v_data.zap_zaktaxi.datvz);
time(&vrem);
bf=localtime(&vrem);  

if(gl_data->metkazapisi == 1)
 {
  gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_DATA_ZAK]),v_data.zap_zaktaxi.datvz.ravno());
//  strcpy(strsql,v_data.zap_zaktaxi.datvz.ravno());
 }
/******
if(gl_data->metkazapisi == 0)
 {
  sprintf(strsql,"%02d.%02d.%04d",bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);
  v_data.zap_zaktaxi.datvz.new_plus(strsql);
 }
*********/
//gtk_widget_set_usize(v_data.entry[E_DATA_ZAK],68,20);
//gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_DATA_ZAK]),strsql);


label=gtk_label_new(iceb_u_toutf("Время вып.заказа"));
v_data.entry[E_VREM_ZAK] = gtk_entry_new_with_max_length (9);
gtk_box_pack_start (GTK_BOX (hbox8), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox8), v_data.entry[E_VREM_ZAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (v_data.entry[E_VREM_ZAK]), "activate",GTK_SIGNAL_FUNC(iceb_get_text_str),&v_data.zap_zaktaxi.vremvz);

if(gl_data->metkazapisi == 1)
 {
  iceb_u_rstime(&d,&m,&g,v_data.zap_zaktaxi.vremvz.ravno());
  if(d != 0)
   sprintf(strsql,"%02d:%02d",d,m);
   gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_VREM_ZAK]),strsql);
 }
/*************
if(gl_data->metkazapisi == 0)
 {
  sprintf(strsql,"%02d:%02d",bf->tm_hour,bf->tm_min);
  v_data.zap_zaktaxi.vremvz.new_plus(strsql);
 }

//gtk_widget_set_usize(v_data.entry[E_VREM_ZAK],68,20);
gtk_entry_set_text(GTK_ENTRY(v_data.entry[E_VREM_ZAK]),strsql);

**************/  

sprintf(strsql,"F2 %s",gettext("Запись"));
v_data.knopka[FK2]=gtk_button_new_with_label(strsql);
GtkTooltips *tooltips0=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips0,v_data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(v_data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(vzaktaxi_knopka),&v_data);
gtk_object_set_user_data(GTK_OBJECT(v_data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), v_data.knopka[FK2], TRUE, TRUE, 0);

if(gl_data->metkazapisi == 0)
 {
  sprintf(strsql,"F4 %s",gettext("Очистить"));
  v_data.knopka[FK4]=gtk_button_new_with_label(strsql);
//  GtkTooltips *tooltips2=gtk_tooltips_new();
  gtk_tooltips_set_tip(tooltips0,v_data.knopka[FK4],gettext("Очистить меню от введеноой информации. Установить текущее время и дату"),NULL);
  gtk_signal_connect(GTK_OBJECT(v_data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(vzaktaxi_knopka),&v_data);
  gtk_object_set_user_data(GTK_OBJECT(v_data.knopka[FK4]),(gpointer)FK4);
  gtk_box_pack_start(GTK_BOX(hboxknop), v_data.knopka[FK4], TRUE, TRUE, 0);
 }

sprintf(strsql,"F10 %s",gettext("Выход"));
v_data.knopka[FK10]=gtk_button_new_with_label(strsql);
GtkTooltips *tooltips1=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips1,v_data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(v_data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(vzaktaxi_knopka),&v_data);
gtk_object_set_user_data(GTK_OBJECT(v_data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), v_data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(v_data.entry[E_ADRES_POD]);

if(gl_data->metkazapisi == 0)
  clear_menu(&v_data);
gtk_widget_show_all (v_data.window);

gtk_main();

gdk_window_set_cursor(gl_data->window->window,gdk_cursor_new(ICEB_CURSOR));

}
/*************************/
/*Обработчик кода клиента*/
/**************************/
void  kodkl_get(GtkWidget *widget,class lzaktaxiv_data *v_data)
{
char kod[300];
char strsql[300];
SQL_str row;

//printf("kodkl_get\n");

strcpy(kod,iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

  v_data->zap_zaktaxi.kodk.new_plus(kod);
  if(v_data->zap_zaktaxi.kodk.getdlinna() <= 1)
   return;


sprintf(strsql,"select fio,adres,telef from Taxiklient where kod='%s'",kod);
//printf("%s\n",strsql);
SQLCURSOR cur;
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"Нет клиента с кодом %s !!!",kod);
  repl.plus(strsql);
  iceb_menu_soob(&repl,NULL);
  return; 
 }


v_data->zap_zaktaxi.fio.new_plus(row[0]);

v_data->zap_zaktaxi.adreso.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(v_data->entry[E_ADRES_POD]))));

if(v_data->zap_zaktaxi.adreso.getdlinna() <= 1)
  v_data->zap_zaktaxi.adreso.new_plus(row[1]);

v_data->zap_zaktaxi.telef.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(v_data->entry[E_TELEF]))));

if(v_data->zap_zaktaxi.telef.getdlinna() <= 1)
  v_data->zap_zaktaxi.telef.new_plus(row[2]);

  iceb_u_str stroka;

  stroka.plus(iceb_u_toutf(v_data->zap_zaktaxi.fio.ravno()));
  gtk_entry_set_text(GTK_ENTRY(v_data->entry[E_FIO]),stroka.ravno());

  stroka.new_plus(iceb_u_toutf(v_data->zap_zaktaxi.adreso.ravno()));
  gtk_entry_set_text(GTK_ENTRY(v_data->entry[E_ADRES_POD]),stroka.ravno());

  stroka.new_plus(iceb_u_toutf(v_data->zap_zaktaxi.telef.ravno()));
  gtk_entry_set_text(GTK_ENTRY(v_data->entry[E_TELEF]),stroka.ravno());


  double suma=taxi_saldo(v_data->zap_zaktaxi.kodk.ravno());
  char bros[50];
  sprintf(bros,"Сальдо: %.2f ",suma);
  stroka.new_plus(iceb_u_toutf(bros));
  gtk_label_set_text(GTK_LABEL(v_data->label_saldo),stroka.ravno());

gtk_entry_select_region(GTK_ENTRY(widget),0,strlen(kod));

}

/*************************************/
/*Обработчик сигнала выбора водителя*/
/************************************/
void  voditel_get(GtkWidget *widget,class lzaktaxiv_data *v_data)
{
char bros[300];
char strsql[300];
SQL_str row;
char kod[100];

//printf("voditel_get\n");

strcpy(kod,iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

  v_data->zap_zaktaxi.kv.new_plus(kod);
  if(v_data->zap_zaktaxi.kv.getdlinna() <= 1)
   return;


if(kod[0] == '\0')
 return;

memset(bros,'\0',sizeof(bros));
if(iceb_u_polen(kod,bros,sizeof(bros),1,' ') != 0)
  strncpy(bros,kod,sizeof(bros)-1);

sprintf(strsql,"select fio,gosn,sm from Taxivod where kod='%s'",bros);
//printf("%s\n",strsql);
SQLCURSOR cur;
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"Нет водителя с кодом %s !!!",bros);
  repl.plus(strsql);
  iceb_menu_soob(&repl,NULL);
  return; 
 }

sprintf(kod,"%s %s %s",bros,row[0],row[1]);

if(atoi(row[2]) != 0)
 {
  iceb_u_str repl;
  repl.plus("Этого водителя нет на смене !!!");
  repl.ps_plus(kod);
  iceb_menu_soob(&repl,NULL);
  gtk_entry_set_text(GTK_ENTRY(widget),"");
  return; 

 }
 
gtk_entry_set_text(GTK_ENTRY(widget),iceb_u_toutf(kod));
gtk_entry_select_region(GTK_ENTRY(widget),0,strlen(kod));

}

/*************************************/
/*Обработчик сигнала выбора кода завершения*/
/************************************/
void  kodzav_get(GtkWidget *widget,class lzaktaxiv_data *v_data)
{
char bros[300];
char strsql[300];
SQL_str row;
SQLCURSOR cur;
char  kod[100];

//printf("kodzav_get**********************************\n");

strcpy(kod,iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

  v_data->zap_zaktaxi.kodzav.new_plus(kod);
  if(v_data->zap_zaktaxi.kodzav.getdlinna() <= 1)
   return;


if(kod[0] == '\0')
 return;

memset(bros,'\0',sizeof(bros));
if(iceb_u_polen(kod,bros,sizeof(bros),1,' ') != 0)
  strncpy(bros,kod,sizeof(bros)-1);

sprintf(strsql,"select naik from Taxikzz where kod=%s",bros);
//printf("%s\n",strsql);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"Нет такого кода завершения !!!");
  repl.plus(strsql);
  iceb_menu_soob(&repl,NULL);
  return; 
 }

sprintf(kod,"%s %s",bros,row[0]);

gtk_entry_set_text(GTK_ENTRY(widget),iceb_u_toutf(kod));
gtk_entry_select_region(GTK_ENTRY(widget),0,strlen(kod));

}


/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  vzaktaxi_knopka(GtkWidget *widget,class lzaktaxiv_data *v_data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//printf("vzaktaxi_knopka knop=%s\n",knop);
switch ((gint)knop)
 {
  case FK2:
    if(zzaktaxi(v_data) == 0)
     {
      if(v_data->metkazapisi == 1)
       {
        gtk_widget_destroy(v_data->window);
       }
      else
       clear_menu(v_data);
     }
    return;  

  case FK4:
   clear_menu(v_data);
   return;
   
  case FK10:
    gtk_widget_destroy(v_data->window);
    return;
 }
}
 
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vzaktaxi_key_press(GtkWidget *widget,GdkEventKey *event,class lzaktaxiv_data *v_data)
{
//char  bros[300];

switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(v_data->knopka[FK2]),"clicked");
    return(TRUE);
    
  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(v_data->knopka[FK4]),"clicked");
    return(TRUE);

  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(v_data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

    v_data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}


/**************************/
/*Запись строки в таблицу*/
/**************************/
//Если вернули 0-записали 1-нет
int    zzaktaxi(class lzaktaxiv_data *v_data)
{
char bros[300];
char strsql[2000];
time_t   vrem;
struct tm *bf;
SQL_str row;
SQLCURSOR cur;

/***********
for(int i=0; i < KOLENTRY-1; i++)
 {
   gtk_signal_emit_by_name(GTK_OBJECT(v_data->entry[i]),"activate");
 }
gtk_signal_emit_by_name(GTK_OBJECT(GTK_COMBO(v_data->windowkodzav)->entry),"activate");
gtk_signal_emit_by_name(GTK_OBJECT(GTK_COMBO(v_data->windowvoditel)->entry),"activate");

***************/
v_data->read_rek();

if(v_data->zap_zaktaxi.datvz.getdlinna() <= 1)
 {
  iceb_menu_soob("Не введена дата выполнения заказа !",v_data->window);
  return(1);
 }

if(v_data->zap_zaktaxi.vremvz.getdlinna() <= 1)
 {
  iceb_menu_soob("Не введено время выполнения заказа !",v_data->window);
  return(1);
 }

 
short  gvz,mvz,dvz;
short  hvz,mnvz,svz;

if(iceb_u_rsdat(&dvz,&mvz,&gvz,v_data->zap_zaktaxi.datvz.ravno(),1) != 0)
 {
  iceb_u_str sp;
  sp.plus_ps("Не верно введена дата выполнения заказа !");
  sp.plus(v_data->zap_zaktaxi.datvz.ravno());
  iceb_menu_soob(&sp,v_data->window);
  return(1);
 }

if(iceb_u_rstime(&hvz,&mnvz,&svz,v_data->zap_zaktaxi.vremvz.ravno()) != 0)
 {
  iceb_menu_soob("Не верно введено время выполнения заказа !",v_data->window);
  return(1);
 }

if(v_data->zap_zaktaxi.kodk.getdlinna() > 1)
 {

  sprintf(strsql,"select fio,adres,telef from Taxiklient where kod='%s'",v_data->zap_zaktaxi.kodk.ravno());
  //printf("%s\n",strsql);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"Нет клиента с кодом %s !!!",v_data->zap_zaktaxi.kodk.ravno());
    iceb_menu_soob(strsql,v_data->window);
    return(1); 
   }
 }
 
if(v_data->zap_zaktaxi.kv.getdlinna() > 1)
 {
  memset(bros,'\0',sizeof(bros));
  if(iceb_u_polen(v_data->zap_zaktaxi.kv.ravno(),bros,sizeof(bros),1,' ') != 0)
    strncpy(bros,v_data->zap_zaktaxi.kv.ravno(),sizeof(bros)-1);

  sprintf(strsql,"select fio,sm from Taxivod where kod='%s'",bros);
  //printf("%s\n",strsql);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    iceb_u_str repl;
    sprintf(strsql,"Нет водителя с кодом %s !!!",bros);
    repl.plus(strsql);
    iceb_menu_soob(&repl,v_data->window);
    return(1); 
   }

  if(v_data->metkazapisi == 0)
   if(atoi(row[1]) != 0)
    {
     iceb_u_str repl;
     repl.plus("Этого водителя нет на смене !!!");
     repl.ps_plus(bros);
     repl.plus(" ");
     repl.plus(row[0]);
     iceb_menu_soob(&repl,v_data->window);
     return(1); 
    }

  v_data->zap_zaktaxi.kv.new_plus(bros);
 }

if(v_data->zap_zaktaxi.kodzav.getdlinna() > 1)
 {
  memset(bros,'\0',sizeof(bros));
  if(iceb_u_polen(v_data->zap_zaktaxi.kodzav.ravno(),bros,sizeof(bros),1,' ') != 0)
    strncpy(bros,v_data->zap_zaktaxi.kodzav.ravno(),sizeof(bros)-1);

  sprintf(strsql,"select naik from Taxikzz where kod=%s",bros);
  //printf("%s\n",strsql);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"Нет кода завершения %s !!!",bros);
    iceb_menu_soob(strsql,v_data->window);
    return(1); 
   }
  v_data->zap_zaktaxi.kodzav.new_plus(bros);
 }




time(&vrem);
bf=localtime(&vrem);
if(v_data->metkazapisi == 0)
 {
  bf=localtime(&v_data->vremnz);
  
  sprintf(strsql,"insert into Taxizak \
values (%d,'%s','%s','%s','%s',%d,\
'%04d-%02d-%02d','%02d:%02d:%02d',\
'%04d-%02d-%02d','%02d:%02d:%02d',\
'%s','%s','%s',%.2f,%d,%d,%ld,'%s')",\
  atoi(v_data->zap_zaktaxi.kodzav.ravno()),\
  v_data->zap_zaktaxi.kodk.ravno(),\
  v_data->zap_zaktaxi.kv.ravno(),\
  v_data->zap_zaktaxi.fio.ravno(),\
  v_data->zap_zaktaxi.telef.ravno(),\
  atoi(v_data->zap_zaktaxi.kolp.ravno()),\
  gvz,mvz,dvz,\
  hvz,mnvz,svz,\
  bf->tm_year+1900,\
  bf->tm_mon+1,\
  bf->tm_mday,\
  bf->tm_hour,
  bf->tm_min,
  bf->tm_sec,
  v_data->zap_zaktaxi.adreso.ravno(),\
  v_data->zap_zaktaxi.adresk.ravno(),\
  v_data->zap_zaktaxi.koment.ravno(),\
  atof(v_data->zap_zaktaxi.suma.ravno()),\
  0,\
  iceb_getuid(v_data->window),\
  vrem,\
  "");  
  
 }
else
 {

  sprintf(strsql,"update Taxizak \
set \
kz=%d,\
kodk='%s',\
kv='%s',\
fio='%s',\
telef='%s',\
kolp='%d',\
datvz='%04d-%02d-%02d',\
vremvz='%02d:%02d:%02d',\
adreso='%s',\
adresk='%s',\
koment='%s',\
suma=%.2f,\
ktoi=%d,\
vrem=%ld \
where \
vrem=%s and \
datz='%s' and \
vremz='%s'",\
  atoi(v_data->zap_zaktaxi.kodzav.ravno()),\
  v_data->zap_zaktaxi.kodk.ravno(),\
  v_data->zap_zaktaxi.kv.ravno(),\
  v_data->zap_zaktaxi.fio.ravno(),\
  v_data->zap_zaktaxi.telef.ravno(),\
  atoi(v_data->zap_zaktaxi.kolp.ravno()),\
  gvz,mvz,dvz,\
  hvz,mnvz,svz,\
  v_data->zap_zaktaxi.adreso.ravno(),\
  v_data->zap_zaktaxi.adresk.ravno(),\
  v_data->zap_zaktaxi.koment.ravno(),\
  atof(v_data->zap_zaktaxi.suma.ravno()),\
  iceb_getuid(v_data->window),\
  vrem,\
  v_data->vrem.ravno(),\
  v_data->datz.ravno(),\
  v_data->vremz.ravno());

 }

//printf("zzaktaxi-%s\n",strsql);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
  {
   iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),v_data->window);
   return(1);
  }
 else
  {
   iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,v_data->window);
   return(1);
  }
 }
//Проверяем есть ли телефон в списке телефонов
if(v_data->zap_zaktaxi.telef.getdlinna() > 1 && v_data->zap_zaktaxi.telef.ravno()[0] != '8')
 {
  sprintf(strsql,"select tel from Taxitel where tel='%s'",v_data->zap_zaktaxi.telef.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) == 0)
   {
    sprintf(strsql,"insert into Taxitel \
values ('%s','%s',%d,%ld)",v_data->zap_zaktaxi.telef.ravno(),v_data->zap_zaktaxi.adreso.ravno(),iceb_getuid(v_data->window),vrem);
    if(sql_zap(&bd,strsql) != 0)
       iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,v_data->window);
   }
 }


if(v_data->metkazapisi == 1)
  gtk_widget_hide(v_data->window); //Скрываем от показа виджет

extern class lzaktaxi_data data;
zapzaktaxi(&data);
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

return(0);
}
/**************************************/
/*Проверка телефона в списке телефонов*/
/**************************************/

void get_telef(GtkWidget *widget,class lzaktaxiv_data *v_data)
{

//char bros[300];
char strsql[300];
SQL_str row;
char  kod[100];

//printf("kodzav_get\n");

strcpy(kod,iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

v_data->zap_zaktaxi.telef.new_plus(kod);

if(kod[0] == '\0')
 return;



sprintf(strsql,"select adres from Taxitel where tel='%s'",kod);
//printf("%s\n",strsql);
SQLCURSOR cur;
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  v_data->zap_zaktaxi.adreso.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(v_data->entry[E_ADRES_POD]))));

  if(v_data->zap_zaktaxi.adreso.getdlinna() <= 1)
   gtk_entry_set_text(GTK_ENTRY(v_data->entry[E_ADRES_POD]),iceb_u_toutf(row[0]));
 }

gtk_entry_set_text(GTK_ENTRY(widget),iceb_u_toutf(kod));
//gtk_entry_select_region(GTK_ENTRY(widget),0,strlen(kod));

}
/************************/
/*Очистка меню и запись нового времени*/
/**************************************/

void clear_menu(class lzaktaxiv_data *v_data)
{

time(&v_data->vremnz);
memset(&v_data->zap_zaktaxi,'\0',sizeof(v_data->zap_zaktaxi));

for(int i=0; i < KOLENTER; i++)
 gtk_entry_set_text(GTK_ENTRY(v_data->entry[i]),"");

//Код водителя
gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(v_data->windowvoditel)->entry),"");
//Код завершения
gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(v_data->windowkodzav)->entry),"");

struct tm *bf;

bf=localtime(&v_data->vremnz);  
char bros[100];
sprintf(bros,"%02d.%02d.%04d",bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);
v_data->zap_zaktaxi.datvz.new_plus(bros);

gtk_entry_set_text(GTK_ENTRY(v_data->entry[E_DATA_ZAK]),v_data->zap_zaktaxi.datvz.ravno());

sprintf(bros,"%02d:%02d:%02d",bf->tm_hour,bf->tm_min,bf->tm_sec);
v_data->zap_zaktaxi.vremvz.new_plus(bros);
gtk_entry_set_text(GTK_ENTRY(v_data->entry[E_VREM_ZAK]),v_data->zap_zaktaxi.vremvz.ravno());

v_data->zap_zaktaxi.suma.new_plus("1");
gtk_entry_set_text(GTK_ENTRY(v_data->entry[E_SUMA]),v_data->zap_zaktaxi.suma.ravno());

gtk_widget_grab_focus(v_data->entry[E_ADRES_POD]);
//gtk_widget_grab_focus(v_data->entry[E_TELEF]);


}
