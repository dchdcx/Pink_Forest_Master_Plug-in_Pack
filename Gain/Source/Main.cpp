#include <JuceHeader.h>
#include "Header.h"

//==============================================================================
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TutorialProcessor();
}
