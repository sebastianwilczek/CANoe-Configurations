#include "capldll.h"

BOOL WINAPI DllMain(HINSTANCE handle, DWORD reason, void*)
{
  switch (reason)
  {
    case DLL_PROCESS_ATTACH:
    {
      // Get path to this DLL
      char modulePath[_MAX_PATH];
      GetModuleFileName(handle, modulePath, _MAX_PATH);

      // Initialize helper
      utils = new Utils(modulePath);

      return 1; // Indicate that the DLL was initialized successfully.
    }

    case DLL_PROCESS_DETACH:
    {
      return 1; // Indicate that the DLL was detached successfully.
    }
  }

  return 1;
}

long CAPLEXPORT far CAPLPASCAL FileExists(const char* relFilePath)
{
  return utils->FileExists(relFilePath);
}

double CAPLEXPORT far CAPLPASCAL GetAngle(double x1, double y1, double x2, double y2)
{
  return utils->GetAngle(x1, y1, x2, y2);
}

double CAPLEXPORT far CAPLPASCAL GetGeoAngle(double latPos1, double lonPos1, double latPos2, double lonPos2)
{
  return utils->GetGeoAngle(latPos1, lonPos1, latPos2, lonPos2);
}

long CAPLEXPORT far CAPLPASCAL CombineImageAndText1(const char* relPathOutput, const char* relPathImage, const char* text)
{
  return utils->CombineImageAndText(relPathOutput, relPathImage, text);
}

long CAPLEXPORT far CAPLPASCAL CombineImageAndText2(const char* relPathOutput, const char* relPathImage, const char* text, double textSize)
{
  return utils->CombineImageAndText(relPathOutput, relPathImage, text, textSize);
}

long CAPLEXPORT far CAPLPASCAL CombineImageAndText3(const char* relPathOutput, const char* relPathImage, const char* text, double textSize, long textOffsetX, long textOffsetY)
{
  return utils->CombineImageAndText(relPathOutput, relPathImage, text, textSize, textOffsetX, textOffsetY);
}

long CAPLEXPORT far CAPLPASCAL CombineImages1(const char* relPathOutput, const char* relPathImage1, const char* relPathImage2)
{
  return utils->CombineImages(relPathOutput, relPathImage1, relPathImage2);
}

long CAPLEXPORT far CAPLPASCAL CombineImages2(const char* relPathOutput, const char* relPathImage1, const char* relPathImage2, double scaleImage2)
{
  return utils->CombineImages(relPathOutput, relPathImage1, relPathImage2, scaleImage2);
}

long CAPLEXPORT far CAPLPASCAL CombineImages3(const char* relPathOutput, const char* relPathImage1, const char* relPathImage2, double scaleImage2, long offsetXImage2, long offsetYImage2)
{
  return utils->CombineImages(relPathOutput, relPathImage1, relPathImage2, scaleImage2, offsetXImage2, offsetYImage2);
}

long CAPLEXPORT far CAPLPASCAL GetRoadSignDescription1(char* output, long pictogramCode)
{
  return utils->GetRoadSignDescription(output, pictogramCode);
}

long CAPLEXPORT far CAPLPASCAL GetRoadSignDescription2(char* output, long pictogramCode, int maxLines, int charsPerLine)
{
  return utils->GetRoadSignDescription(output, pictogramCode, maxLines, charsPerLine);
}