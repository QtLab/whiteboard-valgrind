// Whiteboard
// (C) Maciej Gajewski, 2016

#include "wb_alloctraces.h"

#include "pub_tool_replacemalloc.h"
#include "pub_tool_libcprint.h"


extern VgFile* output;


///////////////// mallocs ///////////////////

void* wb_malloc ( ThreadId tid, SizeT szB )
{
    void* addr = VG_(cli_malloc)(VG_(clo_alignment), szB);
    VG_(fprintf)(output, "{\"action\" : \"allocation\", \"type\" : \"malloc\", \"size\" : %lu, \"addr\" : %p }\n", szB, addr);
    return addr;
}

void* wb___builtin_new ( ThreadId tid, SizeT szB )
{
    void* addr = VG_(cli_malloc)(VG_(clo_alignment), szB);
    VG_(fprintf)(output, "{\"action\" : \"allocation\", \"type\" : \"new\", \"size\" : %lu, \"addr\" : %p }\n", szB, addr);
    return addr;
}

void* wb___builtin_vec_new ( ThreadId tid, SizeT szB )
{
    void* addr = VG_(cli_malloc)(VG_(clo_alignment), szB);
    VG_(fprintf)(output, "{\"action\" : \"allocation\", \"type\" : \"new[]\", \"size\" : %lu, \"addr\" : %p }\n", szB, addr);
    return addr;
}

void* wb_calloc ( ThreadId tid, SizeT m, SizeT szB )
{
    return VG_(cli_malloc)(VG_(clo_alignment), m*szB);
}

void *wb_memalign ( ThreadId tid, SizeT alignB, SizeT szB )
{
    return VG_(cli_malloc)(alignB, szB);
}

void wb_free ( ThreadId tid __attribute__((unused)), void* p )
{
    VG_(fprintf)(output, "{\"action\" : \"free\", \"type\" : \"free\", \"addr\" : %p }\n", p);
    VG_(cli_free)(p);
}

void wb___builtin_delete ( ThreadId tid, void* p )
{
    VG_(fprintf)(output, "{\"action\" : \"free\", \"type\" : \"delete\", \"addr\" : %p }\n", p);
    VG_(cli_free)(p);
}

void wb___builtin_vec_delete ( ThreadId tid, void* p )
{
    VG_(fprintf)(output, "{\"action\" : \"free\", \"type\" : \"delete[]\", \"addr\" : %p }\n", p);
    VG_(cli_free)(p);
}

void* wb_realloc ( ThreadId tid, void* p_old, SizeT new_szB )
{
    return NULL;
}

SizeT wb_malloc_usable_size ( ThreadId tid, void* p )
{
    return VG_(cli_malloc_usable_size)(p);
}                               
