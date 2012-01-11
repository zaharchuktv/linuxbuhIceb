/*$Id: rasndsw_r.c,v 1.22 2011-02-21 07:35:56 sasa Exp $*/
/*15.11.2009	24.04.2005	Белых А.И. 	rasndsw_r.c
Расчёт реестра проводок по видам операций
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "../headers/buhg_g.h"
#include "rasndsw.h"

class rasndsw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class rasndsw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  rasndsw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   rasndsw_r_key_press(GtkWidget *widget,GdkEventKey *event,class rasndsw_r_data *data);
gint rasndsw_r1(class rasndsw_r_data *data);
void  rasndsw_r_v_knopka(GtkWidget *widget,class rasndsw_r_data *data);

void		rasnak(int skl,short d,short m,short g,char nn[],
double *suma,double *sumabn,double *sp,double *spbn,
short vt,double *sumkor,int lnds,FILE *ff,FILE *ffnp,double *sumsnds,
GtkWidget *wpredok);

void  sapitsh(FILE *ff);
void  rasoi(iceb_u_spisok *mssi,iceb_u_double *mspsi,FILE *ff);
void            rasis(char orgn[],double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,char naiskl[],char naik[],
iceb_u_spisok *mso,
iceb_u_double *mspo,
iceb_u_spisok *mss,
iceb_u_double *msps,
int*,int*,
FILE *ff);


void            rasid(double itd,short dn,short mn,short gn,short dk,
short mk,short gk,int skl1,char naiskl[],int*,int*,FILE *ff);

void            gsapr(short dn,short mn,short gn,short dk,short mk,
short gk,int *sl,int *kst,int skl1,char naiskl[],FILE *ff);

void rppvo1(int skl1,char *naiskl,
short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,
int *kst,
FILE *ff);

void rasmasop(iceb_u_spisok *spopr,iceb_u_double *mkm,iceb_u_double *mkm1_1r,iceb_u_double *mkm1_1k,
short tip,FILE *ff,class rasndsw_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern double   okrg1;
extern double	okrcn;
extern short	vtara;

int rasndsw_r(class rasndsw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class rasndsw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать распределение НДС по видам операций"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rasndsw_r_key_press),&data);

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

repl.plus(gettext("Распечатать распределение НДС по видам операций"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(rasndsw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)rasndsw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rasndsw_r_v_knopka(GtkWidget *widget,class rasndsw_r_data *data)
{
//printf("rasndsw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rasndsw_r_key_press(GtkWidget *widget,GdkEventKey *event,class rasndsw_r_data *data)
{
// printf("rasndsw_r_key_press\n");
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

gint rasndsw_r1(class rasndsw_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;

iceb_u_spisok           spoprp;/*Массив с перечнем операций приходов*/
iceb_u_spisok           spoprr;/*Массив с перечнем операций расходов*/
iceb_u_double mkm1p; /*Массив приходов*/
iceb_u_double mkm1r; /*Массив расходов в  ценах накладной*/
iceb_u_double mkm1_1r; /*Массив расходов в  ценах карточек*/
iceb_u_double mkm1_1k; /*Массив кореектировки сумм*/
short dn,mn,gn;
short dk,mk,gk;

iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window);

sprintf(strsql,"%s %d.%d.%d%s %s %d.%d.%d%s\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

/*Создаем массивы операций-материалов*/
sprintf(strsql,"select kod from Prihod");

//dlmp=dlmr=kolopp=kolopr=0;
SQLCURSOR cur;
SQLCURSOR cur1;
SQLCURSOR cur2;
SQL_str row;
SQL_str row1;
SQL_str row2;

int kolstr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
  spoprp.plus(row[0]);

sprintf(strsql,"select kod from Rashod");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
  spoprr.plus(row[0]);

int kolopp=spoprp.kolih();
int kolopr=spoprr.kolih();

sprintf(strsql,"%s:%d\n",gettext("Количество операций приходов"),kolopp);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%s:%d\n",gettext("Количество операций расходов"),kolopr);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

if(kolopp == 0 && kolopr == 0)
 {
  iceb_menu_soob(gettext("Не нашли ни одной операции"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
//Маcсивы приходов
mkm1p.make_class(kolopp*6);

//*Маcсивы расходов
mkm1r.make_class(kolopr*6);
mkm1_1r.make_class(kolopr*6);
mkm1_1k.make_class(kolopr*6);


/*Просматриваем документы и заполняем массивы*/


sprintf(strsql,"select * from Dokummat where datd >= '%d-%02d-%02d' and \
datd <= '%d-%02d-%02d'",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного документа !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
 
char imaf[30];
char imaf1[30];
char imaf2[30];

sprintf(imaf,"op%d.lst",getpid());
sprintf(imaf1,"oh%d.lst",getpid());
sprintf(imaf2,"ohb%d.lst",getpid());
FILE *ff;
FILE *ff1;
FILE *ff2;

data->rk->imaf.new_plus(imaf);
data->rk->imaf.plus(imaf1);
data->rk->imaf.plus(imaf2);

data->rk->naimf.new_plus(gettext("Расчёт распределения НДС по видам операций"));
data->rk->naimf.plus(gettext("Протокол ошибок"));
data->rk->naimf.plus(gettext("Протокол ошибок"));


if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_startfil(ff);
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_startfil(ff1);

if((ff2 = fopen(imaf2,"w")) == NULL)
 {
  iceb_er_op_fil(imaf2,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_startfil(ff2);

fprintf(ff1,"%s\n\n%s.\n",organ,gettext("Контроль ошибок"));

fprintf(ff1,"\n\
--------------------------------------------------------------------------\n");

fprintf(ff1,gettext("\
  Дата    | Документ  |Опер|Код м|Колич.|Цена в док|Цена в кар|  Сумма   |\n"));
  
fprintf(ff1,"\
--------------------------------------------------------------------------\n");

fprintf(ff2,"%s\n\n%s.\n",organ,gettext("Контроль ошибок"));

fprintf(ff2,"\n\
--------------------------------------------------------------------------\n");

fprintf(ff2,gettext("\
  Дата    | Документ  |Опер|Код м|Колич.|Цена в док|Цена в кар|  Сумма   |\n"));
  
fprintf(ff2,"\
--------------------------------------------------------------------------\n");
fprintf(ff,"\x0F\n");

iceb_u_zagolov(gettext("Расчёт распределения НДС по видам операций"),dn,mn,gn,dk,mk,gk,organ,ff);


if(data->rk->sklad.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());

if(data->rk->pr_ras == 1)
  fprintf(ff,"%s\n",gettext("Только приходы"));
if(data->rk->pr_ras == 2)
  fprintf(ff,"%s\n",gettext("Только расходы"));

if(data->rk->kodop.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());

if(data->rk->kontr.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Контрагент"),data->rk->kontr.ravno());

if(data->rk->kodgrmat.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код группы материалла"),data->rk->kodgrmat.ravno());

if(data->rk->kodmat.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код материалла"),data->rk->kodmat.ravno());

if(data->rk->shet.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Счета учета"),data->rk->shet.ravno());



float kolstr1=0;
double itog=0.; 
double itog1=0.; 
char kor[30];
char kop[30];
int tp;
int skl;
int vo;
int kolstr2;
int nomop=0;
short d,m,g;
double suma=0.;
double sumao=0.;
double sumaok=0.;
double sumkor=0.;
double sumak=0.;
int mnds=0;
int nk;
double kolih;
double cena;
double cenak;
int vtr=0;
char shu[30];
double bb;
int kgrm;
double kol=0.;
float pnds=0.;
while(cur.read_cursor(&row) != 0)
 {
/*Если документ не подтвержден то его не смотрим*/
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(atoi(row[7]) == 0)
    continue;

  tp=atoi(row[0]);
  skl=atoi(row[2]);
  strncpy(kor,row[3],sizeof(kor)-1);
  strncpy(kop,row[6],sizeof(kop)-1);

  if(iceb_u_proverka(data->rk->sklad.ravno(),row[2],0,0) != 0)
    continue;  

  if(data->rk->pr_ras != 0 && data->rk->pr_ras != tp)
     continue;

  if(iceb_u_proverka(data->rk->kodop.ravno(),row[6],0,0) != 0)
    continue;  


  if(iceb_u_proverka(data->rk->kontr.ravno(),row[3],0,0) != 0)
    continue;  
  pnds=atof(row[13]);
  
  /*Читем код операции*/
  if(tp == 1)
    sprintf(strsql,"select vido from Prihod where kod='%s'",kop);
  if(tp == 2)
    sprintf(strsql,"select vido from Rashod where kod='%s'",kop);
  vo=0;
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код операции"),kop);
    iceb_menu_soob(strsql,data->window);
   }
  else
   vo=atoi(row1[0]);
  
  /*Читаем запис в накладной*/
  sprintf(strsql,"select * from Dokummat1 where datd='%s' and \
  sklad=%s and nomd='%s'",row[1],row[2],row[4]);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  if(kolstr2 == 0)
   {
    sprintf(strsql,"%s:%s\n",gettext("Нет записей в документе"),row[4]);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    continue;
   }
  if(tp == 1)
    {
     if((nomop=spoprp.find(kop)) < 0 )
      {
       sprintf(strsql,gettext("Не найден код операции %s в массиве операций приходов !\n"),kop);
       iceb_menu_soob(strsql,data->window);
       continue;
      }
    }
  if(tp == 2)
    {
     if((nomop=spoprr.find(kop)) < 0 )
      {
       sprintf(strsql,gettext("Не найден код операции %s в массиве операций расходов !\n"),kop);
       iceb_menu_soob(strsql,data->window);
       continue;
      }
    }   
   iceb_u_rsdat(&d,&m,&g,row[1],2);    
   sumao=sumaok=0.;

    
   while(cur1.read_cursor(&row1) != 0)
    {
     if(iceb_u_proverka(data->rk->kodmat.ravno(),row1[4],0,0) != 0)
       continue;

     nk=atoi(row1[3]);
     cena=atof(row1[6]);
     cena=iceb_u_okrug(cena,okrcn);
     vtr=atoi(row1[8]);     
     kolih=atof(row1[5]);

     
     /*Читаем карточку материалла*/
     if(nk > 0)
      {
       sprintf(strsql,"select cena,shetu from Kart where sklad=%d and nomk=%d",
       skl,nk);
       if(sql_readkey(&bd,strsql,&row2,&cur2) != 1)
        {
         sprintf(strsql,"%s %d %s %d !\n",gettext("Не нашли карточку"),
         nk,
         gettext("Склад"),
         skl);
         iceb_menu_soob(strsql,data->window);

         continue;
        }
      }
     cenak=atof(row2[0]);
     cenak=iceb_u_okrug(cenak,okrcn);
     strncpy(shu,row2[1],sizeof(shu)-1);

     if(cenak-cena > 0.009)
      {
       sprintf(strsql,"%02d.%02d.%d %-*s %*s %10.10g %10.10g\n",
       d,m,g,
       iceb_u_kolbait(11,row[4]),row[4],
       iceb_u_kolbait(5,row1[4]),row1[4],
       cena,cenak);
       iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
       
       bb=kolih*cenak - kolih*cena;

       fprintf(ff1,"%02d.%02d.%d %-*s %-*s %*s %6.6g %10.10g %10.10g %10.10g\n",
       d,m,g,
       iceb_u_kolbait(11,row[4]),row[4],
       iceb_u_kolbait(4,kop),kop,
       iceb_u_kolbait(5,row1[4]),row1[4],
       kolih,cena,cenak,bb);
       itog+=bb;
      }
     if(cenak-cena < 0.009)
      {
       bb=kolih*cena - kolih*cenak;
       fprintf(ff2,"%02d.%02d.%d %-*s %-*s %*s %6.6g %10.10g %10.10g %10.10g\n",
       d,m,g,
       iceb_u_kolbait(11,row[4]),row[4],
       iceb_u_kolbait(4,kop),kop,
       iceb_u_kolbait(5,row1[4]),row1[4],
       kolih,cena,cenak,bb);
       itog1+=bb;
      }
     
     if(vo == 1)
      {
        if(fabs(cenak-cena) > 0.0009)
         {
          sprintf(strsql,"%02d.%02d.%d %-*s %*s %10.10g %10.10g\n",
          d,m,g,
          iceb_u_kolbait(11,row[4]),row[4],
          iceb_u_kolbait(5,row1[4]),row1[4],
          cena,cenak);

          iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

          bb=kolih*cenak - kolih*cena;
          fprintf(ff1,"%02d.%02d.%d %-*s %-*s %*s %6.6g %10.10g %10.10g %10.10g\n",
          d,m,g,
          iceb_u_kolbait(11,row[4]),row[4],
          iceb_u_kolbait(4,kop),kop,
          iceb_u_kolbait(5,row1[4]),row1[4],
          kolih,cena,cenak,bb);
         }
      }

     sprintf(strsql,"select kodgr from Material where kodm=%s",
     row1[4]);
     if(sql_readkey(&bd,strsql,&row2,&cur2) != 1)
      {
       sprintf(strsql,"%s %s !\n",gettext("Не найден код материалла"),row1[4]);
       iceb_menu_soob(strsql,data->window);
       continue;
      }

     if(iceb_u_proverka(data->rk->kodgrmat.ravno(),row2[0],0,0) != 0)
       continue;  
     kgrm=atoi(row2[0]);


     if(iceb_u_proverka(data->rk->shet.ravno(),shu,0,0) != 0)
       continue;  
  

     kol=readkolkw(skl,nk,d,m,g,row[4],data->window);

     suma=kol*cena;
     suma=iceb_u_okrug(suma,okrg1);
     sumao+=suma;
    
     if(tp == 1)       
     if(vtr == 1 || kgrm == vtara)
      {
       mkm1p.plus(suma,6*nomop+5);
      }

     if(tp == 2)       
      {
       sumak=kol*cenak;
       sumak=iceb_u_okrug(sumak,okrg1);
       sumaok+=sumak;
       /*Возвратная тара*/
       if(vtr == 1 || kgrm == vtara)
        {
         mkm1r.plus(suma,6*nomop+5);
         mkm1_1r.plus(suma,6*nomop+5);
        }
      }
    } 



/* Читаем как была оформлена накладная*/
   mnds=0;
   sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=11",
   g,row[4],skl);

   if(sql_readkey(&bd,strsql,&row2,&cur2) == 1)
    mnds=atoi(row2[0]);

/* Читаем сумму корректировки*/

   sumkor=0.;
   sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=13",
   g,row[4],skl);

   if(sql_readkey(&bd,strsql,&row2,&cur2) == 1)
     sumkor=atof(row2[0]);
           
   if(tp == 1)
    {
     mkm1p.plus(sumao,6*nomop+mnds);
   
     if(mnds == 0)
      { 
       //Читаем вручную введенную сумму НДС
       sprintf(strsql,"select sodz from Dokummat2 \
 where god=%d and nomd='%s' and sklad=%d and nomerz=6",
       g,row[4],skl);
       if(sql_readkey(&bd,strsql,&row2,&cur2) == 1)
         suma=atof(row2[0]);
       else
         suma=sumao*pnds/100.;   

       suma=iceb_u_okrug(suma,okrg1);
       mkm1p.plus(suma,6*nomop+4);
      }
    }
 
   if(tp == 2)
    {
      
     mkm1r.plus(sumao,6*nomop+mnds);
     mkm1_1r.plus(sumaok,6*nomop+mnds);
     mkm1_1k.plus(sumkor,6*nomop+mnds);
  
     if(mnds == 0)
      { 
       suma=sumao*pnds/100.;   
       suma=iceb_u_okrug(suma,okrg1);
       mkm1r.plus(suma,6*nomop+4);

       sumak=sumaok*pnds/100.;
       sumak=iceb_u_okrug(sumak,okrg1);

       mkm1_1r.plus(sumak,6*nomop+4);
       mkm1_1k.plus(sumkor,6*nomop+4);
      }
    }

 }

fprintf(ff1,"%*s: %10.10g\n",iceb_u_kolbait(61,gettext("Итого")),gettext("Итого"),itog);
fprintf(ff2,"%*s: %10.10g\n",iceb_u_kolbait(61,gettext("Итого")),gettext("Итого"),itog1);

sprintf(strsql,"%s\n",gettext("Распечатываем"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
/*Распечатываем результат*/
rasmasop(&spoprp,&mkm1p,NULL,NULL,1,ff,data);
rasmasop(&spoprr,&mkm1r,&mkm1_1r,&mkm1_1k,2,ff,data);

iceb_podpis(ff,data->window);
iceb_podpis(ff1,data->window);
iceb_podpis(ff2,data->window);

fprintf(ff,"\x12");

fclose(ff);
fclose(ff1);
fclose(ff2);






gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
/********************/
/*Распечатка массивов*/
/*********************/
void		rasmasop(iceb_u_spisok *spopr,iceb_u_double *mkm,
iceb_u_double *mkm1_1r,
iceb_u_double *mkm1_1k,
short tip,FILE *ff,
class rasndsw_r_data *data)
{
double		suma;
double          its,it1,it2,it3,it4,it5,it6;
double          itsu,it1u,it2u,it3u,it4u,it5u,it6u;
double          itsk,it1k,it2k,it3k,it4k,it5k,it6k;
short		i;
char		naimo[512];
SQL_str         row;
SQLCURSOR       cur;
char		strsql[512];
char		bros[512];
short kolopr=spopr->kolih();

//iceb_poldan("Н.Д.С.",bros,"matnast.alx",data->window);
//float pnds=iceb_u_atof(bros);
float pnds=iceb_pnds(data->window);

/*Проверяем есть ди данные в массиве расходов*/
suma=0.;
//for(i=0; i < 6*kolopr ; i++)
//  suma+=mkm[i];
suma=mkm->suma();

it1=it2=it3=it4=it5=it6=0.;
it1u=it2u=it3u=it4u=it5u=it6u=0.;
it1k=it2k=it3k=it4k=it5k=it6k=0.;
if(suma > 0.009)
 {  
  if(tip == 1)
   {
    sprintf(strsql,"%s\n",gettext("Приходы"));
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    fprintf(ff,"%s\n",gettext("Приходы"));
   }
  if(tip == 2)
   {
    sprintf(strsql,"%s\n",gettext("Расходы"));
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
   fprintf(ff,"%s\n",gettext("Расходы"));
   }
  fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("Опер.|   Наименование операции       |      20%% НДС       |0%% Украина|0%% Експорт|0%% Cт.5   |Оборотная | Итого    |\n"));
fprintf(ff,gettext("     |                               | С у м м а|   НДС   |          |          |          |  тара    |          |\n"));

  fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");
//  for(i=0; i < 6*kolopr && POLE(spopr,bros,i+1,'#') == 0;i++)

  for(i=0; i < kolopr ;i++)
   {
    memset(bros,'\0',sizeof(bros));
    strncpy(bros,spopr->ravno(i),sizeof(bros)-1);
    if(bros[0] == '\0')
     break;
    its=mkm->ravno(i*6)+mkm->ravno(i*6+1)+mkm->ravno(i*6+2)+mkm->ravno(i*6+3)+mkm->ravno(i*6+4)+mkm->ravno(i*6+5);
    if(its < 0.01)
     continue;      

    if(tip == 1)
      sprintf(strsql,"select naik from Prihod where kod='%s'",bros);
    if(tip == 2)
      sprintf(strsql,"select naik from Rashod where kod='%s'",bros);
    memset(naimo,'\0',sizeof(naimo));
    if(sql_readkey(&bd,strsql,&row,&cur) != 1)
     {
      iceb_u_str repl;
      sprintf(bros,"%s:%s\n",gettext("Не нашли код операции"),bros);
      repl.plus(bros);
      repl.plus(strsql);
      fprintf(ff,"%s\n",repl.ravno());
      iceb_menu_soob(&repl,data->window);
      continue;
     }
    strncpy(naimo,row[0],sizeof(naimo)-1);
    it1+=mkm->ravno(i*6);
    it2+=mkm->ravno(i*6+1);
    it3+=mkm->ravno(i*6+2);
    it4+=mkm->ravno(i*6+3);
    it5+=mkm->ravno(i*6+4);
    it6+=mkm->ravno(i*6+5);


    fprintf(ff,"%-*s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
    iceb_u_kolbait(5,bros),bros,
    iceb_u_kolbait(30,naimo),iceb_u_kolbait(30,naimo),naimo,
    mkm->ravno(i*6),mkm->ravno(i*6+4),mkm->ravno(i*6+1),mkm->ravno(i*6+2),
    mkm->ravno(i*6+3),mkm->ravno(i*6+5),its);

    if(tip == 2)
     {
      itsu=mkm1_1r->ravno(i*6)+mkm1_1r->ravno(i*6+1)+mkm1_1r->ravno(i*6+2)+mkm1_1r->ravno(i*6+3)+mkm1_1r->ravno(i*6+4)+mkm1_1r->ravno(i*6+5);
      it1u+=mkm1_1r->ravno(i*6);
      it2u+=mkm1_1r->ravno(i*6+1);
      it3u+=mkm1_1r->ravno(i*6+2);
      it4u+=mkm1_1r->ravno(i*6+3);
      it5u+=mkm1_1r->ravno(i*6+4);
      it6u+=mkm1_1r->ravno(i*6+5);

      fprintf(ff,"%-5s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
      " ",
      iceb_u_kolbait(30,gettext("В учетных ценах")),
      iceb_u_kolbait(30,gettext("В учетных ценах")),
      gettext("В учетных ценах"),
      mkm1_1r->ravno(i*6),mkm1_1r->ravno(i*6+4),mkm1_1r->ravno(i*6+1),mkm1_1r->ravno(i*6+2),
      mkm1_1r->ravno(i*6+3),mkm1_1r->ravno(i*6+5),itsu);

      itsk=mkm1_1k->ravno(i*6)+mkm1_1k->ravno(i*6+1)+mkm1_1k->ravno(i*6+2)+mkm1_1k->ravno(i*6+3)+\
      mkm1_1k->ravno(i*6+4)*pnds/100.+mkm1_1k->ravno(i*6+5);
      it1k+=mkm1_1k->ravno(i*6);
      it2k+=mkm1_1k->ravno(i*6+1);
      it3k+=mkm1_1k->ravno(i*6+2);
      it4k+=mkm1_1k->ravno(i*6+3);
      it5k+=mkm1_1k->ravno(i*6+4)*pnds/100.;
      it6k+=mkm1_1k->ravno(i*6+5);
      
      fprintf(ff,"%-5s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
      " ",
      iceb_u_kolbait(30,gettext("Суммы коректировок")),
      iceb_u_kolbait(30,gettext("Суммы коректировок")),
      gettext("Суммы коректировок"),
      mkm1_1k->ravno(i*6),mkm1_1k->ravno(i*6+4)*pnds/100.,mkm1_1k->ravno(i*6+1),mkm1_1k->ravno(i*6+2),
      mkm1_1k->ravno(i*6+3),mkm1_1k->ravno(i*6+5),itsk);
     }
   }
  its=it1+it5+it2+it3+it4+it6;
  itsk=it1k+it5k+it2k+it3k+it4k+it6k;
  itsu=it1u+it5u+it2u+it3u+it4u+it6u;

  fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n\
%-5s %*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  " ",
  iceb_u_kolbait(30,gettext("Итого")),
  iceb_u_kolbait(30,gettext("Итого")),
  gettext("Итого"),
  it1,it5,it2,it3,it4,it6,its);
  
  if(tip == 2)
   {
    fprintf(ff,"\
%-5s %*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  " ",
  iceb_u_kolbait(30,gettext("Итого корректировок")),
  iceb_u_kolbait(30,gettext("Итого корректировок")),
  gettext("Итого корректировок"),
  it1k,it5k,it2k,it3k,it4k,it6k,itsk);

    fprintf(ff,"\
%-5s %*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  " ",
  iceb_u_kolbait(30,gettext("Итого в учетных ценах")),
  iceb_u_kolbait(30,gettext("Итого в учетных ценах")),
  gettext("Итого в учетных ценах"),
  it1u,it5u,it2u,it3u,it4u,it6u,itsu);

    fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n\
%-5s %*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
    " ",
    iceb_u_kolbait(30,gettext("Разница")),
    iceb_u_kolbait(30,gettext("Разница")),
    gettext("Разница"),
    it1+it1k-it1u,
    it5+it5k-it5u,
    it2+it2k-it2u,
    it3+it3k-it3u,
    it4+it4k-it4u,
    it6+it6k-it6u,
    its+itsk-itsu);

   }
 }
}
