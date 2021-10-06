#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <chrono>
#include <thread>
#include <cmath>
#include <iomanip>
#include <sstream>

const bool debug = true;
const int sampleFormat = 3;
const int bitsPerSample = 32;
const int channels = 2;

float aIn, aOut, bIn, bOut, cIn, cOut, dIn, dOut, divisor;

std::string formatFloat(float f) {
	std::stringstream stream;
	stream << std::fixed << std::setprecision(6) << f;
	return stream.str();
}

void writeBytes(std::ofstream &file, int value, int bytes) {
	file.write(reinterpret_cast<char*>(&value), bytes);
}

void writeBytes(std::ofstream &file, float value, int bytes) {
	file.write(reinterpret_cast<char*>(&value), bytes);
}

float reverseFloat( const float inFloat )
{
   float retVal;
   char *floatToConvert = ( char* ) & inFloat;
   char *returnFloat = ( char* ) & retVal;

   // swap the bytes into a temporary buffer
   returnFloat[0] = floatToConvert[3];
   returnFloat[1] = floatToConvert[2];
   returnFloat[2] = floatToConvert[1];
   returnFloat[3] = floatToConvert[0];

   return retVal;
}

float transformX(float input) {
	return -1 * input;
}
float transformX(float input, std::ifstream & file) {
	float temp, output;
	std::string packet;

	if (!(divisor)) {
		file.seekg(std::ios::beg);
		file >> packet;
		divisor = std::stof(packet);
		packet = "";

		file >> packet;
		cIn = std::stof(packet);
		aIn = cIn;
		packet = "";
		file >> packet;
		cOut = std::stof(packet);
		packet = "";
		file >> packet;
		aOut = std::stof(packet);

		packet = "";
		file >> packet;
		dIn = std::stof(packet);
		bIn = dIn;
		packet = "";
		file >> packet;
		dOut = std::stof(packet);
		packet = "";
		file >> packet;
		bOut = std::stof(packet);
		packet = "";
	}

	if (cIn > input) {
		file.seekg(std::ios::beg);
		file >> packet;
		packet = "";

		file >> packet;
		cIn = std::stof(packet);
		aIn = cIn;
		packet = "";
		file >> packet;
		cOut = std::stof(packet);
		packet = "";
		file >> packet;
		aOut = std::stof(packet);

		packet = "";
		file >> packet;
		dIn = std::stof(packet);
		bIn = dIn;
		packet = "";
		file >> packet;
		dOut = std::stof(packet);
		packet = "";
		file >> packet;
		bOut = std::stof(packet);
		packet = "";
	}

	while (dIn < input) {
		file >> packet;
		cIn = dIn;
		aIn = bIn;
		cOut = dOut;
		aOut = bOut;
		dIn = std::stof(packet);
		bIn = dIn;
		packet = "";
		file >> packet;
		dOut = std::stof(packet);
		packet = "";
		file >> packet;
		bOut = std::stof(packet);
		packet = "";
	}

	float normIn = dIn - cIn;
	float normOut = dOut - cOut;
	float normSample = input - cIn;

	float ratio = normSample / normIn;

	return (cOut + (normOut * ratio)) / divisor;
}

float transformY(float input) {
	return input;
}
float transformY(float input, std::ifstream & file) {
	float temp, output;
	std::string packet;

	if (aIn > input) {
		file.seekg(std::ios::beg);
		file >> packet;
		packet = "";
		file >> packet;
		aIn = std::stof(packet);
		cIn = aIn;
		packet = "";
		file >> packet;
		cOut = std::stof(packet);
		packet = "";
		file >> packet;
		aOut = std::stof(packet);

		packet = "";
		file >> packet;
		bIn = std::stof(packet);
		dIn = bIn;
		packet = "";
		file >> packet;
		dOut = std::stof(packet);
		packet = "";
		file >> packet;
		bOut = std::stof(packet);
		packet = "";
	}

	while (bIn < input) {
		file >> packet;
		aIn = bIn;
		cIn = dIn;
		aOut = bOut;
		cOut = dOut;
		bIn = std::stof(packet);
		dIn = bIn;
		packet = "";
		file >> packet;
		dOut = std::stof(packet);
		packet = "";
		file >> packet;
		bOut = std::stof(packet);
		packet = "";
	}

	float normIn = bIn - aIn;
	float normOut = bOut - aOut;
	float normSample = input - aIn;

	float ratio = normSample / normIn;

	return (aOut + (normOut * ratio)) / divisor;
}

int main(int argc, char **argv) {
	uint32_t seeker;
	char *seek = (char*)&seeker;

	uint16_t twobyte;
	char *int2 = (char*)&twobyte;

	uint16_t d2;
	char *data2 = (char*)&d2;

	float inSample;
	char *sample = (char*)&inSample;

	char text[5];
	char dataTag[5] = {'d', 'a', 't', 'a', 0};
	char fmtTag[5] = {'f', 'm', 't', ' ', 0};

	int sampleRate = 96000;
	bool average = false;

	std::ifstream inFile;
	std::ofstream outFile;

	std::string cInTemp;
	std::string fname;
	std::string iname;
	std::string oname = "result.wav";

	std::cout << "Enter the name of the reshaper function .txt file! ";
	std::cin >> fname;

	std::cout << "Enter the name of the  input .wav file! ";
	std::cin >> iname;
	std::cout << std::endl;

	/*

	std::cout << "Enter the name of the output .wav file! ";
	std::cin >> oname;
	std::cout << std::endl;
	*/

	std::cout << "Starting!" << std::endl;

	inFile.open(iname, std::ios::binary);
	outFile.open(oname, std::ios::binary);

	inFile.seekg(std::ios::beg);
	inFile.read(text, 4);
	std::cout << text << std::endl;
	inFile.read(seek, 4);
	inFile.read(seek, 4);
	seeker = 0;
	while (strcmp(text, dataTag)) {
		for (int i = 0; i < 5; i ++) text[i] = 0;
		inFile.read(text, 4);
		std::cout << "Found chunk: " << text << std::endl;
		inFile.read(seek, 4);
		if (strcmp(text, fmtTag)) {
			if (strcmp(text, dataTag)) inFile.ignore(seeker);
		} else {
			int t = seeker - 8;
			inFile.ignore(2);
			inFile.read(int2, 2);
			if (twobyte > 1) average = true;
			inFile.read(seek, 4);
			sampleRate = seeker;
			inFile.ignore(t);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	std::cout << "Running!" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	// Header
	outFile << "RIFF";
	outFile << "----";
	outFile << "WAVE";

	// Format
	outFile << "fmt ";
	writeBytes(outFile, 16, 4);
	writeBytes(outFile, sampleFormat, 2);
	writeBytes(outFile, channels, 2);
	writeBytes(outFile, sampleRate, 4);
	writeBytes(outFile, (sampleRate * bitsPerSample * channels) / 8, 4);
	writeBytes(outFile, bitsPerSample / 8 * channels, 2);
	writeBytes(outFile, bitsPerSample, 2);

	outFile << "fact";
	writeBytes(outFile, 4, 4);
	int factMarker = outFile.tellp();
	outFile << "----";

	// Data
	outFile << "data";
	outFile << "----";
	int dataStart = outFile.tellp();
	int inMarker = inFile.tellg();

	std::cout << "Measuring File" << std::flush;

	float tempSample, min, max, shift, stretch;
	int iters;
	//iters = 48000;
	iters = seeker >> 2;
	if (average) iters >>= 1;
	for (int i = 0; i < iters; i ++) {
		inFile.read(sample, 4);
		if (average) {
			tempSample = inSample;
			inFile.read(sample, 4);
			inSample = (inSample + tempSample)/2;
		}
		if (!(i % sampleRate)) {
			std::cout << "." << std::flush;
		}
		if (!i) {
			min = inSample;
			max = inSample;
		} else {
			if ( inSample < min  ) min  = inSample;
			if ( inSample > max  ) max  = inSample;
		}
	}
	shift = (max + min) * -1 / 2.0;
	stretch = 2.0 / (max - min);
	std::cout << "Done!" << std::endl << std::endl;

	inFile.seekg(inMarker);
	std::cout << "Running Function" << std::flush;

	std::ifstream funcFile;
	funcFile.open(fname);

	float tempL, tempR, minL, maxL, minR, maxR;
	for (int i = 0; i < iters; i ++) {
		inFile.read(sample, 4);
		if (average) {
			tempSample = inSample;
			inFile.read(sample, 4);
			inSample = (inSample + tempSample)/2;
		}
		inSample += shift;
		inSample *= stretch;
		if (fname.compare("")) {
			tempL = transformX(inSample, funcFile);
			tempR = transformY(inSample, funcFile);
		} else {
			tempL = transformX(inSample);
			tempR = transformY(inSample);
		}
		if (!(i % sampleRate)) {
			std::cout << "." << std::flush;
		}
		writeBytes(outFile, tempL, 4);
		writeBytes(outFile, tempR, 4);
		if (!i) {
			minL = tempL;
			maxL = tempL;
			minR = tempR;
			maxR = tempR;
		} else {
			if ( tempL    < minL ) minL = tempL;
			if ( tempL    > maxL ) maxL = tempL;
			if ( tempR    < minR ) minR = tempR;
			if ( tempR    > maxR ) maxR = tempR;
		}
	}
	std::cout << "Done!" << std::endl << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	int dataEnd = outFile.tellp();

	outFile.seekp(factMarker);
	writeBytes(outFile, (dataEnd - dataStart) / 4, 4);

	outFile.seekp(dataStart - 4);
	writeBytes(outFile, dataEnd - dataStart, 4);

	outFile.seekp(4, std::ios::beg);
	writeBytes(outFile, dataEnd - 8, 4);

	outFile.close();

	std::cout << "All Done!" << std::endl << std::endl;
	std::cout << "Minimum Input Value: "  << formatFloat(min) << "  |  Maximum Input Value: " << formatFloat(max) << std::endl;
	std::cout << "Minimum Left Value: " << formatFloat(minL) << "   |  Maximum Left Value: " << formatFloat(maxL) << std::endl;
	std::cout << "Minimum Right Value: " << formatFloat(minR) << "  |  Maximum Right Value: " << formatFloat(maxR) << std::endl;
	std::cout << std::endl << "Output written to " << oname << std::endl;
	return 0;
}
