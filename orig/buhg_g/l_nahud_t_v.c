/*$Id: l_nahud_t_v.c,v 1.21 2011-02-21 07:35:53 sasa Exp $*/
/*17.10.2006	31.08.2006	Белых А.И.	l_nahud_t_v.c
Ввод и корректировка начислений/удержаний
*/
#include <stdlib.h>
#include "buhg_g.h"

enum
{
 FK2,
 FK10,
 KOL_F_KL
};

enum
 {
  E_SUMA,
  E_DEN_ZAP,
  E_VKM,
  E_KDATA,
  E_KOL_DNEI,
  E_SHET,
  E_KOMENT,
  E_NOM_ZAP,
  E_PODR,
  KOLENTER  
 };

class l_nahud_t_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  //Уникальные реквизиты записи которую корректируют
  class ZARP zp;

  class iceb_u_str nom_dok;
  
  //Реквизиты которые вводятся в меню
  class iceb_u_str suma;
  class iceb_u_str denz;
  class iceb_u_str vkm;
  class iceb_u_str kdata;
  class iceb_u_str kol_dnei;
  class iceb_u_str shet;
  class iceb_u_str koment;
  class iceb_u_str nom_zap;
  class iceb_u_str podr;
  
  l_nahud_t_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_rek();
    nom_dok.plus("");
   }

  void read_rek()
   {
     suma.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SUMA]))));
     shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET]))));
     podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PODR]))));
     denz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DEN_ZAP]))));
     vkm.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VKM]))));
     kdata.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KDATA]))));
     kol_dnei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOL_DNEI]))));
     koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT]))));
     nom_zap.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOM_ZAP]))));
   }
  void clear_rek()
   {
    suma.new_plus("");
    denz.new_plus("");
    vkm.new_plus("");
    kdata.new_plus("");
    kol_dnei.new_plus("");
    shet.new_plus("");
    koment.new_plus("");
    nom_zap.new_plus("");
    podr.new_plus("");
   }
 };

gboolean   l_nahud_t_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_nahud_t_v_data *data);
void    l_nahud_t_v_v_vvod(GtkWidget *widget,class l_nahud_t_v_data *data);
void  l_nahud_t_v_v_knopka(GtkWidget *widget,class l_nahud_t_v_data *data);
void  l_nahud_t_v_v_e_knopka(GtkWidget *widget,class l_nahud_t_v_data *data);

int l_nahud_t_v_zap(class l_nahud_t_v_data *data);

extern char *name_system;
extern SQL_baza bd;
extern double   okrg; /*Округление*/


int l_nahud_t_v(int tabnomz,
short dz,short mz,short gz,
int prnz,
int knahz,
short mesv,short godv,
int podrz,
const char *shetz,
int nom_zapz,
GtkWidget *wpredok)
{
class iceb_u_str kikz;
char naim_kod[512];
char shet_nu[56];
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
static class iceb_u_str denz; //день записи
static class iceb_u_str vkm;  //В счёт какого месяца

class l_nahud_t_v_data data;
data.zp.tabnom=tabnomz;
data.zp.dz=dz;
data.zp.mz=mz;
data.zp.gz=gz;
data.zp.prn=prnz;
data.zp.knu=knahz;
data.zp.mesn=mesv;
data.zp.godn=godv;

data.zp.podr=podrz;
data.podr.new_plus(data.zp.podr);

strncpy(data.zp.shet,shetz,sizeof(data.zp.shet)-1);
data.shet.new_plus(data.zp.shet);

data.zp.nomz=nom_zapz;
data.nom_zap.new_plus(data.zp.nomz);
 

data.denz.new_plus(data.zp.dz);  
if(data.zp.dz == 0)
 {
  if(denz.getdlinna() > 1)
   data.denz.new_plus(denz.ravno());
  else
   { 
    short dt,mt,gt;
//    iceb_u_dpm(&dt,&data.zp.mz,&data.zp.gz,5);
    iceb_u_poltekdat(&dt,&mt,&gt);
    data.denz.new_plus(dt);  
   }
 }

if(data.zp.mesn == 0)
 { 
  if(vkm.getdlinna() > 1)
    strcpy(strsql,vkm.ravno());
  else
   sprintf(strsql,"%d.%d",data.zp.mz,data.zp.gz);
 }
else
   sprintf(strsql,"%d.%d",data.zp.mesn,data.zp.godn);

data.vkm.new_plus(strsql);



//Читаем наименование начисления/удержания
memset(naim_kod,'\0',sizeof(naim_kod));
memset(shet_nu,'\0',sizeof(shet_nu));
if(data.zp.prn == 1)
   sprintf(strsql,"select naik,shet from Nash where kod=%d",data.zp.knu);
if(data.zp.prn == 2)
   sprintf(strsql,"select naik,shet from Uder where kod=%d",data.zp.knu);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  if(data.zp.prn == 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код начисления"),data.zp.knu);
    iceb_menu_soob(strsql,wpredok);
   }
  if(data.zp.prn == 2)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код удержания"),data.zp.knu);
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }
strncpy(naim_kod,row[0],sizeof(naim_kod)-1);
strncpy(shet_nu,row[1],sizeof(shet_nu)-1);

if(data.zp.dz != 0)
 {
  //читаем корректируемую запись
  sprintf(strsql,"select suma,kdn,kom,nd,ktoz,vrem from Zarp where \
datz='%04d-%02d-%02d' and tabn=%d and prn='%d' and knah=%d and godn=%d and mesn=%d \
and podr=%d and shet='%s' and nomz=%d",
  data.zp.gz,data.zp.mz,data.zp.dz,data.zp.tabnom,data.zp.prn,data.zp.knu,data.zp.godn,data.zp.mesn,data.zp.podr,data.zp.shet,data.zp.nomz);   
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }
  
  data.suma.new_plus(atof(row[0]));
  data.kol_dnei.new_plus(row[1]);
  data.koment.new_plus(row[2]);
  data.nom_dok.new_plus(row[3]);    

  kikz.plus(iceb_kikz(row[4],row[5],wpredok));

  //читаем дату до которой действует начисление/удержание  
  sprintf(strsql,"select datd from Zarn1 where tabn=%d and prn=%d and knah=%d",
  data.zp.tabnom,data.zp.prn,data.zp.knu);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    if(row[0][0] != '0')
     {
      short d,m,g;
      
      iceb_u_rsdat(&d,&m,&g,row[0],2);
      sprintf(strsql,"%d.%d.%d",d,m,g);
      data.kdata.new_plus(strsql);
     }    
   }
 }
else
 {
  //Смотрим последний счет в промежуточной записи
  sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn=%d \
and knah=%d",data.zp.tabnom,data.zp.prn,data.zp.knu);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
      data.shet.new_plus(row[0]);


  if(data.shet.getdlinna() <= 1)
   if(shet_nu[0] != '\0');
    data.shet.new_plus(shet_nu);

  if(data.shet.getdlinna() <= 1)
   {
    //читаем счёт в промежуточной записи
    sprintf(strsql,"select shet from Zarn where tabn=%d and god=%d and mes=%d",
    data.zp.tabnom,data.zp.gz,data.zp.mz);
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
       data.shet.new_plus(row[0]);
    
   }

  if(data.shet.getdlinna() <= 1)
   {
    //читаем счёт в карточке
    sprintf(strsql,"select shet from Kartb where tabn=%d",data.zp.tabnom);
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
       data.shet.new_plus(row[0]);
    
   }
 }






data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.zp.dz != 0)
 sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));
else
 sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_nahud_t_v_v_key_press),&data);

iceb_u_str zagolov;
if(data.zp.dz == 0)
  zagolov.new_plus(gettext("Ввод новой записи"));
else
 {
  zagolov.new_plus(gettext("Корректировка записи"));
  zagolov.ps_plus(kikz.ravno());
  if(data.nom_dok.getdlinna() > 1)
   {
    zagolov.ps_plus(gettext("Номер документа"));
    zagolov.plus(":");
    zagolov.plus(data.nom_dok.ravno());
   }
 }
zagolov.ps_plus(data.zp.knu);
zagolov.plus(" ");
zagolov.plus(naim_kod);

GtkWidget *label=gtk_label_new(zagolov.ravno_toutf());

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

sprintf(strsql,"%s",gettext("Сумма"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), label, FALSE, FALSE, 0);

data.entry[E_SUMA] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMA]), "activate",GTK_SIGNAL_FUNC(l_nahud_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.suma.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMA]),(gpointer)E_SUMA);


sprintf(strsql,"%s",gettext("День записи"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DEN_ZAP]), label,FALSE, FALSE, 0);

data.entry[E_DEN_ZAP] = gtk_entry_new_with_max_length (3);
gtk_box_pack_start (GTK_BOX (hbox[E_DEN_ZAP]), data.entry[E_DEN_ZAP],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DEN_ZAP]), "activate",GTK_SIGNAL_FUNC(l_nahud_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DEN_ZAP]),data.denz.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DEN_ZAP]),(gpointer)E_DEN_ZAP);


sprintf(strsql,"%s",gettext("В счёт какого месяца"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_VKM]), label,FALSE, FALSE, 0);

data.entry[E_VKM] = gtk_entry_new_with_max_length (8);
gtk_box_pack_start (GTK_BOX (hbox[E_VKM]), data.entry[E_VKM],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VKM]), "activate",GTK_SIGNAL_FUNC(l_nahud_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VKM]),data.vkm.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VKM]),(gpointer)E_VKM);


//sprintf(strsql,"%s",gettext("До какой даты действует"));
//label=gtk_label_new(iceb_u_toutf(strsql));
//gtk_box_pack_start (GTK_BOX (hbox[E_KDATA]), label,FALSE, FALSE, 0);

sprintf(strsql,"%s",gettext("До какой даты действует"));
data.knopka_enter[E_KDATA]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KDATA]), data.knopka_enter[E_KDATA], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KDATA]),"clicked",GTK_SIGNAL_FUNC(l_nahud_t_v_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KDATA]),(gpointer)E_KDATA);
tooltips_enter[E_KDATA]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KDATA],data.knopka_enter[E_KDATA],gettext("Выбор даты"),NULL);

data.entry[E_KDATA] = gtk_entry_new_with_max_length (11);
gtk_box_pack_start (GTK_BOX (hbox[E_KDATA]), data.entry[E_KDATA],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KDATA]), "activate",GTK_SIGNAL_FUNC(l_nahud_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KDATA]),data.kdata.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KDATA]),(gpointer)E_KDATA);

sprintf(strsql,"%s",gettext("Количество дней"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_DNEI]), label,FALSE, FALSE, 0);

data.entry[E_KOL_DNEI] = gtk_entry_new_with_max_length (3);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_DNEI]), data.entry[E_KOL_DNEI],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOL_DNEI]), "activate",GTK_SIGNAL_FUNC(l_nahud_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOL_DNEI]),data.kol_dnei.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOL_DNEI]),(gpointer)E_KOL_DNEI);

sprintf(strsql,"%s",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(l_nahud_t_v_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(l_nahud_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);


sprintf(strsql,"%s",gettext("Коментарий"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label,FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new_with_max_length (40);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(l_nahud_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);

sprintf(strsql,"%s",gettext("Номер записи"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NOM_ZAP]), label,FALSE, FALSE, 0);

data.entry[E_NOM_ZAP] = gtk_entry_new_with_max_length (5);
gtk_box_pack_start (GTK_BOX (hbox[E_NOM_ZAP]), data.entry[E_NOM_ZAP],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOM_ZAP]), "activate",GTK_SIGNAL_FUNC(l_nahud_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOM_ZAP]),data.nom_zap.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOM_ZAP]),(gpointer)E_NOM_ZAP);

sprintf(strsql,"%s",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODR]),"clicked",GTK_SIGNAL_FUNC(l_nahud_t_v_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODR]),(gpointer)E_PODR);
tooltips_enter[E_PODR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_PODR],data.knopka_enter[E_PODR],gettext("Выбор подразделения"),NULL);

data.entry[E_PODR] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODR]), "activate",GTK_SIGNAL_FUNC(l_nahud_t_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.podr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODR]),(gpointer)E_PODR);


GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введённой в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_nahud_t_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_nahud_t_v_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

denz.new_plus(data.denz.ravno());
vkm.new_plus(data.vkm.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_nahud_t_v_v_e_knopka(GtkWidget *widget,class l_nahud_t_v_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));


switch (knop)
 {
  case E_KDATA:

    if(iceb_calendar(&data->kdata,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KDATA]),data->kdata.ravno());
      
    return;  

  case E_SHET:
    iceb_vibrek(1,"Plansh",&data->shet,&naim,data->window);
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet.ravno_toutf());
    return;

  case E_PODR:
    if(l_zarpodr(1,&kod,&naim,data->window) == 0)
     data->podr.new_plus(kod.ravno());
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->podr.ravno_toutf());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_nahud_t_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_nahud_t_v_data *data)
{

//printf("l_nahud_t_v_v_key_press\n");
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
void  l_nahud_t_v_v_knopka(GtkWidget *widget,class l_nahud_t_v_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_nahud_t_v_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    data->voz=0;
    if(l_nahud_t_v_zap(data) != 0)
     return;

    gtk_widget_destroy(data->window);

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

void    l_nahud_t_v_v_vvod(GtkWidget *widget,class l_nahud_t_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_nahud_t_v_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SUMA:
    data->suma.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


  case E_SHET:
    data->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_PODR:
    data->podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DEN_ZAP:
    data->denz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_VKM:
    data->vkm.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KDATA:
    data->kdata.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOL_DNEI:
    data->kol_dnei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOMENT:
    data->koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOM_ZAP:
    data->nom_zap.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/***********************************/
/*Запись информации*/
/*****************************/

int l_nahud_t_v_zap(class l_nahud_t_v_data *data)
{

if(data->nom_dok.getdlinna() > 1)
 {
  iceb_menu_soob(gettext("Записи сделанные из документов, корректируються только из документов !"),data->window);
  return(1);
 }   

if(data->podr.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён код подразделения !"),data->window);
  return(1);
 }
char strsql[512];
/*Проверяем есть ли код подразделения в списке подразделений*/
sprintf(strsql,"select kod from Podr where kod=%d",data->podr.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 { 
  sprintf(strsql,"%s %s !",gettext("Не найден код подразделения"),data->podr.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
 
if(data->shet.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён счёт !"),data->window);
  return(1);
 }
short dkd=0,mkd=0,gkd=0; //дата конца действия
if(data->kdata.getdlinna() > 1)
if(iceb_u_rsdat(&dkd,&mkd,&gkd,data->kdata.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата конца действия !"),data->window);
  return(1);
 }

short mv,gv;

if(iceb_u_rsdat1(&mv,&gv,data->vkm.ravno()) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата !"),data->window);
  return(1);
 }
SQL_str row;
class SQLCURSOR cur;
short dnr=0,mnr=0,gnr=0;
short dur=0,mur=0,gur=0;

//читаем даты приёма-увольнения с работы
sprintf(strsql,"select datn,datk from Zarn where tabn=%d and god=%d and mes=%d",
data->zp.tabnom,data->zp.gz,data->zp.mz);
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  iceb_u_rsdat(&dnr,&mnr,&gnr,row[0],2);
  iceb_u_rsdat(&dur,&mur,&gur,row[1],2);
 }
else
 {
  sprintf(strsql,"select datn,datk from Kartb where tabn=%d",data->zp.tabnom);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   {
    iceb_u_rsdat(&dnr,&mnr,&gnr,row[0],2);
    iceb_u_rsdat(&dur,&mur,&gur,row[1],2);
   }  
 } 

if(iceb_u_sravmydat(31,mv,gv,dnr,mnr,gnr) < 0)
 {
  //Проверяем может он догда работал
  sprintf(strsql,"select datn from Zarn where god=%d and mes=%d and tabn=%d",
  data->zp.gz,data->zp.mz,data->zp.tabnom);
  if(sql_readkey(&bd,strsql,&row,&cur) < 1)
   {
    sprintf(strsql,"%s %d.%d.%d !",gettext("Принят на работу"),dnr,mnr,gnr);
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 } 

if(data->zp.prn == 1)
if(dur != 0 && iceb_u_sravmydat(1,mv,gv,dur,mur,gur) > 0)
 {
  sprintf(strsql,"%s %d.%d.%d !",gettext("Уволен с работы"),dur,mur,gur);
  iceb_menu_soob(strsql,data->window);
  return(1);
   
 } 
OPSHET rek_shet;

if(iceb_prsh1(data->shet.ravno(),&rek_shet,data->window) != 0)
  return(1);

short d,m,g;  
sprintf(strsql,"%s.%d.%d",data->denz.ravno(),data->zp.mz,data->zp.gz);
if(iceb_u_rsdat(&d,&m,&g,strsql,0) != 0)
 {
  iceb_menu_soob(gettext("Не верно введен день начисления/удержания !"),data->window);
  return(1);
 }
  

if(data->zp.dz == 0 || //Ввод новой записи
(data->zp.dz != 0 && //Корректировка записи
 (data->zp.dz != data->denz.ravno_atoi() || data->zp.mesn != mv || data->zp.godn != gv || \
 data->zp.nomz != data->nom_zap.ravno_atoi() ||
 data->zp.podr != data->podr.ravno_atoi() || iceb_u_SRAV(data->zp.shet,data->shet.ravno(),0) != 0)))
 {
  sprintf(strsql,"select datz from Zarp where datz='%04d-%02d-%02d' and \
tabn=%d and prn='%d' and knah=%d and godn=%d and mesn=%d and podr=%d \
and shet='%s' and nomz=%d and suma != 0.",
  data->zp.gz,data->zp.mz,data->denz.ravno_atoi(),data->zp.tabnom,data->zp.prn,data->zp.knu,gv,mv,data->podr.ravno_atoi(),
  data->shet.ravno(),data->nom_zap.ravno_atoi());
  printf("\nl_nahud_t_v=%s\n",strsql);
  if(iceb_sql_readkey(strsql,data->window) >= 1)
   {
    
    printf("\nl_nahud_t_v=***\n");
    iceb_menu_soob(gettext("Такая запись уже есть !"),data->window);
    return(1);
   }
 }


double suma=data->suma.ravno_atof();
suma=iceb_u_okrug(suma,okrg);
if(data->zp.prn == 2)
  suma*=(-1);


if(zapzarpvw(&data->zp,suma,data->denz.ravno_atoi(),mv,gv,dkd,mkd,gkd,data->kol_dnei.ravno_atoi(),
data->shet.ravno(),data->koment.ravno_filtr(),data->nom_zap.ravno_atoi(),data->podr.ravno_atoi(),data->nom_dok.ravno(),data->window) != 0)
  return(1);
return(0);

}
