#include "MUSI6106Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>
#include <algorithm>

#include "UnitTest++.h"

#include "Synthesis.h"
#include "Vector.h"
#include "ErrorDef.h"

#include "Vibrato.h"
#include "Lfo.h"


SUITE(Vibrato)
{
    struct VibratoData
    {
        VibratoData()
        {
            m_pCVibrato = new CVibrato(m_fSampleFreq, m_iNumChannels);
            m_pfData = new float *[m_iNumChannels];
            m_pfTmp  = new float *[m_iNumChannels];
            for (int i = 0; i < m_iNumChannels; i++)
            {
                m_pfData[i] = new float[m_iDataLength];
                m_pfTmp[i] = new float[m_iDataLength];
            }
            
        }

        ~VibratoData()
        {
            delete [] m_pfData;
            delete [] m_pfTmp;
            delete m_pCVibrato;
            m_pfData = 0;
            m_pfTmp = 0;
            m_pCVibrato = 0;
        }
        
        void resetBuffers()
        {
            for (int i = 0; i < m_iNumChannels; i++)
            {
                memset (m_pfTmp[i], 0, sizeof(float)*m_iDataLength);
                memset (m_pfData[i], 0, sizeof(float)*m_iDataLength);
            }
        }

        static const int    m_iDataLength;
        static const int    m_iNumChannels;
        static const float  m_fSampleFreq;
        
        CVibrato    *m_pCVibrato;
        float       **m_pfData,
                    **m_pfTmp;
    };
    
    const float VibratoData::m_fSampleFreq = 44100.F;
    const int   VibratoData::m_iNumChannels = 2;
    const int   VibratoData::m_iDataLength = 44101; // Length of vibrato internal buffer
    
    TEST_FIXTURE(VibratoData, withoutSettingParams)
    {
        m_pCVibrato->reset();
        resetBuffers();
        
        for (int i = 0; i < m_iNumChannels; i++)
        {
            CSynthesis::generateSine(m_pfData[i], 20.F, m_fSampleFreq, m_iDataLength, .7F, static_cast<float>(M_PI_2));
        }
        m_pCVibrato->process(m_pfData, m_pfTmp, m_iDataLength);
        
        for (int i = 0; i < m_iNumChannels; i++)
        {
            CHECK_ARRAY_CLOSE(m_pfData[i], m_pfTmp[i], m_iDataLength, 1e-3);
        }
    }
    
    TEST_FIXTURE(VibratoData, widthEqualsZero)
    {
        const float fVibratoWidthInSec = 0.F;
        const float fVibratoFreqInHz = 2.67F;
        
        m_pCVibrato->reset();
        resetBuffers();
        
        for (int i = 0; i < m_iNumChannels; i++)
        {
            CSynthesis::generateSine(m_pfData[i], 682.F, m_fSampleFreq, m_iDataLength, 6.7F);
        }
        m_pCVibrato->setParam(CVibrato::kParamVibratoWidthInSec, fVibratoWidthInSec);
        m_pCVibrato->setParam(CVibrato::kParamVibratoFreqInHz, fVibratoFreqInHz);
        m_pCVibrato->process(m_pfData, m_pfTmp, m_iDataLength);
        
        for (int i = 0; i < m_iNumChannels; i++)
        {
            CHECK_ARRAY_CLOSE(m_pfData[i], m_pfTmp[i], m_iDataLength, 1e-3);
        }
        
    }
    
    TEST_FIXTURE(VibratoData, DCInput)
    {
        const float fVibratoWidthInSec = 0.23F;
        const float fVibratoFreqInHz = 13.73F;
        const float fDCValue = 2.32F;
        
        m_pCVibrato->reset();
        resetBuffers();
        
        for (int i = 0; i < m_iNumChannels; i++)
        {
            std::fill_n(m_pfData[i], m_iDataLength, fDCValue);
        }
        
        //Fill the Vibrato internal buffer with DC value;
        m_pCVibrato->setParam(CVibrato::kParamVibratoWidthInSec, 0.0f);
        m_pCVibrato->setParam(CVibrato::kParamVibratoFreqInHz, 0.0f);
        m_pCVibrato->process(m_pfData, m_pfTmp, m_iDataLength);
        
        m_pCVibrato->setParam(CVibrato::kParamVibratoWidthInSec, fVibratoWidthInSec);
        m_pCVibrato->setParam(CVibrato::kParamVibratoFreqInHz, fVibratoFreqInHz);
        m_pCVibrato->process(m_pfData, m_pfTmp, m_iDataLength);
        
        for (int i = 0; i < m_iNumChannels; i++)
        {
            CHECK_ARRAY_CLOSE(m_pfData[i], m_pfTmp[i], m_iDataLength, 1e-3);
        }
        
    }
    
    TEST_FIXTURE(VibratoData, differentInputBlockSize)
    {
        const int blockSize1 = 371;
        const int blockSize2 = 3817;
        const float fVibratoWidthInSec = 0.283F;
        const float fVibratoFreqInHz = 3.87F;
        float       **pfBlock1,
                    **pfBlock2;
        int remainingSample = m_iDataLength;
        pfBlock1 = new float *[m_iNumChannels];
        pfBlock2  = new float *[m_iNumChannels];
        for (int i = 0; i < m_iNumChannels; i++)
        {
            pfBlock1[i] = new float[blockSize1];
            pfBlock2[i] = new float[blockSize2];
        }
        
        m_pCVibrato->reset();
        resetBuffers();
        
        for (int i = 0; i < m_iNumChannels; i++)
        {
            CSynthesis::generateSaw(m_pfData[i], 1247.73F, m_fSampleFreq, m_iDataLength);
        }
        
        m_pCVibrato->setParam(CVibrato::kParamVibratoWidthInSec, fVibratoWidthInSec);
        m_pCVibrato->setParam(CVibrato::kParamVibratoFreqInHz, fVibratoFreqInHz);
        m_pCVibrato->process(m_pfData, m_pfTmp, m_iDataLength);
        
        m_pCVibrato->reset();
        m_pCVibrato->setParam(CVibrato::kParamVibratoWidthInSec, fVibratoWidthInSec);
        m_pCVibrato->setParam(CVibrato::kParamVibratoFreqInHz, fVibratoFreqInHz);
        
        while(remainingSample > 0)
        {
            int blockLength = std::min(remainingSample, blockSize1);
            for (int i = 0; i < m_iNumChannels; i++)
                memcpy (pfBlock1[i], m_pfData[i] + (m_iDataLength - remainingSample), sizeof(float) * blockLength);
            m_pCVibrato->process(pfBlock1, pfBlock1, blockSize1);
            for (int i = 0; i < m_iNumChannels; i++)
                CHECK_ARRAY_CLOSE(m_pfTmp[i] + (m_iDataLength - remainingSample), pfBlock1[i], blockLength, 1e-3);
            remainingSample -= blockSize1;
        }
        
        remainingSample = m_iDataLength;
        m_pCVibrato->reset();
        m_pCVibrato->setParam(CVibrato::kParamVibratoWidthInSec, fVibratoWidthInSec);
        m_pCVibrato->setParam(CVibrato::kParamVibratoFreqInHz, fVibratoFreqInHz);
        
        while(remainingSample > 0)
        {
            int blockLength = std::min(remainingSample, blockSize2);
            for (int i = 0; i < m_iNumChannels; i++)
                memcpy (pfBlock2[i], m_pfData[i] + (m_iDataLength - remainingSample), sizeof(float) * blockLength);
            m_pCVibrato->process(pfBlock2, pfBlock2, blockSize2);
            for (int i = 0; i < m_iNumChannels; i++)
                CHECK_ARRAY_CLOSE(m_pfTmp[i] + (m_iDataLength - remainingSample), pfBlock2[i], blockLength, 1e-3);
            remainingSample -= blockSize2;
        }
        
        for(int i = 0; i < m_iNumChannels; i++)
        {
            delete pfBlock1[i];
            delete pfBlock2[i];
        }
        delete [] pfBlock1;
        delete [] pfBlock2;
        pfBlock1 = 0;
        pfBlock2 = 0;
    }
    
    TEST_FIXTURE(VibratoData, zeroInputSignal)
    {
        const float fVibratoWidthInSec = 0.111F;
        const float fVibratoFreqInHz = 3.13F;
        
        m_pCVibrato->reset();
        resetBuffers();

        m_pCVibrato->setParam(CVibrato::kParamVibratoWidthInSec, fVibratoWidthInSec);
        m_pCVibrato->setParam(CVibrato::kParamVibratoFreqInHz, fVibratoFreqInHz);
        m_pCVibrato->process(m_pfData, m_pfTmp, m_iDataLength);
        
        for (int i = 0; i < m_iNumChannels; i++)
        {
            CHECK_ARRAY_CLOSE(m_pfData[i], m_pfTmp[i], m_iDataLength, 1e-3);
        }
    }

    TEST_FIXTURE(VibratoData, outputStaysSameWithDataInInternalBuffer)
    {
        const float fVibratoWidthInSec = 0.23F;
        const float fVibratoFreqInHz = 13.73F;
        
        m_pCVibrato->reset();
        resetBuffers();
        
        for (int i = 0; i < m_iNumChannels; i++)
        {
            CSynthesis::generateSine(m_pfData[i], 31.4F, m_fSampleFreq, m_iDataLength, 24.3F);
        }
        
        m_pCVibrato->setParam(CVibrato::kParamVibratoWidthInSec, fVibratoWidthInSec);
        m_pCVibrato->setParam(CVibrato::kParamVibratoFreqInHz, fVibratoFreqInHz);
        m_pCVibrato->process(m_pfData, m_pfTmp, m_iDataLength);
        
        m_pCVibrato->setParam(CVibrato::kParamVibratoWidthInSec, 0.0f);
        m_pCVibrato->process(m_pfData, m_pfTmp, m_iDataLength);
        
        for (int i = 0; i < m_iNumChannels; i++)
        {
            CHECK_ARRAY_CLOSE(m_pfData[i], m_pfTmp[i], m_iDataLength, 1e-3);
        }
    }
}

#endif //WITH_TESTS
