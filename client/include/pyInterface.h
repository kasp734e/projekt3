#pragma once
#include "pybind11/include/pybind11/embed.h"
#include <array>
#include <cmath>
#include <pybind11/pybind11.h>
#include <stdexcept>

class PythonLightInterface {
  private:
	pybind11::module_ PythonModule; /*! Holds the python module containing the
	                                   wrapper functions */
	pybind11::scoped_interpreter guard; /*! Keeps Python interpreter alive
	                                       during the lifetime of the object */
  public:
	PythonLightInterface();
	~PythonLightInterface();

	/**
	 * @brief Turns on the given percentage (percent) of the line (lineNr) on
	 * the led matrix
	 * @param lineNr Specifies the lineNr to be modified
	 * @param percent Specifies the percent of LEDs to be turned on
	 * @note percent rounds to nearest 12.5% due to there only being 8 LEDS per
	 * line
	 */
	void setLine(const int &lineNr, const int &percent);

	/**
	 * @brief Wrapper for PythonLightInterface::setLine with the option to use
	 * an array.
	 * @param percentages Array of percentages for the lines on the LED matrix
	 * @note See setLine
	 */
	void setLines(const std::array<int, 16> &percentages);

	/**
	 * @brief Turns off all pixels in LED matrix
	 */
	void clearAllPixels();
};
