/*$Id: iceb_u_utf.h,v 1.5 2011-01-13 13:50:05 sasa Exp $*/
/*
  iceb_u_toutf(input_str, [utf_str]) - преобразует строку input_str во внутренней 
  	8-битной кодировке в UTF-8. Если буфер utf_str указан - результат
  	записывается в него. Иначе - во внутренний статический буфер 
  	функции размером 1024 байт. В любом случае функция возвращает адрес
  	результирующей строки.
*/
extern char* iceb_u_toutf(const char *input_str, char *utf_str=NULL);
/*
  iceb_u_fromutf(utf_str, [out_str]) - преобразует строку из UTF-8
        во внутреннюю 8-битную кодировку. Если буфер out_str указан - 
        результат записывается в него. Иначе - во внутренний статический буфер 
  	функции размером 1024 байт. В любом случае функция возвращает адрес
  	результирующей строки.
*/
extern char* iceb_u_fromutf(const char *utf_str, char *out_str=NULL);
