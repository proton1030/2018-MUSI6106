
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "RingBuffer.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();
void    testcases (CRingBuffer<float>   *&pCRingBuffer);

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    // You don't need to use an audio file this time, though you can try if you want.
    // std::string             sInputFilePath,                 //!< file paths
    //                         sOutputFilePath;
    // static const int        kBlockSize          = 1024;
    // float                   **ppfAudioData      = 0;
    // CAudioFileIf            *phAudioFile        = 0;
    // CAudioFileIf::FileSpec_t stFileSpec;
    // long long               iInFileLength       = 0;        //!< length of input file

    clock_t                 time                = 0;

    CRingBuffer<float>      *pCRingBuffer       = 0;
    float                   *pfTestSignal       = 0;
    float                   fGain               = 1.f;
    int                     bufferLength        = 0;
    int                     testSignalLength    = 0;

    showClInfo ();

    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    time                    = clock();
    
    if (argc < 3)
    {
        cout << "Additional argument (Circular buffer length / Test signal length) expected.";
        return 1;
    }
    else if (atoi(argv[1]) <= 0)
    {
        cout << "Positive length for buffer size expected.";
        return 2;
    }
    else if (atoi(argv[2]) <= 0)
    {
        cout << "Positive length for test signal length expected.";
        return 3;
    };
    
    bufferLength = atoi(argv[1]);
    testSignalLength = atoi(argv[2]);
//    cout << bufferLength << "  " <<testSignalLength << endl;
    pCRingBuffer = new CRingBuffer<float>(bufferLength);
    cout << "----------------------------------------------------" << endl;
    cout << "Basic Function Test: feeding a ramp function, buffer length = " << bufferLength << endl;
    
    // fill the test signal (e.g., a unit impulse)
    pfTestSignal = new float[testSignalLength];
    cout << "input  signal:";
    for (int i = 0; i < testSignalLength; i++)
    {
        pfTestSignal[i] = i;
        cout << i << ",";
    }
    cout << endl;
    //////////////////////////////////////////////////////////////////////////////
    // do processing and tests
    pCRingBuffer -> reset();
    cout << "output signal:";
    for (int i = 0; i < bufferLength + testSignalLength ; i++)
    {
        cout << pCRingBuffer -> getPostInc() << ",";
        pCRingBuffer -> putPostInc(i < testSignalLength ? pfTestSignal[i] : 0);
    }
    cout << endl;
    


    testcases(pCRingBuffer);
    
    cout << "processing done in: \t"    << (clock()-time)*1.F/CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    
    return 0;
}


void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2018 by Alexander Lerch" << endl;
    cout << endl;

    return;
}

void testcases(CRingBuffer<float>   *&pCRingBuffer)
{
    std::fstream fileStream;
    fileStream.open("./testLog.txt", std::fstream::out);
    
    //Test case 1:
    
    pCRingBuffer -> reset();
    cout << "----------------------------------------------------" << endl;
    cout << "Test case 1: testing get function with large offset:" << endl;
    cout << "output: " << pCRingBuffer->get(4235) << "\t expected: " << 0 << endl;
    //write log
    fileStream << "----------------------------------------------------" << endl;
    fileStream << "Test case 1: testing get function with large offset " << endl;
    fileStream << "output: " << pCRingBuffer->get(4235) << "\t expected: " << 0 << endl;
    
    //Test case 2:
    
    pCRingBuffer -> reset();
    cout << "----------------------------------------------------" << endl;
    cout << "Test case 2: testing get function with negative offset:" << endl;
    cout << "output: " << pCRingBuffer->get(-3213.54) << "\t expected: " << 0 << endl;
    //write log
    fileStream << "----------------------------------------------------" << endl;
    fileStream << "Test case 2: testing get function with negative offset:" << endl;
    fileStream << "output: " << pCRingBuffer->get(-3213.54) << "\t expected: " << 0 << endl;
    
    //Test case 3:
    
    cout << "----------------------------------------------------" << endl;
    cout << "Test case 3: testing put and get function:" << endl;
    pCRingBuffer->put(1111.111);
    cout << "output: " << pCRingBuffer -> get() << "\t expected: " << 1111.111 << endl;
    //write log
    fileStream << "----------------------------------------------------" << endl;
    fileStream << "Test case 3: testing put and get function:" << endl;
    fileStream << "output: " << pCRingBuffer -> get() << "\t expected: " << 1111.111 << endl;
    
    //Test case 4:
    
    pCRingBuffer -> reset();
    cout << "----------------------------------------------------" << endl;
    cout << "Test case 4: testing put with changing WriteIdx (+1), and using get function with offset and original ReadIdx:" << endl;
    pCRingBuffer->put(1111.111);
    pCRingBuffer->setWriteIdx(pCRingBuffer->getWriteIdx()+1);
    pCRingBuffer->put(2222.222);
    cout << "offset=0 output: " << pCRingBuffer -> get() << "\t expected: " << 1111.111 << endl;
    cout << "offset=1 output: " << pCRingBuffer -> get(1) << "\t expected: " << 2222.222 << endl;
    //write log
    fileStream << "----------------------------------------------------" << endl;
    fileStream << "Test case 4: testing put with changing WriteIdx (+1), and using get function with offset and original ReadIdx:" << endl;
    fileStream << "offset=0 output: " << pCRingBuffer -> get() << "\t expected: " << 1111.111 << endl;
    fileStream << "offset=1 output: " << pCRingBuffer -> get(1) << "\t expected: " << 2222.222 << endl;
    
    //Test case 5:
    
    pCRingBuffer -> reset();
    cout << "----------------------------------------------------" << endl;
    cout << "Test case 5: testing put with changing WriteIdx (+1) and ReadIdx (-1), and using get function with offset:" << endl;
    pCRingBuffer->put(1111.111);
    pCRingBuffer->setWriteIdx(pCRingBuffer->getWriteIdx()+1);
    pCRingBuffer->put(2222.222);
    pCRingBuffer->setReadIdx(pCRingBuffer->getReadIdx()-1);
    cout << "offset=1 output: " << pCRingBuffer -> get(1) << "\t expected: " << 1111.111 << endl;
    cout << "offset=2 output: " << pCRingBuffer -> get(2) << "\t expected: " << 2222.222 << endl;
    cout << "offset=0 output: " << pCRingBuffer -> get() << "\t expected: " << 0 << endl;
    //write log
    fileStream << "----------------------------------------------------" << endl;
    fileStream << "Test case 5: testing put with changing WriteIdx (+1) and ReadIdx (-1), and using get function with offset:" << endl;
    fileStream << "offset=1 output: " << pCRingBuffer -> get(1) << "\t expected: " << 1111.111 << endl;
    fileStream << "offset=2 output: " << pCRingBuffer -> get(2) << "\t expected: " << 2222.222 << endl;
    fileStream << "offset=0 output: " << pCRingBuffer -> get() << "\t expected: " << 0 << endl;
    
    
    fileStream.close();
}




