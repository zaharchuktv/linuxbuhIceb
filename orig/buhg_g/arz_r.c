/*$Id: arz_r.c,v 1.19 2011-02-21 07:35:51 sasa Exp $*/
/*21.04.2010	08.11.2006	Белых А.И.	arz_r.c
Расчёт движения платёжной ведомости
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "arz.h"

class arz_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class arz_rek *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  arz_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   arz_r_key_press(GtkWidget *widget,GdkEventKey *event,class arz_r_data *data);
gint arz_r1(class arz_r_data *data);
void  arz_r_v_knopka(GtkWidget *widget,class arz_r_data *data);

void	hpzrs(short mr,short gr,FILE *ff1);
void	strsen(int nomer,char *inn,char *bankshet,char *fio,double bbr,FILE *ff1);
void    zsitog(int nomer,double itogoo,FILE *ff1,GtkWidget*);
double zarpv(short mr,short gr,int tabnom,double *sumab,GtkWidget *wpredok);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern int      kol_strok_na_liste;
extern iceb_u_str shrift_ravnohir;
extern short	*obud; /*Обязательные удержания*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern short mmm,ggg;
extern char	pktn[16]; /*Приставка к табельному номеру*/
extern char     shrpz[16]; /*Счет расчетов по зарплате*/
extern double   okrg; /*Округление*/
extern char	*shetb; /*Бюджетные счета начислений*/

int arz_r(class arz_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class arz_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Автоматическая разноска зароботной платы"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(arz_r_key_press),&data);

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

repl.plus(gettext("Автоматическая разноска зароботной платы"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(arz_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)arz_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  arz_r_v_knopka(GtkWidget *widget,class arz_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   arz_r_key_press(GtkWidget *widget,GdkEventKey *event,class arz_r_data *data)
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

gint arz_r1(class arz_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
iceb_u_str repl;
class iceb_clock sss(data->window);
char bros[312];
char		shet_per[24],shet_per_b[24];
char            shet_v_kas[24],shet_v_kas_b[24];
int		metkaras=0; //0-все 1-только хозрасчет 2-только бюджет

memset(shet_v_kas,'\0',sizeof(shet_v_kas));
memset(shet_v_kas_b,'\0',sizeof(shet_v_kas_b));
memset(shet_per_b,'\0',sizeof(shet_per_b));
memset(shet_per,'\0',sizeof(shet_per));


short mr,gr;

iceb_u_rsdat1(&mr,&gr,data->rk->data.ravno());

double proc_vipl=data->rk->proc_vip.ravno_atof();

class SQLCURSOR curr;
SQL_str row;
short dp=0,mp=0,gp=0;
int kodper=0;
int kod_v_kas=0;
class iceb_u_str imaf_nast("zarnast.alx");
if(data->rk->data_per.getdlinna() > 1)
 {
  if(iceb_u_rsdat(&dp,&mp,&gp,data->rk->data_per.ravno(),1) != 0)
   { 
    iceb_menu_soob(gettext("Неправильно введена дата выплаты !"),data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   } 
  /*Читаем код перечисления*/
  memset(bros,'\0',sizeof(bros));
  if(iceb_poldan("Код перечисления зарплаты на счет",bros,imaf_nast.ravno(),data->window) != 0)
   {
    iceb_menu_soob(gettext("Не найден код перечисления зарплаты на счёт !"),data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }
  kodper=atoi(bros);

  if(iceb_poldan("Код получения зарплаты в кассе",bros,imaf_nast.ravno(),data->window) != 0)
   {
    iceb_menu_soob(gettext("Не найден код получения зарплаты в кассе !"),data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }
  kod_v_kas=atoi(bros);

  if(kod_v_kas == 0 && kodper == 0)
   {
    iceb_menu_soob(gettext("Код получения зарплаты в кассе и код перечисления на счёт равны нолю !"),data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

   
/*
  printw("kodper=%d\n",kodper);
  OSTANOV();
*/
  
  //Читаем счет перечисления на карт-счёт
  sprintf(strsql,"select shet from Uder where kod=%d",kodper);
  if(iceb_sql_readkey(strsql,&row,&curr,data->window) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код удержания"),kodper);
    iceb_menu_soob(strsql,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }
  strncpy(shet_per,row[0],sizeof(shet_per)-1);

  //*Читаем счет 
  sprintf(strsql,"select shet from Uder where kod=%d",kod_v_kas);
  if(iceb_sql_readkey(strsql,&row,&curr,data->window) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код удержания"),kod_v_kas);
    iceb_menu_soob(strsql,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }
  strncpy(shet_v_kas,row[0],sizeof(shet_v_kas)-1);

  if(shetbu != NULL)
   {
    memset(bros,'\0',sizeof(bros));
    if(iceb_poldan("Счета перечисления на карт-счёт",bros,imaf_nast.ravno(),data->window) == 0)
     {
      iceb_u_polen(bros,shet_per,sizeof(shet_per),1,',');
      iceb_u_polen(bros,shet_per_b,sizeof(shet_per),2,',');
     }     

    //Проверяем есть ли счета в плане счетов
    sprintf(strsql,"select ns from Plansh where ns='%s'",shet_per);
    int i;
    if((i=sql_readkey(&bd,strsql,&row,&curr)) != 1)
     {
      class iceb_u_str repl;
      sprintf(strsql,gettext("Нет счёта %s в плане счетов !"),shet_per);
      repl.plus(strsql);
      repl.ps_plus(gettext("Счет хозрасчетной кассы, введен в файле настройки"));

      iceb_menu_soob(&repl,data->window);
      sss.clear_data();
      gtk_widget_destroy(data->window);
      return(FALSE);
     }

    sprintf(strsql,"select ns from Plansh where ns='%s'",shet_per_b);
    if(sql_readkey(&bd,strsql,&row,&curr) != 1)
     {
      class iceb_u_str repl;
      sprintf(strsql,gettext("Не найден счет %s в плане счетов !"),shet_per_b);
      repl.plus(strsql);
      repl.ps_plus(gettext("Счет бюджетной кассы, введен в файле настройки"));

      iceb_menu_soob(&repl,data->window);
      sss.clear_data();
      gtk_widget_destroy(data->window);
      return(FALSE);
     }
    memset(bros,'\0',sizeof(bros));

    if(iceb_poldan("Счета выдачи из кассы",bros,imaf_nast.ravno(),data->window) == 0)
     {
      iceb_u_polen(bros,shet_v_kas,sizeof(shet_per),1,',');
      iceb_u_polen(bros,shet_v_kas_b,sizeof(shet_per),2,',');
     }     

    //Проверяем есть ли счета в плане счетов
    sprintf(strsql,"select ns from Plansh where ns='%s'",shet_v_kas);
    if((i=sql_readkey(&bd,strsql,&row,&curr)) != 1)
     {
      class iceb_u_str repl;
      sprintf(strsql,gettext("Нет счёта %s в плане счетов !"),shet_v_kas);
      repl.plus(strsql);
      repl.ps_plus(gettext("Счет хозрасчетной перечисления, введен в файле настройки"));

      iceb_menu_soob(&repl,data->window);
      sss.clear_data();
      gtk_widget_destroy(data->window);
      return(FALSE);
     }

    sprintf(strsql,"select ns from Plansh where ns='%s'",shet_v_kas_b);
    if(sql_readkey(&bd,strsql,&row,&curr) != 1)
     {
      class iceb_u_str repl;
      sprintf(strsql,gettext("Нет счёта %s в плане счетов !"),shet_v_kas_b);
      repl.plus(strsql);
      repl.ps_plus(gettext("Счет бюджетного перечисления, введен в файле настройки"));

      iceb_menu_soob(&repl,data->window);
      sss.clear_data();
      gtk_widget_destroy(data->window);
      return(FALSE);
     }

    iceb_u_str titl;
    iceb_u_str zagolovok;
    iceb_u_spisok punkt_m;

    titl.plus(gettext("Выбор"));


    zagolovok.plus(gettext("Выберите нужное"));


    punkt_m.plus(gettext("Хозрасчёт и бюджет"));//0
    punkt_m.plus(gettext("Только хозрасчёт"));//1
    punkt_m.plus(gettext("Только бюджет"));//2


    metkaras=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,data->window);

    if(metkaras == 3 || metkaras == -1)
     {
      iceb_menu_soob(&repl,data->window);
      sss.clear_data();
      gtk_widget_destroy(data->window);
      return(FALSE);
     }
     
   
   }
  
  if(kodper <= 0)
   {
    strcpy(shet_per,shet_v_kas);
    strcpy(shet_per_b,shet_v_kas_b);
    kodper=kod_v_kas;
   }

  if(kod_v_kas <= 0)
   {
    strcpy(shet_v_kas,shet_per);
    strcpy(shet_v_kas_b,shet_per_b);
    kod_v_kas=kodper;
   }
/*
  printw("shet=%s shet_per_b=%s\n",shet,shet_per_b);
  OSTANOV();
  return;
*/
 }

int uder=2; /*Удержания не выполнять*/
if(dp != 0)
 {

  uder=iceb_menu_danet(gettext("Сделать разноску ? Вы уверены ?"),2,data->window);
  if(iceb_pbpds(mp,gp,data->window) != 0)
   {
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

 }
 

sprintf(strsql,"%s:%.2f\n",gettext("Максимальная сумма виплати"),data->rk->max_sum.ravno_atof());  
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%s\n",gettext("Сортируем записи"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"select tabn,podr,kateg from Zarn where god=%d and mes=%d \
order by podr,tabn asc",gr,mr);

class SQLCURSOR cur;
int kolstr2=0;
if((kolstr2=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr2 == 0)
 {
  iceb_menu_soob(gettext("Не найжено ни одной записи !"),data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }

char imafprot[50];
sprintf(imafprot,"send%d.lst",getpid());

FILE *ff1;
if((ff1 = fopen(imafprot,"w")) == NULL)
 {
  iceb_er_op_fil(imafprot,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

//Вывод шапки
hpzrs(mr,gr,ff1);

float kolstr1=0.;
int podr=0;
double sal=0.,itogo=0.,itogoo=0.;
int nomer=0;
kolstr1=0;
double viplata=0.;
double vid_zar=0.; //Уже выданная зарплата при расчёте с учётом сальдо
double vid_zar_b=0.; //Уже выданная бюджетная зарплата при расчёте с учётом сальдо
int kod_zap=0;
char shet_zap[20];
char shet_zap_b[20];
class SQLCURSOR cur1;
SQL_str row1;
int tabb;
char inn[30];
char fio[512];
char bankshet[32];
double saldb=0.;
double itsal=0.;
int kolstr=0;
double  in1=0,inah=0,nahshet=0,iu=0,ins=0,ius=0,ius1=0,iub=0,iusb=0.;
int prn=0;
int knah=0;
short mz,gz;
double sym=0;
double bbr=0.;
double iusb1=0.;
class ZARP zp;
double bb=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr2,++kolstr1);    
//  strzag(LINES-1,0,kolstr2,++kolstr1);

  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->podr.ravno(),row[1],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->kod_kat.ravno(),row[2],0,0) != 0)
    continue;

  if(data->rk->kod_grup_pod.ravno()[0] != '\0')
   {
    sprintf(strsql,"select grup from Podr where kod=%s",row[1]);
    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
     if(iceb_u_proverka(data->rk->kod_grup_pod.ravno(),row1[0],0,0) != 0)
       continue;
   }

  tabb=atol(row[0]);
  podr=atoi(row[1]);


  memset(inn,'\0',sizeof(inn));

  sprintf(strsql,"select fio,bankshet,inn from Kartb where tabn=%d",tabb);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tabb);
    iceb_menu_soob(strsql,data->window);
    continue;
   }
  strncpy(fio,row1[0],sizeof(fio)-1);
  strncpy(inn,row1[2],sizeof(inn)-1);
  strncpy(bankshet,row1[1],sizeof(bankshet)-1);
  if(bankshet[0] == '\0')
   {
    strcpy(shet_zap,shet_v_kas);
    strcpy(shet_zap_b,shet_v_kas_b); //это пока не сделано
    kod_zap=kod_v_kas;
   }
  else
   {
    strcpy(shet_zap,shet_per);
    strcpy(shet_zap_b,shet_per_b);
    kod_zap=kodper;
   }   

  sal=0.;
  /*Читаем сальдо*/

  sal=zarsaldw(1,mr,gr,tabb,&saldb,data->window);
  itsal+=sal;
  if(sal < 0. && data->rk->metka_saldo == 1)
   {
    class iceb_u_str repl;
    repl.plus(gettext("Внимание ! Отрицательное сальдо !"));
    sprintf(strsql,"%s:%d %s",gettext("Табельный номер"),tabb,fio);
    repl.ps_plus(strsql);
    iceb_menu_soob(strsql,data->window);
   }


  sprintf(strsql,"select datz,prn,knah,suma,godn,mesn,shet \
from Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and \
tabn=%d and suma != 0. order by prn,knah asc",  gr,mr,gr,mr,tabb);

  if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  if(kolstr == 0)
    continue;

  in1=inah=nahshet=iu=ins=ius=ius1=iub=iusb=0.;

  vid_zar=0.; //Уже выданная зарплата при расчёте с учётом сальдо
  vid_zar_b=0.; //Уже выданная бюджетная зарплата при расчёте с учётом сальдо
  int im=0;
  while(cur1.read_cursor(&row1) != 0)
   {

/*
    printw("%s %s %s %s %s %s %s %s\n",
    row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7]);
*/
    
    prn=atoi(row1[1]);

    if(prn == 1)
     {
      if(iceb_u_proverka(data->rk->kod_nah.ravno(),row1[2],0,1) == 0)
        continue;
     }

    knah=atoi(row1[2]);
    sym=atof(row1[3]);
    gz=atoi(row1[4]);
    mz=atoi(row1[5]);

    if(prn == 1)
     {
      in1+=sym;
      inah+=sym;
      ins+=sym;
      if(iceb_u_proverka(shetb,row1[6],0,1) == 0)
         nahshet+=sym;
     }

    if(prn == 2)
     {
      iu+=sym;
      if(iceb_u_proverka(shetbu,row1[6],0,1) == 0)
         iub+=sym;
      
      if(gz == gr && mz == mr)
       {
	ius+=sym;
        if(iceb_u_proverka(shetbu,row1[6],0,1) == 0)
          iusb+=sym;
       }
      else
       {
        if(provkodw(obud,knah) >= 0)
         {
          ius+=sym;
          if(iceb_u_proverka(shetbu,row1[6],0,1) == 0)
            iusb+=sym;
         }
       }


     }


    im++;

   }

  if(im > 0 || sal != 0.)
   {



    if(data->rk->metka_saldo == 0)
     {
      //При расчёте ведомости этой проверки на выданную зарплату нет так как
      //нам нужно получить ведомость без учёта уже выданной зарплаты
//      vid_zar=prbperw(mr,gr,bf->tm_mon+1,bf->tm_year+1900,tabb,&vid_zar_b,1,data->window);
      vid_zar=zarpv(mr,gr,tabb,&iusb1,data->window);
      bbr=in1+iu+sal+vid_zar;
//      printw("%f+%f+%f+%f=%f\n",in1,iu,sal,vid_zar,bbr);

     }
    if(data->rk->metka_saldo == 1)
     {
      ius1=prbperw(mr,gr,bf->tm_mon+1,bf->tm_year+1900,tabb,&iusb1,0,data->window);
      bbr=in1+ius+ius1;
     }

    if(bbr > 0.009)
     {


      viplata=bbr;
      if(proc_vipl != 100.)
       {
        viplata=bbr*proc_vipl/100.;
        viplata=iceb_u_okrug(viplata,okrg);
        }

      if(data->rk->max_sum.ravno_atof() > 0.009) 
       if(viplata > data->rk->max_sum.ravno_atof())
        viplata=data->rk->max_sum.ravno_atof();

      sprintf(strsql,"%4d %-5d %8.2f %-20s %s\n",
      podr,tabb,viplata,bankshet,fio);

      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
      itogo+=viplata;
            
      strsen(++nomer,inn,bankshet,fio,viplata,ff1);

      if(dp != 0 && uder == 1)
       {
        /*Проверяем записан ли стандартный набор*/
        provzzagw(mp,gp,tabb,2,podr,data->window);
/*****
        zp.dz=dp;
        zp.mesn=mr; zp.godn=gr;
        zp.nomz=0;
        zp.podr=podr;
********/
        zp.tabnom=tabb;
        zp.prn=2;
        zp.knu=kod_zap;
        zp.dz=dp;
        zp.mz=mp;
        zp.gz=gp;
        zp.mesn=mr; zp.godn=gr;
        zp.nomz=0;
        zp.podr=podr;

       }

      bb=0.;     

      if(shetbu != NULL && data->rk->max_sum.ravno_atof() < 0.01 && shet_zap_b[0] != '\0')
       {
       
        if(data->rk->metka_saldo == 0 ) /*С учетом сальдо*/
         {
          bb=bbr-(nahshet+iub+saldb+vid_zar_b);

         }               
       
        if(data->rk->metka_saldo == 1 ) /*Без учета сальдо*/
         {
          bb=in1-nahshet+(ius-iusb)+(ius1-iusb1);
         }

        bb=bbr-bb;
        if(bb != 0.)
         {
          fprintf(ff1,"%4s %-5s %8.2f/%.2f %s/%s\n",
          " "," ",bb,bbr-bb,shet_zap_b,shet_zap);
         }       

        if(metkaras == 0 || metkaras == 2)
        if(dp != 0 && uder == 1)
        if(bb > 0.)
         {
          strcpy(zp.shet,shet_zap_b);
          viplata=bb;
          if(proc_vipl != 100.)
           {
            viplata=bb*proc_vipl/100.;
            viplata=iceb_u_okrug(viplata,okrg);
            }

          if(data->rk->max_sum.ravno_atof() > 0.009) 
           if(viplata > data->rk->max_sum.ravno_atof())
            viplata=data->rk->max_sum.ravno_atof();
         
          strncpy(zp.shet,shet_zap_b,sizeof(zp.shet)-1);
          
  
          zapzarpw(&zp,viplata*-1,dp,mr,gr,0,shet_zap_b,"***",0,podr,"",data->window);
  //        zapzarp(dp,mp,gp,tabb,2,kod_zap,viplata*-1,shet_zap_b,mr,gr,0,0,"***",podr,"",zp); 
         }
       }
      if(metkaras == 0 || metkaras == 1)
      if(dp != 0 && uder == 1)
       {
        bb=bbr-bb;
     
        /*Записываем код перечисления*/
        if(bb > 0.)
         {
//          printw("%d.%d.%d %d %d.%d %f\n",dp,mp,gp,tabb,mr,gr,bb);
          strcpy(zp.shet,shet_zap);
          viplata=bb;
          if(proc_vipl != 100.)
           {
            viplata=bb*proc_vipl/100.;
            viplata=iceb_u_okrug(viplata,okrg);
           }

          if(data->rk->max_sum.ravno_atof() > 0.009) 
           if(viplata > data->rk->max_sum.ravno_atof())
            viplata=data->rk->max_sum.ravno_atof();

          strncpy(zp.shet,shet_zap,sizeof(zp.shet)-1);

          zapzarpw(&zp,viplata*-1,dp,mr,gr,0,shet_zap,"***",0,podr,"",data->window);
          
//          if(zapzarp(dp,mp,gp,tabb,2,kod_zap,viplata*-1,shet_zap,mr,gr,0,0,"***",podr,"",zp) != 0)
//            printw("Ошибка ввода записи !!!\n");

         }
       }
      //Делаем проводки
      zaravprw(tabb,mp,gp,NULL,data->window);

     }

   }


 }


itogoo+=itogo;

//Вывод концовки файла
zsitog(nomer,itogoo,ff1,data->window);


sprintf(strsql,"%s: %.2f\n",
gettext("Общий итог"),itogoo);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

data->rk->imaf.plus(imafprot);
data->rk->naimf.plus(gettext("Список разнесеных сумм"));
for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
/**********************************/
/*Шапка                           */
/**********************************/

void	hpzrs(short mr,short gr,FILE *ff1)
{


iceb_u_zagolov(gettext("Список рaзнесённых сумм"),0,0,0,0,mr,gr,organ,ff1);

fprintf(ff1,"\
-------------------------------------------------------------------------------\n");

fprintf(ff1,gettext("\
Ном.|Идентефик.|     Номер          |   Фамилия Имя Отчество       |  Сумма   |\n\
п/п |  номер   | карточного счёта   |                              |          |\n"));
//              12345678901234567890 123456789012345678901234567890 

fprintf(ff1,"\
-------------------------------------------------------------------------------\n");


}

/*************/
/*Вывод строки*/
/**************/

void	strsen(int nomer,char *inn,char *bankshet,char *fio,double bbr,FILE *ff1)
{

fprintf(ff1,"%4d %-10s %-20s %-*.*s %10.2f\n",
nomer,inn,bankshet,
iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,
bbr);

}

/****************************/
/*Вывод концовки распечатки */
/****************************/

void zsitog(int nomer,double itogoo,FILE *ff1,GtkWidget *wpredok)
{

fprintf(ff1,"\
-------------------------------------------------------------------------------\n");
fprintf(ff1,"%*s:%10.2f\n",iceb_u_kolbait(67,gettext("Итого")),gettext("Итого"),itogoo);


iceb_podpis(ff1,wpredok);


fclose(ff1);

}
