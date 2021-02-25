/*
  ==============================================================================

    DeckGUI.h
    Created: 27 Jan 2020 4:52:38pm
    Author:  matthew

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"

using namespace juce;

//==============================================================================
/*
*/
class DeckGUI    : public Component, 
                   public Button::Listener, 
                   public Slider::Listener, 
                   public FileDragAndDropTarget
{
public:
    DeckGUI(DJAudioPlayer* _djAudioPlayer);
    ~DeckGUI();

    void paint (Graphics&) override;
    void resized() override;

    void buttonClicked(Button* button) override;
    void sliderValueChanged (Slider *slider) override;

    bool isInterestedInFileDrag(const StringArray& files) override;
    void filesDropped(const StringArray& files, int x, int y) override;

private:

    TextButton playButton;
    TextButton stopButton;
    TextButton loadButton;
    Slider volumeSlider;
    Slider positionSlider; 
    DJAudioPlayer* djAudioPlayer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};
