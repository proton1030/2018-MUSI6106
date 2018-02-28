
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
#define MIN_VIBRATO_DELAY_IN_SEC 0.0f
#define MAX_VIBRATO_DELAY_IN_SEC 100.0f
#define DEFAULT_VIBRATO_DELAY_IN_SEC 0.0f

CVibrato::CVibrato(float fSampleRateInHz, int iNumChannels):
m_fSampleRate(fSampleRateInHz),
m_iNumChannels(iNumChannels),
m_ppCRingBuffer(0),
m_pCLfo(0)
{
    m_afParam[kParamVibratoWidthInSec] = DEFAULT_VIBRATO_WIDTH_IN_SEC;
    m_afParam[kParamVibratoFreqInHz] = DEFAULT_VIBRATO_FREQ_IN_HZ;
    m_afParam[kParamVibratoDelayInSec] = DEFAULT_VIBRATO_DELAY_IN_SEC;
    m_aafParamRange[kParamVibratoWidthInSec][kVibratoParamMin] = MIN_VIBRATO_WIDTH_IN_SEC;
    m_aafParamRange[kParamVibratoWidthInSec][kVibratoParamMax] = MAX_VIBRATO_WIDTH_IN_SEC;
    m_aafParamRange[kParamVibratoFreqInHz][kVibratoParamMin] = MIN_VIBRATO_FREQ_IN_HZ;
    m_aafParamRange[kParamVibratoFreqInHz][kVibratoParamMax] = MAX_VIBRATO_FREQ_IN_HZ;
    m_aafParamRange[kParamVibratoDelayInSec][kVibratoParamMin] = MIN_VIBRATO_DELAY_IN_SEC;
    m_aafParamRange[kParamVibratoDelayInSec][kVibratoParamMax] = MAX_VIBRATO_DELAY_IN_SEC;
    
    m_pCLfo = new CLfo (m_afParam[kParamVibratoFreqInHz], m_afParam[kParamVibratoWidthInSec], m_fSampleRate);
    m_ppCRingBuffer = new CRingBuffer<float>* [iNumChannels];
    for(int i = 0; i < iNumChannels; i++)
    {
        m_ppCRingBuffer[i] = new CRingBuffer<float>(1 + int(MAX_VIBRATO_DELAY_IN_SEC) +
                                                    int(MAX_VIBRATO_WIDTH_IN_SEC * m_fSampleRate) * 2);
        m_ppCRingBuffer[i]->reset();
    }
    isUsingDefaultParams[kParamVibratoWidthInSec] = true;
    isUsingDefaultParams[kParamVibratoFreqInHz] = true;
    isUsingDefaultParams[kParamVibratoDelayInSec] = true;
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
    m_afParam[kParamVibratoWidthInSec] = DEFAULT_VIBRATO_WIDTH_IN_SEC;
    m_afParam[kParamVibratoFreqInHz] = DEFAULT_VIBRATO_FREQ_IN_HZ;
    m_afParam[kParamVibratoDelayInSec] = DEFAULT_VIBRATO_DELAY_IN_SEC;
    m_pCLfo->reset();
    for(int i = 0; i < m_iNumChannels; i++)
        m_ppCRingBuffer[i]->reset();
    
    return kNoError;
}


Error_t CVibrato::setParam(VibratoParam_t eParam, float fParamValue)
{
    if (!isInParamRange(eParam, fParamValue))
    {
        switch (eParam)
        {
            case kParamVibratoFreqInHz:
            {
                std::cout << "Warning: Vibrato Frequency out of bounds and was set to default value: "
                << DEFAULT_VIBRATO_WIDTH_IN_SEC << "." << std::endl;
                m_afParam[kParamVibratoWidthInSec] = DEFAULT_VIBRATO_WIDTH_IN_SEC;
            }
            break;
            case kParamVibratoWidthInSec:
            {
                std::cout << "Warning: Vibrato Width out of bounds and was set to default value: "
                << DEFAULT_VIBRATO_FREQ_IN_HZ << "." << std::endl;
                m_afParam[kParamVibratoFreqInHz] = DEFAULT_VIBRATO_FREQ_IN_HZ;
            }
            break;
            case kParamVibratoDelayInSec:
            {
                std::cout << "Warning: Vibrato Delay out of bounds and was set to default value: "
                << DEFAULT_VIBRATO_DELAY_IN_SEC << "." << std::endl;
                m_afParam[kParamVibratoDelayInSec] = DEFAULT_VIBRATO_DELAY_IN_SEC;
            }
            break;
            default:
            {
                std::cout << "Warning: Undefined parameter passed." << std::endl;
            }
        }
        
        return kFunctionInvalidArgsError;
    }
    switch (eParam)
    {
        case kParamVibratoFreqInHz:
        {
            isUsingDefaultParams[kParamVibratoFreqInHz] = false;
            m_afParam[kParamVibratoFreqInHz] = fParamValue;
            m_pCLfo->setLfoFreqInHz(fParamValue);
        
        }
        break;
        case kParamVibratoWidthInSec:
        {
            isUsingDefaultParams[kParamVibratoWidthInSec] = false;
            m_pCLfo->setLfoWidthInSamples(fParamValue);
            for(int i = 0; i < m_iNumChannels; i++)
                m_ppCRingBuffer[i]->setWriteIdx(m_ppCRingBuffer[i]->getWriteIdx() + int((fParamValue - m_afParam[kParamVibratoWidthInSec]) * m_fSampleRate));
            m_afParam[kParamVibratoWidthInSec] = fParamValue;
            return kNoError;
        }
        break;
        case kParamVibratoDelayInSec:
        {
            isUsingDefaultParams[kParamVibratoDelayInSec] = false;
            
            for(int i = 0; i < m_iNumChannels; i++)
            {
                m_ppCRingBuffer[i]->setWriteIdx(m_ppCRingBuffer[i]->getWriteIdx() + int((fParamValue - m_afParam[kParamVibratoDelayInSec]) * m_fSampleRate));
            }
            m_afParam[kParamVibratoDelayInSec] = fParamValue;
        }
        break;
        default:
        {
            std::cout << "Undefined parameter changed." << std::endl;
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
    if (fValue < m_aafParamRange[eParam][kVibratoParamMin] || fValue > m_aafParamRange[eParam][kVibratoParamMax])
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
        if (isUsingDefaultParams[kParamVibratoWidthInSec])
            std::cout << "Warning: Using default value (0.0f) for vibrato width in seconds." << std::endl;
        if (isUsingDefaultParams[kParamVibratoFreqInHz])
            std::cout << "Warning: Using default value (0.0f) for vibrato frequency in Hz." << std::endl;
        if (isUsingDefaultParams[kParamVibratoDelayInSec])
            std::cout << "Warning: Using default value (0.0f) for vibrato delay in seconds." << std::endl;
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

