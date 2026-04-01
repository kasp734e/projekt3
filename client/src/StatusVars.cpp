#include "StatusVars.h"

namespace StatusVars {

// Class instance pointers
LightWorker *lightWorkerPointer = nullptr;
AudioWorker *audioWorkerPointer = nullptr;
ButtonWorker *buttonWorkerPointer = nullptr;
ServerComs *server_coms = nullptr;

// Configuration variables
float volume = 0.5;
int DFTsPrSecond = 30;

// Internal variables needed for the program to function
std::filesystem::path currentPath = std::filesystem::path("");
std::array<std::string, 3> songPaths;
bool lightWorkerReady = false;
std::atomic<bool> isSkip = true;
std::atomic<bool> isPaused = false;
std::atomic<bool> firstPlay = true;
std::atomic<bool> programShouldExit = false;
std::atomic<bool> nextSongRequested = false;

// Conditional variables
std::condition_variable cvPlay;
std::condition_variable cvPause;
std::condition_variable firstTimePlayWaitForPlayPause;
std::condition_variable cvLightReady;
std::condition_variable cvLightReadyPlayAudioNow;
std::condition_variable cvTimeForNewSong;

// Mutexes
std::mutex mtxPlayLight;
std::mutex mtxLightReadyPlayAudioNow;
std::mutex currentPathMutex;
} // namespace StatusVars
