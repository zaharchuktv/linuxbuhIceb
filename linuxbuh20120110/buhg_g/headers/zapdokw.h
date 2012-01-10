/*$Id: zapdokw.h,v 1.3 2011-01-13 13:49:55 sasa Exp $*/
/*20.08.2008	05.08.2004	Белых А.И.	zapdokw.h
Структура записи шапки документа
*/

class zapdokw_data
 {
  public:
  
  int tipz; //1-приход 2-расход
  
  //реквизиты вводимые в меню
  iceb_u_str kontr;
  iceb_u_str datad;
  iceb_u_str sklad;
  iceb_u_str nomdok;
  iceb_u_str kodop;
  iceb_u_str dover;
  iceb_u_str data_dover;
  iceb_u_str sherez;
  iceb_u_str nomdok_p; //номер парного документа или накладной поставщика
  iceb_u_str data_dok_p; //дата накладной поставщика
  iceb_u_str nomnalnak;
  iceb_u_str data_pnn;
  iceb_u_str data_opl;
  iceb_u_str osnov; /*основание для расходного документа / коментарий для приходного*/
  int nds;
  //дополнительные реквизиты расходного документа
  
  iceb_u_str uslov_prod;
  iceb_u_str forma_oplat;
  iceb_u_str nomer_mah;
/******************/

  int sklad_par;
  double suma_kor_dok;  
  int loginrash; //логин распечатавшего чек
  
  //Полученные реквизиты для корректировки шапки документа
  short dd_kor;
  short md_kor;
  short gd_kor;
  int skl_kor;
  iceb_u_str nomdok_kor;
  iceb_u_str nomdok_p_kor; //Запоминаем только для расходных документов
  iceb_u_str kontr_kor;
  short metkapr;  //0-запись из i_xmu 1-xpro
  
  zapdokw_data()
   {
    dd_kor=md_kor=gd_kor=0;
    nomdok_kor.new_plus("");      
    nomdok_p_kor.new_plus("");      
    skl_kor=0;
    kontr_kor.new_plus("");
    loginrash=0;
    tipz=0;
    
    clear();
    suma_kor_dok=0.;
    metkapr=0;
   }
  
  void clear()
   {
    sklad_par=0;
    kontr.new_plus("");
    datad.new_plus("");
    sklad.new_plus("");
    kodop.new_plus("");
    nomdok.new_plus("");
    dover.new_plus("");
    data_dover.new_plus("");
    sherez.new_plus("");
    nomdok_p.new_plus("");
    data_dok_p.new_plus("");
    nomnalnak.new_plus("");
    nds=0;
    data_opl.new_plus("");
    data_pnn.new_plus("");
    osnov.new_plus("");
    uslov_prod.new_plus("");
    forma_oplat.new_plus("");
    nomer_mah.new_plus("");
   }   
 };
