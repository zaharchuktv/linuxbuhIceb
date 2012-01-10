/*$Id: zarpdw_r.c,v 1.1 2011-03-28 06:55:46 sasa Exp $*/
/*22.03.2011	22.03.2011	Белых А.И.	zarpdw_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "zarpdw.h"

class zarpdw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zarpdw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zarpdw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zarpdw_r_key_press(GtkWidget *widget,GdkEventKey *event,class zarpdw_r_data *data);
gint zarpdw_r1(class zarpdw_r_data *data);
void  zarpdw_r_v_knopka(GtkWidget *widget,class zarpdw_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
//extern int      kol_strok_na_liste;
extern class iceb_u_str kodpn_all;
extern short *obud; /*Обязательные удержания*/
extern short *knvr;/*Коды начислений не входящие в расчет подоходного налога*/
extern short *knnf; //Коды начислений недежными формами 

int zarpdw_r(class zarpdw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zarpdw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Расчёт подоходного на выданную зарплату"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zarpdw_r_key_press),&data);

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

repl.plus(gettext("Расчёт подоходного на выданную зарплату"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),imabaz,organ);
repl.ps_plus(strsql);

GtkWidget *label=gtk_label_new(repl.ravno_toutf());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),450,300);

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста
//PangoFontDescription *font_pango=pango_font_description_from_string("Nimbus Mono L, Bold 12");
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



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
GtkTooltips *tooltops=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltops,data.knopka,gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zarpdw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zarpdw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zarpdw_r_v_knopka(GtkWidget *widget,class zarpdw_r_data *data)
{
//printf("zarpdw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zarpdw_r_key_press(GtkWidget *widget,GdkEventKey *event,class zarpdw_r_data *data)
{
// printf("zarpdw_r_key_press\n");
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}
/**************************************/
/*Узнаём сумму к выплате и сумму подоходного налога*/
/*****************************************************/
void zarpd_skv(const char *tbn,short mes,short god,double *suma_kvp,double *podoh_kvp,double *suma_dohoda,FILE *ff_prot,GtkWidget *wpredok)
{
char strsql[512];
int kolstr=0;
double suma_nah=0.;
double suma_uder=0.;
double suma=0.;
SQL_str row;
class SQLCURSOR cur;
int knah=0;
*suma_kvp=0.;
*podoh_kvp=0.;
*suma_dohoda=0.;

sprintf(strsql,"select prn,knah,suma from Zarp where datz >= '%04d-%02d-%02d' and datz <= '%04d-%02d-%d' and tabn=%s and suma <> 0.",
god,mes,1,god,mes,31,tbn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  suma=atof(row[2]);
  knah=atoi(row[1]);
  if(atoi(row[0]) == 1) /*начисления*/
   {
    suma_nah+=suma;

    if(provkodw(knvr,knah) >= 0)
     continue;      
    *suma_dohoda+=suma;
   }
  else
   {

//    if(knah == kodpn)
    if(iceb_u_proverka(kodpn_all.ravno(),knah,0,1) == 0)
     {
      *podoh_kvp+=suma;
      continue;
     }

    if(provkodw(obud,knah) < 0)
     continue;
    suma_uder+=suma;
   }

 }

*suma_kvp=suma_nah+suma_uder+*podoh_kvp;


}


/******************************************/
/******************************************/

gint zarpdw_r1(class zarpdw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);


short mr=0,gr=0;
iceb_u_rsdat1(&mr,&gr,data->rk->datar.ravno());

char imaf[64];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
float kolstr1=0;
int kolstr2=0;
double suma_str[11];
double i_suma_str[11];
short m,g;

double sumanu=0;
int knah=0;
sprintf(strsql,"select tabn from Zarn where god=%d and mes=%d",gr,mr);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного табельного номера !"),data->window);
  return(1);
 }

FILE *ff;
sprintf(imaf,"pd%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

FILE *ff_prot;
char imaf_prot[64];
sprintf(imaf_prot,"pdp%d.lst",getpid());

if((ff_prot = fopen(imaf_prot,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_prot,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_zagolov(gettext("Расчёт подоходного на выплаченную зарплату"),0,0,0,0,mr,gr,organ,ff);
fprintf(ff,"Коды начислений неденежными формами:");

if(knnf != NULL)
 {
  for(int nom=1; nom <= knnf[0]; nom++)
   fprintf(ff,"%d ",knnf[nom]);
  fprintf(ff,"\n");
 } 

fprintf(ff,"\n");

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
Таб.н.|       Фамилия                |Начислено |Взято в   |Начисленны|Выплаченый|Выплаченый|Выплаченый|Выплаченый|Выплаченый|Выплаченый|Выплачено |Выплачено |\n\
      |                              |          |расчёт    |й подоходн| доход    |доход за  |доход за  |под-ный   |под-ный за|под-ный за|на руки за|на руки за|\n\
      |                              |          |          |ный налог |          |отчё.месяц|другие пер|налог     |отчё.месяц|другие пер|отч.месяц.|другие пер|\n");

/****
123456 123456789012345678901234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890                       1234567890 1234567890
**********/

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
memset(i_suma_str,'\0',sizeof(i_suma_str));
int nom_mes_god=0;
double suma_nfn=0.;
class iceb_u_str fiov("");

while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
   continue;

  fiov.new_plus("");
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   fiov.new_plus(row1[0]);

  fprintf(ff_prot,"\n\n%s %s\n",row[0],fiov.ravno());

  iceb_printw(fiov.ravno_ps(),data->buffer,data->view);
  
  sprintf(strsql,"select prn,knah,suma,godn,mesn from Zarp where datz >= '%04d-%02d-%02d' and datz <= '%04d-%02d-%d' and tabn=%s and suma <> 0.",
  gr,mr,1,gr,mr,31,row[0]);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  if(kolstr2 == 0)
   {
    fprintf(ff_prot,"Не найдено ни одного начисления или удержания!\n");
    continue;
   }
  memset(suma_str,'\0',sizeof(suma_str));

  class iceb_u_spisok mes_god; /*месяц.год в счёт которого выплата*/
  class iceb_u_double suma_mes_god; /*сумма выплаты в счёт месяца*/
  double suma_ob_ud_rm=0.; /*Cумма обязательных удержаний расчётного месяца*/
  while(cur1.read_cursor(&row1) != 0)
   {
    
    sumanu=atof(row1[2]);
    knah=atoi(row1[1]);
    
    if(atoi(row1[0]) == 1) /*начисления*/
     {
      suma_str[0]+=sumanu;

      if(provkodw(knnf,knah) >= 0)
        suma_nfn+=sumanu;
        
      if(provkodw(knvr,knah) >= 0)
       continue;      
      suma_str[1]+=sumanu;

     }
    else
     {

//      if(knah == kodpn)
      if(iceb_u_proverka(kodpn_all.ravno(),knah,0,1) == 0)
       {
        suma_str[2]+=sumanu;
       }

      if(provkodw(obud,knah) >= 0)
       {
        suma_ob_ud_rm+=sumanu;
        continue;
       }
      suma_str[3]+=sumanu;    

      sprintf(strsql,"%02d.%04d",atoi(row1[4]),atoi(row1[3]));
      if((nom_mes_god=mes_god.find(strsql)) < 0)
       mes_god.plus(strsql);
      suma_mes_god.plus(sumanu,nom_mes_god);
             

     }
   }
    
  double suma_viplat_tm=0.;
  double suma_viplat_pp=0.;
  double suma_kvp=0.; /*Сумма к выплате*/
  double podoh_kvp=0.; /*Cумма подоходного*/
  double suma_dohoda=0.;/*Сумма дохода*/
  
  fprintf(ff_prot,"Cуммы выплат:\n");
  
  for(int nom=0; nom < mes_god.kolih(); nom++)
   {
    iceb_u_rsdat1(&m,&g,mes_god.ravno(nom));
    fprintf(ff_prot,"%02d.%04d %.2f\n",m,g,suma_mes_god.ravno(nom));
    
    if(m == mr && g == gr)
     {
      suma_viplat_tm+=suma_mes_god.ravno(nom);
      suma_str[9]+=suma_mes_god.ravno(nom);

      /*сумма обязательных удержаний отрицательная - поэтому прибавляем*/
      double suma_k_vip=suma_str[1]+suma_ob_ud_rm;

      if(ff_prot != NULL)
       fprintf(ff_prot,"Сумма начиленная=%.2f Сумма обязательных удержаний=%.2f\n\
Cумма к выплате=%.2f+%.2f=%.2f\n",
        suma_str[1],suma_ob_ud_rm,suma_str[1],suma_ob_ud_rm,suma_k_vip);
        
      /*Считаем процент выплаты*/
          
      double proc_vip=0.;
      /*сумма обязательных удержаний отрицательная - поэтому прибавляем*/
      if(suma_k_vip  != 0.)
        proc_vip=suma_mes_god.ravno(nom)*100./suma_k_vip*-1;
       
      /*Считаем сколько на эту выплату приходтся подоходного налога*/
      suma_str[7]+=suma_str[2]*proc_vip/100.;

      /*Считаем сколько на эту выплату приходтся дохода*/
      suma_str[4]+=suma_str[1]*proc_vip/100.;     

      fprintf(ff_prot,"Процент выплаты=%.2f*100./%.2f=%f\n",
      suma_mes_god.ravno(nom),suma_k_vip,proc_vip);

      fprintf(ff_prot,"Расчёт доли подоходного налога = %.2f*%f/100.=%.2f\n",
      suma_str[2],proc_vip,suma_str[2]*proc_vip/100.);

      fprintf(ff_prot,"Расчёт доли дохода=%.2f*%f/100.=%.2f\n",
      suma_str[1],proc_vip,suma_str[1]*proc_vip/100.);      
     }
    else
     {
      fprintf(ff_prot,"Выплата в счёт другого периода!\n");

      suma_viplat_pp+=suma_mes_god.ravno(nom);
      suma_str[10]+=suma_mes_god.ravno(nom);

      zarpd_skv(row[0],m,g,&suma_kvp,&podoh_kvp,&suma_dohoda,ff_prot,data->window);
      fprintf(ff_prot,"Cумма к выдаче=%.2f Cумма подоходного налога=%.2f Cумма дохода=%.2f\n",
      suma_kvp,podoh_kvp,suma_dohoda);

      /*Считаем процент выплаты*/
      double proc_vip=0.;
      if(suma_kvp != 0.)
        proc_vip=suma_mes_god.ravno(nom)*100./suma_kvp*-1;
      fprintf(ff_prot,"Процент выплаты=%.2f*100./%.2f=%f\n",
      suma_mes_god.ravno(nom),suma_kvp,proc_vip);
      
      /*Считаем сколько на эту выплату приходтся подоходного налога*/
      suma_str[8]+=podoh_kvp*proc_vip/100.;
      fprintf(ff_prot,"Расчёт доли подоходного налога=%.2f*%f/100.=%.2f\n",
      podoh_kvp,proc_vip,podoh_kvp*proc_vip/100.);
      
      /*Считаем сколько на эту выплату приходтся дохода*/
      suma_str[5]+=suma_dohoda*proc_vip/100.;     
      fprintf(ff_prot,"Расчёт доли дохода=%.2f*%f/100.=%.2f\n",
      suma_dohoda,proc_vip,suma_dohoda*proc_vip/100.);
      
     }
   }
  
  suma_str[6]=suma_str[7]+suma_str[8];
  suma_str[3]=suma_str[4]+suma_str[5];
  
  fprintf(ff,"%6s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  row[0],
  iceb_u_kolbait(30,fiov.ravno()),
  iceb_u_kolbait(30,fiov.ravno()),
  fiov.ravno(),
  suma_str[0],
  suma_str[1],
  suma_str[2],
  suma_str[3],
  suma_str[4],
  suma_str[5],
  suma_str[6],
  suma_str[7],
  suma_str[8],
  suma_str[9],
  suma_str[10]);
  
  i_suma_str[0]+=suma_str[0];
  i_suma_str[1]+=suma_str[1];
  i_suma_str[2]+=suma_str[2];
  i_suma_str[3]+=suma_str[3];
  i_suma_str[4]+=suma_str[4];
  i_suma_str[5]+=suma_str[5];
  i_suma_str[6]+=suma_str[6];
  i_suma_str[7]+=suma_str[7];
  i_suma_str[8]+=suma_str[8];
  i_suma_str[9]+=suma_str[9];
  i_suma_str[10]+=suma_str[10];
    
 }

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%6s %*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
"",
iceb_u_kolbait(30,gettext("Итого")),
iceb_u_kolbait(30,gettext("Итого")),
gettext("Итого"),
i_suma_str[0],
i_suma_str[1],
i_suma_str[2],
i_suma_str[3],
i_suma_str[4],
i_suma_str[5],
i_suma_str[6],
i_suma_str[7],
i_suma_str[8],
i_suma_str[9],
i_suma_str[10]);

double suma_ndf_v=suma_nfn;

rudnfvw(&suma_ndf_v,suma_nfn,ff);


iceb_podpis(ff,data->window);

fclose(ff);
fclose(ff_prot);






data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расчёт подоходного на выплаченную зарплату"));

data->rk->imaf.plus(imaf_prot);
data->rk->naimf.plus(gettext("Протокол расчёта"));

for(int nomer=0; nomer < data->rk->imaf.kolih(); nomer++)
 iceb_ustpeh(data->rk->imaf.ravno(nomer),3,data->window);





gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
