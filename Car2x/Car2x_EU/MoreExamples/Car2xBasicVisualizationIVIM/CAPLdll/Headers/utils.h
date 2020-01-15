#pragma once
#pragma comment (lib, "Gdiplus.lib")

#define _USE_MATH_DEFINES

#include <windows.h>
#include <map>
#include <fstream>
#include <sstream>
#include <experimental/filesystem>
#include <gdiplus.h>

using namespace std;
using namespace std::experimental::filesystem::v1;
using namespace Gdiplus;

class Utils
{
private:
  path modulePath;
  map<long, string> roadSignCodes;

  path   ConfigPath();
  path   FullPath(const char* relativePath);

  long   GetEncoderClsid(const WCHAR* mimeType, CLSID* pClsid);
  long   GetEncoderClsid(string fileExtension, CLSID* pClsid);
  long   SaveImage(Bitmap* image, path pathOutput);

  void   InitializeRoadSignCodes();

public:
  Utils();
  Utils(path modulePath);

  long   FileExists(const char* relativePath);

  double GetAngle(double x1, double y1, double x2, double y2);
  double GetGeoAngle(double latPos1, double lonPos1, double latPos2, double lonPos2);

  long   CombineImageAndText(const char* pathOutput, const char* pathImage, const char* text, double textSize = 40.0, long textOffsetX = 0, long textOffsetY = 0);
  long   CombineImages(const char* pathOutput, const char* pathOuter, const char* pathInner, double innerScale = 1.0, long innerOffsetX = 0, long innerOffsetY = 0);
  long   GetRoadSignDescription(char* output, long pictogramCode, int maxLines = 5, int charsPerLine = 8);
};

struct GdiplusInit
{
private:
  ULONG_PTR gdiplusToken;

public:
  GdiplusInit()
  {
    GdiplusStartupInput input;
    GdiplusStartupOutput output;

    GdiplusStartup(&gdiplusToken, &input, &output);
  }

  ~GdiplusInit()
  {
    GdiplusShutdown(gdiplusToken);
  }
};