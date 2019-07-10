// =============================================================================
// EdoMacros.h
// Base macro definitions for use in the engine and external applications
//
// Created by Victor on 2019/07/09.
// =============================================================================

#ifndef EDOCORE_EDOMACROS_H
#define EDOCORE_EDOMACROS_H

#include "EdoIncludes.h"

// Define _TCHAR (if not already done by <tchar.h>
#ifndef _TCHAR
#ifdef _UNICODE
#define _TCHAR wchar_t
#else
#define _TCHAR char
#endif // DEFINE _UNICODE
#endif // !DEFINE _TCHAR

// Define T, TEXT, ANSI, and WIDE macros to convert (or not) text
#define WIDE(x) L##x
#define ANSI(x) x
#define TEXT(x) __TEXT(x)
#define __TEXT(x) L##x

#define EdoOut cout
#define EdoIn cin

#endif // EDOCORE_EDOMACROS_H
