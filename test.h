#pragma once

extern "C"
{
    #define EXPORT __attribute__((visibility("default"))) __attribute__((used))
    EXPORT void malloc10();
    EXPORT void malloc11();
}
