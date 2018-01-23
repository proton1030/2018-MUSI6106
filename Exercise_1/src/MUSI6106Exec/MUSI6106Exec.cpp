
#include <iostream>
#include <ctime>
#include <cassert>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"

using std::cout;
using std::endl;

// local function declarations
void showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath;

    long long               iInFileLength       = 0;        //!< length of input file

    clock_t                 time                = 0;

    float                   **ppfAudioData      = 0;

    CAudioFileIf            *phAudioFile        = 0;

    showClInfo ();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    assert(argc==3);
    sInputFilePath = argv[1];
    sOutputFilePath = argv[2];
    
    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioFile);
    phAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    phAudioFile->getLength(iInFileLength);
    
    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    time = clock();
    
    // get audio data and write it to the output file
    ppfAudioData    = new float* [2];
    for (int i = 0; i < 2; i++)
        ppfAudioData[i] = new float[static_cast<unsigned int>(iInFileLength)];
    
    std::fstream *fs = new std::fstream;
    fs->open(sOutputFilePath, std::ofstream::out | std::ofstream::trunc);
    fs->close();
    fs->open(sOutputFilePath, std::fstream::in | std::fstream::out | std::fstream::app);
    
    phAudioFile->readData (ppfAudioData, iInFileLength);
    for (int i = 0; i < iInFileLength; i++)
        *fs << ppfAudioData[0][i] << " " << ppfAudioData[1][i] << endl;
    
    fs->close();
    cout << "reading/writing done in: \t"    << (clock()-time)*1.F/CLOCKS_PER_SEC << " seconds." << endl;
    //////////////////////////////////////////////////////////////////////////////
    // get audio info and print it to stdout
    CAudioFileIf::FileSpec_t *fileSpec = new CAudioFileIf::FileSpec_t;
    phAudioFile->getFileSpec(*fileSpec);
    
    cout << endl << "----------Audio Info----------" << endl;
    cout << "Audio channel quantity: " << fileSpec->iNumChannels << endl;
    cout << "Sampling rate: " << fileSpec->fSampleRateInHz << endl;
    cout << "File format: " << fileSpec->eFormat << endl;
    cout << "Bit stream type: " << fileSpec->eBitStreamType << endl << endl;
    
    //////////////////////////////////////////////////////////////////////////////
    // do processing
    cout << "Hello there!" << endl << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    delete ppfAudioData;
    delete fileSpec;
    
    return 0;
    
}


void showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2018 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

