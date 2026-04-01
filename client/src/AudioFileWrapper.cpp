#include "AudioFileWrapper.h"

audioFileObject getAudioFileObjectFromPath(const std::string &path) {

	// Create return object
	audioFileObject returnObject;

	// Load file from path
	AudioFile<double> file;
	if (!file.load(path)) {
		throw std::runtime_error("Couldnt load WAV file");
	}

	// Get metadata
	int sampleRate = file.getSampleRate();
	int numSamples = file.getNumSamplesPerChannel();

	returnObject.sampleRate = sampleRate;
	returnObject.numSamples = numSamples;

	// Create output vector from decoded samples 
	std::vector<std::complex<double>> outputVector(
	    static_cast<size_t>(numSamples));
	if (!file.samples.empty() && !file.samples[0].empty()) {
		for (int i = 0; i < numSamples; ++i) {
			double sample = file.samples[0][i]; 
			outputVector[static_cast<size_t>(i)] =
			    std::complex<double>(sample, 0.0);
		}
	} else {
		
		for (int i = 0; i < numSamples; ++i) {
			outputVector[static_cast<size_t>(i)] =
			    std::complex<double>(0.0, 0.0);
		}
	}

	// Remove DC bias
	double mean =
	    std::accumulate(outputVector.begin(), outputVector.end(), 0.0,
	                    [](double sum, const std::complex<double> &value) {
		                    return sum + value.real();
	                    }) /
	    outputVector.size();

	for (auto &value : outputVector) {
		value -= mean;
	}

	// Apply Hann window
	double windowSum = 0.0;
	for (size_t i = 0; i < outputVector.size(); ++i) {
		double window =
		    0.5 *
		    (1.0 - std::cos(2.0 * M_PI * static_cast<double>(i) /
		                    (static_cast<double>(outputVector.size()) - 1.0)));
		outputVector[i] *= window;
		windowSum += window;
	}

	// Compensate window amplitude loss so different segment lengths are comparable
	double windowAvg = windowSum / static_cast<double>(outputVector.size());
	if (windowAvg > 0.0) {
		double comp = 1.0 / windowAvg;
		for (auto &v : outputVector)
			v *= comp;
	}

	// Pad the input vector to the next power of 2 (needed for simple_fft)
	size_t originalSize = outputVector.size();
	size_t paddedSize = std::pow(2, std::ceil(std::log2(originalSize)));
	outputVector.resize(paddedSize, {0, 0});

	returnObject.paddedSize = paddedSize;
	returnObject.FFTReadyVector = outputVector;

	return returnObject;
}

std::vector<std::pair<double, double>>
calculateFFT(const audioFileObject &inputObject) {

	// Create FFT output vector
	std::vector<std::complex<double>> fftOutput(
	    inputObject.FFTReadyVector.size());

	// Do the FFT and handle errors by throwing an exception
	const char *errorDescription = nullptr;
	if (!simple_fft::FFT(inputObject.FFTReadyVector, fftOutput,
	                     inputObject.FFTReadyVector.size(), errorDescription)) {
		std::string errorString =
		    "FFT failed: " + static_cast<std::string>(errorDescription);

		throw std::runtime_error(errorString);
	}

	// Compute amplitudes
	std::vector<double> amplitudes(fftOutput.size() / 2);
	// Normalize to single-sided spectrum and use original (non-padded) sample count
	double scale =
	    2.0 / static_cast<double>(std::max(1, inputObject.numSamples));
	for (size_t i = 0; i < amplitudes.size(); ++i) {
		amplitudes[i] = std::abs(fftOutput[i]) * scale;
	}

	// Fill the output vector with frequencies and amplitudes
	double binWidth =
	    static_cast<double>(inputObject.sampleRate) / inputObject.paddedSize;
	std::vector<std::pair<double, double>> result(amplitudes.size());
	for (size_t i = 0; i < amplitudes.size(); ++i) {
		double frequency = i * binWidth;
		result[i] = std::make_pair(frequency, std::abs(amplitudes[i] * 50));
	}

	return result;
}

std::vector<audioFileObject> cutupVector(const audioFileObject &object,
                                         int DFTsPrSecond) {
	// Validate parameters
	if (DFTsPrSecond <= 0) {
		throw std::invalid_argument("DFTsPrSecond must be > 0");
	}

	// Calculate the number of samples per DFT segment
	int samplesPrDFT = object.sampleRate / DFTsPrSecond;
	if (samplesPrDFT <= 0) {
		throw std::invalid_argument("samplesPrDFT computed to 0; increase "
		                            "sampleRate or decrease DFTsPrSecond");
	}

	// Determine the total number of segments that can be created
	int numberOfCutVectors = object.numSamples / samplesPrDFT;

	// Initialize the result vector to store the segmented audioFileObjects
	std::vector<audioFileObject> result;

	// Loop through and create each segment
	for (int i = 0; i < numberOfCutVectors; ++i) {
		audioFileObject segment;

		// Set the metadata for the segment
		segment.sampleRate = object.sampleRate;
		segment.numSamples = samplesPrDFT;

		// Calculate the start and end indices for the current segment
		size_t startIdx =
		    static_cast<size_t>(i) * static_cast<size_t>(samplesPrDFT);
		size_t endIdx = startIdx + static_cast<size_t>(samplesPrDFT);

		// Extract the corresponding portion of the FFTReadyVector
		segment.FFTReadyVector = std::vector<std::complex<double>>(
		    object.FFTReadyVector.begin() + startIdx,
		    object.FFTReadyVector.begin() + endIdx);

		// Ensure the vector length is a power of two by padding to next 2^n
		size_t currSize = segment.FFTReadyVector.size();
		size_t paddedSize = 1;
		while (paddedSize < currSize) {
			paddedSize <<= 1;
		}

		if (paddedSize != currSize) {
			segment.FFTReadyVector.resize(paddedSize,
			                              std::complex<double>(0.0, 0.0));
		}

		segment.paddedSize = paddedSize;

		// Add the segment to the result vector
		result.push_back(std::move(segment));
	}

	// Return the vector containing all the segments
	return result;
}
