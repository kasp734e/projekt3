#include "AudioFileWrapper.h"
#include "AudioWorker.h"
#include "ButtonWorker.h"
#include "StatusVars.h"
#include "pyInterface.h"
#include <LightWorker.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <csignal>
#include <iostream>
#include <string>

#define URL "http://192.168.50.100:8080"

void signalHandler(int signum) {
	std::cout << "Interrupt signal (" << signum << ") received.\n";

	StatusVars::programShouldExit = true;

	// Notify all condition variables to unblock threads
	StatusVars::cvPlay.notify_all();
	StatusVars::cvLightReadyPlayAudioNow.notify_all();
	StatusVars::cvTimeForNewSong.notify_all();
}

int main() {

	signal(SIGINT, signalHandler);

	// Start systems and store global pointers
	LightWorker lightWorker;
	AudioWorker audioWorker;
	ButtonWorker buttonWorker;
	ServerComs server_coms(URL);

	StatusVars::lightWorkerPointer = &lightWorker;
	StatusVars::audioWorkerPointer = &audioWorker;
	StatusVars::buttonWorkerPointer = &buttonWorker;
	StatusVars::server_coms = &server_coms;

	// Start button thread
	std::thread buttonThread(&ButtonWorker::worker, &buttonWorker);

	// Wait for first play button press
	std::mutex mainMtx;
	std::unique_lock<std::mutex> lk(mainMtx);
	std::cout << "Waiting for firstTimePlayWaitForPlayPause\n";
	StatusVars::firstTimePlayWaitForPlayPause.wait(
	    lk, [] { return !StatusVars::firstPlay; });
	std::cout << "Passed firstTimePlayWaitForPlayPause\n";

	// Start the rest of the system
	std::thread lightThread(&LightWorker::worker, &lightWorker);
	std::thread audioThread(&AudioWorker::worker, &audioWorker);

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	lightThread.join();
	audioThread.join();
	buttonThread.join();

	return 0;
}
