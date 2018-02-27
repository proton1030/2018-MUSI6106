#if !defined(__Lfo_hdr__)
#define __Lfo_hdr__

#define _USE_MATH_DEFINES
#include <math.h>

#include "ErrorDef.h"
#include "Synthesis.h"

#define MIN_LFO_FREQ_IN_HZ 0.1f

class CLfo
{
public:
    CLfo (float fLfoFreqInHz, float fLfoWidthInSamples, float fSampleFreqInHz) :
    iReadPtr(0),
    m_fSampleRate(fSampleFreqInHz),
    m_ptSinewaveBuff(0)
    {
        m_iSinewaveBuffLength = fLfoFreqInHz != 0.0f ? int(m_fSampleRate / fLfoFreqInHz) : 1;
        m_ptSinewaveBuff = new float [m_iSinewaveBuffLength];
        CSynthesis::generateSine (m_ptSinewaveBuff, fLfoFreqInHz, m_fSampleRate, m_iSinewaveBuffLength, int(fLfoWidthInSamples));
    }
    
    ~CLfo ()
    {
        delete []  m_ptSinewaveBuff;
        m_ptSinewaveBuff = 0;
    };
    
    void reset ()
    {
        iReadPtr = 0;
    }
    
    float getNextValue ()
    {
        float fReturnValue = m_ptSinewaveBuff[iReadPtr];
        iReadPtr = iReadPtr + 1 >= m_iSinewaveBuffLength ? 0 : iReadPtr + 1;
        return fReturnValue;
    }
    
private:
    int     m_iSinewaveBuffLength,      //!< sine wave table length
            iReadPtr;                   //!< wavetable read pointer
    float   m_fSampleRate;              //!< audio sample rate in Hz
    float   *m_ptSinewaveBuff;          //!< wavetable read pointer
};

#endif // __Lfo_hdr__
