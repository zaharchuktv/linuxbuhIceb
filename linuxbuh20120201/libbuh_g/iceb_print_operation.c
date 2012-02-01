/*$Id: iceb_print_operation.c,v 1.4 2011-08-29 07:13:46 sasa Exp $*/
/*21.08.2011	15.05.20120	Белых А.И.	iceb_print_opration.c
Меню выбора принтера и параметров печати
Единица измерения изображения в пунктах 
Пункт = 1/72 дюйма
Дюйм равен 25.4 миллиметра
1 пунк равен 25.4/72=0.352777 миллиметра
В России 1 пункт равен 0.376 миллиметров
Формат бумаги А4 210*297 миллиметров или 596*842 пункта
Начало координат в левом нижнем углу листа

Размер зоны отображения в пунктах:
по вертикали=783.569764 по горизонтали=559.275591

Размер зоны отображения примем для расчёта в милиметрах:
по вертикали 276.426 по горизонтали 197.3

Если сравнивать с iceb_ps.c то там другая зона отображения в милиметрах:
по вертикали 271.7 по горизонтали 197.2

Программа выдаёт размер зоны для печати в пунктах по горизонтали 571.235613 по вертикали 817.329766
Если перевести в милиметры то размер зоны  по горизонтали 571.235613*25.4/72=201.51923042 
                                           по вертикали   817.329766*25.4/72=288.335778561
Эти размеры соотвецтвуют замерам зоны печати на листе померяным с помощью линейки.
Эта зона по размерам больше чем та, что используется фильтром iceb_ps
По вертикали фактическим ограничением является количество строк 
По горизонтали фактически помещатся больше символов чем в iceb_ps - 79 при 10 символах на дюйм
Если распечатка не разбита на листы то по вертикали при стандартном межстрочном расстоянии помещаться 68 строк
Если распечатку выгрузить в файлы формата ps или pdf то там будет другое количество строк по вертикали и
другое количество символов по горизонтали.

Формула получения размера в пунктах:
 Х*72/25.4
 где Х - расстояние в милиметрах

Формула переведения пункты в милиметры
 X*25.4/72
 где Х - расстояние в пунктах

Ширина шрифта в пунктах для режима 10 знаков на дюйм:
 (25.4/10)*72/25.4 или 72/10   десять знаков  

Применяемые размеры EPSON шрифтов
10 знаков на дюйм
17 знаков на дюйм (это 10 знаков в режиме сжатия)
12 знаков на дюйм 
20 знаков на дюйм (это 12 знаков в режиме сжатия)
15 знаков на дюйм 
22 знаков на дюйм (это 15 знаков в режиме сжатия)

Формула вычисления длинны строки в пунктах:

КЗ/КЗД*72

где:
КЗ  - количество знаков
КЗД - количество знаков на дюйм



Стандартное межстрочное расстояние на матричных принтерах 1/6 дюйма

Перечень команд матричных принтеров применяемых для создания отчетов
в системе iceB.
----------------------------------------------------------------------
ASCII |Десятичное|Шес-чное|           Описание
----------------------------------------------------------------------
ESC     27         1B       Признак команды для принтера
LF      10         0A       Перевод строки
FF      12         0C       Перевод формата
DC2     18         12       Отмена уплотненого режима
DC4     20         14       Отмена двойной ширины (Для одной строки)
SO      14         0E       Выбор двойной ширины (для 1 строки)
SI      15         0F       Выбор уплотнённого режима печати
ESC SI  15         0F       Выбор уплотнённого режима печати
ESC - n 45         2D       Включение/выключение подчеркивания
                            Если n = 0 режим выключен \x30
                                     1 режим включен  \x31
ESC 2   50         32       Выбор 1/6-дюймового интервала между строками
ESC 3 n 51         33       Выбор n/216-дюймового интервала между строками
ESC A n 65         41       Выбор n/72-дюймового интервала между строками
ESC E   69         45       Выбор режима выделенного шрифта
ESC F   70         46       Отмена режима выделенного шрифта
ESC G   71         47       Выбор двухударного режима
ESC H   72         48       Отмена двухударного режима
ESC M   77         4D       Двенадцать знаков на дюйм
ESC P   80         50       Десять знаков на дюйм
ESC g  103         6B       Пятнадцать знаков на дюйм (отрабатывается не всеми принтерами)
ESC U n 85         55       Включение/выключение однонаправленного режима
                            Если n = 0 режим выключен \x30
                                     1 режим включен  \x31
ESC l   108        6C       Установка левого поля
ESC x n 120        78       Выбор режимов высококачественной или черновой 
                            печати. Если n = 0 - Выбор чернового режима
                                             1 - Выбор высококачественной
                                                 печати
----------------------------------------------------------------------------
Примеры использования команд в тексте программы
fprintf(ff,"\x12");  //отмена ужатого режима
fprintf(ff,"\x1b\x6C%c",10-koolk); //Установка левого поля
fprintf(ff,"\x1B\x4D"); //12-знаков
fprintf(ff,"\x0F");  //Ужатый режим
fprintf(ff,"\x1B\x33%c\n",30); //Уменьшаем межстрочный интервал
*/
#include <errno.h>
#include <math.h>
#include "iceb_libbuh.h"
//#include <iceb_libbuh.h>
#include	<ctype.h>


static GtkPrintSettings *print_settings = NULL;

const double PEREVOD_V_PUNKT=72./25.4;
//const double lev_zona=6*PEREVOD_V_PUNKT; /*Левая мёртвая зона в пунктах 6mm- померял на бумаге*/
const double lev_zona=0.; 

/*Ширина символов в пунктах для всех режимов печати*/
const double hir_simv_10 = 25.4/10.*PEREVOD_V_PUNKT; /*ширина символа в пунктах в режиме 10 знаков на дюйм*/
const double hir_simv_10u= 25.4/17.*PEREVOD_V_PUNKT; /*ширина символа в пунктах в режиме 10 знаков на дюйм с ужатием*/
const double hir_simv_12 = 25.4/12.*PEREVOD_V_PUNKT; /*ширина символа в пунктах в режиме 12 знаков на дюйм*/
const double hir_simv_12u= 25.4/20.*PEREVOD_V_PUNKT; /*ширина символа в пунктах в режиме 12 знаков на дюйм с ужатием*/
const double hir_simv_15 = 25.4/15.*PEREVOD_V_PUNKT; /*ширина символа в пунктах в режиме 15 знаков на дюйм*/
const double hir_simv_15u= 25.4/22.*PEREVOD_V_PUNKT; /*ширина символа в пунктах в режиме 15 знаков на дюйм с ужатием*/


/*Мастштабы*/
const double mashtab_10=12./10.; /*масштаб для 10 символов на дюйм*/
const double mashtab_12=1.;      /*масштаб для 12 символов на дюйм*/
const double mashtab_15=12./15.; /*масштаб для 15 символов на дюйм*/
const double mashtab_10u=12./17.; /*масштаб для 10 символов на дюйм в ужатом режиме*/
const double mashtab_12u=12./20.; /*масштаб для 12 символов на дюйм в ужатом режиме*/
const double mashtab_15u=12./22.; /*масштаб для 15 символов на дюйм в ужатом режиме (не точно)*/

class iceb_print_operation_data
{
 public:
   class iceb_u_str filename;
   gdouble font_size;
   gdouble rlist_gor; /*Размер доступной зоны для печати в пунктах по горизонтали*/    
   gdouble rlist_ver; /*Размер доступной зоны для печати в пунктах по вертикали*/    
   short metka_orient; /*0-ориентация портрет 1-ландшафт*/
   gdouble tek_hag_ver; /*Текущий размер шага по вертикали в пунктах*/
   int kolih_str_file; /* Количество строк в файле*/
   int nomer_str_tek; /*Номер текущей строки*/
   class iceb_u_spisok lines; /*Массив в который записан файл который нужно распечатать*/
   char *stroka_in; /*Копия строки из файла большего размера чем исходная так как изза табулостопов может быть больше */
   char *stroka_out; /*Строка которую выводим на печать*/
   class iceb_u_int poz_v_str; /*массив с позициями в строке для печати строки с места обрыва строки, которая не поместилась на лист*/
   unsigned dlina_stroka_inout; /*Размер самой длинной строки*/
   gint kolih_pages; /*Общее количество страниц*/
   unsigned int kolsv;	//Количество символов в строке вывода

   short metka_shrift; //0-10 знаков на дюйм 1- 12 знаков на дюйм 2- 15 знаков
   short metka_shrift_u; //0-нормальный шрифт 1-уплотнённый
   double tek_hir_simv; /*Текущая ширина символа в пунктах*/
   double tek_otstup; /*Текущий отступ от левого края листа в пунктах - применяется для вычисления длинны строки*/
   double tek_otstup_m; /*Текущий отступ от левого края в пунктах с учётом масштаба - применяется для правильного перехода на начало следующей строки*/
   short metka_hl; /*Если больше 0 то лист не поместился по горизонтали и то что не поместилось нужно печатать на следующем листе*/
   double mashtab_po_gor; /*Этот масштаб применяется для установки текущего масштаба в начале формирования листа*/
   double start_y; /*начальная позиция по вертикали на листе в пунктах*/
   PangoWeight bold_on_off; /*Включение/выключение выделения текста*/
   FILE *ff_prot;
   short metka_underline; /* 0-без подчётркивания 1-с подчёркиванием*/
   PangoAttrList * attrs;

   iceb_print_operation_data() /*Конструктор*/
    {
     font_size=10.0; /*Должно быть согласовано с масштабами*/
     kolih_str_file=0;
     kolih_pages=0;
     stroka_in=NULL;
     stroka_out=NULL;
     dlina_stroka_inout=0;
     ff_prot=NULL;
     start_y=0; /*Именно здесь иначе обнулится перед печатью листов*/
     
     start_set(); /*выставляем начальные значения переменных*/

    }

   ~iceb_print_operation_data() /*Деструктор*/
    {
     if(stroka_out != NULL)
      delete [] stroka_out;  
     if(stroka_in != NULL)
      delete [] stroka_in;  

    }

   void start_set() 
    {
     kolsv=0; /*позиция в строке вывода*/
     nomer_str_tek=0; /*номер строки текущий*/
     tek_hag_ver=25.4/6.* PEREVOD_V_PUNKT; /*1/6 дюйма шаг по вертикали по умолчанию*/

     metka_shrift=0; //0-10 знаков на дюйм 1- 12 знаков на дюйм 2- 15 знаков
     metka_shrift_u=0; //0-нормальный шрифт 1-уплотнённый
     metka_hl=0;     
     tek_hir_simv=hir_simv_10; /*текущая ширина символа в пунктах*/
     tek_otstup=0.; /*Текущий отступ в пунктах*/
     tek_otstup_m=0.; /*Текущий отступ в пунктах*/
     mashtab_po_gor=mashtab_10;
     bold_on_off=PANGO_WEIGHT_ULTRALIGHT;
     metka_underline=0;
    }
};

/***************************************/
            
static void begin_print (GtkPrintOperation *operation,GtkPrintContext *context,iceb_print_operation_data *data)
{
//char *contents;
double height;
int kolih_bait;
char stroka[1024000];
if(data->ff_prot != NULL)
 fprintf(data->ff_prot,"Расчёт количества листов\n---------------------------------------\n");

/***************
typedef enum {
  GTK_UNIT_PIXEL,
  GTK_UNIT_POINTS,
  GTK_UNIT_INCH,
  GTK_UNIT_MM
} GtkUnit;
*****************/

GtkPageSetup* pageSetup =gtk_print_context_get_page_setup(context);
/*получаем размер в пунктах*/
height = gtk_page_setup_get_page_height(pageSetup,GTK_UNIT_POINTS);
//height = gtk_print_context_get_height (context);


//printf("height=%f\n",gtk_print_context_get_height (context));                                      
/*******************8                                          
g_file_get_contents (data->filename.ravno(), &contents, NULL, NULL);
                                            
data->lines = g_strsplit (contents, "\n", 0);
g_free (contents);
*********************/

FILE *ff;
if((ff = fopen(data->filename.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(data->filename.ravno(),"",errno,NULL);
  return;
 }
unsigned int razmer_str=0;
int metka_ps=0;
while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  /*удаляем пробелы в конце строки иначе будет вычислятся лишние листы если строка не поместится по горизонтали*/
  kolih_bait=razmer_str=strlen(stroka);

  metka_ps=0;  

  kolih_bait--;
  if(stroka[kolih_bait] == '\n')
   {
    metka_ps=1;
    kolih_bait--;
    if(stroka[kolih_bait] == '\r')
     {
      stroka[kolih_bait]='\n';
      stroka[kolih_bait+1]='\0';
      kolih_bait--;
     }
   }

  for( ;kolih_bait > 0;--kolih_bait)
   {
    if(stroka[kolih_bait] != ' ')
     break;
    if(metka_ps == 0)
     stroka[kolih_bait]='\0';
    else
     {
      stroka[kolih_bait]='\n';
      stroka[kolih_bait+1]='\0';
     }        
   }   

  data->lines.plus(stroka);

  if(razmer_str > data->dlina_stroka_inout)
   data->dlina_stroka_inout=razmer_str;
 }
fclose(ff);
data->kolih_str_file=data->lines.kolih();
if(data->lines.ravno(0)[0] == 27 && data->lines.ravno(0)[1] == 50 && data->lines.ravno(0)[2] == 27 && data->lines.ravno(0)[3] == 50)
 {
  /*Ландшафтная ориетация*/
  data->start_y=3.; /*чтобы поместилось 47 строк смещаем стандартное начало*/
//  data->rlist_gor = gtk_print_context_get_height (context);
//  data->rlist_ver = gtk_print_context_get_width (context)+data->start_y;
  data->rlist_gor = gtk_page_setup_get_page_height(pageSetup,GTK_UNIT_POINTS);
  data->rlist_ver = gtk_page_setup_get_page_width(pageSetup,GTK_UNIT_POINTS);
  data->metka_orient=1;
 }
else                                                
 {
  /*портретная ориентация*/
//  data->rlist_ver = gtk_print_context_get_height (context);
//  data->rlist_gor = gtk_print_context_get_width (context);
  data->rlist_ver = gtk_page_setup_get_page_height(pageSetup,GTK_UNIT_POINTS);
  data->rlist_gor = gtk_page_setup_get_page_width(pageSetup,GTK_UNIT_POINTS);
  data->metka_orient=0;
 }

if(data->ff_prot != NULL)
 fprintf(data->ff_prot,"Размер листа по горизонтали=%f по вертикали=%f\n",data->rlist_gor,data->rlist_ver);

printf("%s-Размер листа по горизонтали=%f по вертикали=%f\n",__FUNCTION__,data->rlist_gor,data->rlist_ver);
 
/*Создаём массив для запоминания позиции обрыва в строке*/
data->poz_v_str.make_class(data->kolih_str_file);

if(data->dlina_stroka_inout > 0)
 {
  data->dlina_stroka_inout*=2; 
  data->stroka_out=new char[data->dlina_stroka_inout];
  data->stroka_in=new char[data->dlina_stroka_inout];
 }                                                              

/************************
Необходимо вычислить количество листов учитывая
изменение межстрочного расстояния
команд на переход на новый лист
строк, которые вылезають за границы листа
**********************/




char *strtmp=NULL;
int kolpb=0;
short metka_2hir=0;
double razmer_X=0.;
//double tek_poz_ver=data->tek_hag_ver-data->start_y; /*Текущая позиция по вертикали в пунктах */
double tek_poz_ver=0.-data->start_y; /*Текущая позиция по вертикали в пунктах */
short metka_hl=0;
double mashtab_ots=1./data->mashtab_po_gor;
/*код перевода строки может содержаться в двубайтных командных последовательностях
в этом случае не должна выводиться строка и выполнятся перевод на следуюющюю
строку Это нужно делать только если метка равна 1
*/
metka_ps=0; /*Метка выставляестя если встретился перевод строки или строка не поместилась по ширине*/
int kolih_str_real=0; /*реальное количество строк*/
int nomer_str_tek_zap=0;
short metka_pages=0; /*Если было увеличение количества листов после подсчёта строк то если сразу после этого идет команда на переход на новый лист она должна игнорироваться*/
while(data->nomer_str_tek < data->kolih_str_file) 
 {
  mashtab_ots=1./data->mashtab_po_gor;
  tek_poz_ver=0.-data->start_y; /*Текущая позиция по вертикали в пунктах */
  if(data->ff_prot != NULL)
    fprintf(data->ff_prot,"\nНомер листа=%d nomer_str_tek=%d\n",data->kolih_pages,data->nomer_str_tek);
  metka_hl=0;
  nomer_str_tek_zap=data->nomer_str_tek;
  metka_ps=0;
  for ( ; tek_poz_ver+data->tek_hag_ver <= data->rlist_ver && data->nomer_str_tek < data->kolih_str_file; data->nomer_str_tek++) 
   {
    if(data->ff_prot != NULL)
     {
      fprintf(data->ff_prot,"Номер строки=%d начальная позиция в строке=%d tek_poz_ver=%f %f tek_hag_ver=%f\n",
      data->nomer_str_tek,data->poz_v_str.ravno(data->nomer_str_tek),tek_poz_ver,data->rlist_ver,data->tek_hag_ver);
      
      fprintf(data->ff_prot,"%s\n", &data->lines.ravno(data->nomer_str_tek)[data->poz_v_str.ravno(data->nomer_str_tek)]);
     }
    int dlinastr=strlen(data->lines.ravno(data->nomer_str_tek));
    if(data->poz_v_str.ravno(data->nomer_str_tek) >= dlinastr) /*строка выведена на предыдущем листе*/
     {
      tek_poz_ver+=data->tek_hag_ver;
      continue;
     }
    
    memset(data->stroka_in,'\0',data->dlina_stroka_inout);
    /*Копируем в строку заведомо большего размера изза табулостопов*/
    strcpy(data->stroka_in,data->lines.ravno(data->nomer_str_tek));
    memset(data->stroka_out,'\0',data->dlina_stroka_inout);
    int kod_simv=0;
    data->kolsv=0;

     /*Если начало строки то в расчёт берётся отступ*/
    if(data->poz_v_str.ravno(data->nomer_str_tek) == 0)
     razmer_X=data->tek_otstup;
    else
     razmer_X=0.;

    if(data->ff_prot != NULL)
     fprintf(data->ff_prot,"Начало строки %d razmer_X=%f data->tek_hir_simv=%f\n",data->nomer_str_tek,razmer_X,data->tek_hir_simv);
/************
    if(data->ff_prot != NULL)
     fprintf(data->ff_prot,"\nСтрока %d\n",data->nomer_str_tek);
************/
    for(int shet=data->poz_v_str.ravno(data->nomer_str_tek); shet < dlinastr; shet++)
     if(iscntrl(kod_simv=data->stroka_in[shet])) //Проверка на управляющий символ
     {
//      if(data->ff_prot != NULL)
//       fprintf(data->ff_prot,"Смещение по горизонтали=%f/%f\n",razmer_X,data->rlist_gor);
      switch(kod_simv)
       {
       /*
        *   ESC-последовательности
        */
        case 27: //Esc

         shet++;
         switch (data->stroka_in[shet])
          {
            case 15: /*Включение уплотнённого режима печати*/
              goto kom15;
              
            case '-': //Включить/выключить режим подчеркивания

               shet++; //Увеличиваем счётчик потому, что нужен следующий элемент строки

               continue;

            case '0':		/* межстрочный интервал 1/8 inch */
               data->tek_hag_ver=25.4/8.*PEREVOD_V_PUNKT;
               continue;

            case '2': //Межстрочный интервал 1/6 дюйма
               data->tek_hag_ver=25.4/6.*PEREVOD_V_PUNKT;
              continue;

            case '3': //Межстрочный интервал n/216 дюйма
               shet++; //Увеличиваем счётчик потому, что нужен следующий элемент строки
               data->tek_hag_ver=data->stroka_in[shet]*25.4/216.*PEREVOD_V_PUNKT;
               continue;

            case 'A': //Межстрочный интервал n/72 дюйма

               shet++;  //Увеличиваем счётчик потому, что нужен следующий элемент строки
               data->tek_hag_ver=data->stroka_in[shet]*25.4/72.*PEREVOD_V_PUNKT;
               continue;

            case 'G': //Двуударный режим печати
            case 'E': //Выделенный шрифт
              continue;

            case 'H': //Отмена двуударного режима печати
            case 'F': //Отмена выделенного шрифта
              continue;

            case 'P': //Десять знаков на дюйм
               data->metka_shrift=0;
               if(data->metka_shrift_u == 0)
                {
                 data->tek_hir_simv=hir_simv_10;
                }
               if(data->metka_shrift_u == 1)
                {
                 data->tek_hir_simv=hir_simv_10u;
                }
               continue;

            case 'M': //Двенадцать знаков на дюйм

               data->metka_shrift=1;
               if(data->metka_shrift_u == 0)
                {
                 data->tek_hir_simv=hir_simv_12;
                }
               if(data->metka_shrift_u == 1)
                {
                 data->tek_hir_simv=hir_simv_12u;
                }
               continue;

            case 'g': //Пятнадцать знаков на дюйм (применяестся не на всех принтерах)

               data->metka_shrift=2;
               if(data->metka_shrift_u == 0)
                {
                 data->tek_hir_simv=hir_simv_15;
                }
               if(data->metka_shrift_u == 1)
                {
                 data->tek_hir_simv=hir_simv_15u;
                }
               continue;

                   
            case 'l': //Установка левого поля

              shet++; //Увеличиваем счётчик потому, что нужен следующий элемент строки
              
              data->tek_otstup=(data->tek_hir_simv*data->stroka_in[shet]-lev_zona);

              if(data->tek_otstup < 0.)
               data->tek_otstup=0.;
               
              if(razmer_X == 0)
               {
                razmer_X=data->tek_otstup;
               }
              continue;
                   
            /* Нереализованные трехсимвольные команды*/
            case 'U': //Включение/выключение однонаправленного режима печати
            case 'x': //Включение/выключение высококачественного режима печати
            case 'S': //Выбор режима печати индексов
            case 'W': //Включение/выключение  режима печати расширенными символами
            case 'a': //Выравнивание текста по правую или левую границу печати
            case 'J': //Перевод бумаги в прямом направлении на заданное количество елементрарных шагов 
            case 'j': //Перевод бумаги в обратном направлении на заданное количество елементрарных шагов
              shet++;  //Трех-символьная команда игнорируем третий символ
              continue;
                   
            case '@': //Инициализация принтера
               //счётчик не увеличиваем-двубайтная последовательность
               continue;

                   
            default: //Игнорируются неизвестные двубайтные последовательности
              continue;
          }
     
       /*
        *   1-байтные управляющие символы
        */
       case 14: //^N - Двойная ширина символа для одной строки
         if(metka_2hir == 0)
           data->tek_hir_simv*=2.;
         metka_2hir=1;
         continue;


       case 15: //^O - Включить уплотненный режим печати
  kom15:;

         data->metka_shrift_u=1;
         if(data->metka_shrift == 0) /*10-знаков*/
          {
           data->tek_hir_simv=hir_simv_10u;
          }
         if(data->metka_shrift == 1) /*12-знаков*/
          {
           data->tek_hir_simv=hir_simv_12u;
          }
         if(data->metka_shrift == 2) /*15 знаков*/
          {
           data->tek_hir_simv=hir_simv_15u;
          }
         continue;
     
       case 18: //^R - Выключить уплотненный режим печати
         data->metka_shrift_u=0;
         if(data->metka_shrift == 0) /*10-знаков*/
          {
           data->tek_hir_simv=hir_simv_10;
          }
         if(data->metka_shrift == 1) /*12-знаков*/
          {
           data->tek_hir_simv=hir_simv_12;
          }
         if(data->metka_shrift == 2) /*15 знаков*/
          {
           data->tek_hir_simv=hir_simv_15;
          }
         continue;

       case 20: //^T - Отмена двойной ширины символа для одной строки
         if(metka_2hir == 1)
           data->tek_hir_simv/=2.;
         metka_2hir=0;
         continue;
     
       case '\n':  //Перевод строки. 
         metka_ps=1; /*выводить строку и делать переход на следующюю только если есть эта метка*/

         if(data->ff_prot != NULL)
            fprintf(data->ff_prot,"Перевод строки-позиция=%d номер строки=%d razmer_X=%f\n",shet,data->nomer_str_tek,razmer_X);
          
         break;

       case '\r':  /*возврат каретки игнорируем*/
         continue;

       case '\f':  //Перевод формата
         if(metka_pages == 0)
          {
           data->kolih_pages++;
           if(data->ff_prot != NULL)
            fprintf(data->ff_prot,"Команда на новый лист %d shet=%d data->nomer_str_tek=%d\n",data->kolih_pages,shet,data->nomer_str_tek);
           data->poz_v_str.new_plus(shet+1,data->nomer_str_tek);
           goto end;
          }         
         continue;
       case '\t':  //Табулятор
         //Тупо заменять табулостопы в строке нельзя так как в ЕSC командах с 
         //с параметрами они могут быть в роли параметра

         strtmp=new char[data->dlina_stroka_inout];
         
         memset(strtmp,'\0',data->dlina_stroka_inout);
         strcpy(strtmp,&data->stroka_in[shet+1]);

         
         kolpb = 8 - (iceb_u_strlen(data->stroka_out) % 8);	// кол-во пробелов до следующего табулостопа 	
         dlinastr+=kolpb;
         char bros[20];
         memset(bros,'\0',sizeof(bros));
         memset(bros,' ',kolpb);

         //чистим концовку строки
         for(unsigned int ii=shet; ii < data->dlina_stroka_inout; ii++)
          data->stroka_in[ii]='\0';

         strcat(data->stroka_in,bros);
         strcat(data->stroka_in,strtmp);
         delete [] strtmp; 
         shet--; //возвращаем счётчик на табулостоп
                
         continue;

        default:
         /* остальные управляющие символы пропускаем */
         continue;
         
       }
     } 
    else 
     {
     /*** 
      ***  Алфавитно-цифровой символ
      ***/
      
      if(razmer_X+data->tek_hir_simv > data->rlist_gor) /*строка не поместилась по горизонтали*/
       {
        metka_hl=1;
        data->poz_v_str.new_plus(shet,data->nomer_str_tek);

        if(data->ff_prot != NULL)
          fprintf(data->ff_prot,"строка %d не поместилась-позиция=%d\n\
razmer_X=%f + data->tek_hir_simv=%f = %f > data->rlist_gor=%f\n",
          data->nomer_str_tek,shet,razmer_X,data->tek_hir_simv,razmer_X+data->tek_hir_simv,data->rlist_gor);

        metka_ps=1;

        break;
       }

      razmer_X+=data->tek_hir_simv;


      if (data->kolsv < data->dlina_stroka_inout) 
       {
        if(g_utf8_get_char_validated (&data->stroka_in[shet],-1) == (gunichar)-1) /*Не utf8 символ*/
         {
          data->stroka_out[data->kolsv++]=data->stroka_in[shet];
         }
        else  /*Символ в кодировке utf8*/
         {
          gchar strprom[16];
          memset(strprom,'\0',sizeof(strprom));        
          g_utf8_strncpy (strprom,&data->stroka_in[shet],1);
          for(unsigned int nom=0; nom < strlen(strprom); nom++)
            data->stroka_out[data->kolsv++]=data->stroka_in[shet++];
          shet--;         
         }
       }

     }


    if(metka_ps == 1) /*Метка выставляестя если встретился перевод строки или строка не поместилась по ширине*/
     {
      if(metka_2hir == 1)
       {
        metka_2hir=0;//^T - Отмена двойной ширины символа для одной строки
        data->tek_hir_simv/=2.;
       }
    
      kolih_str_real++;
      tek_poz_ver+=data->tek_hag_ver;
     }
    if(data->poz_v_str.ravno(data->nomer_str_tek) == 0) /*значит строка поместилась полностью*/
        data->poz_v_str.new_plus(dlinastr,data->nomer_str_tek);
    metka_pages=0;
   }

  data->kolih_pages++;
  metka_pages=1;
  if(data->ff_prot != NULL)
   fprintf(data->ff_prot,"Увеличили счётчик листов %d data->nomer_str_tek=%d\n",data->kolih_pages,data->nomer_str_tek);

end:;

  if(metka_hl == 1) /*вернутся для анализа того что не вошло соответственно увеличится количество листов нужных для печати*/
   {
    if(data->ff_prot != NULL)
     fprintf(data->ff_prot,"metka_hl=%d nomer_str_tek_zap=%d\n",metka_hl,nomer_str_tek_zap);
     data->nomer_str_tek=nomer_str_tek_zap;

   }
 }


if(data->ff_prot != NULL)
 fprintf(data->ff_prot,"Вычисленное количество листов=%d Количество реальных строк=%d\n------------------------------\n",
 data->kolih_pages,kolih_str_real);






data->poz_v_str.clear_class(); /*Обнуляем массив*/

gtk_print_operation_set_n_pages (operation, data->kolih_pages);

data->start_set(); /*выставляем начальные значения переменных*/
}
/******************************/
/*Вывод строки символов       */
/******************************/
void vstr(cairo_t *cr,PangoLayout *layout,class iceb_print_operation_data *data)
{
data->kolsv=0;

if(strlen(data->stroka_out) == 0)
 return;

//char *str = g_strdup_printf("<span foreground=\"#00f\" weight=\"bold\" size=\"x-large\">Image Viewer v.%s</span>",VERSION);
//g_free(str)
//pango_attr_underline_new(PANGO_UNDERLINE_SINGLE);
//sprintf(stroka,"<span underline=\"single\">%s</span>",data->stroka_out);
/****************
  PangoAttrList * attrs = pango_attr_list_new ();

  PangoAttribute * ATTR = NULL;
  ATTR = pango_attr_underline_new (PANGO_UNDERLINE_SINGLE);

  ATTR-> start_index = 0;
  ATTR-> end_index = -1;
  pango_attr_list_change (attrs, ATTR);

    //pango_attr_list_insert (attrs, pango_attr_underline_new(PANGO_UNDERLINE_DOUBLE));

  pango_layout_set_attributes (layout, attrs);
  pango_attr_list_unref (attrs);
********************/
//pango_layout_set_attributes (layout,data->attrs);
pango_layout_set_text (layout, iceb_u_toutf(data->stroka_out), -1);

pango_cairo_show_layout (cr, layout);

int kolih_simv=iceb_u_strlen(data->stroka_out);
if(data->ff_prot != NULL)
 fprintf(data->ff_prot,"%s-%s\nkolih_simv=%d*%f=%f\n",
 __FUNCTION__,data->stroka_out,kolih_simv,data->tek_hir_simv,kolih_simv*data->tek_hir_simv);

/*****************
Сдвигаем текущюю координату по горизонтали для того, чтобы если строка выводится частями
следующая часть строки выводилась после предыдущей. Иначе будет выводится в начале строки поверх уже 
выведенной части.
Ширина символа должна равнятся ширине символа шрифта по умолчанию а это 12 знаков на дюйм
********************/
if(data->metka_underline == 1)
 {
  cairo_rel_move_to (cr,0.,data->tek_hag_ver); /*Переходим на следующюю строку относительное перемещение*/
  cairo_rel_line_to (cr, kolih_simv*(25.4/12.*PEREVOD_V_PUNKT), 0);
                                        
//  cairo_set_source_rgb (cr, 0.8, 0.8, 0.8);
  cairo_set_source_rgb (cr, 0., 0., 0.);
  cairo_fill_preserve (cr);
                                               
  cairo_set_source_rgb (cr, 0, 0, 0);
  cairo_set_line_width (cr, 1);
  cairo_stroke (cr);
 }
else
 cairo_rel_move_to (cr,kolih_simv*(25.4/12.*PEREVOD_V_PUNKT),0.); /*Переходим на следующюю строку относительное перемещение*/

memset(data->stroka_out,'\0',data->dlina_stroka_inout);
}
/*********************************************/


static void draw_page (GtkPrintOperation *operation,GtkPrintContext *context,gint page_nr,
class iceb_print_operation_data *data)
{
//PrintData *data = (PrintData *)user_data;
cairo_t *cr;
PangoLayout *layout;
gdouble width;
PangoFontDescription *desc;
char *strtmp;
int kolpb=0;
double razmer_X=0; /*текущий размер длинны строки в пунктах*/
short metka_hl=0; /*0-текст поместился по горизонтали на лист 1-не поместился*/                               
int nomer_str_tek_zap=data->nomer_str_tek; /*Запоминаем текущий номер строки*/
short metka_2hir=0;
double hir_simv_zap=0.;
double mashtab_ots
=1./data->mashtab_po_gor; /*Масштаб для вычисления размера отступа*/


if(data->ff_prot != NULL)
 fprintf(data->ff_prot,"\nФормирование листа N%d\n=============================================================\n\
data->mashtab_po_gor=%f data->tek_otstup=%f data->tek_otstup_m=%f data->tek_hir_simv=%f\n\
data->start_y=%f mashtab_ots=%f\n",
 page_nr,
 data->mashtab_po_gor,
 data->tek_otstup,
 data->tek_otstup_m,
 data->tek_hir_simv,
 data->start_y,
 mashtab_ots);

cr = gtk_print_context_get_cairo_context (context);
//width = gtk_print_context_get_width (context);
GtkPageSetup* pageSetup =gtk_print_context_get_page_setup(context);
width = gtk_page_setup_get_page_width(pageSetup,GTK_UNIT_POINTS);

/********************** закрасить область рисования

//double height=gtk_print_context_get_height (context);
double height = gtk_page_setup_get_page_height(pageSetup,GTK_UNIT_POINTS);
//printf("%s x=%f y=%f\n",__FUNCTION__,width,height);                                   
//printf("Номер листа=%d\n",page_nr);
cairo_rectangle (cr, 0, 0, width, height);//Рисует прямоугольник
                                      
cairo_set_source_rgb (cr, 0.8, 0.8, 0.8);
cairo_fill_preserve (cr);
                                             
cairo_set_source_rgb (cr, 0, 0, 0);
cairo_set_line_width (cr, 1);
cairo_stroke (cr);
***********************/
if(data->metka_orient == 1)
 {
  cairo_rotate(cr,90./180.*G_PI); //Поворачиваем оси координат на 90 градусов
  cairo_translate(cr,0,width*-1); //Смещаем точку начала координат в правый верхний угол
 }

layout = gtk_print_context_create_pango_layout (context);


desc = pango_font_description_from_string ("mono");

pango_font_description_set_size (desc,data->font_size * PANGO_SCALE);
pango_font_description_set_style(desc,PANGO_STYLE_NORMAL);//PANGO_STYLE_ITALIC
pango_font_description_set_weight (desc,data->bold_on_off); //200
//pango_font_description_set_weight (desc,PANGO_WEIGHT_ULTRALIGHT); //200

//pango_font_description_set_weight(desc,PANGO_WEIGHT_BOLD); //700
//pango_font_description_set_weight (desc,PANGO_WEIGHT_HEAVY);/900

//pango_font_description_set_stretch (desc,  PANGO_STRETCH_CONDENSED); //должно сжимать шрифт но не сжимает- пришлось это делать масштабированием

//pango_attr_underline_new(PANGO_UNDERLINE_SINGLE);

pango_layout_set_font_description (layout, desc);


cairo_scale(cr,data->mashtab_po_gor,1.); //Назначаем масштаб 10 знаков на дюйм (по умолчанию 12)


//double tek_poz_ver=25.4/6.*PEREVOD_V_PUNKT-data->start_y; /*Первая строка всегда с одним и мемже отступом по вертикали */
double tek_poz_ver=0.-data->start_y;

if(data->metka_hl == 0)
  cairo_move_to (cr,data->tek_otstup_m,data->start_y*-1.);
else
  cairo_move_to (cr,0.,data->start_y*-1.);


/***************
gint text_width, text_height;
pango_layout_get_pixel_size (layout, &text_width, &text_height);
printf("text_width=%d text_height=%d\n",text_width, text_height);
******************/
/*код перевода строки может содержаться в двубайтных командных последовательностях
в этом случае не должна выводиться строка и выполнятся перевод на следуюющюю
строку Это нужно делать только если метка равна 1
*/
short metka_ps=0; /*Метка выставляестя если встретился перевод строки или строка не поместилась по ширине*/
int nomer_str_na_stranice=-1;
for ( ; tek_poz_ver+data->tek_hag_ver <= data->rlist_ver && data->nomer_str_tek < data->kolih_str_file; data->nomer_str_tek++) 
 {
  nomer_str_na_stranice++;

  /*Если начало строки то в расчёт берётся отступ*/
  if(data->poz_v_str.ravno(data->nomer_str_tek) == 0)
   razmer_X=data->tek_otstup;
  else
   razmer_X=0.;
  if(data->ff_prot != NULL)
   {
    if(data->ff_prot != NULL)
     fprintf(data->ff_prot,"\n***Номер строки %d начальная позиция=%d\n\
data->mashtab_po_gor=%f data->tek_otstup=%f data->tek_otstup_m=%f data->tek_hir_simv=%f\n\
data->start_y=%f mashtab_ots=%f razmer_X=%f tek_poz_ver=%f\n",
     data->nomer_str_tek,
     data->poz_v_str.ravno(data->nomer_str_tek),
     data->mashtab_po_gor,
     data->tek_otstup,
     data->tek_otstup_m,
     data->tek_hir_simv,
     data->start_y,
     mashtab_ots,
     razmer_X,
     tek_poz_ver);

    
    fprintf(data->ff_prot,"%s\n", &data->lines.ravno(data->nomer_str_tek)[data->poz_v_str.ravno(data->nomer_str_tek)]);
   }
  data->kolsv=0;

  int dlinastr=strlen(data->lines.ravno(data->nomer_str_tek));
  if(data->poz_v_str.ravno(data->nomer_str_tek) >= dlinastr) /*строка выведена на предыдущем листе*/
   {
    tek_poz_ver+=data->tek_hag_ver;
    cairo_rel_move_to (cr, 0, data->tek_hag_ver); /*Переходим на следующюю строку относительное перемещение*/
    continue;
   }
  
  memset(data->stroka_in,'\0',data->dlina_stroka_inout);
  /*Копируем в строку заведомо большего размера изза табулостопов -тоесть длинна строки может увеличится*/
  strcpy(data->stroka_in,data->lines.ravno(data->nomer_str_tek));
  memset(data->stroka_out,'\0',data->dlina_stroka_inout);
  int kod_simv=0;


  metka_ps=0;   


  for(int shet=data->poz_v_str.ravno(data->nomer_str_tek); shet < dlinastr; shet++)
   if(iscntrl(kod_simv=data->stroka_in[shet])) //Проверка на управляющий символ
   {
    switch(kod_simv)
     {
     /*
      *   ESC-последовательности
      */
      case 27: //Esc
       vstr(cr,layout,data); /*вывести содержимое строки до установки нового режима печати*/
       if(data->ff_prot != NULL)
        fprintf(data->ff_prot,"ESC-позиция %d\n",shet);
       shet++;
       switch (data->stroka_in[shet])
        {
          case 15: /*Включение уплотнённого режима печати*/
            goto kom15;
            
          case '-': //Включить/выключить режим подчеркивания

             shet++; //Увеличиваем счётчик потому, что нужен следующий элемент строки

             if(data->stroka_in[shet] == 49) //Включить
              {
               data->metka_underline=1;
               if(data->ff_prot != NULL)
                fprintf(data->ff_prot,"Включить режим подчёркивания-позиция=%d номер строки=%d\n",
                shet,data->nomer_str_tek);
              }

             if(data->stroka_in[shet] == 48) //Выключить
              {
               data->metka_underline=0;
               if(data->ff_prot != NULL)
                fprintf(data->ff_prot,"Выключить режим подчёркивания-позиция=%d номер строки=%d\n",
                shet,data->nomer_str_tek);
              }
             continue;

          case '0':		/* межстрочный интервал 1/8 inch */
             data->tek_hag_ver=25.4/8.*PEREVOD_V_PUNKT;
            if(data->ff_prot != NULL)
             fprintf(data->ff_prot,"Межстрочный интервал 1/8 дюйма-позиция=%d номер строки=%d data->tek_hag_ver=%f\n",
             shet,data->nomer_str_tek,data->tek_hag_ver);
             continue;

          case '2': //Межстрочный интервал 1/6 дюйма
            data->tek_hag_ver=25.4/6.*PEREVOD_V_PUNKT;
            if(data->ff_prot != NULL)
             fprintf(data->ff_prot,"Межстрочный интервал 1/6 дюйма-позиция=%d номер строки=%d data->tek_hag_ver=%f\n",
             shet,data->nomer_str_tek,data->tek_hag_ver);
            continue;

          case '3': //Межстрочный интервал n/216 дюйма
            shet++; //Увеличиваем счётчик потому, что нужен следующий элемент строки
            data->tek_hag_ver=data->stroka_in[shet]*25.4/216.*PEREVOD_V_PUNKT;
            if(data->ff_prot != NULL)
             fprintf(data->ff_prot,"Межстрочный интервал n/216 дюйма-позиция=%d номер строки=%d data->tek_hag_ver=%f\n",
             shet,data->nomer_str_tek,data->tek_hag_ver);
             continue;

          case 'A': //Межстрочный интервал n/72 дюйма

            shet++;  //Увеличиваем счётчик потому, что нужен следующий элемент строки
            data->tek_hag_ver=data->stroka_in[shet]*25.4/72.*PEREVOD_V_PUNKT;
            if(data->ff_prot != NULL)
             fprintf(data->ff_prot,"Межстрочный интервал n/72 дюйма-позиция=%d номер строки=%d data->tek_hag_ver=%f\n",
             shet,data->nomer_str_tek,data->tek_hag_ver);
             continue;

          case 'G': //Двуударный режим печати
          case 'E': //Выделенный шрифт

            vstr(cr,layout,data); //вывести содержимое строки до установки нового режима печати
            if(data->ff_prot != NULL)
             fprintf(data->ff_prot,"Включение двуударного режима(выделенного шрифта)-позиция=%d номер строки=%d razmer_X=%f mashtab_ost=%f\n",
             shet,data->nomer_str_tek,razmer_X,mashtab_ots);

            data->bold_on_off=PANGO_WEIGHT_HEAVY;
            pango_font_description_set_weight (desc,PANGO_WEIGHT_HEAVY);//900
            
            //data->bold_on_off=PANGO_WEIGHT_BOLD;
            //pango_font_description_set_weight(desc,PANGO_WEIGHT_BOLD); //700
            pango_layout_set_font_description (layout, desc);

            continue;

          case 'H': //Отмена двуударного режима печати
          case 'F': //Отмена выделенного шрифта
            vstr(cr,layout,data); //вывести содержимое строки до установки нового режима печати
            pango_font_description_set_weight (desc,PANGO_WEIGHT_ULTRALIGHT); //200
            pango_layout_set_font_description (layout, desc);

            data->bold_on_off=PANGO_WEIGHT_ULTRALIGHT;

            if(data->ff_prot != NULL)
             fprintf(data->ff_prot,"Отмена двуударного режима(выделенного шрифта)-позиция=%d номер строки=%d\n",
             shet,data->nomer_str_tek);
            continue;

          case 'P': //Десять знаков на дюйм
             vstr(cr,layout,data); /*вывести содержимое строки до установки нового режима печати*/
             data->metka_shrift=0;
             hir_simv_zap=data->tek_hir_simv;
             if(data->metka_shrift_u == 0)
              {
               data->mashtab_po_gor=mashtab_10;
               data->tek_hir_simv=hir_simv_10;
              }
             if(data->metka_shrift_u == 1)
              {
               data->mashtab_po_gor=mashtab_10u;
               data->tek_hir_simv=hir_simv_10u;
              }
            mashtab_ots=1./(data->tek_hir_simv/hir_simv_zap);
            if(data->tek_otstup_m > 0.)
             data->tek_otstup_m*=mashtab_ots;
            cairo_scale(cr,data->tek_hir_simv/hir_simv_zap,1.); //Изменяем мастштаб по горизонтали

            if(data->ff_prot != NULL)
             fprintf(data->ff_prot,"Десять знаков на дюйм-позиция=%d номер строки=%d ширина символа=%f\n",
             shet,data->nomer_str_tek,data->tek_hir_simv);
             continue;

          case 'M': //Двенадцать знаков на дюйм

             vstr(cr,layout,data); /*вывести содержимое строки до установки нового режима печати*/
             data->metka_shrift=1;
             hir_simv_zap=data->tek_hir_simv;
                 
             if(data->metka_shrift_u == 0)
              {
               data->mashtab_po_gor=mashtab_12;
               data->tek_hir_simv=hir_simv_12;
              }
             if(data->metka_shrift_u == 1)
              {
               data->mashtab_po_gor=mashtab_12u;
               data->tek_hir_simv=hir_simv_12u;
              }
            mashtab_ots=1./(data->tek_hir_simv/hir_simv_zap);
            if(data->tek_otstup_m > 0.)
             data->tek_otstup_m*=mashtab_ots;
            cairo_scale(cr,data->tek_hir_simv/hir_simv_zap,1.); //Изменяем мастштаб по горизонтали

            if(data->ff_prot != NULL)
             fprintf(data->ff_prot,"\
Двенадцать знаков на дюйм-позиция=%d номер строки=%d data->tek_hir_simv=%f data->tek_otstup_m=%f\n\
%f/%f =mashtab_ots=%f\n",
             shet,data->nomer_str_tek,data->tek_hir_simv,data->tek_otstup_m,data->tek_hir_simv,hir_simv_zap,mashtab_ots);
//            pango_layout_set_width (layout, data->tek_hir_simv);
             continue;

          case 'g': //Пятнадцать знаков на дюйм (применяестся не на всех принтерах)

             vstr(cr,layout,data); /*вывести содержимое строки до установки нового режима печати*/
             hir_simv_zap=data->tek_hir_simv;
             data->metka_shrift=2;
             if(data->metka_shrift_u == 0)
              {
               data->mashtab_po_gor=mashtab_15;
               data->tek_hir_simv=hir_simv_15;
              }
             if(data->metka_shrift_u == 1)
              {
               data->mashtab_po_gor=mashtab_15u;
               data->tek_hir_simv=hir_simv_15u;
              }
            
            mashtab_ots=1./(data->tek_hir_simv/hir_simv_zap);
            if(data->tek_otstup_m > 0.)
             data->tek_otstup_m*=mashtab_ots;
            cairo_scale(cr,data->tek_hir_simv/hir_simv_zap,1.); //Изменяем мастштаб по горизонтали

            if(data->ff_prot != NULL)
             fprintf(data->ff_prot,"Пятнадцать знаков на дюйм-позиция=%d номер строки=%d  ширина символа=%f\n",
             shet,data->nomer_str_tek,data->tek_hir_simv);
             continue;

                 
          case 'l': //Установка левого поля

            if(data->ff_prot != NULL)
             fprintf(data->ff_prot,"Установка отступа-позиция=%d номер строки=%d отступ=%d/tek_otstup_m=%f tek_otstup=%f mashtab_ost=%f lev_zona=%f\n",
             shet,data->nomer_str_tek,data->stroka_in[shet+1],data->tek_otstup_m,data->tek_otstup,mashtab_ots,lev_zona);

            shet++; //Увеличиваем счётчик потому, что нужен следующий элемент строки
            data->tek_otstup=data->tek_hir_simv*data->stroka_in[shet]-lev_zona;

            if(data->tek_otstup < 0.)
             data->tek_otstup=0.;

            data->tek_otstup_m=data->tek_otstup*mashtab_ots;
                      

            if(razmer_X == 0)
             {
              cairo_rel_move_to (cr, data->tek_otstup_m,0); /*Сдвигаемся на отступ по Х*/
              razmer_X=data->tek_otstup;
             }
            if(data->ff_prot != NULL)
             fprintf(data->ff_prot,"Текущий отступ=%f*%d-%f=%f*%f=%f\n",
             data->tek_hir_simv,data->stroka_in[shet],lev_zona,data->tek_otstup,mashtab_ots,data->tek_otstup_m);
            continue;
                 
          /* Нереализованные трехсимвольные команды*/
          case 'U': //Включение/выключение однонаправленного режима печати
          case 'x': //Включение/выключение высококачественного режима печати
          case 'S': //Выбор режима печати индексов
          case 'W': //Включение/выключение  режима печати расширенными символами
          case 'a': //Выравнивание текста по правую или левую границу печати
          case 'J': //Перевод бумаги в прямом направлении на заданное количество елементрарных шагов 
          case 'j': //Перевод бумаги в обратном направлении на заданное количество елементрарных шагов
            if(data->ff_prot != NULL)
             fprintf(data->ff_prot,"Неотрабатываемая трёхбазйтная последовательность позиция=%d пропускаем следующий символ\n",shet);
            shet++;  //Трех-символьная команда игнорируем третий символ
            continue;
                 
          case '@': //Инициализация принтера
             //счётчик не увеличиваем-двубайтная последовательность
            if(data->ff_prot != NULL)
             fprintf(data->ff_prot,"Инициализация принтера -Неотрабатываемая команда позиция=%d\n",shet);
             continue;

                 
          default: //Игнорируются неизвестные двубайтные последовательности
            if(data->ff_prot != NULL)
             fprintf(data->ff_prot,"Неизвестная команда позиция=%d\n",shet);
            continue;
        }
   
     /*
      *   1-байтные управляющие символы
      */
     case 14: //^N - Двойная ширина символа для одной строки

       if(metka_2hir == 0)
        {
         vstr(cr,layout,data); /*вывести содержимое строки до установки нового режима печати*/


         data->mashtab_po_gor*=2.;
         data->tek_hir_simv*=2.;

         mashtab_ots=0.5;

         if(data->tek_otstup_m > 0.)
             data->tek_otstup_m*=mashtab_ots;
         cairo_scale(cr,2.,1.); //Изменяем мастштаб по горизонтали

         if(data->ff_prot != NULL)
          fprintf(data->ff_prot,"Двойная ширина символа для одной строки-позиция=%d номер строки=%d  ширина символа=%f\n",
          shet,data->nomer_str_tek,data->tek_hir_simv);
         }
        metka_2hir=1;

       continue;


     case 15: //^O - Включить уплотненный режим печати
kom15:;

       vstr(cr,layout,data); /*вывести содержимое строки до установки нового режима печати*/

       data->metka_shrift_u=1;
       hir_simv_zap=data->tek_hir_simv;
       if(data->metka_shrift == 0) /*10-знаков*/
        {
         data->mashtab_po_gor=mashtab_10u;
         data->tek_hir_simv=hir_simv_10u;
        }
       if(data->metka_shrift == 1) /*12-знаков*/
        {
         data->mashtab_po_gor=mashtab_12u;
         data->tek_hir_simv=hir_simv_12u;
        }
       if(data->metka_shrift == 2) /*15 знаков*/
        {
         data->mashtab_po_gor=mashtab_15u;
         data->tek_hir_simv=hir_simv_15u;
        }
       if(data->ff_prot != NULL)
         fprintf(data->ff_prot,"Текущий отступ=%f mashtab_ots=%f\n",data->tek_otstup_m,mashtab_ots);
       mashtab_ots=1./(data->tek_hir_simv/hir_simv_zap);
       
       if(data->tek_otstup_m > 0.)
             data->tek_otstup_m*=mashtab_ots;

       cairo_scale(cr,data->tek_hir_simv/hir_simv_zap,1.); //Изменяем мастштаб по горизонтали

       if(data->ff_prot != NULL)
        fprintf(data->ff_prot,"Включить уплотённый режим печати-позиция=%d номер строки=%d  ширина символа=%f mashtab_ost=%f\n",
        shet,data->nomer_str_tek,data->tek_hir_simv,mashtab_ots);


       continue;
   
     case 18: //^R - Выключить уплотненный режим печати

       vstr(cr,layout,data); /*вывести содержимое строки до установки нового режима печати*/

       data->metka_shrift_u=0;
       hir_simv_zap=data->tek_hir_simv;
       if(data->metka_shrift == 0) /*10-знаков*/
        {
         data->mashtab_po_gor=mashtab_10;
         data->tek_hir_simv=hir_simv_10;
        }
       if(data->metka_shrift == 1) /*12-знаков*/
        {
         data->mashtab_po_gor=mashtab_12;
         data->tek_hir_simv=hir_simv_12;
        }
       if(data->metka_shrift == 2) /*15 знаков*/
        {
         data->mashtab_po_gor=mashtab_15;
         data->tek_hir_simv=hir_simv_15;
        }

       mashtab_ots=1./(data->tek_hir_simv/hir_simv_zap);
       if(data->tek_otstup_m > 0.)
             data->tek_otstup_m*=mashtab_ots;
       cairo_scale(cr,data->tek_hir_simv/hir_simv_zap,1.); //Изменяем мастштаб по горизонтали

       if(data->ff_prot != NULL)
        fprintf(data->ff_prot,"Выключить уплотённый режим печати-позиция=%d номер строки=%d  ширина символа=%f\n",
        shet,data->nomer_str_tek,data->tek_hir_simv);

       continue;

     case 20: //^T - Отмена двойной ширины символа для одной строки
       if(metka_2hir == 1)
        {
         vstr(cr,layout,data); /*вывести содержимое строки до установки нового режима печати*/
         metka_2hir=0;
         data->mashtab_po_gor/=2.;
         data->tek_hir_simv/=2.;
         mashtab_ots=2.;
         if(data->tek_otstup_m > 0.)
               data->tek_otstup_m*=mashtab_ots;
         cairo_scale(cr,0.5,1.); //Изменяем мастштаб по горизонтали

         if(data->ff_prot != NULL)
          fprintf(data->ff_prot,"Отмена довойной ширины символа-позиция=%d номер строки=%d ширина символа=%f\n",
          shet,data->nomer_str_tek,data->tek_hir_simv);
        }
       continue;
   
     case '\n':  //Перевод строки. 
       metka_ps=1;
       if(data->ff_prot != NULL)
        fprintf(data->ff_prot,"Перевод строки-позиция=%d номер строки=%d\n",
        shet,data->nomer_str_tek);
       break;       

     case '\r':  /*возврат каретки игнорируем*/
       continue;

     case '\f':  //Перевод формата

      /*Если это первая строка на листе и первая позиция то эта команда не должна отрабатываться 
      так как лист только что начался*/
       if(nomer_str_na_stranice == 0 && shet == 0) 
         continue;

       vstr(cr,layout,data); /*вывести содержимое строки до установки команды на новый лист*/
      
        /*Чтобы сменить лист нужно выйти из подпрограммы и зайти на том же номере строки
          и нереагировать на перевод формата*/

       if(data->ff_prot != NULL)
         fprintf(data->ff_prot,"Новый лист-позиция=%d номер строки=%d\n",
         shet,data->nomer_str_tek);
       vstr(cr,layout,data);
       data->poz_v_str.new_plus(shet+1,data->nomer_str_tek);
       goto end;

     case '\t':  //Табулятор
       //Тупо заменять табулостопы в строке нельзя так как в ЕSC командах с 
       //с параметрами они могут быть в роли параметра
       if(data->ff_prot != NULL)
         fprintf(data->ff_prot,"Табулятор-позиция=%d номер строки=%d\n",
         shet,data->nomer_str_tek);

       strtmp=new char[data->dlina_stroka_inout];
       
       memset(strtmp,'\0',data->dlina_stroka_inout);
       strcpy(strtmp,&data->stroka_in[shet+1]);

       
       kolpb = 8 - (iceb_u_strlen(data->stroka_out) % 8);	// кол-во пробелов до следующего табулостопа 	
       dlinastr+=kolpb;
       char bros[20];
       memset(bros,'\0',sizeof(bros));
       memset(bros,' ',kolpb);

       //чистим концовку строки
       for(unsigned int ii=shet; ii < data->dlina_stroka_inout; ii++)
        data->stroka_in[ii]='\0';

       strcat(data->stroka_in,bros);
       strcat(data->stroka_in,strtmp);
       delete [] strtmp; 
       shet--; //возвращаем счётчик на табулостоп
              
       continue;

      default:
       /* остальные управляющие символы пропускаем */
       continue;
       
     }
   } 
  else 
   {
   /*** 
    ***  Алфавитно-цифровой символ
    ***/

    if(razmer_X+data->tek_hir_simv > data->rlist_gor) /*строка не поместилась по горизонтали*/
     {
      if(data->ff_prot != NULL)
        fprintf(data->ff_prot,"строка не поместилась-позиция=%d номер строки=%d razmer_X=%f+%f=%f > %f\ndata->stroka_out-%s\n",
        shet,data->nomer_str_tek,
        razmer_X,data->tek_hir_simv,
        razmer_X+data->tek_hir_simv,
        data->rlist_gor,
        data->stroka_out);

      vstr(cr,layout,data); //Выводим строку если она есть
      metka_hl=1;
      data->poz_v_str.new_plus(shet,data->nomer_str_tek);
      metka_ps=1;
      break;
     }
    razmer_X+=data->tek_hir_simv;

    if (data->kolsv < data->dlina_stroka_inout) 
     {
      if(g_utf8_get_char_validated (&data->stroka_in[shet],-1) == (gunichar)-1) /*Не utf8 символ*/
       {
//        if(data->ff_prot != NULL)
//         fprintf(data->ff_prot,"Не utf8 cимвол shet=%d data->kolsv=%d\n",shet,data->kolsv);  
        data->stroka_out[data->kolsv++]=data->stroka_in[shet];
       }
      else  /*Символ в кодировке utf8*/
       {
//        if(data->ff_prot != NULL)
//         fprintf(data->ff_prot,"utf8 cимвол shet=%d data->kolsv=%d\n",shet,data->kolsv);  
        gchar strprom[16];
        memset(strprom,'\0',sizeof(strprom));        
        g_utf8_strncpy (strprom,&data->stroka_in[shet],1);
//        if(data->ff_prot != NULL)
//         fprintf(data->ff_prot,"strprom=%s/%d\n",strprom,strlen(strprom));
        for(unsigned int nom=0; nom < strlen(strprom); nom++)
          data->stroka_out[data->kolsv++]=data->stroka_in[shet++];
        shet--;         
       }
     }
   }

  vstr(cr,layout,data); //Выводим строку если она есть
  if(metka_ps == 1)/*Метка выставляестя если встретился перевод строки или строка не поместилась по ширине*/
   {


    if(metka_2hir == 1) //^T - Отмена двойной ширины символа для одной строки
     {
         metka_2hir=0;
         vstr(cr,layout,data);
         data->mashtab_po_gor/=2.;
         data->tek_hir_simv/=2.;
         mashtab_ots=2.;
       if(data->tek_otstup_m > 0.)
             data->tek_otstup_m*=mashtab_ots;
         cairo_scale(cr,0.5,1.); //Изменяем мастштаб по горизонтали

         if(data->ff_prot != NULL)
          fprintf(data->ff_prot,"Отмена двойной ширины символа-ширина символа=%f\n",
          data->tek_hir_simv);
     }

    tek_poz_ver+=data->tek_hag_ver;
    if(data->metka_hl == 0)
     {
      cairo_move_to (cr,data->tek_otstup_m,tek_poz_ver); /*Переход в точку*/
      
      if(data->ff_prot != NULL)
       fprintf(data->ff_prot,"Переход на начало следующей строки- по x=%f по у=%f data->tek_har_ver=%f %f(гор=%f вер=%f)\n",
       data->tek_otstup_m,tek_poz_ver,data->tek_hag_ver,tek_poz_ver+data->tek_hag_ver,data->rlist_gor,data->rlist_ver);
     }
    else
     {
   //   cairo_rel_move_to (cr, 0, data->tek_hag_ver); /*Переходим на следующюю строку относительное перемещение*/
      cairo_move_to (cr,0,tek_poz_ver); /*Переход в точку*/
      if(data->ff_prot != NULL)
       fprintf(data->ff_prot,"Переход на начало следующей строки- по x=%f по у=%f data->tek_har_ver=%f %f (гор=%f вер=%f)Текущий отступ=%f\n",0.,
       tek_poz_ver,data->tek_hag_ver,tek_poz_ver+data->tek_hag_ver,data->rlist_gor,data->rlist_ver,data->tek_otstup_m);
     }
    

   }




  if(data->poz_v_str.ravno(data->nomer_str_tek) == 0) /*значит строка поместилась полностью*/
      data->poz_v_str.new_plus(dlinastr,data->nomer_str_tek);

  if(data->ff_prot != NULL)
   fprintf(data->ff_prot,"tek_poz_ver=%f data->rlist_ver=%f data->nomer_str_tek=%d data->kolih_str_file=%d\n",
   tek_poz_ver,data->rlist_ver,data->nomer_str_tek,data->kolih_str_file);  

 }
if(data->ff_prot != NULL)
 fprintf(data->ff_prot,"Закончен лист\n");
 
/*Для вывода последней строки у которой нет в конце строки перевода строки*/
vstr(cr,layout,data); //Выводим строку если она есть

end:;

data->metka_hl=metka_hl;
if(data->metka_hl == 1)
 data->nomer_str_tek=nomer_str_tek_zap;
pango_font_description_free (desc);                                                                                                                                 
g_object_unref (layout);
}
/************************/

static void end_print (GtkPrintOperation *operation,GtkPrintContext *context,class iceb_print_operation_data *data)
{
//g_strfreev (data->lines); Перенесено в деструктор класса
}

/****************************************/

int iceb_print_operation(GtkPrintOperationAction operation_action,const char *filname,GtkWidget *wpredok)
{
GtkPrintOperation *operation;
class iceb_print_operation_data data;
GError *error = NULL;
GtkPrintOperationResult res;
FILE *ff=NULL; 

/*Проверяем возможность прочесть файл*/
if((ff = fopen(filname,"r")) == NULL)
 {
  iceb_er_op_fil(filname,"",errno,wpredok);
  return(1);
 }
fclose(ff);
/*************
//файл протокола работы программы
if((data.ff_prot = fopen("iceb_print_operation.tmp","w")) == NULL)
  iceb_er_op_fil("iceb_print_operation.tmp","",errno,wpredok);
****************/
if(data.ff_prot != NULL)
 fprintf(data.ff_prot,"%s\n",__FUNCTION__);

operation = gtk_print_operation_new ();

gtk_print_operation_set_unit(operation, GTK_UNIT_POINTS); /*получать все размеры в пунктах при запросах на получение нужных размеров*/

if(print_settings != NULL)
  gtk_print_operation_set_print_settings (operation,print_settings);

gtk_print_operation_set_show_progress (operation,TRUE); /*прогрес выполнения*/

data.filename.plus(filname);
              
g_signal_connect (G_OBJECT (operation), "begin-print",G_CALLBACK (begin_print), &data);
g_signal_connect (G_OBJECT (operation), "draw-page", G_CALLBACK (draw_page), &data);
g_signal_connect (G_OBJECT (operation), "end-print",G_CALLBACK (end_print), &data);

if(operation_action == GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG) /*Диалог печати*/
  res=gtk_print_operation_run (operation, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, GTK_WINDOW (wpredok), &error);

if(operation_action == GTK_PRINT_OPERATION_ACTION_PREVIEW) /*Сразу просмотр перед печатью*/
  res=gtk_print_operation_run (operation,GTK_PRINT_OPERATION_ACTION_PREVIEW , GTK_WINDOW (wpredok), &error);

if(operation_action == GTK_PRINT_OPERATION_ACTION_EXPORT) /*Сразу экспорт в PDF файл*/
 {
  class iceb_u_str pdf_file(filname);
  pdf_file.plus(".pdf");

  gtk_print_operation_set_export_filename (operation,pdf_file.ravno()); /*назначаем имя файла для экпорта в формате PDF*/
  res=gtk_print_operation_run (operation,GTK_PRINT_OPERATION_ACTION_EXPORT, GTK_WINDOW (wpredok), &error);
 }

if(operation_action == GTK_PRINT_OPERATION_ACTION_PRINT) /*Сразу печатаем без диалога печати*/
  res=gtk_print_operation_run (operation,GTK_PRINT_OPERATION_ACTION_EXPORT, GTK_WINDOW (wpredok), &error);



 if (res == GTK_PRINT_OPERATION_RESULT_APPLY)
  {
   if (print_settings != NULL)
     g_object_unref (print_settings);
   print_settings = (GtkPrintSettings*)g_object_ref (gtk_print_operation_get_print_settings (operation));
  }

if(error)
 {
  GtkWidget *dialog;
                 
  dialog = gtk_message_dialog_new (GTK_WINDOW (wpredok),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,
  GTK_BUTTONS_CLOSE,"%s", error->message);
  g_error_free (error);
                                                                                              
  g_signal_connect (dialog, "response",G_CALLBACK (gtk_widget_destroy), NULL);
                                                                                                   
  gtk_widget_show (dialog);      
 }

g_object_unref (operation);

if(data.ff_prot != NULL)
  fclose(data.ff_prot);                                             

return(0);
}
