/*$Id: zar_f1df_r.c,v 1.41 2011-08-29 07:13:44 sasa Exp $*/
/*14.01.2010	20.12.2007	Белых А.И.	zar_f1df_r.c
Расчёт формы 1ДФ
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include        "dbfhead.h"
#include "buhg_g.h"
#include "zar_f1df.h"
#include "zar_f1dfr.h"
#include "zarp1.h"
#include "sprkvrt1w.h"

class zarkvrt_mas
 {
  public:
  class iceb_u_spisok mesgod_koddoh;  /*месяц год в рамках квартала|код дохода */
                              /*Для начислений и обязательных удержаний это дата начисления. Для выплат это дата в счёт которой выплачена сумма*/
  class iceb_u_double dohod_nah;
  class iceb_u_double dohod_vip;
  class iceb_u_double podoh_nah;
  class iceb_u_double podoh_vip;  
 };


class zarkvrt_poi
 {
  public:
   class iceb_u_str kvrt;
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str tabnom;

   zarkvrt_poi()
    {
     clear();
    }

   void clear()
    {
     kvrt.new_plus("");
     datan.new_plus("");
     datak.new_plus("");
     tabnom.new_plus("");
    }


 };

class zar_f1df_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zar_f1df_rek *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_f1df_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };

gboolean   zar_f1df_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_f1df_r_data *data);
gint zar_f1df_r1(class zar_f1df_r_data *data);
void  zar_f1df_r_v_knopka(GtkWidget *widget,class zar_f1df_r_data *data);

void zarsdf1dfw(class sprkvrt1_nast *nast,GtkWidget *wpredok);
int zarkvrt_gkw(short dn,short mn,short gn,short dk,short mk,short gk,const char *shkas_shkom,const char *prist_k_tabnom,int kod_doh,const char *imatmptab,FILE *ff_prot,GtkWidget *wpredok);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

//extern short    kodpn;   /*Код подоходного налога*/
//extern short    kodperpn;   /*Код перерасчета подоходного налога*/
extern class iceb_u_str kodpn_all;
extern int      kol_strok_na_liste;
extern short	*obud; /*Обязательные удержания*/
extern struct ZAR zar;

int zar_f1df_r(class zar_f1df_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_f1df_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать форму 1ДФ"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_f1df_r_key_press),&data);

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

repl.plus(gettext("Распечатать форму 1ДФ"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_f1df_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_f1df_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_f1df_r_v_knopka(GtkWidget *widget,class zar_f1df_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_f1df_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_f1df_r_data *data)
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


/******************************/
/* Выдача шапки*/
/*******************/

void spkvd(int *klst,FILE *ff1)
{

*klst+=5;

fprintf(ff1,"\x1B\x2D\x30"); /*Выключить подчеркивание*/
fprintf(ff1,"\
------------------------------------------------------------------------------------------------------------\n\
N п/п|Ідентифі- |Сума  нар.|Сума  вип.|Сума нар. |Сума  пер.|Ознака|     Д а т а         |  Ознака  |Ознака|\n\
     |каційний  | доходу   | доходу   |приб.подат|приб.подат|доходу|---------------------|подат.соц.|(0,1) |\n\
     |  номер   |(грн.коп.)|(грн.коп.)|(грн.коп.)|(грн.коп.)|      |прийняття |звільнення|  пільги  |      |\n\
------------------------------------------------------------------------------------------------------------");
fprintf(ff1,"\x1B\x2D\x31\n"); /*Включить подчеркивание*/

}

/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void create_field(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
int *header_len,int *rec_len)
{

strncpy(f->name, fn, 11);
f->type = ft;
f->length = l1;
f->dec_point = l2;
*header_len=*header_len+sizeof(DBASE_FIELD);
*rec_len=*rec_len+l1;
}
/***********************************************/
void creatheaddbf(char imaf[],int kolz,GtkWidget *wpredok)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
#define		kolpol 15 //Количество колонок (полей) в dbf файле
DBASE_FIELD f[kolpol];
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

memset(&h,'\0',sizeof(h));
memset(&f, '\0', sizeof(f));

h.version = 3;

time(&tmm);
bf=localtime(&tmm);

h.l_update[0] = bf->tm_year;       /* yymmdd for last update*/
h.l_update[1] = bf->tm_mon+1;       /* yymmdd for last update*/
h.l_update[2] = bf->tm_mday;       /* yymmdd for last update*/

h.count = kolz;              /* number of records in file*/

header_len = sizeof(h);
rec_len = 0;
int shetshik=0;
create_field(&f[shetshik++],"NP", 'N', 5, 0,&header_len,&rec_len);
create_field(&f[shetshik++],"PERIOD", 'N', 1, 0,&header_len,&rec_len);
create_field(&f[shetshik++],"RIK", 'N', 4, 0,&header_len,&rec_len);
create_field(&f[shetshik++],"KOD", 'C', 10, 0,&header_len,&rec_len);
create_field(&f[shetshik++],"TYP", 'N', 1, 0,&header_len,&rec_len);
create_field(&f[shetshik++],"TIN", 'C', 10, 0,&header_len,&rec_len);
create_field(&f[shetshik++],"S_NAR", 'N', 12, 2, &header_len,&rec_len);
create_field(&f[shetshik++],"S_DOX", 'N', 12, 2, &header_len,&rec_len);
create_field(&f[shetshik++],"S_TAXN", 'N', 12, 2, &header_len,&rec_len);
create_field(&f[shetshik++],"S_TAXP", 'N', 12, 2, &header_len,&rec_len);
/*******до 1.1.2011
create_field(&f[shetshik++],"OZN_DOX", 'N', 2, 0, &header_len,&rec_len);
**********/
create_field(&f[shetshik++],"OZN_DOX", 'N', 3, 0, &header_len,&rec_len);
create_field(&f[shetshik++],"D_PRIYN", 'D', 8, 0, &header_len,&rec_len);
create_field(&f[shetshik++],"D_ZVILN", 'D', 8, 0, &header_len,&rec_len);
create_field(&f[shetshik++],"OZN_PILG", 'N', 2, 0, &header_len,&rec_len);
create_field(&f[shetshik++],"OZNAKA", 'N', 1, 0, &header_len,&rec_len);

h.header = header_len + 1;/* length of the header
                           * includes the \r at end
                           */
h.lrecl= rec_len + 1;     /* length of a record
                           * includes the delete
                           * byte
                          */
/*
 printw("h.header=%d h.lrecl=%d\n",h.header,h.lrecl);
*/


fd = fileno(ff);

write(fd, &h, sizeof(h));
for(i=0; i < kolpol; i++) 
 {
  write(fd, &f[i], sizeof(DBASE_FIELD));
 }
fputc('\r', ff);

fclose(ff);

}

/**********************************************/
/*Определение всех выплат между двумя месяцами*/
/***********************************************/

double svmdm(short mn,short gn,short mk,short gk,int tbn,
short obud[],short *kodudnvr,GtkWidget *wpredok)
{
short		m,g;
double		sum,sym;
int		knah;
short		mz,gz;
long		kolstr;
SQL_str         row;
char		strsql[512];

sum=0.;

m=mn;

for(g=gn; g <= gk ; g++)
for(;  ; m++)
 {
  if(g == gk && m >= mk)
   {
    break;
   }
  if(m == 13)
   {
    m=1;
    break;
   }

  /*Смотрим было ли выдача по кассе в этом периоде*/    
  /*  
  printw("Определяем былали выдача по кассе.\n");
  refresh();
  */    

  sprintf(strsql,"select knah,suma,mesn,godn,datz from Zarp where \
datz >= '%d-%02d-01' and datz <= '%d-%02d-31' and tabn=%d and prn='2' \
and suma != 0.",
  g,m,g,m,tbn);
  class SQLCURSOR cur;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    continue;
   }
  if(kolstr <= 0)
    continue;

  while(cur.read_cursor(&row) != 0)
   {
    knah=atoi(row[0]);
    if(provkodw(kodudnvr,knah) >= 0)
      continue; 
    sym=atof(row[1]);
    mz=atoi(row[2]);
    gz=atoi(row[3]);
/********       
    for(i=1; i < obud[0]+1 ; i++)
     if(knah == obud[i])
       goto vp;
*************/
    if(provkodw(obud,knah) < 0)
    if(mz == mn && gz == gn && sym < 0.)
      sum+=sym;

//vp:;
 
   }      
 }
return(sum);

}

/***********************/
/*Концовка распечаток  */
/***********************/
void koncf8dr(int kvrt,
short gn,
const char *koded,
short TYP,
double it1,double it1n,
double it2,double it2n,
double it11,double it11n,
double it21,double it21n,
int kolsh, //Количество физических лиц
int klls, //Количество листов в распечатке
int *npp1,
int metkar, //0-только распечатки 1-только для дискеты 2-распечатки и для дискеты
FILE *ff1,FILE *ff2,FILE *ff4,
GtkWidget *wpredok)
{

if(metkar == 0 || metkar == 2)
 {
  fprintf(ff1,"\x1B\x2D\x30");
  fprintf(ff2,"\x1B\x2D\x30");
  fprintf(ff1,"%-5s|%-10s|%10.2f|%10.2f|%10.2f|%10.2f|  %-4s|%-10s|%-10s|%-10s|%-6s|\n",
" X","    Х",it1n,it1,it2n,it2," Х","    X","    X","   X","  X");


  fprintf(ff2,"%5s %-10s %10.2f %10.2f %10.2f %10.2f\n",
" "," ",it11n,it11,it21n,it21);
  fprintf(ff2,"Кількість працівників - %d\n",kolsh);

  fprintf(ff1,"\
------------------------------------------------------------------------------------------------------------\n");
 }
class iceb_fioruk_rk rkruk;
iceb_fioruk(1,&rkruk,wpredok);

*npp1+=1;

if(metkar == 1 || metkar == 2)
  fprintf(ff4, " %5d%1d%04d%-10.10s%d%10.10s%12s%-12.12s%12s%12s%2s%8s%8.8s%2s%s",
  99991,kvrt,gn,koded,TYP,rkruk.inn.ravno()," ",rkruk.telef.ravno()," "," "," "," "," "," "," ");

class iceb_fioruk_rk rkbuh;
iceb_fioruk(2,&rkbuh,wpredok);

*npp1+=1;
if(metkar == 1 || metkar == 2)
 fprintf(ff4, " %5d%1d%04d%-10.10s%d%10.10s%12s%-12.12s%12s%12s%2s%8s%8.8s%2s%s",
 99992,kvrt,gn,koded,TYP,rkbuh.inn.ravno()," ",rkbuh.telef.ravno()," "," "," "," "," "," "," ");

if(metkar == 0 || metkar == 2)
 {
  zarsf1dfw(*npp1,kolsh,klls,TYP,ff1,wpredok);
 }
*npp1+=1;

if(metkar == 1 || metkar == 2)
 fprintf(ff4, " %5d%1d%04d%-10.10s%d%10.10s%12.2f%12.2f%12.2f%12.2f%2s%8s%8.8s%2s%s",
 99999,kvrt,gn,koded,TYP," ",it1n,it1,it2n,it2," "," "," "," "," ");

if(metkar == 0 || metkar == 2)
 {


  iceb_podpis(ff1,wpredok);
  iceb_podpis(ff2,wpredok);

 }
}

/*********************************/
/*Запись в dbf файл              */
/*********************************/

void  zapf8dr(int npp1,short kvrt,short gn,const char *koded,short TYP,const char *inn,
double suman,double sumav,double podohn,double podohv,const char *priz,const char *datn,const char *datk,
short kodlg,short vidz,FILE *ff4)
{
/*******до 1.1.2011
fprintf(ff4, " %5.5d%1d%04d%-10.10s%d%-10.10s%12.2f%12.2f%12.2f%12.2f%2.2s%8.8s%8.8s%2.2d%1.1d",
npp1,kvrt,gn,koded,TYP,inn,suman,sumav,podohn,podohv,priz,datn,datk,kodlg,vidz);
******************/
fprintf(ff4, " %5.5d%1d%04d%-10.10s%d%-10.10s%12.2f%12.2f%12.2f%12.2f%3.3s%8.8s%8.8s%2.2d%1.1d",
npp1,kvrt,gn,koded,TYP,inn,suman,sumav,podohn,podohv,priz,datn,datk,kodlg,vidz);
}

/***********************/
/*Счетчик строк        */
/***********************/

void ssf8dr(int *klst,int *kollist,FILE *ff)
{
*klst+=1;
if(*klst < kol_strok_na_liste)
  return; 
fprintf(ff,"\f");
*klst=0;
spkvd(klst,ff);
*kollist+=1;
}

/*********************/
/*Заголовок формы 1дф*/
/*********************/
void zagf1df(int kolrh, //Количество работников
int kolrs,//Количество совместителей
short kvrt,const char *kod,short TYP,
int *klst,int vidd, //1-вилучення
short dn,short mn,short gn,
short dk,short mk,short gk,
int metka_oth, //0-звітний 1-новий звітний 2-уточнюючий
FILE *ff1,FILE *ff2,
GtkWidget *wpredok)
{
char  stroka[1024];
struct  tm      *bf;
time_t          tmm;
char            bros[512];
time(&tmm);
bf=localtime(&tmm);
iceb_u_zagolov("Розсчёт довідки за квартал (форма N1ДФ)",dn,mn,gn,dk,mk,gk,organ,ff2);
fprintf(ff2,"%s:%d\n","Квартал",kvrt);



int kkk=0;
spkvd(&kkk,ff2);

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"zarsf1df_1.alx"};

class iceb_u_str adres("");
sprintf(strsql,"select adres from Kontragent where kodkon='00'");
if(iceb_sql_readkey(strsql,&row_alx,&cur_alx,wpredok) == 1)
 adres.new_plus(row_alx[0]);

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return;
 }


char naim_ni[512];
char kodni[32];
kodni[0]='\0';
iceb_poldan("Код налоговой инспекции",kodni,"zarsdf1df.alx",wpredok);

memset(naim_ni,'\0',sizeof(naim_ni));
iceb_poldan("Название налоговой инспекции",naim_ni,"zarsdf1df.alx",wpredok);

char period[40];
memset(period,'\0',sizeof(period));

if(kvrt == 1)
  sprintf(period,"I квартал %dр.",gn);
if(kvrt == 2)
  sprintf(period,"II квартал %dр.",gn);
if(kvrt == 3)
  sprintf(period,"III квартал %dр.",gn);
if(kvrt == 4)
  sprintf(period,"IV квартал %dр.",gn);

int nomstr=0;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomstr++;
  memset(stroka,'\0',sizeof(stroka));
  strncpy(stroka,row_alx[0],sizeof(stroka)-1);
  switch(nomstr)
   {
    case 7:
     iceb_u_bstab(stroka,kod,1,28,2);
     break;

    case 10:
      if(metka_oth == 0) //звитний
       iceb_u_bstab(stroka,"X",88,95,1);
     break;

    case 12:
      if(TYP == 0) //юридическое лицо
       iceb_u_bstab(stroka,"X",2,4,1);
      if(metka_oth == 1) //новий звитний
       iceb_u_bstab(stroka,"X",87,95,1);
     break;

    case 14:
      if(TYP == 1) //физицеское лицо
       iceb_u_bstab(stroka,"X",2,4,1);
      if(metka_oth == 2) //уточнюючий
       iceb_u_bstab(stroka,"X",87,95,1);
     break;

    case 19:
     iceb_u_bstab(stroka,organ,16,115,1);
     break;

    case 22:
     iceb_u_bstab(stroka,adres.ravno(),17,115,1);
     break;    

    case 25:
     iceb_u_bstab(stroka,naim_ni,27,115,1);
     break;

    case 26:
     iceb_u_bstab(stroka,kodni,1,21,2);
     break;     
    case 28:
     iceb_u_bstab(stroka,period,37,105,1);
     break;
     
    case 31:
     if(kolrh != 0)
      {  
       sprintf(bros,"%d",kolrh);
       iceb_u_bstab(stroka,bros,44,54,1);
      }
     break;   

    case 33:
     if(kolrs != 0)
      { 
       sprintf(bros,"%d",kolrs);
       iceb_u_bstab(stroka,bros,44,54,1);
      }
     break;   
   }


  fprintf(ff1,"%s",stroka);  
  *klst+=1;
 }
 
}
/**************************/
/*Чтение нужных записей по фондам*/
/***********************/

double suma_gosstrah(int tabn,short mn,short gn,char *gosstrah,
int metka, //0-за квартал 1-за месяц
GtkWidget *wpredok)
{
SQL_str row;
SQLCURSOR cur;
int kolstr;
char strsql[512];

if(gosstrah[0] == '\0')
 return(0.);

if(metka == 0)
  sprintf(strsql,"select sumap from Zarsocz where datz >= '%04d-%02d-01' \
and datz <=  '%04d-%02d-31' and tabn=%d ",gn,mn,gn,mn+2,tabn);
if(metka == 1)
  sprintf(strsql,"select sumap from Zarsocz where datz >= '%04d-%02d-01' \
and datz <=  '%04d-%02d-31' and tabn=%d ",gn,mn,gn,mn,tabn);
 
char bros[512];

int kol_pol=iceb_u_pole2(gosstrah,',');
if(kol_pol == 0)
 {
  sprintf(bros," and kodz=%s",gosstrah);
  strcat(strsql,bros);
 }
else
 {
  char kod[32];
  for(int i=0; i < kol_pol; i++)
   {
    iceb_u_polen(gosstrah,kod,sizeof(kod),i+1,',');
    if(i == 0)
      sprintf(bros," and (kodz=%s",kod);
    else
      sprintf(bros," or kodz=%s",kod);
    strcat(strsql,bros);
   }
  strcat(strsql,")");
 }




//printw("%s\n",strsql);
//OSTANOV();

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }
double suma=0.;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s\n",row[0]);
  
  suma+=atof(row[0]);  
 }
//OSTANOV();
 
return(suma);
}
/****************************/
/*запись в массивы*/
/******************************/

void zarkvrt_zp(short m,short g,
int kod_doh,
class zarkvrt_mas *mas,
double dohod_nah,
double dohod_vip,
double podoh_nah,
double podoh_vip,
FILE *ff_prot)
{
int nomer_v_sp=0;
char strsql[512];
sprintf(strsql,"%02d.%04d|%d",m,g,kod_doh);

if((nomer_v_sp=mas->mesgod_koddoh.find(strsql)) < 0)
 mas->mesgod_koddoh.plus(strsql);

fprintf(ff_prot,"%s-дата:%d.%d|%d\n",__FUNCTION__,m,g,kod_doh);
if(nomer_v_sp < 0)
 fprintf(ff_prot,"\
Сумма до записи    %10.2f %10.2f %8.2f %8.2f %d\n",0.,0.,0.,0.,nomer_v_sp);
else
 fprintf(ff_prot,"\
Сумма до записи    %10.2f %10.2f %8.2f %8.2f %d\n",
 mas->dohod_nah.ravno(nomer_v_sp),
 mas->dohod_vip.ravno(nomer_v_sp),
 mas->podoh_nah.ravno(nomer_v_sp),
 mas->podoh_vip.ravno(nomer_v_sp),
 nomer_v_sp);
mas->dohod_nah.plus(dohod_nah,nomer_v_sp);
mas->dohod_vip.plus(dohod_vip,nomer_v_sp);
mas->podoh_nah.plus(podoh_nah,nomer_v_sp);
mas->podoh_vip.plus(podoh_vip,nomer_v_sp);

if(nomer_v_sp < 0)
 nomer_v_sp=mas->mesgod_koddoh.kolih()-1;
 
 
fprintf(ff_prot,"\
Записываем         %10.2f %10.2f %8.2f %8.2f\n\
Сумма после записи %10.2f %10.2f %8.2f %8.2f %d\n",
dohod_nah,
dohod_vip,
podoh_nah,
podoh_vip,
mas->dohod_nah.ravno(nomer_v_sp),
mas->dohod_vip.ravno(nomer_v_sp),
mas->podoh_nah.ravno(nomer_v_sp),
mas->podoh_vip.ravno(nomer_v_sp),
nomer_v_sp);
}
/*******************************************/
/*Определяем сколько начислено и сколько к выдаче наруки*/
/*********************************************************/

void zarkvrt_nv(int tabn,short m,short g,double *nahisl,double *suma_obud,double *podoh_nah,GtkWidget *wpredok)
{
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
double suma=0.;
*nahisl=0.;
*suma_obud=0.;

sprintf(strsql,"select prn,knah,suma from Zarp where tabn=%d and datz >= '%04d-%02d-01' and \
datz <= '%04d-%02d-31' and suma <> 0.",tabn,g,m,g,m);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 return;
 
while(cur.read_cursor(&row) != 0)
 {
  suma=atof(row[2]);
  if(atoi(row[0]) == 1)
   {
    *nahisl+=suma;   
   }
  else
   {
    if(provkodw(obud,row[1]) >= 0)
     *suma_obud+=suma;        
    if(iceb_u_proverka(kodpn_all.ravno(),row[1],0,0) == 0)
     *podoh_nah+=suma;
   }
 }

*podoh_nah*=-1; /*Делаем поплжительным*/

}
/**********************************************/
/*Запись во временную таблицу*/
/****************************************/
void  zarkvrt_zapvtabl(const char *imatmptab,
const char *inn,
int kod_doh,
const char *kod_lgoti,
double dohod_nah,
double dohod_vip,
double podoh_nah,
double podoh_vip,
const char *datanr,
const char *datakr,
const char *fio,       /*фамилия или наименование контрагента*/
const char *tab_kontr, /*табелыный номeр или код контрагента*/
FILE *ff_prot,
GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select inn from %s where inn='%s' and pd=%d and kl='%s'",imatmptab,inn,kod_doh,kod_lgoti);

if(iceb_sql_readkey(strsql,wpredok) == 0) /*нет такой записи*/
 {
   sprintf(strsql,"insert into %s values('%s',%d,%.2f,%.2f,%.2f,%.2f,'%s','%s','%s','%s','%s')",
   imatmptab,
   inn,
   kod_doh,
   dohod_nah,
   dohod_vip,
   podoh_nah,
   podoh_vip,
   datanr,
   datakr,
   kod_lgoti,
   fio,
   tab_kontr);
 }
else /*запись существует*/
 {
   sprintf(strsql,"update %s set \
ndoh=ndoh+%.2f,\
vdoh=vdoh+%.2f,\
npod=npod+%.2f,\
vpod=vpod+%.2f,\
kl='%s' \
where inn='%s' and pd=%d and kl='%s'",
   imatmptab,
   dohod_nah,
   dohod_vip,
   podoh_nah,
   podoh_vip,
   kod_lgoti,
   inn,kod_doh,kod_lgoti);
 }
fprintf(ff_prot,"%s-%s\n",__FUNCTION__,strsql);
     
iceb_sql_zapis(strsql,1,0,wpredok);


}
/*********************************/
/*чтение начислений на зарплату*/
/*******************************/
double zarkvrt_esv(int tabn,
short mn,short gn, /*месяц начала*/
short dk,short mk,short gk, /*Дата конца для получения отчёта за квартал*/
char *gosstrah,
int metka, //0-за квартал 1-за месяц
GtkWidget *wpredok)
{
SQL_str row;
SQLCURSOR cur;
int kolstr;
char strsql[1024];
if(gosstrah[0] == '\0')
 return(0.);

if(metka == 0)
 {
  short denk=0,mesk=0,godk=0;
  short dkm=0; /*последний день в месяце конца*/

  iceb_u_dpm(&dkm,&mk,&gk,5); /*получаем последний день в месяце конца*/
  mesk=mk;
  godk=gk;
  if(dkm > dk) /*расчёт с захватом выплат в следующем квартале*/
   {
    iceb_u_dpm(&denk,&mesk,&godk,4); /*уменьшаем на месяц*/
    iceb_u_dpm(&denk,&mesk,&godk,5); /*получаем последний день месяца*/
   } 
  else
   {
    denk=dkm;
   }
  sprintf(strsql,"select sumap from Zarsocz where datz >= '%04d-%02d-01' \
and datz <=  '%04d-%02d-%02d' and tabn=%d ",gn,mn,godk,mesk,denk,tabn);
 }

if(metka == 1)
  sprintf(strsql,"select sumap from Zarsocz where datz >= '%04d-%02d-01' \
and datz <=  '%04d-%02d-31' and tabn=%d ",gn,mn,gn,mn,tabn);
 
char bros[512];

int kol_pol=iceb_u_pole2(gosstrah,',');
if(kol_pol == 0)
 {
  sprintf(bros," and kodz=%s",gosstrah);
  strcat(strsql,bros);
 }
else
 {
  char kod[32];
  for(int i=0; i < kol_pol; i++)
   {
    iceb_u_polen(gosstrah,kod,sizeof(kod),i+1,',');
    if(i == 0)
      sprintf(bros," and (kodz=%s",kod);
    else
      sprintf(bros," or kodz=%s",kod);
    strcat(strsql,bros);
   }
  strcat(strsql,")");
 }




//printw("%s\n",strsql);
//OSTANOV();

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }
double suma=0.;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s\n",row[0]);
  
  suma+=atof(row[0]);  
 }
//OSTANOV();
 
return(suma);
}
/****************************************/
/*определение кода дохода для подоходного налога*/
/**************************************************/

int zarkvrt_kdfpn(short mr,short gr,class zarkvrt_mas *mas,FILE *ff_prot)
{
int kod_doh=0;
char bros[512];
short m,g;
int kolih=mas->mesgod_koddoh.kolih();

if(kolih == 0) /*нет начислений*/
 return(101);

/*проверяем есть ли начисление по 101 коду дохода - если есть возвращаем 101*/
for(int nom=0; nom < kolih; nom++)
 {
  iceb_u_polen(mas->mesgod_koddoh.ravno(nom),bros,sizeof(bros),1,'|');
  iceb_u_rsdat1(&m,&g,bros);
  if(iceb_u_sravmydat(1,mr,gr,1,m,g) == 0)
   {
    iceb_u_polen(mas->mesgod_koddoh.ravno(nom),&kod_doh,2,'|');
    if(kod_doh == 101)
      return(kod_doh);    
   }  
 }

/*берём первый в списке*/
for(int nom=0; nom < kolih; nom++)
 {
  iceb_u_polen(mas->mesgod_koddoh.ravno(nom),bros,sizeof(bros),1,'|');
  iceb_u_rsdat1(&m,&g,bros);
  if(iceb_u_sravmydat(1,mr,gr,1,m,g) == 0)
   {
    iceb_u_polen(mas->mesgod_koddoh.ravno(nom),&kod_doh,2,'|');
    return(kod_doh);    
   }  
 }
/*если по месяцу нет записей возвращаем 101*/
return(101);
}



/******************************************/
/******************************************/

gint zar_f1df_r1(class zar_f1df_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);
//class dlgot sl;   //Выплаченные суммы
//class dlgot slnah;  //Начисленные суммы

short dn,mn,gn;
short dk,mk,gk;

iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window);


short dkk=0; /*последний день месяца*/
iceb_u_dpm(&dkk,&mk,&gk,5);

class iceb_u_str fio("");

char imaf_1df[64];
char imaf_1df_fam[64];
char imaf_prot[64];
char imaf_dbf[64];
char imaf_dbf_tmp[64];

char imaf_xml[64];
int kolstr=0;
int kolstr1=0;
class iceb_u_str inn("");
SQL_str row,row1;
class SQLCURSOR cur,cur1;
short d,m,g;
class iceb_u_str datanr(""); /*дата начала работы*/
class iceb_u_str datakr(""); /*дата конца работы*/
int kolih_rab=0;
int kolih_sovm=0;
int tabnom=0;
int nomer_v_sp=0;
FILE *ff_prot=NULL;
int metka_sovm=0;
int kod_doh=101;
int kod_doh_for_podoh=0; /*Код дохода для подоходного налога*/

//читаем список кодов доходов 
class sprkvrt1_nast nast;
zarsdf1dfw(&nast,data->window);






sprintf(strsql,"select distinct tabn from Zarp where datz >= '%04d-%02d-%02d' and \
datz <= '%04d-%02d-%02d' and suma != 0.",gn,mn,1,gk,mk,dk);

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
sprintf(imaf_prot,"zkvrt%d.lst",getpid());
if((ff_prot=fopen(imaf_prot,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_prot,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_tmptab tabtmp;
const char *imatmptab={"zarkvrt"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
inn char(16) not null default '',\
pd int not null default 0,\
ndoh double(10,2) not null default 0.,\
vdoh double(10,2) not null default 0.,\
npod double(8,2) not null default 0.,\
vpod double(8,2) not null default 0.,\
datap DATE NOT NULL DEFAULT '0000-00-00',\
datau DATE NOT NULL DEFAULT '0000-00-00',\
kl char(8) NOT NULL DEFAULT '',\
fio char(128) NOT NULL DEFAULT '',\
tn char(32) NOT NULL DEFAULT '',\
unique (inn,pd,kl))",
imatmptab);

/*********************
 0 inn   - индивидуальный налоговый номер
 1 pd    - признак дохода
 2 ndoh  - начисленный доход
 3 vdoh  - выплаченый доход
 4 npod  - начисленный подоходный налог
 5 vpod  - выплаченный подоходный налог
 6 datap - дата приёма на работу
 7 datau - дата увольнения с работы
 8 kl    - код льготы
 9 fio   - фамилия / наименование контрагента
10 tn    - табельный номер/код контрагента
***********************/

if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 return(-1);

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
//  strzag(LINES-1,0,kolstr,++kolstr1);
  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
   continue;
  
  tabnom=atoi(row[0]);
  sprintf(strsql,"select fio,datn,datk,sovm,inn from Kartb where tabn=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
   {
    sprintf(strsql,"%s %s!",gettext("Не найдена карточка для табельного номера"),row[0]);
    iceb_menu_soob(strsql,data->window);
    continue;
   }  
    
  fio.new_plus(row1[0]);
  inn.new_plus(row1[4]); 

  sprintf(strsql,"%6s %s\n",row[0],fio.ravno());
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  fprintf(ff_prot,"\n%s %s\n---------------------------------\nДата приёма на работу:%s Дата увольнения:%s\n",row[0],fio.ravno(),row1[1],row1[2]);
  
  if(inn.getdlinna() <= 1)
   {
    fprintf(ff_prot,"Не введён идентификационной номер!!!!!!!\n");
    continue;
   }

  datanr.new_plus("");
  datakr.new_plus("");

  iceb_u_rsdat(&d,&m,&g,row1[1],2);
  if(iceb_u_sravmydat(d,m,g,1,mn,gn) >= 0 && iceb_u_sravmydat(d,m,g,31,mk,gk) <= 0)
   datanr.new_plus(row1[1]);     

  iceb_u_rsdat(&d,&m,&g,row1[2],2);
  if(iceb_u_sravmydat(d,m,g,1,mn,gn) >= 0 && iceb_u_sravmydat(d,m,g,31,mk,gk) <= 0)
   datakr.new_plus(row1[2]);     
  
  metka_sovm=atoi(row1[3]);
  

  class zarkvrt_mas mas;
  
   
  /*Смотрим все записи  в диапазоне дат День начала обязательно первый день месяца начала */
  sprintf(strsql,"select datz,prn,knah,suma,godn,mesn from Zarp where tabn=%d and datz >= '%04d-%02d-%02d' and \
datz <= '%04d-%02d-%02d' and suma <> 0. order by prn asc",tabnom,gn,mn,1,gk,mk,dk);

  if(cur1.make_cursor(&bd,strsql) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue; 
   }


  class iceb_u_spisok mesn_godn; /*список дат в счёт которых были выплаты|месяц когда выплачено*/
  class iceb_u_double suma_viplat;

  kod_doh_for_podoh=0; /*Код дохода для подоходного налога*/
  int knah=0;
  double suma=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
    iceb_u_rsdat(&d,&m,&g,row1[0],2);
    knah=atoi(row1[2]);
    suma=atof(row1[3]);
    fprintf(ff_prot,"%s %s %s %s %s %s\n",row1[0],row1[1],row1[2],row1[3],row1[4],row1[5]);
    if(atoi(row1[1]) == 1) /*начисления*/    
     {
      //коды которые не входят в 1ДФ
      if(provkodw(nast.kodnvf8dr,knah) >= 0)
          continue; 

      if(iceb_u_sravmydat(d,m,g,1,mn,gn) >= 0 && iceb_u_sravmydat(d,m,g,31,mk,gk) <= 0) /*Дата строго в рамках квартала*/
       {
        kod_doh=101;
        for(int nom=0; nom < nast.kodi_nah.kolih(); nom++)
         {
          if(iceb_u_proverka(nast.kodi_nah.ravno(nom),knah,0,1) == 0)
            kod_doh=nast.kodi_pd.ravno(nom);         
         }          
        zarkvrt_zp(m,g,kod_doh,&mas,suma,0.,0.,0.,ff_prot);

            
       }
     }
    else  /*Удержания*/
     {
      /*Выплаты не входящие в 1ДФ*/
      if(provkodw(nast.kodud,knah) >= 0)
         continue;

      if(iceb_u_sravmydat(d,m,g,1,mn,gn) >= 0 && iceb_u_sravmydat(d,m,g,31,mk,gk) <= 0) /*строго квартал*/
       {
        if(iceb_u_proverka(kodpn_all.ravno(),row1[2],0,0) == 0) /*начисленный подоходный налог */
         {
          /*нужно определить код дохода для подоходного налога - если в массиве уже есть 101 то на него если его вообще нет то на первый*/                         
          if(kod_doh_for_podoh == 0)
            kod_doh_for_podoh=zarkvrt_kdfpn(m,g,&mas,ff_prot);
          zarkvrt_zp(m,g,kod_doh_for_podoh,&mas,0.,0.,suma*-1,0.,ff_prot);
          continue;
         }
       }            

      if(provkodw(obud,knah) < 0) /*если это не обязательное удержание то это выплата*/
       {
        short godn=atoi(row1[4]);
        short mesn=atoi(row1[5]); 

        if(iceb_u_sravmydat(d,m,g,dn,mn,gn)  < 0)  /*если выплаты до даты начала и в счёт предыдущего периода то пропускаем*/
         if(iceb_u_sravmydat(1,mesn,godn,1,mn,gn) < 0)
          {
           fprintf(ff_prot,"Выплата в счёт предыдущего квартала!\n");
           continue;
          }

        if(dkk > dk) /*если день конца не равен последнему дню месяца значит захватываем следующий месяц за кварталом*/
        if(iceb_u_sravmydat(d,m,g,1,mk,gk) >= 0 && iceb_u_sravmydat(d,m,g,dk,mk,gk)  <= 0)  /*если выплаты от конца квартала и до даты конца и в счёт сдедующего квартала то пропускаем*/
         if(iceb_u_sravmydat(1,mesn,godn,1,mk,gk) >= 0)
          {
           fprintf(ff_prot,"Выплата в счёт следующего квартала! %d.%d\n",mesn,godn);
           continue;
          }

        fprintf(ff_prot,"%02d.%02d.%04d -> %02d.%04d Сумма выплаты:%.2f\n",d,m,g,mesn,godn,suma);        

        sprintf(strsql,"%02d.%04d|%02d.%04d",mesn,godn,m,g);
        if((nomer_v_sp=mesn_godn.find(strsql)) < 0)
         mesn_godn.plus(strsql);

        suma_viplat.plus(suma,nomer_v_sp);
        
       }

     }      
      
       
   
   }
  /*читаем начисления на зарплату за квартал*/
  double suma_esv_nah_kvrt=zarkvrt_esv(tabnom,mn,gn,dk,mk,gk,nast.gosstrah,0,data->window);
  double suma_esv_vip=0.;
  
  /*определяем выплаченный доход и подоходный налог*/
  for(int nom=0; nom < mesn_godn.kolih(); nom++)
   {
    double suma_v=suma_viplat.ravno(nom);
    
    d=1;
    short mes_vip=0;
    short god_vip=0;        
    iceb_u_rsdat1(&m,&g,mesn_godn.ravno(nom)); /*Дата в счёт которой выплачено*/

    double suma_esv_nah_mes=zarkvrt_esv(tabnom,m,g,0,0,0,nast.gosstrah,1,data->window); /*сумма начислений на заралату в месяце в счёт которого выплачено*/
    
    
    iceb_u_polen(mesn_godn.ravno(nom),strsql,sizeof(strsql),1,'|');
    iceb_u_rsdat1(&mes_vip,&god_vip,strsql);
    fprintf(ff_prot,"\n%d.%d -> %d.%d\n",mes_vip,god_vip,m,g);

    double nahisl=0.;
    double suma_obud=0.;
    double suma_kvid=0.;
    double podoh_nah=0.;

    /*определяем сколько начислено, сумму обязательных удержаний и сумму подоходного налога*/
    zarkvrt_nv(tabnom,m,g,&nahisl,&suma_obud,&podoh_nah,data->window);

    suma_kvid=nahisl+suma_obud;
    double koef_v=0.;
    if(suma_kvid != 0.)
      koef_v=suma_v/suma_kvid*-1; /*Умножаем на -1 чтобы был коэффициент положительный*/
    double vip_dohod=nahisl*koef_v;
    double vip_podoh=podoh_nah*koef_v;

    suma_esv_vip+=suma_esv_nah_mes*koef_v;    

    fprintf(ff_prot,"\
Сумма выплаты               :%.2f\n\
Сумма всех начислений       :%.2f\n\
Сумма обязательных удержаний:%.2f\n\
Cумма к выдаче              :%.2f\n\
Коэффициент выплаты %.2f/%.2f=%f\n\
Выплаченый доход:%.2f*%f=%.2f\n\
Выплаченный подоходный налог:%.2f*%f=%.2f\n",
    suma_v,
    nahisl,
    suma_obud,
    suma_kvid,
    suma_v,
    suma_kvid,
    koef_v,
    nahisl,
    koef_v,
    vip_dohod,
    podoh_nah,
    koef_v,
    vip_podoh);

    if(kod_doh_for_podoh == 0) /*будет равен 0 если небыло выплат*/    
       kod_doh_for_podoh=zarkvrt_kdfpn(mes_vip,god_vip,&mas,ff_prot);

    zarkvrt_zp(mes_vip,god_vip,kod_doh_for_podoh,&mas,0.,vip_dohod,0.,vip_podoh,ff_prot);
    
   
   }

  if(suma_esv_nah_kvrt != 0. || suma_esv_vip != 0.)
   {
    zarkvrt_zp(mn,gn,133,&mas,suma_esv_nah_kvrt,suma_esv_vip,0.,0.,ff_prot);
   }

  if(mas.mesgod_koddoh.kolih() == 0)
   {
    fprintf(ff_prot,"Не найдено ни начислений ни удержаний!\n");
    continue;
   }

  if(metka_sovm == 0)
   kolih_rab++;
  else
   kolih_sovm++;
  
  class iceb_u_spisok mes_god_kod_lgoti; /*месяц год*/
  class iceb_u_spisok sp_kod_lgoti; /*Список кодов льгот по датам*/
  class iceb_u_str kod_lgoti("");

   /*Записываем в промежуточную таблицу*/
  for(int nom=0; nom < mas.mesgod_koddoh.kolih(); nom++)
   {
    kod_lgoti.new_plus("");
    iceb_u_polen(mas.mesgod_koddoh.ravno(nom),&kod_doh,2,'|');

    if(kod_doh == 101)
     {
      iceb_u_rsdat1(&m,&g,mas.mesgod_koddoh.ravno(nom));
      
      sprintf(strsql,"%02d.%04d",m,g);
      if((nomer_v_sp=mes_god_kod_lgoti.find(strsql)) < 0)
       {
        mes_god_kod_lgoti.plus(strsql);
        sprintf(strsql,"select lgot from Zarn where tabn=%d and god=%d and mes=%d",tabnom,g,m);

        if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
         {
          if(atoi(row1[0]) != 0) /*вместо кода льготы может быть мусор*/
           {
            sprintf(strsql,"%02d",atoi(row1[0]));
            kod_lgoti.new_plus(strsql);
           }
         }        
        sp_kod_lgoti.plus(kod_lgoti.ravno());
        
       }
      else
       {
        kod_lgoti.new_plus(sp_kod_lgoti.ravno(nomer_v_sp));
       }
     }
    
    /*Запись во временную таблицу*/   
    zarkvrt_zapvtabl(imatmptab,inn.ravno(),kod_doh,kod_lgoti.ravno(),
    mas.dohod_nah.ravno(nom),
    mas.dohod_vip.ravno(nom),
    mas.podoh_nah.ravno(nom),
    mas.podoh_vip.ravno(nom),
    datanr.ravno(),datakr.ravno(),fio.ravno_filtr(),row[0],ff_prot,data->window);
       

    datanr.new_plus("0000-00-00");
    datakr.new_plus("0000-00-00");
   }      
 }

/*Смотрим настройки поиска данных в проводках*/

char prist_k_tabnom[64];

memset(prist_k_tabnom,'\0',sizeof(prist_k_tabnom));
iceb_poldan("Приставка к табельному номеру",prist_k_tabnom,"zarnast.alx",data->window);
kod_doh=0;
sprintf(strsql,"Кількість налаштовок на пошук проводок:%d\n",nast.kodi_pd_prov.kolih());
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

if(data->rk->tabnom.getdlinna() <= 1)
for(int kkk=0; kkk < nast.kodi_pd_prov.kolih(); kkk++)
 {
  kod_doh=nast.kodi_pd_prov.ravno(kkk);

  sprintf(strsql,"%d:%s\n",kod_doh,nast.sheta.ravno(kkk));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  zarkvrt_gkw(1,mn,gn,31,mk,gk,nast.sheta.ravno(kkk),prist_k_tabnom,kod_doh,imatmptab,ff_prot,data->window);
  
 }



/*формируем отчёт*/
sprintf(strsql,"select * from %s order by inn asc,pd asc",imatmptab);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи в промежуточной таблице!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_str naim_kontr("");
class iceb_u_str kod_edrpou("");
class iceb_u_str telefon("");
sprintf(strsql,"select naikon,kod,telef from Kontragent where kodkon='00'");
if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
 {
  naim_kontr.new_plus(row1[0]);
  kod_edrpou.new_plus(row1[1]);
  telefon.new_plus(row1[2]);
 }

FILE *ff_1df;
sprintf(imaf_1df,"f1df%d.lst",getpid());
if((ff_1df= fopen(imaf_1df,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_1df,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

FILE *ff_1df_fam;
sprintf(imaf_1df_fam,"f1dff%d.lst",getpid());
if((ff_1df_fam= fopen(imaf_1df_fam,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_1df_fam,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

FILE *ff_dbf_tmp;
sprintf(imaf_dbf_tmp,"f1df%d.tmp",getpid());
if((ff_dbf_tmp= fopen(imaf_dbf_tmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_dbf_tmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int shet_str_1df=0; /*счётчик записей в файле*/
int kolih_str_1df=0; /*Количество зписей в файле*/
int nomer_xml_str=0;
//Заголовок распечаток
zagf1df(kolih_rab,kolih_sovm,data->rk->kvart.ravno_atoi(),kod_edrpou.ravno(),nast.TYP,&shet_str_1df,0,dn,mn,gn,dk,mk,gk,0,ff_1df,ff_1df_fam,data->window);

spkvd(&shet_str_1df,ff_1df);

//заголовок xml файла
FILE *ff_xml=NULL;
memset(imaf_xml,'\0',sizeof(imaf_xml));
f1df_xml_zagw(data->rk->kvart.ravno_atoi(),gn,kod_edrpou.ravno(),nast.TYP,0,kolih_rab,kolih_sovm,naim_kontr.ravno(),imaf_xml,&ff_xml,data->window);

double nah_dohod=0.;
double vip_dohod=0.;
double nah_podoh=0.;
double vip_podoh=0.;

double itogo_nah_dohod=0.;
double itogo_vip_dohod=0.;
double itogo_nah_podoh=0.;
double itogo_vip_podoh=0.;
int kol_list=0;
int nomer_str=0;
short dp,mp,gp;
short du,mu,gu;
char datnr_lst[16];
char datnr_dbf[16];
char datkr_lst[16];
char datkr_dbf[16];
char datnr_xml[16];
char datkr_xml[16];

while(cur.read_cursor(&row) != 0)
 {
  nah_dohod=atof(row[2]);
  vip_dohod=atof(row[3]);
  nah_podoh=atof(row[4]);
  vip_podoh=atof(row[5]);  

  itogo_nah_dohod+=nah_dohod;
  itogo_vip_dohod+=vip_dohod;
  itogo_nah_podoh+=nah_podoh;
  itogo_vip_podoh+=vip_podoh;

  memset(datnr_lst,'\0',sizeof(datnr_lst));  
  memset(datkr_lst,'\0',sizeof(datkr_lst));  
  memset(datnr_dbf,'\0',sizeof(datnr_dbf));  
  memset(datkr_dbf,'\0',sizeof(datkr_dbf));  
  memset(datnr_xml,'\0',sizeof(datnr_xml));  
  memset(datkr_xml,'\0',sizeof(datkr_xml));  

  iceb_u_rsdat(&dp,&mp,&gp,row[6],2);
  iceb_u_rsdat(&du,&mu,&gu,row[7],2);
  if(dp != 0)
   {
    sprintf(datnr_lst,"%02d.%.2d.%04d",dp,mp,gp);
    sprintf(datnr_dbf,"%04d%02d%02d",gp,mp,dp);
    sprintf(datnr_xml,"%02d%02d%04d",dp,mp,gp);
   }
  if(du != 0)
   {
    sprintf(datkr_lst,"%02d.%.2d.%04d",du,mu,gu);
    sprintf(datkr_dbf,"%04d%2d%02d",gu,mu,du);
    sprintf(datkr_xml,"%02d%2d%04d",du,mu,gu);
   }
 
  kolih_str_1df++;
  
  fprintf(ff_1df,"%-5d|%-10s|%10.2f|%10.2f|%10.2f|%10.2f|  %-4s|%-10s|%-10s|%-10s|%6s|\n",
  kolih_str_1df,
  row[0],
  nah_dohod,
  vip_dohod,
  nah_podoh,
  vip_podoh,  
  row[1],datnr_lst,datkr_lst,row[8]," ");

  fprintf(ff_1df_fam,"%-5d|%-10s|%10.2f|%10.2f|%10.2f|%10.2f|  %-4s|%-10s|%-10s|%-10s|%6s|%s %s\n",
  kolih_str_1df,
  row[0],
  nah_dohod,
  vip_dohod,
  nah_podoh,
  vip_podoh,  
  row[1],datnr_lst,datkr_lst,row[8]," ",row[10],row[9]);
  
  zapf8dr(kolih_str_1df,data->rk->kvart.ravno_atoi(),gn,kod_edrpou.ravno(),nast.TYP,row[0],nah_dohod,vip_dohod,nah_podoh,vip_podoh,row[1],datnr_dbf,datkr_dbf,atoi(row[8]),0,ff_dbf_tmp);

  f1df_xml_strw(&nomer_xml_str,row[0],nah_dohod,vip_dohod,nah_podoh,vip_podoh,row[1],datnr_xml,datkr_xml,row[8],"0",ff_xml);

 }
sprintf(strsql,"\
%*s:%10.2f\n\
%*s:%10.2f\n\
%*s:%10.2f\n\
%*s:%10.2f\n",
iceb_u_kolbait(30,gettext("Начисленный доход")),gettext("Начисленный доход"),itogo_nah_dohod,
iceb_u_kolbait(30,gettext("Выплаченный доход")),gettext("Выплаченный доход"),itogo_vip_dohod,
iceb_u_kolbait(30,gettext("Начисленный подоходный")),gettext("Начисленный подоходный"),itogo_nah_podoh,
iceb_u_kolbait(30,gettext("Выплаченный подоходный")),gettext("Выплаченный подоходный"),itogo_vip_podoh);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

int kol_fiz_lic=kolih_rab+kolih_sovm;

//Распечатка концовки файлов
koncf8dr(data->rk->kvart.ravno_atoi(),gn,kod_edrpou.ravno(),nast.TYP,
itogo_vip_dohod,itogo_nah_dohod,itogo_vip_podoh,itogo_nah_podoh,
itogo_vip_dohod,itogo_nah_dohod,itogo_vip_podoh,itogo_nah_podoh,
kol_fiz_lic,kol_list,&nomer_str,2,ff_1df,ff_1df_fam,ff_dbf_tmp,data->window);


f1df_xml_konw(itogo_nah_dohod,itogo_vip_dohod,itogo_nah_podoh,itogo_vip_podoh,nomer_xml_str,kol_fiz_lic,kol_list,naim_kontr.ravno(),telefon.ravno(),nast.TYP,ff_xml,data->window);
 
fputc(26,ff_dbf_tmp);
fclose(ff_dbf_tmp);



fclose(ff_prot);
fclose(ff_1df);
fclose(ff_1df_fam);

int nomer_rasp=1;
sprintf(imaf_dbf,"da0000%02d.%d",nomer_rasp,data->rk->kvart.ravno_atoi());
if(iceb_poldan("Имя DBF файла",strsql,"zarsdf1df.alx",data->window) == 0)
  sprintf(imaf_dbf,"%s%02d.%d",strsql,nomer_rasp,data->rk->kvart.ravno_atoi());

creatheaddbf(imaf_dbf,kolih_str_1df,data->window);

/*Сливаем два файла*/
iceb_cat(imaf_dbf,imaf_dbf_tmp,data->window);

unlink(imaf_dbf_tmp);



data->rk->imaf.plus(imaf_1df);
data->rk->naimf.plus(gettext("Расчет формы 1ДФ"));

data->rk->imaf.plus(imaf_1df_fam);

sprintf(strsql,"%s+%s",gettext("Расчет формы 1ДФ"),gettext("Фамилия"));

data->rk->naimf.plus(strsql);

data->rk->imaf.plus(imaf_prot);
data->rk->naimf.plus(gettext("Протокол расчёта"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),1,data->window);
 
data->rk->imaf.plus(imaf_dbf);
data->rk->naimf.plus(gettext("Документ в формате dbf"));

data->rk->imaf.plus(imaf_xml);
data->rk->naimf.plus(gettext("Документ в формате xml"));



/***********
data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Распечатка формы 1ДФ (для сдачи в налоговую)"));

data->rk->imaf.plus(imaf1);
data->rk->naimf.plus(gettext("Распечатка формы 1ДФ (с фамилиями)"));

data->rk->imaf.plus(imafpr);
data->rk->naimf.plus(gettext("Протокол хода расчёта"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),1,data->window);

data->rk->imaf.plus(imafdbf);
data->rk->naimf.plus(gettext("Форма 1ДФ в DBF формате"));

data->rk->imaf.plus(imaf_xml);
data->rk->naimf.plus(gettext("Форма 1ДФ в XML формате"));
***************/



data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
