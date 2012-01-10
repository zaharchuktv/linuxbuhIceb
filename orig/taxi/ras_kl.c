/*$Id: ras_kl.c,v 1.11 2011-02-21 07:36:21 sasa Exp $*/
/*12.10.2007	09.10.2007	Белых А.И.	ras_kl.c
Расчёт клиента 
*/
#include <unistd.h>
#include <stdlib.h>
#include        "i_rest.h"
enum
 {
  VOZVRAT_KAR,
  VOZVRAT_SUM,
  OPLATA,
  VID_KART,
  PROS_SHETS,
  PROV_VV,
  PROS_VV,
  VIHOD,
  KOLKNOP
 };

class ras_kl_data
 {
  public:
   GtkWidget *window;
   int       knopka;
 };

int ras_kl_menu(int kn_fokus,const char *kod_kl,const char *fio,int denrog,int pol,class iceb_u_str *zap_vv,int metka_blok,double*,double*);
gboolean ras_kl_destroy(GtkWidget *widget,GdkEvent *event,int *knopka);
void   ras_kl_knopka(GtkWidget *widget,class ras_kl_data *data);

void ras_kl_vk(const char *kod_kl,double saldo,double saldo_po_kas);
int vsvks(int metka,const char*,double,struct KASSA *kasr);
int  l_vhvi_pkr(const char *kodkart,GtkWidget *wpredok);
void l_saldokl(const char*,GtkWidget*);
void prov_vv(const char *kod_kart,GtkWidget *wpredok);

extern char	*name_system;
extern SQL_baza	bd;
extern int  nomer_kas;  //номер кассы
extern uid_t kod_operatora;
extern class KASSA kasr;

void ras_kl(const char *kod_kl)
{

char strsql[512];
SQL_str row;
SQLCURSOR cur;
char data_vv[56];
char kod_pd_vv[16];
int tp_vv=0;

memset(data_vv,'\0',sizeof(data_vv));
memset(kod_pd_vv,'\0',sizeof(kod_pd_vv));

char dvk[50];
char fio[60];
int denrog=0;
int pol=0;
int metka_blok=0;

denrog=0;
memset(fio,'\0',sizeof(fio));
memset(dvk,'\0',sizeof(dvk));
//читаем фамилию клиента
sprintf(strsql,"select fio,dvk,pl,denrog,mb from Taxiklient where kod='%s'",kod_kl);
if(iceb_sql_readkey(strsql,&row,&cur,NULL) == 1)
 {
  strncpy(fio,row[0],sizeof(fio)-1);
  strncpy(dvk,row[1],sizeof(dvk)-1);
  pol=atoi(row[2]);
  if(row[3][0] != '0')
   {
    short dt,mt,gt;
    iceb_u_poltekdat(&dt,&mt,&gt);
    short dr,mr,gr;
    iceb_u_rsdat(&dr,&mr,&gr,row[3],2);
    if(iceb_u_sravmydat(dt,mt,gt,dr,mr,gr) == 0)
     denrog=1;     
   }
  metka_blok=atoi(row[4]);
 }
else
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код клиента"),kod_kl);
  iceb_menu_soob(strsql,NULL);
  return;
 }  




int zap=0;
//смотрим последнюю запись по карточке в таблице входов/выходов
sprintf(strsql,"select dv,kp,tp from Restvv where kk='%s' and dv > '%s' order by dv desc limit 2",kod_kl,dvk);
//printf("%s\n",strsql);
class iceb_u_str zap_vv;

if((zap=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return;
 }
int metka_str=0;
while(cur.read_cursor(&row) != 0)
 {
  strncpy(data_vv,iceb_u_datetime_pr(row[0]),sizeof(data_vv)-1);
  strncpy(kod_pd_vv,row[1],sizeof(kod_pd_vv)-1);
  tp_vv=atoi(row[2]);  
  
  sprintf(strsql,"%s %s:%s ",
  data_vv,
  gettext("Подразделение"),kod_pd_vv);

  if(metka_str == 0)
    zap_vv.plus(strsql);
  else
    zap_vv.ps_plus(strsql);

  if(tp_vv == 1)
   zap_vv.plus(gettext("Вход"));
  if(tp_vv == 2)
   zap_vv.plus(gettext("Выход"));
  metka_str++;
 }

int kom=0;
double saldo_po_kas=0.;
double saldo=0.;

while( kom >= 0 && kom < KOLKNOP-1 )
 {
  kom=ras_kl_menu(kom,kod_kl,fio,denrog,pol,&zap_vv,metka_blok,&saldo,&saldo_po_kas);
  
  switch(kom)
   {

    case VOZVRAT_KAR:
      ras_kl_vk(kod_kl,saldo,saldo_po_kas);
      return;
      break;

    case VOZVRAT_SUM:
      vsvks(1,kod_kl,saldo,&kasr);
      break;

    case OPLATA:
     vsvks(0,kod_kl,saldo,&kasr);
      break;

    case VID_KART:
      vid_kar_kl(kod_kl);
      break;

    case PROS_SHETS:
      l_saldokl(kod_kl,NULL);
      break;

    case PROV_VV:
      prov_vv(kod_kl,NULL);
      break;

    case PROS_VV:
      l_vhvi_pkr(kod_kl,NULL);
      break;


    case VIHOD:

      break;

   }
 }
}
/*****************************/
/*Меню*/
/****************************/
int ras_kl_menu(int kn_fokus,const char *kod_kl,const char *fio,int denrog,int pol,class iceb_u_str *zap_vv,int metka_blok,double *saldo,double *saldo_po_kas)
{

class iceb_gdite_data gdite;
iceb_gdite(&gdite,1,NULL); //открываем окно ждите
iceb_refresh();
double ost_na_saldo=0.;
*saldo_po_kas=0.;
*saldo=ras_sal_kl(kod_kl,&ost_na_saldo,saldo_po_kas,NULL);

class iceb_u_str zagolovok;
zagolovok.plus(gettext("Клиент"));
zagolovok.plus(":");
zagolovok.plus(kod_kl);
zagolovok.plus(" ");
zagolovok.plus(fio);

zagolovok.ps_plus(gettext("Общее сальдо"));
zagolovok.plus(":");
zagolovok.plus(*saldo);

zagolovok.ps_plus(gettext("Наличное сальдо"));
zagolovok.plus(":");
zagolovok.plus(*saldo_po_kas);

zagolovok.ps_plus(gettext("Безналичное сальдо"));
zagolovok.plus(":");
zagolovok.plus(*saldo-*saldo_po_kas);

zagolovok.ps_plus(gettext("Пол"));
zagolovok.plus(":");
if(pol == 0)
 zagolovok.plus(gettext("Мужчина"));
else
 zagolovok.plus(gettext("Женщина"));


if(denrog == 1)
 {
  zagolovok.ps_plus(gettext("У клиента сегодня день рождения"));
 }

if(metka_blok == 1)
  zagolovok.ps_plus(gettext("Внимание !!! Карточка заблокирована !!!"));

iceb_refresh();


gdite.close(); //закрываем окно ждите
















class ras_kl_data data;
char bros[300];

data.window = gtk_window_new( GTK_WINDOW_TOPLEVEL );

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

sprintf(bros,"%s %s",NAME_SYSTEM,gettext("Расчёт клиентов"));
gtk_window_set_title(GTK_WINDOW(data.window),iceb_u_toutf(bros));

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),1);
//gtk_widget_show(data.window); //Отрисовываем, чтобы можно было взять цвет

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(ras_kl_destroy),&data.knopka);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_vihod),&data.knopka);

GtkWidget *vbox=gtk_vbox_new(FALSE,1);
gtk_container_add(GTK_CONTAINER(data.window),vbox);

GtkWidget *label=gtk_label_new(zagolovok.ravno_toutf());
gtk_box_pack_start((GtkBox*)vbox,label,FALSE,FALSE,1);

if(zap_vv->getdlinna() > 1)
 {
  GdkColor color;
  label=gtk_label_new(zap_vv->ravno_toutf());
  gdk_color_parse("red",&color);
  gtk_widget_modify_fg(label,GTK_STATE_NORMAL,&color);
  gtk_box_pack_start((GtkBox*)vbox,label,FALSE,FALSE,1);
 }
 
GtkWidget *knopka[KOLKNOP];


knopka[VOZVRAT_KAR]=gtk_button_new_with_label(gettext("Возврат карточки клиентом"));
gtk_box_pack_start((GtkBox*)vbox,knopka[VOZVRAT_KAR],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(knopka[VOZVRAT_KAR]),"clicked",GTK_SIGNAL_FUNC(ras_kl_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[VOZVRAT_KAR]),(gpointer)VOZVRAT_KAR);

knopka[VOZVRAT_SUM]=gtk_button_new_with_label(gettext("Возвратить деньги из кассы"));
gtk_box_pack_start((GtkBox*)vbox,knopka[VOZVRAT_SUM],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(knopka[VOZVRAT_SUM]),"clicked",GTK_SIGNAL_FUNC(ras_kl_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[VOZVRAT_SUM]),(gpointer)VOZVRAT_SUM);

knopka[OPLATA]=gtk_button_new_with_label(gettext("Оплата"));
gtk_box_pack_start((GtkBox*)vbox,knopka[OPLATA],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(knopka[OPLATA]),"clicked",GTK_SIGNAL_FUNC(ras_kl_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[OPLATA]),(gpointer)OPLATA);

knopka[VID_KART]=gtk_button_new_with_label(gettext("Выдача карточки клиенту"));
gtk_box_pack_start((GtkBox*)vbox,knopka[VID_KART],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(knopka[VID_KART]),"clicked",GTK_SIGNAL_FUNC(ras_kl_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[VID_KART]),(gpointer)VID_KART);

knopka[PROS_SHETS]=gtk_button_new_with_label(gettext("Просмотр счетов по клиенту"));
gtk_box_pack_start((GtkBox*)vbox,knopka[PROS_SHETS],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(knopka[PROS_SHETS]),"clicked",GTK_SIGNAL_FUNC(ras_kl_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[PROS_SHETS]),(gpointer)PROS_SHETS);

knopka[PROV_VV]=gtk_button_new_with_label(gettext("Проверка возможности входа/выхода в подразделение"));
gtk_box_pack_start((GtkBox*)vbox,knopka[PROV_VV],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(knopka[PROV_VV]),"clicked",GTK_SIGNAL_FUNC(ras_kl_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[PROV_VV]),(gpointer)PROV_VV);

knopka[PROS_VV]=gtk_button_new_with_label(gettext("Просмотр движения по карточке"));
gtk_box_pack_start((GtkBox*)vbox,knopka[PROS_VV],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(knopka[PROS_VV]),"clicked",GTK_SIGNAL_FUNC(ras_kl_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[PROS_VV]),(gpointer)PROS_VV);


knopka[VIHOD]=gtk_button_new_with_label(gettext("Выход"));
gtk_box_pack_start((GtkBox*)vbox,knopka[VIHOD],TRUE,TRUE,1);
gtk_signal_connect(GTK_OBJECT(knopka[VIHOD]),"clicked",GTK_SIGNAL_FUNC(ras_kl_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[VIHOD]),(gpointer)VIHOD);

gtk_widget_grab_focus(knopka[kn_fokus]);

gtk_widget_show_all( data.window );

gtk_main();


return(data.knopka);

}
/***************************/
/*Обработчик нажатия кнопок*/
/***************************/

void   ras_kl_knopka(GtkWidget *widget,class ras_kl_data *data)
{


int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

data->knopka=(int)knop;

gtk_widget_destroy(data->window);


}
/*************************************/
/*Обработчик  сигнала destroy_event*/
/************************************/
gboolean ras_kl_destroy(GtkWidget *widget,GdkEvent *event,int *knopka)
{

*knopka=-1;

gtk_widget_destroy(widget);
return(FALSE);
}

/***************************/
/*Возврат карточки клиентом*/
/***************************/
void ras_kl_vk(const char *kod_kl,double saldo,double saldo_po_kas)
{
char strsql[500];

if(saldo < -0.0099 )
 {
  sprintf(strsql,"%s\n%s",gettext("Внимание ! Отрицательное сальдо !"),
  gettext("Невозможно вернуть карточку !"));
  iceb_menu_soob(strsql,NULL);
  return;        
 }
/* Долго считает при большом количестве записей
      saldo_kas=ras_sal_kas(nomer_kas,NULL);            
      if(saldo_kas-saldo_po_kas < -0.009)
       {
        sprintf(strsql,"%s\n%.2f < %.2f",gettext("Остаток по кассе меньше чем сумма к возврату !"),
        saldo_kas,saldo_po_kas);
        iceb_menu_soob(strsql,NULL);
        continue;
       }
*/
if(saldo_po_kas > 0.009)
 {
  sprintf(strsql,"%s:%.2f\n%s",
  gettext("Сумма к возврату"),
  saldo_po_kas,
  gettext("Деньги клиенту возвращены ?"));
  
  if(iceb_menu_danet(strsql,2,NULL) == 2)
   return;
  int kod_voz;
  if((kod_voz=zap_v_kas(nomer_kas,kod_kl,saldo_po_kas*-1.,0,"",0,0,0,0,kod_operatora,gettext("Возврат карточки клиентом"))) != 0)
   {
    sprintf(strsql,"Ошибка записи %d !",kod_voz);
    iceb_menu_soob(strsql,NULL);
    return; 
   }             
 }
time_t vrem;    
time(&vrem);
struct tm *bf;
bf=localtime(&vrem);

sprintf(strsql,"update Taxiklient set \
ktoi=%d,\
vrem=%ld,\
dvk='%04d-%02d-%02d %02d:%02d:%02d' \
where kod='%s'",
iceb_getuid(NULL),
vrem,
bf->tm_year+1900,
bf->tm_mon+1,
bf->tm_mday,
bf->tm_hour,bf->tm_min,bf->tm_sec,
kod_kl);

iceb_sql_zapis(strsql,1,0,NULL);

}



