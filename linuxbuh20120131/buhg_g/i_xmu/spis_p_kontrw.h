/*$Id: spis_p_kontrw.h,v 1.3 2011-01-13 13:49:53 sasa Exp $*/
/*20.06.2010	19.06.2006	Белых А.И.	spis_p_kontrw.h
Реквизиты для расчёта списания/получения материалов по контрагентам
*/

class spis_p_kontrw_rr
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str sklad;
   class iceb_u_str kontr;
   class iceb_u_str grup_mat;
   class iceb_u_str kodop;
   class iceb_u_str kodmat;
   class iceb_u_str shetu;
   class iceb_u_str nomdok;
   int prih_rash; /*0-всё 1-приходы 2-расходы*/
   

   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   spis_p_kontrw_rr()
    {
     clear_data();
    }
  
   void clear_data()
    {
     datan.new_plus("");
     datak.new_plus("");
     sklad.new_plus("");
     kontr.new_plus("");
     grup_mat.new_plus("");
     kodop.new_plus("");
     kodmat.new_plus("");
     shetu.new_plus("");
     nomdok.new_plus("");
     prih_rash=0;
     
    }

 }; 
