
#include <iostream>
#include <ctime>
#include <string.h>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "CombFilterIf.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo();
int     testcases(CCombFilterIf *&pInstance);

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath;

    static const int        kBlockSize = 1024;
    static const float      fMaxFilterDelayTime = 1.0f;
    static const float      fDefaultFilterDelayTime = 0.5f;
    static const float      fDefaultFilterGain = 0.5f;

    clock_t                 time = 0;
    
    int                     iFilterType;
    float                   fFilterDelayTime;
    float                   fFilterGain;

    float                   **ppfAudioData = 0;
    float                   **ppfAudioOutputData = 0;

    CAudioFileIf            *phAudioFile = 0;
    CAudioFileIf            *phAudioOutputFile = 0;
    
    CAudioFileIf::FileSpec_t stFileSpec;

    CCombFilterIf           *pInstance = 0;
    CCombFilterIf::create(pInstance);
    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    // arguments should be in this sequence: (input_file_path filter_type delay_time gain output_file_path)
    if (argc == 1)
    {
        cout << "No arguments given. Running tests on the comb filter design." << endl;
        return testcases(pInstance);
    }
    if (argc != 6)
    {
        cout << "Missing arguments!" << endl;
        return -1;
    }
    if (strcmp(argv[2], "FIR")!=0 && strcmp(argv[2], "IIR")!=0)
    {
        cout << "Unexpected filter type!" << endl;
        return -1;
    }
    
    sInputFilePath = argv[1];
    iFilterType = strcmp(argv[2], "FIR") == 0 ? 0 : 1;
    fFilterDelayTime = atof(argv[3]);
    fFilterGain = atof(argv[4]);
    sOutputFilePath = argv[5];
    
    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioFile);
    CAudioFileIf::create(phAudioOutputFile);
    
    phAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    phAudioFile->getFileSpec(stFileSpec);
    phAudioOutputFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec);
    
    if (!phAudioFile->isOpen())
    {
        cout << "Input file open error!";
        return -1;
    }
    else if (!phAudioOutputFile->isOpen())
    {
        cout << "Output file cannot be initialized!";
        return -1;
    }
    
    //////////////////////////////////////////////////////////////////////////////
    // Init Comb Filter
    pInstance->init((iFilterType ? CCombFilterIf::CombFilterType_t::kCombIIR : CCombFilterIf::CombFilterType_t::kCombFIR), fMaxFilterDelayTime, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);

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
    if (pInstance->setParam(CCombFilterIf::FilterParam_t::kParamDelay, fFilterDelayTime))
    {
        pInstance->setParam(CCombFilterIf::FilterParam_t::kParamDelay, fDefaultFilterDelayTime);
        cout << "Warning: Filter delay length out of bounds. Delay time length been set to " << fDefaultFilterDelayTime << " seconds." << endl;
    }
    if (pInstance->setParam(CCombFilterIf::FilterParam_t::kParamGain, fFilterGain))
    {
        pInstance->setParam(CCombFilterIf::FilterParam_t::kParamGain, fDefaultFilterGain);
        cout << "Warning: Filter gain out of bounds. Gain been set to " << fDefaultFilterGain << "." << endl;
    }
    
    // get audio data and write it to the output file
    while (!phAudioFile->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioFile->readData(ppfAudioData, iNumFrames);

        pInstance -> process (ppfAudioData, ppfAudioOutputData, iNumFrames);
        
        phAudioOutputFile->writeData(ppfAudioOutputData, iNumFrames);
    }

    cout << "\nreading/writing done in: \t" << (clock() - time)*1.F / CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    CAudioFileIf::destroy(phAudioFile);
    CAudioFileIf::destroy(phAudioOutputFile);
    CCombFilterIf::destroy(pInstance);
    
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

void showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2018 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

int testcases(CCombFilterIf *&pInstance)
{
    pInstance->init(CCombFilterIf::CombFilterType_t::kCombIIR, 0.1, 1000, 1);
    pInstance->setParam(CCombFilterIf::FilterParam_t::kParamDelay, 0.01);
    pInstance->setParam(CCombFilterIf::FilterParam_t::kParamGain, 0.5);
    
//    pInstance -> process (ppfAudioData, ppfAudioOutputData, iNumFrames);
    
    
    
    
    return 0;
}




