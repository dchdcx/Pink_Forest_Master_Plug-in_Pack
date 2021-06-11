#pragma once

using namespace std;

//==============================================================================
class GenericEditor : public AudioProcessorEditor
{
public:
    enum
    {
        paramControlHeight = 40,
        paramLabelWidth    = 80,
        paramSliderWidth   = 300
    };

    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    GenericEditor (AudioProcessor& parent, AudioProcessorValueTreeState& vts)
        : AudioProcessorEditor (parent),
          valueTreeState (vts)
    {
        gainLabel.setText ("Gain", dontSendNotification);
        addAndMakeVisible (gainLabel);

        addAndMakeVisible (gainSlider);
        gainAttachment.reset (new SliderAttachment (valueTreeState, "gain", gainSlider));

        invertButton.setButtonText ("Enable Inverting Phase");
        addAndMakeVisible (invertButton);
        invertAttachment.reset (new ButtonAttachment (valueTreeState, "invertPhase", invertButton));

        setSize (paramSliderWidth + paramLabelWidth, jmax (100, paramControlHeight * 2));
    }

    void resized() override
    {
        auto r = getLocalBounds();

        auto gainRect = r.removeFromTop (paramControlHeight);
        gainLabel .setBounds (gainRect.removeFromLeft (paramLabelWidth));
        gainSlider.setBounds (gainRect);

        invertButton.setBounds (r.removeFromTop (paramControlHeight));
    }

    void paint (Graphics& g) override
    {
        setSize (500, 250);
        g.fillAll(Colours::dimgrey );
    }

private:
    AudioProcessorValueTreeState& valueTreeState;

    Label gainLabel;
    Slider gainSlider;
    unique_ptr<SliderAttachment> gainAttachment;

    ToggleButton invertButton;
    unique_ptr<ButtonAttachment> invertAttachment;
};

//==============================================================================
class TutorialProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    TutorialProcessor()
        : parameters (*this, nullptr, Identifier ("APVTSTutorial"),
                      {
                          make_unique<AudioParameterFloat> ("gain",            // parameterID
                                                                       "Gain",            // parameter name
                                                                       0.0f,              // minimum value
                                                                       10.0f,              // maximum value
                                                                       1.0f),             // default value
                          make_unique<AudioParameterBool> ("invertPhase",      // parameterID
                                                                      "Invert Phase",     // parameter name
                                                                      false)              // default value
                      })
    {
        phaseParameter = parameters.getRawParameterValue ("invertPhase");
        gainParameter  = parameters.getRawParameterValue ("gain");
    }

    //==============================================================================
    void prepareToPlay (double, int) override
    {
        auto phase = *phaseParameter < 0.5f ? 1.0f : -1.0f;
        previousGain = *gainParameter * phase;
    }

    void releaseResources() override {}

    void processBlock (AudioSampleBuffer& buffer, MidiBuffer&) override
    {
        auto phase = *phaseParameter < 0.5f ? 1.0f : -1.0f;
        auto currentGain = *gainParameter * phase;

        if (currentGain == previousGain)
        {
            buffer.applyGain (currentGain);
        }
        else
        {
            buffer.applyGainRamp (0, buffer.getNumSamples(), previousGain, currentGain);
            previousGain = currentGain;
        }
    }

    //==============================================================================
    AudioProcessorEditor* createEditor() override          { return new GenericEditor (*this, parameters); }
    bool hasEditor() const override                              { return true; }

    //==============================================================================
    const String getName() const override                  { return "APVTS Tutorial"; }
    bool acceptsMidi() const override                            { return false; }
    bool producesMidi() const override                           { return false; }
    double getTailLengthSeconds() const override                 { return 0; }

    //==============================================================================
    int getNumPrograms() override                                { return 1; }
    int getCurrentProgram() override                             { return 0; }
    void setCurrentProgram (int) override                        {}
    const String getProgramName (int) override             { return {}; }
    void changeProgramName (int, const String&) override   {}

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override
    {
        auto state = parameters.copyState();
        unique_ptr<XmlElement> xml (state.createXml());
        copyXmlToBinary (*xml, destData);
    }

    void setStateInformation (const void* data, int sizeInBytes) override
    {
        unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

        if (xmlState.get() != nullptr)
            if (xmlState->hasTagName (parameters.state.getType()))
                parameters.replaceState (ValueTree::fromXml (*xmlState));
    }

private:
    //==============================================================================
    AudioProcessorValueTreeState parameters;
    float previousGain; // [1]

    atomic<float>* phaseParameter = nullptr;
    atomic<float>* gainParameter  = nullptr;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TutorialProcessor)
};
