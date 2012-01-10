/*$Id: rest_oth_r.c,v 1.31 2011-02-21 07:36:21 sasa Exp $*/
/*23.01.2006	06.07.2004	Белых А.И.	rest_oth_r.c
Расчёт отчёта по оплаченным счетам
*/
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include "i_rest.h"
#include "rest_oth.h"


class rest_oth_r_data
 {
  public:
  
  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  int    kolstr;  //Количество строк в курсоре

  class rest_oth_data *rk;
  short voz;
  time_t vremn;
  
  rest_oth_r_data()
   {
    voz=0;
   }
   
 };

gboolean   rest_oth_r_key_press(GtkWidget *widget,GdkEventKey *event,class rest_oth_r_data *data);
gint rest_oth_r1(class rest_oth_r_data *data);
void  rest_oth_r_v_knopka(GtkWidget *widget,class rest_oth_r_data *data);

void  itog_skl(char *sklz,double *itogskl,FILE *ff,GtkWidget *view,GtkTextBuffer *buffer);
void  itog_podr(char *podrz,double *itogop,FILE *ff,GtkWidget *view,GtkTextBuffer *buffer);
void rest_oth_sap(class rest_oth_r_data *data,int,FILE *ff);
void  rest_oth_r_razuz(iceb_u_spisok *,iceb_u_double *,iceb_u_spisok *,iceb_u_double *,GtkWidget *wpredok);

extern SQL_baza	bd;
extern char *name_system;
extern char *organ;
extern uid_t kod_operatora;

//extern class iceb_get_dev_data config_dev;

int     rest_oth_r(class rest_oth_data *datap)
{
printf("rest_oth_r-Начало работы\n");

iceb_u_str soob;
char strsql[300];
rest_oth_r_data data;
data.rk=datap;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Отчёт по оплаченным счетам"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rest_oth_r_key_press),&data);

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));
gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);


soob.new_plus(strsql);

sprintf(strsql,"%s %s %s => %s %s",gettext("Расчет за период"),
data.rk->datan.ravno(),
data.rk->vremn.ravno_time(),
data.rk->datak.ravno(),
data.rk->vremk.ravno_time_end());
soob.new_plus(strsql);

GtkWidget *label=gtk_label_new(soob.ravno_toutf());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);


data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),400,300);

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста
PangoFontDescription *font_pango=pango_font_description_from_string("Nimbus Mono L, Bold 10");
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(rest_oth_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна


gtk_idle_add((GtkFunction)rest_oth_r1,&data);

gtk_main();

//if(data.voz == 0)
//  iceb_rabfil(&data.imaf,&data.naim,"",config_dev.metka_klav,NULL);
return(data.voz); 




}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rest_oth_r_v_knopka(GtkWidget *widget,class rest_oth_r_data *data)
{
// printf("rest_oth_r_v_knopka\n");
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rest_oth_r_key_press(GtkWidget *widget,GdkEventKey *event,class rest_oth_r_data *data)
{
// printf("rest_oth_r_key_press\n");

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

gint rest_oth_r1(class rest_oth_r_data *data)
{
time(&data->vremn);
char strsql[1000];
iceb_u_str soob;
SQLCURSOR cur;
SQL_str   row;
int       kolstr=0;

//список документов
sprintf(strsql,"select datd,nomd,podr,ps,ktoi \
from Restdok where mo=1 and vremo >= %ld and vremo <= %ld \
order by datd asc",
iceb_u_vrem(&data->rk->datan,&data->rk->vremn,0),
iceb_u_vrem(&data->rk->datak,&data->rk->vremk,1));

//printf("rest_oth_r strsql=%s\n",strsql);

//iceb_printw(strsql,data->buffer,data->view);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  data->voz=1;
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найдено ни одной записи !"));
  iceb_menu_soob(&repl,data->window);
  data->voz=1;
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

FILE *ff;
sprintf(strsql,"rest_oth_dok%d.lst",getpid());
printf("Имя файла=%s\n",strsql);

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  data->voz=1;
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_startfil(ff);
fprintf(ff,"\x1b\x6C%c",5); /*Установка левого поля*/
rest_oth_sap(data,0,ff);

data->rk->imaf.new_plus(strsql);
data->rk->naim.new_plus(gettext("Список документов"));

fprintf(ff,"\
------------------------------------------\n");

fprintf(ff,"\
   Дата   |Номер док.|К.п.|П.с.| Сумма \n");
fprintf(ff,"\
------------------------------------------\n");
short d,m,g;  
double itogo=0.;
double suma;

while(cur.read_cursor(&row) != 0)
{
 if(iceb_u_proverka(data->rk->podr.ravno(),row[2],0,0) != 0)
   continue;
 if(iceb_u_proverka(data->rk->kod_op.ravno(),row[4],0,0) != 0)
   continue;

//  if(iceb_u_proverka(data->rk->skl.ravno(),row[3],0,0) != 0)
//   continue;

 iceb_u_rsdat(&d,&m,&g,row[0],2);
 suma=sumapsh(g,row[1],data->rk->skl.ravno(),data->window);
 itogo+=suma;
 
 fprintf(ff,"%s %-10s %-4s %4s %10.2f\n",iceb_u_sqldata(row[0]),row[1],row[2],row[3],suma);
 
} 
fprintf(ff,"\
------------------------------------------\n");
fprintf(ff,"%31s:%10.2f\n",gettext("Итого"),itogo);

fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля*/
iceb_podpis(kod_operatora,ff,data->window);

fclose(ff);

sprintf(strsql,"select i.datd,i.nomd,i.podr,o.skl,o.mz,o.kodz,o.kolih,o.cena,i.ps,o.ktoi \
from Restdok i,Restdok1 o where i.mo=1 and i.vremo >= %ld and i.vremo <= %ld \
and i.datd=o.datd and i.nomd=o.nomd order by i.podr asc, o.mz asc,o.skl asc",
iceb_u_vrem(&data->rk->datan,&data->rk->vremn,0),
iceb_u_vrem(&data->rk->datak,&data->rk->vremk,1));


printf("rest_oth_r strsql=%s\n",strsql);


//sprintf(strsql,"select skl,mz,kodz,kolih,cena,podr from Restdok1 where 

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  data->voz=1;
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найдено ни одной записи !"));
  iceb_menu_soob(&repl,data->window);
  data->voz=1;
  return(FALSE);
 }

class iceb_u_spisok skl_mz;

class iceb_u_spisok podr_mz_skl_kodt_cena; 
class iceb_u_double kodt_kolih;
class iceb_u_double kodt_suma;

class iceb_u_spisok podr_mz_kodt;  //Список по подразделениям для разузлования
class iceb_u_double podr_mz_kodt_kolih;
class iceb_u_spisok skl_mz_kodt;  //Список по складам для разузлования
class iceb_u_double skl_mz_kodt_kolih;

gfloat kolstr1=0.; 

int nomer=0;
float procent_sk=0.;
double suma_skidki=0.;
double kolih_kodt=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->podr.ravno(),row[2],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->rk->skl.ravno(),row[3],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->rk->kod_op.ravno(),row[9],0,0) != 0)
   continue;


//  sprintf(strsql,"%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);
//  iceb_printw(strsql,data->buffer,data->view);
  kolih_kodt=atof(row[6]);
    
  sprintf(strsql,"%s|%s",row[3],row[4]);
  if(skl_mz.find(strsql) < 0)
    skl_mz.plus(strsql);

  sprintf(strsql,"%s|%s|%s",row[2],row[4],row[5]);
  if((nomer=podr_mz_kodt.find(strsql)) == -1)
   podr_mz_kodt.plus(strsql);
  
  podr_mz_kodt_kolih.plus(kolih_kodt,nomer);
  
  sprintf(strsql,"%s|%s|%s",row[3],row[4],row[5]);
  if((nomer=skl_mz_kodt.find(strsql)) == -1)
   skl_mz_kodt.plus(strsql);
  
  skl_mz_kodt_kolih.plus(kolih_kodt,nomer);
  //подразделение|товар/услуга|склад|код записи|цена     
  sprintf(strsql,"%s|%s|%s|%s|%s",row[2],row[4],row[3],row[5],row[7]);

  suma=kolih_kodt*atof(row[7]);
  suma=iceb_u_okrug(suma,0.01);

  if((nomer=podr_mz_skl_kodt_cena.find(strsql)) == -1)
    podr_mz_skl_kodt_cena.plus(strsql);
    
  kodt_kolih.plus(kolih_kodt,nomer);    

  kodt_suma.plus(suma,nomer);

  procent_sk=atof(row[8]);
  suma_skidki+=suma*procent_sk/100.;
     
//  iceb_printw(soob.ravno_toutf(),data->buffer,data->view);
  
 }

sprintf(strsql,"rest_oth_p%d.lst",getpid());
printf("Имя файла=%s\n",strsql);

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  data->voz=1;
  return(FALSE);
 }

iceb_u_startfil(ff);
printf("Распечатываем по подразделениям.\n");

data->rk->imaf.plus(strsql);
data->rk->naim.plus(gettext("Отчёт по подразделениям"));

iceb_printw(gettext("Отчёт по подразделениям"),data->buffer,data->view);
iceb_printw("\n",data->buffer,data->view);
char bros[300];

sprintf(bros,"-----------------------------------------------\n");
iceb_printw(iceb_u_toutf(bros),data->buffer,data->view);

rest_oth_sap(data,1,ff);

int kolih=podr_mz_skl_kodt_cena.kolih();
char kod[30];
char cena[30];
char podr[30];
char podrz[30];
char naim[50];
itogo=0.;
double itogop=0.;
double itogoskl=0.;

memset(podrz,'\0',sizeof(podrz));
char metka[4];
char skl[30];
char sklz[30];
memset(sklz,'\0',sizeof(sklz));

for(int i=0; i < kolih; i++)
 {
  iceb_u_polen(podr_mz_skl_kodt_cena.ravno(i),podr,sizeof(podr),1,'|');
  iceb_u_polen(podr_mz_skl_kodt_cena.ravno(i),metka,sizeof(metka),2,'|');
  iceb_u_polen(podr_mz_skl_kodt_cena.ravno(i),skl,sizeof(skl),3,'|');
  iceb_u_polen(podr_mz_skl_kodt_cena.ravno(i),kod,sizeof(kod),4,'|');
  iceb_u_polen(podr_mz_skl_kodt_cena.ravno(i),cena,sizeof(cena),5,'|');

  if(iceb_u_SRAV(sklz,skl,0) != 0)
   {
    itog_skl(sklz,&itogoskl,ff,data->view,data->buffer);
    strcpy(sklz,skl);
   }

  if(iceb_u_SRAV(podrz,podr,0) != 0)
   {
    itog_skl(sklz,&itogoskl,ff,data->view,data->buffer);
    itog_podr(podrz,&itogop,ff,data->view,data->buffer);
    fprintf(ff,"\n");
    strcpy(podrz,podr);
   }


  memset(naim,'\0',sizeof(naim));
  if(metka[0] == '0')
   {
    sprintf(strsql,"select naimat from Material where kodm=%s",kod);
    strcpy(metka,"T");
   }
  if(metka[0] == '1')
   {
    sprintf(strsql,"select naius from Uslugi where kodus=%s",kod);
    strcpy(metka,"У");
   }
  
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   strncpy(naim,row[0],sizeof(naim)-1);
     
  fprintf(ff,"%-3s %-5s %1.1s %-30.30s %8.8g %7.7g %10.2f\n",
  skl,kod,metka,naim,kodt_kolih.ravno(i),atof(cena),kodt_suma.ravno(i));
  itogo+=kodt_suma.ravno(i);
  itogop+=kodt_suma.ravno(i);
  itogoskl+=kodt_suma.ravno(i);
 }

itog_skl(sklz,&itogoskl,ff,data->view,data->buffer);
itog_podr(podrz,&itogop,ff,data->view,data->buffer);

fprintf(ff,"\
-----------------------------------------------------------------------\n");
fprintf(ff,"%59s:%10.2f\n",gettext("Итого"),itogo);
fprintf(ff,"%59s:%10.2f\n",gettext("Сумма скидки"),suma_skidki);
fprintf(ff,"%59s:%10.2f\n",gettext("Вместе со скидкой"),itogo-suma_skidki);


sprintf(bros,"-----------------------------------------------\n");
iceb_printw(iceb_u_toutf(bros),data->buffer,data->view);

sprintf(bros,"%-30.30s:%10.2f\n",gettext("Итого"),itogo);
iceb_printw(iceb_u_toutf(bros),data->buffer,data->view);
 
printf("Делаем разузлование для подразделения.\n");

//Разузлование для подразделений 
/***************************************************************************/
iceb_u_spisok podr_kodmv;
iceb_u_double podr_kodmv_kolih;

rest_oth_r_razuz(&podr_mz_kodt,&podr_mz_kodt_kolih,&podr_kodmv,&podr_kodmv_kolih,data->window);

fprintf(ff,"\n%s:\n",gettext("Список входящих материалов"));

kolih=podr_kodmv.kolih();
memset(podrz,'\0',sizeof(podrz));
for(int i=0; i < kolih; i++)
 {
//  printf("%s\n",podr_kodmv.ravno(i));

  iceb_u_polen(podr_kodmv.ravno(i),podr,sizeof(podr),1,'|');
  iceb_u_polen(podr_kodmv.ravno(i),metka,sizeof(metka),2,'|');
  iceb_u_polen(podr_kodmv.ravno(i),kod,sizeof(kod),3,'|');
  if(iceb_u_SRAV(podrz,podr,0) != 0)
   {
    memset(naim,'\0',sizeof(naim));
    sprintf(strsql,"select naik from Restpod where kod=%s",podr);
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
      strncpy(naim,row[0],sizeof(naim)-1);
    fprintf(ff,"\n%s: %s %s\n",gettext("Подразделение"),podr,naim);

    fprintf(ff,"\
-----------------------------------------------------------------------\n");

    strcpy(podrz,podr);
   }

  memset(naim,'\0',sizeof(naim));
  if(metka[0] == '0')
   {
    sprintf(strsql,"select naimat from Material where kodm=%s",kod);
    strcpy(metka,"T");
   }
  if(metka[0] == '1')
   {
    sprintf(strsql,"select naius from Uslugi where kodus=%s",kod);
    strcpy(metka,"У");
   }
//  printf("%s\n",strsql);  
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   strncpy(naim,row[0],sizeof(naim)-1);
     
  fprintf(ff,"%-5s %1.1s %-30.30s %8.8g\n",
  kod,metka,naim,podr_kodmv_kolih.ravno(i));

 }
fprintf(ff,"\
-----------------------------------------------------------------------\n");
//printf("Конец распечатки\n");


iceb_podpis(ff,data->window);

fclose(ff);
printf("Распечатываем по складам.\n");

//Распечатываем по складам
//**********************************************************************
sprintf(strsql,"rest_oth_s%d.lst",getpid());

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  data->voz=1;
  return(FALSE);
 }
iceb_u_startfil(ff);

data->rk->imaf.plus(strsql);
data->rk->naim.plus(gettext("Отчёт по складам"));
iceb_printw("\n",data->buffer,data->view);
iceb_printw(gettext("Отчёт по складам"),data->buffer,data->view);
iceb_printw("\n",data->buffer,data->view);

sprintf(bros,"-----------------------------------------------\n");
iceb_printw(iceb_u_toutf(bros),data->buffer,data->view);

rest_oth_sap(data,1,ff);
char sklp[20];
char metkap[20];

memset(sklz,'\0',sizeof(sklz));

itogo=itogop=itogoskl=0.;
int kolih1=skl_mz.kolih();
kolih=podr_mz_skl_kodt_cena.kolih();
for(int i1=0; i1 < kolih1; i1++)
 {
  iceb_u_polen(skl_mz.ravno(i1),sklp,sizeof(sklp),1,'|');
  iceb_u_polen(skl_mz.ravno(i1),metkap,sizeof(metkap),2,'|');

//  iceb_printw(skl_mz.ravno(i1),data->buffer,data->view);
  
  for(int i=0; i < kolih; i++)
   {
    iceb_u_polen(podr_mz_skl_kodt_cena.ravno(i),podr,sizeof(podr),1,'|');
    iceb_u_polen(podr_mz_skl_kodt_cena.ravno(i),metka,sizeof(metka),2,'|');
    iceb_u_polen(podr_mz_skl_kodt_cena.ravno(i),skl,sizeof(skl),3,'|');
    iceb_u_polen(podr_mz_skl_kodt_cena.ravno(i),kod,sizeof(kod),4,'|');
    iceb_u_polen(podr_mz_skl_kodt_cena.ravno(i),cena,sizeof(cena),5,'|');
    
    if(iceb_u_SRAV(sklp,skl,0) != 0)
      continue;
    if(iceb_u_SRAV(metkap,metka,0) != 0)
      continue;

/*****************    
    if(iceb_u_SRAV(podrz,podr,0) != 0)
     {
      itog_podr(podrz,&itogop,ff,data->view,data->buffer);
      strcpy(podrz,podr);
     }
******************/

    if(iceb_u_SRAV(sklz,skl,0) != 0)
     {
//      itog_podr(podrz,&itogop,ff,data->view,data->buffer);
      itog_skl(sklz,&itogoskl,ff,data->view,data->buffer);
      fprintf(ff,"\n");
      strcpy(sklz,skl);
     }

    memset(naim,'\0',sizeof(naim));
    if(metka[0] == '0')
     {
      sprintf(strsql,"select naimat from Material where kodm=%s",kod);
      strcpy(metka,"T");
     }
    if(metka[0] == '1')
     {
      sprintf(strsql,"select naius from Uslugi where kodus=%s",kod);
      strcpy(metka,"У");
     }
    
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     strncpy(naim,row[0],sizeof(naim)-1);
       
    fprintf(ff,"%-3s %-5s %1.1s %-30.30s %8.8g %7.7g %10.2f\n",
    skl,kod,metka,naim,kodt_kolih.ravno(i),atof(cena),kodt_suma.ravno(i));
    itogo+=kodt_suma.ravno(i);
    itogop+=kodt_suma.ravno(i);
    itogoskl+=kodt_suma.ravno(i);
   }

 }

//itog_podr(podrz,&itogop,ff,data->view,data->buffer);
itog_skl(sklz,&itogoskl,ff,data->view,data->buffer);

fprintf(ff,"\
-----------------------------------------------------------------------\n");
fprintf(ff,"%59s:%10.2f\n",gettext("Итого"),itogo);
fprintf(ff,"%59s:%10.2f\n",gettext("Сумма скидки"),suma_skidki);
fprintf(ff,"%59s:%10.2f\n",gettext("Вместе со скидкой"),itogo-suma_skidki);

sprintf(bros,"-----------------------------------------------\n");
iceb_printw(iceb_u_toutf(bros),data->buffer,data->view);

sprintf(bros,"%-30.30s:%10.2f\n",gettext("Итого"),itogo);
iceb_printw(iceb_u_toutf(bros),data->buffer,data->view);

printf("Делаем разузлование для складов.\n");
//Разузлование для складов
/***************************************************************************/

podr_kodmv.free_class();
podr_kodmv_kolih.free_class();

rest_oth_r_razuz(&skl_mz_kodt,&skl_mz_kodt_kolih,&podr_kodmv,&podr_kodmv_kolih,data->window);

fprintf(ff,"\n%s:\n",gettext("Список входящих материалов"));
kolih1=skl_mz.kolih();
kolih=podr_kodmv.kolih();
memset(podrz,'\0',sizeof(podrz));
for(int i1=0; i1 < kolih1; i1++)
 {
  iceb_u_polen(skl_mz.ravno(i1),sklp,sizeof(sklp),1,'|');
  iceb_u_polen(skl_mz.ravno(i1),metkap,sizeof(metkap),2,'|');
  for(int ii=0; ii < kolih; ii++)
   {

    iceb_u_polen(podr_kodmv.ravno(ii),podr,sizeof(podr),1,'|');
    iceb_u_polen(podr_kodmv.ravno(ii),metka,sizeof(metka),2,'|');
    iceb_u_polen(podr_kodmv.ravno(ii),kod,sizeof(kod),3,'|');

    if(iceb_u_SRAV(sklp,podr,0) != 0)
      continue;
    if(iceb_u_SRAV(metkap,metka,0) != 0)
      continue;

    if(iceb_u_SRAV(podrz,podr,0) != 0)
     {
      memset(naim,'\0',sizeof(naim));
      sprintf(strsql,"select naik from Sklad where kod=%s",podr);
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
        strncpy(naim,row[0],sizeof(naim)-1);
      fprintf(ff,"\n%s: %s %s\n",gettext("Склад"),podr,naim);

      fprintf(ff,"\
-----------------------------------------------------------------------\n");

      strcpy(podrz,podr);
     }

    memset(naim,'\0',sizeof(naim));
    if(metka[0] == '0')
     {
      sprintf(strsql,"select naimat from Material where kodm=%s",kod);
      strcpy(metka,"T");
     }
    if(metka[0] == '1')
     {
      sprintf(strsql,"select naius from Uslugi where kodus=%s",kod);
      strcpy(metka,"У");
     }
  //  printf("%s\n",strsql);  
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     strncpy(naim,row[0],sizeof(naim)-1);
       
    fprintf(ff,"%-5s %1.1s %-30.30s %8.8g\n",
    kod,metka,naim,podr_kodmv_kolih.ravno(ii));

   }   
}
//printf("Конец работы.\n");
iceb_podpis(ff,data->window);

fclose(ff);






iceb_printw_vr(data->vremn,data->buffer,data->view);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);
//printf("Расчёт закончен.\n");

return(FALSE);

}
/***************/
/*итог по складам*/
/*****************/

void  itog_skl(char *sklz,double *itogskl,FILE *ff,
GtkWidget *view,
GtkTextBuffer *buffer)
{

if(sklz[0] == '\0')
 return;
 
if(*itogskl == 0.)
 return;
char strsql[300];
char bros[300];
 
sprintf(strsql,"%s %s",gettext("Итого по складу"),sklz);
fprintf(ff,"%59s:%10.2f\n",strsql,*itogskl);
sprintf(bros,"%-30.30s:%10.2f\n",strsql,*itogskl);
iceb_printw(iceb_u_toutf(bros),buffer,view);
*itogskl=0.;

}
/********************/
/*итог по подразделению*/
/***********************/

void  itog_podr(char *podrz,double *itogop,FILE *ff,
GtkWidget *view,
GtkTextBuffer *buffer)
{
if(podrz[0] == '\0')
 return;
if(*itogop == 0.)
 return;
char strsql[300];
char bros[300];

sprintf(strsql,"%s %s",gettext("Итого по подразделению"),podrz);
fprintf(ff,"%59s:%10.2f\n",strsql,*itogop);
sprintf(bros,"%-30.30s:%10.2f\n",strsql,*itogop);
iceb_printw(iceb_u_toutf(bros),buffer,view);
*itogop=0.;

}
/****************************/
/*начало*/
/*******************/

void rest_oth_sap(class rest_oth_r_data *data,int metka,FILE *ff)
{
fprintf(ff,"%s\n\n",organ);

fprintf(ff,gettext("Отчёт по оплаченным счетам"));
fprintf(ff,"\n%s %s %s %s %s %s\n",
gettext("Период с"),
data->rk->datan.ravno(),
data->rk->vremn.ravno_time(),
gettext("по"),
data->rk->datak.ravno(),
data->rk->vremk.ravno_time_end());

time_t     vrem;

time(&vrem);
struct tm *bf;
bf=localtime(&vrem);
fprintf(ff,"\
%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

if(data->rk->podr.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Подразделение"),data->rk->podr.ravno());
if(data->rk->skl.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Склад"),data->rk->skl.ravno());
if(data->rk->kod_op.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код оператора"),data->rk->kod_op.ravno());

if(metka == 1)
 { 
  fprintf(ff,"\
-----------------------------------------------------------------------\n");
  fprintf(ff,gettext("\
Скл| Код |M|  Наименование товара/услуги  |Кол-ство| Цена  |   Сумма  |\n"));
  fprintf(ff,"\
-----------------------------------------------------------------------\n");
 }
}

/******************************/
/*Разузлование кода материала*/
/*****************************/
void  rest_oth_r_razuz(iceb_u_spisok *podr_mz_kodt, //Список материалов которые надо разузловать
iceb_u_double *podr_mz_kodt_kolih, //Количество материалов, которые надо разузловать
iceb_u_spisok *podr_kodmv, //список входяших кодов материалов
iceb_u_double *podr_kodmv_kolih, //Количество входящих кодов материалов
GtkWidget *wpredok)
{
int nomer=0;
char pod_kod[300];

char podr[30];
char metka[10];
char kod[30];

int kolih=podr_mz_kodt->kolih();

for(int i=0; i < kolih; i++)
 {
  iceb_u_polen(podr_mz_kodt->ravno(i),podr,sizeof(podr),1,'|');
  iceb_u_polen(podr_mz_kodt->ravno(i),metka,sizeof(metka),2,'|');
  iceb_u_polen(podr_mz_kodt->ravno(i),kod,sizeof(kod),3,'|');

  //iceb_u_int kodmu;
  //iceb_u_double kolihmu;
  //iceb_u_int metka_zap; //Список меток записей 0-материал 1-услуга
  class iceb_razuz_data data;
  data.kod_izd=atoi(kod);
//  if(iceb_razuz_kod(atoi(kod),&kodmu,&kolihmu,&metka_zap,wpredok) == 0)
  if(iceb_razuz_kod(&data,wpredok) == 0)
   {
    
    sprintf(pod_kod,"%s|%s|%s",podr,metka,kod);
    if((nomer=podr_kodmv->find(pod_kod)) < 0)
     podr_kodmv->plus(pod_kod);

    podr_kodmv_kolih->plus(podr_mz_kodt_kolih->ravno(i),nomer);
    continue;        
   }

  int kolih_mat=data.kod_det_ei.kolih();
  for(int i1=0 ; i1 < kolih_mat; i1++)
   {
//    printf("%s|%s|%s\n",podr,metka,data.kod_det_ei.ravno(i1));
    /*нужно помнить что data.kod_det_ei состоит из кода детали|единицы измерения*/
    sprintf(pod_kod,"%s|%d|%s",podr,data.metka_mu.ravno(i1),data.kod_det_ei.ravno(i1));
    if((nomer=podr_kodmv->find(pod_kod)) < 0)
     podr_kodmv->plus(pod_kod);
    podr_kodmv_kolih->plus(data.kolih_det_ei.ravno(i1)*podr_mz_kodt_kolih->ravno(i),nomer);
   }
 
 }
}
