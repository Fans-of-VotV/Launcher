#pragma once

extern void MyAssertFailure(const char* expr, const char* file, int line);

#define ASSERT(expr) (void)(!(expr) && (MyAssertFailure(#expr, __FILE__, __LINE__), 0))
