#if !defined(__Vibrato_hdr__)
#define __Vibrato_hdr__

#include "ErrorDef.h"

class CLfo;
template <class T>
class CRingBuffer;

#include "ErrorDef.h"

class CVibrato
{
public:

    enum VibratoParam_t
    {
        kParamModWidthInS,
        kParamModFreqInHz,

        kNumVibratoParams
    };
    
    enum VibratoParamBounds_t
    {
        kParamMin,
        kParamMax,
        
        kNumVibratoParamBounds
    };

    static Error_t createInstance (CVibrato*& pCVibrato);
    static Error_t destroyInstance (CVibrato*& pCVibrato);

    Error_t initInstance (float fMaxModWidthInS, float fSampleRateInHz, int iNumChannels);
    Error_t resetInstance ();

    Error_t setParam (VibratoParam_t eParam, float fParamValue);
    float getParam (VibratoParam_t eParam) const;
    float getParamRange(VibratoParam_t eParam, VibratoParamBounds_t eParamBound);

    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
    
    void    setBypassState(bool state);
    bool    getBypassState();

protected:
    CVibrato ();
    virtual ~CVibrato ();

private:
    bool isInParamRange (VibratoParam_t eParam, float fValue);

    bool m_bIsInitialized;

    CLfo *m_pCLfo;
    CRingBuffer<float> **m_ppCRingBuff;

    float   m_fSampleRate;
    int     m_iNumChannels;
    bool    m_bBypassState;
    float   m_aafParamRange[kNumVibratoParams][kNumVibratoParamBounds];
};

#endif // #if !defined(__Vibrato_hdr__)
