
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "RingBuffer.h"
#include "Fft.h"

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

    static const int        kBlockSize = atoi(argv[2]);
    static const int        kHopSize = atoi(argv[3]);

    clock_t                 time = 0;

    float                   **ppfAudioData = 0;

    CAudioFileIf            *phAudioFile = 0;
    std::fstream            hOutputFile;
    CAudioFileIf::FileSpec_t stFileSpec;
    CRingBuffer<float>      **pCRingBuffer = 0;
    float                   *pCurrentBlock = 0;
    CFft::complex_t         *pComplexSpectrum = 0;
    CFft::CFft              *pCFft = 0;

    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 4)
    {
        cout << "Missing audio input path!";
        return -1;
    }
    else
    {
        sInputFilePath = argv[1];
        sOutputFilePath = sInputFilePath + ".txt";
    }

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioFile);
    phAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioFile->isOpen())
    {
        cout << "Wave file open error!";
        return -1;
    }
    phAudioFile->getFileSpec(stFileSpec);
    
    pCRingBuffer = new CRingBuffer<float>* [stFileSpec.iNumChannels];
    for(int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        pCRingBuffer[i] = new CRingBuffer<float>(kBlockSize);
    }
    
    CFft::createInstance (pCFft);
    pCFft->initInstance(kBlockSize);
    //////////////////////////////////////////////////////////////////////////////
    // open the output text file
    hOutputFile.open(sOutputFilePath.c_str(), std::ios::out);
    if (!hOutputFile.is_open())
    {
        cout << "Text file open error!";
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    
    ppfAudioData = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioData[i] = new float[kBlockSize];

    time = clock();
    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output file
    
    long long iNumFrames = kBlockSize;
    long long iNumHopFrames = kHopSize;
    pCurrentBlock = new float(kBlockSize);
    pComplexSpectrum = new CFft::complex_t(kBlockSize*2);
    
    phAudioFile->readData(ppfAudioData, iNumFrames);
    
    
    cout << "\r" << "reading and writing";
    for (int c = 0; c < stFileSpec.iNumChannels; c++)
    {
        pCRingBuffer[c]->put(ppfAudioData[c], iNumFrames);
        pCRingBuffer[c]->get(pCurrentBlock, iNumFrames);
        
        pCFft->doFft (pComplexSpectrum, pCurrentBlock);
        pCFft->getMagnitude(pCurrentBlock, pComplexSpectrum);
        
        for (int i = 0; i < iNumFrames; i++)
        {
            hOutputFile << pCurrentBlock[i] << "\t";
        }
        hOutputFile << endl;
        
    }
    
    while (!phAudioFile->isEof())
    {
        phAudioFile->readData(ppfAudioData, iNumHopFrames);
        for (int c = 0; c < stFileSpec.iNumChannels; c++)
        {
            pCRingBuffer[c]->put(ppfAudioData[c], iNumHopFrames);
            
            pCRingBuffer[c]->setReadIdx(pCRingBuffer[c]->getReadIdx() + iNumHopFrames);
            pCRingBuffer[c]->setWriteIdx(pCRingBuffer[c]->getWriteIdx() + iNumHopFrames);
            
            pCRingBuffer[c]->get(pCurrentBlock, iNumFrames);
            pCFft->doFft (pComplexSpectrum, pCurrentBlock);
            pCFft->getMagnitude(pCurrentBlock, pComplexSpectrum);
            
            for (int i = 0; i < iNumFrames; i++)
            {
                hOutputFile << pCurrentBlock[i] << "\t";
            }
            
            hOutputFile << endl;
        }
    }

    cout << "\nreading/writing done in: \t" << (clock() - time)*1.F / CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    CAudioFileIf::destroy(phAudioFile);
    hOutputFile.close();

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete[] ppfAudioData[i];
    delete[] ppfAudioData;
    ppfAudioData = 0;

    return 0;

}


void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2018 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}












