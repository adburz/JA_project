#pragma once
#ifdef JACPPALGO_API_EXPORTS
#define JACPPALGO_API __declspec(dllexport)
#else
#define JACPPALGO_API __declspec(dllimport)
#endif

extern "C" JACPPALGO_API void decoding(char* bmpArray, int aBegin, char* msg, int mBegin, int mEnd);
extern "C" JACPPALGO_API void encoding(char* bmpArray, int aBegin, char* msg, int mBegin, int mEnd);