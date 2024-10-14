#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "key_codes.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace cv;
using namespace std;

enum class WinID {
  MAIN_WIN_ID,
  ZOOM_WIN_ID
}

const float ZoomFactor = 0.3;
const char *MainWinName = "Main";
const char *ZoomWinName = "Zoom";
Mat *MainMat;

Mat *FocusMat;
unique_ptr<Mat> ZoomMat;

// Map Mat to window name
unordered_map<int, shared_ptr<Mat>> MatMap;
unordered_map<int, string> WinNameMap;

// TODO: function to remove window

optional<shared_ptr<Mat>> getWin(WinID ID) {
  auto Itr = WinNameMap.find(ID);
  if (Itr == WinNameMap.end()) {
    return nullopt;
  }
  string &WinName = Itr->second;
  // Check if window exists
  bool Exists = getWindowProperty(WinName, WND_PROP_VISIBLE);
  if (!Exists) {
    // Window is closed by other people
    // TODO: Check if Mat exists to clean
    return nullopt;
  }

  auto MatItr = MatMap.find(ID);

  // FIXME

  return MatWindowMap[WinName];
}

void CallBackFunc(int event, int x, int y, int flags, void *userdata) {
  if (event == EVENT_LBUTTONDOWN) {
    cout << "Left button of the mouse is clicked - position (" << x << ", " << y
         << ")" << endl;
  } else if (event == EVENT_RBUTTONDOWN) {
    cout << "Right button of the mouse is clicked - position (" << x << ", "
         << y << ")" << endl;
  }
#if 0
  else if  ( event == EVENT_MBUTTONDOWN )
  {
    cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
  }
  else if ( event == EVENT_MOUSEMOVE )
  {
    cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
  }
#endif
}

bool handleKey(int Key) {
  switch (Key) {
  case KEY_CODE_PLUS: {
    auto Mat = getWinMat(ZOOM_WIN_ID);
    if (Mat) {
      cout << "Zoom In" << endl;
      // Mat ROI = (*FocusMat)(Select);
      // imshow(ZoomWinName, *ZoomMat);
    }
    break;
  }
  case KEY_CODE_MINUS:
  case KEY_CODE_LEFT:
  case KEY_CODE_UP:
  case KEY_CODE_RIGHT:
  case KEY_CODE_DOWN: {
    cout << "known keys" << endl;
    break;
  }
  case KEY_CODE_Z: {
    // TODO: Destroy Existing Zoom window
    cout << "Select zoom" << endl;
    auto Mat = getWinMat(MAIN_WIN_ID);
    if (!Mat) {
      break;
    }
    Rect Select = selectROI(MainWinName, *MainMat);
    cout << "Selected: (" << Select.x << "," << Select.y
         << ") h:" << Select.height << ", w:" << Select.width << endl;

    Mat ROI = (*FocusMat)(Select);
    ZoomMat = make_unique<Mat>(ROI);
    string ZoomWinName = "Zoom Window";
    namedWindow(ZoomWinName, WINDOW_KEEPRATIO); // Create a window for display.
    imshow(ZoomWinName, *ZoomMat);
    // MatWindowMap[ZoomWinName] = ZoomMat;

    break;
  }
  case KEY_CODE_ESC:
    cout << "ESC key" << endl;
    return true;
  }
  return false;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    cout << " Usage: " << argv[0] << " ImageToLoadAndDisplay" << endl;
    return -1;
  }

  Mat image;
  image = imread(argv[1], IMREAD_COLOR); // Read the file
  MainMat = &image;
  int Rows = image.rows;
  int Cols = image.cols;
  cout << "Read image, rows: " << image.rows << ", cols: " << image.cols
       << endl;

  if (image.empty()) // Check for invalid input
  {
    cout << "Could not open or find the image" << endl;
    return -1;
  }

  Mat ZoomImg = image(Range(50, 150), Range(50, 200));

  // namedWindow( "Display window", WINDOW_FULLSCREEN); // Create a window for
  // display.
  namedWindow(MainWinName, WINDOW_AUTOSIZE); // Create a window for display.
  // setMouseCallback(MainWinName, CallBackFunc, NULL);
  imshow(MainWinName, image); // Show our image inside it.

  // namedWindow( "Zoom window", WINDOW_KEEPRATIO ); // Create a window for
  // display. imshow( "Zoom window", ZoomImg ); // Show our image inside it.

  int i = 0;
  while (true) {
    int Key = waitKeyEx(0); // Wait for a keystroke in the window
    bool IsExit = handleKey(Key);
    if (IsExit)
      break;
  }
  return 0;
}
