'use strict';

var mallocPtr = Module.findExportByName("libc.so.6", "malloc");
var malloc = new NativeFunction(mallocPtr, 'pointer', ['long']);

var freePtr = Module.findExportByName("libc.so.6", "free");
var freel = new NativeFunction(freePtr, 'void', ['pointer']);

var reallocPtr = Module.findExportByName("libc.so.6", "realloc");
var reallocl = new NativeFunction(reallocPtr, 'pointer', ['pointer', 'int']);

var sleepPtr = Module.findExportByName("libc.so.6", "usleep");
var usleepl = new NativeFunction(sleepPtr, 'void', ['int']);

var lock = 0;
var malloc_count = 0;

/* Doesn't work ... :(
var mainPtr = new NativePointer(0x400470);
console.error("Main pointer is " + mainPtr);
var mainl = new NativeFunction(mainPtr, 'int', ['int', 'pointer']);
console.error("Main is mainl ");
*/

/*
Interceptor.replace(mainPtr, new NativeCallback(function (argc, argv) {
    console.error("[+] entry point hit - starting heap tracing");
    mainl(argc, argv);
}, 'int', ['int', 'pointer']));
*/

Interceptor.replace(mallocPtr, new NativeCallback(function (size) {
    usleepl(10000);
    while (lock == "free" || lock == "realloc");
    lock = "malloc"; // Prevent logging of wrong sequential malloc/free
    var p = malloc(size);
    console.error("malloc(" + size +") = " + p);
    lock = null;
    return p;
}, 'pointer', ['int']));

Interceptor.replace(freePtr, new NativeCallback(function (p) {
    usleepl(15000);
    while (lock == "malloc" || lock == "realloc");
    lock = "free";
    freel(p);
    if (p) console.error("free(" + p + ") = <void>");
    lock = null;
}, 'void', ['pointer']));

Interceptor.replace(reallocPtr, new NativeCallback(function (p, size) {
    var p_ret;
    usleepl(15000);
    while (lock == "free" || lock == "malloc");
    lock = "realloc";
    p_ret = reallocl(p, size);
    console.error("realloc(" + p + ", " + size + ") = " + p_ret);
    lock = null;
}, 'void', ['pointer', 'int']));
