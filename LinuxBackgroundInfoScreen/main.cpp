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

#include <fstream>

using namespace cimg_library;
using namespace std;

bool fileExists(const std::string &name)
{
  ifstream f(name.c_str());
  return f.good();
}

class InfoScreen
{

public:
  inline InfoScreen(string argv0, string directory, bool debug)
  {
    workDir = argv0;
    workDir = eraseSubStr(workDir, "/DesktopbackgroundInfoScreen");
    filepath = directory + "/info.bmp";
    debugFlag = debug;
    // get screen resolution
    Display *display = XOpenDisplay(NULL);
    Screen *screen = DefaultScreenOfDisplay(display);
    x_res = screen->width;
    y_res = screen->height;
    log("info.bmp will be written to path: " + filepath, true);
    log("setWallpaper script will be searched in path: " + workDir, true);
  }
  inline int setStatusText()
  {
    statusText = UpdateNordVPN();
    bool statechanged = setState(statusText);
    if (statechanged || ( count >= 10 ))
    {
      createImage(statusText, filepath);
      string cmdstring = workDir + "/setWallpaper " + filepath + " manual >/dev/null 2>&1";
      exec(cmdstring.c_str());
      count = 0;
    }
    count++;
    return 2;
  }

  inline bool setState(string text)
  {
      bool stateChanged = false;
      if(text.find("Disconnected")!= string::npos)
      {
        currentState = DISCONNECTED;
      }
      else
      {
        if(text.find("Connected") != string::npos)
        {
          currentState = CONNECTED;
        }
      }

      if(lastState != currentState)
      {
        stateChanged = true;
        log("State changed.", debugFlag);
      }
      else
      {
        stateChanged = false;
      }
      lastState = currentState;
      return stateChanged;


  }

private:
  string errortext = "";
  string statusText = "";
  string workDir;
  string filepath;
  bool debugFlag = false;
  int count = 0;
  typedef enum State{CONNECTED, DISCONNECTED} State_e;
  State lastState = DISCONNECTED;
  State currentState = DISCONNECTED;

  int x_res;
  int y_res;

  inline void createImage(string text, string path)
  {

    // Create image
    CImg<unsigned char> image(x_res, y_res, 1, 3);

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
  inline std::string exec(const char *cmd)
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
  inline string eraseSubStr(std::string &mainStr, const std::string &toErase)
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

  void notifyUser(string text)
  {
    string fileName = "/usr/bin/notify-send ";
    string commandString = fileName + "\"" + text + "\"";
    exec(commandString.c_str());
  }

  string UpdateNordVPN()
  {
    return exec("nordvpn status");
  }

  void log(string text, bool debugflag)
  {
    if (debugflag)
    {
      cout << string(text).c_str() << endl;
    }
  }
};

int main(int argc, char *argv[])
{

  string directory;
  if (argc >= 2)
  {
    directory = argv[1];
    if(directory.back() == '/')
    {
      directory.pop_back();
    }
  }
  else
  {
    cout << "Error: No Filepath given" << endl;
    cout << "Please specify path where a temp file can be saved." << endl;
    return EXIT_FAILURE;
  }
  bool debugflag = ((argc == 3) && (string(argv[2]) == "d") && (fileExists("/usr/bin/notify-send")));
  InfoScreen infoScreen(argv[0], directory, debugflag);

  while (true)
  {
    try
    {
      int sleeptime = infoScreen.setStatusText();
      this_thread::sleep_for(chrono::seconds(sleeptime));
    }
    catch (const std::exception &e)
    {
      if (debugflag)
      {
        cout << "error: " << e.what();
      }
    }
  }

  return EXIT_SUCCESS;
}
