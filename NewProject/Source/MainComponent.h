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
                       public Slider::Listener,
                       private ChangeListener,
                       private Timer
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
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Pausing,
        Paused,
        Stopping
    };


    void paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    void paintIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);

    TextButton playButton;
    TextButton stopButton;
    TextButton loadButton;

    Slider volumeSlider;
    Slider speedSlider;

    Toolbar toolBar1;
    // WebBrowserComponent browser;
    AudioVisualiserComponent audioVis{2};
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;

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
    TransportState state;  

    void loadURL(URL audioURL, File file);

    
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;

    // Timer callback
    void timerCallback() override;

    void transportSourceChanged()
    {
        changeState (transportSource.isPlaying() ? Playing : Stopped);
    }
    void thumbnailChanged()
    {
        repaint();
    }
        void changeState (TransportState newState)
    {
        if (state != newState)
        {
            state = newState;

            switch (state)
            {
                case Stopped:
                    playButton.setButtonText ("Play");
                    stopButton.setButtonText ("Stop");
                    stopButton.setEnabled (false);
                    transportSource.setPosition (0.0);
                    break;
 
                case Starting:
                    transportSource.start();
                    break;
 
                case Playing:
                    playButton.setButtonText ("Pause");
                    stopButton.setButtonText ("Stop");
                    stopButton.setEnabled (true);
                    break;
 
                case Pausing:
                    transportSource.stop();
                    break;
 
                case Paused:
                    playButton.setButtonText ("Resume");
                    stopButton.setButtonText ("Return to Zero");
                    break;
 
                case Stopping:
                    transportSource.stop();
                    break;

                default:
                    jassertfalse;
                    break;
            }
        }
    }



    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
