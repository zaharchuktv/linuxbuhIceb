/*$Id: iceb_u_file.h,v 1.2 2011-01-13 13:50:05 sasa Exp $*/
/*04.12.2007	04.12.2007	Белых А.И.	iceb_u_file.h
Класс для гарантированного закрытия файла
*/

class iceb_u_file
 {
  public:
   FILE *ff;
  
  iceb_u_file()
   {
    ff=NULL;
   }
  ~iceb_u_file()
   {
    close();
   }
  void close()
   {
    if(ff != NULL)
     fclose(ff);
    ff=NULL;
   }
 };
