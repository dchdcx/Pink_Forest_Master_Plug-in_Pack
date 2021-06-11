#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (500, 250);
}

MainComponent::~MainComponent()
{
    
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll(Colours::dimgrey );

    g.setFont (20.0f);
    g.setColour (Colours::pink); //❀
    g.drawText ("Welcome to Pink Forest master plug-ins Pack", getLocalBounds(), Justification::centred, true);
    g.drawText ("_________________", getLocalBounds(), Justification::centred, true);
}

void MainComponent::resized()
{

}
