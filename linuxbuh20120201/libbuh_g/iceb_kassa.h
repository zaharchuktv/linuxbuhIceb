/*$Id: iceb_kassa.h,v 1.7 2011-01-13 13:49:59 sasa Exp $*/
/*28.12.2004	20.09.2004	Белых А.И.	iceb_u_kassa.h
*/
typedef	int icebcon;
class	KASSA 
{
 public:
 icebcon	handle;
 int	sklad;
 int	nomer;
 char 	*host;
 char	*parol;
 char	*port;
 iceb_u_str prodavec;

 KASSA()
  {
   handle=0;
   sklad=0;
   nomer=0;
   host=NULL;
   parol=NULL;
   port=NULL;
   prodavec.new_plus("");
  }
};

