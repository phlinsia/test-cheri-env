// Copyright Microsoft and CHERIoT Contributors.
// SPDX-License-Identifier: MIT
#include <compartment.h>
#include <debug.hh>
#include <unwind.h>
// REMOVED: #include <fail-simulator-on-error.h>

using Debug = ConditionalDebug<true, "Use After Free Compartment">;

int __cheri_compartment("use-after-free") vuln1()
{
    Debug::log("Testing Use-After-Free (C++)...");
    int* ptr = new int;
    if (!ptr)
    {
        Debug::log("Allocation failed!");
        return 0;
    }
    *ptr = 123;
    Debug::log("ptr capability: {}", ptr);
    Debug::log("ptr points to memory with value: {}", *ptr);
    delete ptr;
    Debug::log("Memory has been freed.");
    
    // START MODIFICATION
    CHERIOT_DURING {
        *ptr = 456;
        Debug::log("Value is now: {} (this should not be printed)", *ptr);
    } CHERIOT_HANDLER {
        Debug::log("Error detected: Use After Free (C++)!");
        return -1;
    } CHERIOT_END_HANDLER
    // END MODIFICATION

    return 0;
}