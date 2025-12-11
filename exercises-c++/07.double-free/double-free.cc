// Diagnostics for double-free behavior
// SPDX-License-Identifier: MIT

#include <compartment.h>
#include <debug.hh>
#include <unwind.h>
// REMOVED: #include <fail-simulator-on-error.h>

using Debug = ConditionalDebug<true, "Double Compartment">;


__cheri_compartment("double-free") int vuln1(void)
{
    Debug::log("Testing Double Free...");

    int *ptr = (int*)malloc(sizeof(int));
    if (!ptr) { Debug::log( "malloc returned NULL"); return 0; }
    *ptr = 42;

    int *ptr2 = ptr; // Alias

    free(ptr);
    Debug::log("After first free");

    // START MODIFICATION
    CHERIOT_DURING {
        // Probe to see if the pointer is still valid (revoked check).
        volatile int probe = *ptr2; 
        
        free(ptr2); // Will execute only if probe passes (it shouldn't)
        Debug::log("After second free");
    } CHERIOT_HANDLER {
        Debug::log("Error detected: Double Free (accessing revoked memory C++)!");
        return -1;
    } CHERIOT_END_HANDLER
    // END MODIFICATION

    return 0;
}