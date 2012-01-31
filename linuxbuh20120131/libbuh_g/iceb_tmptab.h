/*$Id: iceb_tmptab.h,v 1.2 2011-01-13 13:49:59 sasa Exp $*/
/*03.03.2010	03.03.2010	Белых А.И.	iceb_tmptab.c
Класс для работы с временными таблицами
*/
class iceb_tmptab
 {
  private:
   int metka;
   class iceb_u_str ima_tab;  

  public:

  iceb_tmptab();
  int create_tab(const char *ima_tab,const char *zapros,GtkWidget *wpredok);
  int drop_tab(GtkWidget *wpredok);
  ~iceb_tmptab();
  
 };
