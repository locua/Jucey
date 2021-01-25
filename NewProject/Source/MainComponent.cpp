#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
    }

    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);

    getLookAndFeel().setColour(juce::Slider::textBoxTextColourId, juce::Colours::red);
    
    // addAndMakeVisible(toolBar1);
    // addAndMakeVisible(browser);

    addAndMakeVisible(audioVis);

    // Sliders
    addAndMakeVisible(volumeSlider);
    addAndMakeVisible(track1mix);
    addAndMakeVisible(track2mix);
    addAndMakeVisible(speedSlider);

    getLookAndFeel().setColour(juce::Slider::textBoxTextColourId, juce::Colours::red);

    volumeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    track1mix.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    track2mix.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    speedSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);

    // listeners
    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    volumeSlider.addListener(this);
    track2mix.addListener(this);
    track1mix.addListener(this);
    speedSlider.addListener(this);

    formatManager.registerBasicFormats();

    volumeSlider.setRange(0.0, 1.0);

    // list basic formats
    for (int i=0;i<formatManager.getNumKnownFormats(); i++){
        std::string s = formatManager.getKnownFormat(i)->getFormatName().toStdString();
        std::cout << i << " " << s << std::endl;
    }
        
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    phase = 0.0;
    dphase = 0.01;
    
    transportSource.prepareToPlay(
        samplesPerBlockExpected, 
        sampleRate);

    resampleSource.prepareToPlay(
        samplesPerBlockExpected, 
        sampleRate);

    // URL audioURL{"file:///home/louca/Desktop/Weeping.wav"};
    // AudioFormatReader* reader = formatManager.createReaderFor (audioURL.createInputStream (false));

    // if (reader != nullptr)
    // {
    //     std::unique_ptr<AudioFormatReaderSource> newSource (new AudioFormatReaderSource (reader, true));
    //     transportSource.setSource ( newSource.get(), 0, nullptr, reader->sampleRate);
    //     readerSource.reset (newSource.release());
    // }
    // else
    // {
    //     std::cout << "Something went wrong loading the file " << std::endl;
    // }
}


void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)

    // bufferToFill.clearActiveBufferRegion();

    /* BASIC SYNTHESIS */

    // auto* leftChan  = bufferToFill.buffer->getWritePointer(0,bufferToFill.startSample);
    // auto* rightChan = bufferToFill.buffer->getWritePointer(1,bufferToFill.startSample);

    // for(auto i=0; i < bufferToFill.numSamples; ++i)
    // {
    //     // double sample = rand.nextDouble() * 0.25; // random
    //     // double sample = fmod(phase, 0.2); // saw tooth
    //     double sample = sin(phase)*0.1; // sine wave

    //     leftChan[i] = sample;
    //     rightChan[i] = sample;

    //     phase+=dphase;
    // }

    /* Playing a file */

    // transportSource.getNextAudioBlock (bufferToFill);
    resampleSource.getNextAudioBlock (bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    transportSource.releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!

    // g.fillAll(juce::Colours::azure);

    
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    double rowH = getHeight()/5;
    double colW = getWidth()/2;

    playButton.setBounds(0,0,colW, rowH);
    volumeSlider.setBounds(0,rowH, getWidth(), rowH);
    audioVis.setBounds(0, rowH*2, getWidth(), rowH);
    track1mix.setBounds(0, rowH*3, colW, rowH);
    track2mix.setBounds(colW, rowH*3, colW, rowH);
    stopButton.setBounds(colW, 0, colW, rowH);
    loadButton.setBounds(0, rowH*4, colW, rowH);
    speedSlider.setBounds(colW, rowH*4, colW, rowH);

    // disk1
    //chooser1.
    // toolBar1.setBounds(0, rowH*2, getWidth(), rowH);

    // browser.setBounds(0, rowH*3, getWidth(),1000);
    // browser.goToURL("https://duckduckgo.com");
    
}


void MainComponent::buttonClicked(Button* button)
{
    if (button==&playButton)
    {
        std::cout << "play button was clicked" << std::endl;
        // playing=true;
        // dphase=0;
        // transportSource.setPosition(0);
        transportSource.start();
    }
    if(button==&stopButton)
    {
        std::cout << "stop button clickeed" << std::endl;
        transportSource.stop();
    }
    if(button==&loadButton)
    {
        // std::cout << "load button clickeed" << std::endl;
        FileChooser chooser{"Select a file"};

        if(chooser.browseForFileToOpen())
        {
            loadURL(URL{chooser.getResult()});
        }
    }
}

void MainComponent::sliderValueChanged (Slider *slider)
{
    if(slider==&volumeSlider)
    {
        // std::cout << "vol slider " << slider->getValue() << std::endl;
        // dphase = volumeSlider.getValue()*0.01; // modulation for synthesis
        transportSource.setGain(slider->getValue());
    }
    if(slider==&track1mix)
    {
        std::cout << "track1 slider " << slider->getValue() << std::endl;
    }
    if(slider==&track2mix)
    {
        std::cout << "track1 slider " << slider->getValue() << std::endl;
    }
    if(slider==&speedSlider)
    {
        // std::cout << "speed slider " << slider->getValue() << std::endl;
        resampleSource.setResamplingRatio(slider->getValue());
    }

}

void MainComponent::loadURL(URL audioURL)
{  
    std::cout << audioURL.toString(false) << std::endl;

    auto* reader = formatManager.createReaderFor (audioURL.createInputStream (false));
    if(reader!=nullptr)
    {
        // Exception safe local scope smart pointer variable
        std::unique_ptr<AudioFormatReaderSource> newSource (new AudioFormatReaderSource (reader, true));
        // Object level scope source 
        transportSource.setSource ( newSource.get(), 0, nullptr, reader->sampleRate);
        // Handing over object to readerSource
        readerSource.reset (newSource.release());
    } 
    else
    {
        std::cout << "there was a problem loading the file" << std::endl;
    }
}