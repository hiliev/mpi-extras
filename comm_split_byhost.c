#include "himpi.h"

#ifdef _DEBUG
static void show_string(char *s, int len)
{
    int i;

    printf("'");
    for (i = 0; i < len; i++)
    {
        printf("%c", s[i] ? s[i] : ';');
    }
    printf("'\n");
}
#endif

/*
 * Partitions the group associated with comm into disjoint subgroups,
 * where each subgroup contains processes that run on the same node.
 * Ranks in each subgroup are ordered according to the value of key.
 */
int
HIMPI_Comm_split_byhost(MPI_Comm comm,
                        int key,
                        MPI_Comm *newcomm)
{
    int rank, size;
    int prev, next;
    char myproc[MPI_MAX_PROCESSOR_NAME];
    int myproclen;
    int groupid = 0;
    MPI_Status status;

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    MPI_Get_processor_name(myproc, &myproclen);
#ifdef _DEBUG
    printf("[0:%02d] running on %s [%d]\n", rank, myproc, myproclen);
#endif

    prev = (rank == 0) ? MPI_PROC_NULL : (rank - 1);
    next = (rank == size-1) ? MPI_PROC_NULL : (rank + 1);

    // Receive the host list from the previous rank

    MPI_Probe(prev, HIMPI_TAG_SPLIT_BYHOST, comm, &status);
    int listsize;
    MPI_Get_elements(&status, MPI_CHAR, &listsize);
#ifdef _DEBUG
    printf("[1:%02d] listsize = %d\n", rank, listsize);
#endif

    // Allocate enough space to hold the list
    // and the local hostname (+ NUL) if not in the listr,.
    // then receive the list

    char *list = malloc(listsize + myproclen + 1);
    MPI_Recv(list, listsize, MPI_CHAR,
             prev, HIMPI_TAG_SPLIT_BYHOST, comm, &status);
#ifdef _DEBUG
    printf("[2:%02d] list = ", rank); show_string(list, listsize);
#endif

    // Check if the local hostname is already on the list

    int pos = 0;
    while (pos < listsize)
    {
        int length = strlen(list + pos);
        if ((length == myproclen) &&
            (strncmp(list + pos, myproc, myproclen+1) == 0))
        {
            break;
        }
        groupid++;
        pos += length + 1;
    }

#ifdef _DEBUG
    printf("[3:%02d] after search group = %d, pos = %d\n", rank, groupid, pos);
#endif

    if (pos >= listsize)
    {
        // Append the local hostname at the end of the list
        strncpy(list + listsize, myproc, myproclen + 1);
        listsize += myproclen + 1;
#ifdef _DEBUG
        printf("[4:%02d] added local hostname to the list\n", rank);
#endif
    }

    // Forward the list further on
    MPI_Send(list, listsize, MPI_CHAR, next, HIMPI_TAG_SPLIT_BYHOST, comm);
    free(list);

    // Split the communicator
    return MPI_Comm_split(comm, groupid, key, newcomm);
}
