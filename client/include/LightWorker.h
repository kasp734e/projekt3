#pragma once
#include "AudioFileWrapper.h"
#include "StatusVars.h"
#include "pyInterface.h"
#include "server_coms.h"
#include <chrono>
#include <vector>
class LightWorker {
  public:
	LightWorker();
	~LightWorker();

	// Function that runs in the lightWorker thread
	void worker();

	// Holds the edges of the frequency bands for mapping DFT to LED matrix
	std::array<int, 17> bandWidths = {0,     78,    313,   703,   1250, 1953,
	                                  2813,  3828,  5000,  6328,  7813, 9453,
	                                  11250, 13203, 15313, 17578, 20000}; 
};
