/*$Id: zar_bazno_r.c,v 1.17 2011-04-14 16:09:36 sasa Exp $*/
/*14.04.2010	22.01.2007	Белых А.И.	zar_bazno_r.c
Расчёт баз налогообложения для социальных фондов
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"

class zar_bazno_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  const char *data_d;
  class iceb_u_spisok *imaf;
  class iceb_u_spisok *naimf;
    
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_bazno_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_bazno_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_bazno_r_data *data);
gint zar_bazno_r1(class zar_bazno_r_data *data);
void  zar_bazno_r_v_knopka(GtkWidget *widget,class zar_bazno_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

extern short	metkarnb; //0-расчет налогов для бюджета пропорциональный 1-последовательный
extern short    *knnf; //Коды начислений недежными формами 
extern short	*kodnvpen; /*Коды не входящие в расчет пенсионного отчисления*/
extern short	*kodbzrnv; //Коды не входящие в расчет отчислений на безработицу
extern short	*kodsocstrnv; //Коды не входящие в расчет соцстраха
//extern float    prog_min_tek; //Прожиточный минимум текущий
extern short    *kodmp;   /*Коды материальной помощи*/
extern char	*shetb; /*Бюджетные счета*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern short    *kodbl;  /*Код начисления больничного*/
extern short    *kuvvr_pens_sfz; //Коды удержаний входящие в расчет при отчислении с фонда зарплаты
extern short    kodpen;  /*Код пенсионных отчислений*/
extern short    kodsocstr;  /*Код отчисления на соц-страх*/
extern short    kodbzr;  /*Код отчисления на безработицу*/
extern double	ogrzp[2]; /*Разграниечение зарплаты для расчета пенсионных*/

int zar_bazno_r(const char *data_d,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_bazno_r_data data;

data.data_d=data_d;
data.imaf=imaf;
data.naimf=naimf;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать базы налогообложения для соц. фондов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_bazno_r_key_press),&data);

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

repl.plus(gettext("Распечатать базы налогообложения для соц. фондов"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_bazno_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_bazno_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_bazno_r_v_knopka(GtkWidget *widget,class zar_bazno_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_bazno_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_bazno_r_data *data)
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

gint zar_bazno_r1(class zar_bazno_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);


short mr,gr;
iceb_u_rsdat1(&mr,&gr,data->data_d);



sprintf(strsql,"%s:%d.%d%s\n",
gettext("Дата"),mr,gr,gettext("г."));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

//читаем настройки
zar_read_tnw(1,mr,gr,NULL,data->window);
 
SQLCURSOR cur;
SQLCURSOR cur1;
SQLCURSOR curr;
int	kolstr=0;
float kolstr1=0;
int kolstr2=0;
SQL_str		row,row1;

sprintf(strsql,"select tabn from Zarn where god=%d and mes=%d",
gr,mr);

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

double    sumanah[2]; //Общая сумма которая начислена/бюджет
memset(sumanah,'\0',sizeof(sumanah));
double    sumanahtab[2]; //Общая сумма которая начислена/бюджет
memset(sumanahtab,'\0',sizeof(sumanahtab));

double    pensnvr[2]; //Пенсионные не вошедшие в расчет
double    pensnvrtab[2]; //Пенсионные не вошедшие в расчет
memset(pensnvr,'\0',sizeof(pensnvr));
memset(pensnvrtab,'\0',sizeof(pensnvrtab));

double    socnvr[2]; //Соцстрах не вошедший в расчет
double    socnvrtab[2]; //Соцстрах не вошедший в расчет
memset(socnvr,'\0',sizeof(socnvr));
memset(socnvrtab,'\0',sizeof(socnvrtab));

double    bezrnvr[2]; //Безработица не вошедшая в расчет
double    bezrnvrtab[2]; //Безработица не вошедшая в расчет
memset(bezrnvr,'\0',sizeof(bezrnvr));
memset(bezrnvrtab,'\0',sizeof(bezrnvrtab));
/*************
double    nmatpom[2]; //Необлагаемая мат. помощь для пенсионного 
double    nmatpomtab[2]; //Необлагаемая мат. помощь
memset(nmatpom,'\0',sizeof(nmatpom));
memset(nmatpomtab,'\0',sizeof(nmatpomtab));
******************/

double    sumaprevm[2];  //Сумма превышающая максимум с которой берется налог
double    sumaprevmsoc[2];  //Сумма превышающая максимум с которой берется налог
double    sumaprevmbez[2];  //Сумма превышающая максимум с которой берется налог
memset(sumaprevm,'\0',sizeof(sumaprevm));
memset(sumaprevmsoc,'\0',sizeof(sumaprevmsoc));
memset(sumaprevmbez,'\0',sizeof(sumaprevmbez));

double	  sumvrpens1[2];
memset(sumvrpens1,'\0',sizeof(sumvrpens1));
double	  sumvrpens2[2];
memset(sumvrpens2,'\0',sizeof(sumvrpens2));

short	  mespp,godpp;

double	  sumapnal[2];
memset(sumapnal,'\0',sizeof(sumapnal));
double	  sumapnaltab[2];
memset(sumapnaltab,'\0',sizeof(sumapnaltab));

double	  sumapnal1[2];
memset(sumapnal1,'\0',sizeof(sumapnal1));
double	  sumapnal2[2];
memset(sumapnal2,'\0',sizeof(sumapnal2));

double	  sumvrsoc1[2];
memset(sumvrsoc1,'\0',sizeof(sumvrsoc1));
double	  sumvrsoc2[2];
memset(sumvrsoc2,'\0',sizeof(sumvrsoc2));

double	  sumvrbez1=0.;
double	  sumvrbez2=0.;

double	  sumasnal[2];
memset(sumasnal,'\0',sizeof(sumasnal));
double	  sumasnaltab[2];
memset(sumasnaltab,'\0',sizeof(sumasnaltab));

double	  sumabnal[2];
memset(sumabnal,'\0',sizeof(sumabnal));
double	  sumabnaltab[2];
memset(sumabnaltab,'\0',sizeof(sumabnaltab));

double	  sumasnal1[2];
memset(sumasnal1,'\0',sizeof(sumasnal1));
double	  sumasnal2[2];
memset(sumasnal2,'\0',sizeof(sumasnal2));

double	  sumanvbezr[2]; //Сумма не вошедшая в расчет безработицы (пенсионеры и инвалиды)
memset(sumanvbezr,'\0',sizeof(sumanvbezr));

double    sumaogr[2];
memset(sumaogr,'\0',sizeof(sumaogr));

//*****************************
double suma_ndfnz_pens[2]; //неденежные формы начисления зарплаты
memset(&suma_ndfnz_pens,'\0',sizeof(suma_ndfnz_pens));

double suma_ndfnz_pens_tab[2]; //неденежные формы начисления зарплаты
memset(&suma_ndfnz_pens_tab,'\0',sizeof(suma_ndfnz_pens_tab));

double suma_ufn_pens_tab[2]; //сумма на которую увеличился фонд облагаемый изза неденежных форм начисления зарплаты
memset(&suma_ufn_pens_tab,'\0',sizeof(suma_ufn_pens_tab));
double suma_ufn_pens[2]; //сумма на которую увеличился фонд облагаемый изза неденежных форм начисления зарплаты
memset(&suma_ufn_pens,'\0',sizeof(suma_ufn_pens));


//*****************************
double suma_ndfnz_socstr[2]; //неденежные формы начисления зарплаты
memset(&suma_ndfnz_socstr,'\0',sizeof(suma_ndfnz_socstr));

double suma_ndfnz_socstr_tab[2]; //неденежные формы начисления зарплаты
memset(&suma_ndfnz_socstr_tab,'\0',sizeof(suma_ndfnz_socstr_tab));

double suma_ufn_socstr_tab[2]; //сумма на которую увеличился фонд облагаемый изза неденежных форм начисления зарплаты
memset(&suma_ufn_socstr_tab,'\0',sizeof(suma_ufn_socstr_tab));
double suma_ufn_socstr[2]; //сумма на которую увеличился фонд облагаемый изза неденежных форм начисления зарплаты
memset(&suma_ufn_socstr,'\0',sizeof(suma_ufn_socstr));


//*****************************
double suma_ndfnz_bezr[2]; //неденежные формы начисления зарплаты
memset(&suma_ndfnz_bezr,'\0',sizeof(suma_ndfnz_bezr));

double suma_ndfnz_bezr_tab[2]; //неденежные формы начисления зарплаты
memset(&suma_ndfnz_bezr_tab,'\0',sizeof(suma_ndfnz_bezr_tab));

double suma_ufn_bezr_tab[2]; //сумма на которую увеличился фонд облагаемый изза неденежных форм начисления зарплаты
memset(&suma_ufn_bezr_tab,'\0',sizeof(suma_ufn_bezr_tab));
double suma_ufn_bezr[2]; //сумма на которую увеличился фонд облагаемый изза неденежных форм начисления зарплаты
memset(&suma_ufn_bezr,'\0',sizeof(suma_ufn_bezr));

double suma_boln_ntm=0.; //Сумма больничных не текущего месяца
double suma_boln_ntm_b=0.; //Сумма больничных не текущего месяца Бюджет

double    sbolno=0.;  //Общая сумма больничных
double    sbolnpp=0.; //Сумма больничных предыдущих периодов
double    sumpenpp=0.; //Сумма пенсионных в счет предыдущих периодов
class iceb_u_double KODNVPENSUM; /*Коды не входящие в расчет пенсионного отчисления*/
class iceb_u_double KODBZRNVSUM; //Коды не входящие в расчет отчислений на безработицу
class iceb_u_double KODSOCSTRNVSUM; //Коды не входящие в расчет соцстраха

if(kodnvpen != NULL)
  KODNVPENSUM.make_class(kodnvpen[0]);
if(kodbzrnv != NULL)
  KODBZRNVSUM.make_class(kodbzrnv[0]);
if(kodsocstrnv != NULL)
  KODSOCSTRNVSUM.make_class(kodsocstrnv[0]);

long      tabn=0;
short     prn=0;
short     knah=0;
double    suma=0.;
short     metkamatpom=0;
double    bb,bb1;
char	  naik[40];
int	  i;
char      shetmatpom[20];
short     metkabezr=0,metkabezrb=0;
char imaf[56];
sprintf(imaf,"pensoc%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_zagolov(gettext("Расчет баз налогообложения для соц. отчислений"),0,0,0,0,0,0,organ,ff);
fprintf(ff,"Дата %d.%d\n",mr,gr);

if(knnf != NULL)
 {
  char naim[512];
  fprintf(ff,"%s:\n",gettext("Коды неденежных форм начисления зарплаты"));
  for(int nom=0; nom < knnf[0]; nom++)
   {
    memset(naim,'\0',sizeof(naim));
    sprintf(strsql,"select naik from Nash where kod=%d",knnf[nom+1]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      strncpy(naim,row1[0],sizeof(naim)-1);
    fprintf(ff,"%2d %s\n",knnf[nom+1],naim);
   }
 }

char imaf_pr_soc1[32]; //Файд в который будут выгружены работники у которых сумма зарплаты в 1-вом диапазоне
char imaf_pr_soc2[32]; //Файд в который будут выгружены работники у которых сумма зарплаты в 2-вом диапазоне
sprintf(imaf_pr_soc1,"prsoc1%d.lst",getpid());
sprintf(imaf_pr_soc2,"prsoc2%d.lst",getpid());
FILE *ff_pr_soc1;
FILE *ff_pr_soc2;

if((ff_pr_soc1 = fopen(imaf_pr_soc1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_pr_soc1,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if((ff_pr_soc2 = fopen(imaf_pr_soc2,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_pr_soc2,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char fio[512];


class zar_read_tn1h nastr;
zar_read_tn1w(1,mr,gr,&nastr,NULL,data->window);

fprintf(ff_pr_soc1,"%s\nРасчёт для соц-страха.\nСписок работников у которых начислено до %.2f\n",
organ,nastr.prog_min_tek);
fprintf(ff_pr_soc2,"%s\nРасчёт для соц-страха.\nСписок работников у которых начислено больше %.2f\n",
organ,nastr.prog_min_tek);
double itogo_suma_soc1[2];
double itogo_suma_soc2[2];
memset(&itogo_suma_soc1,'\0',sizeof(itogo_suma_soc1));
memset(&itogo_suma_soc2,'\0',sizeof(itogo_suma_soc2));
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  tabn=atol(row[0]);
  memset(fio,'\0',sizeof(fio));
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   strncpy(fio,row1[0],sizeof(fio)-1);

//  printw("%s\n",row[0]);
    
  sprintf(strsql,"select prn,knah,suma,shet,godn,mesn from Zarp \
where tabn=%ld and datz >= '%04d-%d-01' and datz <= '%04d-%d-31' \
and suma <> 0.",tabn,gr,mr,gr,mr);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  if(kolstr2 == 0)
   continue;
  
  suma_boln_ntm=0.;
  suma_boln_ntm_b=0.;
  metkamatpom=0;
  metkabezr=metkabezrb=0;
  memset(sumapnaltab,'\0',sizeof(sumapnaltab));
  memset(sumasnaltab,'\0',sizeof(sumasnaltab));
  memset(sumabnaltab,'\0',sizeof(sumabnaltab));

  memset(sumanahtab,'\0',sizeof(sumanahtab));
  memset(pensnvrtab,'\0',sizeof(pensnvrtab));
  memset(socnvrtab,'\0',sizeof(socnvrtab));
  memset(bezrnvrtab,'\0',sizeof(bezrnvrtab));
//  memset(nmatpomtab,'\0',sizeof(nmatpomtab));

  memset(&suma_ndfnz_pens_tab,'\0',sizeof(suma_ndfnz_pens_tab));
  memset(&suma_ndfnz_socstr_tab,'\0',sizeof(suma_ndfnz_socstr_tab));
  memset(&suma_ndfnz_bezr_tab,'\0',sizeof(suma_ndfnz_bezr_tab));

  while(cur1.read_cursor(&row1) != 0)
   {
    prn=atoi(row1[0]);
    knah=atoi(row1[1]);
    suma=atof(row1[2]);
    godpp=atoi(row1[4]);
    mespp=atoi(row1[5]);
    if(prn == 1) //Начисления
     {
      if(provkodw(kodmp,knah) >= 0)
       {
        metkamatpom=1;
        strcpy(shetmatpom,row1[3]);
       }
      sumanahtab[0]+=suma;
      if(iceb_u_proverka(shetb,row1[3],0,1) == 0)
        sumanahtab[1]+=suma;
        
      if(provkodw(kodnvpen,knah) >= 0)
       {
        pensnvrtab[0]+=suma;      
        if(iceb_u_proverka(shetb,row1[3],0,1) == 0)
          pensnvrtab[1]+=suma;


        //определяем номер в массиве
        for(i=1; i <= kodnvpen[0]; i ++)
          if(knah == kodnvpen[i])
           {
            KODNVPENSUM.plus(suma,i-1);
            break;
           }
       }
      else
       {
        suma_ndfnz_pens_tab[0]+=knnfndsw(1,knah,row1[3],suma,&suma_ndfnz_pens_tab[1],NULL);

       }
      if(provkodw(kodbl,knah) >= 0 && (mespp != mr || godpp != gr )) 
       {
        suma_boln_ntm+=suma;
        if(iceb_u_proverka(shetb,row1[3],0,1) == 0)
          suma_boln_ntm_b+=suma;
       }       
      if(provkodw(kodsocstrnv,knah) >= 0)
       {
        socnvrtab[0]+=suma;      
        if(iceb_u_proverka(shetb,row1[3],0,1) == 0)
          socnvrtab[1]+=suma;

        //определяем номер в массиве
        for(i=1; i <= kodsocstrnv[0]; i++)
         if(kodsocstrnv[i] == knah)
          {
           KODSOCSTRNVSUM.plus(suma,i-1);
           break;
          }

       }
      else
       {
        suma_ndfnz_socstr_tab[0]+=knnfndsw(1,knah,row1[3],suma,&suma_ndfnz_socstr_tab[1],NULL);
       }

      if(provkodw(kodbzrnv,knah) >= 0)
       {
        bezrnvrtab[0]+=suma;      
        if(iceb_u_proverka(shetb,row1[3],0,1) == 0)
          bezrnvrtab[1]+=suma;

        //определяем номер в массиве
        for(i=1; i <= kodbzrnv[0]; i++)
         if(kodbzrnv[i] == knah)
          {
           KODBZRNVSUM.plus(suma,i-1);
          }
       }
      else
       {
        suma_ndfnz_bezr_tab[0]+=knnfndsw(1,knah,row1[3],suma,&suma_ndfnz_bezr_tab[1],NULL);
       }

      if(provkodw(kodbl,knah) >= 0)
       {
        sbolno+=suma;      
        if(godpp != gr || mespp != mr)
          sbolnpp+=suma;      
        
       }
     }

    if(prn == 2) //Удержания
     {

      if(provkodw(kuvvr_pens_sfz,knah) >= 0)
       {
        pensnvrtab[0]+=suma; //Так как удержание отрицательное, то для уменьшения суммы его надо просто прибавить
        if(iceb_u_proverka(shetbu,row1[3],0,1) == 0)
          pensnvrtab[1]+=suma;

       }

      if(knah == kodpen)
       {
        sumapnaltab[0]+=suma;
        if(iceb_u_proverka(shetbu,row1[3],0,1) == 0)
         {
          sumapnaltab[1]+=suma;
         }
        if(godpp != gr || mespp != mr)
          sumpenpp+=suma;
       }

      if(knah == kodsocstr)
       {
        sumasnaltab[0]+=suma;
        if(iceb_u_proverka(shetbu,row1[3],0,1) == 0)
          sumasnaltab[1]+=suma;
       }
      if(knah == kodbzr)
       {
        metkabezr=1;
        sumabnaltab[0]+=suma;
        if(iceb_u_proverka(shetbu,row1[3],0,1) == 0)
         {
          sumabnaltab[1]+=suma;
          metkabezrb=1;
         }
       }


     }
   }

  sumapnal[0]+=sumapnaltab[0];
  sumapnal[1]+=sumapnaltab[1];

  sumasnal[0]+=sumasnaltab[0];
  sumasnal[1]+=sumasnaltab[1];

  sumabnal[0]+=sumabnaltab[0];
  sumabnal[1]+=sumabnaltab[1];

  sumanah[0]+=sumanahtab[0];
  sumanah[1]+=sumanahtab[1];


  pensnvr[0]+=pensnvrtab[0];
  pensnvr[1]+=pensnvrtab[1];

  socnvr[0]+=socnvrtab[0];
  socnvr[1]+=socnvrtab[1];

  bezrnvr[0]+=bezrnvrtab[0];
  bezrnvr[1]+=bezrnvrtab[1];


  suma_ndfnz_pens[0]+=suma_ndfnz_pens_tab[0];
  suma_ndfnz_pens[1]+=suma_ndfnz_pens_tab[1];

  suma_ndfnz_socstr[0]+=suma_ndfnz_socstr_tab[0];
  suma_ndfnz_socstr[1]+=suma_ndfnz_socstr_tab[1];

  suma_ndfnz_bezr[0]+=suma_ndfnz_bezr_tab[0];
  suma_ndfnz_bezr[1]+=suma_ndfnz_bezr_tab[1];
  
  suma_ufn_pens_tab[0]=bb=sumanahtab[0]-pensnvrtab[0];
  suma_ufn_pens_tab[1]=bb1=sumanahtab[1]-pensnvrtab[1];
  
  //Увеличиваем доход если есть не денежные формы начисления зарплаты
//  rudnfvw(&bb,suma_ndfnz_pens_tab[0],NULL);
//  rudnfvw(&bb1,suma_ndfnz_pens_tab[1],NULL);
  
  suma_ufn_pens_tab[0]=bb-suma_ufn_pens_tab[0];
  suma_ufn_pens_tab[1]=bb1-suma_ufn_pens_tab[1];

  suma_ufn_pens[0]+=suma_ufn_pens_tab[0];
  suma_ufn_pens[1]+=suma_ufn_pens_tab[1];
  
  memset(&sumaogr,'\0',sizeof(sumaogr));

  if(bb-suma_boln_ntm > ogrzp[1])
   {
    sprintf(strsql,"Превышение для %ld\n",tabn);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    sumaogr[0]=bb-suma_boln_ntm-ogrzp[1];
    if(metkarnb == 0) //Пропорциональная схема расчета
      sumaogr[1]=(bb-suma_boln_ntm-ogrzp[1])*bb1/bb;
   }

  if(metkarnb == 1 && bb1 > ogrzp[1]) //Последовательная схема расчета
      sumaogr[1]=bb1-ogrzp[1];

  sumaprevm[0]+=sumaogr[0];
  sumaprevm[1]+=sumaogr[1];
  
  bb-=sumaogr[0];
  bb1-=sumaogr[1];
  
  if(bb <= ogrzp[0])
   {
    sumvrpens1[0]+=bb;
    sumvrpens1[1]+=bb1;
    sumapnal1[0]+=sumapnaltab[0];
    sumapnal1[1]+=sumapnaltab[1];
   }
  else
   {
    sumapnal2[0]+=sumapnaltab[0];
    sumapnal2[1]+=sumapnaltab[1];
    if(bb <= ogrzp[1])
     {
      sumvrpens2[0]+=bb;
      sumvrpens2[1]+=bb1;
     }
    else   
     {
      /***************
      Сумма больше максимального ограничения.
      В этой сумме могут быть больничные предыдущий месяцев
      В поэтому нужно брасть сумму ограничения плюс сумму на которую она должна увеличиться 
      из-за больничных предыдущих периодов
      **************/      
      double kor_sum=0.;
      double kor_sum_b=0.;
      if(suma_boln_ntm > 0.) //Значит в сумме включены больничные предыдущих месяцев
       {
        if(bb-ogrzp[1] >= suma_boln_ntm)
         kor_sum=suma_boln_ntm;
        else
         kor_sum=bb-ogrzp[1];

        if(bb1-ogrzp[1] >= suma_boln_ntm_b)
         kor_sum_b=suma_boln_ntm_b;
        else
         kor_sum_b=bb1-ogrzp[1];
       }
       
      //Делим превышение пропорционально
      if(metkarnb == 0) //Пропорциональная схема расчета
       {
        sumvrpens2[0]+=bb*(ogrzp[1]+kor_sum)/(bb+bb1);
        sumvrpens2[1]+=bb1*(ogrzp[1]+kor_sum)/(bb+bb1);
       }
      if(metkarnb == 1) //Последовательная схема расчета
       {
        sumvrpens2[0]+=ogrzp[1]+kor_sum;
        if(bb1 > ogrzp[1])
         sumvrpens2[1]+=ogrzp[1]+kor_sum_b;
       }
     }
   }


  //соцстрах
//  suma_ufn_socstr_tab[0]=bb=sumanahtab[0]-socnvrtab[0]-nmatpomtab[0];
//  suma_ufn_socstr_tab[1]=bb1=sumanahtab[1]-socnvrtab[1]-nmatpomtab[1];
  suma_ufn_socstr_tab[0]=bb=sumanahtab[0]-socnvrtab[0];
  suma_ufn_socstr_tab[1]=bb1=sumanahtab[1]-socnvrtab[1];

  //Увеличиваем доход если есть не денежные формы начисления зарплаты
//  rudnfvw(&bb,suma_ndfnz_socstr_tab[0],NULL);
//  rudnfvw(&bb1,suma_ndfnz_socstr_tab[1],NULL);
  
  suma_ufn_socstr_tab[0]=bb-suma_ufn_socstr_tab[0];
  suma_ufn_socstr_tab[1]=bb1-suma_ufn_socstr_tab[1];

  suma_ufn_socstr[0]+=suma_ufn_socstr_tab[0];
  suma_ufn_socstr[1]+=suma_ufn_socstr_tab[1];

  memset(&sumaogr,'\0',sizeof(sumaogr));

  if(bb > ogrzp[1])
   {
    sumaogr[0]=bb-ogrzp[1];
    sprintf(strsql,"Превышение %.2f для соцстраха Табельный номер %ld\n\
%.2f-%.2f=%.2f\n",
    ogrzp[1],tabn,bb,ogrzp[1],sumaogr[0]);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    if(metkarnb == 0) //Пропорциональная схема расчета
      sumaogr[1]=(bb-ogrzp[1])*bb1/bb;
   }

  if(metkarnb == 1 && bb1 > ogrzp[1] ) //Последовательная схема расчета
      sumaogr[1]=bb1-ogrzp[1];
  sumaprevmsoc[0]+=sumaogr[0];
  sumaprevmsoc[1]+=sumaogr[1];
 
  bb-=sumaogr[0];
  bb1-=sumaogr[1];


  if(bb <= nastr.prog_min_tek)
   {
    sumvrsoc1[0]+=bb;
    sumvrsoc1[1]+=bb1;
    fprintf(ff_pr_soc1,"%4s %-*.*s %8.2f %5.2f\n",
    row[0],
    iceb_u_kolbait(20,fio),iceb_u_kolbait(20,fio),fio,
    bb,sumasnaltab[0]);
    
    itogo_suma_soc1[0]+=bb;
    itogo_suma_soc1[1]+=sumasnaltab[0];
    
   }
  else
   {
    if(bb <= ogrzp[1])
     {
      sumvrsoc2[0]+=bb;
      sumvrsoc2[1]+=bb1;
      fprintf(ff_pr_soc2,"%4s %-*.*s %8.2f %5.2f\n",
      row[0],
      iceb_u_kolbait(20,fio),iceb_u_kolbait(20,fio),fio,
      bb,sumasnaltab[0]);
      itogo_suma_soc2[0]+=bb;
      itogo_suma_soc2[1]+=sumasnaltab[0];
     }
    else   
     {
      //Делим превышение пропорционально
      if(metkarnb == 0) //Пропорциональная схема расчета
       {
        sumvrsoc2[0]+=bb*ogrzp[1]/(bb+bb1);
        sumvrsoc2[1]+=bb1*ogrzp[1]/(bb+bb1);
       }
      if(metkarnb == 1) //Последовательная схема расчета
       {
        sumvrsoc2[0]+=ogrzp[1];
        if(bb1 > ogrzp[1])
         sumvrsoc2[1]+=ogrzp[1];
       }
      fprintf(ff_pr_soc2,"%4s %-*.*s %8.2f %5.2f превышение максимальной суммы\n",
      row[0],
      iceb_u_kolbait(20,fio),iceb_u_kolbait(20,fio),fio,
      ogrzp[1],sumasnaltab[0]);
      itogo_suma_soc2[0]+=ogrzp[1];
      itogo_suma_soc2[1]+=sumasnaltab[0];

     }

   }  

  if(bb <= nastr.prog_min_tek)
   {
    sumasnal1[0]+=sumasnaltab[0];
    sumasnal1[1]+=sumasnaltab[1];
   }
  else
   {
    sumasnal2[0]+=sumasnaltab[0];
    sumasnal2[1]+=sumasnaltab[1];
   }


  //Безработица*******************************

  suma_ufn_bezr_tab[0]=bb=sumanahtab[0]-bezrnvrtab[0];
  suma_ufn_bezr_tab[1]=bb1=sumanahtab[1]-bezrnvrtab[1];

  //Увеличиваем доход если есть не денежные формы начисления зарплаты
//  rudnfvw(&bb,suma_ndfnz_bezr_tab[0],NULL);
//  rudnfvw(&bb1,suma_ndfnz_bezr_tab[1],NULL);
  
  suma_ufn_bezr_tab[0]=bb-suma_ufn_bezr_tab[0];
  suma_ufn_bezr_tab[1]=bb1-suma_ufn_bezr_tab[1];

  suma_ufn_bezr[0]+=suma_ufn_bezr_tab[0];
  suma_ufn_bezr[1]+=suma_ufn_bezr_tab[1];

  memset(&sumaogr,'\0',sizeof(sumaogr));
  if(bb > ogrzp[1])
   {
    sumaogr[0]=bb-ogrzp[1];
    if(metkarnb == 0) //Пропорциональная схема расчета
       sumaogr[1]=sumaogr[0]*bb1/bb;
   }

  if(metkarnb == 1 && bb1 > ogrzp[1]) //Последовательная схема расчета
       sumaogr[1]=bb1-ogrzp[1];

  sumaprevmbez[0]+=sumaogr[0];
  sumaprevmbez[1]+=sumaogr[1];

  if(metkabezr == 0)
   {
    sumanvbezr[0]+=bb-sumaogr[0];
    if(metkabezrb == 0)
      sumanvbezr[1]+=bb1-sumaogr[1];
   }
   
  if(bb <= ogrzp[0])
   {
    sumvrbez1+=bb;
   }
  else
   {
    if(bb <= ogrzp[1])
      sumvrbez2+=bb;
    else   
      sumvrbez2+=ogrzp[1];
   }

 }

fprintf(ff_pr_soc1,"%*s %8.2f %5.2f\n",iceb_u_kolbait(25,gettext("Итого")),gettext("Итого"),itogo_suma_soc1[0],itogo_suma_soc1[1]);
fprintf(ff_pr_soc2,"%*s %8.2f %5.2f\n",iceb_u_kolbait(25,gettext("Итого")),gettext("Итого"),itogo_suma_soc2[0],itogo_suma_soc2[1]);

iceb_podpis(ff_pr_soc1,data->window);
iceb_podpis(ff_pr_soc2,data->window);

fclose(ff_pr_soc1);
fclose(ff_pr_soc2);


fprintf(ff,"%s:\n",gettext("Коды не входящие в расчет пенсионного отчисления"));
double itogonvr=0.;

if(kodnvpen != NULL)
 for(i=1; i <= kodnvpen[0]; i++)
  {
   memset(naik,'\0',sizeof(naik));
   sprintf(strsql,"select naik from Nash where kod=%d",kodnvpen[i]);
   if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     strncpy(naik,row[0],sizeof(naik)-1);   
   fprintf(ff,"%3d %-40s %8.2f\n",kodnvpen[i],naik,KODNVPENSUM.ravno(i-1));
   itogonvr+=KODNVPENSUM.ravno(i-1);
  }  

fprintf(ff,"%*s %8.2f\n",iceb_u_kolbait(44,gettext("Итого")),gettext("Итого"),itogonvr);

fprintf(ff,"%s %.2f %.2f\n",gettext("Сумма неденежных форм начисления зарплаты"),
suma_ndfnz_pens[0],suma_ndfnz_pens[1]);

double sumavras[2];
sumavras[0]=sumanah[0]-pensnvr[0]-sumaprevm[0]+suma_ufn_pens[0];
sumavras[1]=sumanah[1]-pensnvr[1]-sumaprevm[1]+suma_ufn_pens[1];


sprintf(strsql,"%s %.2f",gettext("Сумма превышающая"),ogrzp[1]);

fprintf(ff,"\n%s:\n\
%43s %s %s\n\
%-*s %10.2f %10.2f %10.2f\n\
%-*s %10.2f %10.2f %10.2f\n\
%-*s %10.2f %10.2f %10.2f\n\
%-*s %10.2f %10.2f %10.2f\n\n\
%-*s %10.2f %10.2f %10.2f\n",
gettext("Пенсионный фонд"),
" ",gettext("Хозрасчет"),gettext("Бюджет"),

iceb_u_kolbait(43,gettext("Общая сумма начислений")),gettext("Общая сумма начислений"),
sumanah[0],
sumanah[0]-sumanah[1],
sumanah[1],

iceb_u_kolbait(43,gettext("Сумма увеличения фонда")),gettext("Сумма увеличения фонда"),
suma_ufn_pens[0],
suma_ufn_pens[0]-suma_ufn_pens[1],
suma_ufn_pens[1],

iceb_u_kolbait(43,gettext("Сумма не входящая в расчет пенсионного")),gettext("Сумма не входящая в расчет пенсионного"),
pensnvr[0],
pensnvr[0]-pensnvr[1],
pensnvr[1],
iceb_u_kolbait(43,strsql),strsql,
sumaprevm[0],
sumaprevm[0]-sumaprevm[1],
sumaprevm[1],

iceb_u_kolbait(43,gettext("Сумма взятая в расчет")),gettext("Сумма взятая в расчет"),
sumavras[0],
sumavras[0]-sumavras[1],
sumavras[1]);

fprintf(ff,"\
%-*s %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(43,gettext("Сумма пенсионного отчисления с работника")),gettext("Сумма пенсионного отчисления с работника"),
sumapnal[0],sumapnal[0]-sumapnal[1],sumapnal[1]);

sprintf(strsql,"%s %s %.2f",
gettext("Сумма"),
gettext("до"),ogrzp[0]);

fprintf(ff,"\n\
%-*s %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(43,strsql),strsql,sumvrpens1[0],sumvrpens1[0]-sumvrpens1[1],sumvrpens1[1]);

fprintf(ff,"\
%-*s %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(43,gettext("Сумма налога")),gettext("Сумма налога"),
sumapnal1[0],sumapnal1[0]-sumapnal1[1],sumapnal1[1]);

sprintf(strsql,"%s  %.2f => %.2f",gettext("Сумма"),ogrzp[0],ogrzp[1]);
fprintf(ff,"\n\
%-*s %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(43,strsql),strsql,sumvrpens2[0],sumvrpens2[0]-sumvrpens2[1],sumvrpens2[1]);

fprintf(ff,"\
%-*s %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(43,gettext("Сумма налога")),gettext("Сумма налога"),
sumapnal2[0],sumapnal2[0]-sumapnal2[1],sumapnal2[1]);

fprintf(ff,"\n\
%-*s %10.2f\n\
%-*s %10.2f\n\
%-*s %10.2f\n",
iceb_u_kolbait(43,gettext("Сумма больничных")),gettext("Сумма больничных"),
sbolno,
iceb_u_kolbait(43,gettext("Сумма больничных предыдущих периодов")),gettext("Сумма больничных предыдущих периодов"),
sbolnpp,
iceb_u_kolbait(43,gettext("Сумма пенсионных в счет пред-щих периодов")),gettext("Сумма пенсионных в счет пред-щих периодов"),
sumpenpp);

/********************************/
fprintf(ff,"\n--------------------------------------------------------------------------\n");

fprintf(ff,"%s:\n",gettext("Коды не входящие в расчет соцстраха"));
itogonvr=0.;

itogonvr=0.;
if(kodsocstrnv != NULL)
 for(i=1; i <= kodsocstrnv[0]; i++)
  {
   memset(naik,'\0',sizeof(naik));
   sprintf(strsql,"select naik from Nash where kod=%d",kodsocstrnv[i]);
   if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     strncpy(naik,row[0],sizeof(naik)-1);   
   fprintf(ff,"%3d %-*s %8.2f\n",
   kodsocstrnv[i],iceb_u_kolbait(40,naik),naik,KODSOCSTRNVSUM.ravno(i-1));
   itogonvr+=KODSOCSTRNVSUM.ravno(i-1);
  }  
fprintf(ff,"%*s %8.2f\n",iceb_u_kolbait(44,gettext("Итого")),gettext("Итого"),itogonvr);

fprintf(ff,"%s %.2f %.2f\n",gettext("Сумма неденежных форм начисления зарплаты"),
suma_ndfnz_socstr[0],suma_ndfnz_socstr[1]);

sumavras[0]=sumanah[0]-socnvr[0]-sumaprevmsoc[0]+suma_ufn_socstr[0];
sumavras[1]=sumanah[1]-socnvr[1]-sumaprevmsoc[1]+suma_ufn_socstr[1];

sprintf(strsql,"%s %.2f",gettext("Сумма превышающая"),ogrzp[1]);

fprintf(ff,"\n\
%s:\n\
%43s %s %s\n\
%-*s %10.2f %10.2f %10.2f\n\
%-*s %10.2f %10.2f %10.2f\n\
%-*s %10.2f %10.2f %10.2f\n\
%-*s %10.2f %10.2f %10.2f\n\n\
%-*s %10.2f %10.2f %10.2f\n",

gettext("Соцстрах"),
" ",gettext("Хозрасчет"),gettext("Бюджет"),

iceb_u_kolbait(43,gettext("Общая сумма начислений")),gettext("Общая сумма начислений"),
sumanah[0],
sumanah[0]-sumanah[1],
sumanah[1],

iceb_u_kolbait(43,gettext("Сумма увеличения фонда")),gettext("Сумма увеличения фонда"),
suma_ufn_socstr[0],
suma_ufn_socstr[0]-suma_ufn_socstr[1],
suma_ufn_socstr[1],

iceb_u_kolbait(43,gettext("Сумма не входящая в расчет соц.страха")),gettext("Сумма не входящая в расчет соц.страха"),
socnvr[0],
socnvr[0]-socnvr[1],
socnvr[1],

iceb_u_kolbait(43,strsql),strsql,
sumaprevmsoc[0],
sumaprevmsoc[0]-sumaprevmsoc[1],
sumaprevmsoc[1],

iceb_u_kolbait(43,gettext("Сумма взятая в расчет")),gettext("Сумма взятая в расчет"),
sumavras[0],
sumavras[0]-sumavras[1],
sumavras[1]);

fprintf(ff,"\
%-*s %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(43,gettext("Сумма соц. страха с работника")),gettext("Сумма соц. страха с работника"),
sumasnal[0],sumasnal[0]-sumasnal[1],sumasnal[1]);

//***************************
sprintf(strsql,"%s %s %.2f",
gettext("Сумма"),
gettext("до"),nastr.prog_min_tek);

fprintf(ff,"\n%-*s %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(43,strsql),strsql,sumvrsoc1[0],sumvrsoc1[0]-sumvrsoc1[1],sumvrsoc1[1]);

fprintf(ff,"\
%-*s %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(43,gettext("Сумма налога")),gettext("Сумма налога"),
sumasnal1[0],sumasnal1[0]-sumasnal1[1],sumasnal1[1]);

sprintf(strsql,"%s  %.2f => %.2f",gettext("Сумма"),nastr.prog_min_tek,ogrzp[1]);

fprintf(ff,"\n%-*s %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(43,strsql),strsql,sumvrsoc2[0],sumvrsoc2[0]-sumvrsoc2[1],sumvrsoc2[1]);

fprintf(ff,"\
%-*s %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(43,gettext("Сумма налога")),gettext("Сумма налога"),
sumasnal2[0],sumasnal2[0]-sumasnal2[1],sumasnal2[1]);
//**************************************/

fprintf(ff,"\n--------------------------------------------------------------------------\n");

fprintf(ff,"%s:\n",
gettext("Коды не входящие в расчет отчисления на безработицу"));
itogonvr=0.;
if(kodbzrnv != NULL)
 for(i=1; i <= kodbzrnv[0]; i++)
  {
   memset(naik,'\0',sizeof(naik));
   sprintf(strsql,"select naik from Nash where kod=%d",kodbzrnv[i]);
   if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     strncpy(naik,row[0],sizeof(naik)-1);   
   fprintf(ff,"%3d %-*s %8.2f\n",kodbzrnv[i],iceb_u_kolbait(40,naik),naik,KODBZRNVSUM.ravno(i-1));
   itogonvr+=KODBZRNVSUM.ravno(i-1);
  }  
fprintf(ff,"%*s %8.2f\n",iceb_u_kolbait(44,gettext("Итого")),gettext("Итого"),itogonvr);

fprintf(ff,"%s %.2f %.2f\n",gettext("Сумма неденежных форм начисления зарплаты"),
suma_ndfnz_bezr[0],suma_ndfnz_bezr[1]);

sumavras[0]=sumanah[0]-bezrnvr[0]-sumaprevmbez[0]-sumanvbezr[0]+suma_ufn_bezr[0];
sumavras[1]=sumanah[1]-bezrnvr[1]-sumaprevmbez[1]-sumanvbezr[1]+suma_ufn_bezr[1];

sprintf(strsql,"%s %.2f",gettext("Сумма превышающая"),ogrzp[1]);

fprintf(ff,"\n\
%s:\n\
%43s %s %s\n\
%-*s %10.2f %10.2f %10.2f\n\
%-*s %10.2f %10.2f %10.2f\n\
%-*s %10.2f %10.2f %10.2f\n\
%-*s %10.2f %10.2f %10.2f\n\
%-*s %10.2f %10.2f %10.2f\n\n\
%-*s %10.2f %10.2f %10.2f\n",

gettext("Фонд безработицы"),
" ",gettext("Хозрасчет"),gettext("Бюджет"),

iceb_u_kolbait(43,gettext("Общая сумма начислений")),gettext("Общая сумма начислений"),
sumanah[0],
sumanah[0]-sumanah[1],
sumanah[1],

iceb_u_kolbait(43,gettext("Сумма увеличения фонда")),gettext("Сумма увеличения фонда"),
suma_ufn_bezr[0],
suma_ufn_bezr[0]-suma_ufn_bezr[1],
suma_ufn_bezr[1],

iceb_u_kolbait(43,gettext("Сумма не входящая в расчет безработицы")),gettext("Сумма не входящая в расчет безработицы"),
bezrnvr[0],
bezrnvr[0]-bezrnvr[1],
bezrnvr[1],

iceb_u_kolbait(43,gettext("Сумма пенсионеров и инвалидов")),gettext("Сумма пенсионеров и инвалидов"),
sumanvbezr[0],
sumanvbezr[0]-sumanvbezr[1],
sumanvbezr[1],

iceb_u_kolbait(43,strsql),strsql,
sumaprevmbez[0],
sumaprevmbez[0]-sumaprevmbez[1],
sumaprevmbez[1],

iceb_u_kolbait(43,gettext("Сумма взятая в расчет")),gettext("Сумма взятая в расчет"),
sumavras[0],
sumavras[0]-sumavras[1],
sumavras[1]);

fprintf(ff,"\
%-*s %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(43,gettext("Сумма отчисления в фонд безработицы")),gettext("Сумма отчисления в фонд безработицы"),
sumabnal[0],sumabnal[0]-sumabnal[1],sumabnal[1]);

iceb_podpis(ff,data->window);
fclose(ff);






data->imaf->plus(imaf);
data->imaf->plus(imaf_pr_soc1);
data->imaf->plus(imaf_pr_soc2);


data->naimf->plus(gettext("Расчёт баз налогообложения для соц. фондов"));
data->naimf->plus(gettext("Протокол хода расчёта"));
data->naimf->plus(gettext("Протокол хода расчёта"));

for(int nom=0; nom < data->imaf->kolih(); nom++)
 iceb_ustpeh(data->imaf->ravno(nom),0,data->window);


data->kon_ras=0;
data->voz=0;
gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

return(FALSE);

}
