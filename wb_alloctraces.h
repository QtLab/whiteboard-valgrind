#pragma once

#include "pub_tool_basics.h"

void* wb_malloc ( ThreadId tid, SizeT szB );
void* wb___builtin_new ( ThreadId tid, SizeT szB );
void* wb___builtin_vec_new ( ThreadId tid, SizeT szB );
void* wb_calloc ( ThreadId tid, SizeT m, SizeT szB );
void *wb_memalign ( ThreadId tid, SizeT alignB, SizeT szB );
void wb_free ( ThreadId tid __attribute__((unused)), void* p );
void wb___builtin_delete ( ThreadId tid, void* p );
void wb___builtin_vec_delete ( ThreadId tid, void* p );
void* wb_realloc ( ThreadId tid, void* p_old, SizeT new_szB );
SizeT wb_malloc_usable_size ( ThreadId tid, void* p );
