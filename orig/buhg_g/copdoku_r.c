/*$Id: copdoku_r.c,v 1.14 2011-02-21 07:35:51 sasa Exp $*/
/*28.03.2008	19.10.2004	Белых А.И.	copdoku_r.c
Выполнение операции копирования документов в "Учёте услуг"
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "buhg_g.h"
#include "copdokus.h"

class copdokus_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  short prkk;
  
  class copdokus_data *rk;
    

 };

gboolean   copdokus_r_key_press(GtkWidget *widget,GdkEventKey *event,class copdokus_r_data *data);
gint copdokus_r1(class copdokus_r_data *data);
void  copdokus_r_v_knopka(GtkWidget *widget,class copdokus_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern iceb_u_str shrift_ravnohir;
 
void copdoku_r(class copdokus_data *rek_ras,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class copdokus_r_data data;
data.rk=rek_ras;

repl_s.plus(gettext("Если не делать привязки к карточкам, то одинаковые материалы\n\
с разных документов и разных карточек будут записаны одной записью с общим\n\
количеством и наибольшей ценой."));

if(rek_ras->tipz == 1)
  repl_s.plus(gettext("При выполнении привязки, привязка выполняется\n\
к карточкам с такою же ценой, или заводятся новые карточки."));

repl_s.plus(gettext("Сделать привязку к карточкам ?"));

data.prkk=iceb_menu_danet(&repl_s,2,wpredok);






data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Копировать записи из других документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(copdokus_r_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

GtkWidget *label=gtk_label_new(gettext("Копировать записи из других документов"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),450,300);

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста
PangoFontDescription *font_pango=pango_font_description_from_string(shrift_ravnohir.ravno());
gtk_widget_modify_font(GTK_WIDGET(data.view),font_pango);
pango_font_description_free(font_pango);

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
//gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_ALWAYS,GTK_POLICY_ALWAYS);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);

data.bar=gtk_progress_bar_new();

gtk_progress_bar_set_bar_style(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_CONTINUOUS);

gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_LEFT_TO_RIGHT);

gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);


gtk_widget_show_all(data.window);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
GtkTooltips *tooltops=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltops,data.knopka,gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(copdokus_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)copdokus_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  copdokus_r_v_knopka(GtkWidget *widget,class copdokus_r_data *data)
{
 printf("copdokus_r_v_knopka\n");
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   copdokus_r_key_press(GtkWidget *widget,GdkEventKey *event,class copdokus_r_data *data)
{
 printf("copdokus_r_key_press\n");

switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
}
return(TRUE);
}

/******************************************/
/******************************************/

gint copdokus_r1(class copdokus_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
char bros[512];
int kolstr;
iceb_u_str repl;
int koldok=0;
int kolsz=0;
short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  return(FALSE);
 }

int tipzz=data->rk->pr_ras.ravno_pr();
char nomdokp[50];
  
short mprd=0;
if(iceb_u_pole(data->rk->nomdok.ravno(),strsql,1,'*') == 0)
 {
  mprd=1;
  strcpy(nomdokp,strsql);
 }  
else
  strcpy(nomdokp,data->rk->nomdok.ravno());



int mud=iceb_menu_danet(gettext("Удалить документ после копирования ?"),2,data->window);
 
sprintf(strsql,"%s %d.%d.%d%s %s %d.%d.%d%s\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%d.%d.%d %s %d\n",data->rk->dd,data->rk->md,data->rk->gd,data->rk->nomdok_c.ravno(),data->rk->pod);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"select tp,datd,podr,kontr,nomd,kodop,sumkor \
from Usldokum where datd >= '%d-%02d-%02d' and datd <= '%d-%02d-%02d'",
gn,mn,dn,gk,mk,dk);
SQLCURSOR cur;
SQLCURSOR curr;
SQL_str row;
SQL_str row1;
SQL_str row2;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  repl.new_plus(gettext("Не найдено ни одного документа !"));
  iceb_menu_soob(&repl,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
//printw("nomdokp=%s\n",nomdokp);

koldok=kolsz=0;
float kolstr1=0.;
int tp=0;
short d,m,g;
int podr1=0;
char kodop[32];
double sumkor=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
/*
  printw("%s %s %s %s %s %s\n",
  row[0],row[1],row[2],row[3],row[4],row[5]);
  refresh();
*/  
  tp=atoi(row[0]);
  if(tipzz != 0 && tipzz != tp)
      continue;

  if(iceb_u_proverka(data->rk->kontr.ravno(),row[3],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->rk->kodop.ravno(),row[5],0,0) != 0)
   continue;


  if(iceb_u_proverka(nomdokp,row[4],mprd,0) != 0)
   continue;

  iceb_u_rsdat(&d,&m,&g,row[1],2);  
  podr1=atoi(row[2]);
  strncpy(kodop,row[5],sizeof(kodop)-1);

  /*Не копировать себя самого*/
  if(podr1 == data->rk->pod && iceb_u_SRAV(data->rk->nomdok_c.ravno(),row[4],0) == 0 && 
  data->rk->gd == g && data->rk->md == m && data->rk->dd == d && tp == data->rk->tipz)
    continue;

  memset(bros,'\0',sizeof(bros));
  if(tp == 1)
   strcpy(bros,"+");
  if(tp == 2)
   strcpy(bros,"-");

  sprintf(strsql,"%s %02d.%02d.%d %-3s %-4s %-5s %s\n",
  bros,d,m,g,row[6],row[3],row[4],row[2]);

  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  
  /*Читаем сумму корректировки*/

  sumkor=atof(row[6]);

  if(sumkor != 0)
   {

    sprintf(strsql,"update Usldokum set sumkor=sumkor+%.2f where \
datd='%s' and nomd='%s' and podr=%s and tp=%s",
    sumkor,row[1],row[4],row[2],row[0]);

    iceb_sql_zapis(strsql,1,0,data->window);
    
   }  


  sprintf(strsql,"select metka,kodzap,kolih,cena,ei,shetu,shsp from Usldokum1 \
where datd='%s' and  nomd='%s' and podr=%s and tp=%s",
    row[1],row[4],row[2],row[0]);
//  printw("%s\n",strsql);
//  OSTANOV();
  SQLCURSOR cur1;
  int kolstr2;
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  if(kolstr2 == 0)
    continue;
         
  koldok++;
  while(cur1.read_cursor(&row1) != 0)
   {
    int metka=atoi(row1[0]);
    int kodzap=atoi(row1[1]);
    double kolih=atof(row1[2]);
    double cena=atof(row1[3]);
    double cenaz=0.;
    char ei[32];
    strncpy(ei,row1[4],sizeof(ei)-1);
    char shetu[30];
    strncpy(shetu,row1[5],sizeof(shetu)-1);
    kolsz++;
       
    /*Проверяем нет ли уже такой записи*/
    sprintf(strsql,"select kolih,cena from Usldokum1 where \
datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and metka=%d and \
kodzap=%d and tp=%d and shetu='%s'",
    data->rk->gd,data->rk->md,data->rk->dd,data->rk->pod,data->rk->nomdok_c.ravno(),
    metka,kodzap,data->rk->tipz,shetu);
      
    if(iceb_sql_readkey(strsql,&row2,&curr,data->window) == 1)
     {
      kolih+=atof(row2[0]);
      cenaz=atof(row2[1]);
/*
      printw("Перезаписываем %d.%d.%d %s %d\n",dnk,mnk,gnk,nomdok,podr);
      refresh();  
*/
      if(fabs(cenaz-cena) > 0.009)
       {
        sprintf(strsql,"%s %s %s %d.%d.%d, %d. %s %.10g %.10g\n\%s.\n",
        gettext("Документ"),
        row[4],
        gettext("от"),
        d,m,g,kodzap,
        gettext("Не совпадает цена !"),
        cena,cenaz,
        gettext("Берем большую цену"));

        iceb_menu_soob(strsql,data->window);        

        if(cena-cenaz < 0.)
         cena=cenaz;
       }
      
      sprintf(strsql,"update Usldokum1 \
set \
cena=%.10g,\
kolih=%.10g,\
ktoi=%d \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and metka=%d \
and kodzap=%d and tp=%d",
      cena,kolih,iceb_getuid(data->window),data->rk->gd,data->rk->md,data->rk->dd,data->rk->pod,data->rk->nomdok_c.ravno(),
      metka,kodzap,data->rk->tipz);

      iceb_sql_zapis(strsql,1,0,data->window);

     }
    else
     {
/*
      printw("Пишем %d.%d.%d %s %d %d\n",dnk,mnk,gnk,nomdok,podr,nk);
      refresh();  
*/
      zapuvdokw(data->rk->tipz,data->rk->dd,data->rk->md,data->rk->gd,data->rk->nomdok_c.ravno(),
      metka,kodzap,kolih,cena,ei,shetu,data->rk->pod,row1[6],"",data->window);

     }


   }

  /*Удаление после копирования*/
  if(mud == 1)
   {
    if(iceb_pblok(m,g,ICEB_PS_UU,data->window) != 0)
     {
      sprintf(strsql,gettext("Документ %s удалить невозможно. Дата %d.%dг. заблокирована !"),
      row[4],m,g);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
    if(iceb_udprgr(gettext("УСЛ"),d,m,g,row[4],podr1,tp,data->window) != 0)
     continue;
    uduvdokw(tp,d,m,g,row[4],podr1,0,0,0,data->window);

   } 


 }


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

sprintf(strsql,"%s %d !",gettext("Количество переписаних документов"),koldok);
repl.new_plus(strsql);
sprintf(strsql,"%s %d !",gettext("Количество переписаних записей"),kolsz);
repl.ps_plus(strsql);
iceb_menu_soob(&repl,data->window);

return(FALSE);
}
