#if !defined(__Vibrato_hdr__)
#define __Vibrato_hdr__

#include "ErrorDef.h"

// forward declaration
class CLfo;
template <class T>
class CRingBuffer;

/*
 * brief explanation about your class-interface design
 */

class CVibrato
{
public:
    enum VibratoParam_t
    {
        kParamDelay,                    //!< delay in seconds for specification of ring buffer
        kParamVibratoWidth,
        kParamVibratoFreq,
        
        kNumVibratoParams
    };
    
    CVibrato (float fVibratoFreqInHz, float fVibratoWidthInSec, float fSampleRateInHz, int iNumChannels);
    
    virtual ~CVibrato ();
    
    /*! resets the internal variables (requires new call of init)
     \return Error_t
     */
    Error_t reset ();
    
    /*! sets a comb filter parameter
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
    int     m_iNumChannels;
    
    CRingBuffer<float>  **m_ppCRingBuffer;
    CLfo *m_pCLfo;
    
    int   m_afParam[kNumVibratoParams];
    int   m_aafParamRange[kNumVibratoParams][2];
    
    bool    isUsingDefaultParams[2];
    bool    isFirstTimeProcess;
    
    bool    isInParamRange (VibratoParam_t eParam, float fValue);
    Error_t instantiateLfo (CLfo*& pCLfo);

};

#endif // #if !defined(__Vibrato_hdr__)
