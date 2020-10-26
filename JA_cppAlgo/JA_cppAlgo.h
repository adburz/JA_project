#pragma once
#ifdef JACPPALGO_API_EXPORTS
#define JACPPALGO_API __declspec(dllexport)
#else
#define JACPPALGO_API __declspec(dllimport)
#endif

extern "C" JACPPALGO_API void cppDecoding(char* bmpArray, int aBegin, char* msg, int mBegin, int mEnd);
extern "C" JACPPALGO_API void cppEncoding(char* bmpArr, int aBegin, char* msg, int mBegin, int mEnd);