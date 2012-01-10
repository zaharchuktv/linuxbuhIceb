/*$Id: taxi_oth.h,v 1.24 2011-01-13 13:50:09 sasa Exp $*/
/*12.03.2005	08.12.2003	Белых А.И.	taxi_oth.h
Класс для получения отчета по водителям и операторам
*/

enum
 {
  E_DATAN,
  E_VREMN,
  E_DATAK,
  E_VREMK,
  E_KODVOD,
  E_KODOP,
  E_KODZAV,
  E_KODKLIENTA,
  KOLENTER
 };


class taxi_oth_data
 {
  public:
  
  static iceb_u_str datn;     //Дата начала периода
  static iceb_u_str datk;     //Дата конца периода
  static iceb_u_str vremn;     //Время начала периода
  static iceb_u_str vremk;     //Время конца периода
  static iceb_u_str kodvod; //Коды водителей
  static iceb_u_str kodop;  //Коды операторов
  static iceb_u_str kodzav;  //Коды завершения
  static iceb_u_str kodklienta; //Код клиента
  
  class iceb_u_spisok imafil;            //Список имен файлов
  class iceb_u_spisok nameoth;           //Список наименований отчетов

  GtkWidget *window;
  GtkWidget *bar;
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *radiobutton0; //По дате записи
  GtkWidget *radiobutton1; //По дате заказа
  short metkarr; //0-по дате записи 1-по дате заказа  
  int metka_voz;   //0-выход из меню без запуска отчета 1- с запуском отчета
  int kolstr;
  gfloat kolstr1;
  
  SQLCURSOR cur;  
  FILE *ffprot;
  char imafprot[40];
  
  iceb_u_spisok vod;           //Список кодов водителей
  iceb_u_int op;     //Список кодов операторов
  iceb_u_int zav;     //Список кодов завершения
  iceb_u_int vod_zav; //Массив коды водителей-коды завершения
  iceb_u_int op_zav;  //Массив коды операторов-коды завершения
  iceb_u_int vod_bp;  //Количество бесплатных поездок по водителям
  iceb_u_int vod_pp;  //Количество платных поездок по водителям
  iceb_u_double vod_sum_pp;  //Сумма платных поездок по водителям
  iceb_u_double vod_sum_bp;  //Сумма бесплатных поездок по водителям

  iceb_u_spisok klient; //Список постоянных клиентов
  iceb_u_int    kli_kp_o; //Количество оплаченных поездок    
  iceb_u_int    kli_kp_b; //Количество бесплатных поездок    
  iceb_u_double sum_op;   //Сумма оплаченных поездок
  iceb_u_double sum_bp;   //Сумма бесплатных поездок

  //Конструктор
  taxi_oth_data()
   {
    kolstr1=0.;
    metka_voz=0;
    metkarr=0;
   
   }

  void clear_zero()
   {
    datn.new_plus("");
    datk.new_plus("");
    vremn.new_plus("");
    vremk.new_plus("");
    kodvod.new_plus("");
    kodop.new_plus("");
    kodzav.new_plus("");
    kodklienta.new_plus("");
   }

 };
