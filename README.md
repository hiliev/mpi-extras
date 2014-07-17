mpi-extras
==========

A collection of useful MPI extensions:

* `HIMPI_Comm_split_byhost`: partitions the group associated with the provided communicator into disjoint subgroups, such that each subgroup contains processes that run on the same host. Equivalent to `MPI_Comm_split_type` (MPI-3.0) with `split_type = MPI_COMM_TYPE_SHARED`.
