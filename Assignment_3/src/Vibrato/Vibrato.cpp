
// standard headers

// project headers
#include <iostream>
#include "MUSI6106Config.h"
#include "ErrorDef.h"
#include "RingBuffer.h"
#include "Lfo.h"
#include "Vibrato.h"

#define MIN_VIBRATO_WIDTH_IN_SEC 0.0f
#define MAX_VIBRATO_WIDTH_IN_SEC 1000.0f
#define DEFAULT_VIBRATO_WIDTH_IN_SEC 0.0f
#define MIN_VIBRATO_FREQ_IN_HZ 0.0f
#define MAX_VIBRATO_FREQ_IN_HZ 100.0f
#define DEFAULT_VIBRATO_FREQ_IN_HZ 0.0f

CVibrato::CVibrato(float fSampleRateInHz, int iNumChannels):
m_fSampleRate(fSampleRateInHz),
m_iNumChannels(iNumChannels),
m_ppCRingBuffer(0),
m_pCLfo(0)
{
    m_afParam[kParamVibratoWidthInSec] = DEFAULT_VIBRATO_WIDTH_IN_SEC;
    m_afParam[kParamVibratoFreqInHz] = DEFAULT_VIBRATO_FREQ_IN_HZ;
    m_aafParamRange[kParamVibratoWidthInSec][0] = MIN_VIBRATO_WIDTH_IN_SEC;
    m_aafParamRange[kParamVibratoWidthInSec][1] = MAX_VIBRATO_WIDTH_IN_SEC;
    m_aafParamRange[kParamVibratoFreqInHz][0] = MIN_VIBRATO_FREQ_IN_HZ;
    m_aafParamRange[kParamVibratoFreqInHz][1] = MAX_VIBRATO_FREQ_IN_HZ;
    
    m_pCLfo = new CLfo (m_afParam[kParamVibratoFreqInHz], m_afParam[kParamVibratoWidthInSec], m_fSampleRate);
    m_ppCRingBuffer = new CRingBuffer<float>* [iNumChannels];
    for(int i = 0; i < iNumChannels; i++)
    {
        m_ppCRingBuffer[i] = new CRingBuffer<float>(1 + int(MAX_VIBRATO_WIDTH_IN_SEC * m_fSampleRate) * 2);
        m_ppCRingBuffer[i]->reset();
    }
    isUsingDefaultParams[0] = true;
    isUsingDefaultParams[1] = true;
    isFirstTimeProcess = true;
}

CVibrato::~CVibrato()
{
    delete m_pCLfo;
    m_pCLfo = 0;
    
    for(int i = 0; i < m_iNumChannels; i++)
        delete m_ppCRingBuffer[i];
    delete [] m_ppCRingBuffer;
    m_ppCRingBuffer = 0;
}


Error_t CVibrato::reset()
{
    m_afParam[kParamVibratoWidthInSec] = 0.0f;
    m_afParam[kParamVibratoFreqInHz] = 0.0f;
    instantiateLfo(m_pCLfo);
    
    for(int i = 0; i < m_iNumChannels; i++)
        m_ppCRingBuffer[i]->reset();
    
    return kNoError;
}

Error_t CVibrato::instantiateLfo (CLfo*& pCLfo)
{
    delete m_pCLfo;
    m_pCLfo = new CLfo (m_afParam[kParamVibratoFreqInHz], m_afParam[kParamVibratoWidthInSec] * m_fSampleRate, m_fSampleRate);
    
    return kNoError;
}

Error_t CVibrato::setParam(VibratoParam_t eParam, float fParamValue)
{
    if (!isInParamRange(eParam, fParamValue))
    {
        std::cout << "Warning: " << (eParam ? "Vibrato Frequency" : "Vibrato Width") << " out of bounds and was set to default value: "
                  << (eParam ? DEFAULT_VIBRATO_FREQ_IN_HZ : DEFAULT_VIBRATO_WIDTH_IN_SEC) << "." << std::endl;
        return kFunctionInvalidArgsError;
    }
    switch (eParam)
    {
        case kParamVibratoFreqInHz:
        {
            isUsingDefaultParams[1] = false;
            m_afParam[kParamVibratoFreqInHz] = fParamValue;
            instantiateLfo(m_pCLfo);
        }
        default:
        case kParamVibratoWidthInSec:
        {
            isUsingDefaultParams[0] = false;
            m_afParam[kParamVibratoWidthInSec] = fParamValue;
            instantiateLfo(m_pCLfo);
            for(int i = 0; i < m_iNumChannels; i++)
            {
                m_ppCRingBuffer[i]->reset();
                m_ppCRingBuffer[i]->setWriteIdx(int(m_afParam[kParamVibratoWidthInSec] * m_fSampleRate));
                m_ppCRingBuffer[i]->setReadIdx(0);
            }
        }
    }

    return kNoError;
}

float CVibrato::getParam(VibratoParam_t eParam) const
{
    return m_afParam[eParam];

}

bool CVibrato::isInParamRange(VibratoParam_t eParam, float fValue)
{
    if (fValue < m_aafParamRange[eParam][0] || fValue > m_aafParamRange[eParam][1])
    {
        return false;
    }
    else
    {
        return true;
    }
}

Error_t CVibrato::process( float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames )
{
    if (isFirstTimeProcess)
    {
        if (isUsingDefaultParams[0])
            std::cout << "Warning: Using default value (0.0f) for vibrato width in seconds." << std::endl;
        if (isUsingDefaultParams[1])
            std::cout << "Warning: Using default values (0.0f) for vibrato frequency in Hz." << std::endl;
        isFirstTimeProcess = false;
    }
    

    for(int j = 0; j < iNumberOfFrames; j++)
    {
        float fLfoValue = m_pCLfo->getNextValue();
        for(int i = 0; i < m_iNumChannels; i++)
        {
            m_ppCRingBuffer[i]->putPostInc(ppfInputBuffer[i][j]);
            ppfOutputBuffer[i][j] = m_ppCRingBuffer[i]->get(fLfoValue);
            m_ppCRingBuffer[i]->getPostInc();
            
        }
    
    }
    
    return kNoError;
}

