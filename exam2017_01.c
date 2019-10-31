#include <mpi.h>

#define WORKTAG 1
#define DIETAG 2
void master(void)
void slave(void);

int main(int argc, char *argv[])
{
    int rank;
    // Initialise MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(&rank);
    if (rank == 0)
        master();
    else
        slave();

    // Cleanup MPI
    MPI_Finalize();
    return 0;
}

void master(void)
{
    int ntasks, work;
    double result;
    MPI_Status status;
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

    // Seed the slaves.
    for (int rank = 1; rank < ntasks; ++rank)
    {
        work = /* get_next_work_request */;
        MPI_Send(&work,
                 1,
                 MPI_DOUBLE,
                 rank,
                 WORKTAG,
                 MPI_COMM_WORLD);
    }

    // Receive a result from any slave and dispatch a new work
    // request work requests have been exhausted.
    work = 0;
    while (work > 10)
    {
        MPI_Recv(&result,
                 1,
                 MPI_DOUBLE,
                 MPI_ANY_SOURCE,
                 MPI_ANY_TAG,
                 MPI_COMM_WORLD,
                 &status);
        MPI_Send(&work,
                 1,
                 MPI_INT,
                 status.MPI_SOURCE,
                 WORKTAG,
                 MPI_COMM_WORLD);

        work--;
    }

    //Receive results for outstanding work requests.
    for (int rank = 1; rank < ntasks; ++rank)
    {
        MPI_Recv(&result,
                 1,
                 MPI_DOUBLE,
                 MPI_ANY_SOURCE,
                 MPI_ANY_TAG,
                 MPI_COMM_WORLD,
                 &status);
    }
    // Tell all the slaves to exit.
    for (int rank = 1; rank < ntasks; ++rank)
    {
        MPI_Send(0,
                 0,
                 MPI_INT,
                 rank,
                 DIETAG,
                 MPI_COMM_WORLD);
    }

    return;
}

void slave(void)
{
    double result;
    int work;
    MPI_Status status;
    for (;;)
    {
        MPI_Irecv(&work,
                  1,
                  MPI_INT,
                  0,
                  MPI_ANY_TAG,
                  MPI_COMM_WORLD,
                  &status);

        // Check the tag of the received message.
        if (status.MPI_TAG == DIETAG)
        {
            return;
        }
        result = work * 2.0;
        MPI_Send(&result,
                 1,
                 MPI_DOUBLE,
                 0,
                 0,
                 MPI_COMM_WORLD);
    }

    return;
}