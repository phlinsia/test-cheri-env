#include <errno.h>
#include <compartment.h>
#include <debug.h>
#include <unwind.h>
#include <stdlib.h>

#define DEBUG_CONTEXT "Double Free Compartment"

__cheri_compartment("double-free") int vuln1(void)
{
    CHERIOT_DEBUG_LOG(DEBUG_CONTEXT, "Testing Double Free...");

    int *ptr = (int*)malloc(sizeof(int));
    if (!ptr) { CHERIOT_DEBUG_LOG(DEBUG_CONTEXT, "malloc returned NULL"); return 0; }
    *ptr = 42;

    int *ptr2 = ptr;

    free(ptr);
    CHERIOT_DEBUG_LOG(DEBUG_CONTEXT, "After first free");

    // START MODIFICATION
    CHERIOT_DURING {
        // Probe the pointer to see if it is still valid.
        // Since ptr was freed above, ptr2 (which aliases ptr) is now revoked (Tag=0).
        // Attempting to read it will cause a CHERI Tag Violation Fault.
        volatile int probe = *ptr2; 
        
        // If we survived the probe (unlikely if CHERI is working), try to free.
        free(ptr2);
        CHERIOT_DEBUG_LOG(DEBUG_CONTEXT, "After second free");
    } CHERIOT_HANDLER {
        CHERIOT_DEBUG_LOG(DEBUG_CONTEXT, "Error detected: Double Free (accessing revoked memory)!");
        return -1;
    } CHERIOT_END_HANDLER
    // END MODIFICATION

    return 0;
}