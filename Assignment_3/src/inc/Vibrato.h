#if !defined(__Vibrato_hdr__)
#define __Vibrato_hdr__

#include "ErrorDef.h"

// forward declaration
class CLfo;
template <class T> class CRingBuffer;

/*
 * brief explanation about your class-interface design
 */

class CVibrato
{
public:
    enum VibratoParam_t
    {
        kParamVibratoWidthInSec,
        kParamVibratoFreqInHz,
        kParamVibratoDelayInSec,
        
        kNumVibratoParams
    };
    
    enum VibratoParamBound_t
    {
        kVibratoParamMax,
        kVibratoParamMin,
        
        kNumVibratoParamBounds
    };

    CVibrato (float fSampleRateInHz, int iNumChannels);
    
    ~CVibrato ();
    
    /*! resets the internal variables
     \return Error_t
     */
    Error_t reset ();
    
    /*! sets a vibrato parameter
     \param eParam what parameter (see ::FilterParam_t)
     \param fParamValue value of the parameter
     \return Error_t
     */
    Error_t setParam (VibratoParam_t eParam, float fParamValue);
    
    /*! return the value of the specified parameter
     \param eParam
     \return float
     */
    float   getParam (VibratoParam_t eParam) const;
    
    /*! processes one block of audio
     \param ppfInputBuffer input buffer [numChannels][iNumberOfFrames]
     \param ppfOutputBuffer output buffer [numChannels][iNumberOfFrames]
     \param iNumberOfFrames buffer length (per channel)
     \return Error_t
     */
    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);

private:
    float   m_fSampleRate;      //!< audio sample rate in Hz
    int     m_iNumChannels;     //!< number of channels for input data
    
    CRingBuffer<float>  **m_ppCRingBuffer;
    CLfo *m_pCLfo;
    
    float   m_afParam[kNumVibratoParams];
    float   m_aafParamRange[kNumVibratoParams][kNumVibratoParamBounds];
    
    bool    isUsingDefaultParams[kNumVibratoParams];
    bool    isFirstTimeProcess;
    
    bool    isInParamRange (VibratoParam_t eParam, float fValue);
};

#endif // #if !defined(__Vibrato_hdr__)
