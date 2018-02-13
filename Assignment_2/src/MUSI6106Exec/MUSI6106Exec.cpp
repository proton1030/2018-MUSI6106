
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "CombFilterIf.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath;

    static const int        kBlockSize = 1024;

    clock_t                 time = 0;

    float                   **ppfAudioData = 0;
    float                   **ppfAudioOutputData = 0;

    CAudioFileIf            *phAudioFile = 0;
    CAudioFileIf            *phAudioOutputFile = 0;
    
    CAudioFileIf::FileSpec_t stFileSpec;
    CAudioFileIf::FileSpec_t stOutputFileSpec;

    CCombFilterIf           *pInstance = 0;
    CCombFilterIf::create(pInstance);
    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 2)
    {
        cout << "Missing audio input path!";
        return -1;
    }
    else
    {
        sInputFilePath = argv[1];
        sOutputFilePath = argv[2];
    }

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioFile);
    CAudioFileIf::create(phAudioOutputFile);
    
    phAudioFile->getFileSpec(stFileSpec);
    stOutputFileSpec = stFileSpec;
    stOutputFileSpec.eFormat = CAudioFileIf::FileFormat_t::kFileFormatWav;
    stOutputFileSpec.fSampleRateInHz = 44100.0f;
    
    phAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    phAudioOutputFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stOutputFileSpec);
    
    if (!phAudioFile->isOpen() || !phAudioOutputFile->isOpen())
    {
        cout << "Wave file open error!";
        return -1;
    }
    
    //////////////////////////////////////////////////////////////////////////////
    // Init Comb Filter
    pInstance->init(CCombFilterIf::CombFilterType_t::kCombIIR, 1.0f, stOutputFileSpec.fSampleRateInHz, stOutputFileSpec.iNumChannels);

    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfAudioData = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioData[i] = new float[kBlockSize];
    
    ppfAudioOutputData = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioOutputData[i] = new float[kBlockSize];

    time = clock();
    
    //////////////////////////////////////////////////////////////////////////////
    // set parameters of the comb filter
    pInstance->setParam(CCombFilterIf::FilterParam_t::kParamDelay, 0.01f);
    pInstance->setParam(CCombFilterIf::FilterParam_t::kParamGain, 0.8f);
    
    // get audio data and write it to the output file
    while (!phAudioFile->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioFile->readData(ppfAudioData, iNumFrames);

        for (int c = 0; c < stFileSpec.iNumChannels; c++)
        {
            pInstance -> process (ppfAudioData, ppfAudioOutputData, iNumFrames);
        }

        phAudioOutputFile->writeData(ppfAudioOutputData, iNumFrames);
        
    }

    cout << "\nreading/writing done in: \t" << (clock() - time)*1.F / CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    CAudioFileIf::destroy(phAudioFile);
    CAudioFileIf::destroy(phAudioOutputFile);

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        delete[] ppfAudioData[i];
        delete[] ppfAudioOutputData[i];
    }
    delete[] ppfAudioData;
    delete[] ppfAudioOutputData;
    ppfAudioData = 0;
    ppfAudioOutputData = 0;

    return 0;

}


void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2018 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

