#include "MUSI6106Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>
#include <algorithm>

#include "UnitTest++.h"

#include "Synthesis.h"
#include "Vector.h"
#include "ErrorDef.h"

#include "Vibrato.h"

SUITE(Vibrato)
{
    struct VibratoData
    {
        VibratoData()
        {
            const float     fSampleFreq = 44100.F;
            const int       iNumChannels = 2;
            
            m_pCVibrato = new CVibrato(fSampleFreq, iNumChannels);
        }

        ~VibratoData()
        {
            // teardown
            // e.g., deallocate the vibrato object and test signal
        }

        // e.g., a reusable process() function

        // e.g., a member vibrato object to be reused in each test
        CVibrato    *m_pCVibrato;
    };

    TEST(MyTestWithNoFixture)
    {
        // e.g., allocate & deallocate local data for testing
    }

    TEST_FIXTURE(VibratoData, MyTestWithFixture)
    {
        // e.g., you can use the "VibratoData" contents
    }
}

#endif //WITH_TESTS
