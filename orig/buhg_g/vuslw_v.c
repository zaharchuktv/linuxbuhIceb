/*$Id: vuslw_v.c,v 1.18 2011-02-21 07:35:58 sasa Exp $*/
/*25.11.2005	19.10.2005	Белых А.И.	vuslw_v.c
Меню ввода и корректировки услуги/материалла
*/
#include <stdlib.h>
#include "buhg_g.h"

enum
{
  FK2,
//  FK3,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KOLIH,
  E_CENA_SNDS,
  E_CENA_BNDS,
  E_EI,
  E_SHETU,
  E_SUMA_BNDS,
  E_SUMA_SNDS,
  E_SHET_PS,
  KOLENTER  
 };

class vuslw_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет

  //Реквизиты документ в который вводим запись
  short dd,md,gd;
  int tipz;
  int podr;
  iceb_u_str nomdok;
  iceb_u_str kodzap;
  int metka_zap;
  int nom_zap;
  float pnds;

  //Реквизиты введённые в меню
  iceb_u_str kolih;
  iceb_u_str cena_snds;
  iceb_u_str cena_bnds;
  iceb_u_str ei;
  iceb_u_str shetu;
  iceb_u_str suma_bnds;
  iceb_u_str suma_snds;
  iceb_u_str shet_ps;

  vuslw_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    kolih.plus("");
    cena_snds.plus("");
    cena_bnds.plus("");
    ei.plus("");
    shetu.plus("");
    suma_bnds.plus("");
    suma_snds.plus("");
    shet_ps.plus("");
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      if(i == E_CENA_SNDS)
       continue;
      if(i == E_SUMA_SNDS)
       continue;
      if(i == E_SUMA_BNDS)
       continue;
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
     }
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    void read_rek();
   }
 };

void  vuslw_v_e_knopka(GtkWidget *widget,class vuslw_v_data *data);
void    vuslw_v_vvod(GtkWidget *widget,class vuslw_v_data *data);
gboolean   vuslw_v_key_press(GtkWidget *widget,GdkEventKey *event,class vuslw_v_data *data);
void  vuslw_v_knopka(GtkWidget *widget,class vuslw_v_data *data);

int vuslw_v_zap(class vuslw_v_data *data);

extern SQL_baza	bd;
extern char *name_system;
extern double	okrcn;  /*Округление цены*/
extern double	okrg1;  /*Округление1 */


int vuslw_v(short dd,short md,short gd, //Дата документа
int tipz,
int podr,
const char *nomdok,
const char *kod,
int metka, //0-материал 1-услуга
int nom_zap, //Номер записи в документе. Если равно -1 то это новая запись.
float pnds, //Процент НДС на момент создания документа
GtkWidget *wpredok)
{
char strsql[512];
class vuslw_v_data data;

data.dd=dd;
data.md=md;
data.gd=gd;
data.tipz=tipz;
data.podr=podr;
data.nomdok.new_plus(nomdok);
data.kodzap.new_plus(kod);
data.metka_zap=metka;
data.nom_zap=nom_zap;
data.pnds=pnds;
SQL_str row;
SQLCURSOR cur;


//читаем код записи и узнаем нужные реквизиты
double  ndscen=0.;
if(metka == 1)
 {
  sprintf(strsql,"select cena,ei,naius,nds,shetu from Uslugi \
  where kodus=%s",kod);

  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код услуги"),kod);
    iceb_menu_soob(strsql,wpredok);
    return(1);
   }
   data.shetu.new_plus(row[4]);
 }

if(metka == 0)
 {
  sprintf(strsql,"select cenapr,ei,naimat,nds from Material \
  where kodm=%s",kod);

  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),kod);
    iceb_menu_soob(strsql,wpredok);
    return(1);
   }
 }
 
double cenabn,cena;

cenabn=cena=atof(row[0]);
data.ei.new_plus(row[1]);
iceb_u_str naim_zap;
naim_zap.plus(row[2]);

ndscen=atof(row[3]);


if(cena != 0.  && ndscen == 0.)
 {
  cenabn=cena-cena*pnds/(100.+pnds);
  cenabn=iceb_u_okrug(cenabn,okrcn);
 }

if(data.nom_zap != -1) //Корректировать запись
 {
  //Читаем количество и цену в документе
  sprintf(strsql,"select kolih,cena,ei,shetu,shsp from Usldokum1 where \
datd='%04d-%02d-%02d' and podr=%d and tp=%d and nomd='%s' and metka=%d \
and kodzap=%s and nz=%d",
  gd,md,dd,podr,tipz,nomdok,metka,kod,nom_zap);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_menu_soob(gettext("Не найдена запись в документе !"),wpredok);
    return(1);
   }
  data.kolih.new_plus(row[0]);

  cenabn=atof(row[1]);
  cenabn=iceb_u_okrug(cenabn,okrcn);

  data.ei.new_plus(row[2]);
  data.shetu.new_plus(row[3]);
  data.shet_ps.new_plus(row[4]);
 }


double bb=0.;

if(cenabn != 0.)
 {
  sprintf(strsql,"%.10g",cenabn);
  data.cena_bnds.new_plus(strsql);
  bb=cenabn+cenabn*pnds/100.;
  bb=iceb_u_okrug(bb,okrg1);
 }
if(bb != 0.)
 {
  sprintf(strsql,"%.10g",bb);
  data.cena_snds.new_plus(strsql);  
 }




data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.metka_zap == 0)
 sprintf(strsql,"%s %s",name_system,gettext("Ввод материалла"));
if(data.metka_zap == 1)
 sprintf(strsql,"%s %s",name_system,gettext("Ввод услуги"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(vuslw_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

iceb_u_str nadpis;

if(data.metka_zap == 0)
 nadpis.plus(gettext("Ввод материалла"));
if(data.metka_zap == 1)
 nadpis.plus(gettext("Ввод услуги"));

nadpis.ps_plus(kod);
nadpis.plus(" ");
nadpis.plus(naim_zap.ravno());

GtkWidget *label=gtk_label_new(nadpis.ravno_toutf());

GtkWidget *vbox = gtk_vbox_new (FALSE, 1);
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

label=gtk_label_new(gettext("Количество"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), label, FALSE, FALSE, 0);

data.entry[E_KOLIH] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), data.entry[E_KOLIH], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOLIH]), "activate",GTK_SIGNAL_FUNC(vuslw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH]),data.kolih.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOLIH]),(gpointer)E_KOLIH);

label=gtk_label_new(gettext("Цена с НДС"));
gtk_box_pack_start (GTK_BOX (hbox[E_CENA_SNDS]), label, FALSE, FALSE, 0);

data.entry[E_CENA_SNDS] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA_SNDS]), data.entry[E_CENA_SNDS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_CENA_SNDS]), "activate",GTK_SIGNAL_FUNC(vuslw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA_SNDS]),data.cena_snds.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_CENA_SNDS]),(gpointer)E_CENA_SNDS);

label=gtk_label_new(gettext("Цена без НДС"));
gtk_box_pack_start (GTK_BOX (hbox[E_CENA_BNDS]), label, FALSE, FALSE, 0);

data.entry[E_CENA_BNDS] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA_BNDS]), data.entry[E_CENA_BNDS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_CENA_BNDS]), "activate",GTK_SIGNAL_FUNC(vuslw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA_BNDS]),data.cena_bnds.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_CENA_BNDS]),(gpointer)E_CENA_BNDS);

sprintf(strsql,"%s",gettext("Единица измерения"));
data.knopka_enter[E_EI]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.knopka_enter[E_EI], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_EI]),"clicked",GTK_SIGNAL_FUNC(vuslw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_EI]),(gpointer)E_EI);
tooltips_enter[E_EI]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_EI],data.knopka_enter[E_EI],gettext("Выбор единицы измерения."),NULL);

data.entry[E_EI] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_EI]), "activate",GTK_SIGNAL_FUNC(vuslw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.ei.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_EI]),(gpointer)E_EI);



label=gtk_label_new(gettext("Сумма без НДС"));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_BNDS]), label, FALSE, FALSE, 0);

data.entry[E_SUMA_BNDS] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_BNDS]), data.entry[E_SUMA_BNDS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMA_BNDS]), "activate",GTK_SIGNAL_FUNC(vuslw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA_BNDS]),data.suma_bnds.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMA_BNDS]),(gpointer)E_SUMA_BNDS);

label=gtk_label_new(gettext("Сумма з НДС"));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_SNDS]), label, FALSE, FALSE, 0);

data.entry[E_SUMA_SNDS] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_SNDS]), data.entry[E_SUMA_SNDS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SUMA_SNDS]), "activate",GTK_SIGNAL_FUNC(vuslw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA_SNDS]),data.suma_snds.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SUMA_SNDS]),(gpointer)E_SUMA_SNDS);



sprintf(strsql,"%s",gettext("Счёт учёта"));
data.knopka_enter[E_SHETU]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.knopka_enter[E_SHETU], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHETU]),"clicked",GTK_SIGNAL_FUNC(vuslw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHETU]),(gpointer)E_SHETU);
tooltips_enter[E_SHETU]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHETU],data.knopka_enter[E_SHETU],gettext("Выбор счёта в плане счетов."),NULL);

data.entry[E_SHETU] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.entry[E_SHETU], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHETU]), "activate",GTK_SIGNAL_FUNC(vuslw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETU]),data.shetu.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHETU]),(gpointer)E_SHETU);

if(data.tipz == 2)
 sprintf(strsql,"%s",gettext("Счёт списания"));
if(data.tipz == 1)
 sprintf(strsql,"%s",gettext("Счёт приобретения"));

data.knopka_enter[E_SHET_PS]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_PS]), data.knopka_enter[E_SHET_PS], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET_PS]),"clicked",GTK_SIGNAL_FUNC(vuslw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET_PS]),(gpointer)E_SHET_PS);
tooltips_enter[E_SHET_PS]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET_PS],data.knopka_enter[E_SHET_PS],gettext("Выбор счёта в плане счетов."),NULL);

data.entry[E_SHET_PS] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_PS]), data.entry[E_SHET_PS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET_PS]), "activate",GTK_SIGNAL_FUNC(vuslw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET_PS]),data.shet_ps.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET_PS]),(gpointer)E_SHET_PS);




GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Записать"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введённой в меню информации."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(vuslw_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(vuslw_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(vuslw_v_knopka),&data);
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
void  vuslw_v_e_knopka(GtkWidget *widget,class vuslw_v_data *data)
{
struct OPSHET	shetv;
iceb_u_str kod;
iceb_u_str naim;
kod.new_plus("");
naim.new_plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
   
  case E_EI:

    iceb_vibrek(1,"Edizmer",&data->shetu,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),data->ei.ravno_toutf());

  case E_SHETU:

    iceb_vibrek(1,"Plansh",&data->shetu,data->window);
    if(iceb_prsh1(data->shetu.ravno(),&shetv,data->window) != 0)
      data->shetu.new_plus("");
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETU]),data->shetu.ravno_toutf());

    return;  

  case E_SHET_PS:

    iceb_vibrek(1,"Plansh",&data->shet_ps,data->window);
    if(iceb_prsh1(data->shet_ps.ravno(),&shetv,data->window) != 0)
      data->shet_ps.new_plus("");
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET_PS]),data->shet_ps.ravno_toutf());

    return;  


 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vuslw_v_key_press(GtkWidget *widget,GdkEventKey *event,class vuslw_v_data *data)
{

//printf("vuslw_v_key_press\n");
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
void  vuslw_v_knopka(GtkWidget *widget,class vuslw_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(vuslw_v_zap(data) != 0)
     return;
     
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  


  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    vuslw_v_vvod(GtkWidget *widget,class vuslw_v_data *data)
{
char strsql[512];
double suma_snds;
double suma_bnds;
double cena_snds;
double cena_bnds;
double kolih;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("vuslw_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOLIH:
    data->kolih.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    if(data->kolih.ravno_atof() != 0.)
    if(data->cena_bnds.ravno_atof() != 0.)
     {
      //Сумма без НДС
      sprintf(strsql,"%.10g",data->kolih.ravno_atof()*data->cena_bnds.ravno_atof());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA_BNDS]),strsql);

      //Сумма с НДС      
      cena_snds=data->cena_bnds.ravno_atof()+data->cena_bnds.ravno_atof()*data->pnds/100.;
      cena_snds=iceb_u_okrug(cena_snds,okrg1);
      sprintf(strsql,"%.10g",data->kolih.ravno_atof()*cena_snds);

      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA_SNDS]),strsql);
     }
    break;

  case E_CENA_SNDS:
    data->cena_snds.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(data->cena_snds.ravno_atof() > 0.)
     {
      cena_bnds=data->cena_snds.ravno_atof()-data->cena_snds.ravno_atof()*data->pnds/(100.+data->pnds);
      cena_bnds=iceb_u_okrug(cena_bnds,okrcn);
      sprintf(strsql,"%.10g",cena_bnds);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENA_BNDS]),strsql);

      if(data->kolih.ravno_atof() != 0.)
       {
        //Сумма без НДС
        sprintf(strsql,"%.10g",data->kolih.ravno_atof()*cena_bnds);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA_BNDS]),strsql);

        //Сумма с НДС      
        cena_snds=cena_bnds+cena_bnds*data->pnds/100.;
        cena_snds=iceb_u_okrug(cena_snds,okrg1);
        sprintf(strsql,"%.10g",data->kolih.ravno_atof()*cena_snds);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA_SNDS]),strsql);
       }      
     }
    break;

  case E_CENA_BNDS:
    data->cena_bnds.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    if(data->cena_bnds.ravno_atof() != 0.)
     {
      cena_snds=data->cena_bnds.ravno_atof() + data->cena_bnds.ravno_atof()*data->pnds/100.;
      cena_snds=iceb_u_okrug(cena_snds,okrg1);
      sprintf(strsql,"%.10g",data->kolih.ravno_atof()*cena_snds);

      gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENA_SNDS]),strsql);

      if(data->kolih.ravno_atof() != 0.)
       {
        //Сумма без НДС
        cena_bnds=data->cena_bnds.ravno_atof();
        cena_bnds=iceb_u_okrug(cena_bnds,okrcn);
        sprintf(strsql,"%.10g",data->kolih.ravno_atof()*cena_bnds);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA_BNDS]),strsql);

        //Сумма с НДС      
        cena_snds=cena_bnds+cena_bnds*data->pnds/100.;
        cena_snds=iceb_u_okrug(cena_snds,okrg1);
        sprintf(strsql,"%.10g",data->kolih.ravno_atof()*cena_snds);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA_SNDS]),strsql);
       }      
      
     }
    break;

  case E_SUMA_BNDS:
    data->suma_bnds.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(data->suma_bnds.ravno_atof() != 0.)
     {
      kolih=data->kolih.ravno_atof();
      if(kolih == 0.)
       {
        if(data->cena_bnds.ravno_atof() != 0.)
         {
          kolih=data->suma_bnds.ravno_atof()/data->cena_bnds.ravno_atof();
          sprintf(strsql,"%.10g",kolih);
          gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOLIH]),strsql);
         }
       }
      if(kolih == 0.)
       break;

      //Пересчитываем цену без НДС
      cena_bnds=data->suma_bnds.ravno_atof()/kolih;
      cena_bnds=iceb_u_okrug(cena_bnds,okrcn);

      sprintf(strsql,"%.10g",cena_bnds);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENA_BNDS]),strsql);

      //Пересчитываем цену с НДС
      cena_snds=cena_bnds+cena_bnds*data->pnds/100.;
      cena_snds=iceb_u_okrug(cena_snds,okrg1);

      sprintf(strsql,"%.10g",cena_snds);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENA_SNDS]),strsql);

      //Пересчитываем сумму с НДС
      suma_snds=cena_snds*kolih;
      suma_snds=iceb_u_okrug(suma_snds,okrg1);
      sprintf(strsql,"%.10g",suma_snds);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA_SNDS]),strsql);
      
     }
    break;

  case E_SUMA_SNDS:
    data->suma_snds.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(data->suma_snds.ravno_atof() == 0.)
     break;

    suma_bnds=data->suma_snds.ravno_atof()-data->suma_snds.ravno_atof()*data->pnds/(100.-data->pnds);
    suma_bnds=iceb_u_okrug(suma_bnds,okrg1);
    
    kolih=data->kolih.ravno_atof();
    if(kolih == 0.)
     {
      if(data->cena_snds.ravno_atof() != 0.)
       {
        kolih=data->suma_snds.ravno_atof()/data->cena_snds.ravno_atof();
        sprintf(strsql,"%.10g",kolih);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOLIH]),strsql);
       }
     }
    if(kolih == 0.)
     break;

    //Пересчитываем цену без НДС
    cena_bnds=suma_bnds/kolih;
    cena_bnds=iceb_u_okrug(cena_bnds,okrcn);

    sprintf(strsql,"%.10g",cena_bnds);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENA_BNDS]),strsql);
    //Пересчитываем цену с НДС
    cena_snds=cena_bnds+cena_bnds*data->pnds/100.;
    cena_snds=iceb_u_okrug(cena_snds,okrg1);

    sprintf(strsql,"%.10g",cena_snds);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENA_SNDS]),strsql);
    break;

  case E_SHETU:
    data->shetu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_EI:
    data->ei.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHET_PS:
    data->shet_ps.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись в документ*/
/**********************/

int vuslw_v_zap(class vuslw_v_data *data)
{

if(data->kolih.getdlinna() <= 1 || data->cena_bnds.getdlinna() <= 1 \
 || data->shetu.getdlinna() <= 1 || data->ei.getdlinna() <= 1)
  {
   iceb_menu_soob(gettext("Не введены все обязательные реквизиты !"),data->window);
   return(1);
  }

struct OPSHET	shetv;
if(iceb_prsh1(data->shetu.ravno(),&shetv,data->window) != 0)
 return(1);

char strsql[512];

/*Проверяем единицу измерения*/
sprintf(strsql,"select kod from Edizmer where kod='%s'",data->ei.ravno());

if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не нашли единицу измерения"),data->ei.ravno());
   iceb_menu_soob(strsql,data->window);
   return(1);
 }

double cenabn=data->cena_bnds.ravno_atof();
cenabn=iceb_u_okrug(cenabn,okrcn);
if(cenabn < 0.01)
 {
   iceb_menu_soob(gettext("Не введена цена без НДС !"),data->window);
   return(1);
 }


if(data->shet_ps.getdlinna() > 1)
 if(iceb_prsh1(data->shet_ps.ravno(),&shetv,data->window) != 0)
  return(1);

if(data->nom_zap == -1) //Новая запись
 {
  if(zapuvdokw(data->tipz,data->dd,data->md,data->gd,data->nomdok.ravno(),data->metka_zap,
  data->kodzap.ravno_atoi(),data->kolih.ravno_atof(),cenabn,
  data->ei.ravno(),data->shetu.ravno(),data->podr,data->shet_ps.ravno(),"",data->window) != 0)
   return(1);
 }
else  //Корректировка старой записи
 {
  sprintf(strsql,"update Usldokum1 set \
kolih=%.10g,\
cena=%.10g,\
ei='%s',\
shsp='%s',\
shetu='%s' \
where datd='%04d-%d-%d' and podr=%d and nomd='%s' and metka=%d and \
kodzap=%d and tp=%d and nz=%d",
  data->kolih.ravno_atof(),
  cenabn,
  data->ei.ravno(),
  data->shet_ps.ravno(),
  data->shetu.ravno(),
  data->gd,data->md,data->dd,data->podr,data->nomdok.ravno(),data->metka_zap,data->kodzap.ravno_atoi(),
  data->tipz,data->nom_zap);
//  printf("vuslw_v-%s\n",strsql);
  if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
   return(1);
 }

return(0);
}
