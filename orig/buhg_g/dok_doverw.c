/*$Id: dok_doverw.c,v 1.23 2011-02-21 07:35:51 sasa Exp $*/
/*17.05.2010	18.03.2004	Белых А.И.	dok_doverw.c
Печать доверенности
*/
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include "buhg_g.h"
#include <unistd.h>
enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAV,
  E_DATA_DO,
  E_TABNOM,
  E_POSTAVHIK,
  E_DOKUM,
  E_NOMER_DOV,
  KOLENTER  
 };

class dok_doverw_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_fio;
  short kl_shift;
  
  class iceb_u_str datav;
  class iceb_u_str data_do;
  class iceb_u_str tabnom;
  class iceb_u_str postavhik;
  class iceb_u_str dokum;
  class iceb_u_str nomer_dov;  
  class iceb_u_str fio;
  
  //реквизиты организации
  class iceb_u_str inn;
  class iceb_u_str nsv;
  class iceb_u_str naim;
  class iceb_u_str naimbank;
  class iceb_u_str adres;
  class iceb_u_str adresban;
  class iceb_u_str kod;
  class iceb_u_str rasshet;
  class iceb_u_str mfo;
  class iceb_u_str telefon;
  dok_doverw_data() //Конструктор
   {
    clear_data();
    kl_shift=0;
  
    inn.plus("");
    nsv.plus("");
    naim.plus("");
    adres.plus("");
    adresban.plus("");
    naimbank.plus("");
    kod.plus("");
    rasshet.plus("");
    mfo.plus("");
    telefon.new_plus("");
   }
  
  void clear_data()
   {
    datav.new_plus("");
    data_do.new_plus("");
    tabnom.new_plus("");
    postavhik.new_plus("");
    dokum.new_plus("");
    nomer_dov.new_plus("");  
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    clear_data();
    gtk_label_set_text(GTK_LABEL(label_fio),"");
   }
 };


gboolean   dok_doverw_v_key_press(GtkWidget *widget,GdkEventKey *event,class dok_doverw_data *data);
void    dok_doverw_v_vvod(GtkWidget *widget,class dok_doverw_data *data);
void  dok_doverw_v_knopka(GtkWidget *widget,class dok_doverw_data *data);
void  dok_doverw_v_e_knopka(GtkWidget *widget,class dok_doverw_data *data);

void dok_doverw_ras(class dok_doverw_data *data);

void dok_dover_rl(const char *nomer_dov,short dv,short mv,short gv,short dd,short md,short gd,short dvd,short mvd,short gvd,const char *fio,
const char *seriq,const char *nomer,const char *vid,const char *naim,const char *naimbank,const char *kod,const char *adres,const char *rasshet,const char *mfo,const char *post,const char *inn,
const char *nsv,const char *telefon,const char *ndok,const char *imaf,GtkWidget *wpredok);


extern char *name_system;
extern SQL_baza bd;

void   dok_doverw()
{
SQL_str row;
SQLCURSOR cur;
char strsql[512];
class dok_doverw_data data;
data.datav.poltekdat();

sprintf(strsql,"select * from Kontragent where kodkon='00'");
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  data.naim.new_plus(row[1]);
  data.adres.new_plus(row[3]);
  data.kod.new_plus(row[5]);
  data.inn.new_plus(row[8]);
  data.nsv.new_plus(row[9]);
  data.mfo.new_plus(row[6]);
  data.rasshet.new_plus(row[7]);
  data.naimbank.new_plus(row[2]);

  data.adresban.new_plus(row[4]);

  if(iceb_u_polen(row[10],&data.telefon,1,' ') != 0)
   data.telefon.new_plus(row[10]);

 }


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Выписка доверенностей"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(dok_doverw_v_key_press),&data);

data.label_fio=gtk_label_new("");
GtkWidget *label=gtk_label_new(gettext("Выписка доверенностей"));

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

sprintf(strsql,"%s (%s)",gettext("Дата выдачи доверенности"),gettext("д.м.г"));
data.knopka_enter[E_DATAV]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAV]), data.knopka_enter[E_DATAV], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAV]),"clicked",GTK_SIGNAL_FUNC(dok_doverw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAV]),(gpointer)E_DATAV);
tooltips_enter[E_DATAV]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAV],data.knopka_enter[E_DATAV],gettext("Выбор даты"),NULL);

data.entry[E_DATAV] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAV]), data.entry[E_DATAV], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAV]), "activate",GTK_SIGNAL_FUNC(dok_doverw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAV]),data.datav.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAV]),(gpointer)E_DATAV);


sprintf(strsql,"%s (%s)",gettext("Действительно до"),gettext("д.м.г"));
data.knopka_enter[E_DATA_DO]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_DO]), data.knopka_enter[E_DATA_DO], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_DO]),"clicked",GTK_SIGNAL_FUNC(dok_doverw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_DO]),(gpointer)E_DATA_DO);
tooltips_enter[E_DATA_DO]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_DO],data.knopka_enter[E_DATA_DO],gettext("Выбор даты"),NULL);

data.entry[E_DATA_DO] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_DO]), data.entry[E_DATA_DO], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_DO]), "activate",GTK_SIGNAL_FUNC(dok_doverw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_DO]),data.data_do.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_DO]),(gpointer)E_DATA_DO);


sprintf(strsql,"%s",gettext("Табельный номер"));
data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_TABNOM]),"clicked",GTK_SIGNAL_FUNC(dok_doverw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_TABNOM]),(gpointer)E_TABNOM);
tooltips_enter[E_TABNOM]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_TABNOM],data.knopka_enter[E_TABNOM],gettext("Выбор табельного номера"),NULL);

data.entry[E_TABNOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TABNOM]), "activate",GTK_SIGNAL_FUNC(dok_doverw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.tabnom.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TABNOM]),(gpointer)E_TABNOM);

gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.label_fio, TRUE, TRUE, 0);


sprintf(strsql,"%s",gettext("Поставщик"));
data.knopka_enter[E_POSTAVHIK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_POSTAVHIK]), data.knopka_enter[E_POSTAVHIK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_POSTAVHIK]),"clicked",GTK_SIGNAL_FUNC(dok_doverw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_POSTAVHIK]),(gpointer)E_POSTAVHIK);
tooltips_enter[E_POSTAVHIK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_POSTAVHIK],data.knopka_enter[E_POSTAVHIK],gettext("Выбор поставщика"),NULL);

data.entry[E_POSTAVHIK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_POSTAVHIK]), data.entry[E_POSTAVHIK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_POSTAVHIK]), "activate",GTK_SIGNAL_FUNC(dok_doverw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_POSTAVHIK]),data.postavhik.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_POSTAVHIK]),(gpointer)E_POSTAVHIK);


sprintf(strsql,"%s",gettext("По документу"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DOKUM]), label, FALSE, FALSE, 0);

data.entry[E_DOKUM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_DOKUM]), data.entry[E_DOKUM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DOKUM]), "activate",GTK_SIGNAL_FUNC(dok_doverw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DOKUM]),data.dokum.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DOKUM]),(gpointer)E_DOKUM);

sprintf(strsql,"%s",gettext("Номер доверенности"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_DOV]), label, FALSE, FALSE, 0);

data.entry[E_NOMER_DOV] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_DOV]), data.entry[E_NOMER_DOV], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMER_DOV]), "activate",GTK_SIGNAL_FUNC(dok_doverw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMER_DOV]),data.dokum.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMER_DOV]),(gpointer)E_NOMER_DOV);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Печать"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Распечатать доверенность"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(dok_doverw_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(dok_doverw_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(dok_doverw_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[1]);

gtk_widget_show_all (data.window);

gtk_main();


}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  dok_doverw_v_e_knopka(GtkWidget *widget,class dok_doverw_data *data)
{
class iceb_u_str kod("");
class iceb_u_str fio("");
char strsql[512];
SQL_str row;
SQLCURSOR cur;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case E_DATAV:

    if(iceb_calendar(&data->datav,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAV]),data->datav.ravno());
      
    return;  

  case E_DATA_DO:

    if(iceb_calendar(&data->data_do,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_DO]),data->data_do.ravno());
      
    return;  
  
  case E_TABNOM:
    if(l_sptbn1(&kod,&fio,0,data->window) == 0)
     {
      data->tabnom.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->tabnom.ravno());

      gtk_label_set_text(GTK_LABEL(data->label_fio),fio.ravno_toutf(20));
     }
    return;  

  case E_POSTAVHIK:
    if(iceb_vibrek(1,"Kontragent",&kod,data->window) == 0)
     {
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kod.ravno());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {      
        data->postavhik.new_plus(row[0]);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_POSTAVHIK]),data->postavhik.ravno_toutf());
       }
     }
    return;
   
 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   dok_doverw_v_key_press(GtkWidget *widget,GdkEventKey *event,class dok_doverw_data *data)
{

//printf("dok_doverw_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);

  case GDK_F3:
//    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
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
void  dok_doverw_v_knopka(GtkWidget *widget,class dok_doverw_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dok_doverw_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

   dok_doverw_ras(data);
        
    return;  

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    dok_doverw_v_vvod(GtkWidget *widget,class dok_doverw_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dok_doverw_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAV:
    data->datav.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATA_DO:
    data->data_do.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_TABNOM:
    data->tabnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(data->tabnom.getdlinna() > 1)
     {
      sprintf(strsql,"select fio from Kartb where tabn=%d",data->tabnom.ravno_atoi());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
        gtk_label_set_text(GTK_LABEL(data->label_fio),iceb_u_toutf(row[0]));
     }
    break;

  case E_POSTAVHIK:
    data->postavhik.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    if(isdigit(data->postavhik.ravno()[0]) != 0)
     {
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->postavhik.ravno());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {
        data->postavhik.new_plus(row[0]);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_POSTAVHIK]),data->postavhik.ravno_toutf());
       }
     }
    break;

  case E_DOKUM:
    data->dokum.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOMER_DOV:
    data->nomer_dov.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/*******************************************/
/*Печать доверенности*/
/*********************************/

void dok_doverw_ras(class dok_doverw_data *data)
{

short d1,m1,g1;
short d2,m2,g2;
short d3,m3,g3;

if(iceb_u_rsdat(&d1,&m1,&g1,data->datav.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата выдачи доверенности !"),data->window);
  return;
 }
if(iceb_u_rsdat(&d2,&m2,&g2,data->data_do.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата действительно до !"),data->window);
  return;
 }
if(data->tabnom.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён табельный номер!"),data->window);
  return;
 }
SQL_str row;
SQLCURSOR cur;

char mesqc[20];
char mesd[20];

iceb_mesc(m1,1,mesqc);
iceb_mesc(m2,1,mesd);
char fio[512];
char vid[512];
char dvd[512];

memset(fio,'\0',sizeof(fio));
memset(vid,'\0',sizeof(vid));
memset(dvd,'\0',sizeof(dvd));
char seriq[5];
char nomer[32];
char strsql[512];

sprintf(strsql,"select * from Kartb where tabn=%d",data->tabnom.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден табельный номер"),data->tabnom.ravno());
  iceb_menu_soob(strsql,data->window);
  return;
 }
else
 {
  strncpy(fio,row[1],sizeof(fio)-1);
  
  iceb_u_polen(row[12],seriq,sizeof(seriq),1,' ');
  iceb_u_polen(row[12],nomer,sizeof(nomer),2,' ');

  strncpy(vid,row[13],sizeof(vid)-1);
  strncpy(dvd,row[19],sizeof(dvd)-1);  
 }
//sprintf(iff,"dow%d.tmp",getpid());
char imaf[56];

sprintf(imaf,"dov%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }	
iceb_u_startfil(ff);

char str[1024];  

fprintf(ff,"\x1b\x6C%c",8); /*Установка левого поля*/
fprintf(ff,"\x1B\x4D"); //12 знаков на дюйм
//fprintf(ff,"\x1B\x45"); /*Включение режима выделенной печати*/

fprintf(ff,"                               IПH - %s  NCB - %s тел.%.*s",
data->inn.ravno(),data->nsv.ravno(),
iceb_u_kolbait(15,data->telefon.ravno()),data->telefon.ravno());  

fprintf(ff,"\x1B\x33%c\n",25);   
fprintf(ff,"\n");

fprintf(ff,"  %s",data->naim.ravno());
fprintf(ff,"\x1B\x33%c\n",33);   

fprintf(ff,"\n");

if(strlen(data->adres.ravno()) > 50)
 {
  fprintf(ff,"\x0F");  /*Ужатый режим*/
  fprintf(ff,"  %s",data->adres.ravno());  
  fprintf(ff,"\x12\n");  /*Нормальный режим печати*/
 }
else
 fprintf(ff,"  %s\n",data->adres.ravno());  

fprintf(ff,"%s%s\n","                       ",data->kod.ravno());  
fprintf(ff,"  %s\n",data->naim.ravno());
fprintf(ff,"\n");  

if(strlen(data->adres.ravno()) > 50)
 {
  fprintf(ff,"\x0F");  //Ужатый режим
  fprintf(ff,"  %s",data->adres.ravno());  
  fprintf(ff,"\x12\n");  //Нормальный режим печати
 }
else
 fprintf(ff,"  %s",data->adres.ravno());    

fprintf(ff,"\x1B\x33%c\n",34); 
fprintf(ff,"         %s         %s\n",data->rasshet.ravno(),data->mfo.ravno());  

sprintf(str,"%s %s",data->naimbank.ravno(),data->adresban.ravno());    
fprintf(ff,"  %-*s %02d %-*s  %02d\n",
iceb_u_kolbait(57,str),str,
d2,
iceb_u_kolbait(12,mesd),mesd,
g2-2000);

fprintf(ff,"\x1B\x33%c\n",44);   
fprintf(ff,"\n");    
fprintf(ff,"\n");    
fprintf(ff,"\n");    
sprintf(str,"                                    %02d %-*s %02d",d1,iceb_u_kolbait(13,mesqc),mesqc,g1-2000);
fprintf(ff,"%s\n",str);    
fprintf(ff,"\n");    
sprintf(str,"            %s",fio);
fprintf(ff,"%s",str);    
fprintf(ff,"\x1B\x33%c\n",40);
fprintf(ff,"\n");
sprintf(str,"                               Паcпорт");  
fprintf(ff,"%s",str);
fprintf(ff,"\x1B\x33%c\n",40);  
fprintf(ff,"\n");

iceb_u_rsdat(&d3,&m3,&g3,dvd,2);

char naz_m[40];
memset(naz_m,'\0',sizeof(naz_m));

iceb_mesc(m3,1,naz_m);

fprintf(ff,"           %s           %s          %02d %s %04d\n",seriq,nomer,d3,naz_m,g3);

fprintf(ff,"\n");  
sprintf(str,"            %s",vid);
fprintf(ff,"%s\n",str); 
fprintf(ff,"\x1B\x33%c\n",30);    
sprintf(str,"                    %s",data->postavhik.ravno());
fprintf(ff,"%s\n",str);   
fprintf(ff,"\n"); 
//  fprintf(ff,"\x1B\x33%c\n",28);    
sprintf(str,"                 %s",data->dokum.ravno());
fprintf(ff,"%s\n",str);     

fprintf(ff,"\x1B\x48"); /*Выключение режима двойного удара*/

fclose(ff);

char imaf_l[50];

sprintf(imaf_l,"dovl%d.lst",getpid());
dok_dover_rl(data->nomer_dov.ravno(),d1,m1,g1,d2,m2,g2,d3,m3,g3,fio,seriq,nomer,vid,
data->naim.ravno(),data->naimbank.ravno(),data->kod.ravno(),data->adres.ravno(),data->rasshet.ravno(),data->mfo.ravno(),
data->postavhik.ravno(),data->inn.ravno(),data->nsv.ravno(),data->telefon.ravno(),data->dokum.ravno(),imaf_l,data->window);

class iceb_u_spisok imaf_r;
class iceb_u_spisok naimf_r;

imaf_r.plus(imaf_l);
naimf_r.plus(gettext("Распечатка доверенности"));

imaf_r.plus(imaf);
naimf_r.plus(gettext("Распечатка доверенности на бланке строгой отчётности"));

iceb_rabfil(&imaf_r,&naimf_r,"",0,data->window);

}
/************************/
/*Печать доверенности на листе бумаги*/
/***********************************/
void dok_dover_rl(const char *nomer_dov,
short dv,short mv,short gv,
short dd,short md,short gd,
short dvd,short mvd,short gvd,
const char *fio,
const char *seriq,
const char *nomer,
const char *vid,
const char *naim,
const char *naimbank,
const char *kod,
const char *adres,
const char *rasshet,
const char *mfo,
const char *post,
const char *inn,const char *nsv,const char *telefon,
const char *ndok,
const char *imaf,
GtkWidget *wpredok)
{
char mesqc[24];

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='dok_dover_r.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"dok_dover_r.alx");
  iceb_menu_soob(strsql,wpredok);
  return;
 }

FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }	
iceb_u_startfil(ff);
fprintf(ff,"\x1b\x6C%c",6); /*Установка левого поля*/
fprintf(ff,"\x1B\x4D"); /*12-знаков*/

fprintf(ff,"                                IПH - %s  NCB - %s тел.%.*s\n",inn,nsv,iceb_u_kolbait(15,telefon),telefon);  

char bros[512];
char stroka[1024];
int nomer_str=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomer_str++;
  memset(stroka,'\0',sizeof(stroka));
  strncpy(stroka,row_alx[0],sizeof(stroka)-1);
  switch(nomer_str)
   {
    case 1:
     iceb_u_bstab(stroka,naim,0,54,1);
     break;

    case 3:
     iceb_u_bstab(stroka,adres,0,47,1);
     if(strlen(adres) > 47);
      {
       fprintf(ff,"%s",stroka);
       memset(stroka,'\0',sizeof(stroka));
       for(int kk=47; kk < (int) strlen(adres); kk+=47)
        {
         fprintf(ff,"%s\n",&adres[kk]);
        }     
       continue;
      }
     break;

    case 4:
     iceb_u_bstab(stroka,kod,28,54,1);
     break;

    case 5:
     iceb_u_bstab(stroka,naim,0,54,1);
     break;

    case 7:
     iceb_u_bstab(stroka,adres,0,47,1);
     if(strlen(adres) > 47);
      {
       fprintf(ff,"%s",stroka);
       memset(stroka,'\0',sizeof(stroka));
       for(int kk=47; kk < (int) strlen(adres); kk+=47)
        {
         fprintf(ff,"%s\n",&adres[kk]);
        }     
       continue;
      }
     break;

    case 8:
     iceb_u_bstab(stroka,rasshet,8,29,1);
     iceb_u_bstab(stroka,mfo,33,42,1);
     break;

    case 9:
     iceb_u_bstab(stroka,naimbank,0,54,1);
     memset(mesqc,'\0',sizeof(mesqc));
     iceb_mesc(md,1,mesqc);
     sprintf(bros,"%02d %s %d %s",dd,mesqc,gd,gettext("г."));
     iceb_u_bstab(stroka,bros,67,91,1);
        
     break;

    case 13:
     iceb_u_bstab(stroka,nomer_dov,43,64,1);
     break;

    case 14:
     memset(mesqc,'\0',sizeof(mesqc));
     iceb_mesc(mv,1,mesqc);
     sprintf(bros,"%02d %s %d %s",dv,mesqc,gv,gettext("г."));
     iceb_u_bstab(stroka,bros,34,70,1);
        
     break;

    case 16:
     iceb_u_bstab(stroka,fio,7,92,1);
     break;

    case 20:
     iceb_u_bstab(stroka,seriq,6,17,1);
     iceb_u_bstab(stroka,nomer,19,37,1);
     sprintf(bros,"%02d.%02d.%d %s",dvd,mvd,gvd,gettext("г."));
     iceb_u_bstab(stroka,bros,41,92,1);

     break;


    case 22:
     iceb_u_bstab(stroka,vid,8,92,1);
     break;

    case 24:
     iceb_u_bstab(stroka,post,17,92,1);
     break;

    case 26:
     iceb_u_bstab(stroka,ndok,13,92,1);
     break;
   }
   
  fprintf(ff,"%s",stroka);
  
 }


fclose(ff);


}
