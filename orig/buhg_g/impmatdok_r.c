/*$Id: impmatdok_r.c,v 1.19 2011-02-21 07:35:52 sasa Exp $*/
/*11.11.2008	16.11.2004	Белых А.И.	impmatdok_r.c
импорт документов из файла
*/
#include  <stdlib.h>
#include  <math.h>
#include  <errno.h>
#include  <unistd.h>
#include        <sys/stat.h>
#include "buhg_g.h"

class impmatdok_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
    
  iceb_u_str imafz;
  
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчет завершен 1-нет
  impmatdok_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   impmatdok_r_key_press(GtkWidget *widget,GdkEventKey *event,class impmatdok_r_data *data);
gint impmatdok_r1(class impmatdok_r_data *data);
void  impmatdok_r_v_knopka(GtkWidget *widget,class impmatdok_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern double okrcn;
extern iceb_u_str shrift_ravnohir;

int impmatdok_r(const char *imaf,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class impmatdok_r_data data;

data.imafz.new_plus(imaf);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Импорт документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(impmatdok_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Импорт документов"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(impmatdok_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)impmatdok_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  impmatdok_r_v_knopka(GtkWidget *widget,class impmatdok_r_data *data)
{
printf("impmatdok_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   impmatdok_r_key_press(GtkWidget *widget,GdkEventKey *event,class impmatdok_r_data *data)
{
 printf("impmatdok_r_key_press\n");
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

gint impmatdok_r1(class impmatdok_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
iceb_u_str repl;
struct stat work;


if(stat(data->imafz.ravno(),&work) != 0)
 {
  repl.new_plus(gettext("Нет такого файла !"));
  repl.ps_plus(data->imafz.ravno());
  iceb_menu_soob(&repl,data->window);
  return(FALSE);
 }



char kodiopuc[112];
int kodopuc=0; //Метка операции списания по учетной цене

iceb_poldan("Коды операций расхода по учетным ценам",kodiopuc,"matnast.alx",data->window);
short metkaproh=0;
FILE *ff;

nazad:;


if((ff = fopen(data->imafz.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(data->imafz.ravno(),"",errno,data->window);
  return(FALSE);
 }


if(metkaproh == 0)
 sprintf(strsql,"%s\n",gettext("Проверяем записи в файле"));
if(metkaproh == 1)
 sprintf(strsql,"\n\n%s\n\n",gettext("Загружаем документы из файла"));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

float razmer=0;
char nomdok[40];
memset(nomdok,'\0',sizeof(nomdok));
int koldok=0;
int koloh=0;
short dd,md,gd;
int tipz=0;
int sklad=0;
char strok[1024];
int i;
char nomnalnak[56];
SQL_str row1;
SQL_str row;
SQLCURSOR cur1;
char kontr[40];
char kodop[40];
char nomvstdok[40];
short dpnn,mpnn,gpnn;
time_t tmm;
time(&tmm);
OPSHET shetv;
int vtara;
float nds;
char nomzak[56];
int kodmat;
double kolih=0.;
double cena=0.;
char eiz[40];
int ndsvkl;
char shetu[50];
char naimat[512];
char naim[512];
char		rnd[20]; //Регистрационный номер для медпрепаратов
char		innom[20];
int kodusl=0;
memset(innom,'\0',sizeof(innom));
memset(rnd,'\0',sizeof(rnd));
memset(shetu,'\0',sizeof(shetu));

while(fgets(strok,sizeof(strok),ff) != NULL)
 {
  i=strlen(strok);
  razmer+=i;
  if(strok[i-2] == '\r') //Возврат каректки меняем на пробел
   strok[i-2]=' ';

  iceb_printw(iceb_u_toutf(strok),data->buffer,data->view);
  iceb_pbar(data->bar,work.st_size,razmer);    
//  strzag(LINES-1,0,work.st_size,razmer);

  if(iceb_u_SRAV(strok,"DOC|",1) == 0)
   {
    if(koldok > 0 && metkaproh > 0 && nomdok[0] != '\0')
     {
      podtdok1w(dd,md,gd,sklad,nomdok,tipz,dd,md,gd,1,data->window);
      podvdokw(dd,md,gd,nomdok,sklad,data->window);
     }     
    koldok++;
    
    memset(nomdok,'\0',sizeof(nomdok));
    dd=md=gd=0;
    tipz=0;

    iceb_u_polen(strok,strsql,sizeof(strsql),2,'|');

    if(strsql[0] == '+')
      tipz=1;
    if(strsql[0] == '-')
      tipz=2;

    if(tipz == 0)
     {
      repl.new_plus(gettext("Не определен вид документа (приход/расход) !"));
      iceb_menu_soob(&repl,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok,strsql,sizeof(strsql),3,'|');
    if(iceb_u_rsdat(&dd,&md,&gd,strsql,1) != 0)
     {
      repl.new_plus(gettext("Не верно введена дата!"));
      iceb_menu_soob(&repl,data->window);
      koloh++;
      continue;
     }
    iceb_u_polen(strok,strsql,sizeof(strsql),4,'|');
    sklad=atoi(strsql);
    sprintf(strsql,"select kod from Sklad where kod=%d",sklad);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      repl.new_plus(gettext("Не найден код склада"));
      repl.plus(" ");
      repl.plus(sklad);
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);
      
      koloh++;
      continue;
     }

    iceb_u_polen(strok,kontr,sizeof(kontr),5,'|');
    sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",kontr);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      repl.new_plus(gettext("Не нашли код контрагента"));
      repl.plus(" ");
      repl.plus(kontr);
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);
      
      koloh++;
      continue;
     }


    iceb_u_polen(strok,nomnalnak,sizeof(nomnalnak),7,'|');

    if(tipz == 2 && nomnalnak[0] != '\0')
     {
      sprintf(strsql,"select datd,nomd from Dokummat where tip=%d and \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and nomnn='%s'",
      tipz,gd,gd,nomnalnak);
      if(sql_readkey(&bd,strsql,&row1,&cur1) >= 1)
       {
        sprintf(strsql,gettext("С номером %s налоговая накладная уже есть !"),nomnalnak);
        repl.new_plus(strsql);
        sprintf(strsql,"%s %s %s",gettext("Материальный учет"),row1[0],row1[1]);
        repl.ps_plus(strsql);
        iceb_menu_soob(&repl,data->window);
        koloh++;
        continue;
       }

      sprintf(strsql,"select datd,nomd from Usldokum where tp=%d and \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and nomnn='%s'",
      tipz,gd,gd,nomnalnak);
      if(sql_readkey(&bd,strsql,&row1,&cur1) >= 1)
       {
        sprintf(strsql,gettext("С номером %s налоговая накладная уже есть !"),nomnalnak);
        repl.new_plus(strsql);
        sprintf(strsql,"%s %s %s",gettext("Учёт услуг"),row1[0],row1[1]);
        repl.ps_plus(strsql);
        iceb_menu_soob(&repl,data->window);
        koloh++;
        continue;
       }
     }

    iceb_u_polen(strok,kodop,sizeof(kodop),8,'|');
    if(tipz == 1)
      sprintf(strsql,"select kod from Prihod where kod='%s'",kodop);
    if(tipz == 2)
      sprintf(strsql,"select kod from Rashod where kod='%s'",kodop);

    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      repl.new_plus(gettext("Не найден код операции"));
      repl.plus(" ");
      repl.plus(kodop);
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);
      
      koloh++;
      continue;
     }

    iceb_u_polen(strok,nomvstdok,sizeof(nomvstdok),9,'|');

    dpnn=mpnn=gpnn=0;
    if(nomnalnak[0] != '\0')
     {
      iceb_u_polen(strok,strsql,sizeof(strsql),10,'|');
      
      if(strsql[0] != '\0')
       {
        if(iceb_u_rsdat(&dpnn,&mpnn,&gpnn,strsql,1) != 0)
         {
          if(tipz == 1)
            repl.new_plus(gettext("Не верно введена дата получения налоговой накладной !"));
          if(tipz == 2)
            repl.new_plus(gettext("Не верно введена дата выдачи налоговой накладной !"));
          iceb_menu_soob(&repl,data->window);
          koloh++;
          continue;
         }
       }
      else
       {
        dpnn=dd; mpnn=md; gpnn=gd;
       }
     }

    
    //Номер документа определяем последним так как он является индикатором
    //наличия шапки документа
    iceb_u_polen(strok,nomdok,sizeof(nomdok),6,'|');
//  printw("***nomdok=%s %d\n",nomdok,metkaproh);
 
    sprintf(strsql,"select nomd from Dokummat where tip=%d and \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and sklad=%d and nomd='%s'",
    tipz,gd,gd,sklad,nomdok);
    if(sql_readkey(&bd,strsql,&row1,&cur1) >= 1)
     {
      sprintf(strsql,gettext("С номером %s документ уже есть !"),nomdok);
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    //Запись в базу
    if(metkaproh != 0 && nomdok[0] != '\0')
     {
      zap_s_mudokw(tipz,dd,md,gd,sklad,kontr,nomdok,nomnalnak,kodop,0,0,nomvstdok,dpnn,mpnn,gpnn,0,data->window);
     }        
   }

  //Строка записи в документе
  if(iceb_u_SRAV(strok,"ZP1|",1) == 0)
   {
    iceb_u_polen(strok,strsql,sizeof(strsql),2,'|');
    kodmat=atoi(strsql);    
    sprintf(strsql,"select kodm from Material where kodm=%d",kodmat);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      repl.new_plus(gettext("Не найден код материалу"));
      repl.plus(" ");
      repl.plus(kodmat);
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);
      koloh++;
      continue;
     }
    iceb_u_polen(strok,strsql,sizeof(strsql),3,'|');
    kolih=atof(strsql);
    if(kolih == 0)
     {
      repl.new_plus(gettext("В записи нулевое количество материалла"));
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok,strsql,sizeof(strsql),4,'|');
    cena=atof(strsql);
    cena=iceb_u_okrug(cena,okrcn);

    iceb_u_polen(strok,eiz,sizeof(eiz),5,'|');
    sprintf(strsql,"select kod from Edizmer where kod='%s'",eiz);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      repl.new_plus(gettext("Не найдена единица измерения"));
      repl.plus(" ");
      repl.plus(eiz);
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok,strsql,sizeof(strsql),6,'|');
    vtara=atoi(strsql);

    iceb_u_polen(strok,strsql,sizeof(strsql),7,'|');
    nds=atof(strsql);

    iceb_u_polen(strok,strsql,sizeof(strsql),8,'|');
    ndsvkl=atoi(strsql);

    shetu[0]='\0';

    iceb_u_polen(strok,shetu,sizeof(shetu),9,'|'); //Для расходных документов не обязательный реквизит
    if(tipz == 1 || (tipz == 2  && shetu[0] != '\0'))
      if(iceb_prsh1(shetu,&shetv,data->window) != 0)
       {
        koloh++;
        continue;
       }

    iceb_u_polen(strok,nomzak,sizeof(nomzak),10,'|');
   
    if(metkaproh != 0 && nomdok[0] != '\0')
     {
//      printw("%d.%d.%d %d %d %s %.10g %.10g %s %f %d %d %d\n",
//      dd,md,gd,sklad,kodmat,nomdok,kolih,cena,eiz,nds,ndsvkl,vtara,tipz);      

      //Проверяем может такой код материалла уже есть в накладной
      if(tipz == 2)
       {
        kodopuc=0;
        if(iceb_u_proverka(kodiopuc,kodop,0,1) == 0)
           kodopuc=1;  //Списание по учетной цене

        sprintf(strsql,"select kodm from Dokummat1 where tipz=%d and \
  datd >= '%04d-01-01' and datd <= '%04d-12-31' and sklad=%d and nomd='%s' \
  and kodm=%d",tipz,gd,gd,sklad,nomdok,kodmat);
        if(sql_readkey(&bd,strsql,&row,&cur1) > 0)
         {
          memset(naimat,'\0',sizeof(naimat)-1);
          sprintf(strsql,"select naimat from Material where kodm=%d",kodmat);
          if(sql_readkey(&bd,strsql,&row,&cur1) > 0)
            strncpy(naimat,row[0],sizeof(naimat)-1);

          repl.new_plus(gettext("Код материалла"));
          repl.plus(" ");
          repl.plus(kodmat);
          repl.plus(" ");
          repl.plus(naimat);
          
          repl.ps_plus(gettext("Такая запись в документе уже есть !"));
          repl.ps_plus(gettext("Документ"));
          repl.plus(" N");
          repl.plus(nomdok);
          iceb_menu_soob(&repl,data->window);
          continue;
         }
       }

      if(tipz == 1)      
       {
        zap_prihodw(dd,md,gd,nomdok,sklad,kodmat,kolih,cena,eiz,shetu,nds,vtara,ndsvkl,nomzak,"",data->window);

       }

      if(tipz == 2)
       {
        priv_k_kartw(dd,md,gd,nomdok,sklad,kodmat,kolih,cena,shetu,eiz,nds,ndsvkl,vtara,nomzak,kodopuc,"",data->window);
       }
     }
   }

  //Строка записи услуг
  if(iceb_u_SRAV(strok,"ZP2|",1) == 0)
   {
    iceb_u_polen(strok,eiz,sizeof(eiz),2,'|');
    sprintf(strsql,"select kod from Edizmer where kod='%s'",eiz);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      repl.new_plus(gettext("Не найдена единица измерения"));
      repl.plus(" ");
      repl.plus(eiz);
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);
      koloh++;
      continue;
     }
    iceb_u_polen(strok,strsql,sizeof(strsql),3,'|');
    kolih=atof(strsql);

    iceb_u_polen(strok,strsql,sizeof(strsql),4,'|');
    cena=atof(strsql);
    cena=iceb_u_okrug(cena,okrcn);
    iceb_u_polen(strok,strsql,sizeof(strsql),5,'|');
    nds=atof(strsql);

    memset(naim,'\0',sizeof(naim));
    iceb_u_polen(strok,naim,sizeof(naim),6,'|');

    if(metkaproh != 0 && nomdok[0] != '\0')
     {
      sprintf(strsql,"insert into Dokummat3 (datd,sklad,nomd,ei,kolih,cena,nds,naimu,ktoi,vrem,ku)\
values ('%04d-%02d-%02d',%d,'%s','%s',%.10g,%.10g,%.10g,'%s',%d,%ld,%d)",
      gd,md,dd,sklad,nomdok,eiz,kolih,cena,nds,naim,
      iceb_getuid(data->window),tmm,kodusl);
//      printw("%s\n",strsql);
      if(sql_zap(&bd,strsql) != 0)
        iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,data->window);
     }
   }

  //Строка записи дополнительной информации к документа
  if(iceb_u_SRAV(strok,"ZP3|",1) == 0)
   {
    iceb_u_polen(strok,strsql,sizeof(strsql),2,'|');
    int nomz=atoi(strsql);
    if(nomz == 0)
      continue;    
    iceb_u_polen(strok,naim,sizeof(naim),3,'|');

    if(nomz == 8)
     {
      sprintf(strsql,"select kod from Foroplat where kod='%s'",naim);
      if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
       {
        repl.new_plus(gettext("Не найдена форма оплаты"));
        repl.plus(" ");
        repl.plus(naim);
        repl.plus(" !");
        iceb_menu_soob(&repl,data->window);
        koloh++;
        continue;
       }

     }

    if(metkaproh != 0 && nomdok[0] != '\0')
     {
      sprintf(strsql,"insert into Dokummat2 \
values (%d,%d,'%s',%d,'%s')",gd,sklad,nomdok,nomz,naim);
      if(sql_zap(&bd,strsql) != 0)
        iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,data->window);
     }    
   }
 }

fclose(ff);
if(koldok > 0 && metkaproh > 0 && nomdok[0] != '\0')
 {
  podtdok1w(dd,md,gd,sklad,nomdok,tipz,dd,md,gd,1,data->window);
  podvdokw(dd,md,gd,nomdok,sklad,data->window);
 }

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

if(koloh != 0)
 {



  repl.new_plus(gettext("Количество ошибок"));
  repl.plus(":");
  repl.plus(koloh);
  
  repl.ps_plus(gettext("Импорт документов осуществляется если нет ни одной ошибки"));
  iceb_menu_soob(&repl,data->window);
  return(FALSE);
 }

if(koldok == 0)
 {

  repl.new_plus(gettext("Не найдено ни одного документа !"));
  iceb_menu_soob(&repl,data->window);


  return(FALSE);
 }

if(metkaproh == 0)
 {
  metkaproh=1;
  goto nazad;
 }


repl.new_plus(gettext("Загрузка завершена"));
repl.ps_plus(gettext("Количество документов"));
repl.plus(":");
repl.plus(koldok);

iceb_menu_soob(&repl,data->window);

data->voz=0;

return(FALSE);
}
