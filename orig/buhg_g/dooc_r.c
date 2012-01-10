/*$Id: dooc_r.c,v 1.14 2011-02-21 07:35:51 sasa Exp $*/
/*16.12.2009	21.10.2004	Белых А.И.	dooc_r.c
Расчёт дооценки
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "buhg_g.h"
#include "dooc.h"

class dooc_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  
  class dooc_data *rk;
  
  
 };

gboolean   dooc_r_key_press(GtkWidget *widget,GdkEventKey *event,class dooc_r_data *data);
gint dooc_r1(class dooc_r_data *data);
void  dooc_r_v_knopka(GtkWidget *widget,class dooc_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern double	nds1;
extern short	vtara; /*Код группы возвратная тара*/
extern double	okrcn;
extern iceb_u_str shrift_ravnohir;
 
void dooc_r(class dooc_data *rek_ras,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class dooc_r_data data;
data.rk=rek_ras;



data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Дооценить документ"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(dooc_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Дооценить документ"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),450,300);

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(dooc_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)dooc_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  dooc_r_v_knopka(GtkWidget *widget,class dooc_r_data *data)
{
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   dooc_r_key_press(GtkWidget *widget,GdkEventKey *event,class dooc_r_data *data)
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

gint dooc_r1(class dooc_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
iceb_u_str repl;

char rnd[56];
memset(rnd,'\0',sizeof(rnd));
short dr,mr,gr;
iceb_u_rsdat(&dr,&mr,&gr,data->rk->datad.ravno(),1);

if(iceb_pbpds(mr,gr,data->window) != 0)
 {
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char kodopdooc[50];

memset(kodopdooc,'\0',sizeof(kodopdooc));


if(iceb_poldan("Код операции автоматической дооценки",kodopdooc,"matnast.alx",data->window) != 0)
 {
kkkk:;
  iceb_menu_soob(gettext("В файле настройки не введён код операции автоматической дооценки !"),data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(kodopdooc[0] == '\0')
  goto kkkk;
SQL_str row;
SQLCURSOR cur;
//Проверяем есть ли код операции в списке приходов и расходов
sprintf(strsql,"select prov from Prihod where kod='%s'",kodopdooc);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  repl.new_plus(gettext("Не найден код операции прихода"));
  repl.plus(" ");
  repl.plus(kodopdooc);
  repl.plus(" !");
  
  iceb_menu_soob(&repl,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int prov1=atoi(row[0]);
sprintf(strsql,"select prov from Rashod where kod='%s'",kodopdooc);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  repl.new_plus(gettext("Не найден код операции расхода"));
  repl.plus(" ");
  repl.plus(kodopdooc);
  repl.plus(" !");
  
  iceb_menu_soob(&repl,data->window);

  gtk_widget_destroy(data->window);
  return(FALSE);
 }


int prov2=atoi(row[0]);

sprintf(strsql,"select nomkar,kodm,kolih,cena from Dokummat1 \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and tipz=1 \
and nomkar != 0 and voztar=0  order by kodm asc",
data->rk->gd,data->rk->md,data->rk->dd,data->rk->skl,data->rk->nomdok_c.ravno());
int kolstr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи"),data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int nomkar[kolstr];
int kodm[kolstr];
double kolih[kolstr];
double kolihok[kolstr];
double suma[kolstr];
strcpy(strsql,"LOCK TABLES Kart READ,Zkart READ,Material READ,icebuser READ");
if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка блокировки таблицы !"),strsql,data->window);


int nomstr=0;
SQL_str row1;
SQLCURSOR cur1;
double kolihdok=0.;
double sumai=0.;
double kolihost=0.;
short ddd,mmm,ggg;
iceb_u_poltekdat(&ddd,&mmm,&ggg);
class ostatok oo;

while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(data->rk->kodmat.ravno(),row[1],0,0) != 0)
    continue;    

  sprintf(strsql,"select kodgr from Material where kodm=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   if(vtara == atoi(row1[0]))
     continue;

  nomkar[nomstr]=atoi(row[0]);
  kodm[nomstr]=atoi(row[1]);
  kolihdok+=kolih[nomstr]=atof(row[2]);
  sumai+=suma[nomstr]=kolih[nomstr]*atof(row[3]);
  ostkarw(1,1,ggg,ddd,mmm,ggg,data->rk->skl,nomkar[nomstr],&oo);
  kolihost+=kolihok[nomstr]=oo.ostg[3];  
  
  nomstr++;
 }


if(kolihdok - kolihost > 0.009)
 {
  iceb_u_spisok repp;
  
  repp.plus(gettext("Остаки на карточках не равны количеству в документе !"));

  sprintf(strsql,"%s:%d.%d.%d %s:%f != %s:%f",
  gettext("Дата"),
  ddd,mmm,ggg,gettext("Документ"),
  kolihdok,gettext("Карточки"),kolihost);

  repp.plus(strsql);
  
  repp.plus(gettext("Сделать дооценку на остатки ?"));

  if(iceb_menu_danet(&repp,2,data->window) == 2)
   {
    sprintf(strsql,"UNLOCK TABLES");
    if(sql_zap(&bd,strsql) != 0)
      iceb_msql_error(&bd,gettext("Ошибка разблокировки таблицы !"),strsql,data->window);

    gtk_widget_destroy(data->window);
    return(FALSE);
   }
 }


sprintf(strsql,"LOCK TABLES Dokummat WRITE,Dokummat1 WRITE,\
Dokummat2 WRITE,Kart WRITE,icebuser READ");
if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка блокировки таблицы !"),strsql,data->window);

//Определяем номера документов
char	sklad[24];
sprintf(sklad,"%d",data->rk->skl);
iceb_u_str nomd1;
iceb_u_str nomd2;
iceb_nomnak(gr,sklad,&nomd1,1,0,0,data->window);
iceb_nomnak(gr,sklad,&nomd2,2,0,0,data->window);
char kontr[24];
sprintf(kontr,"00-%d",data->rk->skl);

time_t tmm;
time(&tmm);

if(zap_s_mudokw(1,dr,mr,gr,data->rk->skl,kontr,nomd1.ravno(),"",kodopdooc,0,prov1,nomd2.ravno(),0,0,0,1,data->window) != 0)
 return(FALSE);

sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',3,'%s %s')",gr,sklad,nomd1.ravno(),data->rk->nomdok_c.ravno(),data->rk->osnov.ravno());
iceb_sql_zapis(strsql,1,0,data->window);

if(zap_s_mudokw(2,dr,mr,gr,data->rk->skl,kontr,nomd2.ravno(),"",kodopdooc,0,prov2,nomd1.ravno(),0,0,0,1,data->window) != 0)
 return(FALSE);


sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',3,'%s %s')",gr,sklad,nomd2.ravno(),data->rk->nomdok_c.ravno(),data->rk->osnov.ravno());
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"nomd1=%s nomd2=%s\n",nomd1.ravno(),nomd2.ravno());
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

double	cena=0.;
double	kof=0.;
kof=(sumai+data->rk->sumadooc.ravno_atof())/sumai;

sprintf(strsql,"kof=%f=(%.2f+%.2f)/%.2f\n",kof,sumai,data->rk->sumadooc.ravno_atof(),sumai);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

char shetu[20],ei[32],nds[10],mnds[10],krat[20];
int	nomk=0;
double cenakar=0.;
short denv=0,mesv=0,godv=0;
char innom[16];
innom[0]='\0';
char nomz[50];
short deng,mesg,godg;

//Формируем записи приходного документа
for(int i=0 ; i < kolstr ; i++)
 {
  sprintf(strsql,"%4d %4d %8.8g %8.8g %8.8g\n",
  nomkar[i],kodm[i],kolih[i],kolihok[i],suma[i]);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  
  if(kolih[i] == 0. || suma[i] == 0.)
    continue;

  cena=(suma[i]*kof)/kolih[i];
  
  sprintf(strsql,"cena=(%f*%f)/%f=%f\n",suma[i],kof,kolih[i],cena);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  
  cena=iceb_u_okrug(cena,okrcn);
  
  memset(shetu,'\0',sizeof(shetu));
  memset(ei,'\0',sizeof(ei));
  memset(nds,'\0',sizeof(nds));
  memset(mnds,'\0',sizeof(mnds));
  memset(krat,'\0',sizeof(krat));

  deng=mesg=godg=0;
  //  Читаем реквизиты карточки
  sprintf(strsql,"select shetu,ei,nds,krat,mnds,cena,innom,rnd,nomz,dat_god from \
Kart where sklad=%d and nomk=%d",data->rk->skl,nomkar[i]);
  if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
   {
    sprintf(strsql,"Не нашли карточку %d на складе %d !\n",nomkar[i],data->rk->skl);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    continue;
   }

  strncpy(shetu,row[0],sizeof(shetu)-1);
  strncpy(ei,row[1],sizeof(ei)-1);
  strncpy(nds,row[2],sizeof(nds)-1);
  strncpy(krat,row[3],sizeof(krat)-1);
  strncpy(mnds,row[4],sizeof(mnds)-1);
  cenakar=atof(row[5]);
  cenakar=iceb_u_okrug(cenakar,okrcn);
  strncpy(innom,row[6],sizeof(innom)-1);
  strncpy(rnd,row[7],sizeof(rnd)-1);
  strncpy(nomz,row[8],sizeof(nomz)-1);
  iceb_u_rsdat(&deng,&mesg,&godg,row[9],2);
   
  //Проверяем может карточка с введенными уникальными реквизитами уже есть на складе
  sprintf(strsql,"select nomk from Kart where kodm=%d and sklad=%d \
  and shetu='%s' and cena=%.10g and ei='%s' and nds=%s and cenap=%.10g \
  and krat=%s and mnds=%s",
  kodm[i],data->rk->skl,shetu,cena,ei,nds,0.,krat,mnds);

  /*
  printw("\n%s\n",strsql);
  OSTANOV();
  */
  if(sql_readkey(&bd,strsql,&row,&cur1) > 0)
   {
    nomk=atoi(row[0]);
    zapvdokw(dr,mr,gr,data->rk->skl,nomk,kodm[i],nomd1.ravno(),kolih[i],cena,
    ei,atof(nds),atoi(mnds),0,1,nomkar[i],"",nomz,data->window);
   }
  else
   {
    int nomk=nomkrw(data->rk->skl,data->window);
//    printw("nds=%s shbm=%s\n",nds,shetu);
//    refresh();
         
    if(zapkarw(data->rk->skl,nomk,kodm[i],cena,ei,shetu,atof(krat),0.,0,
    atof(nds),atoi(mnds),0.,denv,mesv,godv,innom,rnd,nomz,deng,mesg,godg,0,data->window) == 0)
      zapvdokw(dr,mr,gr,data->rk->skl,nomk,kodm[i],nomd1.ravno(),kolih[i],cena,
      ei,atof(nds),atoi(mnds),0,1,nomkar[i],"",nomz,data->window);
   }

  //Формируем запись расходного документа
  zapvdokw(dr,mr,gr,data->rk->skl,nomkar[i],kodm[i],nomd2.ravno(),kolih[i],cenakar,
  ei,atof(nds),atoi(mnds),0,2,nomk,"",nomz,data->window);

 }

sprintf(strsql,"UNLOCK TABLES");
if(sql_zap(&bd,strsql) != 0)
      iceb_msql_error(&bd,gettext("Ошибка разблокировки таблицы !"),strsql,data->window);


sprintf(strsql,"%s: %s\n",
gettext("Номер расходного документа"),nomd2.ravno());
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%s: %s\n",
gettext("Номер приходного документа"),nomd1.ravno());
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);
return(FALSE);
}
