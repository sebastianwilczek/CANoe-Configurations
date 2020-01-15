#include "utils.h"

Utils::Utils() { }

Utils::Utils(path modulePath)
{
  this->modulePath = modulePath;

  this->InitializeRoadSignCodes();
}

#pragma region Path functions

path Utils::ConfigPath()
{
  // modulePath = ...\Config\CAPLdll\Release\capldll.dll
  return path(modulePath).parent_path().parent_path().parent_path();
}

path Utils::FullPath(const char* relativePath)
{
  return ConfigPath().append("\\").append(relativePath);
}

long Utils::FileExists(const char* relativePath)
{
  return exists(FullPath(relativePath));
}

#pragma endregion

#pragma region Math functions

double Utils::GetAngle(double x1, double y1, double x2, double y2)
{
  double theta = (180.0 / M_PI) * atan2(y2 - y1, x2 - x1);

  double degreesNorth = 90.0 - theta;

  if (degreesNorth < 0)
  {
    degreesNorth += 360.0;
  }

  return degreesNorth;
}

double Utils::GetGeoAngle(double latPos1, double lonPos1, double latPos2, double lonPos2)
{
  double earthRadius = 6371000.8;
  double radPerDegree = M_PI / 180.0;
  double latConversionFactor = earthRadius * radPerDegree;

  double result = 0.0;

  double diffLat = (latPos2 - latPos1) * latConversionFactor;
  double diffLon = (lonPos2 - lonPos1) * latConversionFactor * cos(latPos1 * M_PI / 180.0);

  if (diffLat == 0)
  {
    if (diffLon < 0)
    {
      return 270;
    }
    else if (diffLon > 0)
    {
      return 90;
    }
  }
  else
  {
    result = 180.0 * atan(abs(diffLon / diffLat)) / M_PI;

    if (diffLat < 0)
    {
      if (diffLon > 0)            // 90°-180°
      {
        result = 180.0 - result;
      }
      else                        // 180°-270°
      {
        result += 180.0;
      }
    }
    else if (diffLon < 0)         // 270°-360°
    {
      result = 360.0 - result;
    }
  }

  return result;
}

#pragma endregion

#pragma region Pictogram functions

long Utils::GetEncoderClsid(const WCHAR* mimeType, CLSID* pClsid)
{
  UINT num = 0;  // Number of image encoders
  UINT size = 0; // Size of the image encoder array in bytes

  ImageCodecInfo* pImageCodecInfo = NULL;

  GetImageEncodersSize(&num, &size);
  if (size == 0)
  {
    return -1; // Failure
  }

  pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
  if (pImageCodecInfo == NULL)
  {
    return -1; // Failure
  }

  GetImageEncoders(num, size, pImageCodecInfo);

  for (UINT j = 0; j < num; ++j)
  {
    if (wcscmp(pImageCodecInfo[j].MimeType, mimeType) == 0)
    {
      *pClsid = pImageCodecInfo[j].Clsid;
      free(pImageCodecInfo);
      return j; // Success
    }
  }

  free(pImageCodecInfo);
  return -1; // Failure
}

long Utils::GetEncoderClsid(string fileExtension, CLSID* pClsid)
{
  const WCHAR *mimeType;

  if (fileExtension == ".bmp")
  {
    mimeType = L"image/bmp";
  }
  else if (fileExtension == ".png")
  {
    mimeType = L"image/png";
  }
  else if (fileExtension == ".jpeg")
  {
    mimeType = L"image/jpeg";
  }
  else if (fileExtension == ".gif")
  {
    mimeType = L"image/gif";
  }
  else if (fileExtension == ".tiff")
  {
    mimeType = L"image/tiff";
  }
  else
  {
    return -1; // Failure
  }

  return GetEncoderClsid(mimeType, pClsid);
}

long Utils::SaveImage(Bitmap* image, path pathOutput)
{
  CLSID clsid;
  if (GetEncoderClsid(pathOutput.extension().string(), &clsid) == -1)
  {
    return -1; // Failure
  }

  if (!exists(pathOutput.parent_path()))
  {
    create_directory(pathOutput.parent_path());
  }

  return image->Save(pathOutput.wstring().c_str(), &clsid, NULL);
}

void Utils::InitializeRoadSignCodes()
{
  ifstream file;
  file.open(FullPath("RoadSignCodes\\ISO14823.txt").string());

  string line;
  while (getline(file, line))
  {
    size_t delimiterPos = line.find(":");
    string number = line.substr(0, delimiterPos);
    string description = line.substr(delimiterPos + 1);

    roadSignCodes[stol(number)] = description;
  }
}

long Utils::CombineImageAndText(const char* relPathOutput, const char* relPathImage, const char* text, double textSize, long textOffsetX, long textOffsetY)
{
  GdiplusInit gdiplusinit;

  Bitmap *image = Bitmap::FromFile(FullPath(relPathImage).wstring().c_str());

  int w = image->GetWidth();
  int h = image->GetHeight();

  Bitmap *output = new Bitmap(w, h);
  Graphics g(output);

  Font font(L"Arial", (float)textSize, FontStyleBold, UnitPixel);
  PointF center((float)w / 2 + textOffsetX, (float)h / 2 + textOffsetY);
  SolidBrush brush(Color(255, 0, 0, 0));
  StringFormat format = new StringFormat();
  format.SetAlignment(StringAlignmentCenter);
  format.SetLineAlignment(StringAlignmentCenter);

  g.DrawImage(image, 0, 0);
  g.DrawString(path(text).wstring().c_str(), -1, &font, center, &format, &brush);

  return SaveImage(output, FullPath(relPathOutput));
}

long Utils::CombineImages(const char* relPathOutput, const char* relPathOuter, const char* relPathInner, double innerScale, long innerOffsetX, long innerOffsetY)
{
  GdiplusInit gdiplusinit;

  Bitmap *outerImage = Bitmap::FromFile(FullPath(relPathOuter).wstring().c_str());
  Bitmap *innerImage = Bitmap::FromFile(FullPath(relPathInner).wstring().c_str());

  int w = outerImage->GetWidth();
  int h = outerImage->GetHeight();

  Bitmap *output = new Bitmap(w, h);
  Graphics g(output);

  if (innerScale == 1.0)
  {
    g.DrawImage(outerImage, 0, 0);
    g.DrawImage(innerImage, innerOffsetX, innerOffsetY);
  }
  else
  {
    Bitmap *innerImageScaled = new Bitmap(innerImage->GetWidth(), innerImage->GetHeight());
    Graphics gs(innerImageScaled);

    gs.ScaleTransform((float)innerScale, (float)innerScale);
    gs.DrawImage(innerImage, 0, 0);

    int posX = (int)(w * (1.0 - innerScale) * 0.5);
    int posY = (int)(h * (1.0 - innerScale) * 0.5);

    g.DrawImage(outerImage, 0, 0);
    g.DrawImage(innerImageScaled, posX + innerOffsetX, posY + innerOffsetY);
  }

  return SaveImage(output, FullPath(relPathOutput));
}

long Utils::GetRoadSignDescription(char* output, long pictogramCode, int maxLines, int charsPerLine)
{
  long result;

  stringstream description;
  stringstream formattedDescription;

  if (roadSignCodes.find(pictogramCode) == roadSignCodes.end())
  {
    description << "Info N/A " << pictogramCode;
    result = -1;
  }
  else
  {
    description << roadSignCodes[pictogramCode];
    result = 0; 
  }

  string s = description.str();
  for (string::size_type i = 0; i < s.size(); i++)
  {
    formattedDescription << s[i];

    if (i > 0 && i % charsPerLine == 0)
    {
      formattedDescription << "\n";
    }

    if (i > charsPerLine * maxLines - 3)
    {
      formattedDescription << "...";
      break;
    }
  }

  strcpy(output, formattedDescription.str().c_str());

  return result;
}

#pragma endregion