/*$Id: i_rest.c,v 1.139 2011-02-21 07:36:20 sasa Exp $*/
/*24.05.2010	19.02.2004	Белых А.И.	i_rest.c
Программа выписки заказов в ресторане
*/
#define         DVERSIQ "24.05.2010"
#include        <stdlib.h>
#include        <unistd.h>
#include        <string.h>
#include        <pwd.h>
#include        <locale.h>
#include        <errno.h>
#include        <sys/stat.h>
#include        "i_rest.h"
#include        "rest_oth.h"
#include        "../buhg_g/icebr200.xpm"
#include        "../buhg_g/flag.xpm"


enum
 {
  NOVZAK,
  SPZAK,
  POIDOK,
  NSI,
  OTHET,
  SPISANIE,
  VIHOD,
  KOLKNOP
 };

class i_rest_data
 {
  public:
  GtkWidget *label;
  GtkWidget *window;
  GtkWidget *hrift;
  int       knopka;
 };

gboolean i_rest_destroy(GtkWidget *widget,GdkEvent *event,int *knopka);
void   i_rest_knopka(GtkWidget *widget,class i_rest_data *data);
int glmenu(int);
int  m_poird(iceb_u_str *nomd,short *god,GtkWidget *wpredok);
         
int   rest_oth_v(class rest_oth_data *);
int   rest_oth_r(class rest_oth_data *);
void    i_rest_nsi(void);
int restnast(void);
int rest_vibor(void);
int  l_vrint(int podr,GtkWidget *wpredok);
void l_vhvi(void);
int l_sptur(GtkWidget*);
void i_rest_spt();
int rest_sptov_m(time_t *vremspis,GtkWidget *wpredok);

extern char	*imabaz;
extern SQL_baza	bd;
extern char     *putnansi;
extern char	*host;
const char		*name_system={NAME_SYSTEM};
const char            *version={VERSION};
extern char	*organ;
extern iceb_u_str shrift_rek_raz;
uid_t           kod_operatora=0;
int		kodpodr=0; //Код подразделения
iceb_u_str      naimpodr;
extern class iceb_get_dev_data config_dev;
int             nomer_kas=0;  //номер кассы в настоящий момент всегда равно нолю так как работа с кассовыми регистраторами не реализована
                              //Эта переменная необходима при записи в таблицу Restkas при списании 
extern iceb_u_str iceb_menu_mv_str1;
extern iceb_u_str iceb_menu_mv_str2;

int main(int argc,char **argv)
{
char		strsql[512];

iceb_start(argc,argv);

if(kodpodr != 0)
 {
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select naik from Restpod where kod=%d",kodpodr);
  if(iceb_sql_readkey(strsql,&row,&cur,NULL) == 1)
   naimpodr.new_plus(row[0]);
 }

if(iceb_nastsys() != 0)
 iceb_exit(1);
if(restnast() != 0)
 iceb_exit(1);
if(iceb_perzap(1,NULL))
 iceb_exit(1);

iceb_menu_mv_str1.new_plus(organ);
iceb_menu_mv_str2.new_plus(gettext("Выписка счетов"));

//Читаем в настройках имя компъютера и имя порта
//читать нужно до появления первого меню

//sprintf(strsql,"%s/config_dev.alx",putnansi);
if(iceb_get_dev("config_dev.alx") == 0)
 {
  //Соединение с сервером чтения карточек
 iceb_connect_dserver(config_dev.ima_komp.ravno(),config_dev.name_dev_card.ravno());
//  if(iceb_connect_dserver(config_dev.ima_komp.ravno(),config_dev.name_dev_card.ravno()) == 0)
//    iceb_close_dserver();

 }

printf("Имя компъютера=%s\n\
Имя порта=%s\n",config_dev.ima_komp.ravno(),config_dev.name_dev_card.ravno());


if(config_dev.metka_mpr == 1)
 {
  iceb_u_str parol;
  char imaf_parol[512];
//  sprintf(imaf_parol,"%s/parol.alx",putnansi);
  sprintf(imaf_parol,"parol.alx");
  char kodop[512];
  for(;;)
   {
    parol.new_plus("");
    if(iceb_mous_klav(gettext("Введите пароль"),&parol,40,1,1,1,NULL) != 0)
     {
      if(iceb_menu_danet(gettext("Завершить работу с программой ?"),2,NULL) == 2)
        continue;
      break;
     }
    
    if(parol.getdlinna() <= 1)
     {
      char cod[11];
      memset(cod,'\0',sizeof(cod));      

#ifdef   READ_CARD
//     if(iceb_connect_dserver(config_dev.ima_komp.ravno(),config_dev.name_dev_card.ravno()) == 0)
//      {
       read_card(0,cod,NULL);
//       iceb_close_dserver();
//      }
#endif
      printf("код = %s\n",cod);
      parol.new_plus((char*)cod);
      
     }
    if(parol.getdlinna() <= 1)
      continue;
    if(iceb_u_SRAV(",,,",parol.ravno(),0) == 0)
     break;
    memset(kodop,'\0',sizeof(kodop));


    if(iceb_poldan(parol.ravno(),kodop,imaf_parol,NULL) != 0)
     {
      
      iceb_menu_soob(gettext("Не правильно введен пароль !"),NULL);
      continue;
     }
    
    kod_operatora=atoi(kodop);
    struct  passwd  *ktoz; /*Кто записал*/
    if(kod_operatora == 0)
     {
      iceb_menu_soob(gettext("Нулевой код оператора!"),NULL);
      continue;
     }
    if((ktoz=getpwuid(kod_operatora)) == NULL)
     {
      sprintf(strsql,"%d %s",kod_operatora,gettext("Неизвестный логин"));
      iceb_menu_soob(strsql,NULL);
      continue;
     }        
    rest_vibor();
   }
 }
else  
 {
  kod_operatora=getuid();
  rest_vibor();
 }

iceb_close_dserver();

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );

}
/****************************************/
/*Выбор пункта меню*/
/**************************/

int rest_vibor()
{
iceb_u_str nomdok;
short god;
int kom=0;
rest_oth_data rek_oth;


while( kom >= 0 && kom < KOLKNOP-1 )
 {
  kom=glmenu(kom);
  switch(kom)
   {

    case  NOVZAK:
      
      god=0;
      nomdok.new_plus("");
      
      if(vrshet_v(&god,&nomdok,NULL) == 0)
       vip_shet(god,&nomdok,NULL);
      break;

    case SPZAK:
      l_srestdok(NULL);
      break;

    case NSI:
      i_rest_nsi();
      break;


    case POIDOK:
      if(m_poird(&nomdok,&god,NULL) == 0)
       vip_shet(god,&nomdok,NULL);
      break;


    case OTHET:
      if(rest_oth_v(&rek_oth) == 0)
       {
        printf("Запускаем расчёт\n");
        if(rest_oth_r(&rek_oth) == 0)
         {
          printf("Запускаем меню работы с отчетами\n");
          iceb_rabfil(&rek_oth.imaf,&rek_oth.naim,"",config_dev.metka_klav,NULL);
         }
       }
      break;

    case SPISANIE:
      i_rest_spt();
      break;

  case VIHOD:

      break;

   }

 }
return(0);
}





/***********************************/
/* Главное меню*/
/***********************************/
int glmenu(int knfokus)
{
struct  tm      *bf;
time_t  tmm;
iceb_u_str NADPIS;
char     bros[300];
i_rest_data data;
struct  passwd  *ktor; /*Кто работает*/

//printf("glmenu\n");
int metka_spis_tov=0;
class iceb_u_str spis_tov;
//class iceb_u_str imaf_nast;
//iceb_imafn("restnast.alx",&imaf_nast);
if(iceb_poldan("Список логинов, которым разрешено выполнять списание товаров",&spis_tov,"restnast.alx",NULL) == 0)
 {
  ktor=getpwuid(getuid());
  
  if(iceb_u_proverka(spis_tov.ravno(),ktor->pw_name,0,0) != 0)
   metka_spis_tov=1;
 }
time(&tmm);
bf=localtime(&tmm);


short ddd=bf->tm_mday;
short mmm=bf->tm_mon+1;
short ggg=bf->tm_year+1900;

iceb_infosys(host,VERSION,DVERSIQ,ddd,mmm,ggg,imabaz,&NADPIS,kod_operatora);

data.window = gtk_window_new( GTK_WINDOW_TOPLEVEL );

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

sprintf(bros,"%s %s",NAME_SYSTEM,gettext("Выписка счетов"));
gtk_window_set_title(GTK_WINDOW(data.window),iceb_u_toutf(bros));

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),10);
/******************
gtk_window_set_default_size(GTK_WINDOW(data.window),
    500, //Горизонталь
    500); //вертикаль
*******************/
//gtk_widget_show(data.window); //Отрисовываем, чтобы можно было взять цвет

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(i_rest_destroy),&data.knopka);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_vihod),&data.knopka);

GtkWidget *hbox=gtk_hbox_new(FALSE,0);
GtkWidget *vbox=gtk_vbox_new(FALSE,1);
GtkWidget *vbox1=gtk_vbox_new(FALSE,5);

gtk_container_add(GTK_CONTAINER(data.window),hbox);
gtk_container_add(GTK_CONTAINER(hbox),vbox);
gtk_container_add(GTK_CONTAINER(hbox),vbox1);

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
iceb_u_str stroka;
stroka.plus(organ);
stroka.ps_plus(gettext("Выписка счетов"));
if(kodpodr != 0)
 {
  sprintf(bros,"%s:%d %s",gettext("Подразделение"),kodpodr,naimpodr.ravno());
  stroka.ps_plus(bros);
 }
data.label=gtk_label_new(stroka.ravno_toutf());

//PangoFontDescription *font_pango=pango_font_description_from_string("Sans 16");
//gtk_widget_modify_font(data.label,font_pango);
//pango_font_description_free(font_pango);


gtk_box_pack_start((GtkBox*)vbox,data.label,FALSE,FALSE,0);

//font_pango=pango_font_description_from_string("Sans 14");

GtkWidget *knopka[KOLKNOP];

GtkWidget *labelk=gtk_label_new(gettext("Ввод нового счета"));
//gtk_widget_modify_font(labelk,font_pango);

knopka[NOVZAK]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[NOVZAK]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[NOVZAK],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[NOVZAK]),"clicked",GTK_SIGNAL_FUNC(i_rest_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[NOVZAK]),(gpointer)NOVZAK);


labelk=gtk_label_new(gettext("Просмотр списка счетов"));
//gtk_widget_modify_font(labelk,font_pango);

knopka[SPZAK]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[SPZAK]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[SPZAK],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[SPZAK]),"clicked",GTK_SIGNAL_FUNC(i_rest_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[SPZAK]),(gpointer)SPZAK);

if(config_dev.metka_mpr == 0)
 {
  labelk=gtk_label_new(gettext("Ввод и корректировка нормативно-справочной информации"));
//gtk_widget_modify_font(labelk,font_pango);

  knopka[NSI]=gtk_button_new();
  gtk_container_add(GTK_CONTAINER(knopka[NSI]),labelk);
  gtk_box_pack_start((GtkBox*)vbox,knopka[NSI],TRUE,TRUE,0);
  gtk_signal_connect(GTK_OBJECT(knopka[NSI]),"clicked",GTK_SIGNAL_FUNC(i_rest_knopka),&data);
  gtk_object_set_user_data(GTK_OBJECT(knopka[NSI]),(gpointer)NSI);
 }

labelk=gtk_label_new(gettext("Поиск документов по номеру"));
//gtk_widget_modify_font(labelk,font_pango);

knopka[POIDOK]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[POIDOK]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[POIDOK],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[POIDOK]),"clicked",GTK_SIGNAL_FUNC(i_rest_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[POIDOK]),(gpointer)POIDOK);


labelk=gtk_label_new(gettext("Распечатка отчётов"));
knopka[OTHET]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[OTHET]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[OTHET],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[OTHET]),"clicked",GTK_SIGNAL_FUNC(i_rest_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[OTHET]),(gpointer)OTHET);

labelk=gtk_label_new(gettext("Списание товаров"));
knopka[SPISANIE]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[SPISANIE]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[SPISANIE],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[SPISANIE]),"clicked",GTK_SIGNAL_FUNC(i_rest_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[SPISANIE]),(gpointer)SPISANIE);


labelk=gtk_label_new(gettext("Выход"));
//gtk_widget_modify_font(labelk,font_pango);
knopka[VIHOD]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[VIHOD]),labelk);
//knopka[VIHOD]=gtk_button_new_with_label(iceb_u_toutf("Выход"));
gtk_box_pack_start((GtkBox*)vbox,knopka[VIHOD],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[VIHOD]),"clicked",GTK_SIGNAL_FUNC(i_rest_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[VIHOD]),(gpointer)VIHOD);

GtkWidget *mainvbox1=gtk_vbox_new(FALSE,5);
gtk_widget_realize(data.window);
GtkStyle *style;
GdkPixmap *pixmap;
GdkBitmap *mask;
GtkWidget *pixmapWidget;

style = gtk_widget_get_style(data.window);
pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],
                                    (gchar **)icebr200_xpm);
pixmapWidget=gtk_pixmap_new(pixmap,mask);
gtk_container_add(GTK_CONTAINER(mainvbox1),pixmapWidget);

pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],
                                    (gchar **)flag_xpm);
pixmapWidget=gtk_pixmap_new(pixmap,mask);
gtk_container_add(GTK_CONTAINER(mainvbox1),pixmapWidget);

gtk_container_add(GTK_CONTAINER(vbox1),mainvbox1);

GtkWidget *label_nadpis = gtk_label_new (iceb_u_toutf(NADPIS.ravno()));

gtk_box_pack_start (GTK_BOX (vbox1), label_nadpis, TRUE, TRUE, 0);

if(shrift_rek_raz.getdlinna() > 1)
 {
  PangoFontDescription *font_pango=pango_font_description_from_string(shrift_rek_raz.ravno());
  gtk_widget_modify_font(label_nadpis,font_pango);
  pango_font_description_free(font_pango);
 }
gtk_widget_grab_focus(knopka[knfokus]);
if(metka_spis_tov == 1)
  gtk_widget_set_sensitive(GTK_WIDGET(knopka[SPISANIE]),FALSE);//Недоступна
 
gtk_widget_show_all( data.window );
//pango_font_description_free(font_pango);

gtk_main();


return(data.knopka);

}

/***************************/
/*Обработчик нажатия кнопок*/
/***************************/

void   i_rest_knopka(GtkWidget *widget,class i_rest_data *data)
{

//int knop=GPOINTER_TO_INT(gtk_widget_get_name(widget);
//int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

data->knopka=(int)knop;

gtk_widget_destroy(data->window);


}
/*************************************/
/*Обработчик  сигнала destroy_event*/
/************************************/
gboolean i_rest_destroy(GtkWidget *widget,GdkEvent *event,int *knopka)
{

*knopka=-1;

gtk_widget_destroy(widget);
return(FALSE); 
}
/****************************************/
/*Ввод нормативно-справочной информации*/
/****************************************/

void    i_rest_nsi(void)
{
int nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("НСИ"));


zagolovok.plus(gettext("Ввод и корректировка нормативно-справочной информации"));

punkt_m.plus(gettext("Ввод и корректировка групп клиентов"));//0
punkt_m.plus(gettext("Ввод и корректировка списка клиентов"));//1
punkt_m.plus(gettext("Ввод и корректировка списка подразделений"));//2
punkt_m.plus(gettext("Просмотр списка товаров"));//3
punkt_m.plus(gettext("Просмотр списка услуг"));//4
punkt_m.plus(gettext("Работа с файлом настройки"));//5
punkt_m.plus(gettext("Посмотреть настройки рабочего места"));//6
punkt_m.plus(gettext("Ввод и корректировка стоимости входа в подразделения"));//7
punkt_m.plus(gettext("Ввод и корректировка списка турникетов"));//8
punkt_m.plus(gettext("Просмотр движения людей по подразделениям"));//9
punkt_m.plus(gettext("Выбор шрифта"));//10


while(nomer >= 0)
 {

  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);



  iceb_u_str kod;
  iceb_u_str naikod;
  kod.plus("");
  naikod.plus("");  
  int kodmu;
  int kodskl;
  switch (nomer)
   {
    case -1:
      return;
      
    case 0:
      l_grkl(0,&kod,&naikod,NULL);
      break;
    case 1:
      l_klientr(0,&kod,NULL);
      break;

    case 2:
      rpod_l(0,&kod,&naikod,NULL);
      break;

    case 3:
      l_spisok_v(&kodmu,&kodskl,0,0,NULL);
      break;

    case 4:
      l_spisok_v(&kodmu,&kodskl,1,0,NULL);
      break;

    case 5:
      if(config_dev.metka_klav != 0)
       {
        iceb_menu_soob("Клавиатура отключена !!!",NULL);
        break;       
       }

      iceb_f_redfil("restnast.alx",2,NULL);
      restnast(); 
     break;

    case 6:
     kod.new_plus("Конфигурация рабочего места:\nИдентефикатор рабочего места-");
     if(config_dev.workid_answer != NULL)
       kod.plus(config_dev.workid_answer);
     else     
       kod.plus("Не определен");

     kod.ps_plus("Клавиатура-");
     if(config_dev.metka_klav == 0)
      kod.plus("включена");
     else
      kod.plus("отключена");

     kod.ps_plus("Сенсорный экран-");
     if(config_dev.metka_screen == 0)
      kod.plus("выключен");
     else
      kod.plus("включен");

     kod.ps_plus("Многопользовательская работа-");
     if(config_dev.metka_mpr == 0)
      kod.plus("выключенa");
     else
      kod.plus("включенa");
      
     iceb_menu_soob(&kod,NULL);
     break;

    case 7:
      if(kod_podr_v(&kod,NULL) == 0)
         l_vrint(kod.ravno_atoi(),NULL);
     break;

    case 8:
     l_sptur(NULL);
     break;

    case 9:
     l_vhvi();
     break;

    case 10:
     iceb_font_selection(NULL);
     break;
      
   }
   
 }

}
/**********************/
/*Списание товаров*/
/**********************/
void i_rest_spt()
{
time_t vremsp;
if(rest_sptov_m(&vremsp,NULL) != 0)
 return;
if(iceb_parol(0,NULL) != 0)
 return;
 
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,NULL);
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

char strsql[500];
sprintf(strsql,"select datd,nomd from Restdok where ms=0 and ((vremo != 0 and vremo <= %ld) \
or (mo=1 && vremo=0))",vremsp);
//printf("%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return;
 }
float kolstr1=0.;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  //Списываем записи в документе
  sprintf(strsql,"update Restdok1 set ms=1 where datd='%s' and nomd='%s' and mz=0",row[0],row[1]);  
  iceb_sql_zapis(strsql,1,0,NULL);
  //помечаем документ как списанный  
  sprintf(strsql,"update Restdok set ms=1 where datd='%s' and nomd='%s'",row[0],row[1]);  
  iceb_sql_zapis(strsql,1,0,NULL);
 
 }

gdite.close();

sprintf(strsql,"%s:%d",gettext("Количество документов"),kolstr); 
iceb_menu_soob(strsql,NULL);

}






