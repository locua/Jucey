#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    : state(Stopped),
      thumbnailCache (5),                            // [4]
      thumbnail (512, formatManager, thumbnailCache)
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

    formatManager.registerBasicFormats();

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
    // addAndMakeVisible()

    getLookAndFeel().setColour(juce::Slider::textBoxTextColourId, juce::Colours::palevioletred);

    volumeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    // track1mix.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    track2mix.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    speedSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);

    playButton.addListener(this);
    playButton.setButtonText ("play");
    playButton.setColour (juce::TextButton::buttonColourId, juce::Colours::green);
    playButton.setEnabled(false);

    stopButton.addListener(this);
    stopButton.setButtonText("Stop");
    stopButton.setColour (juce::TextButton::buttonColourId, juce::Colours::red);
    stopButton.setEnabled (false);

    loadButton.addListener(this);
    loadButton.setButtonText("Open...");

    volumeSlider.addListener(this);
    track2mix.addListener(this);
    track1mix.addListener(this);
    speedSlider.addListener(this);
    thumbnail.addChangeListener(this);

    transportSource.addChangeListener(this);

    volumeSlider.setRange(0.0, 1.0);
    speedSlider.setRange(0.5, 10.0);

    // list basic formats
    for (int i=0;i<formatManager.getNumKnownFormats(); i++){
        std::string s = formatManager.getKnownFormat(i)->getFormatName().toStdString();
        std::cout << i << " " << s << std::endl;
    }

    audioVis.setSamplesPerBlock(16);

    // changeState(Stopped);

    std::cout << state << std::endl;

    // start timer
    startTimer(40);
    
        
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

    // resampleSource.prepareToPlay(
    //     samplesPerBlockExpected, 
    //     sampleRate);

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

    transportSource.getNextAudioBlock (bufferToFill);
    // resampleSource.getNextAudioBlock (bufferToFill);
    // audioVis.pushBuffer(bufferToFill);
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

    double rowH = getHeight()/5;
    juce::Rectangle<int> thumbnailBounds(0, rowH*2, getWidth(), rowH);

    if(thumbnail.getNumChannels()==0)
        paintIfNoFileLoaded(g, thumbnailBounds);
    else
        paintIfFileLoaded(g, thumbnailBounds);

}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    double rowH = getHeight()/5;
    double colW = getWidth()/2;
    double srowH = rowH/5;
    

    playButton.setBounds(0,0,getWidth(), srowH);
    stopButton.setBounds(0, srowH, getWidth(), srowH);
    loadButton.setBounds(0, srowH*2, getWidth(), srowH);

    volumeSlider.setBounds(0,rowH, colW, rowH);
    // audioVis.setBounds(0, rowH*2, getWidth(), rowH);
    track1mix.setBounds(0, rowH*3, colW, rowH);
    track2mix.setBounds(colW, rowH*3, colW, rowH);
    speedSlider.setBounds(colW, rowH, colW, rowH);

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
        // transportSource.start();
        std::cout << state << std::endl;

        if ((state == Stopped) || (state == Paused))
        {
            changeState (Starting);
            
        }
        else if (state == Playing){
            changeState (Pausing);
            std::cout << state << std::endl;
        }

    }
    if(button==&stopButton)
    {
        std::cout << "stop button clickeed" << std::endl;
        // transportSource.stop();
        if (state == Paused)
            changeState (Stopped);
        else
            changeState (Stopping);
    }
    if(button==&loadButton)
    {
        // std::cout << "load button clickeed" << std::endl;
        FileChooser chooser{"Select a file"};

        if(chooser.browseForFileToOpen())
        {
            loadURL(URL{chooser.getResult()}, chooser.getResult());
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
        // std::cout << "track1 slider " << transportSource.getCurrentPosition() << std::endl;
        transportSource.setPosition(slider->getValue());

    }
    if(slider==&track2mix)
    {
        std::cout << "track2 slider " << slider->getValue() << std::endl;
    }
    if(slider==&speedSlider)
    {
        // std::cout << "speed slider " << slider->getValue() << std::endl;
        resampleSource.setResamplingRatio(slider->getValue());
    }

}

void MainComponent::loadURL(URL audioURL, File file)
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
        thumbnail.setSource (new juce::FileInputSource (file));
        readerSource.reset (newSource.release());
        // set range of track 1 mixer in between 0 and length
        track1mix.setRange(0, transportSource.getLengthInSeconds());

        playButton.setEnabled(true);
    } 
    else
    {
        std::cout << "there was a problem loading the file" << std::endl;
    }
}

void MainComponent::changeListenerCallback (juce::ChangeBroadcaster* source) 
{
    if (source == &transportSource) 
    {
        if(transportSource.isPlaying()){
            std::cout << "im playingnow" << std::endl;
            changeState(Playing);
        }
        else if ((state == Stopping) || (state == Playing))
            changeState (Stopped);
        else if (Pausing == state)
            changeState (Paused); 
    }
    if (source == &thumbnail)       thumbnailChanged();
}

void MainComponent::paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour (juce::Colours::darkgrey);
    g.fillRect (thumbnailBounds);
    g.setColour (juce::Colours::white);
    g.drawFittedText ("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);

    
}


 void MainComponent::paintIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
 {
    g.setColour (juce::Colours::white);
    g.fillRect (thumbnailBounds);
    g.setColour (juce::Colours::red);                               // [8]
    auto aLen = (float) thumbnail.getTotalLength();
    thumbnail.drawChannels (g,                                      // [9]
                                thumbnailBounds,
                                0.0,                                    // start time
                                aLen,             // end time
                                1.0f);                                  // vertical zoom
    
    g.setColour(juce::Colours::green);
    auto audioPosition = (float) transportSource.getCurrentPosition();
    auto drawPosition = (audioPosition / aLen * (float) thumbnailBounds.getWidth()
                        + (float) thumbnailBounds.getX());
    g.drawLine (drawPosition, (float)thumbnailBounds.getY(), drawPosition,
                (float) thumbnailBounds.getBottom(), 2.0f);
    
}

void MainComponent::timerCallback() 
{
    repaint();
}