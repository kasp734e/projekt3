#include "AudioWorker.h"

AudioWorker::AudioWorker() {
	// Init engine and sound objects
	result = ma_engine_init(NULL, &engine);
	if (result != MA_SUCCESS) {
		std::cout << "Failed to initialize audio engine.\n";
		return;
	}
	ma_sound_init_from_file(&engine, "dummy.wav", 0, NULL, NULL, &sound);
	std::cout << "Audio engine initialized successfully.\n";
}

AudioWorker::~AudioWorker() {
	// Uninit engine
	ma_engine_uninit(&engine);
	std::cout << "Audio engine uninitialized.\n";
}

ma_engine *AudioWorker::getEngine() { return &engine; }

void AudioWorker::pause() {

	if (StatusVars::isPaused) { // Check so we don't "double pause"
		std::cout << "Audio is already paused.\n";
		return;
	} else {
		ma_device_stop(ma_engine_get_device(&engine)); // Pause song
		StatusVars::isPaused = true;
		std::cout << "Audio paused.\n";
		return;
	}
}

void AudioWorker::resume() {

	if (!StatusVars::isPaused) { // Check so we don't "double resume"
		std::cout << "Audio is already playing.\n";
		return;
	} else {
		ma_device_start(ma_engine_get_device(&engine)); // Resume song
		StatusVars::isPaused = false;
		StatusVars::cvPlay.notify_all();
		std::cout << "Audio resumed.\n";
		return;
	}
}

void AudioWorker::worker() {
	std::filesystem::path songToBePlayed;
	while (!StatusVars::programShouldExit) {

		{
			std::unique_lock<std::mutex> lk2(audioMutex);
			StatusVars::cvTimeForNewSong.wait(
			    lk2, [] { return StatusVars::programShouldExit || true; });
		}
		if (StatusVars::programShouldExit)
			break;

		// Wait for LightWorker to be ready with the FFT data for this song.
		{
			std::unique_lock<std::mutex> lk(
			    StatusVars::mtxLightReadyPlayAudioNow);
			StatusVars::cvLightReadyPlayAudioNow.wait(lk, [] {
				return StatusVars::programShouldExit ||
				       StatusVars::lightWorkerReady;
			}); // Wait with a predicate
			if (StatusVars::programShouldExit)
				break;
			std::cout << "AudioWorker proceeding to play audio after "
			             "LightWorker signal.\n";
		}

		// Uninitialize the previous sound before loading a new one.
		{
			std::unique_lock<std::mutex> lk(StatusVars::currentPathMutex);
			ma_sound_uninit(&sound);
			result = ma_sound_init_from_file(
			    &engine, StatusVars::currentPath.string().c_str(), 0, NULL,
			    NULL, &sound);
		}
		if (result != MA_SUCCESS) {
			std::cout << "Failed to initialize sound from file: "
			          << songToBePlayed << std::endl;
			continue; // Skip to next loop iteration
		}

		ma_sound_set_volume(&sound, StatusVars::volume);
		ma_sound_start(&sound);

		// Wait for the song to finish by polling the sound object itself.
		while (ma_sound_is_playing(&sound) && !StatusVars::programShouldExit) {
			if (StatusVars::nextSongRequested) {
				ma_sound_stop(&sound);
				StatusVars::nextSongRequested = false; // Reset flag
				break; // Exit loop to fetch the next song
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		if (StatusVars::programShouldExit) {
			ma_sound_stop(&sound);
		}
		std::cout << "Song finished or was skipped." << std::endl;
		StatusVars::lightWorkerReady = false;
	}
}
