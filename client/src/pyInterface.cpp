#include "pyInterface.h"

PythonLightInterface::PythonLightInterface() : guard() {

	pybind11::module_ sys = pybind11::module_::import("sys");
	sys.attr("path").attr("insert")(0, "src/py");
	PythonModule = pybind11::module_::import("wrapper");
	PythonModule.attr("initLeds")();
	PythonModule.attr("clearPixels")();
}

PythonLightInterface::~PythonLightInterface() { clearAllPixels(); }

void PythonLightInterface::setLine(const int &lineNr, const int &percent) {
	
	// Turns a percentage into a number of pixels from 0-8
	int amountPixels = std::round((double)percent / 12.5);

	PythonModule.attr("setPixelsInLine")(amountPixels, lineNr);
}

void PythonLightInterface::setLines(const std::array<int, 16> &percentages) {

	clearAllPixels();

	// Batch updates the values and only after shows the new pixles. Calling showPixels for every line takes forever
	for (int i = 0; i < 16; ++i) {
		int amountPixels = std::round((double)percentages[i] / 12.5);
		PythonModule.attr("setPixelsInLine")(amountPixels, i + 1);
	}
	PythonModule.attr("showPixels")();
}

void PythonLightInterface::clearAllPixels() {
	
	// Calls the clearPixels() function in the python code (py/wrapper.py)
	PythonModule.attr("clearPixels")();
}
