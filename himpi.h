#ifndef __HIMPI_H__
#define __HIMPI_H__

#include <mpi.h>

#define HIMPI_TAG_BASE            (23456)
#define HIMPI_TAG_SPLIT_BYHOST    (HIMPI_TAG_BASE+0)

extern int HIMPI_Comm_split_byhost(MPI_Comm comm, int key, MPI_Comm *newcomm);

#endif
