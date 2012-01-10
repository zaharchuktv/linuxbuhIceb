/*$Id: zar_otrvr_r.c,v 1.12 2011-02-21 07:35:59 sasa Exp $*/
/*17.03.2010	29.11.2006	Белых А.И.	zar_otrvr_r.c
Расчёт свода начислений/удержаний по категориям
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "zar_otrvr.h"

class zar_otrvr_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zar_otrvr_rek *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_otrvr_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_otrvr_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_otrvr_r_data *data);
gint zar_otrvr_r1(class zar_otrvr_r_data *data);
void  zar_otrvr_r_v_knopka(GtkWidget *widget,class zar_otrvr_r_data *data);

void rasvtn(short dn,short mn,short gn,short dk,short mk,short gk,short kka,class iceb_u_double *nao,class iceb_u_int *tn,short mkt[],int metkar,FILE *kaw,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int zar_otrvr_r(class zar_otrvr_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_otrvr_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать свод отработанного времени по работникам"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_otrvr_r_key_press),&data);

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

repl.plus(gettext("Распечатать свод отработанного времени по работникам"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_otrvr_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_otrvr_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_otrvr_r_v_knopka(GtkWidget *widget,class zar_otrvr_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_otrvr_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_otrvr_r_data *data)
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

gint zar_otrvr_r1(class zar_otrvr_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);

short mn,gn;
short mk,gk;
iceb_u_rsdat1(&mn,&gn,data->rk->datan.ravno());
iceb_u_rsdat1(&mk,&gk,data->rk->datak.ravno());
if(mk == 0)
 {
  mk=mn;
  gk=gn;
 }




/*Создаем массив видов табеля*/
sprintf(strsql,"select kod from Tabel");
SQLCURSOR cur;
SQLCURSOR curr;
int kolvt=0;
if((kolvt=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolvt == 0)
 {
  iceb_menu_soob(gettext("Не введены виды табеля !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

short	mkt[kolvt]; /*Массив кодов табелей*/
SQL_str row;
int i=0;
while(cur.read_cursor(&row) != 0)
  mkt[i++]=atoi(row[0]);


/*Создаем файл табельных номеров для последующей сортировки*/
if(gn != gk)
  sprintf(strsql,"select tabn,mes,god from Ztab where \
(god > %d and god < %d) or \
(god = %d and mes >= %d) or \
(god = %d and mes <= %d)",
gn,gk,
gn,mn,
gk,mk);
else
  sprintf(strsql,"select tabn,mes,god from Ztab where \
god = %d and mes >= %d and mes <= %d",
gn,mn,mk);
/*
printw("%s\n",strsql);
refresh();
*/
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
FILE *ff; 
sprintf(imaf,"vtn%d.tmp",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
short m,g;
int tabn=0;
SQL_str row1;
while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
   continue;
  tabn=atoi(row[0]);
  if(data->rk->podr.ravno()[0] != '\0')
   {
    m=atoi(row[1]);
    g=atoi(row[2]);
    /*определяем подразделение*/
    sprintf(strsql,"select podr from Zarn where god=%d and mes=%d and \
tabn=%d",g,m,tabn);

    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
       continue;

    if(iceb_u_proverka(data->rk->podr.ravno(),row1[0],0,0) != 0)
      continue;
   }

  fprintf(ff,"%d\n",tabn);

 }
fclose(ff);
char bros[512];
sprintf(bros,"sort -o %s -n -u %s",imaf,imaf);
system(bros);

/*Определяем количество табельных номеров*/

if((ff = fopen(imaf,"r")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_int mtn;

while(fscanf(ff,"%d\n",&tabn) != EOF)
 {
  mtn.plus(tabn,-1);
 /* printw("%ld\n",mtn[i]);*/
 }
fclose(ff);
unlink(imaf);

int koltn=mtn.kolih();

sprintf(strsql,"%s %d\n",gettext("Количество табельных номеров"),koltn);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

if(koltn == 0)
 {
  return(FALSE);
 } 
/*Открывем массив для видов табелей*/
class iceb_u_double mtab;
mtab.make_class(koltn*kolvt*3);

/*Заряжаем массив табельных номеров*/


/*Заряжаем массив видов табелей*/
if(gn != gk)
  sprintf(strsql,"select tabn,kodt,dnei,has,kdnei from Ztab where \
(god > %d and god < %d) or \
(god = %d and mes >= %d) or \
(god = %d and mes <= %d)",
  gn,gk,
  gn,mn,
  gk,mk);
else
  sprintf(strsql,"select tabn,kodt,dnei,has,kdnei from Ztab where \
god = %d and mes >= %d and mes <= %d",
  gn,mn,mk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int vt;
float dnei;
float has;
float kdnei;
int i1=0;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
   continue;
  tabn=atoi(row[0]);
  vt=atoi(row[1]);
  dnei=atof(row[2]);
  has=atof(row[3]);
  kdnei=atof(row[4]);
  
  if((i=mtn.find(tabn)) < 0)
   {

    sprintf(strsql,gettext("Не найден табельный номер %d в массиве табельных номеров !"),tabn);
    iceb_menu_soob(strsql,data->window);
    continue;
   }         
  for(i1=0; i1 < kolvt; i1++)
    if(vt == mkt[i1])
      break;

  if(i1 == kolvt)
   {
    sprintf(strsql,gettext("Не найден код табеля %d в массиве видов табелей !"),vt);
    iceb_menu_soob(strsql,data->window);
    continue;
   }         
  mtab.plus(dnei,i*kolvt*3+i1*3);
  mtab.plus(has,i*kolvt*3+i1*3+1);
  mtab.plus(kdnei,i*kolvt*3+i1*3+2);
 }

sprintf(imaf,"vtn%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

short dk=0;
iceb_u_dpm(&dk,&mk,&gk,5);
iceb_u_zagolov(gettext("Свод отработанного времени по работникам"),1,mn,gn,dk,mk,gk,organ,ff);

if(data->rk->podr.ravno()[0] != '\0')
  fprintf(ff,"%s : %s\n",gettext("Подразделение"),data->rk->podr.ravno());
if(data->rk->tabnom.ravno()[0] != '\0')
  fprintf(ff,"%s : %s\n",gettext("Табельный номер"),data->rk->tabnom.ravno());
  
rasvtn(1,mn,gn,dk,mk,gk,kolvt,&mtab,&mtn,mkt,0,ff,data->view,data->buffer,data->window);

fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
iceb_podpis(ff,data->window);
fclose(ff);

char imafn[50];
//Распечатываем с начислениями
sprintf(imafn,"vtnn%d.lst",getpid());
if((ff = fopen(imafn,"w")) == NULL)
 {
  iceb_er_op_fil(imafn,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_zagolov(gettext("Свод отработанного времени по работникам"),1,mn,gn,dk,mk,gk,organ,ff);

if(data->rk->podr.ravno()[0] != '\0')
  fprintf(ff,"%s : %s\n",gettext("Подразделение"),data->rk->podr.ravno());
if(data->rk->tabnom.ravno()[0] != '\0')
  fprintf(ff,"%s : %s\n",gettext("Табельный номер"),data->rk->tabnom.ravno());
  
rasvtn(1,mn,gn,dk,mk,gk,kolvt,&mtab,&mtn,mkt,1,ff,data->view,data->buffer,data->window);


iceb_podpis(ff,data->window);
fclose(ff);
iceb_ustpeh(imaf,0,data->window);
iceb_ustpeh(imafn,0,data->window);

data->rk->imaf.plus(imaf);
data->rk->imaf.plus(imafn);
data->rk->naimf.plus(gettext("Свод отработанного времени по работникам"));
data->rk->naimf.plus(gettext("Свод отработанного времени по работникам (с начислениями)"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),0,data->window);


data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
/***************/
/*Шапка таблицы*/
/***************/
void            saptbn(short kka, //Количество видов табелей
char st[],
long pn, //Позиция начала
long pk, //Позиция конца
long konu, //Количество людей
class iceb_u_double *nao,
short ka[], //Массив видов табелей
int metkar, //0-без начислений 1-с начислениями
FILE *kaw,
GtkWidget *wpredok)
{
int             i,i1,i2;
char            nai[512];
double          br2;
SQL_str         row1;
char		strsql[512];
SQLCURSOR curr;
//printw("Печатьуем шапку\n");

/*printw("kka=%d pn=%d pk=%d konu=%ld\n",kka,pn,pk,konu);*/

/*Формируем строку подчеркивания*/
/*Полка над наименованием*/
if(metkar == 0)
  strcpy(st,"------------------------------------------");
if(metkar == 1)
  strcpy(st,"---------------------------------------------------------------------------------------------------");
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i*3; i1< kka*3*konu ; i1+=kka*3)
    if(fabs((br2=nao->ravno(i1)+nao->ravno(i1+1)+nao->ravno(i1+2))) > 0.01)
     break;

  if(fabs(br2) > 0.01 )
    strcat(st,"-------------------");
  else
   i2++;
 }

/*Полка над ИТОГО*/
if(i == kka)
  strcat(st,"-------------------");

fprintf(kaw,"\n%s\n",st);
if(metkar == 0)
 fprintf(kaw,gettext("Н/п | Т/н | Фамилия Имя Отчество         "));
if(metkar == 1)
 fprintf(kaw,gettext("Н/п | Т/н | Фамилия Имя Отчество         |    Должность       |К.п|Наим. подразделения |  Сумма   "));

i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i*3; i1 < kka*3*konu ; i1+=kka*3)
    if(fabs((br2=nao->ravno(i1)+nao->ravno(i1+1)+nao->ravno(i1+2))) > 0.01)
     break;
  if(fabs(br2) < 0.01)
   {
    i2++;
    continue;
   }
  sprintf(strsql,"select naik from Tabel where kod=%d",ka[i]);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    sprintf(strsql,"%s %d !\n",gettext("Не найден код табеля"),ka[i-1]);
    iceb_menu_soob(strsql,wpredok);
   }
  strncpy(nai,row1[0],sizeof(nai)-1);
  fprintf(kaw,"| %-*.*s",iceb_u_kolbait(17,nai),iceb_u_kolbait(17,nai),nai);
 }

if(i == kka)
  fprintf(kaw,"| %-*.*s|\n",iceb_u_kolbait(17,gettext("Итого")),iceb_u_kolbait(17,gettext("Итого")),gettext("Итого"));
else
  fprintf(kaw,"|\n");

if(metkar == 0)
  fprintf(kaw,"    |     |%-30s"," ");
if(metkar == 1)
  fprintf(kaw,"    |     |%-30s|%20s|%3s|%20s|%10s"," "," "," "," "," ");

i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i*3; i1< kka*3*konu ; i1+=kka*3)
    if(fabs((br2=nao->ravno(i1)+nao->ravno(i1+1)+nao->ravno(i1+2))) > 0.01)
     break;
  if(fabs(br2) > 0.01)
   {
    fprintf(kaw,"| %-17d",ka[i]);
   }
  else
    i2++;
 }

if(i == kka)
  fprintf(kaw,"| %-17.17s|\n"," ");
else
  fprintf(kaw,"|\n");

fprintf(kaw,"%s\n",st);

}

/*********************************/
/*Расапечатка результатов расчета*/
/*********************************/

void rasvtn(short dn,short mn,short gn,
short dk,short mk,short gk,
short kka, //Количество видов табелей
class iceb_u_double *nao,
class iceb_u_int *tn, //Массив табельных номеров
short mkt[],
int metkar, //0-без начислений 1-с начислениями
FILE *kaw,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok)
{
int  koltb=tn->kolih(); //Количество табельных номеров
long		i,i1,i2,ii,skn;
short           kkol=12; /*Количество колонок в распечатке*/
double		br1,br2;
short           korr;
double          sn1,sn2,sn3;
char		st[1024];
unsigned short	porn; /*Порядковый номер*/
double		it1,it2,it3;
char		fio[512];
SQL_str         row1;
char		strsql[512];
char            dolg[512];
double		suman=0.;
char		kodpodr[32];
char		naimpodr[512];
SQLCURSOR curr;
if(metkar == 1)
 kkol=9999;

/*Распечатываем*/
sprintf(strsql,"%s %d\n-------------------------\n",
gettext("Распечатываем"),metkar);

iceb_printw(iceb_u_toutf(strsql),buffer,view);

porn=korr=0;
sn1=sn2=sn3=0.;
for(ii=0 ; ii < kka; ii += kkol)
 {
  /*printw("ii-%d kka-%d korr-%d\n",ii,kka,korr);*/
  /*Проверяем если все равно нулю то печатаем следующюю группу столбиков*/
  i2=0; br2=0.;
  for(i=ii*3; i<ii*3+kkol+i2 && i < kka*3;i+=3)
   {
    br1=0.;
    for(skn=i;skn< kka*3*koltb+ii*3+kkol && skn<kka*3*koltb; skn+=kka*3)
      if(fabs((br1=nao->ravno(skn)+nao->ravno(skn+1)+nao->ravno(skn+2))) > 0.009)
        break;
    if(fabs(br1) < 0.01)
     i2++;
    br2+=br1;
   }
  if(fabs(br2) < 0.01)
    continue;
  
  memset(st,'\0',sizeof(st));
  saptbn(kka,st,ii,ii+kkol,koltb,nao,mkt,metkar,kaw,wpredok);
  br1=0.;
  for(i=0 ; i<koltb;i++)
   {
    /*Проверяем если нет равных нолю позиций печатаем*/
    br1=0.;
    for(skn=0;skn< kka*3 ; skn++)
      if(fabs(nao->ravno(skn+i*kka*3)) > 0.009)
        break;

    if(fabs(nao->ravno(skn+i*kka*3)) < 0.01)
      continue;

    memset(naimpodr,'\0',sizeof(naimpodr));
    memset(kodpodr,'\0',sizeof(kodpodr));
    
    sprintf(strsql,"select fio,dolg,podr from Kartb where tabn=%d",tn->ravno(i));
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tn->ravno(i));
      iceb_menu_soob(strsql,wpredok);
      continue;
     }
       
    strncpy(fio,row1[0],sizeof(fio)-1);
    strncpy(dolg,row1[1],sizeof(dolg)-1);
    strncpy(kodpodr,row1[2],sizeof(kodpodr)-1);

    if(metkar == 1) //Читаем должность и сумму начислений
     {
      sprintf(strsql,"select dolg,podr from Zarn where tabn=%d and god=%d and mes=%d",
      tn->ravno(i),gn,mn);
      if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
       {
        memset(kodpodr,'\0',sizeof(kodpodr));
        memset(dolg,'\0',sizeof(dolg));
        strncpy(dolg,row1[0],sizeof(dolg)-1);
        strncpy(kodpodr,row1[1],sizeof(kodpodr)-1);
       }
      sprintf(strsql,"select naik from Podr where kod=%s",kodpodr);
      if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
        strncpy(naimpodr,row1[0],sizeof(naimpodr)-1);
      
      sprintf(strsql,"%d",tn->ravno(i));
      suman=sosnsumw(strsql,dn,mn,gn,dk,mk,gk,wpredok);
     }

    porn++;
    if(metkar == 0)
     fprintf(kaw,"%4d %5d %-*.*s",porn,tn->ravno(i),iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio);
    if(metkar == 1)
     fprintf(kaw,"%4d %5d %-*.*s %-*.*s %-3s %-*.*s %10.2f",
     porn,tn->ravno(i),
     iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,
     iceb_u_kolbait(20,dolg),iceb_u_kolbait(20,dolg),dolg,
     kodpodr,
     iceb_u_kolbait(20,naimpodr),iceb_u_kolbait(20,naimpodr),naimpodr,
     suman);

    /*Предыдущая информация*/
    it1=it2=it3=0.;
    if(ii > 0 )
     {
      for(skn=i*kka*3; skn<i*kka*3+ii*3 && skn < kka*3*koltb ;skn+=3)
       {
	/*printw(" skn=%d",skn);*/
	 it1+=nao->ravno(skn);
	 it2+=nao->ravno(skn+1);
	 it3+=nao->ravno(skn+1);
       }
     }

    i2=0;
    for(skn=i*kka*3+ii*3; skn<i*kka*3+ii*3+kkol*3+i2*3 && skn < i*kka*3+kka*3 ;skn+=3)
     {
      /*Если колонка нулевая не печатать*/
      br2=0.;
      for(i1=skn-(i*kka*3); i1< kka*3*koltb ; i1+=kka*3)
	if(fabs((br2+=nao->ravno(i1)+nao->ravno(i1+1)+nao->ravno(i1+2))) > 0.01)
	  break;

      if(fabs(br2) > 0.01)
       {
	fprintf(kaw," %5.5g %6.6g %5.5g",nao->ravno(skn),nao->ravno(skn+1),nao->ravno(skn+2));
        it1+=nao->ravno(skn);
        it2+=nao->ravno(skn+1);
        it3+=nao->ravno(skn+2);
       }
      else
       {
	i2++;
       }

     }
/*    printw("skn=%d i=%d kka=%d  %d koltb=%ld\n",skn,i,kka,i*kka*3+kka*3-1-8,koltb);*/
    if(skn == (i+1)*kka*3)
     {
      fprintf(kaw," %5.5g %6.6g %5.5g\n",it1,it2,it3);
     }
    else
      fprintf(kaw,"\n");
   }

  fprintf(kaw,"%s\n",st);
  if(metkar == 0)
    fprintf(kaw,"%-*s",iceb_u_kolbait(41,gettext("Итого")),gettext("Итого"));
  if(metkar == 1)
    fprintf(kaw,"%-*s",iceb_u_kolbait(98,gettext("Итого")),gettext("Итого"));
  porn=0;
  i2=0;
  /*printw("kkol=%d ii=%d kka=%d koltb=%d\n",kkol,ii,kka,koltb);*/
  for(i=ii; i<ii+kkol+i2 && i < kka ;i++)
   {
    it1=it2=it3=0.;
/*    for(skn=i*3;skn < kka*3*(koltb-1)+ii*3+kkol+i2 && skn< kka*3*koltb ; skn+=kka*3)*/
    for(skn=i*3; skn < kka*3*koltb ; skn+=kka*3)
     {
      it1+=nao->ravno(skn);
      it2+=nao->ravno(skn+1);
      it3+=nao->ravno(skn+2);
     }

    if(fabs(it1+it2+it3) > 0.01)
      fprintf(kaw," %5.5g %6.6g %5.5g",it1,it2,it3);
    else
      i2++;

    sn1+=it1;
    sn2+=it2;
    sn3+=it3;
   }
  if(i == kka)
   {
    fprintf(kaw," %5.5g %6.6g %5.5g\n",sn1,sn2,sn3);
   }
  else
   {
    fprintf(kaw,"\n");
   }
  fprintf(kaw,"%s\n",st);

  if(i == kka)
   break;
  ii+=i2;
 }
}
