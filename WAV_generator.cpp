// WAV_generator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>

#define M_PI 3.14
#define SAMPLE_RATE 44100
#define BIT_DEPTH 16

/*
* Class for creating a sine oscillator with given frequency and amplitude
*/
class SineOscillator 
{
private:
    float frequency, amplitude, angle = 0.0f;
    float offset = 0.0f;

public:
    SineOscillator(float freq, float amp) : frequency(freq), amplitude(amp) {
        //How much we will move in the sine wave for each step in time (where 1s is divided into SAMPLE_RATE parts)
        offset = 2 * M_PI * frequency / SAMPLE_RATE;
    }

    /*
    * Calculates the current value and also updates the value
    */
    float process()
    {
        float sample = amplitude * sin(angle);
        angle += offset;
        return sample;
    }
};

/*
* Writes to the given file
* 
* @param file
* @param value
* @param size (given in bytes)
*/
void writeToFile(std::ofstream* file, int value, int size)
{
    file->write(reinterpret_cast<const char*> (&value), size);
}

int main()
{
    int duration;
    float amplitude, frequency;

    std::cout << "Please enter duration of note: ";
    std::cin >> duration;

    std::cout << "Please enter the frequency of note: ";
    std::cin >> frequency;

    std::cout << "Please enter amplitude of note: ";
    std::cin >> amplitude;

    std::ofstream audioFile;

    audioFile.open("waveform.wav", std::ios::binary);
    SineOscillator sineOscillator(frequency, amplitude);

    //Header chunk
    audioFile << "RIFF";
    audioFile << "----"; //Placeholder
    audioFile << "WAVE";

    // Format chunk
    audioFile << "fmt ";
    writeToFile(&audioFile, 16, 4); //Set size of fmt
    writeToFile(&audioFile, 1, 2); //Compression code
    writeToFile(&audioFile, 1, 2); //number of channels 
    writeToFile(&audioFile, SAMPLE_RATE, 4); //Sample rate 
    writeToFile(&audioFile, SAMPLE_RATE * BIT_DEPTH >> 3, 4); //Set average bytes per second
    writeToFile(&audioFile, BIT_DEPTH >> 3, 2); //Block align
    writeToFile(&audioFile, BIT_DEPTH, 2); //Bit depth

    //Data chunk
    audioFile << "data";
    audioFile << "----"; //placeholder (We want size of data here, which we don't know yet)

    int preAudioPos = audioFile.tellp();

    int maxAmplitude = (2 << (BIT_DEPTH - 1)) - 1;
    for (int i = 0; i < SAMPLE_RATE * duration; i++)
    {
        float sample = sineOscillator.process();
        sample *= maxAmplitude; //Scale sample to be within size of signed 16-bit integer
        int intSample = static_cast<int> (sample);

        writeToFile(&audioFile, intSample, 2);
    }

    int postAudioPos = audioFile.tellp();

    audioFile.seekp(preAudioPos - 4);
    writeToFile(&audioFile, postAudioPos - preAudioPos, 4);

    audioFile.seekp(4, std::ios::beg);

    writeToFile(&audioFile, postAudioPos - 8, 4);

    audioFile.close();
    return 0;
}

