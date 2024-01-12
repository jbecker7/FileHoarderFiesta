#include <chrono>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

using namespace std;
namespace fs = std::__fs::filesystem;

bool createDirectory(const fs::path& dir) {
  if (!fs::exists(dir) && !fs::create_directory(dir)) {
    cout << "Failed to create directory: " << dir << endl;
    return false;
  }
  return true;
}

bool isScreenshot(const string& filename) {
  return filename.rfind("Screenshot", 0) == 0;
}

void moveFileToDirectory(const fs::path& file, const fs::path& targetDir,
                         bool makeNoise = false, string noise = "sound.wav") {
  try {
    fs::path destination = targetDir / file.filename();
    fs::rename(file, destination);
    if (makeNoise) {
      string command =
          "afplay " +
          noise;  // No need to use '../', sound files are in the same directory
      system(command.c_str());
    }
    cout << "Moved: " << file.filename() << endl;
  } catch (const fs::filesystem_error& e) {
    cerr << "Failed to move " << file.filename() << ": " << e.what() << endl;
  }
}

int main() {
  cout << "Do you want it to make a noise when it moves a file? (y/n) ";
  char response;
  string noise;
  cin >> response;
  bool makeNoise = response == 'y';

  if (makeNoise) {
    cout << "Do you want a funny noise or a normal noise? (f/n)" << endl;
    cin >> response;
    if (response == 'f') {
      noise = "funny.wav";
    } else {
      noise = "normal.wav";
    }
  }
  cout << "Starting..." << endl;
  cout << "Press Ctrl+C to stop." << endl;
  cout << "Otherwise, it will run every 10 minutes." << endl;
  cout << "-----------------------------------------" << endl;
  while (true) {
    fs::path targetDir = "images";
    if (!createDirectory(targetDir)) {
      return 1;
    }

    for (const auto& entry :
         fs::directory_iterator(fs::current_path().parent_path())) {
      if (entry.is_regular_file() &&
          isScreenshot(entry.path().filename().string())) {
        moveFileToDirectory(entry.path(), targetDir, makeNoise, noise);
      }
    }

    this_thread::sleep_for(chrono::minutes(10));
  }

  return 0;
}
