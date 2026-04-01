#pragma once
#include "StatusVars.h"
#include <linux/gpio.h>
#include <vector>

#define RewindSongButton 0
#define PlayPauseButton 5
#define SkipSongButton 6
#define VolumeDownButton 13
#define ShuffleSongButton 19
#define VolumeUpButton 26

// #define PlayPauseButton 13

class ButtonWorker {
  private:
  // Class for cleaning up button presses
	struct EdgeDetector {
		int lastValue = 1;

		EdgeDetector(int initialValue) : lastValue(initialValue) {};
		bool valueChanged(int value) {
			bool falling = (value != lastValue && value == 0);
			lastValue = value;
			return falling;
		}
	};
	EdgeDetector *edgeDetectors[6];

	// Internals for the button communication
	int chip_fd = -1; 
	struct gpiohandle_request req_in; 
	struct gpiohandle_data data_in;


  public:
	ButtonWorker();
	~ButtonWorker();
	void readMusicButtons();
	void readGPIOs();

	void worker(); // Function that runs in the buttonWorker thread. Polls the buttons for input

	enum ButtonMap {
		PLAYPAUSE = 0,
		SKIPSONG = 1,
		REWIND = 2,
		SHUFFLE = 3,
		VOLUMEUP = 4,
		VOLUMEDOWN = 5,
		NONE = 6
	};

	ButtonMap lastPressedButton = NONE;
};
