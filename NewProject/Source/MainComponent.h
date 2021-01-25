#pragma once

#include <JuceHeader.h>

using namespace juce;

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent,
                       public Button::Listener,
                       public Slider::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    // implement buttonlistener
    void buttonClicked (Button *) override;
    void sliderValueChanged (Slider *slider) override;

private:
    //==============================================================================
    // Your private member variables go here...



    TextButton playButton{"PLAY"};
    TextButton stopButton{"STOP"};
    TextButton loadButton{"Load"};

    Slider volumeSlider;
    Toolbar toolBar1;
    // WebBrowserComponent browser;
    AudioVisualiserComponent audioVis{1};

    Slider track1mix;
    Slider track2mix;

    // Audio stuff
    Random rand;

    double phase;
    double dphase;

    // Audioformatmanager
    AudioFormatManager formatManager;

    std::unique_ptr<AudioFormatReaderSource> readerSource;

    AudioTransportSource transportSource;
    ResamplingAudioSource resampleSource{&transportSource, false, 2};

    bool playing;

    void loadURL(URL audioURL);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
