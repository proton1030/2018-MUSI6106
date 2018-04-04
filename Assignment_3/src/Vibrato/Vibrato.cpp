
// standard headers

// project headers
#include "MUSI6106Config.h"
#include "ErrorDef.h"
#include "RingBuffer.h"
#include "Lfo.h"
#include "Vibrato.h"
#include <limits>

#define MIN_VIBRATO_WIDTH_IN_SEC 0.0f
#define MAX_VIBRATO_WIDTH_IN_SEC 0.5f
#define MIN_VIBRATO_FREQ_IN_HZ 0.0f
#define MAX_VIBRATO_FREQ_IN_HZ std::numeric_limits<float>::max()
#define DEFAULT_VIBRATO_WIDTH_IN_SEC MIN_VIBRATO_WIDTH_IN_SEC
#define DEFAULT_VIBRATO_FREQ_IN_HZ MIN_VIBRATO_FREQ_IN_HZ

CVibrato::CVibrato(float fSampleRateInHz, int iNumChannels):
m_fSampleRate(fSampleRateInHz),
m_iNumChannels(iNumChannels),
m_ppCRingBuffer(0),
m_pCLfo(0)
{
    m_afParam[kParamVibratoWidthInSec] = DEFAULT_VIBRATO_WIDTH_IN_SEC;
    m_afParam[kParamVibratoFreqInHz] = DEFAULT_VIBRATO_FREQ_IN_HZ;
    m_aafParamRange[kParamVibratoWidthInSec][kVibratoParamMin] = MIN_VIBRATO_WIDTH_IN_SEC;
    m_aafParamRange[kParamVibratoWidthInSec][kVibratoParamMax] = MAX_VIBRATO_WIDTH_IN_SEC;
    m_aafParamRange[kParamVibratoFreqInHz][kVibratoParamMin] = MIN_VIBRATO_FREQ_IN_HZ;
    m_aafParamRange[kParamVibratoFreqInHz][kVibratoParamMax] = MAX_VIBRATO_FREQ_IN_HZ;

    m_pCLfo = new CLfo (m_afParam[kParamVibratoFreqInHz], m_afParam[kParamVibratoWidthInSec], m_fSampleRate);
    m_ppCRingBuffer = new CRingBuffer<float>* [iNumChannels];
    for(int i = 0; i < iNumChannels; i++)
    {
        m_ppCRingBuffer[i] = new CRingBuffer<float>(1 + int(MAX_VIBRATO_WIDTH_IN_SEC * m_fSampleRate) * 2);
        m_ppCRingBuffer[i]->reset();
    }
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
    m_pCLfo->reset();
    for(int i = 0; i < m_iNumChannels; i++)
        m_ppCRingBuffer[i]->reset();

    return kNoError;
}


Error_t CVibrato::setParam(VibratoParam_t eParam, float fParamValue)
{
    if (!isInParamRange(eParam, fParamValue))
    {
        return kFunctionInvalidArgsError;
    }
    switch (eParam)
    {
        case kParamVibratoFreqInHz:
        {
            m_afParam[kParamVibratoFreqInHz] = fParamValue;
            m_pCLfo->setLfoFreqInHz(fParamValue);

        }
        break;
        case kParamVibratoWidthInSec:
        {
            m_pCLfo->setLfoWidthInSamples(fParamValue);
            for(int i = 0; i < m_iNumChannels; i++)
                m_ppCRingBuffer[i]->setWriteIdx(m_ppCRingBuffer[i]->getReadIdx() + int(fParamValue * m_fSampleRate));
            m_afParam[kParamVibratoWidthInSec] = fParamValue;
        }
        break;
        default:
        {
            return kFunctionInvalidArgsError;
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
