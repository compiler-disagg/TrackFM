# include <math.h>
# include <stdio.h>
# include <stdlib.h>
#include <time.h>

# include "kmeans.h"


void test04 ( );

/******************************************************************************/

void _Z7do_workPv(void * ptr)

/******************************************************************************/
/*
  Purpose:

    kmeans_test() tests kmeans().

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    08 March 2018

  Author:

    John Burkardt
*/
{
  timestamp ( );
  printf ( "\n" );
  printf ( "KMEANS_TEST\n" );
  printf ( "  C version\n" );
  printf ( "  Test the KMEANS library.\n" );

  test04 ( );
/*
  Terminate.
*/
  printf ( "\n" );
  printf ( "KMEANS_TEST\n" );
  printf ( "  Normal end of execution.\n" );
  printf ( "\n" );
  timestamp ( );

}
#define MICRO 1000000
double get_time_us() {

	struct timespec start;
	clock_gettime( CLOCK_BOOTTIME, &start );
	return (start.tv_sec * MICRO) + (start.tv_nsec/1000);
}
/******************************************************************************/

void test04 ( )

/******************************************************************************/
/*
  Purpose:

    TEST04 tries out the KMEANS_02 routine.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    08 March 2018

  Author:

    John Burkardt
*/
{
  char *center_filename = "test04_centers.txt";
  int *cluster;
  double *cluster_center;
  double *cluster_energy;
  char *cluster_filename = "test04_clusters.txt";
  int cluster_num;
  int *cluster_population;
  double *cluster_variance;
  int dim_num;
  int it_max;
  int it_num;
  double *point;
  char *point_filename = "points_100.txt";
  int point_num;
  double times[10];

  printf ( "\n" );
  printf ( "TEST04\n" );
  printf ( "  Test the KMEANS_02 algorithm.\n" );
  printf ( "  (Applied Statistics Algorithm #136)\n" );
/*
  Read the data points.
*/
  printf ( "\n" );
  printf ( "  Data points will be read from \"%s\"\n", point_filename );

  r8mat_header_read ( point_filename, &dim_num, &point_num );

  printf ( "\n" );
  printf ( "  Point spatial dimension = %d\n", dim_num  );
  printf ( "  Number of points = %d\n", point_num );

  point = r8mat_data_read ( point_filename, dim_num, point_num );
/*
  Clustering parameters.
*/
  cluster_num = 5;
  it_max = 20;

  cluster = i4vec_negone_new ( point_num );
  cluster_energy = ( double * ) malloc ( cluster_num * sizeof ( double ) );
  cluster_population = ( int * ) malloc ( cluster_num * sizeof ( int ) );

  printf ( "\n" );
  printf ( "  Number of iterations allowed is %d\n", it_max );
/*
  Initialize the centers.
*/
  cluster_center = cluster_initialize_1 ( dim_num, point_num, cluster_num, point );

  times[0] = get_time_us();
  kmeans_02 ( dim_num, point_num, cluster_num, it_max, &it_num, point,
    cluster, cluster_center, cluster_population, cluster_energy );


  cluster_variance = cluster_variance_compute ( dim_num, point_num, cluster_num,
    point, cluster, cluster_center );

  times[1] = get_time_us();
  printf ( "\n" );
  printf ( "Kmeans Time taken : %lf \n" , (times[1] - times[0]));
  printf ( "  Number of iterations taken is %d\n", it_num );
  cluster_print_summary ( point_num, cluster_num,
    cluster_population, cluster_energy, cluster_variance );

  r8mat_write ( center_filename, dim_num, cluster_num, cluster_center );

  printf ( "\n" );
  printf ( "  Cluster centers written to \"%s\"\n", center_filename );

  i4mat_write ( cluster_filename, 1, point_num, cluster );

  printf ( "  Cluster assignments written to \"%s\"\n", cluster_filename );

  free ( cluster );
  free ( cluster_center );
  free ( cluster_energy );
  free ( cluster_population );
  free ( cluster_variance );
  free ( point );

  return;
}
/******************************************************************************/

/******************************************************************************/

int __attribute((optnone)) main() {
	_Z7do_workPv(NULL);
	return 0;
}
