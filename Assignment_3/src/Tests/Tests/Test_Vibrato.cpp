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
            m_pCVibrato     = new CVibrato(m_fSampleFreq, m_iNumChannels);
            m_pfData = new float *[m_iNumChannels];
            m_pfTmp  = new float *[m_iNumChannels];
            for (int i = 0; i < m_iNumChannels; i++)
            {
                m_pfData[i] = new float[m_iDataLength];
                m_pfTmp[i] = new float[m_iDataLength];
                CSynthesis::generateSine(m_pfData[i], 20.F, m_fSampleFreq, m_iDataLength, .7F, static_cast<float>(M_PI_2));
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
        m_pCVibrato->process(m_pfData, m_pfTmp, m_iDataLength);
        
        for (int i = 0; i < m_iNumChannels; i++)
        {
            CHECK_ARRAY_CLOSE(m_pfData[i], m_pfTmp[i], m_iDataLength, 1e-3);
        }
    }
    
    TEST_FIXTURE(VibratoData, widthEqualsZero)
    {
        m_pCVibrato->reset();
        const float fVibratoWidthInSec = 0.F;
        const float fVibratoFreqInHz = 2.67F;
        
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
        float **fDCData = new float *[m_iNumChannels];
        for (int i = 0; i < m_iNumChannels; i++)
        {
            fDCData[i] = new float[m_iDataLength];
            std::fill_n(fDCData[i], m_iDataLength, fDCValue);
        }
        
        m_pCVibrato->reset();
        
        //Fill the Vibrato internal buffer with DC value;
        m_pCVibrato->setParam(CVibrato::kParamVibratoWidthInSec, 0.0f);
        m_pCVibrato->setParam(CVibrato::kParamVibratoFreqInHz, 0.0f);
        m_pCVibrato->process(fDCData, m_pfTmp, m_iDataLength);
        
        m_pCVibrato->setParam(CVibrato::kParamVibratoWidthInSec, fVibratoWidthInSec);
        m_pCVibrato->setParam(CVibrato::kParamVibratoFreqInHz, fVibratoFreqInHz);
        m_pCVibrato->process(fDCData, m_pfTmp, m_iDataLength);
        
        for (int i = 0; i < m_iNumChannels; i++)
        {
            CHECK_ARRAY_CLOSE(fDCData[i], m_pfTmp[i], m_iDataLength, 1e-3);
        }
        
    }
    
    TEST_FIXTURE(VibratoData, differentInputBlockSize)
    {
        
        
    }
    
    TEST_FIXTURE(VibratoData, zeroInputSignal)
    {
        
    }

    TEST_FIXTURE(VibratoData, MyTestWithFixture)
    {
        // e.g., you can use the "VibratoData" contents
    }
}

#endif //WITH_TESTS
