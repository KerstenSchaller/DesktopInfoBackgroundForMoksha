#include <iostream>

#include <X11/Xlib.h>

#define cimg_display 0
#include "CImg.h"

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <thread>
#include <chrono>

using namespace cimg_library;
using namespace std;

void createImage(string text, string path)
{
  // get screen resolution
  Display *display = XOpenDisplay(NULL);
  Screen *screen = DefaultScreenOfDisplay(display);

  // Create 640x480 image
  CImg<unsigned char> image(screen->width, screen->height, 1, 3);

  // Fill with black
  cimg_forXY(image, x, y)
  {
    image(x, y, 0, 0) = 0;
    image(x, y, 0, 1) = 0;
    image(x, y, 0, 2) = 0;
  }

  // Make some colours
  unsigned char white[] = {255, 255, 255};
  unsigned char black[] = {0, 0, 0};

  // Draw text
  image.draw_text(30, 150, text.c_str(), white, black, 1, 32);

  // Save result image
  image.save_bmp(path.c_str());
}

// exec bash command and get return value as string
std::string exec(const char *cmd)
{
  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if (!pipe)
  {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
  {
    result += buffer.data();
  }
  return result;
}

/*
 * Erase First Occurrence of given  substring from main string.
 */
string eraseSubStr(std::string &mainStr, const std::string &toErase)
{
  // Search for the substring in string
  size_t pos = mainStr.find(toErase);
  if (pos != std::string::npos)
  {
    // If found then erase it from string
    mainStr.erase(pos, toErase.length());
  }
  return mainStr;
}

int main(int argc, char *argv[])
{
  while (true)
  {
    string statusText = exec("nordvpn status");

    string directory;
    if (argc > 1)
    {
      directory = argv[1];
    }
    else
    {
      cout << "Error: No Filepath given" << endl;
      cout << "Please specify path where a temp file can be saved." << endl;
      return EXIT_FAILURE;
    }
    string workDir = argv[0];
    workDir = eraseSubStr(workDir, "/DesktopbackgroundInfoScreen");

    string filepath = directory + "/info.bmp";
    createImage(statusText, filepath);
    string cmdstring = workDir + "/setWallpaper " + filepath + " manual >/dev/null 2>&1";
    system(cmdstring.c_str());

    this_thread::sleep_for(chrono::seconds(2));
  }

  return EXIT_SUCCESS;
}
