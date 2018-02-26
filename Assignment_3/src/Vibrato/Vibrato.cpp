
// standard headers

// project headers
#include "MUSI6106Config.h"

#include "ErrorDef.h"

#include "RingBuffer.h"

#include "Lfo.h"
#include "Vibrato.h"

CVibrato::CVibrato(float fVibratoFreqInHz, float fVibratoWidthInSec, float fSampleRateInHz, int iNumChannels):
m_fSampleRate(fSampleRateInHz),
m_iNumChannels(iNumChannels),
m_afParam[kParamDelay](int(round(fVibratoWidthInSec * fSampleRateInHz))),
m_afParam[kParamVibratoFreq](fVibratoFreqInHz),
m_afParam[kParamVibratoWidth](int(round(fVibratoWidthInSec * fSampleRateInHz)))
{
    
    instantiateLfo(m_pCLfo);
    m_ppCRingBuffer = new CRingBuffer<float>* [iNumChannels];
    for(int i = 0; i < iNumChannels; i++)
    {
        m_ppCRingBuffer[i] = new CRingBuffer<float>(m_afParam[kParamDelay] + m_afParam[kParamVibratoWidth] * 2);
    }
    
    
}

CVibrato::~CVibrato()
{
    delete[] m_pCLfo;
    m_pCLfo = 0;
    
    for(int i = 0; i < m_iNumChannels; i++)
    {
        delete m_ppCRingBuffer[i];
    }
    delete [] m_ppCRingBuffer;
    m_ppCRingBuffer = 0;
}


Error_t CVibrato::resetInstance()
{
    return kNoError;
}

Error_t instantiateLfo (CLfo*& pCLfo)
{
    delete[] m_pCLfo;
    m_pCLfo = new CLfo (m_afParam[kParamVibratoFreq], m_afParam[kParamVibratoWidth], m_fSampleRate);
}

Error_t CVibrato::setParam(VibratoParam_t eParam, float fParamValue)
{
    if (!isInParamRange(eParam, fParamValue))
        return kFunctionInvalidArgsError;
    switch (eParam)
    {
        case kParamDelay:
            m_afParam[kParamDelay] = int(round(fParamValue * fSampleRateInHz));
        case kParamVibratoFreq:
        {
            m_afParam[kParamVibratoFreq] = fParamValue;
            instantiateLfo(m_pCLfo);
        }
        default:
        case kParamVibratoWidth:
        {
            m_afParam[kParamVibratoWidth] = fParamValue;
            instantiateLfo(m_pCLfo);
        }
    }
    

    return kNoError;
}

float CVibrato::getParam(VibratoParam_t eParam) const
{
    return 0;
}

bool CVibrato::isInParamRange(VibratoParam_t eParam, float fParamValue)
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
    
}

