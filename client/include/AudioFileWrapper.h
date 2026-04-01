#pragma once

#include "AudioFile/AudioFile.h"
#include "simple_fft/fft.h"
#include <complex>
#include <exception>
#include <numeric>
#include <stdexcept>
#include <vector>

/**
 *
 * @brief Struct to store the data needed to process a song
 * @note Used by getAudioFileObjectFromPath, cutupVector and calculateFFT
 */

struct audioFileObject {
	std::vector<std::complex<double>> FFTReadyVector;
	size_t paddedSize;
	int sampleRate;
	int numSamples;
};

/**
 * @brief Takes a filepath string
 * @param path std::string to a filepath. Relative to where the program gets
 * run.
 * @return audioFileObject struct. Consists of the FFTReadyVector vector,
 * the padded size, the sample rate and the number of samples.
 */
audioFileObject getAudioFileObjectFromPath(const std::string &path);

/**
 * @brief Takes an audioFileObject object, does a Fourier transform on the data
 * and retuns a std::vector<pair> with the magnitudes for each frequency bin
 * @param inputObject Takes an audioFileObject object. These objects are created
 * by getAudioFileObjectFromPath
 * @return std::vector<std::pair<double, double>> Returns a vector with pairs of
 * doubles containing the frequency bin and the magnitude.
 */
std::vector<std::pair<double, double>>
calculateFFT(const audioFileObject &inputObject);

/**
 * @brief Cuts up a song from in a audioFileObject. DFT Hz are the amount of
 * bits pr second of song.
 * @param object Takes an audioFileObject with the info for a song. See
 * getAudioFileObjectFromPath()
 * @param DFTsPrSecond specifies the amount of output audiofile objects pr
 * second of song in the input object
 * @return std::vector<audioFileObject> Vector of audioFileObjects
 */
std::vector<audioFileObject> cutupVector(const audioFileObject &object,
                                         int DFTsPrSecond);
