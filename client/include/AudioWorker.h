#pragma once
#include "StatusVars.h"
#include "miniaudio.h"
#include <filesystem>
#include <iostream>
class AudioWorker {
  public:
	AudioWorker();
	~AudioWorker();
	void worker(); // Function that runs in the audioWorker thread
	void pause(); // Pauses the playing of the song
	void resume(); // Resumes the playing of the song
	ma_engine *getEngine(); // Returns a pointer to the engine used by the AudioWorker

  private:
	ma_result result; // Result of the initialization of the ma_engine
	ma_engine engine; // The internal object needed for the playback to function
	ma_sound sound; // The object representing the current sound (song)
	std::mutex audioMutex;
};
