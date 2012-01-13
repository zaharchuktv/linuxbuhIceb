/*$Id: go_ss_nbs.c,v 1.19 2011-02-21 07:35:51 sasa Exp $*/
/*17.06.2006	26.03.2004	Белых А.И.	go_ss_nbs.c
Журнал-ордер для небалансовых счетов со свернутым сальдо
*/
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "go.h"

class go_ss_nbs_r_data
 {
  public:
  
  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  gfloat kolstr1; //Количество выполненных проходов
  int    kolstr;  //Количество проходов при расчете
  short     prohod;
  iceb_u_spisok imaf;
  iceb_u_spisok naim;

  go_rr *rek_r;
  short dn,mn,gn;
  short dk,mk,gk;
  short  godn;
  time_t vremn;
  
  short d2,m2,g2;
  double sdeb,skre;
  double odeb,okre;
  double sndeb,snkre;
  short metkasaldo;
  FILE  *ff;
  //Конструктор  
  go_ss_nbs_r_data()
   {
    kolstr1=0.;
    kolstr=9;
    prohod=0;
    sdeb=skre=0.;
    odeb=okre=0.;
    sndeb=snkre=0.;
    metkasaldo=0;
   }

 };

gboolean   go_ss_nbs_r_key_press(GtkWidget *widget,GdkEventKey *event,class go_ss_nbs_r_data *data);
gint go_ss_nbs_r1(class go_ss_nbs_r_data *data);
void  go_ss_nbs_r_v_knopka(GtkWidget *widget,class go_ss_nbs_r_data *data);

 
extern SQL_baza bd;
extern char *organ;
extern char *name_system;
extern short vplsh; /*0-двух порядковый план счетов 1-многпорядковый*/
extern short	startgodb; /*Стартовый год*/
extern short    koolk; /*Корректор отступа от левого края*/
extern iceb_u_str shrift_ravnohir;

void go_ss_nbs(class go_rr *data_rr)
{
go_ss_nbs_r_data data;

data.rek_r=data_rr;

char strsql[512];
iceb_u_str soob;



printf("go_ss_nbs\n");
if(iceb_rsdatp(&data.dn,&data.mn,&data.gn,data.rek_r->datan.ravno(),
&data.dk,&data.mk,&data.gk,data.rek_r->datak.ravno(),NULL) != 0)
  return;
  
data.godn=startgodb;
if(startgodb == 0 || startgodb > data.gn)
 data.godn=data.gn;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Журнал ордер"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(go_ss_nbs_r_key_press),&data);

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

sprintf(strsql,"%s %s",gettext("Расчет журнал-ордера по счету"),data.rek_r->shet.ravno());
soob.new_plus(strsql);

sprintf(strsql,"%s %d.%d.%d => %d.%d.%d",gettext("Расчет за период"),
data.dn,data.mn,data.gn,
data.dk,data.mk,data.gk);
soob.ps_plus(strsql);

GtkWidget *label=gtk_label_new(soob.ravno_toutf());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);


data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),400,300);

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(go_ss_nbs_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)go_ss_nbs_r1,&data);

gtk_main();

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);





}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  go_ss_nbs_r_v_knopka(GtkWidget *widget,class go_ss_nbs_r_data *data)
{
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   go_ss_nbs_r_key_press(GtkWidget *widget,GdkEventKey *event,class go_ss_nbs_r_data *data)
{

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

gint go_ss_nbs_r1(class go_ss_nbs_r_data *data)
{
char strsql[512];
iceb_u_str soob;

//printf("go_ss_nbs_r1 data->prohod=%d\n",data->prohod);

if(data->prohod == 0)
 {
  data->prohod++;
  time(&data->vremn);
  
  /*Узнаем начальное сальдо по счету*/

  /*Отчет по для многопорядкового плана счетов*/
  if(vplsh == 1)
   sprintf(strsql,"select deb,kre from Saldo where kkk='0' and gs=%d and \
  ns like '%s%%'",data->godn,data->rek_r->shet.ravno());

  /*Отчет по субсчету для двух-порядкового плана счетов*/
  if(data->rek_r->vds == 1 && vplsh == 0)
   sprintf(strsql,"select deb,kre from Saldo where kkk='0' and gs=%d and \
ns='%s'",data->godn,data->rek_r->shet.ravno());

  /*Отчет по счету для двух-порядкового плана счетов*/
  if(data->rek_r->vds == 0 && vplsh == 0)
   sprintf(strsql,"select deb,kre from Saldo where kkk='0' and gs=%d and \
ns='%s%%'",data->godn,data->rek_r->shet.ravno());

  //printw("%s\n",strsql);
  //refresh();
  SQLCURSOR cur;
  SQL_str   row;
  int       kolstr;  
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {

    data->sdeb+=atof(row[0]);
    data->skre+=atof(row[1]);
   }

  sprintf(strsql,"%s %.2f %.2f %d\n",
  gettext("Стартовое сальдо"),data->sdeb,data->skre,data->godn);

  soob.new_plus(strsql);
  iceb_printw(soob.ravno_toutf(),data->buffer,data->view);

  char imaf[30];  
  sprintf(imaf,"gnbs%d.lst",getpid());

  sprintf(strsql,"%s %s",gettext("Журнал ордер"),data->rek_r->shet.ravno());
  data->naim.new_plus(strsql);


  data->imaf.new_plus(imaf);

  if((data->ff = fopen(imaf,"w")) == NULL)
   {
    iceb_er_op_fil(imaf,"",errno,data->window);
    return(FALSE);
   }
  iceb_u_startfil(data->ff);

  fprintf(data->ff,"\x1b\x6C%c",10); /*Установка левого поля*/


  sprintf(strsql,"%s %s %s (%s)",gettext("Журнала ордер"),
  data->rek_r->shet.ravno(),data->rek_r->naimshet.ravno(),gettext("Внебалансовый"));

//  printf("strsql=%s\n",strsql);
  iceb_u_zagolov(strsql,data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,organ,data->ff);

  fprintf(data->ff,"\
-------------------------------------------------------------\n");
  fprintf(data->ff,gettext("  Дата    |  Кто   |  Дебет   |  Кредит  | Коментарий\n"));

  fprintf(data->ff,"\
-------------------------------------------------------------\n");
  data->d2=1; data->m2=1; data->g2=data->godn;
  data->kolstr=iceb_u_period(1,1,data->godn,data->dk,data->mk,data->gk,0);
  
  return(TRUE);
 }

if(data->prohod == 1)
 {
/**************
  sprintf(strsql,"%d.%d.%d %d.%d.%d %d %f\n",
  data->d2,data->m2,data->g2,data->dk,data->mk,data->gk,data->kolstr,data->kolstr1);
  soob.new_plus(strsql);
  iceb_printw(soob.ravno_toutf(),data->buffer,data->view);
**************/


  while(iceb_u_sravmydat(data->d2,data->m2,data->g2,data->dk,data->mk,data->gk) <= 0)
   {
    iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
//    printf("%d.%d.%d\n",data->d2,data->m2,data->g2);
    if(data->metkasaldo == 0 && iceb_u_sravmydat(data->d2,data->m2,data->g2,data->dn,data->mn,data->gn) == 0)
     {
      
      data->metkasaldo=1;
  //    printw("%f %f %f %f\n",data->sdeb,data->sndeb,data->skre,data->snkre);
      
      
      if(data->sdeb+data->sndeb > data->skre+data->snkre)
       {
        sprintf(strsql,"%*s %10.2f\n",
        iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
        (data->sdeb+data->sndeb)-(data->skre+data->snkre));
       
        fprintf(data->ff,"%*.*s %10.2f\n",
        iceb_u_kolbait(19,gettext("Сальдо начальное")),iceb_u_kolbait(19,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
        (data->sdeb+data->sndeb)-(data->skre+data->snkre));
       }
      else
       {
        sprintf(strsql,"%*s %10s %10.2f\n",
        iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
        " ",(data->skre+data->snkre)-(data->sdeb+data->sndeb));

        fprintf(data->ff,"%*.*s %10s %10.2f\n",
        iceb_u_kolbait(19,gettext("Сальдо начальное")),iceb_u_kolbait(19,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
        " ",(data->skre+data->snkre)-(data->sdeb+data->sndeb));
       }

      soob.new_plus(strsql);
      iceb_printw(soob.ravno_toutf(),data->buffer,data->view);

     }

    //Для многопорядкового плана счетов
    if(vplsh == 1)
      sprintf(strsql,"select deb,kre,kto,ktoi,komen from Prov \
  where val=-1 and datp = '%d-%02d-%02d' and sh like '%s%%'",
      data->g2,data->m2,data->d2,data->rek_r->shet.ravno());

    /*Отчет по субсчету для двух-порядкового плана счетов*/
    if(data->rek_r->vds == 1 && vplsh == 0)
      sprintf(strsql,"select deb,kre,kto,ktoi,komen from Prov \
  where val=-1 and datp = '%d-%02d-%02d' and sh='%s'",
      data->g2,data->m2,data->d2,data->rek_r->shet.ravno());

    /*Отчет по счету для двух-порядкового плана счетов*/
    if(data->rek_r->vds == 1 && vplsh == 0)
      sprintf(strsql,"select deb,kre,kto,ktoi,komen from Prov \
  where val=-1 and datp = '%d-%02d-%02d' and sh like '%s%%'",
      data->g2,data->m2,data->d2,data->rek_r->shet.ravno());

    SQLCURSOR cur;
    SQL_str   row;
    int kolstr;
    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

    double deb,kre;
    while(cur.read_cursor(&row) != 0)
     {
      deb=atof(row[0]);
      kre=atof(row[1]);
      if(iceb_u_sravmydat(data->d2,data->m2,data->g2,data->dn,data->mn,data->gn) < 0)
       {
        data->sndeb+=deb;
        data->snkre+=kre;
       }
      if(iceb_u_sravmydat(data->d2,data->m2,data->g2,data->dn,data->mn,data->gn) >= 0)
       {
        data->odeb+=deb;
        data->okre+=kre;
        fprintf(data->ff,"%02d.%02d.%d %-4s%4s %10.2f %10.2f %s\n",
        data->d2,data->m2,data->g2,row[2],row[3],deb,kre,row[4]);
       }


     }

    iceb_u_dpm(&data->d2,&data->m2,&data->g2,1);
    return(TRUE);
   }
  data->prohod++;

  fprintf(data->ff,"\
-------------------------------------------------------------\n");

  sprintf(strsql,"%*s %10.2f %10.2f\n",
  iceb_u_kolbait(20,gettext("Оборот за период")),gettext("Оборот за период"),
  data->odeb,data->okre);

  soob.new_plus(strsql);

  fprintf(data->ff,"%*.*s %10.2f %10.2f\n",
  iceb_u_kolbait(19,gettext("Оборот за период")),iceb_u_kolbait(19,gettext("Оборот за период")),gettext("Оборот за период"),
  data->odeb,data->okre);

  if(data->sdeb+data->sndeb+data->odeb > data->skre+data->snkre+data->okre)
   {
    sprintf(strsql,"%*s %10.2f\n",
    iceb_u_kolbait(20,gettext("Сальдо конечное")),gettext("Сальдо конечное"),
    (data->sdeb+data->sndeb+data->odeb)-(data->skre+data->snkre+data->okre));

    fprintf(data->ff,"%*.*s %10.2f\n",
    iceb_u_kolbait(19,gettext("Сальдо конечное")),iceb_u_kolbait(19,gettext("Сальдо конечное")),gettext("Сальдо конечное"),
    (data->sdeb+data->sndeb+data->odeb)-(data->skre+data->snkre+data->okre));
   }
  else
   {
    sprintf(strsql,"%*s %10s %10.2f\n",
    iceb_u_kolbait(20,gettext("Сальдо конечное")),gettext("Сальдо конечное"),
    " ",(data->skre+data->snkre+data->okre)-(data->sdeb+data->sndeb+data->odeb));

    fprintf(data->ff,"%*.*s %10s %10.2f\n",
    iceb_u_kolbait(19,gettext("Сальдо конечное")),iceb_u_kolbait(19,gettext("Сальдо конечное")),gettext("Сальдо конечное"),
    " ",(data->skre+data->snkre+data->okre)-(data->sdeb+data->sndeb+data->odeb));
   }

  soob.plus(strsql);
  iceb_printw(soob.ravno_toutf(),data->buffer,data->view);

  iceb_podpis(data->ff,data->window);

  fprintf(data->ff,"\x1B\x50"); /*10-знаков*/

  fprintf(data->ff,"\x12");  /*Нормальный режим печати*/
  fprintf(data->ff,"\x1b\x6C%c",1); /*Установка левого поля в ноль*/
  fprintf(data->ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/

  fclose(data->ff);

  return(TRUE);
 }

if(data->prohod == 2)
 {
  data->prohod++;

  iceb_printw_vr(data->vremn,data->buffer,data->view);
  return(TRUE);
 }



gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);
return(FALSE);

}
