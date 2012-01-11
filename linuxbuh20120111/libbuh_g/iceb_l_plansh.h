/*$Id: iceb_l_plansh.h,v 1.6 2011-01-13 13:49:59 sasa Exp $*/
/*03.05.2007	02.01.2004	Белых А.И.	iceb_l_plansh.h
*/




class  plansh_rek
 {
  public:
  
  iceb_u_str shet;
  iceb_u_str naim;
  int        tip;
  int        vid;
  int        saldo;
  int        status;
  int        val;  //Пока не используется

  class iceb_u_str kod_subbal;
  //Конструктор
  plansh_rek()
   {
    clear_zero();
   }  


  void clear_zero() //Записать нулевой байт во все переменные
   {
    shet.new_plus("");
    naim.new_plus("");
    val=tip=vid=saldo=status=0;
    kod_subbal.new_plus("");
   }

 };


extern nl_catd	fils; /*Указатель на базу сообщений*/
extern SQL_baza	bd;
extern char *name_system;
