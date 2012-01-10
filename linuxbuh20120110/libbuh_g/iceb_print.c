/*$Id: iceb_print.c,v 1.13 2011-01-13 13:49:59 sasa Exp $*/
/*11.11.2008	16.05.2004	Белых А.И.	iceb_print.c
распечатка файла
*/
#include "iceb_libbuh.h"

void iceb_print_read_nast(char *kluh_kol_kop,char *kluh_name_pr,char *prog_pehati,GtkWidget *wpredok);


int iceb_print(const char *imaf,
int kolkop,
const char *name_print, //имя принтера
const char *kluh_peh, //Дополнительный ключ для печати
GtkWidget *wpredok)
{
char prog_pehati[56];
char kluh_name_pr[56];
char kluh_kol_kop[56];
memset(prog_pehati,'\0',sizeof(prog_pehati));
memset(kluh_name_pr,'\0',sizeof(kluh_name_pr));
memset(kluh_kol_kop,'\0',sizeof(kluh_kol_kop));


iceb_print_read_nast(kluh_kol_kop,kluh_name_pr,prog_pehati,wpredok);

return(iceb_u_print(imaf,kolkop,prog_pehati,name_print,kluh_peh,kluh_name_pr,kluh_kol_kop));

}
/****************************************/

int iceb_print(const char *imaf,GtkWidget *wpredok)
{
char prog_pehati[56];
char kluh_name_pr[56];
char kluh_kol_kop[56];
memset(prog_pehati,'\0',sizeof(prog_pehati));
memset(kluh_name_pr,'\0',sizeof(kluh_name_pr));
memset(kluh_kol_kop,'\0',sizeof(kluh_kol_kop));

iceb_print_read_nast(kluh_kol_kop,kluh_name_pr,prog_pehati,wpredok);

return(iceb_u_print(imaf,1,prog_pehati,"","",kluh_name_pr,kluh_kol_kop));

}
/***************************/
/*Чтение настроек*/
/*************************/

void iceb_print_read_nast(char *kluh_kol_kop,
char *kluh_name_pr,
char *prog_pehati,GtkWidget *wpredok)
{


if(iceb_poldan("Системный принтер",prog_pehati,"nastsys.alx",wpredok) != 0)
  printf("%s-Не найден \"Системный принтер\" !!!\n",__FUNCTION__);

memset(kluh_name_pr,'\0',sizeof(kluh_name_pr));
memset(kluh_kol_kop,'\0',sizeof(kluh_kol_kop));

iceb_poldan("Ключ для указания имени принтера",kluh_name_pr,"nastsys.alx",wpredok);
iceb_poldan("Ключ для указания количества копий",kluh_kol_kop,"nastsys.alx",wpredok);

if(kluh_kol_kop[0] == '\0')
 strcpy(kluh_kol_kop,"-#");
if(kluh_name_pr[0] == '\0')
 strcpy(kluh_kol_kop,"-#");
if(prog_pehati[0] == '\0')
 strcpy(prog_pehati,"lpr"); 
}
