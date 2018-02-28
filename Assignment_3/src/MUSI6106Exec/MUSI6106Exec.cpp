
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "Vibrato.h"

//class CVibrato;

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string                 sInputFilePath,                 //!< file paths
                                sOutputFilePath;

    static const int            kBlockSize = 1024;

    clock_t                     time = 0;

    float                       **ppfAudioInputData = 0;
    float                       **ppfAudioOutputData = 0;

    CAudioFileIf                *phAudioInputFile = 0;
    CAudioFileIf                *phAudioOutputFile = 0;
    CAudioFileIf::FileSpec_t    stFileSpec;

    CVibrato                    *pVibratoInstance = 0;

    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    // command line arguments are expected to be: inputFilePath outputFilePath
    if (argc < 2)
    {
        cout << "Missing audio input or output path!";
        return -1;
    }
    else
    {
        sInputFilePath = argv[1];
        sOutputFilePath = argv[2];
    }

    //////////////////////////////////////////////////////////////////////////////
    // open the input and output wave file
    CAudioFileIf::create(phAudioInputFile);
    CAudioFileIf::create(phAudioOutputFile);
    phAudioInputFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioInputFile->isOpen())
    {
        cout << "Wave file open error!";
        return -1;
    }
    phAudioInputFile->getFileSpec(stFileSpec);
    phAudioOutputFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec);
    if (!phAudioOutputFile->isOpen())
    {
        cout << "Output file cannot be initialized!";
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////
    // Init Vibrato
    pVibratoInstance = new CVibrato(stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);

    // set parameters of the vibrato
    pVibratoInstance->setParam(CVibrato::VibratoParam_t::kParamVibratoWidthInSec, 0.05f);
    pVibratoInstance->setParam(CVibrato::VibratoParam_t::kParamVibratoFreqInHz, 10.0f);
//    pVibratoInstance->setParam(CVibrato::VibratoParam_t::kParamVibratoDelayInSec, 0.f);

    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfAudioInputData = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioInputData[i] = new float[kBlockSize];

    ppfAudioOutputData = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioOutputData[i] = new float[kBlockSize];

    time = clock();

    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output file
    while (!phAudioInputFile->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioInputFile->readData(ppfAudioInputData, iNumFrames);
        pVibratoInstance->process(ppfAudioInputData, ppfAudioOutputData, iNumFrames);
        phAudioOutputFile->writeData(ppfAudioOutputData, iNumFrames);
    }

    cout << "\nreading/writing done in: \t" << (clock() - time)*1.F / CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    CAudioFileIf::destroy(phAudioInputFile);
    CAudioFileIf::destroy(phAudioOutputFile);

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete[] ppfAudioInputData[i];
    delete[] ppfAudioInputData;
    ppfAudioInputData = 0;

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete[] ppfAudioOutputData[i];
    delete[] ppfAudioOutputData;
    ppfAudioOutputData = 0;

    delete pVibratoInstance;
    pVibratoInstance = 0;

    return 0;

}


void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2018 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

