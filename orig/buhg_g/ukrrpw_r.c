/*$Id: ukrrpw_r.c,v 1.10 2011-02-21 07:35:58 sasa Exp $*/
/*17.11.2009	01.01.2008	Белых А.И.	ukrrpw_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "buhg_g.h"
#include <unistd.h>
#include "ukrrpw.h"

class ukrrpw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class ukrrpw_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  
  ukrrpw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   ukrrpw_r_key_press(GtkWidget *widget,GdkEventKey *event,class ukrrpw_r_data *data);
gint ukrrpw_r1(class ukrrpw_r_data *data);
void  ukrrpw_r_v_knopka(GtkWidget *widget,class ukrrpw_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;

int ukrrpw_r(class ukrrpw_data *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class ukrrpw_r_data data;
data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Реестр проводок по видам операций"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(ukrrpw_r_key_press),&data);

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

repl.plus(gettext("Реестр проводок по видам операций"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(ukrrpw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)ukrrpw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  ukrrpw_r_v_knopka(GtkWidget *widget,class ukrrpw_r_data *data)
{
if(data->kon_ras == 1) return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   ukrrpw_r_key_press(GtkWidget *widget,GdkEventKey *event,class ukrrpw_r_data *data)
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
/****************************************/
/*  Шапка документа                     */
/****************************************/
void	saprppvo(
short dn,short mn,short gn,
short dk,short mk,short gk,
int *nlist,int *kolst,
FILE *ff)
{
fprintf(ff,"\
%s %02d.%02d.%4d%s %s %02d.%02d.%4d%s %30s%sN%d\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
"",gettext("Лист"),
*nlist);

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
	Контрагент                              |Дата док. |Дата пров.|N документа|Nнал.н|  Счета   |       Сумма        |Коментарий\n"));

/*
123456789012345678901234567890123456789012345678 1234567890 1234567890 12345678901 123456 1234567890 12345678901234567890
*/

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------\n");
*nlist+=1;
*kolst+=4;
}

/***************************************/
/*Счетчик строк                        */
/***************************************/
void	shetrppvo(short dn,short mn,short gn,
short dk,short mk,short gk,
int *nlist,int *kolst,FILE *ff)
{
*kolst+=1;
if(*kolst < kol_strok_na_liste)
  return; 

fprintf(ff,"\f");
*kolst=0;
saprppvo(dn,mn,gn,dk,mk,gk,nlist,kolst,ff);

}

/**************************************/
/*Распечатка итога                    */
/**************************************/

void	itrppvo(int metka, //0-по контрагенту 1-по виду командировки 2-общий итог
const char	*kod,
short dn,short mn,short gn,
short dk,short mk,short gk,
int *nlist,int *kolst,
class iceb_u_spisok *PARSH,class iceb_u_double *kontr,FILE *ff)
{
SQLCURSOR curr;
double itog=0.;
int	kolih=PARSH->kolih();
char	naim[512];
char	strsql[512];
SQL_str row;
double	suma=0;
char    naimitog[50];

memset(naim,'\0',sizeof(naim));

if(kolih > 0)
 {
  memset(naimitog,'\0',sizeof(naimitog));
  if(metka == 0)
   {
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kod);
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      strncpy(naim,row[0],sizeof(naim)-1);    
    strcpy(naimitog,gettext("Итого по контрагенту"));
   }     
  if(metka == 1)
   {
    sprintf(strsql,"select naik from Ukrvkr where kod='%s'",kod);
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      strncpy(naim,row[0],sizeof(naim)-1);    
    strcpy(naimitog,gettext("Итого по виду командировки"));
   }
  if(metka == 2)
    strcpy(naimitog,gettext("Общий итог"));

  shetrppvo(dn,mn,gn,dk,mk,gk,nlist,kolst,ff);
  fprintf(ff,"%s %s:\n",naimitog,naim);
 }
/*printw("kso- %d\n",kso);*/

for(int i=0; i< kolih ;i++)
 {
  suma=kontr->ravno(i);
  if(suma == 0.)
   continue;

  shetrppvo(dn,mn,gn,dk,mk,gk,nlist,kolst,ff);
  fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(20,PARSH->ravno(i)),PARSH->ravno(i),
  iceb_u_prnbr(suma));
  itog+=suma;
 }
if(itog != 0.)
 {
  shetrppvo(dn,mn,gn,dk,mk,gk,nlist,kolst,ff);
  fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(itog));

  shetrppvo(dn,mn,gn,dk,mk,gk,nlist,kolst,ff);
  fprintf(ff,"\
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
 }
kontr->clear_class();
}
/****************************************/
/*Итог по документа                     */
/****************************************/
void	itdok(double itogd,FILE *ff)
{
fprintf(ff,"%*s: %20s\n",iceb_u_kolbait(100,gettext("Итого по документу")),
gettext("Итого по документу"),iceb_u_prnbr(itogd));
}

/******************************************/
/******************************************/

gint ukrrpw_r1(class ukrrpw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
class SQLCURSOR cur;


short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


sprintf(strsql,"%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


int kolstr=0;

sprintf(strsql,"select datp,sh,shk,nomd,pod,deb,datd,komen from Prov \
where val=0 and \
datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' and kto='%s' and deb != 0.",
gn,mn,dn,gk,mk,dk,gettext("УКР"));
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }    
char imaftmp[50];
FILE *fftmp;
sprintf(imaftmp,"vidop%d.tmp",getpid());
if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int kolstr2=0;
float kolstr1=0.;
SQL_str row1;
class SQLCURSOR curr;
char		vkom[20];
char		kontr[20];
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  /*Счет*/
  if(iceb_u_proverka(data->rk->shetu.ravno(),row[1],0,0) != 0)
    continue;
   
  /*Читаем документ*/
  sprintf(strsql,"select kont,vkom from Ukrdok where datd='%s' and \
nomd='%s'",row[6],row[3]);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
   {
    sprintf(strsql,"Не найден документ N %s Дата=%s\n",
    row[3],row[6]);
    
    repl.new_plus(strsql);
    
    sprintf(strsql,"%s %s %s %s",gettext("Проводка"),
    row[1],row[2],row[3]);
    repl.plus(strsql);
    
    iceb_menu_soob(&repl,data->window);
    continue;
   }
  memset(kontr,'\0',sizeof(kontr));
  strncpy(kontr,row1[0],sizeof(kontr)-1);
  memset(vkom,'\0',sizeof(vkom));
  strncpy(vkom,row1[1],sizeof(vkom)-1);

  if(iceb_u_proverka(data->rk->kontr.ravno(),kontr,0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->vidkom.ravno(),vkom,0,0) != 0)
    continue;
  kolstr2++;
  fprintf(fftmp,"%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
  vkom,kontr,row[3],row[0],row[1],row[2],row[6],row[5],row[7]);  

 }
fclose(fftmp);

//sprintf(strsql,"sort -o %s -t\\| +0 -1 +1 -2 +2n -3 +3 %s",imaftmp,imaftmp);
sprintf(strsql,"sort -o %s -t\\| -k1,2 -k2,3 -k3,4n %s",imaftmp,imaftmp);
system(strsql);

short	dd,md,gd; //Дата документа
short	dp,mp,gp; //Дата проводки
char	nomdok[32];
char	shet[32],shetk[32];
double	deb=0.;
char	koment[512];
int 	nlist=1;
int	kolst=0;
int	kom=0;

if((fftmp = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf[56];
FILE *ff;
sprintf(imaf,"rpkr%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

//bf=localtime(&tmmn);

if(data->rk->kontr.ravno()[0] != '\0')
 { 
  fprintf(ff,"%s: %s\n",gettext("Контрагент"),data->rk->kontr.ravno());
  kolst++;
 }

if(data->rk->vidkom.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Вид командировки"),data->rk->vidkom.ravno());
  kolst++;
 }

if(data->rk->shetu.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Счет"),data->rk->shetu.ravno());
  kolst++;
 }


class iceb_u_spisok PARSH; //Список пар счетов
class iceb_u_double oper; //Суммы по операциям
class iceb_u_double kont; //Суммы по конрагентам
class iceb_u_double itog; //Суммы по всему расчету

char  vkom1[20];
char  kontr1[20];
char  shett[40];
char  naikont[80];
char  nomdok1[20];
char  naivkom[80];
double itogd=0.;
vkom1[0]='\0';
kontr1[0]='\0';
nomdok1[0]='\0';

kolstr1=0.;
char stroka[1024];
while(fgets(stroka,sizeof(stroka),fftmp) != NULL)
 {
  
  iceb_pbar(data->bar,kolstr2,++kolstr1);    

  iceb_u_polen(stroka,vkom,sizeof(vkom),1,'|');
  iceb_u_polen(stroka,kontr,sizeof(kontr),2,'|');
  iceb_u_polen(stroka,nomdok,sizeof(nomdok),3,'|');
  iceb_u_polen(stroka,strsql,sizeof(strsql),4,'|');
  iceb_u_rsdat(&dp,&mp,&gp,strsql,2);
  iceb_u_polen(stroka,shet,sizeof(shet),5,'|');
  iceb_u_polen(stroka,shetk,sizeof(shetk),6,'|');
  iceb_u_polen(stroka,strsql,sizeof(strsql),7,'|');
  iceb_u_rsdat(&dd,&md,&gd,strsql,2);
  iceb_u_polen(stroka,strsql,sizeof(strsql),8,'|');
  deb=atof(strsql);    
  iceb_u_polen(stroka,koment,sizeof(koment),9,'|');

  if(iceb_u_SRAV(nomdok,nomdok1,0) != 0)
   {
    if(nomdok1[0] != '\0')
     {
      shetrppvo(dn,mn,gn,dk,mk,gk,&nlist,&kolst,ff);
      itdok(itogd,ff);
     }
    strcpy(nomdok1,nomdok);
    itogd=0.;
   }

  if(iceb_u_SRAV(kontr,kontr1,0) != 0)
   {
    if(kontr1[0] != '\0')
      itrppvo(0,kontr1,dn,mn,gn,dk,mk,gk,&nlist,&kolst,&PARSH,&kont,ff);

    memset(naikont,'\0',sizeof(naikont));
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr);
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      strncpy(naikont,row[0],sizeof(naikont)-1);    

    kom=1;
    strcpy(kontr1,kontr);
   }

  if(iceb_u_SRAV(vkom,vkom1,0) != 0)
   {
    if(vkom1[0] != '\0')
     {
      itrppvo(1,vkom1,dn,mn,gn,dk,mk,gk,&nlist,&kolst,&PARSH,&oper,ff);
      fprintf(ff,"\f");
      kolst=0;
     }    

    iceb_u_zagolov(gettext("Реестр проводок по виду командировок"),dn,mn,gn,dk,mk,gk,organ,ff);
    kolst+=5;

    memset(naivkom,'\0',sizeof(naivkom));
    sprintf(strsql,"select naik from Ukrvkr where kod='%s'",vkom);
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     strncpy(naivkom,row[0],sizeof(naivkom)-1);   

    fprintf(ff,"%s:%s %s\n",gettext("Вид командировки"),
    vkom,naivkom);
    kolst++;

    saprppvo(dn,mn,gn,dk,mk,gk,&nlist,&kolst,ff);
    kom=1;
    strcpy(vkom1,vkom);
   }

  sprintf(shett,"%s,%s",shet,shetk);
  int i=0;
  if((i=PARSH.find(shett)) == -1)
    PARSH.plus(shett);

  oper.plus(deb,i);
  kont.plus(deb,i);
  itog.plus(deb,i);
  itogd+=deb;

  sprintf(strsql,"%02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %15.2f\n",
  dp,mp,gp,dd,md,gd,
  iceb_u_kolbait(11,nomdok),nomdok,
  iceb_u_kolbait(11,shett),shett,
  deb);

  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  shetrppvo(dn,mn,gn,dk,mk,gk,&nlist,&kolst,ff);

  if(kom == 1)
     fprintf(ff,"\
%-*s %-*.*s %02d.%02d.%4d %02d.%02d.%4d %-*s %-6s %-*s %20s %s\n",
     iceb_u_kolbait(7,kontr),kontr,
     iceb_u_kolbait(40,naikont),iceb_u_kolbait(40,naikont),naikont,
     dd,md,gd,dp,mp,gp,
     iceb_u_kolbait(11,nomdok),nomdok,
     "",
     iceb_u_kolbait(10,shett),shett,
     iceb_u_prnbr(deb),koment);

  if(kom == 0)
    fprintf(ff,"\
%48s %02d.%02d.%4d %02d.%02d.%4d %-*s %-6s %-*s %20s %s\n",
    " ",dd,md,gd,dp,mp,gp,
    iceb_u_kolbait(11,nomdok),nomdok,
    "",
    iceb_u_kolbait(10,shett),shett,
    iceb_u_prnbr(deb),koment);

  kom=0;
   
 }

itdok(itogd,ff);
itrppvo(0,kontr1,dn,mn,gn,dk,mk,gk,&nlist,&kolst,&PARSH,&kont,ff);
itrppvo(1,vkom1,dn,mn,gn,dk,mk,gk,&nlist,&kolst,&PARSH,&oper,ff);
itrppvo(2,"",dn,mn,gn,dk,mk,gk,&nlist,&kolst,&PARSH,&itog,ff);

iceb_podpis(ff,data->window);

fclose(ff);
fclose(fftmp);
unlink(imaftmp);

data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Реестр проводок по видам командировок"));
iceb_ustpeh(imaf,1,data->window);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
