
// standard headers

// project headers
//#include "MUSI6106Config.h"

#include "ErrorDef.h"

#include "RingBuffer.h"

#include "Lfo.h"
#include "Vibrato.h"



CVibrato::CVibrato () :
    m_bIsInitialized(false),
    m_pCLfo(0),
    m_ppCRingBuff(0),
    m_fSampleRate(44100),
    m_iNumChannels(0)
{

    // this never hurts
    this->resetInstance ();
}


CVibrato::~CVibrato ()
{
    this->resetInstance ();
}

Error_t CVibrato::createInstance (CVibrato*& pCVibrato)
{
    pCVibrato = new CVibrato ();

    if (!pCVibrato)
        return kUnknownError;


    return kNoError;
}

Error_t CVibrato::destroyInstance (CVibrato*& pCVibrato)
{
    if (!pCVibrato)
        return kUnknownError;
    
    pCVibrato->resetInstance ();
    
    delete pCVibrato;
    pCVibrato = 0;

    return kNoError;

}

Error_t CVibrato::initInstance( float fMaxModWidthInS, float fSampleRateInHz, int iNumChannels )
{
    // set parameters
    m_fSampleRate       = fSampleRateInHz;
    m_iNumChannels      = iNumChannels;
    m_bBypassState      = false;

    // set parameter ranges
    m_aafParamRange[kParamModFreqInHz][kParamMin] = 0;
    m_aafParamRange[kParamModFreqInHz][kParamMax] = fSampleRateInHz*.5F;
    m_aafParamRange[kParamModWidthInS][kParamMin] = 0;
    m_aafParamRange[kParamModWidthInS][kParamMax] = fMaxModWidthInS;


    // create instances
    m_ppCRingBuff       = new CRingBuffer<float>*[m_iNumChannels];
    for (int c= 0; c < m_iNumChannels; c++)
    {
        m_ppCRingBuff[c]= new CRingBuffer<float>(CUtil::float2int<int>(fMaxModWidthInS*m_fSampleRate*2+1));
        m_ppCRingBuff[c]->setWriteIdx(CUtil::float2int<int>(fMaxModWidthInS*m_fSampleRate+1));
    }

    m_pCLfo             = new CLfo(m_fSampleRate);

    m_bIsInitialized    = true;

    return kNoError;
}

Error_t CVibrato::resetInstance ()
{
    for (int i = 0; i < kNumVibratoParams; i++)
        setParam((VibratoParam_t)i, 0);

    for (int c= 0; c < m_iNumChannels; c++)
        delete m_ppCRingBuff[c];
    delete [] m_ppCRingBuff;
    m_ppCRingBuff       = 0;
    delete m_pCLfo;
    m_pCLfo             = 0;
    
    m_iNumChannels      = 0;
    m_bIsInitialized    = false;
    m_bBypassState      = false;

    return kNoError;
}

Error_t CVibrato::setParam( VibratoParam_t eParam, float fParamValue )
{
    if (!m_bIsInitialized)
        return kNotInitializedError;
    if (!isInParamRange(eParam, fParamValue))
        return kFunctionInvalidArgsError;

    switch (eParam)
    {
    case kParamModFreqInHz:
        return m_pCLfo->setParam(CLfo::kLfoParamFrequency, fParamValue);
    case kParamModWidthInS:
        return m_pCLfo->setParam(CLfo::kLfoParamAmplitude, fParamValue * m_fSampleRate);
    case kNumVibratoParams:
        return kFunctionInvalidArgsError;
    }

    return kNoError;
}

float CVibrato::getParam( VibratoParam_t eParam ) const
{
    switch (eParam)
    {
    case kParamModFreqInHz:
        return m_pCLfo->getParam(CLfo::kLfoParamFrequency);
    default:
    case kParamModWidthInS:
        return m_pCLfo->getParam(CLfo::kLfoParamAmplitude) / m_fSampleRate;
    }

    return kNoError;
}

float CVibrato::getParamRange(VibratoParam_t eParam, VibratoParamBounds_t eParamBound)
{
    return m_aafParamRange[eParam][eParamBound];
}

bool CVibrato::isInParamRange( VibratoParam_t eParam, float fValue )
{
    if (fValue < m_aafParamRange[eParam][kParamMin] || fValue > m_aafParamRange[eParam][kParamMax])
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
    if (!ppfInputBuffer || !ppfOutputBuffer || iNumberOfFrames < 0)
        return kFunctionInvalidArgsError;
    
    
    for (int i = 0; i < iNumberOfFrames; i++)
    {
        float fOffset = m_pCLfo->getNext();
        for (int c = 0; c < m_iNumChannels; c++)
        {
            m_ppCRingBuff[c]->putPostInc(ppfInputBuffer[c][i]);
            if (m_bBypassState)
                ppfOutputBuffer[c][i]   = ppfInputBuffer[c][i];
            else
                ppfOutputBuffer[c][i]   = m_ppCRingBuff[c]->get(fOffset);
            m_ppCRingBuff[c]->getPostInc(); // dummy call to keep write and read idx in sync
        }
    }

    return kNoError;
}

void CVibrato::setBypassState(bool state)
{
    m_bBypassState = state;
}
bool CVibrato::getBypassState()
{
    return m_bBypassState;
}


