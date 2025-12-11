// Copyright Microsoft and CHERIoT Contributors.
// SPDX-License-Identifier: MIT
#include <compartment.h>
#include <debug.hh>
#include <unwind.h>
// REMOVED: #include <fail-simulator-on-error.h>

using Debug = ConditionalDebug<true, "Type Confusion Compartment">;

const char Hello[] = "Hello World!";

union long_ptr {
    long l;
    const char* ptr;
} lp = { .ptr = Hello };

void inc_long_ptr(long_ptr* lpp) {
    lpp->l++;
}

int __cheri_compartment("type-confusion") vuln1()
{
    Debug::log("Testing Type confusion (C++)...");
    Debug::log("Before inc_long_ptr: lp.ptr = {}", lp.ptr);
    inc_long_ptr(&lp);
    
    // START MODIFICATION
    CHERIOT_DURING {
        // This log tries to print a corrupted pointer, causing a fault.
        Debug::log("After inc_long_ptr: lp.ptr = {}", lp.ptr);
    } CHERIOT_HANDLER {
        Debug::log("Error detected: Type confusion (C++)!");
        return -1;
    } CHERIOT_END_HANDLER
    // END MODIFICATION

    return 0;
}