/*
  A tiny program to read a WAV file, display it's metadata, modify the audio sample data (reduce the volume by half)
  and write the audio data back into a new file. The program is designed to help understand the details
  of how the WAV file format is read to a data structure. It is not meant to be used as a library for reading .wav files.

  To run: g++ <filename.cpp>
  Written by: Akash Murthy
 */

#include <iostream>
#include <fstream>
#include <vector>
#include "reader.h"

using namespace std;

/*
 * A structure to hold a Wav file's metadata.
 * Note that this will not support all types of wave files!!!
 * Wave files could have a variable length format chunks, they could have chunks organized differently, etc.
 * This is only meant as a example on how to read binary data to a struct.
*/
struct WavHeader {
    char riffId[4];
    unsigned long headerChunkSize;
    char waveId[4];
    char formatChunkId[4];
    unsigned long formatChunkSize;
    unsigned short compressionCode;
    unsigned short numberOfChannels;
    unsigned long samplesRate;
    unsigned long byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    char dataChunkId[4];
    unsigned long dataChunkSize;
};

/*
struct WAVHeader {
  char chunkId[4];          // 4 bytes
  uint32_t chunkSize;       // 4 bytes
  char format[4];           // 4 bytes
  char subchunk1Id[4];      // 4 bytes
  uint32_t subchunk1Size;   // 4 bytes
  uint16_t audioFormat;     // 2 bytes
  uint16_t numChannels;     // 2 bytes
  uint32_t sampleRate;      // 4 bytes
  uint32_t byteRate;        // 4 bytes
  uint16_t blockAlign;      // 2 bytes
  uint16_t bitsPerSample;   // 2 bytes
  char subchunk2Id[4];      // 4 bytes
  uint32_t subchunk2Size;   // 4 bytes
};*/


/*
https://github.com/hypevhs/brrtools/blob/2671a2f818755d0a7c5844db4ef37e8852cbbdc1/brr.c#L62
void generate_wave_file(FILE *outwav, unsigned int samplerate, pcm_t *buffer, size_t k)
{
	struct
	{
		char chunk_ID[4];				// Should be 'RIFF'
		u32 chunk_size;
		char wave_str[4];				// Should be 'WAVE'
		char sc1_id[4];					// Should be 'fmt '
		u32 sc1size;					// Should be at least 16
		u16 audio_format;				// Should be 1 for PCM
		u16 chans;						// 1 for mono, 2 for stereo, etc...
		u32 sample_rate;
		u32 byte_rate;
		u16 block_align;
		u16 bits_per_sample;
		char sc2_id[4];
		u32 sc2size;
	}
	hdr =
	{
		.chunk_ID = "RIFF",
		.chunk_size = 32*k + 36,
		.wave_str = "WAVE",
		.sc1_id = "fmt ",
		.sc1size = 16,				//Size of header
		.audio_format = 1,			//PCM format, no compression
		.chans = 1,					//Mono
		.sample_rate = samplerate,	//Sample rate
		.byte_rate = 2*samplerate,	//Byte rate
		.block_align = 2,			//BlockAlign
		.bits_per_sample = 16,		//16-bit samples
		.sc2_id = "data",
		.sc2size = 32*k
	};
	fwrite(&hdr, 1, sizeof(hdr), outwav);	// Write header
	fwrite(buffer, 2, 16*k, outwav); 		//write samplebuffer to file
}
*/

// Utility function to just print out some metadata.
void displayWavFileInfo(WavHeader& file) {
    cout << "Header chunk ID: " << string (file.riffId) << endl;
    cout << "Header chunk size: " << file.headerChunkSize << endl;
    cout << "Wave ID: " << string (file.waveId) << endl;

    cout << "Format chunk ID: " << string (file.formatChunkId) << endl;
    cout << "Format chunk size: " << file.formatChunkSize << endl;
    cout << "Compression code: " << file.compressionCode << endl;
    cout << "Number of channels: " << file.numberOfChannels << endl;
    cout << "Sample rate: " << file.samplesRate << endl;
    cout << "Byte rate: " << file.byteRate << endl;
    cout << "Block align: " << file.blockAlign << endl;
    cout << "Bits per sample: " << file.bitsPerSample << endl;

    cout << "Data chunk ID: " << string (file.dataChunkId) << endl;
    cout << "Data chunk size: " << (file.dataChunkSize) << endl;
}

// For this simple example, we only support Linear PCM, with 16 bit integer format
bool isSupported(WavHeader& header) {
    if (header.compressionCode == 1 && header.bitsPerSample == 16) {
        return true;
    }
    return false;
}

Reader::Reader() {

}

bool Reader::read(const std::string file_name) {
    ifstream audioFile;
    audioFile.open(file_name, ios::binary);
    audioFile.seekg(0, ios::beg);

    // Read and display the wave header alone
    WavHeader wavHeader{};
    audioFile.read((char*)&wavHeader, sizeof(WavHeader));
    displayWavFileInfo(wavHeader);

    // Decide if the wav file chosen is supported or not
    if (!isSupported(wavHeader)) {
        cout << "Wav file is not supported" << endl;
        audioFile.close();
        return false;
    }

    /* dataChunkSize is the total number of bytes of audio data. Each byte is 8 bits.
       Since we need 16 bit integer (short) for storing the audio sample value, the total number of samples
       are dataChunkSize / 2
    */
    // const auto totalNumberOfSamples = wavHeader.dataChunkSize/2;

    // // Seek to the start of the audio data
    // audioFile.seekg(sizeof(WavHeader), ios::beg);

    // // Initialize a vector container for storing all the audio samples and read data to it.
    // vector<short> wavData;
    // wavData.resize(totalNumberOfSamples);
    // audioFile.read((char*)wavData.data(), wavHeader.dataChunkSize);

    // // Do some processing on the data
    // for (int i = 0; i < totalNumberOfSamples; i++) {
    //     // Half the amplitude level of each sample.
    //     // Same as reducing the gain by 6dBFS
    //     wavData[i] *= 0.5;
    // }

    // // Write the modified data into a new file
    // ofstream outputFile;
    // outputFile.open("waveform-modified.wav", ios::binary);
    // outputFile.write((char*)&wavHeader, sizeof(WavHeader));
    // outputFile.write((char*)wavData.data(), wavHeader.dataChunkSize);

    // audioFile.close();
    // outputFile.close();
    return true;
}
