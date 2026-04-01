#include "LightWorker.h"

LightWorker::LightWorker() = default;
LightWorker::~LightWorker() = default;

void LightWorker::worker() {

	// Initialize PythonLightInterface
	std::cout << "LightWorker started." << std::endl;
	PythonLightInterface pyLightInterface;
	std::cout << "PythonLightInterface initialized." << std::endl;

	while (!StatusVars::programShouldExit) {
		// Goes here when new song is needed

		audioFileObject newSong;

		if (StatusVars::programShouldExit) { // For ctrl-c
			break;
		}

		{
			std::unique_lock<std::mutex> lk(
			    StatusVars::currentPathMutex); // Gets mutex to use currentPath

			if (StatusVars::isSkip) { // Checks if we need to go to next or prev
				                      // song
				std::cout << "Going to next song in LightWorker." << std::endl;
				StatusVars::currentPath =
				    StatusVars::server_coms->getNext().string();
			} else {
				StatusVars::currentPath =
				    StatusVars::server_coms->getPrevious().string();
				std::cout << "Going to previous song in LightWorker."
				          << std::endl;
			}

			// Converts into a audioFileObject
			newSong =
			    getAudioFileObjectFromPath(StatusVars::currentPath.string());
		}

		std::cout << "Loaded new song for light processing." << std::endl;

		// Cut up the vector for processing
		std::vector<audioFileObject> songsBits =
		    cutupVector(newSong, StatusVars::DFTsPrSecond);

		// Play the audio
		{
			std::lock_guard<std::mutex> lk(
			    StatusVars::mtxLightReadyPlayAudioNow);
			StatusVars::lightWorkerReady = true;
		}
		StatusVars::cvLightReadyPlayAudioNow.notify_all();

		int currentBitIndex = 0;
		int firstPlay = true;
		bool skipRequested = false;

		while (!StatusVars::programShouldExit && !skipRequested) {

			// goes here when paused

			if (currentBitIndex >= songsBits.size()) {
				std::cout << "Going to process next song in LightWorker."
				          << std::endl;
				break;
			}

			std::cout << "LightWorker waiting for play signal." << std::endl;
			if (!firstPlay) { // skal ignoreres første gang
				std::unique_lock<std::mutex> lk(StatusVars::mtxPlayLight);
				StatusVars::cvPlay.wait(lk, [] {
					return StatusVars::programShouldExit ||
					       !StatusVars::isPaused ||
					       StatusVars::nextSongRequested; // Also wake on skip
				});
			}
			if (StatusVars::programShouldExit)
				break;

			if (StatusVars::nextSongRequested) { // Check before continuing
				skipRequested = true;
				break;
			}

			std::cout << "LightWorker received play signal." << std::endl;

			while (!StatusVars::isPaused && !StatusVars::programShouldExit) {

				firstPlay = false;
				auto timeStart = std::chrono::high_resolution_clock::now();

				if (StatusVars::nextSongRequested) {
					std::cout << "Going to next song in LightWorker."
					          << std::endl;
					skipRequested = true;
					break;
				}

				if (currentBitIndex >= songsBits.size()) {
					std::cout << "Completed all segments for current song."
					          << std::endl;
					break;
				}

				audioFileObject currentBit = songsBits[currentBitIndex];

				std::vector<std::pair<double, double>> fftResult =
				    calculateFFT(currentBit); // (frequency, amplitude) pairs

				// Map frequencies to bands
				std::array<double, 16> bandAmplitudes = {0.0};
				std::array<int, 16> bandCount = {0};

				for (const auto &pair : fftResult) {
					double frequency = pair.first;
					double amplitude = pair.second;

					for (size_t band = 0; band < bandWidths.size() - 1;
					     ++band) {
						if ((frequency <= bandWidths[band + 1]) &&
						    (frequency > bandWidths[band])) {

							// Average amplitudes in band
							bandAmplitudes[band] += amplitude;
							bandCount[band] += 1;
							break;
						}
					}
				}
				for (size_t band = 0; band < bandAmplitudes.size(); ++band) {
					if (bandCount[band] > 0) {
						bandAmplitudes[band] /=
						    static_cast<double>(bandCount[band]);
					}
				}

				// Normalize and convert to percentage
				std::array<int, 16> bandPercentages = {0};
				for (size_t band = 0; band < bandAmplitudes.size(); ++band) {
					double dbValue = 20.0 * log10(bandAmplitudes[band] +
					                              1e-6); // avoid log(0)
					double normalized = (dbValue + 60.0) / 60.0;
					if (normalized < 0.0)
						normalized = 0.0;
					if (normalized > 1.0)
						normalized = 1.0;
					bandPercentages[band] =
					    static_cast<int>(normalized * 100.0);
				}

				;
				pyLightInterface.setLines(bandPercentages);

				auto timeEnd = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double, std::milli> elapsed =
				    timeEnd - timeStart;
				double targetDurationMs = (1000.0 / StatusVars::DFTsPrSecond);
				if (elapsed.count() < targetDurationMs) {

					std::this_thread::sleep_for(std::chrono::milliseconds(
					    static_cast<int>(targetDurationMs - elapsed.count())));
				}
				currentBitIndex++;
			}
		}
		if (StatusVars::programShouldExit) {
			break;
		}
		StatusVars::cvTimeForNewSong.notify_all();
	}
}
