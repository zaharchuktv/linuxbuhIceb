/*$Id: rnn_d5w.h,v 1.3 2011-01-13 13:49:52 sasa Exp $*/
/*19.08.2008	03.04.2008	Белых А.И.	rnn_d5w.h
Реквизиты для расчёта "Розшифровки податкових зобов'язань та податкового кредиту в розрізі контрагентів"
*/

class rnn_d5w
 {
  public:
    /*полученные налоговые накладные*/
    class iceb_u_spisok innn1;
    class iceb_u_spisok innn_per1;
    class iceb_u_double suma_bez_nds1;
    class iceb_u_double suma_nds1;
    class iceb_u_spisok naim_kontr1;
    double os_sr1[2]; /*Сумма по основным сдерствам*/

    class iceb_u_spisok innn1d;
    class iceb_u_spisok innn_per1d;
    class iceb_u_double suma_bez_nds1d;
    class iceb_u_double suma_nds1d;
    class iceb_u_spisok naim_kontr1d;


    /*выданные налоговые накладные*/
       
    class iceb_u_spisok innn2;
    class iceb_u_double suma_bez_nds2;
    class iceb_u_double suma_nds2;
    class iceb_u_spisok naim_kontr2;
    double os_sr2[2]; /*Сумма по основным сдерствам*/
  rnn_d5w()
   {
    os_sr1[0]=os_sr1[1]=0.;
    os_sr2[0]=os_sr2[1]=0.;
   }

 };
