/*$Id: l_zar_dok_hap.c,v 1.18 2011-02-21 07:35:54 sasa Exp $*/
/*10.03.2009	26.10.2006	Белых А.И.	l_zar_dok_hap.c
Ввод и корректировка шапки документа в "Заработной плате"
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"

enum
 {
  E_DATAD,
  E_NOMDOK,
  E_PODR,
  E_KOMENT,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

class l_zar_dok_hap_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str datad_k;
  class iceb_u_str nomdok_k;
  class iceb_u_str podr_k;
  
  class iceb_u_str datad;
  class iceb_u_str nomdok;
  class iceb_u_str podr;
  class iceb_u_str koment;
  short prn;
      
  l_zar_dok_hap_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_rek();
    podr_k.new_plus("");
   }

  void read_rek()
   {
    datad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAD]))));
    nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK]))));
    podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PODR]))));
    koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT]))));
   }

  void clear_rek()
   {
    datad.new_plus("");
    nomdok.new_plus("");
    podr.new_plus("");
    koment.new_plus("");
   }
 };


gboolean   l_zar_dok_hap_key_press(GtkWidget *widget,GdkEventKey *event,class l_zar_dok_hap_data *data);
void  l_zar_dok_hap_knopka(GtkWidget *widget,class l_zar_dok_hap_data *data);
void    l_zar_dok_hap_vvod(GtkWidget *widget,class l_zar_dok_hap_data *data);
int l_zar_dok_hap_zap(class l_zar_dok_hap_data *data);

int zapzardkw(short dd,short md,short gd,const char *nomdok,int podr,const char *koment,short prn,GtkWidget *wpredok);
void  l_zar_dok_hap_e_knopka(GtkWidget *widget,class l_zar_dok_hap_data *data);

extern SQL_baza  bd;
extern char      *name_system;

int l_zar_dok_hap(class iceb_u_str *datad,class iceb_u_str *nomdok,short prn,GtkWidget *wpredok)
{

class l_zar_dok_hap_data data;
char strsql[512];
iceb_u_str kikz;

data.datad_k.new_plus(datad->ravno());
data.datad.new_plus(datad->ravno());
data.nomdok_k.new_plus(nomdok->ravno());
data.nomdok.new_plus(nomdok->ravno());
data.prn=prn;

if(data.datad_k.getdlinna() >  1)
 {
  sprintf(strsql,"select pd,koment,ktoi,vrem from Zardok where datd='%s' and nomd='%s'",
  data.datad_k.ravno_sqldata(),data.nomdok.ravno());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.podr.new_plus(row[0]);
  data.podr_k.new_plus(row[0]);
  data.koment.new_plus(row[1]);
  kikz.plus(iceb_kikz(row[2],row[3],wpredok));
  
 }
else
 {
  short dt,mt,gt;
  iceb_u_poltekdat(&dt,&mt,&gt);
  sprintf(strsql,"%d.%d.%d",dt,mt,gt);
  data.datad.new_plus(strsql);
  data.nomdok.new_plus(nomdokw(gt,"Zardok",wpredok));
 } 

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.datad_k.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));
  label=gtk_label_new(gettext("Ввод нового документа"));
 }
else
 {
  sprintf(strsql,"%s %s",name_system,gettext("Просмотр шапки документа"));
  iceb_u_str repl;
  repl.plus(gettext("Просмотр шапки документа"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno_toutf());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_zar_dok_hap_key_press),&data);

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
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


//label=gtk_label_new(gettext("Код"));
//gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), label, FALSE, FALSE, 0);
GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Дата документа"));
data.knopka_enter[E_DATAD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.knopka_enter[E_DATAD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAD]),"clicked",GTK_SIGNAL_FUNC(l_zar_dok_hap_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAD]),(gpointer)E_DATAD);
tooltips_enter[E_DATAD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAD],data.knopka_enter[E_DATAD],gettext("Выбор даты"),NULL);

data.entry[E_DATAD] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.entry[E_DATAD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAD]), "activate",GTK_SIGNAL_FUNC(l_zar_dok_hap_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAD]),data.datad.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAD]),(gpointer)E_DATAD);

label=gtk_label_new(gettext("Номер документа"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), label, FALSE, FALSE, 0);

data.entry[E_NOMDOK] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMDOK]), "activate",GTK_SIGNAL_FUNC(l_zar_dok_hap_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.nomdok.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMDOK]),(gpointer)E_NOMDOK);


sprintf(strsql,"%s",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODR]),"clicked",GTK_SIGNAL_FUNC(l_zar_dok_hap_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODR]),(gpointer)E_PODR);
tooltips_enter[E_PODR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_PODR],data.knopka_enter[E_PODR],gettext("Выбор подразделения"),NULL);

data.entry[E_PODR] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODR]), "activate",GTK_SIGNAL_FUNC(l_zar_dok_hap_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.podr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODR]),(gpointer)E_PODR);



label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(l_zar_dok_hap_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);

GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],data.knopka[PFK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK2]),"clicked",GTK_SIGNAL_FUNC(l_zar_dok_hap_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK2]),(gpointer)PFK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],data.knopka[PFK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK10]),"clicked",GTK_SIGNAL_FUNC(l_zar_dok_hap_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK10]),(gpointer)PFK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0)
 {
  datad->new_plus(data.datad.ravno());
  nomdok->new_plus(data.nomdok.ravno());
 }
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_zar_dok_hap_e_knopka(GtkWidget *widget,class l_zar_dok_hap_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATAD:

    if(iceb_calendar(&data->datad,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAD]),data->datad.ravno());
      
    return;  


  case E_PODR:

    if(l_zarpodr(1,&kod,&naim,data->window) == 0)
     {
      data->podr.plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->podr.ravno_toutf());
     }

    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zar_dok_hap_key_press(GtkWidget *widget,GdkEventKey *event,class l_zar_dok_hap_data *data)
{
//char  bros[512];

//printf("l_zar_dok_hap_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PFK2]),"clicked");

    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PFK10]),"clicked");

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
void  l_zar_dok_hap_knopka(GtkWidget *widget,class l_zar_dok_hap_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_zar_dok_hap_knopka knop=%d\n",knop);

switch (knop)
 {
  case PFK2:
    data->read_rek();
    if(l_zar_dok_hap_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  


  case PFK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_zar_dok_hap_vvod(GtkWidget *widget,class l_zar_dok_hap_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_zar_dok_hap_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAD:
    data->datad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NOMDOK:
    data->nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_PODR:
    data->podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KOMENT:
    data->koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int l_zar_dok_hap_zap(class l_zar_dok_hap_data *data)
{

char strsql[1024];
//SQL_str row;
time_t   vrem;
time(&vrem);
short dd,md,gd;

if(iceb_u_rsdat(&dd,&md,&gd,data->datad.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата документа !"),data->window);
  return(1);
 }

//Проверяем код подразделения
if(data->podr.ravno_atoi() > 0)
 {
  sprintf(strsql,"select naik from Podr where kod=%d",data->podr.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код подразделения !"),data->podr.ravno_atoi());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }
//проверяем вновь введённую дату
if(iceb_pbpds(md,gd,data->window) != 0)
 return(1);


short dk=0,mk=0,gk=0;
if(data->datad_k.getdlinna() > 1)
 {
  iceb_u_rsdat(&dk,&mk,&gk,data->datad_k.ravno(),1);
  
  if(l_nahud_prov_blok(mk,gk,0,data->window) != 0)
   return(1);;
 }

class iceb_lock_tables blok_tabl("LOCK TABLE Zardok WRITE,icebuser READ");

if(gk != gd || iceb_u_SRAV(data->nomdok_k.ravno(),data->nomdok.ravno(),0) != 0 )
 {  
  //Проверяем может документ с таким номером уже есть 
  sprintf(strsql,"select god from Zardok where nomd='%s' and god=%d",data->nomdok.ravno(),gd);
  if(iceb_sql_readkey(strsql,data->window) > 0)
   {
    iceb_menu_soob(gettext("Документ с таким номером уже есть !"),data->window);
    return(1);
   }
 }

if(data->datad_k.getdlinna() <= 1)
 {

  if(zapzardkw(dd,md,gd,data->nomdok.ravno(),data->podr.ravno_atoi(),data->koment.ravno(),data->prn,data->window) != 0)
    return(1);
 }
else //корректировка старой
 {
  time_t vrem;
  time(&vrem);
  
  
  sprintf(strsql,"update Zardok set \
god=%d,\
datd='%04d-%02d-%02d',\
nomd='%s',\
pd=%d,\
koment='%s',\
ktoi=%d,\
vrem=%ld,\
td=%d \
where god=%d and nomd='%s'",
  gd,
  gd,md,dd,
  data->nomdok.ravno(),
  data->podr.ravno_atoi(),
  data->koment.ravno_filtr(),
  iceb_getuid(data->window),
  vrem,
  data->prn,
  gk,data->nomdok_k.ravno());  


  if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
    return(1);
  
  blok_tabl.unlock();
  
  SQL_str row;
  class SQLCURSOR cur;
  
  //Корректируем записи в документе
  if(iceb_u_sravmydat(dk,mk,gk,dd,md,gd) != 0 || iceb_u_SRAV(data->nomdok_k.ravno(),data->nomdok.ravno(),0) != 0 )
   {
    sprintf(strsql,"select datz,tabn,prn,knah,godn,mesn,podr,shet,nomz from Zarp where \
datz='%04d-%02d-%02d' and nd='%s' and prn='%d'",gk,mk,dk,data->nomdok_k.ravno(),data->prn);
    int kolstr=0;
    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
        iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

    if(kolstr > 0)
     while(cur.read_cursor(&row) != NULL)
      {
       for(int nomer=atoi(row[8]); nomer < 1000; nomer++)
        {          
         sprintf(strsql,"update Zarp set \
datz='%04d-%02d-%02d',\
nd='%s',\
nomz=%d \
where datz='%s' and tabn=%s and prn='%s' and knah=%s and godn=%s and mesn=%s \
and podr=%s and shet='%s' and nomz=%s",
         gd,md,dd,
         data->nomdok.ravno(),
         nomer,
         row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8]);
            
         if(sql_zap(&bd,strsql) == 0)
          break; 
        }

      }


   if(iceb_u_sravmydat(dk,mk,gk,dd,md,gd) != 0 ) //Корректируем проводки
    {
     //дата и номер документа уже изменены
     sprintf(strsql,"select distinct tabn from Zarp where datz='%04d-%02d-%02d' and nd='%s' and prn='%d'",
     gd,md,dd,data->nomdok.ravno(),data->prn);

     if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
        iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
     if(kolstr > 0)
      {
       while(cur.read_cursor(&row) != 0)
        {          
         zarsocw(md,gd,atoi(row[0]),NULL,data->window);
         zaravprw(atoi(row[0]),md,gd,NULL,data->window);
         if(mk != md || gk != gd)
          {
           zarsocw(mk,gk,atoi(row[0]),NULL,data->window);
           zaravprw(atoi(row[0]),mk,gk,NULL,data->window);
          }
        }
      }         
    }

   }
 }


return(0);

}
