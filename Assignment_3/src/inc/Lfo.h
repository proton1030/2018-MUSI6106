#if !defined(__Lfo_hdr__)
#define __Lfo_hdr__

#define _USE_MATH_DEFINES
#include <math.h>

#include "ErrorDef.h"
#include "Synthesis.h"
#include "RingBuffer.h"

#include <iostream>

#define WAVETABLE_FREQ 1.0f
#define WAVETABLE_WIDTH 1.0f


class CLfo
{
public:
    CLfo (float fLfoFreqInHz, float fLfoWidthInSamples, float fSampleFreqInHz) :
    m_iBufferLength(int(ceil(fSampleFreqInHz))),
    m_fSampleRate(fSampleFreqInHz),
    m_fLfoFreqInHz(fLfoFreqInHz),
    m_fLfoWidthInSamples(fLfoWidthInSamples),
    m_fReadLoc(0.0f),
    m_ptSinewaveBuff(0),
    m_pCRingBuffer(0)
    {
        m_ptSinewaveBuff = new float[m_iBufferLength];
        m_pCRingBuffer = new CRingBuffer<float>(m_iBufferLength);
        CSynthesis::generateSine (m_ptSinewaveBuff, WAVETABLE_FREQ, m_iBufferLength, m_iBufferLength);
        m_pCRingBuffer->put(m_ptSinewaveBuff , m_iBufferLength);
    }
    
    ~CLfo ()
    {
        delete []  m_ptSinewaveBuff;
        m_ptSinewaveBuff = 0;
        
        m_pCRingBuffer->reset();
        delete m_pCRingBuffer;
        m_pCRingBuffer = 0;
        
    };
    
    void reset ()
    {
        m_fReadLoc = 0.0f;
    }
    
    float getNextValue ()
    {
        float fReturnValue = m_fLfoWidthInSamples * m_pCRingBuffer->get(m_fReadLoc);
        m_fReadLoc = m_fReadLoc + m_fLfoFreqInHz * (m_iBufferLength / m_fSampleRate) > m_iBufferLength ?
                    m_fReadLoc + m_fLfoFreqInHz * (m_iBufferLength / m_fSampleRate) - m_iBufferLength :
                    m_fReadLoc + m_fLfoFreqInHz * (m_iBufferLength / m_fSampleRate);
        return fReturnValue;
    }
    
    void setLfoFreqInHz (float freqInHz)
    {
        m_fLfoFreqInHz = freqInHz;
    }
    
    void setLfoWidthInSamples (float widthInSecs)
    {
        m_fLfoWidthInSamples = widthInSecs * m_fSampleRate;
    }
    
    
private:
    int                     m_iBufferLength;
    float                   m_fSampleRate,
                            m_fLfoFreqInHz,
                            m_fLfoWidthInSamples,
                            m_fReadLoc;                 //!< audio sample rate in Hz
    float                   *m_ptSinewaveBuff;
    CRingBuffer<float>      *m_pCRingBuffer;
};

#endif // __Lfo_hdr__
