#include "ButtonWorker.h"
#include "AudioWorker.h"
#include "StatusVars.h"
#include "server_coms.h"
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
ButtonWorker::ButtonWorker() {

	for (int i = 0; i < 6; i++) {
		edgeDetectors[i] = new EdgeDetector(1);
	}

	chip_fd = open("/dev/gpiochip0", O_RDONLY); // open the io chip in read mode
	if (chip_fd < 0) {                          // check for errors
		std::cerr << "Failed to open /dev/gpiochip0: " << strerror(errno)
		          << std::endl;
		exit(EXIT_FAILURE);
	}

	memset(&req_in, 0, sizeof(req_in)); // zeros out the req struct

	req_in.lines = 6; // ask for 6 gpio lines
	req_in.lineoffsets[0] = PlayPauseButton;
	req_in.lineoffsets[1] = SkipSongButton;
	req_in.lineoffsets[2] = RewindSongButton;
	req_in.lineoffsets[3] = ShuffleSongButton;
	req_in.lineoffsets[4] = VolumeUpButton;
	req_in.lineoffsets[5] = VolumeDownButton;
	req_in.flags =
	    GPIOHANDLE_REQUEST_INPUT |
	    GPIOHANDLE_REQUEST_BIAS_PULL_UP; // set as input with pull-up resistors

	strcpy(req_in.consumer_label, "button-input"); // label for the request

	// get linehandles and check for errors
	if (ioctl(chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &req_in) < 0) {
		std::cerr << "GPIO_GET_LINEHANDLE_IOCTL (button) failed: "
		          << strerror(errno) << std::endl;
		close(chip_fd);
		exit(EXIT_FAILURE);
	}

	if (ioctl(req_in.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data_in) >= 0) {
		for (int i = 0; i < 6; i++) {
			edgeDetectors[i]->lastValue = data_in.values[i];
		}
	}
}

// close file descriptors
ButtonWorker::~ButtonWorker() {
	for (int i = 0; i < 6; i++) {
		delete edgeDetectors[i];
	}

	if (req_in.fd >= 0) {
		close(req_in.fd);
		req_in.fd = -1;
	}

	if (chip_fd >= 0) {
		close(chip_fd);
		chip_fd = -1;
	}
}

void ButtonWorker::readMusicButtons() {
	// Get a new button value
	readGPIOs();

	// Check the new button value
	switch (lastPressedButton) {
	case PLAYPAUSE:
		std::cout << "you pressed Play/Pause" << std::endl;

		// Logic that waits for the first playPause press on startup
		if (StatusVars::firstPlay) {
			StatusVars::firstPlay = false;
			StatusVars::firstTimePlayWaitForPlayPause.notify_all();
			break;
		}

		// Normal playPause logic
		if (StatusVars::isPaused) {
			StatusVars::audioWorkerPointer->resume();
		} else {
			StatusVars::audioWorkerPointer->pause();
		}
		break;

	case SKIPSONG:
		std::cout << "you pressed Skip" << std::endl;
		// Set appropriate flags
		StatusVars::nextSongRequested = true;
		StatusVars::isSkip = true;

		// Notify that it's time for a new song
		StatusVars::cvTimeForNewSong.notify_all();
		break;

	case REWIND:
		std::cout << "you pressed Rewind" << std::endl;
		// Set appropriate flags
		StatusVars::nextSongRequested = true;
		StatusVars::isSkip = false;

		// Notify that it's time for a new song
		StatusVars::cvTimeForNewSong.notify_all();
		break;

	case SHUFFLE:
		std::cout << "you pressed Shuffle" << std::endl;
		StatusVars::server_coms->shufflePlaylist();
		break;

	case VOLUMEUP:

		// Calculate the new volume
		if (StatusVars::volume < 1.0) { // check upper limit
			StatusVars::volume += 0.1;
		}
		if (StatusVars::volume > 1.0) {
			StatusVars::volume = 1.0; // clamp to 1.0
		}

		std::cout << "Volume increased to " << StatusVars::volume << std::endl;

		// Use the pointer to AudioWorker to get the engine
		ma_engine_set_volume(StatusVars::audioWorkerPointer->getEngine(),
		                     StatusVars::volume);
		break;

	case VOLUMEDOWN:

		// Calculate the new volume
		if (StatusVars::volume > 0.0) { // check lower limit
			StatusVars::volume -= 0.1;
		}
		if (StatusVars::volume < 0.0) {
			StatusVars::volume = 0.0; // clamp to 0.0
		}

		std::cout << "Volume decreased to " << StatusVars::volume << std::endl;

		// Use the pointer to AudioWorker to get the engine and set new volume
		ma_engine_set_volume(StatusVars::audioWorkerPointer->getEngine(),
		                     StatusVars::volume);
		break;

	case NONE:
		// ingen knapper trykkes
		break;
	}
}

void ButtonWorker::readGPIOs() {
	if (ioctl(req_in.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data_in) < 0) {
		std::cerr << "GET_LINE_VALUES (button) failed: " << strerror(errno)
		          << std::endl;
		return;
	}

	// input is active high, read the buttons and update lastPressedButton
	// class member
	if (edgeDetectors[0]->valueChanged(data_in.values[0])) {
		lastPressedButton = PLAYPAUSE;
	} else if (edgeDetectors[1]->valueChanged(data_in.values[1])) {
		lastPressedButton = SKIPSONG;
	} else if (edgeDetectors[2]->valueChanged(data_in.values[2])) {
		lastPressedButton = REWIND;
	} else if (edgeDetectors[3]->valueChanged(data_in.values[3])) {
		lastPressedButton = SHUFFLE;
	} else if (edgeDetectors[4]->valueChanged(data_in.values[4])) {
		lastPressedButton = VOLUMEUP;
	} else if (edgeDetectors[5]->valueChanged(data_in.values[5])) {
		lastPressedButton = VOLUMEDOWN;
	} else {
		lastPressedButton = NONE;
	}
}

void ButtonWorker::worker() {
	while (
	    !StatusVars::programShouldExit) { // Exits when exit is wanted from main
		readMusicButtons();               // Reads the buttons
		usleep(100000);                   // Sleep for 0.1 seconds
	}
}
