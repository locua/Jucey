/*
  ==============================================================================

    DeckGUI.cpp
    Created: 27 Jan 2020 4:52:38pm
    Author:  matthew

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _djAudioPlayer) : djAudioPlayer{_djAudioPlayer}
{
    playButton.setButtonText("PLAY");
    stopButton.setButtonText("STOP");
    volumeSlider.setRange(0.0, 1.0);
    positionSlider.setRange(0.0, 1.0);
    loadButton.setButtonText("LOAD");

    playButton.addListener(this);
    stopButton.addListener(this);
    volumeSlider.addListener(this);
    positionSlider.addListener(this);
    loadButton.addListener(this);
    
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(volumeSlider);
    addAndMakeVisible(positionSlider);
}

DeckGUI::~DeckGUI()
{
}

void DeckGUI::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
}

void DeckGUI::resized()
{
    float rowH = getHeight() / 5;
    playButton.setBounds(0, 0, getWidth(), rowH);
    stopButton.setBounds(0, rowH, getWidth(), rowH);
    volumeSlider.setBounds(0, rowH*2, getWidth(), rowH);
    positionSlider.setBounds(0, rowH*3, getWidth(), rowH);
    loadButton.setBounds(0, rowH*4, getWidth(), rowH);
}

void DeckGUI::buttonClicked(Button* button)
{
    if (button == &playButton ) 
    {
        djAudioPlayer->setPosition(0);
        djAudioPlayer->play();
    }
    if (button == &stopButton ) 
    {
        djAudioPlayer->stop();
    }
    if (button == &loadButton ) 
    {
        FileChooser chooser ("Select an audio file to play...");                                     
        if (chooser.browseForFileToOpen())                                  
        {
            URL audioURL = URL{chooser.getResult()};
            djAudioPlayer->load(audioURL);            
        }
    }
}

void DeckGUI::sliderValueChanged(Slider* slider)
{
    if (slider == &volumeSlider)
    {
        djAudioPlayer->setGain(slider->getValue());
    }
    if (slider == &positionSlider)
    {
        djAudioPlayer->setPositionRelative(slider->getValue());
    }
}

bool DeckGUI::isInterestedInFileDrag(const StringArray& files)
{
    std::cout << "DeckGUI::isInterestedInFileDrag" << std::endl; 
    return true; 
}

void DeckGUI::filesDropped(const StringArray& files, int x, int y)
{
    for (String filename : files)
    {
        std::cout << "DeckGUI::filesDropped " << filename << std::endl; 
        URL fileURL = URL{File{filename}};
        djAudioPlayer->load(fileURL);
        return; 
    }
}



