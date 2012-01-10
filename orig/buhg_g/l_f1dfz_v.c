/*$Id: l_f1dfz_v.c,v 1.16 2011-02-21 07:35:52 sasa Exp $*/
/*11.01.2007	22.12.2006	Белых А.И.	l_f1dfz_v.c
Ввод и корректировка записи по работнику предприятия в документ формы 1ДФ
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
enum
 {
  E_TABNOM,
  E_INN,
  E_VIP_DOH,
  E_VIP_NAL,
  E_NAH_DOH,
  E_NAH_NAL,
  E_OZ_DOH,
  E_DATA_PNR,
  E_DATA_USR,
  E_LGOTA,
  E_PRIZNAK,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_f1dfz_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *label_fio;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет
  
  //Уникальные реквизиты строки
  class iceb_u_str nomdok;
  class iceb_u_str inn_k;
  class iceb_u_str oz_doh_k;  
  class iceb_u_str lgota_k;  
  class iceb_u_str pr_k;  
  
  //Реквизиты меню
  class iceb_u_str tabnom;
  class iceb_u_str inn;
  class iceb_u_str vip_doh;
  class iceb_u_str vip_nal;
  class iceb_u_str nah_doh;
  class iceb_u_str nah_nal;
  class iceb_u_str oz_doh;
  class iceb_u_str data_pnr;
  class iceb_u_str data_usr;
  class iceb_u_str lgota;
  class iceb_u_str pr;
        
  class iceb_u_str fio;
  short viddok; 
    
  l_f1dfz_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_rek();
   }

  void read_rek()
   {
    tabnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM]))));
    inn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_INN]))));
    vip_doh.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VIP_DOH]))));
    vip_nal.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VIP_NAL]))));
    nah_doh.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NAH_DOH]))));
    nah_nal.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NAH_NAL]))));
    oz_doh.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_OZ_DOH]))));
    data_pnr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_PNR]))));
    data_usr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_USR]))));
    lgota.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_LGOTA]))));
    pr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PRIZNAK]))));
   }
  void clear_rek()
   {
    tabnom.new_plus("");
    inn.new_plus("");
    vip_doh.new_plus("");
    vip_nal.new_plus("");
    nah_doh.new_plus("");
    nah_nal.new_plus("");
    oz_doh.new_plus("");
    data_pnr.new_plus("");
    data_usr.new_plus("");
    lgota.new_plus("");
    pr.new_plus("");
    fio.new_plus("");
   }
 };

gboolean   l_f1dfz_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_f1dfz_v_data *data);
void  l_f1dfz_v_knopka(GtkWidget *widget,class l_f1dfz_v_data *data);
void    l_f1dfz_v_vvod(GtkWidget *widget,class l_f1dfz_v_data *data);
int l_f1dfz_zap(class l_f1dfz_v_data *data);
void  l_f1dfz_v_e_knopka(GtkWidget *widget,class l_f1dfz_v_data *data);

extern SQL_baza  bd;
extern char      *name_system;

int l_f1dfz_v(const char *nomdok,
class iceb_u_str *inn,
class iceb_u_str *priz,
class iceb_u_str *lgota,
class iceb_u_str *pr,
GtkWidget *wpredok)
{
SQL_str row;
SQLCURSOR cur;

class l_f1dfz_v_data data;
char strsql[512];
iceb_u_str kikz;
data.nomdok.new_plus(nomdok);
data.inn_k.new_plus(inn->ravno());
data.oz_doh_k.new_plus(priz->ravno());
data.lgota_k.new_plus(lgota->ravno());
data.pr_k.new_plus(pr->ravno());

data.viddok=0;  //0-отчётный  1-новый отчётный 2-уточняющий
//читаем вид документа
sprintf(strsql,"select vidd from F8dr where nomd='%s'",data.nomdok.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 data.viddok=atoi(row[0]);
if(data.inn_k.getdlinna() >  1)
 {
  sprintf(strsql,"select sumad,suman,datap,datau,ktoz,vrem,sumadn,sumann from F8dr1 where nomd='%s'\
and inn='%s' and priz='%s' and lgota='%s' and pr=%d",
  data.nomdok.ravno(),
  data.inn_k.ravno(),
  data.oz_doh_k.ravno(),
  data.lgota_k.ravno(),
  data.pr_k.ravno_atoi());
  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  data.inn.new_plus(data.inn_k.ravno());
  data.pr.new_plus(data.pr_k.ravno());  
  data.oz_doh.new_plus(data.oz_doh_k.ravno());
  
  data.vip_doh.new_plus(row[0]);
  data.vip_nal.new_plus(row[1]);
  if(row[2][0] != '0')
   data.data_pnr.new_plus(iceb_u_sqldata(row[2]));
  if(row[3][0] != '0')
   data.data_usr.new_plus(iceb_u_sqldata(row[3]));
  data.nah_doh.new_plus(row[6]);
  data.nah_nal.new_plus(row[7]);  
  kikz.plus(iceb_kikz(row[4],row[5],wpredok));
  
  
  //Читаем фамилию
  sprintf(strsql,"select tabn,fio from Kartb where inn='%s'",data.inn.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    data.tabnom.new_plus(row[0]);  
    data.fio.new_plus(row[1]);  
   }
 }
else
 {
  if(data.viddok == 0)
   data.pr.new_plus("0");
 }
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.inn_k.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));
  label=gtk_label_new(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));
  iceb_u_str repl;
  repl.plus(gettext("Корректировка записи"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno_toutf());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_f1dfz_v_key_press),&data);

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

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
data.label_fio=gtk_label_new(data.fio.ravno_toutf());
gtk_container_add (GTK_CONTAINER (vbox), data.label_fio);

for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Табельный номер"));
data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_TABNOM]),"clicked",GTK_SIGNAL_FUNC(l_f1dfz_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_TABNOM]),(gpointer)E_TABNOM);
tooltips_enter[E_TABNOM]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_TABNOM],data.knopka_enter[E_TABNOM],gettext("Выбор табельного номера"),NULL);

data.entry[E_TABNOM] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TABNOM]), "activate",GTK_SIGNAL_FUNC(l_f1dfz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.tabnom.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TABNOM]),(gpointer)E_TABNOM);



label=gtk_label_new(gettext("Инд. нал. номер"));
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), label, FALSE, FALSE, 0);

data.entry[E_INN] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), data.entry[E_INN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_INN]), "activate",GTK_SIGNAL_FUNC(l_f1dfz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INN]),data.inn.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_INN]),(gpointer)E_INN);


label=gtk_label_new(gettext("Вып-ный доход"));
gtk_box_pack_start (GTK_BOX (hbox[E_VIP_DOH]), label, FALSE, FALSE, 0);

data.entry[E_VIP_DOH] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_VIP_DOH]), data.entry[E_VIP_DOH], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VIP_DOH]), "activate",GTK_SIGNAL_FUNC(l_f1dfz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VIP_DOH]),data.vip_doh.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VIP_DOH]),(gpointer)E_VIP_DOH);

label=gtk_label_new(gettext("Вып-ный налог"));
gtk_box_pack_start (GTK_BOX (hbox[E_VIP_NAL]), label, FALSE, FALSE, 0);

data.entry[E_VIP_NAL] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_VIP_NAL]), data.entry[E_VIP_NAL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VIP_NAL]), "activate",GTK_SIGNAL_FUNC(l_f1dfz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VIP_NAL]),data.vip_nal.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VIP_NAL]),(gpointer)E_VIP_NAL);

label=gtk_label_new(gettext("Нач-ный доход"));
gtk_box_pack_start (GTK_BOX (hbox[E_NAH_DOH]), label, FALSE, FALSE, 0);

data.entry[E_NAH_DOH] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_NAH_DOH]), data.entry[E_NAH_DOH], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAH_DOH]), "activate",GTK_SIGNAL_FUNC(l_f1dfz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAH_DOH]),data.nah_doh.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAH_DOH]),(gpointer)E_NAH_DOH);

label=gtk_label_new(gettext("Нач-ный налог"));
gtk_box_pack_start (GTK_BOX (hbox[E_NAH_NAL]), label, FALSE, FALSE, 0);

data.entry[E_NAH_NAL] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_NAH_NAL]), data.entry[E_NAH_NAL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAH_NAL]), "activate",GTK_SIGNAL_FUNC(l_f1dfz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAH_NAL]),data.nah_nal.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAH_NAL]),(gpointer)E_NAH_NAL);

label=gtk_label_new(gettext("Признак дохода"));
gtk_box_pack_start (GTK_BOX (hbox[E_OZ_DOH]), label, FALSE, FALSE, 0);

data.entry[E_OZ_DOH] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_OZ_DOH]), data.entry[E_OZ_DOH], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_OZ_DOH]), "activate",GTK_SIGNAL_FUNC(l_f1dfz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_OZ_DOH]),data.oz_doh.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_OZ_DOH]),(gpointer)E_OZ_DOH);


sprintf(strsql,"%s (%s)",gettext("Дата приёма"),gettext("д.м.г"));
data.knopka_enter[E_DATA_PNR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_PNR]), data.knopka_enter[E_DATA_PNR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_PNR]),"clicked",GTK_SIGNAL_FUNC(l_f1dfz_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_PNR]),(gpointer)E_DATA_PNR);
tooltips_enter[E_DATA_PNR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_PNR],data.knopka_enter[E_DATA_PNR],gettext("Выбор табельного номера"),NULL);

data.entry[E_DATA_PNR] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_PNR]), data.entry[E_DATA_PNR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_PNR]), "activate",GTK_SIGNAL_FUNC(l_f1dfz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_PNR]),data.data_pnr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_PNR]),(gpointer)E_DATA_PNR);

sprintf(strsql,"%s (%s)",gettext("Дата увольнения"),gettext("д.м.г"));
data.knopka_enter[E_DATA_USR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_USR]), data.knopka_enter[E_DATA_USR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_USR]),"clicked",GTK_SIGNAL_FUNC(l_f1dfz_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_USR]),(gpointer)E_DATA_USR);
tooltips_enter[E_DATA_USR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_USR],data.knopka_enter[E_DATA_USR],gettext("Выбор табельного номера"),NULL);

data.entry[E_DATA_USR] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_USR]), data.entry[E_DATA_USR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_USR]), "activate",GTK_SIGNAL_FUNC(l_f1dfz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_USR]),data.data_usr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_USR]),(gpointer)E_DATA_USR);


label=gtk_label_new(gettext("Код льготы"));
gtk_box_pack_start (GTK_BOX (hbox[E_LGOTA]), label, FALSE, FALSE, 0);

data.entry[E_LGOTA] = gtk_entry_new_with_max_length (50);
gtk_box_pack_start (GTK_BOX (hbox[E_LGOTA]), data.entry[E_LGOTA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_LGOTA]), "activate",GTK_SIGNAL_FUNC(l_f1dfz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_LGOTA]),data.lgota.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_LGOTA]),(gpointer)E_LGOTA);






sprintf(strsql,"%s (0/1)",gettext("Признак"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_PRIZNAK]), label, FALSE, FALSE, 0);

data.entry[E_PRIZNAK] = gtk_entry_new_with_max_length (1);
gtk_box_pack_start (GTK_BOX (hbox[E_PRIZNAK]), data.entry[E_PRIZNAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PRIZNAK]), "activate",GTK_SIGNAL_FUNC(l_f1dfz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PRIZNAK]),data.pr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PRIZNAK]),(gpointer)E_PRIZNAK);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_f1dfz_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_f1dfz_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

if(data.inn_k.getdlinna() >  1)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(hbox[E_TABNOM]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(hbox[E_INN]),FALSE);//Недоступна
  gtk_widget_grab_focus(data.entry[E_VIP_DOH]);
 }
else
  gtk_widget_grab_focus(data.entry[E_TABNOM]);

if(data.viddok == 0)
 {
//  gtk_widget_hide(hbox[E_PRIZNAK);
  
  gtk_widget_set_sensitive(GTK_WIDGET(hbox[E_PRIZNAK]),FALSE);//Недоступна
 }
gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 {
  inn->new_plus(data.inn.ravno());
  priz->new_plus(data.oz_doh.ravno());
  lgota->new_plus(data.lgota.ravno());
  pr->new_plus(data.pr.ravno());
  
 }
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_f1dfz_v_e_knopka(GtkWidget *widget,class l_f1dfz_v_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   

  case E_TABNOM:
    if(l_sptbn(1,&kod,&naim,0,data->window) == 0)
     data->tabnom.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->tabnom.ravno_toutf());

    //Читаем фамилию
    sprintf(strsql,"select fio,inn from Kartb where tabn=%d",data->tabnom.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      data->fio.new_plus(row[0]);  
      data->inn.new_plus(row[1]);  
     }
    gtk_label_set_text(GTK_LABEL(data->label_fio),data->fio.ravno_toutf());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_INN]),data->inn.ravno_toutf());

    return;

  case E_DATA_PNR:
    iceb_calendar(&data->data_pnr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_PNR]),data->data_pnr.ravno_toutf());
    return;

  case E_DATA_USR:
    iceb_calendar(&data->data_usr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_USR]),data->data_usr.ravno_toutf());
    return;
  
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_f1dfz_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_f1dfz_v_data *data)
{
//char  bros[512];

//printf("l_f1dfz_v_key_press\n");
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
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_f1dfz_v_knopka(GtkWidget *widget,class l_f1dfz_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_f1dfz_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_f1dfz_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  


  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_f1dfz_v_vvod(GtkWidget *widget,class l_f1dfz_v_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_f1dfz_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_VIP_DOH:
    data->vip_doh.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_INN:
    data->inn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_TABNOM:
    data->tabnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    //Читаем фамилию
    sprintf(strsql,"select fio,inn from Kartb where tabn=%d",data->tabnom.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      data->fio.new_plus(row[0]);  
      data->inn.new_plus(row[1]);  
     }
    gtk_label_set_text(GTK_LABEL(data->label_fio),data->fio.ravno_toutf());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_INN]),data->inn.ravno_toutf());
    break;

  case E_DATA_PNR:
    data->data_pnr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_DATA_USR:
    data->data_usr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NAH_DOH:
    data->nah_doh.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  
  case E_VIP_NAL:
    data->vip_nal.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NAH_NAL:
    data->nah_nal.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_OZ_DOH:
    data->oz_doh.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_LGOTA:
    data->lgota.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int l_f1dfz_zap(class l_f1dfz_v_data *data)
{
char strsql[1024];
//SQL_str row;
time_t   vrem;
time(&vrem);

if(data->inn.getdlinna() <= 1 || data->vip_doh.ravno_atof() == 0. || data->oz_doh.ravno_atoi() == 0 || \
data->pr.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели все обязательные реквизиты !"),data->window);
  return(1);
 }

short d1=0,m1=0,g1=0,d2=0,m2=0,g2=0;

if(data->data_pnr.getdlinna() > 1)
  if(iceb_u_rsdat(&d1,&m1,&g1,data->data_pnr.ravno(),1) != 0)
   {
    iceb_menu_soob(gettext("Неправильно ввели дату приёма на работу !"),data->window);
    return(1);
   }
if(data->data_usr.getdlinna() > 1)
  if(iceb_u_rsdat(&d2,&m2,&g2,data->data_usr.ravno(),1) != 0)
   {
    iceb_menu_soob(gettext("Неправильно ввели дату увольнения с работы !"),data->window);
    return(1);
   }

int pr=0;
if(data->viddok == 1 || data->viddok == 2)
    pr=data->pr.ravno_atoi();

if(pr != 0 && pr != 1)
 {
  iceb_menu_soob(gettext("Неправильно ввели признак записи !"),data->window);
  return(1);
 }

if(iceb_u_SRAV(data->inn.ravno(),data->inn_k.ravno(),0) != 0 || \
iceb_u_SRAV(data->lgota.ravno(),data->lgota_k.ravno(),0) != 0 || \
iceb_u_SRAV(data->oz_doh.ravno(),data->oz_doh_k.ravno(),0) != 0 || \
data->pr.ravno_atoi() != data->pr_k.ravno_atoi())
  {
   sprintf(strsql,"select inn from F8dr1 where nomd='%s' and inn='%s' and priz='%s' \
 and lgota='%s' and pr=%d",
   data->nomdok.ravno(),
   data->inn.ravno(),
   data->oz_doh.ravno(),
   data->lgota.ravno(),
   data->pr.ravno_atoi());
   
   if(iceb_sql_readkey(strsql,data->window) > 0)
    {
     iceb_menu_soob(gettext("Такая запись уже есть !"),data->window);
     return(1);
    }
  }
 

if(data->inn_k.getdlinna() <= 1)
  sprintf(strsql,"insert into F8dr1 \
values ('%s','%s',%.2f,%.2f,'%s','%04d-%d-%d','%04d-%d-%d','%s',%d,%ld,'%s',%.2f,%.2f,%d)",
  data->nomdok.ravno(),
  data->inn.ravno(),
  data->vip_doh.ravno_atof(),
  data->vip_nal.ravno_atof(),
  data->oz_doh.ravno(),
  g1,m1,d1,
  g2,m2,d2,
  data->lgota.ravno(),
  iceb_getuid(data->window),
  vrem,
  "\0",
  data->nah_doh.ravno_atof(),
  data->nah_nal.ravno_atof(),
  data->pr.ravno_atoi());
else
  sprintf(strsql,"update F8dr1 set \
sumad=%.2f,\
suman=%.2f,\
priz='%s',\
datap='%04d-%d-%d',\
datau='%04d-%d-%d',\
lgota='%s',\
ktoz=%d,\
vrem=%ld,\
fio='%s',\
sumadn=%.2f,\
sumann=%.2f,\
pr=%d \
where nomd='%s' and inn='%s' and priz='%s' and lgota='%s' and pr=%d",
  data->vip_doh.ravno_atof(),
  data->vip_nal.ravno_atof(),
  data->oz_doh.ravno(),
  g1,m1,d1,
  g2,m2,d2,
  data->lgota.ravno(),
  iceb_getuid(data->window),
  vrem,
  "\0",
  data->nah_doh.ravno_atof(),
  data->nah_nal.ravno_atof(),
  data->pr.ravno_atoi(),
  data->nomdok.ravno(),data->inn_k.ravno(),data->oz_doh_k.ravno(),data->lgota_k.ravno(),data->pr_k.ravno_atoi());

if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
 return(1);

gtk_widget_hide(data->window);


return(0);

}
