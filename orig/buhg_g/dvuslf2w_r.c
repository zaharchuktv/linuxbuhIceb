/*$Id: dvuslf2w_r.c,v 1.11 2011-02-21 07:35:51 sasa Exp $*/
/*11.11.2009	16.12.2005	Белых А.И.	dvuslf2w_r.c
Расчёт движения услуг
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "dvuslf2w.h"

class dvuslf2w_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class dvuslf2w_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  dvuslf2w_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   dvuslf2w_r_key_press(GtkWidget *widget,GdkEventKey *event,class dvuslf2w_r_data *data);
gint dvuslf2w_r1(class dvuslf2w_r_data *data);
void  dvuslf2w_r_v_knopka(GtkWidget *widget,class dvuslf2w_r_data *data);

void gsapp1(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,FILE *ff);
void itgdvus(short met,char kod[],char naik[],double *ikol,double *isum,int *kst,FILE *ff);
void gsapp1_shetl(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,FILE *ff);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern double	okrg1;  /*Округление 1*/
extern double	okrcn;  /*Округление цены*/
extern int      kol_strok_na_liste;
extern iceb_u_str shrift_ravnohir;

int dvuslf2w_r(class dvuslf2w_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class dvuslf2w_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечетать движение товарно-материальных ценностей (форма 1)"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(dvuslf2w_r_key_press),&data);

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

repl.plus(gettext("Распечетать движение товарно-материальных ценностей (форма 1)"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(dvuslf2w_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)dvuslf2w_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  dvuslf2w_r_v_knopka(GtkWidget *widget,class dvuslf2w_r_data *data)
{
//printf("dvuslf2w_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   dvuslf2w_r_key_press(GtkWidget *widget,GdkEventKey *event,class dvuslf2w_r_data *data)
{
// printf("dvuslf2w_r_key_press\n");
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

gint dvuslf2w_r1(class dvuslf2w_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);

short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int tipz=data->rk->prr;


sprintf(strsql,"select * from Usldokum2 where datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' order by shetu,podr,datp asc",
gn,mn,dn,gk,mk,dk);
SQLCURSOR cur;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf[56];
sprintf(imaf,"dv%d.lst",getpid());

data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Движение услуг"));

FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_startfil(ff);

fprintf(ff,"\x1B\x4D");
fprintf(ff,"\x0F");

iceb_u_zagolov(gettext("Движение услуг"),dn,mn,gn,dk,mk,gk,organ,ff);
int kst=5;

if(tipz == 0)
 {
  fprintf(ff,"%s\n",gettext("Перечень приходов/расходов"));
  kst++;
 }
if(tipz == 1)
 {
  fprintf(ff,"%s\n",gettext("Перечень приходов"));
  kst++;
 }
if(tipz == 2)
 {
  fprintf(ff,"%s\n",gettext("Перечень расходов"));
  kst++;
 }

if(data->rk->podr.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Подразделение"),data->rk->podr.ravno());
  kst++;
 }
if(data->rk->kontr.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код контрагента"),data->rk->kontr.ravno());
  kst++;
 }
if(data->rk->kodop.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
  kst++;
 }
if(data->rk->grupa.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Группа"),data->rk->grupa.ravno());
  kst++;
 }
if(data->rk->kodusl.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код услуги"),data->rk->kodusl.ravno());
  kst++;
 }
if(data->rk->shet.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Счета учета"),data->rk->shet.ravno());
  kst++;
 }
 
int podr=0;
int mvstr=0,podrl=0;

sprintf(strsql,"%s %d.%d.%d %s %d.%d.%d\n",gettext("Период с"),dn,mn,gn,gettext("по"),
dk,mk,gk);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

float kolstr1=0.;
char shet1[30];
memset(shet1,'\0',sizeof(shet1));
char shet[32];
memset(shet,'\0',sizeof(shet));
double ishetk=0.,ishets=0.;
int sli=0;
char naimsh[200];
double itk=0,its=0.;
SQL_str row;
SQL_str row1;
SQLCURSOR cur1;
double cena=0.;
double suma=0.;
double kolih=0.;
char ei[32];
char kontr[20];
char kodop[32];
short metka=0;
int kodzap=0;
char naim[512];
short d,m,g;
short dp,mp,gp;
char naim_kontr[512];

gsapp1(dn,mn,gn,dk,mk,gk,&sli,&kst,ff);


while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  podr=atoi(row[10]);
  strncpy(shet,row[9],sizeof(shet)-1);  

  if(iceb_u_proverka(data->rk->shet.ravno(),shet,1,0) != 0)
     continue;

  if(iceb_u_proverka(data->rk->podr.ravno(),row[10],0,0) != 0)
    continue;

  if(tipz != 0 )
   if(tipz != atoi(row[0]))
       continue;


  
  /*Читаем накладную*/
  sprintf(strsql,"select kontr,kodop from Usldokum \
where datd='%s' and podr=%d and nomd='%s' and tp=%s",
  row[1],podr,row[3],row[0]);

  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найден документ !"));

    sprintf(strsql,"%s=%s %s=%s %s=%d",
    gettext("Дата"),row[4],
    gettext("Документ"),row[2],
    gettext("Подраделение"),podr);   
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,data->window);

    fprintf(ff,gettext("Не найден документ !"));
    fprintf(ff,"%s=%s %s=%s %s=%d\n",
    gettext("Дата"),row[4],
    gettext("Документ"),row[2],
    gettext("Подраделение"),podr);   
    continue;
   }

  strncpy(kontr,row1[0],sizeof(kontr)-1);
  strncpy(kodop,row1[1],sizeof(kodop)-1);

  if(iceb_u_proverka(data->rk->kontr.ravno(),kontr,0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->kodop.ravno(),kodop,0,0) != 0)
    continue;

  metka=atoi(row[4]);
  kodzap=atoi(row[5]);
  cena=atof(row[7]);
  strncpy(ei,row[8],sizeof(ei)-1);


  if(iceb_u_proverka(data->rk->kodusl.ravno(),row[5],0,0) != 0)
    continue;

  if(metka == 0)
   {
    /*Узнаем наименование материалла*/
    sprintf(strsql,"select naimat,kodgr from Material where kodm=%d",
    kodzap);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      fprintf(ff,"%s %d !\n",gettext("Не найден код материалла"),kodzap);
      sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodzap);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
   }

  if(metka == 1)
   {
    /*Читаем наименование услуги*/
    sprintf(strsql,"select naius,kodgr from Uslugi where kodus=%d",
    kodzap);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,"%s %d !\n",gettext("Не найден код услуги"),kodzap);
      fprintf(ff,"%s %d !\n",gettext("Не найден код услуги"),kodzap);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
   }

  memset(naim,'\0',sizeof(naim));
  strncpy(naim,row1[0],sizeof(naim));

  if(iceb_u_proverka(data->rk->grupa.ravno(),row1[1],0,0) != 0)
     continue;

  if(podrl != podr)
   {
    if(podrl != 0)
     {
      gsapp1_shetl(dn,mn,gn,dk,mk,gk,&sli,&kst,ff);
      fprintf(ff,"%s %d",gettext("Подразделение"),podr);
      fprintf(ff,"----------------------------------\n");
     } 
    podrl=podr;
   } 


  if(iceb_u_SRAV(shet1,shet,0) != 0)
   {
    if(shet1[0] != '\0')
      itgdvus(0,shet1,naimsh,&ishetk,&ishets,&kst,ff);

    memset(naimsh,'\0',sizeof(naimsh));
    sprintf(strsql,"select nais from Plansh where ns='%s'",shet);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,gettext("Не найден счет %s в плане счетов !"),shet);
      iceb_menu_soob(strsql,data->window);
     }
    else
     strncpy(naimsh,row1[0],sizeof(naimsh)-1);   

    gsapp1_shetl(dn,mn,gn,dk,mk,gk,&sli,&kst,ff);

    fprintf(ff,"%s %s %s\n",gettext("Счет"),shet,naimsh);
    strcpy(shet1,shet);
   }

  if(metka == 1) //Смотрим дополнение к наименованию
   {
    sprintf(strsql,"select dnaim from Usldokum1 where datd='%s' and \
podr=%s and nomd='%s' and metka=1 and kodzap=%s and tp=%s",
    row[1],row[10],row[3],row[5],row[0]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     {
      strcat(naim," ");
      strcat(naim,row1[0]);
     }    
   }     
  

  /*printw("%d %s\n",mt5.nkr,naim);*/
  mvstr=0;
  
  iceb_u_rsdat(&dp,&mp,&gp,row[2],2);
  iceb_u_rsdat(&d,&m,&g,row[1],2);
  kolih=atof(row[6]);
  

  /*Узнаем наименование контрагента*/
  if(kontr[0] == '0' && iceb_u_pole(kontr,strsql,1,'-') == 0)
     strcpy(kontr,strsql);   

  memset(naim_kontr,'\0',sizeof(naim_kontr));
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'", kontr);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
     fprintf(ff,"%s %s !\n",gettext("Не найден код контрагента"),kontr);
     sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kontr);
     iceb_menu_soob(strsql,data->window);
   }
  else
    strncpy(naim_kontr,row1[0],sizeof(naim_kontr)-1);

  gsapp1_shetl(dn,mn,gn,dk,mk,gk,&sli,&kst,ff);

  if(atoi(row[0]) == 1)
     strcat(kodop,"+");
  if(atoi(row[0]) == 2)
     strcat(kodop,"-");

  sprintf(strsql,"%s\n",naim);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  if(mvstr == 0)
   {
     
    fprintf(ff,"\
%-5d %-*.*s %-*s %-*s %14.10g %12.12g %15.2f %15s %02d.%02d.%d %02d.%02d.%d %-7s %3s %s\n",
    kodzap,
    iceb_u_kolbait(35,naim),iceb_u_kolbait(35,naim),naim,
    iceb_u_kolbait(5,ei),ei,
    iceb_u_kolbait(7,row[9]),row[9],
    cena,
    kolih,kolih*cena,row[3],d,m,g,dp,mp,gp,kodop,kontr,naim_kontr);
    if(iceb_u_strlen(naim) > 35)
     {
      gsapp1_shetl(dn,mn,gn,dk,mk,gk,&sli,&kst,ff);
      fprintf(ff,"%5s %s\n"," ",iceb_u_adrsimv(35,naim));
     }
   }
  else
   {
/*
    printw("%s %s %s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],
    row[5],row[6],row[7]);
    printw("cena=%f\n",cena);
*/
    fprintf(ff,"\
%5s %-35s %-15s %-*s %-*s %14.10g %12.12g %15.2f %15s %02d.%02d.%d %02d.%02d.%d %-7s %3s %s\n",
    " "," "," ",
    iceb_u_kolbait(5,ei),ei,
    iceb_u_kolbait(7,row[9]),row[9],
    cena,
    kolih,kolih*cena,row[3],d,m,g,dp,mp,gp,kodop,kontr,naim_kontr);
   }  
  
  suma=cena*kolih;
  suma=iceb_u_okrug(suma,0.01);
  its+=suma;
  itk+=kolih;
  ishetk+=kolih;
  ishets+=suma;  

  mvstr++;

 }


//Итого по счету
itgdvus(0,shet1,naimsh,&ishetk,&ishets,&kst,ff);

//Общий итог
ishetk=itk; //Чтобы не обнулились результаты
ishets=its;
itgdvus(1,shet1,naimsh,&ishetk,&ishets,&kst,ff);

sprintf(strsql,"%s:\n\
%-*s %12.12g\n\
%-*s %12.2f%s\n",
gettext("Итого"),
iceb_u_kolbait(10,gettext("количество")),gettext("количество"),
itk,
iceb_u_kolbait(10,gettext("сумма")),gettext("сумма"),
its,
gettext("грн.")); 

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

//fprintf(ff,"\x1B\x50");
//fprintf(ff,"\x12");

fclose(ff);



data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
/***********************************/
/*Выдача шапки*/
/**********************************/
void            gsapp1(short dn,short mn,short gn,short dk,short mk,
short gk,
int *sl, //Счетчик листов
int *kst,  //Счетчик строк
FILE *ff)
{
*sl+=1;

fprintf(ff,"\
%s %02d.%02d.%4d%s %s %02d.%02d.%4d%s %100s%sN%d\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
"",
gettext("Лист"),
*sl);

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext(" Код |        Наименование               |Един.| Счет  |              |            |               |               | Д а т а  | Д а т а  |       |\n\
     |          услуги                   |изме-| учета |   Ц е н а    | Количество |     Сумма     |Номер документа| выписки  |подтверж- |Код оп.|         К о н т р а г е н т\n\
     |                                   |рения|       |              |            |               |               |документа | дения    |       |\n"));

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

*kst+=6;

}
/**************/
/*Выдача итога*/
/**************/

void		itgdvus(short met, //0-по счету 1 общий итог
char kod[],char naik[],double *ikol,double *isum,
int *kst,FILE *ff)
{
char		bros[512];

if(met == 0)
 sprintf(bros,"%s %s %s",gettext("Итого по счету"),kod,naik);
if(met == 1)
 sprintf(bros,"%s",gettext("Общий итог"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%*.*s:%12.12g %15.2f\n",iceb_u_kolbait(70,bros),iceb_u_kolbait(70,bros),bros,*ikol,*isum);

*kst=*kst+2;

*ikol=0.;
*isum=0.;
}
/*******/
/*Шапка*/
/*******/
void            gsapp1_shetl(short dn,short mn,short gn,short dk,short mk,
short gk,
int *sl, //Счетчик листов
int *kst,  //Счетчик строк
FILE *ff)
{
*kst+=1;

if( *kst <= kol_strok_na_liste)
 return;

*kst=0;
fprintf(ff,"\f");

gsapp1(dn,mn,gn,dk,mk,gk,sl,kst,ff);
*kst+=1;
}
