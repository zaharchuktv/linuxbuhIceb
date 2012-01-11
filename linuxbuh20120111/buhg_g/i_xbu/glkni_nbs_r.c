/*$Id: glkni_nbs_r.c,v 1.27 2011-02-21 07:35:51 sasa Exp $*/
/*12.02.2010	29.03.2004	Белых А.И.	glkni_nbs_r.c
Расчет оборотного баланса по небалансовым счетам
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "glkni_nbs.h"


class glkni_nbs_r_data
 {

  public:


  glkni_nbs_rr *rek_r;


  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  gfloat kolstr1; //Количество прочитанных строк
  int    kolstr;  //Количество строк в курсоре
  iceb_u_spisok imaf;
  iceb_u_spisok naim;

  short dn,mn,gn;
  short dk,mk,gk;
  short  koolk;       //Корректор отсупа распечатки от левого края
  int sb; //0-расчет по субсчетам 1-расчет по счетам
  int godn;
  time_t vremn;
  SQLCURSOR cur;

  iceb_u_spisok SHET; //Общий список счетов
  iceb_u_spisok SHETRS; //Список счетов с развернутым сальдо
  iceb_u_spisok KONTR; //Общий список контрагентов

  iceb_u_double DEB; //Дебеты счетов до периода
  iceb_u_double KRE; //Кредиты счетов до периода
  iceb_u_double DEB1; //Дебеты счетов за период
  iceb_u_double KRE1; //Кредиты счетов за период
  iceb_u_double DEBRS; //Дебеты по счет/контрагент до периода
  iceb_u_double KRERS; //Кредиты по счет/контрагент до периода
  iceb_u_double DEBRS1; //Дебеты по счет/контрагент периода
  iceb_u_double KRERS1; //Кредиты по счет/контрагент периода
  int kolshet;
  int kolshetrs;
  double ideb,ikre,ideb1,ikre1,ideb2,ikre2;
  int kolstrok;
  int kollist;
  FILE *ff;
  int nomshet;
  short metka_zav;    
  //Конструктор
  glkni_nbs_r_data()
   {
    metka_zav=1;
    godn=0;
    sb=1;
    kolstr=0;
    kolstr1=0.;
    ideb=ikre=ideb1=ikre1=ideb2=ikre2=0.;
    kollist=0;
    nomshet=0;
   }

  //Записать данные
  int glkni_nbs_r_zapis(char*,char*,int,double,double);

  //Поиск записи
  int glkni_nbs_r_poisk(char*,char*,int,double*,double*);


 };

gboolean   glkni_nbs_r_key_press(GtkWidget *widget,GdkEventKey *event,class glkni_nbs_r_data *data);
gint glkni_nbs_r1(class glkni_nbs_r_data *data);
void  glkni_nbs_r_v_knopka(GtkWidget *widget,class glkni_nbs_r_data *data);

void	sapoborbal(int list,FILE *ff);
void	shetstnbs(int *kolstr,int *kollist,FILE *ff);
void            rasbnsrs(char *sh,char *nash,int vids,class glkni_nbs_r_data *data);

extern SQL_baza bd;
extern short startgodb;
extern char *organ;
extern char *sbshet; //Список бюджетных счетов
extern char *name_system;
extern int kol_strok_na_liste;
extern iceb_u_str shrift_ravnohir;

void glkni_nbs_r(class glkni_nbs_rr *datap)
{
glkni_nbs_r_data data;

data.rek_r=datap;
char strsql[320];
data.sb=data.rek_r->metka_r;
iceb_u_str soob;

//printf("glkni_nbs_r\n");

if(iceb_rsdatp(&data.dn,&data.mn,&data.gn,data.rek_r->datan.ravno(),
&data.dk,&data.mk,&data.gk,data.rek_r->datak.ravno(),NULL) != 0)
  return;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Главная книга"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(glkni_nbs_r_key_press),&data);

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

sprintf(strsql,gettext("Расчет оборотного баланса по внебалансовым счетам"));
soob.new_plus(strsql);

sprintf(strsql,"%s %d.%d.%d => %d.%d.%d",gettext("Расчет за период"),
data.dn,data.mn,data.gn,
data.dk,data.mk,data.gk);
soob.ps_plus(strsql);

GtkWidget *label=gtk_label_new(soob.ravno_toutf());

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



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
GtkTooltips *tooltops=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltops,data.knopka,gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(glkni_nbs_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);

//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна


gtk_idle_add((GtkFunction)glkni_nbs_r1,&data);

gtk_main();

if(data.imaf.kolih() > 0)
  iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  glkni_nbs_r_v_knopka(GtkWidget *widget,class glkni_nbs_r_data *data)
{
// printf("glkni_nbs_r_v_knopka\n");
 if(data->metka_zav != 0)
  return;
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   glkni_nbs_r_key_press(GtkWidget *widget,GdkEventKey *event,class glkni_nbs_r_data *data)
{
// printf("glkni_nbs_r_key_press\n");

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

gint glkni_nbs_r1(class glkni_nbs_r_data *data)
{
char strsql[512];
//SQL_str row;
//GtkTextIter iter;
iceb_u_str soob;

time(&data->vremn);

data->godn=startgodb;
if(startgodb == 0 || startgodb > data->gn)
 data->godn=data->gn;

sprintf(strsql,"%s:%d\n",gettext("Стаpтовый год"),data->godn);
soob.new_plus(strsql);

//printw(gettext("Формируем массив счетов.\n"));


/*Формируем массив счетов*/

sprintf(strsql,"select ns,vids,saldo from Plansh where stat=1 \
order by ns asc");
soob.plus(strsql);

SQLCURSOR cur;
SQL_str row;
int kolstr;
  
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("В плане счетов нет небалансовых счетов !"));
  iceb_menu_soob(&repl,data->window);
  return(FALSE);
 }

int pozz=0;
char shet[32];
while(cur.read_cursor(&row) != 0)
 {
  pozz++;

  if(atoi(row[1]) == 1 && data->sb == 0)
    continue;
  strncpy(shet,row[0],sizeof(shet)-1);
  if(data->sb == 1 && atoi(row[1]) == 0 && pozz < kolstr-1)
   {
    cur.read_cursor(&row);
    /*Если нет субсчетов запоминаем счет*/
    if(iceb_u_SRAV(shet,row[0],1) != 0)
     {
      cur.poz_cursor(pozz-1);
      cur.read_cursor(&row);
     }
    else
     pozz++;
   }

  data->SHET.plus(row[0]);
  if(atoi(row[2]) == 3)
    data->SHETRS.plus(row[0]);

 }
data->kolshet=data->SHET.kolih();
data->kolshetrs=data->SHETRS.kolih();

sprintf(strsql,"%s: %d\n",gettext("Количество счетов"),data->kolshet);
soob.new_plus(strsql);

sprintf(strsql,"%s: %d\n",gettext("Количество счетов с развёрнутым сальдо"),data->kolshetrs);
soob.new_plus(strsql);

data->DEB.make_class(data->kolshet);
data->KRE.make_class(data->kolshet);

data->DEB1.make_class(data->kolshet);
data->KRE1.make_class(data->kolshet);

int kolkontr;
//Определяем количество контрагентов
for(int i=0; i < data->kolshetrs ;i ++)
 {
 //    SHETRS->pol_masiv_char(i+1,shet);
  sprintf(strsql,"select distinct kodkon from Skontr where ns='%s'",data->SHETRS.ravno(i));
  if((kolkontr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  while(cur.read_cursor(&row) != 0)
    if(data->KONTR.find(row[0]) < 0)
       data->KONTR.plus(row[0]);

 }
 //  printw("%s:%d\n", gettext("Количество контрагентов"),KONTR->kolelem_masiv_char());

int razmer=data->kolshetrs*data->KONTR.kolih();
if(razmer > 0)
 {
  data->DEBRS.make_class(razmer);
  data->KRERS.make_class(razmer);
  data->DEBRS1.make_class(razmer);
  data->KRERS1.make_class(razmer);
 }

/***********************************/
/*Узнаем стартовые сальдо по счетам*/
/***********************************/
  //  printw(gettext("Вычисляем стартовые сальдо по счетам.\n"));

for(int i=0; i < data->kolshet ; i++)
 {

  sprintf(strsql,"select saldo,vids from Plansh where ns='%s'",data->SHET.ravno(i));
  SQLCURSOR cur1;    
  
  if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
   {
    iceb_u_str repl;
   
    sprintf(strsql,gettext("Нет счёта %s в плане счетов !"),data->SHET.ravno(i));
    repl.plus(strsql);
    iceb_menu_soob(&repl,data->window);
    continue;
   }

  int tipsaldo=0;
  if(atoi(row[0]) == 3)
    tipsaldo=1;
  int vids=atoi(row[1]);
  
  if(vids == 0) /*Счет*/
   sprintf(strsql,"select ns,deb,kre,kodkon from Saldo where kkk='%d' \
and gs=%d and ns like '%s%%'",tipsaldo,data->godn,shet);

  if(vids == 1) /*Субсчет*/
   sprintf(strsql,"select ns,deb,kre,kodkon from Saldo where kkk='%d' \
and gs=%d and ns='%s'",tipsaldo,data->godn,shet);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

   if(kolstr == 0)
     continue;

  double deb=0.,kre=0.;
  while(cur.read_cursor(&row) != 0)
   {
    deb+=atof(row[1]);
    kre+=atof(row[2]);
   }

  if(tipsaldo == 0)  /*Счета с не развернутым сальдо*/
   {
    if(deb >= kre)
     {
      deb-=kre;
      kre=0.;
     }
    else
     {
      kre-=deb;
      deb=0.;
     }
   }

  data->DEB.plus(deb,i);
  data->KRE.plus(kre,i);

 }

/******************************************/
/*Просматриваем записи и заполняем массивы*/
/******************************************/

sprintf(strsql,"%s\n",gettext("Просматриваем проводки, заполняем массивы"));
soob.plus(strsql);
iceb_printw(soob.ravno_toutf(),data->buffer,data->view);

sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre from Prov \
where val=-1 and datp >= '%d-%02d-%02d' and \
datp <= '%d-%02d-%02d'",data->godn,1,1,data->gk,data->mk,data->dk);
printf("%s\n",strsql);

if((data->kolstr=data->cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(data->kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
 }



short den,mes,god;  
char  kontr[30];
double deb,kre;
int    i,i1,i2=0;

while(data->cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

  iceb_u_rsdat(&den,&mes,&god,row[0],2);
  strcpy(shet,row[1]);
  strcpy(kontr,row[3]);
  deb=atof(row[4]);   
  kre=atof(row[5]);

  if((i=data->SHET.find(shet)) < 0)
   {
    iceb_u_str repl;
    
    sprintf(strsql,"%s %s %s %s %s %s",
    row[0],row[1],row[2],row[3],row[4],row[5]);
    repl.plus(strsql);
    
    sprintf(strsql,gettext("Не найден счет %s в массиве счетов !"),shet);
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,data->window);      

    continue;
   }

  if((i1=data->SHETRS.find(shet)) >= 0)
    i2=data->KONTR.find(kontr);

  if(iceb_u_sravmydat(den,mes,god,data->dn,data->mn,data->gn) < 0)
   {
    data->DEB.plus(deb,i);
    data->KRE.plus(kre,i);
    if(i1 >= 0 && i2 >= 0)
     {
      data->DEBRS.plus(deb,i1*data->kolshetrs+i2);
      data->KRERS.plus(kre,i1*data->kolshetrs+i2);
     }
   }
  else
   {

    data->DEB1.plus(deb,i);
    data->KRE1.plus(kre,i);
    if(i1 >= 0 && i2 >= 0)
     {
      data->DEBRS1.plus(deb,i1*data->kolshetrs+i2);
      data->KRERS1.plus(kre,i1*data->kolshetrs+i2);
     }
   }

 }


data->naim.plus(gettext("Распечатка проводок с дневными итогами"));  

char imaf[30];

sprintf(imaf,"obnds%d.lst",getpid());

if((data->ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return(FALSE);
 }
iceb_u_startfil(data->ff);

fprintf(data->ff,"\x12");
fprintf(data->ff,"\x1b\x6C%c",8); /*Установка левого поля*/
fprintf(data->ff,"\x1B\x4D"); /*12-знаков*/
fprintf(data->ff,"\x0F");  /*Ужатый режим*/

sprintf(strsql,"%s (%s)",gettext("Оборотный баланс"),
gettext("Внебалансовый"));
data->naim.plus(strsql);
data->imaf.plus(imaf);

iceb_u_zagolov(strsql,data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,organ,data->ff);

data->kollist=1;
sapoborbal(data->kollist,data->ff);
data->kolstrok=11;


data->kolstr1=0.;
data->kolstr=data->SHET.kolih();

while(data->nomshet < data->kolstr)
 {
  data->nomshet++;
  
  iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
  memset(shet,'\0',sizeof(shet));
  strcpy(shet,data->SHET.ravno(data->nomshet-1));
  
  if(iceb_u_proverka(data->rek_r->shet.ravno(),shet,1,0) != 0)
    continue;

  sprintf(strsql,"select saldo,nais,vids from Plansh where ns='%s'",shet);
  SQLCURSOR cur1;    
  SQL_str   row;
  if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
   {
    iceb_u_str repl;
   
    sprintf(strsql,gettext("Нет счёта %s в плане счетов !"),shet);
    repl.plus(strsql);
    iceb_menu_soob(&repl,data->window);
    continue;
   }
  int vids=atoi(row[2]);
  int saldo=atoi(row[0]);
  char nais[100];
  strncpy(nais,row[1],sizeof(nais)-1);  

  sprintf(strsql,"%-5s %s\n",shet,nais);
  soob.new_plus(strsql);
  if(saldo == 3)
   {
    rasbnsrs(shet,nais,vids,data);
   }
  else
   {
    
    double deb=data->DEB.ravno(data->nomshet-1);
    double kre=data->KRE.ravno(data->nomshet-1);
    double deb1=data->DEB1.ravno(data->nomshet-1);
    double kre1=data->KRE1.ravno(data->nomshet-1);
    double deb2=deb+deb1;
    double kre2=kre+kre1;     
    
    if(deb == 0. && kre == 0. && deb1 == 0. && kre1 == 0. &&\
    deb2 == 0. && kre2 == 0.)
      continue;
    shetstnbs(&data->kolstrok,&data->kollist,data->ff);

    if(deb > kre)
     {
      data->ideb+=deb-kre;
      fprintf(data->ff,"%-*.*s %15.2f %15s %15.2f %15.2f",
      iceb_u_kolbait(13,shet),iceb_u_kolbait(13,shet),shet,deb-kre," ",deb1,kre1);
     }
    else
     {
      data->ikre+=kre-deb;
      fprintf(data->ff,"%-*.*s %15s %15.2f %15.2f %15.2f",
      iceb_u_kolbait(13,shet),iceb_u_kolbait(13,shet),shet," ",kre-deb,deb1,kre1);
     }

    data->ikre1+=kre1;
    data->ideb1+=deb1;

    if(deb2 > kre2)
     {
      data->ideb2+=deb2-kre2;
      fprintf(data->ff," %15.2f\n",deb2-kre2);
     }
    else
     {
      data->ikre2+=kre2-deb2;
      fprintf(data->ff," %15s %15.2f\n"," ",kre2-deb2);
     }
   }

  iceb_printw(soob.ravno_toutf(),data->buffer,data->view);

 }

fprintf(data->ff,"\
-------------------------------------------------------------------------------------------------------------\n\
%-*s %15.2f %15.2f %15.2f %15.2f %15.2f %15.2f\n",
iceb_u_kolbait(13,gettext("Итого")),gettext("Итого"),
data->ideb,data->ikre,data->ideb1,data->ikre1,data->ideb2,data->ikre2);

sprintf(strsql,"\n%*s:%15s",iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),iceb_u_prnbr(data->ideb));
soob.new_plus(strsql);

sprintf(strsql," %15s\n",iceb_u_prnbr(data->ikre));
soob.plus(strsql);

sprintf(strsql,"%*s:%15s",iceb_u_kolbait(20,gettext("Оборот за период")),gettext("Оборот за период"),iceb_u_prnbr(data->ideb1));
soob.plus(strsql);

sprintf(strsql," %15s\n",iceb_u_prnbr(data->ikre1));
soob.plus(strsql);

sprintf(strsql,"%*s:%15s",iceb_u_kolbait(20,gettext("Сальдо конечное")),gettext("Сальдо конечное"),iceb_u_prnbr(data->ideb2));
soob.plus(strsql);

sprintf(strsql," %15s\n",iceb_u_prnbr(data->ikre2));
soob.plus(strsql);


iceb_podpis(data->ff,data->window);
fprintf(data->ff,"\x1B\x50"); /*10-знаков*/
fprintf(data->ff,"\x12");  /*Нормальный режим печати*/
fprintf(data->ff,"\x1b\x6C%c",1); /*Установка левого поля в ноль*/

fclose(data->ff);


iceb_printw(soob.ravno_toutf(),data->buffer,data->view);



iceb_printw_vr(data->vremn,data->buffer,data->view);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
data->metka_zav=0;
return(FALSE);

}

/*******************************/
/*Шарка оборотного баланса     */
/*******************************/
void	sapoborbal(int list,FILE *ff)
{

fprintf(ff,"%100s%s%d\n","",gettext("Лист N"),list);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("             |          C а л ь д о          |         О б о р о т           |         С а л ь д о          |\n"));
fprintf(ff,gettext("  Счёт    ------------------------------------------------------------------------------------------------\n"));
fprintf(ff,gettext("             |    Дебет      |    Кредит     |    Дебет      |     Кредит    |    Дебет      |    Кредит    |\n"));

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------\n");
}

/***************/
/*Счетчик строк*/
/***************/
void	shetstnbs(int *kolstr,int *kollist,FILE *ff)
{
*kolstr+=1;
if(*kolstr > kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  *kollist+=1;
  *kolstr=6;
  sapoborbal(*kollist,ff);
 }
}
/****************************************/
/*Счет с развернутым сальдо              */
/*****************************************/
void            rasbnsrs(char *sh,
char *nash,  //Наименование счета
int vids,  //0-счет 1-субсчет
class glkni_nbs_r_data *data)
{
short           i1,i2;
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
double		db=0.,kr=0.,db1=0.,kr1=0.;
double		deb=0.,kre=0.,deb1=0.,kre1=0.,deb2=0.,kre2=0.;

/*
printw("Обрабатываем счет с развернутым сальдо.\n");
printw("Счет %s\n",sh);
refresh();
*/


/*printw("%d %s %s rs\n",i,sh,nash);*/

/******************
if(vplsh == 1)
 sprintf(strsql,"select kodkon,ns from Skontr where ns='%s'",sh);
if(vplsh == 0 && sb == 0)
 sprintf(strsql,"select kodkon,ns from Skontr where ns like '%s%%'",sh);
if(vplsh == 0 && sb == 1)
 sprintf(strsql,"select kodkon,ns from Skontr where ns='%s'",sh);
*********************/
if(data->rek_r->metka_r == 0)
 sprintf(strsql,"select kodkon,ns from Skontr where ns like '%s%%'",sh);
if(data->rek_r->metka_r == 1)
 sprintf(strsql,"select kodkon,ns from Skontr where ns='%s'",sh);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);


if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {

  /*Определяем стартовое сальдо по организации*/

   sprintf(strsql,"select deb,kre,ns from Saldo where kkk='1' and gs=%d and \
ns like '%s' and kodkon='%s'",data->godn,row[1],row[0]);

   db=kr=db1=kr1=0.;
   
   SQLCURSOR cur1;
   if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
   while(cur1.read_cursor(&row1) != 0)
    {
     db+=atof(row1[0]);
     kr+=atof(row1[1]);
    }
   i2=data->KONTR.find(row[0]);
   i1=data->SHETRS.find(row[1]);
   
   if(i1 >= 0 && i2 >= 0)
    {

     db+=data->DEBRS.ravno(i1*data->kolshetrs+i2);
     kr+=data->KRERS.ravno(i1*data->kolshetrs+i2);
     db1+=data->DEBRS1.ravno(i1*data->kolshetrs+i2);
     kr1+=data->KRERS1.ravno(i1*data->kolshetrs+i2);
    }

   if(db > kr)
     deb+=db-kr;
   else
     kre+=kr-db;

   deb1+=db1;
   kre1+=kr1;

   if(db+db1 > kr+kr1)
     deb2+=(db+db1)-(kr+kr1);
   else
     kre2+=(kr+kr1)-(db+db1);
 }
if(fabs(deb) < 0.01 && fabs(kre) < 0.01 && fabs(deb1) < 0.01
&& fabs(kre1) < 0.01 && fabs(deb2) < 0.01 && fabs(kre2) < 0.01)
  return;

shetstnbs(&data->kolstrok,&data->kollist,data->ff);

fprintf(data->ff,"%-*s %15.2f %15.2f %15.2f %15.2f %15.2f %15.2f\n",
iceb_u_kolbait(13,sh),sh, deb, kre, deb1, kre1 , deb2 ,kre2);

data->ideb+=deb;
data->ikre+=kre;
data->ideb1+=deb1;
data->ikre1+=kre1;
data->ideb2+=deb2;
data->ikre2+=kre2;

}
