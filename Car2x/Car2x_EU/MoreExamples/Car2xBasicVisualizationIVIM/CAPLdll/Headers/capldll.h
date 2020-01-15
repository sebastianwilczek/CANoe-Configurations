#pragma once

#define USECDLL_FEATURE
#define _BUILDNODELAYERDLL

#include "cdll.h"
#include "via.h"
#include "via_CDLL.h"

#include "utils.h"

Utils *utils;

BOOL   WINAPI DllMain(HINSTANCE handle, DWORD reason, void*);

long   CAPLEXPORT far CAPLPASCAL FileExists(const char* relFilePath);

double CAPLEXPORT far CAPLPASCAL GetAngle(double x1, double y1, double x2, double y2);
double CAPLEXPORT far CAPLPASCAL GetGeoAngle(double latPos1, double lonPos1, double latPos2, double lonPos2);

long   CAPLEXPORT far CAPLPASCAL CombineImageAndText1(const char* relPathOutput, const char* relPathImage, const char* text);
long   CAPLEXPORT far CAPLPASCAL CombineImageAndText2(const char* relPathOutput, const char* relPathImage, const char* text, double textSize);
long   CAPLEXPORT far CAPLPASCAL CombineImageAndText3(const char* relPathOutput, const char* relPathImage, const char* text, double textSize, long textOffsetX, long textOffsetY);

long   CAPLEXPORT far CAPLPASCAL CombineImages1(const char* relPathOutput, const char* relPathImage1, const char* relPathImage2);
long   CAPLEXPORT far CAPLPASCAL CombineImages2(const char* relPathOutput, const char* relPathImage1, const char* relPathImage2, double scaleImage2);
long   CAPLEXPORT far CAPLPASCAL CombineImages3(const char* relPathOutput, const char* relPathImage1, const char* relPathImage2, double scaleImage2, long offsetXImage2, long offsetYImage2);

long   CAPLEXPORT far CAPLPASCAL GetRoadSignDescription1(char* output, long pictogramCode);
long   CAPLEXPORT far CAPLPASCAL GetRoadSignDescription2(char* output, long pictogramCode, int maxLines, int charsPerLine);

CAPL_DLL_INFO4 table[] =
{
  { CDLL_VERSION_NAME, (CAPL_FARCALL)CDLL_VERSION, "", "", CAPL_DLL_CDECL, 0xabcd, CDLL_EXPORT },

  { "DLLFileExists",              (CAPL_FARCALL)FileExists,              "CAPL_DLL", "Determines whether the file already exists (path relative to configuration path)",     'L', 1, "C",      "\001",                       { "relFilePath" } },

  { "DLLGetAngle",                (CAPL_FARCALL)GetAngle,                "CAPL_DLL", "Returns the angle in degrees north between two points (x1, y1) and (x2, y2)",          'F', 4, "FFFF",   "\000\000\000\000",           { "x1", "y1", "x2", "y2" } },
  { "DLLGetGeoAngle",             (CAPL_FARCALL)GetGeoAngle,             "CAPL_DLL", "Returns the angle in degrees north between two geo locations",                         'F', 4, "FFFF",   "\000\000\000\000",           { "latPos1", "lonPos1", "latPos2", "lonPos2" } },

  { "DLLCombineImageAndText",     (CAPL_FARCALL)CombineImageAndText1,    "CAPL_DLL", "Write text to the center of the image (filepath relative to configuration path)",      'L', 3, "CCC",    "\001\001\001",               { "relPathOutput", "relPathImage", "text" } },
  { "DLLCombineImageAndText",     (CAPL_FARCALL)CombineImageAndText2,    "CAPL_DLL", "Write text to the center of the image (filepath relative to configuration path)",      'L', 4, "CCCF",   "\001\001\001\000",           { "relPathOutput", "relPathImage", "text", "textSize" } },
  { "DLLCombineImageAndText",     (CAPL_FARCALL)CombineImageAndText3,    "CAPL_DLL", "Write text to the center of the image (filepath relative to configuration path)",      'L', 6, "CCCFLL", "\001\001\001\000\000\000",   { "relPathOutput", "relPathImage", "text", "textSize", "textOffsetX", "textOffsetY" } },

  { "DLLCombineImages",           (CAPL_FARCALL)CombineImages1,          "CAPL_DLL", "Combine two images into one (filepaths relative to configuration path)",               'L', 3, "CCC",    "\001\001\001",               { "relPathOutput", "relPathImage1", "relPathImage2" } },
  { "DLLCombineImages",           (CAPL_FARCALL)CombineImages2,          "CAPL_DLL", "Combine two images into one (filepaths relative to configuration path)",               'L', 4, "CCCF",   "\001\001\001\000",           { "relPathOutput", "relPathImage1", "relPathImage2", "scaleImage2" } },
  { "DLLCombineImages",           (CAPL_FARCALL)CombineImages3,          "CAPL_DLL", "Combine two images into one (filepaths relative to configuration path)",               'L', 6, "CCCFLL", "\001\001\001\000\000\000",   { "relPathOutput", "relPathImage1", "relPathImage2", "scaleImage2", "offsetXImage2", "offsetYImage2" } },

  { "DLLGetRoadSignDescription",  (CAPL_FARCALL)GetRoadSignDescription1, "CAPL_DLL", "Retrieve the road sign description (standard format) and store it to output",          'L', 2, "CL",     "\001\000",                   { "output", "pictogramCode" } },
  { "DLLGetRoadSignDescription",  (CAPL_FARCALL)GetRoadSignDescription2, "CAPL_DLL", "Retrieve the road sign description (custom format) and store it to output",            'L', 4, "CLLL",   "\001\000\000\000",           { "output", "pictogramCode", "maxLines", "charsPerLine" } },

  { 0, 0 }
};
CAPLEXPORT CAPL_DLL_INFO4 far * caplDllTable4 = table;