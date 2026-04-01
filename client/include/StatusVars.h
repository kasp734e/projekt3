#pragma once

#include "miniaudio.h"
#include <array>
#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <string>

// Forward declarations
class AudioWorker;
class ButtonWorker;
class LightWorker;
class ServerComs;

namespace StatusVars {

// Class instance pointers
extern LightWorker *lightWorkerPointer;
extern AudioWorker *audioWorkerPointer;
extern ButtonWorker *buttonWorkerPointer;
extern ServerComs *server_coms;

// Configuration variables
extern float volume;
extern int DFTsPrSecond;

// Internal variables needed for the program to function
extern std::filesystem::path currentPath;
extern std::array<std::string, 3> songPaths;
extern bool lightWorkerReady;
extern std::atomic<bool> isSkip;
extern std::atomic<bool> isPaused;
extern std::atomic<bool> firstPlay;
extern std::atomic<bool> programShouldExit;
extern std::atomic<bool> nextSongRequested;

// Conditional variables
extern std::condition_variable cvPlay;
extern std::condition_variable cvPause;
extern std::condition_variable firstTimePlayWaitForPlayPause;
extern std::condition_variable cvLightReady;
extern std::condition_variable cvLightReadyPlayAudioNow;
extern std::condition_variable cvTimeForNewSong;

// Mutexes
extern std::mutex mtxPlayLight;
extern std::mutex mtxLightReadyPlayAudioNow;
extern std::mutex currentPathMutex;
} // namespace StatusVars
