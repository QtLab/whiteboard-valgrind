// Whiteboard
// (C) Maciej Gajewski, 2016

#include "pub_tool_basics.h"
#include "pub_tool_tooliface.h"

static void wb_post_clo_init(void)
{
}

static
IRSB* wb_instrument ( VgCallbackClosure* closure,
                      IRSB* bb,
                      const VexGuestLayout* layout, 
                      const VexGuestExtents* vge,
                      const VexArchInfo* archinfo_host,
                      IRType gWordTy, IRType hWordTy )
{
    return bb;
}

static void wb_fini(Int exitcode)
{
}

static void wb_pre_clo_init(void)
{
   VG_(details_name)            ("Whiteboard");
   VG_(details_version)         (NULL);
   VG_(details_description)     ("TODO");
   VG_(details_copyright_author)(
      "Copyright (C) 2016 Maciej Gajewski");
   VG_(details_bug_reports_to)  (VG_BUGS_TO);

   VG_(details_avg_translation_sizeB) ( 275 );

   VG_(basic_tool_funcs)        (wb_post_clo_init,
                                 wb_instrument,
                                 wb_fini);

   /* No needs, no core events to track */
}

VG_DETERMINE_INTERFACE_VERSION(wb_pre_clo_init)

/*--------------------------------------------------------------------*/
/*--- end                                                          ---*/
/*--------------------------------------------------------------------*/
