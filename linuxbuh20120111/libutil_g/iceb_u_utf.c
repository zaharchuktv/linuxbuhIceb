/*$Id: iceb_u_utf.c,v 1.12 2011-01-13 13:50:05 sasa Exp $*/
/*11.11.2008	05.12.2004	Белых А.И.	iceb_u_utf.c 
 UTF-8 encoding/decoding.
 */
#include <glib.h>

char* iceb_u_toutf(const char *input_str, char *utf_str) 
{
gsize b1,b2;
GError *er;
return(g_locale_to_utf8(input_str,-1,&b1,&b2,&er));
}

char* iceb_u_fromutf(const char *utf_str, char *out_str) 
{
gsize b1,b2;
GError *er;
return(g_locale_from_utf8(utf_str,-1,&b1,&b2,&er));
}
#if 0
#include <iconv.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "iceb_u_utf.h"
#################

#define ICONV_MAX 1024
#define default_incode_name "KOI8-U"
#define utf_name "UTF8"

static iconv_t incode_table = NULL;
static iconv_t utf_table = NULL;
static char iconv_buf[ICONV_MAX*sizeof(wchar_t)];*/
static char badchar = '*';

int init_iconv() {
//  int i;
  char *incode_name;
  
  incode_name = strchr(setlocale (LC_MESSAGES, ""), '.');
  
  if (incode_name != NULL) 
    incode_name++;
  else 
    incode_name = default_incode_name;
  
  utf_table = iconv_open(utf_name, incode_name);
  
  if (utf_table == (iconv_t) -1)
      return 1;
  
  incode_table = iconv_open(incode_name, utf_name);
  
  if (incode_table == (iconv_t) -1)
      return 1;
  
  return 0;
}
char* iceb_u_toutf(char *input_str, char *utf_str) 
{
  if(input_str == NULL)
   return("");
  
  char *pi, *po;
  char *ret;
  size_t l, li, lo;
  
  l = 0;
  li = strlen(input_str);
  
  if (li > ICONV_MAX) 
    li = ICONV_MAX;
  
  lo = ICONV_MAX;
  
  pi = input_str;
  
  po = ret = utf_str ? utf_str : iconv_buf;

  if (!incode_table) 
    l = init_iconv();
  
  if ((l==0) && (li > 0)) 
   {
    while ((l = iconv(utf_table, &pi, &li, &po, &lo))) 
     {
        /* bad char */
//	printf("li=%d; lo=%d\n", li, lo);
        pi++;
        li--;
        *po = badchar;
	po++;
	lo--;
     }
   }
  *po = '\0';
  return(ret);
}

char* iceb_u_fromutf(char *utf_str, char *out_str) 
{
  char *pi, *po;
  char *ret;
  size_t l, li, lo;

  l = 0;
  li = strlen((char*)utf_str);
  
  if (li > (ICONV_MAX)) 
    li = ICONV_MAX;
  
  lo = ICONV_MAX;
  pi = utf_str;
  po = ret = out_str ? out_str : (char*) iconv_buf;
  
  if (!incode_table) 
     l = init_iconv();
  
  if ((l==0) && (li > 0)) 
   {
    while ((l = iconv(incode_table, &pi, &li, &po, &lo))) 
     {
        /* bad char */
//	printf("li=%d; lo=%d\n", li, lo);
        pi++;
        li--;
        *po = badchar;
	po++;
        lo--;
     }
   }
  *po = '\0';
  return(ret);

}
#############
#endif

