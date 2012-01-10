/*$Id: pvbank_r.c,v 1.20 2011-02-21 07:35:56 sasa Exp $*/
/*29.09.2009	08.04.2004	Белых А.И.	pvbank_r.c
Расчет перечислений на карт-счета 
*/
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "pvbank.h"


class pvbank_r_data
 {
  public:
  
  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  gfloat kolstr1; //Количество прочитанных строк
  int    kolstr;  //Количество строк в курсоре
  short     prohod;
  iceb_u_spisok imaf;
  iceb_u_spisok naim;

  pvbank_rr *rek_r;
  short dn,mn,gn;
  short dk,mk,gk;
  short  godn;
  time_t vremn;
    
  FILE *ffsp;
  FILE *ffdis;
  char rukov[512];
  char glavbuh[40];
  char pris[20];
  double sumalist;
  double itogoo;
  short dlpris;
  int  kolstrkartr;
  int nomer;
  SQLCURSOR cur;

  //Конструктор  
  pvbank_r_data()
   {
    kolstr1=0.;
    kolstr=0;
    prohod=0;

    sumalist=0.;
    itogoo=0.;
    dlpris=0;
    kolstrkartr=0;
    nomer=0;
   }

 };

gboolean   pvbank_r_key_press(GtkWidget *widget,GdkEventKey *event,class pvbank_r_data *data);
gint pvbank_r1(class pvbank_r_data *data);
void  pvbank_r_v_knopka(GtkWidget *widget,class pvbank_r_data *data);

extern SQL_baza bd;
extern char *organ;
extern char *name_system;
extern short vplsh; /*0-двух порядковый план счетов 1-многпорядковый*/
extern short	startgodb; /*Стартовый год*/
extern int kol_strok_na_liste;
extern iceb_u_str shrift_ravnohir;

void pvbank_r(class pvbank_rr *data_rr)
{
char strsql[512];
iceb_u_str soob;

pvbank_r_data data;

data.rek_r=data_rr;


printf("pvbank\n");
if(iceb_rsdatp(&data.dn,&data.mn,&data.gn,data.rek_r->datan.ravno(),
&data.dk,&data.mk,&data.gk,data.rek_r->datak.ravno(),NULL) != 0)
  return;
  
data.godn=startgodb;
if(startgodb == 0 || startgodb > data.gn)
 data.godn=data.gn;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать документы для перечисления на карт-счета"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(pvbank_r_key_press),&data);

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);
sprintf(strsql,"%s %s",gettext("Расчет с использованием проводок"),data.rek_r->shet.ravno());

soob.new_plus(strsql);

sprintf(strsql,"%s %d.%d.%d => %d.%d.%d",gettext("Расчет за период"),
data.dn,data.mn,data.gn,
data.dk,data.mk,data.gk);
soob.ps_plus(strsql);

GtkWidget *label=gtk_label_new(soob.ravno_toutf());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);


data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),400,300);

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
gtk_widget_show_all(data.window);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
GtkTooltips *tooltops=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltops,data.knopka,gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(pvbank_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)pvbank_r1,&data);

gtk_main();

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);





}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  pvbank_r_v_knopka(GtkWidget *widget,class pvbank_r_data *data)
{
// printf("pvbank_r_v_knopka\n");
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   pvbank_r_key_press(GtkWidget *widget,GdkEventKey *event,class pvbank_r_data *data)
{
// printf("pvbank_r_key_press\n");

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

gint pvbank_r1(class pvbank_r_data *data)
{
char strsql[512];
iceb_u_str soob;

//printf("pvbank_r1 data->prohod=%d %d %f \n",data->prohod,data->kolstr,data->kolstr1);

if(data->prohod == 0)
 {
  time(&data->vremn);
  data->prohod++;
  SQL_str row1;
  SQLCURSOR cur1;
  char imafsp[30];
  char imafdis[30];
  char bros[512];

  if(data->rek_r->metka_r == 0)
    sprintf(strsql,"select sh,kodkon,deb from Prov where datp >= '%d-%d-%d' and datp <= '%d-%d-%d' \
  and kodkon <> '' and deb <> 0.",data->gn,data->mn,data->dn,data->gk,data->mk,data->dk);
  else
    sprintf(strsql,"select sh,kodkon,kre from Prov where datp >= '%d-%d-%d' and datp <= '%d-%d-%d' \
  and kodkon <> '' and kre <> 0.",data->gn,data->mn,data->dn,data->gk,data->mk,data->dk);


  if((data->kolstr=data->cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    return(FALSE);
   }

  if(data->kolstr == 0)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдено ни одной записи !"));
    iceb_menu_soob(&repl,data->window);
    return(FALSE);
   }

  memset(data->rukov,'\0',sizeof(data->rukov));
  memset(data->glavbuh,'\0',sizeof(data->glavbuh));
  memset(data->pris,'\0',sizeof(data->pris));


  iceb_poldan("Приставка к табельному номеру",data->pris,"zarnast.alx",data->window);
  data->dlpris=strlen(data->pris);
  
  iceb_poldan("Табельный номер руководителя",bros,"zarnast.alx",data->window);
  if(bros[0] != '\0')
   {
    sprintf(strsql,"select fio from Kartb where tabn=%s",bros);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      strncpy(data->rukov,row1[0],sizeof(data->rukov)-1);
   } 

  iceb_poldan("Табельный номер бухгалтера",bros,"zarnast.alx",data->window);

  if(bros[0] != '\0')
   {
    sprintf(strsql,"select fio from Kartb where tabn=%s",bros);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      strncpy(data->glavbuh,row1[0],sizeof(data->glavbuh)-1);
   }
   
  sprintf(imafsp,"pib%d.lst",getpid());
  sprintf(imafdis,"pibd%d.lst",getpid());

  data->imaf.plus(imafsp);
  data->imaf.plus(imafdis);

  data->naim.plus(gettext("Ведомость на перечисление"));  

  data->naim.plus(gettext("Файл для записи на дискету"));  

  if((data->ffsp = fopen(imafsp,"w")) == NULL)
   {
    iceb_er_op_fil(imafsp,"",errno,data->window);
    return(FALSE);
   }
  iceb_u_startfil(data->ffsp);
  hrvnks(data->ffsp,&data->kolstrkartr);

  if((data->ffdis = fopen(imafdis,"w")) == NULL)
   {
    iceb_er_op_fil(imafdis,"",errno,data->window);
    return(FALSE);
   }
  
  return(TRUE);
 }

if(data->prohod == 1)
 {
  SQL_str row;
  char  bankshet[32];
  char tabnom[20];
  short shethik=0;
  char  fio[512];
//  int   dlkodkon=0;
  double sum=0.;
  int   i=0;
  SQLCURSOR cur1;
  SQL_str   row1;
  while(data->cur.read_cursor(&row) != 0)
   {
    iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

    if(iceb_u_proverka(data->rek_r->shet.ravno(),row[0],0,0) != 0)
      continue;
    if(iceb_u_proverka(data->rek_r->kontr.ravno(),row[1],0,0) != 0)
      continue;

  //  printw("Контрагент-%s\n",row[1]);  

    if(iceb_u_SRAV(data->pris,row[1],1) != 0)
     continue;

    memset(tabnom,'\0',sizeof(tabnom));
    shethik=0;
    int dlkodkon=strlen(row[1]);
    for(i=data->dlpris; i < dlkodkon; i++)
     tabnom[shethik++]=row[1][i];

    memset(fio,'\0',sizeof(fio));
    memset(bankshet,'\0',sizeof(bankshet));
    
    //Читем в карточке фамилию и картсчет   
    sprintf(strsql,"select fio,bankshet from Kartb where tabn=%s",tabnom);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     {
      strncpy(fio,row1[0],sizeof(fio)-1);
      strncpy(bankshet,row1[1],sizeof(bankshet)-1);
     }

    if(bankshet[0] == '\0')
     {
      iceb_u_str repl;
      repl.plus(gettext("Не введено счёт !"));
      sprintf(strsql,"%s:%s %s",gettext("Табельный номер"),tabnom,fio);
      repl.ps_plus(strsql);
      iceb_menu_soob(&repl,data->window);
      continue;
     }

    sum=atof(row[2]);
    
    data->kolstrkartr++;
    if(data->kolstrkartr > kol_strok_na_liste-5)
     {
      fprintf(data->ffsp,"%-*s:%8.2f\n\n",iceb_u_kolbait(16,"Всього по листу"),"Всього по листу",data->sumalist);
      fprintf(data->ffsp,"\
              Керівник _______________%s\n\n\
    Головний бухгалтер _______________%s\n",data->rukov,data->glavbuh);
      data->kolstrkartr=1;
      data->sumalist=0.;
      fprintf(data->ffsp,"\f");
     }
    data->sumalist+=sum;
    data->itogoo+=sum;
    data->nomer++;
    fprintf(data->ffsp,"%-16s %8.2f %s\n",bankshet,sum,fio);
    
    fprintf(data->ffdis,"%-16s %8.2f %s\n",bankshet,sum,fio);

    sprintf(strsql,"%-16s %8.2f %s\n",bankshet,sum,fio);
    soob.new_plus(strsql);
    iceb_printw(soob.ravno_toutf(),data->buffer,data->view);

    return(TRUE);

   }

  fprintf(data->ffsp,"%-*s:%8.2f\n\n",iceb_u_kolbait(16,"Всього по листу"),"Всього по листу",data->sumalist);
  fprintf(data->ffsp,"%*s:%8.2f\n",iceb_u_kolbait(16,gettext("Итого")),gettext("Итого"),data->itogoo);


  fprintf(data->ffsp,"\nКількість одержувачів коштів: %d чоловік\n",data->nomer);

  memset(strsql,'\0',sizeof(strsql));
  iceb_u_preobr(data->itogoo,strsql,0);

  fprintf(data->ffsp,"На суму:%s\n\n",strsql);

  fprintf(data->ffsp,"\
            Керівник _______________%s\n\n\
  Головний бухгалтер _______________%s\n",data->rukov,data->glavbuh);

  fprintf(data->ffsp,"\x1b\x6C%c",1); /*Установка левого поля в ноль*/

  fclose(data->ffsp);
  fclose(data->ffdis);

  //Записываем шапку и концовку
  hdisk(data->nomer,data->itogoo,data->imaf.ravno(1));


  data->prohod++;
  return(TRUE);
 
 }
if(data->prohod == 2)
 {
  data->prohod++;

  iceb_printw_vr(data->vremn,data->buffer,data->view);
  return(TRUE);
 }

//gtk_progress_bar_set_text(GTK_PROGRESS_BAR(data->bar),"100%%");

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);
return(FALSE);

}
