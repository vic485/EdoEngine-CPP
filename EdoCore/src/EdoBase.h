// =============================================================================
// EdoBase.h
// Various necessary portions required throughout the engine.
//
// Created by Victor on 2019/05/27.
// =============================================================================

#ifndef EDOCORE_EDOBASE_H
#define EDOCORE_EDOBASE_H

#if (defined(_EDO_WINDOWS) || defined(_WIN32))
#ifdef _EXPORT_DLL
#define EDO_API __declspec(dllexport)
#else
#define EDO_API __declspec(dllimport)
#endif // _EXPORT_DLL
#else
// TODO: Linux library import / export
#error Edo currently only supports Windows!
#endif

#endif // EDOCORE_EDOBASE_H
