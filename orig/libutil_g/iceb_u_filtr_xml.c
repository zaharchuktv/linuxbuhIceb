/*$Id: iceb_u_filtr_xml.c,v 1.2 2011-08-29 07:13:49 sasa Exp $*/
/*15.08.2011	13.10.2006	Белых А.И.	iceb_u_filtr_xml.c
Преобразование символьных строк по требованию налоговиков для символьных строк

> - &gt;
< - &lt;
" - &quot;
' - &apos;
& - &amp;

*/
#include "iceb_util.h"

const char *iceb_u_filtr_xml(const char *stroka)
{
static class iceb_u_str str_out;
str_out.new_plus(stroka);

iceb_u_zvstr(&str_out,"&","&amp;"); /*обязательно первым так как символ & встречается в оразцах на которые заменяють*/
iceb_u_zvstr(&str_out,">","&gt;");
iceb_u_zvstr(&str_out,"<","&lt;");
iceb_u_zvstr(&str_out,"\"","&quot;");
iceb_u_zvstr(&str_out,"'","&apos;");

return(str_out.ravno());

}
