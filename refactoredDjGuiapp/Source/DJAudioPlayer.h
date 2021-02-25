/*
  ==============================================================================

    DJAudioPlayer.h
    Created: 22 Jan 2020 7:55:53pm
    Author:  matthew

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

using namespace juce;

class DJAudioPlayer : public AudioSource {
  public:
    DJAudioPlayer();
    ~DJAudioPlayer();
    void load(URL file);
    void play();
    void stop();
    void setPosition(double posInSecs);
    void setPositionRelative(double pos); 
    void setGain(double gain);

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;


private:

    AudioFormatManager formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;

};