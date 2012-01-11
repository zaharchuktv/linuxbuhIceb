/*$Id: dooc.h,v 1.3 2011-01-13 13:49:49 sasa Exp $*/
/*21.10.2004	20.10.2004	Белых А.И.	dooc.h
Данные для дооценки документа
*/

class dooc_data
 {
  public:
  static iceb_u_str datad;
  static iceb_u_str sumadooc;
  static iceb_u_str osnov;
  static iceb_u_str kodmat;

  //Реквизиты документа куда копируем
  short dd,md,gd;
  int    tipz; //1-приход 2-расход
  iceb_u_str nomdok_c;
  int    skl;
  
  dooc_data()
   {
    clear_data();
   }

  void clear_data()
   {
    datad.new_plus("");
    osnov.new_plus("");
    kodmat.new_plus("");
    sumadooc.new_plus("");
   }
 };
