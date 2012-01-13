/*$Id: iceb_spks_r.c,v 1.18 2011-02-21 07:36:08 sasa Exp $*/
/*05.09.2007	18.12.2003	Белых А.И.	iceb_spks_r.c
Расчет сальдо по контрагенту
*/
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include "iceb_libbuh.h"
#include "iceb_spks.h"

gint     iceb_spks_r1(class iceb_spks_data *data);
gboolean iceb_spks_r_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_spks_data *data);
void     salorksl(double, double,double,double,double,double,FILE *,FILE *);


extern SQL_baza	bd;
extern char *name_system;
extern char *version;
extern short startgodb;
extern char *organ;
/*
FILE   *prot;
char   *imafprot={"prot.lst"};
*/
int  iceb_spks_r(class iceb_spks_data *data,iceb_u_str *zag)
{
char strsql[512];
/*
  if((prot = fopen(imafprot,"w")) == NULL)
    iceb_er_op_fil(imafprot,"",errno);
*/
printf("iceb_spks_r\n");

data->window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data->window),ICEB_POS_CENTER);
gtk_widget_set_usize(GTK_WIDGET(data->window), 500,400);

sprintf(strsql,"%s %s",name_system,gettext("Расчет сальдо по контрагенту."));
gtk_window_set_title(GTK_WINDOW(data->window),strsql);

gtk_signal_connect(GTK_OBJECT(data->window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data->window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data->window),"key_press_event",GTK_SIGNAL_FUNC(iceb_spks_r_key_press),data);

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data->window), vbox);

data->label=gtk_label_new(gettext("Ждите !!!"));
gtk_box_pack_start(GTK_BOX(vbox),data->label,FALSE,FALSE,0);

GtkWidget *label=gtk_label_new(iceb_u_toutf(zag->ravno()));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data->view=gtk_text_view_new();

//Установка перноса строк которые не влазят по длинне
gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(data->view),GTK_WRAP_CHAR);

//Установка запрета на редактируемость текста
gtk_text_view_set_editable(GTK_TEXT_VIEW(data->view),FALSE);

data->buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data->view);

sprintf(strsql,"F10 %s",gettext("Выход"));
data->knopka=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox),data->knopka,FALSE,FALSE, 0);
GtkTooltips *tooltips=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips,data->knopka,gettext("Завершение работы в этом окне."),NULL);
gtk_signal_connect_object(GTK_OBJECT(data->knopka), "clicked",GTK_SIGNAL_FUNC(gtk_widget_destroy),data->window);
//gtk_object_set_user_data(GTK_OBJECT(data->knopka),(gpointer)FK10);

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),FALSE);//не доступна

gtk_widget_show_all(data->window);

gtk_idle_add((GtkFunction)iceb_spks_r1,data);

gtk_main();

//fclose(prot);

return(data->metka_vozr);

}
/**********************************/
/*Расчет                          */
/**********************************/

gint	iceb_spks_r1(class iceb_spks_data *data)
{
//SQL_str row;
char    strsql[512];
iceb_u_str stroka;


data->sproh++;

printf("iceb_spks_r1 data->sproh=%d\n",data->sproh);

short dn,mn,gn;
short dk,mk,gk;

iceb_rsdatp(&dn,&mn,&gn,data->datn,&dk,&mk,&gk,data->datk,data->window);

short godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

if(data->sproh == 1)
 {
  time(&data->vrem_nr);
  char    strsql[512];
    
  sprintf(strsql,"slo%d.lst",getpid());
  data->imaf.plus(strsql);
  data->naim.plus(gettext("Сальдо по контрагенту"));
  if((data->ff1 = fopen(strsql,"w")) == NULL)
   { 
    iceb_er_op_fil(strsql,"",errno,data->window);
    data->metka_vozr=1;
    return(FALSE);
   }

  iceb_u_startfil(data->ff1);

  sprintf(strsql,"act%d.lst",getpid());
  data->imaf.plus(strsql);
  data->naim.plus(gettext("Акт сверки"));
  if((data->ffact = fopen(strsql,"w")) == NULL)
   { 
    iceb_er_op_fil(strsql,"",errno,data->window);
    data->metka_vozr=1;
    return(FALSE);
   }
  iceb_u_startfil(data->ffact);
  return(TRUE);
 }
   
if(data->sproh == 2)
 {
  SQLCURSOR cur;
  SQL_str row;
  char    strsql[512];
  sprintf(strsql,"select deb,kre from Saldo where kkk='1' and gs=%d and \
  ns='%s' and kodkon='%s'",godn,data->shet,data->kontr.ravno_filtr());
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    data->ddn=atof(row[0]);
    data->kkn=atof(row[1]);
   }
  time_t tmm;
  tm *bf;
  
  time(&tmm);
  bf=localtime(&tmm);
  fprintf(data->ff1,"\x0F");
  fprintf(data->ff1,"%s\n\
%s %s %s %s %s\n\
     %s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d  %s - %d:%d\n",
organ,
gettext("Распечатка проводок по счету"),
data->shet,
gettext("для контрагента"),
data->kontr.ravno(),data->naikontr.ravno(),
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),
bf->tm_hour,bf->tm_min);

  fprintf(data->ff1,"\
--------------------------------------------------------------------------------------------------\n");

fprintf(data->ff1,gettext("  Дата    |Сче.к.|Записал|Номер доку.|    Дебет   |   Кредит   |      К о м е н т а р и й       |\n"));
/*********
                                                                                                      12345678901234567890123456789012
**********/
  fprintf(data->ff1,"\
--------------------------------------------------------------------------------------------------\n");


  //Заголовок акта сверки
  iceb_zagacts(dn,mn,gn,dk,mk,gk,data->kontr.ravno(),data->naikontr.ravno(),data->ffact);
  iceb_zagsh(data->shetsrs.ravno(),data->ffact,data->window);

  return(TRUE);
 }

if(data->sproh == 3)
 {
  SQLCURSOR cur;
  SQL_str row;
  char    strsql[512];
  short           mvs=0; /*Метка выдачи сальдо*/
  
  short d,m,g;
  g=godn;
  d=1;
  m=1;
  int kolstr=0;
  short mes=0;
  double deb=0.,kre=0.;
  iceb_u_str  koment;
           
  while(iceb_u_sravmydat(d,m,g,dk,mk,gk) <= 0)
   {
/*
    sprintf(strsql,"%d.%d.%d %d.%d.%d\n",d,m,g,dk,mk,gk);
    stroka.new_plus(strsql);
    iceb_printw(stroka.ravno_toutf(),data->buffer,data->view);
*/        
    if(iceb_u_sravmydat(d,m,g,dn,mn,gn) == 0 && mvs == 0)
     {
      
      if(data->ddn > data->kkn)    
       {
        data->ddn=data->ddn-data->kkn;
        data->kkn=0.;
        fprintf(data->ff1,"%*s:%12s\n",
        iceb_u_kolbait(37,gettext("Сальдо начальное")),gettext("Сальдо начальное"),iceb_u_prnbr(data->ddn));

        sprintf(strsql,"%*s:%12s\n",
        iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),iceb_u_prnbr(data->ddn));
        stroka.new_plus(strsql);

        fprintf(data->ffact,"%02d.%02d.%4d %-*.*s %8.2f %8s| |\n",
        d,m,g,
        iceb_u_kolbait(20,gettext("Сальдо начальное")),iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
        data->ddn," ");

        fprintf(data->ffact,"\
-------------------------------------------------- ---------------------------------------------------\n");

       }
      else
       {
        data->kkn=data->kkn-data->ddn;
        data->ddn=0.;
        fprintf(data->ff1," %*s:%12s %12s\n",
        iceb_u_kolbait(37,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",iceb_u_prnbr(data->kkn));

        sprintf(strsql," %*s:%12s %12s\n",
        iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",iceb_u_prnbr(data->kkn));
        stroka.new_plus(strsql);

        fprintf(data->ffact,"%02d.%02d.%4d %-*.*s %8s %8.2f| |\n",
        d,m,g,
        iceb_u_kolbait(20,gettext("Сальдо начальное")),iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
        " ",data->kkn);

        fprintf(data->ffact,"\
-------------------------------------------------- ---------------------------------------------------\n");
       }     

      iceb_printw(stroka.ravno_toutf(),data->buffer,data->view);

      mvs=1;
     }

    sprintf(strsql,"select * from Prov \
where val=0 and datp='%d-%02d-%02d' and sh='%s' and kodkon='%s'",
    g,m,d,data->shet,data->kontr.ravno_filtr());

    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,"",strsql,NULL);
      data->metka_vozr=1;
      return(FALSE);
     }
    //printw("rozden-kolstr=%d\nsrtsql=%s\n",kolstr,strsql);
    //getch();
    if(kolstr == 0)
     {
      //Увеличиваем дату на день
      iceb_u_dpm(&d,&m,&g,1);
      continue;
     }
    short shett=0;
    while(cur.read_cursor(&row) != 0)
     {
      printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
      if(data->metka_prov == 1)
       {
        if(iceb_u_SRAV(row[2],data->shetsrs.ravno(),1) == 0 && iceb_u_SRAV(row[3],data->shets,1) == 0)
        //Увеличиваем дату на день
        iceb_u_dpm(&d,&m,&g,1);
 	continue;
       }

      deb=atof(row[9]);
      kre=atof(row[10]);

      /*printw("%d.%d.%d %.2f %.2f\n",bh2n.god,bh2n.mes,bh2n.den,ddn,kkn);*/
      if(iceb_u_sravmydat(d,m,g,dn,mn,gn) >= 0)
      if(iceb_u_sravmydat(dk,mk,gk,d,m,g) >= 0)
       {
        printf("Период\n");
        
          if(shett == 0)
           {
            if(mes != m)
             {
              if(mes != 0)
               {
                salorksl(data->dd,data->kk,data->ddn,data->kkn,data->debm,data->krem,data->ff1,data->ffact);
                data->debm=data->krem=0.;
               }
              mes=m;
             }
            shett=1;
           }

          data->debm+=deb;
          data->krem+=kre;

          fprintf(data->ff1,"%02d.%02d.%4d %-*s %-3s%4s %-*s %12s",
          d,m,g,
          iceb_u_kolbait(6,row[3]),row[3],
          row[5],row[11],
          iceb_u_kolbait(11,row[6]),row[6],
          iceb_u_prnbr(deb));
          
          fprintf(data->ff1," %12s %-*.*s\n",
          iceb_u_prnbr(kre),
          iceb_u_kolbait(32,row[13]),iceb_u_kolbait(32,row[13]),row[13]);

          koment.new_plus(row[6]);
          koment.plus(" ");
          koment.plus(row[13]);
          
          fprintf(data->ffact,"%02d.%02d.%4d %-*.*s %8.2f %8.2f| |\n",
          d,m,g,
          iceb_u_kolbait(20,koment.ravno()),iceb_u_kolbait(20,koment.ravno()),koment.ravno(),
          deb,kre);

          fprintf(data->ffact,"\
 -------------------------------------------------- ---------------------------------------------------\n");

        data->dd+=deb;
        data->kk+=kre;

       }
      if(iceb_u_sravmydat(d,m,g,dn,mn,gn) < 0)
       {
//        printf("До периода\n");
        data->ddn+=deb;
        data->kkn+=kre;
       }

     }

    //Увеличиваем дату на день
    iceb_u_dpm(&d,&m,&g,1);
   }
  return(TRUE);
 }

data->db+=data->dd+data->ddn;
data->kr+=data->kk+data->kkn;

double brr;

sprintf(strsql,"%*s:%12s",
iceb_u_kolbait(20,gettext("Оборот за период")),gettext("Оборот за период"),iceb_u_prnbr(data->dd));
stroka.new_plus(strsql);

sprintf(strsql," %12s\n",iceb_u_prnbr(data->kk));
stroka.plus(strsql);

iceb_printw(stroka.ravno_toutf(),data->buffer,data->view);

if(data->dd+ data->ddn >= data->kk+data->kkn)
 {
  brr=(data->dd+data->ddn)-(data->kk+data->kkn);
  sprintf(strsql,"%*s:%12s\n",
  iceb_u_kolbait(20,gettext("Сальдо конечное")),gettext("Сальдо конечное"),
  iceb_u_prnbr(brr));
  
  stroka.new_plus(strsql);
 }
else
 {
  brr=(data->kk+data->kkn)-(data->dd+data->ddn);
  sprintf(strsql,"%*s:%12s %12s\n",
  iceb_u_kolbait(20,gettext("Сальдо конечное")),gettext("Сальдо конечное"),
  " ",iceb_u_prnbr(brr));
  stroka.new_plus(strsql);
 }
iceb_printw(stroka.ravno_toutf(),data->buffer,data->view);

salorksl(data->dd,data->kk,data->ddn,data->kkn,data->debm,data->krem,data->ff1,data->ffact);

if(data->shets[0] != '\0' && data->metka_proh == 0)
 {
  strcpy(data->shet,data->shets);
  data->metka_proh=1;
  data->sproh=1;
  return(TRUE);
 }


if(data->metka_proh == 1)
 {
  fprintf(data->ff1,"\
---------------------------------------------------------------------------------------\n\
%*s:%12s",
  iceb_u_kolbait(37,gettext("Общий итог")),gettext("Общий итог"),iceb_u_prnbr(data->db));
  fprintf(data->ff1,"\
 %12s\n",iceb_u_prnbr(data->kr));

  if(data->db >= data->kr)
   {
    brr=data->db-data->kr;
    fprintf(data->ff1,"\
%37s %12s\n"," ",iceb_u_prnbr(brr));
   }
  else
   {
    brr=data->kr-data->db;
    fprintf(data->ff1,"\
%37s %12s %12s\n"," "," ",iceb_u_prnbr(brr));
   }
 }

iceb_podpis(data->ff1,data->window);
fprintf(data->ff1,"\x12");
fclose(data->ff1);
iceb_konact(data->ffact,data->window);
fclose(data->ffact);

iceb_printw_vr(data->vrem_nr,data->buffer,data->view);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет завершен."));
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
data->metka_zavr=1;
gtk_widget_grab_focus(data->knopka);

return(FALSE);
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_spks_r_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_spks_data *data)
{
//char  bros[512];

switch(event->keyval)
 {

  case GDK_Escape:
  case GDK_F10:
    if(data->metka_zavr == 0)
      return(TRUE);

    gtk_widget_destroy(data->window);
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

//    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}

/**************/
/*Вывод сальдо*/
/**************/
void salorksl(double dd, double kk,double ddn,double kkn,
double debm,double krem,
FILE *ff1,FILE *ffact)
{

double brr=dd+ddn;
double krr=kk+kkn;
fprintf(ff1,"\
---------------------------------------------------------------------------------------\n");

if(debm != 0. || krem != 0.)
 {
  fprintf(ff1,"\
%*s:%12s",iceb_u_kolbait(37,gettext("Оборот за месяц")),gettext("Оборот за месяц"),iceb_u_prnbr(debm));

  fprintf(ff1," %12s\n",iceb_u_prnbr(krem));
  if(ffact != NULL)
   fprintf(ffact,"%31s %8.2f %8.2f| |\n",
   gettext("Оборот за месяц"),debm,krem);
 }

fprintf(ff1,"%*s:%12s",
iceb_u_kolbait(37,gettext("Оборот за период")),gettext("Оборот за период"),iceb_u_prnbr(dd));

fprintf(ff1," %12s\n",iceb_u_prnbr(kk));

if(ffact != NULL)
 fprintf(ffact,"%*s %8.2f %8.2f| |\n",
 iceb_u_kolbait(31,gettext("Оборот за период")),gettext("Оборот за период"),dd,kk);

fprintf(ff1,"%*s:%12s",
iceb_u_kolbait(37,gettext("Сальдо раз-нутое")),gettext("Сальдо раз-нутое"),iceb_u_prnbr(brr));
fprintf(ff1," %12s\n",iceb_u_prnbr(brr));

if(ffact != NULL)
 fprintf(ffact,"%*s %8.2f %8.2f| |\n",
 iceb_u_kolbait(31,gettext("Сальдо раз-нутое")),gettext("Сальдо раз-нутое"),brr,krr);

if(dd+ ddn >= kk+kkn)
 {
  brr=(dd+ddn)-(kk+kkn);
  fprintf(ff1,"%*s:%12s\n",
  iceb_u_kolbait(37,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),iceb_u_prnbr(brr));

  if(ffact != NULL)
   fprintf(ffact,"%*s %8.2f %8s| |\n",
   iceb_u_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),brr," ");
 }
else
 {
  brr=(kk+kkn)-(dd+ddn);
  fprintf(ff1,"%*s:%12s %12s\n",
  iceb_u_kolbait(37,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",iceb_u_prnbr(brr));


  if(ffact != NULL)
   fprintf(ffact,"%*s %8s %8.2f| |\n",
   iceb_u_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",brr);

 }
}
