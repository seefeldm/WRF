/***********************************************************************
     
                              COPYRIGHT
     
     The following is a notice of limited availability of the code and 
     Government license and disclaimer which must be included in the 
     prologue of the code and in all source listings of the code.
     
     Copyright notice
       (c) 1977  University of Chicago
     
     Permission is hereby granted to use, reproduce, prepare 
     derivative works, and to redistribute to others at no charge.  If 
     you distribute a copy or copies of the Software, or you modify a 
     copy or copies of the Software or any portion of it, thus forming 
     a work based on the Software and make and/or distribute copies of 
     such work, you must meet the following conditions:
     
          a) If you make a copy of the Software (modified or verbatim) 
             it must include the copyright notice and Government       
             license and disclaimer.
     
          b) You must cause the modified Software to carry prominent   
             notices stating that you changed specified portions of    
             the Software.
     
     This software was authored by:
     
     Argonne National Laboratory
     J. Michalakes: (630) 252-6646; email: michalak@mcs.anl.gov
     Mathematics and Computer Science Division
     Argonne National Laboratory, Argonne, IL  60439
     
     ARGONNE NATIONAL LABORATORY (ANL), WITH FACILITIES IN THE STATES 
     OF ILLINOIS AND IDAHO, IS OWNED BY THE UNITED STATES GOVERNMENT, 
     AND OPERATED BY THE UNIVERSITY OF CHICAGO UNDER PROVISION OF A 
     CONTRACT WITH THE DEPARTMENT OF ENERGY.
     
                      GOVERNMENT LICENSE AND DISCLAIMER
     
     This computer code material was prepared, in part, as an account 
     of work sponsored by an agency of the United States Government.
     The Government is granted for itself and others acting on its 
     behalf a paid-up, nonexclusive, irrevocable worldwide license in 
     this data to reproduce, prepare derivative works, distribute 
     copies to the public, perform publicly and display publicly, and 
     to permit others to do so.  NEITHER THE UNITED STATES GOVERNMENT 
     NOR ANY AGENCY THEREOF, NOR THE UNIVERSITY OF CHICAGO, NOR ANY OF 
     THEIR EMPLOYEES, MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR 
     ASSUMES ANY LEGAL LIABILITY OR RESPONSIBILITY FOR THE ACCURACY, 
     COMPLETENESS, OR USEFULNESS OF ANY INFORMATION, APPARATUS, 
     PRODUCT, OR PROCESS DISCLOSED, OR REPRESENTS THAT ITS USE WOULD 
     NOT INFRINGE PRIVATELY OWNED RIGHTS.

***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "rsl.h"
#include <sys/stat.h>


static int show_domain_first = 1 ;

SHOW_DOMAIN_DECOMP ( d_p )
  int_p d_p ;
{
  rsl_index_t d ;
  int Phist[RSL_MAXPROC] ;
  char fname[50] ;
  FILE * fp ;
  rsl_index_t i, j ;
  char * code ;

  d = *d_p ;

  RSL_TEST_ERR(d < 0 || d >= RSL_MAXDOMAINS,
          "bad domain descriptor" ) ;
  RSL_TEST_ERR( domain_info[d].valid != RSL_VALID,
        "descriptor for invalid domain" ) ;

  if ( show_domain_first )
  {
    code = "w" ;
    show_domain_first = 0 ;
  }
  else
  {
    code = "r+" ;
  }
  printf("show_domain_decomp called on %d\n",rsl_myproc) ;
  sprintf(fname,"show_domain_%04d",rsl_myproc) ;
  if (( fp = fopen ( fname, code )) == NULL )
  {
     perror(fname) ;
     exit(2) ;
  }
  fseek(fp,0L,2) ;
  fprintf(fp,"domain=%d, len_n=%d, len_m=%d\n", 
	   d, domain_info[d].len_n, domain_info[d].len_m ) ;
  if ( domain_info[d].decomposed != 1 )
  {
    fprintf(fp,"not decomposed at this point in program") ;
    return ;
  }
  for ( i = 0 ; i < RSL_MAXPROC ; i++ ) Phist[i] = 0 ;
  for ( i = domain_info[d].len_m-1 ; i >= 0 ; i-- )
  {
    for ( j = 0 ; j < domain_info[d].len_n ; j++ )
    {
      if ( domain_info[d].domain[INDEX_2(j,i,domain_info[d].len_m)].children_p != NULL )
        fprintf(fp, "%2d ",domain_info[d].domain[INDEX_2(j,i,domain_info[d].len_m)].P+90) ;
      else
        fprintf(fp, "%2d ",domain_info[d].domain[INDEX_2(j,i,domain_info[d].len_m)].P) ;
      Phist[domain_info[d].domain[INDEX_2(j,i,domain_info[d].len_m)].P]++ ;
    }
    fprintf(fp,"\n") ;
  }
  for ( i = 0 ; i < rsl_nproc_all ; i++ )
  {
    fprintf(fp,"%5d  %7d\n",i,Phist[i]) ;
  }
  fclose(fp) ;
}

GET_DOMAIN_DECOMP ( d_p, wk, nwk_p )
  int_p d_p ;
  int_p wk ;
  int_p nwk_p ;
{
  rsl_index_t d ;
  char fname[50] ;
  FILE * fp ;
  int nwk ;
  rsl_index_t i, j, m ;
  char * code ;

  d = *d_p ;
  nwk = *nwk_p ;

  RSL_TEST_ERR(d < 0 || d >= RSL_MAXDOMAINS,
          "bad domain descriptor" ) ;
  RSL_TEST_ERR( domain_info[d].valid != RSL_VALID,
        "descriptor for invalid domain" ) ;
  RSL_TEST_ERR(wk == NULL, "GET_DOMAIN_DECOMP: NULL argument wk" ) ;
  if ( domain_info[d].len_m * domain_info[d].len_n > nwk )
  {
    sprintf(mess,"%d ints would overwrite input array (size=%d ints)\n",
	    domain_info[d].len_m * domain_info[d].len_n,
	    nwk);
    RSL_TEST_ERR(1,mess) ;
  }

  if ( domain_info[d].decomposed != 1 )
  {
#if 0
    fprintf(stderr,"not decomposed at this point in program") ;
#endif
    return ;
  }
  m = domain_info[d].len_m ;

fprintf(stderr,"d %d m %d, n %d\n",d,domain_info[d].len_m,domain_info[d].len_n) ;
  for ( j = 0 ; j < domain_info[d].len_n ; j++ )
  {
    for ( i = 0 ; i < domain_info[d].len_m ; i++ )
    {
      wk[INDEX_2(j,i,m)] = domain_info[d].domain[INDEX_2(j,i,m)].P ;
    }
  }
}
