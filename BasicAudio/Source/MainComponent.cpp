// Music 256a / CS 476a | fall 2016
// CCRMA, Stanford University
//
// Author: Romain Michon (rmichonATccrmaDOTstanfordDOTedu)
// Description: Simple JUCE sine wave synthesizer

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Sine.h"
#define oscNum 8
#define keyNum 8

class MainContentComponent :

    public AudioAppComponent,
    private Slider::Listener,
    private Button::Listener
{
public:
    MainContentComponent() :  masterGain(0.0), onOff (2), samplingRate(0.0), capStatus(0)
 
    {
        // Capping Indicator configuration here
        capIndicator.setButtonText("CLIPPED! Click to dismiss");
        capIndicator.addListener(this);
        
        
        // configuring frequency slider and adding it to the main window
        for (int i = 0; i < oscNum; i++) {
            addAndMakeVisible (frequencySlider[i]);
            frequencySlider[i].setRange (50.0, 18000.0);
            frequencySlider[i].setSkewFactorFromMidPoint (1500.0);
            frequencySlider[i].setValue(0); // will also set the default frequency of the sine osc
            frequencySlider[i].addListener (this);
        }
            frequencySlider[0].setValue(440);
            
        // configuring frequency label box and adding it to the main window
            frequencyLabel[0].setText("Fundamental", dontSendNotification);
            frequencyLabel[0].attachToComponent(&frequencySlider[0], true);
        for (int i = 1; i < oscNum; i++) {
            addAndMakeVisible(frequencyLabel[i]);
            frequencyLabel[i].setText ("Frequency", dontSendNotification);
            frequencyLabel[i].attachToComponent (&frequencySlider[i], true);
        }
        
        // configuring gain slider and adding it to the main window
        for (int i = 0; i < oscNum; i++) {
            addAndMakeVisible (gainSlider[i]);
            gainSlider[i].setRange (0.0, 1.0);
            gainSlider[i].setValue(0.0); // will alsi set the default gain of the sine osc
            gainSlider[i].addListener (this);
            localGain[i] = 0.0;
        }
        
        // MasterGain - VOLUME
        addAndMakeVisible(masterGainSlider);
        addAndMakeVisible(volumeLabel);
        
        masterGainSlider.setRange (0.0, 1.0);
        masterGainSlider.setValue(0.5); // will alsi set the default gain of the sine osc
        masterGainSlider.addListener (this);
        volumeLabel.setText("Volume", dontSendNotification);
        volumeLabel.attachToComponent(&masterGainSlider, true);
        
        
        
        // configuring gain label and adding it to the main window
        for (int i = 0; i < oscNum; i++) {
            addAndMakeVisible(gainLabel);
            gainLabel[i].setText ("Gain", dontSendNotification);
            gainLabel[i].attachToComponent (&gainSlider[i], true);
        }
        
        // configuring on/off button and adding it to the main window
        addAndMakeVisible(onOffButton);
        onOffButton.addListener(this);
             
        
        // configuring on/off label and adding it to the main window
        addAndMakeVisible(onOffLabel);
        onOffLabel.setText ("On/Off", dontSendNotification);
        onOffLabel.attachToComponent (&onOffButton, true);
        
        /***** Creating New Timbre PRESETS HERE ********/
        addAndMakeVisible(sinButton);
        addAndMakeVisible(triButton);
        addAndMakeVisible(sqrButton);
        addAndMakeVisible(sawButton);
        addAndMakeVisible(resetButton);
        
        sinButton.setButtonText("Sine Wave");
        triButton.setButtonText("Triangle Wave");
        sqrButton.setButtonText("Square Wave");
        sawButton.setButtonText("Sawtooth Wave");
        resetButton.setButtonText("Reset");


        sinButton.addListener(this);
        triButton.addListener(this);
        sqrButton.addListener(this);
        sawButton.addListener(this);
        resetButton.addListener(this);
        
        /************** Keys *******************/
        for (int i=0; i < keyNum; i++) {
            addAndMakeVisible(key[i]);
            key[i].addListener(this);
        }
        key[0].setButtonText("C4"); key[1].setButtonText("D4"); key[2].setButtonText("E4");
        key[3].setButtonText("F4"); key[4].setButtonText("G4"); key[5].setButtonText("A4");
        key[6].setButtonText("B4"); key[7].setButtonText("C5"); 
        
        
        setSize (600, 100*(oscNum-1));
        nChans = 1;
        setAudioChannels (0, nChans); // no inputs, one output
    }
    
    ~MainContentComponent()
    {
        shutdownAudio();
    }
    
    
    
    //***************************** Change Resized Info HERE **************************************//
    void resized() override
    {
        // placing the UI elements in the main window
        // getWidth has to be used in case the window is resized by the user
        const int sliderLeft = 80;
        int topBound = 10;
        onOffButton.setBounds (sliderLeft, topBound, 20, 20);
        capIndicator.setBounds(int(getWidth() /2), topBound, 200, 20);
        
        for (int i = 0; i < oscNum; i++) {
            frequencySlider[i].setBounds (sliderLeft, 40+60*i, getWidth() - sliderLeft, 20);
            gainSlider[i].setBounds (sliderLeft, 70+60*i, getWidth() - sliderLeft, 20);
        }
        sinButton.setBounds(sliderLeft, 100*(oscNum-1)-150, 90, 20);
        triButton.setBounds(sliderLeft+100, 100*(oscNum-1)-150, 90, 20);
        sqrButton.setBounds(sliderLeft+200, 100*(oscNum-1)-150, 90, 20);
        sawButton.setBounds(sliderLeft+300, 100*(oscNum-1)-150, 90, 20);
        resetButton.setBounds(sliderLeft, 100*(oscNum-1)-110, 90, 20);
        
        masterGainSlider.setBounds(sliderLeft, 100*(oscNum-1)-60, 200, 20);

        for (int i = 0; i < keyNum; i++) {
            key[i].setBounds(int(getWidth()/2)+30*i, 100*(oscNum-1)-110, 30, 90)
            ;
        }
        

    }
    
    
    /**************************** Slider Value Changed *********************************/
    void sliderValueChanged (Slider* slider) override
    {
        if (samplingRate > 0.0){
            for (int i = 0; i < oscNum; i++) {
                if (timbreSelection == 0 || timbreSelection == 1) { // Reset and Sine
                    if (slider == &frequencySlider[i]){
                        //sine[i].setFrequency(frequencySlider[i].getValue());
                    }
                    
                }
                else if (timbreSelection == 2 || timbreSelection == 3) { // Triangle and Square
                    if (slider == &frequencySlider[0]) {
                        frequencySlider[i].setValue(frequencySlider[0].getValue()*(2*i+1));
                    }
                }
                else {
                    if (slider == &frequencySlider[0]) {
                        frequencySlider[i].setValue(frequencySlider[0].getValue()*(i+1));
                    }
                }
                sine[i].setFrequency(frequencySlider[i].getValue());

                
                if (slider == &gainSlider[i]){
                    localGain[i] = gainSlider[i].getValue();
                }
            }
            if (slider == &masterGainSlider) {
                masterGain = masterGainSlider.getValue();
            }
            else {}

        }
    }
    
    /********** Button Function *************/
    void buttonClicked (Button* button) override
    {
        // turns ALL audio on or off
        if(button == &onOffButton && onOffButton.getToggleState()){
            onOff = 1;
        }
        else if (button == &onOffButton && !onOffButton.getToggleState() ) {
            onOff = 0;
        }
        // Dismiss Clipping Indicator
        else if (button == &capIndicator) {
            removeChildComponent(&capIndicator);
            capStatus = 0;
        }
        else if (button == &sinButton) { //SINE WAVE
            timbreSelection = 1;
            gainSlider[0].setValue(1.0);
        
            for (int i = 1; i < oscNum; i++) {
                frequencySlider[i].setValue(0.0);
                gainSlider[i].setValue(0.0);
            } 
        }
        else if (button == &triButton) { // TRIANGLE WAVE
            timbreSelection = 2;
            gainSlider[0].setValue(1.0);
            for (int i = 1; i < oscNum; i++) {
                frequencySlider[i].setValue(frequencySlider[0].getValue()*(2*i+1));
                gainSlider[i].setValue(gainSlider[0].getValue()/((2*i+1)*(2*i+1)));
            }
        }
        else if (button == &sqrButton) { // SQUARE WAVE
            timbreSelection = 3;
            gainSlider[0].setValue(1.0);
            for (int i = 1; i < oscNum; i++) {
                frequencySlider[i].setValue(frequencySlider[0].getValue()*(2*i+1));
                gainSlider[i].setValue(gainSlider[0].getValue()/ (2*(i+1)-1) );
            }
        }
        else if (button == &sawButton) { //SAWTOOTH WAVE
            timbreSelection = 4;
            gainSlider[0].setValue(0.8);
            for (int i = 1; i < oscNum; i++) {
                frequencySlider[i].setValue(frequencySlider[0].getValue()*(i+1));
                gainSlider[i].setValue(gainSlider[0].getValue()/(i+1));
            }
        }
        else if (button == &resetButton) { //RESET
            timbreSelection = 0;
            for (int i=0; i < oscNum; i++) {
                frequencySlider[i].setValue(0);
                gainSlider[i].setValue(0.0);
            }
            frequencySlider[0].setValue(440);
        }
        else {
            if (button == &key[0])                  //C4
                frequencySlider[0].setValue(261);
            else if (button == &key[1])             //D4
                frequencySlider[0].setValue(294);
            else if (button == &key[2])             //E4
                frequencySlider[0].setValue(329);
            else if (button == &key[3])             //F4
                frequencySlider[0].setValue(349);
            else if (button == &key[4])             //G4
                frequencySlider[0].setValue(392);
            else if (button == &key[5])             //A4
                frequencySlider[0].setValue(440);
            else if (button == &key[6])             //B4
                frequencySlider[0].setValue(494);
            else                                    //C5
                frequencySlider[0].setValue(523);
    
        }
            
        
    }
    
    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        samplingRate = sampleRate;
        for (int i = 0; i < oscNum; i++) {
            sine[i].setSamplingRate(sampleRate);
        }
    }
    
    void releaseResources() override
    {
    }
    
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        // getting the audio output buffer to be filled
        float* const buffer = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
        
        // computing one block
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            if(onOff == 1) {
                float temp = sine[0].tick() * localGain[0];
                for (int i = 1;i < oscNum; i++) {
                    if (timbreSelection == 0 || timbreSelection == 1 || timbreSelection == 3) //for Reset/Sin/Sqr
                        temp += sine[i].tick() * localGain[i];
                    else { //for Tri and Saw ---->>> the nature of these: + - + - + - ...
                        if (i % 2 == 1)
                            temp -= sine[i].tick()*localGain[i];
                        else
                            temp += sine[i].tick() * localGain[i];
                    }
                }
                if (timbreSelection == 4) 
                    temp = -temp;  // transform from reversed sawtooth to sawtooth
                    
                temp *= masterGain; //apply volume
                
                // SOFTWARE LIMITER: Hard Clip if sample > 1.0 or < -1.0;
                    if (temp >= 1.00) {
                        temp = 0.99;
                        if (!capStatus) {
                            addAndMakeVisible(capIndicator);
                            capStatus = !capStatus;
                        }
                        
                    }
                    if (temp <= -1.0) {
                        temp = -0.99;
                        if (!capStatus) {
                            addAndMakeVisible(capIndicator);
                            capStatus = !capStatus;
                        }
                    }
                        
                buffer[sample] = temp; //pass processed sample to buffer
                
            }
            else buffer[sample] = 0.0;
        }
    }
    
    
private:

    // UI Elements
    Slider frequencySlider[oscNum];
    Slider gainSlider[oscNum], masterGainSlider;
    ToggleButton onOffButton;
    TextButton capIndicator;
    TextButton sinButton, triButton, sqrButton, sawButton, resetButton;
    TextButton key[keyNum];
    
        
    Label frequencyLabel[oscNum], gainLabel[oscNum], onOffLabel, localOnOffLabel[oscNum], volumeLabel;
    
    Sine sine[oscNum]; // the sine wave oscillator
    
    // Global Variables
    float masterGain, localGain[oscNum];
    int onOff, samplingRate, nChans, capStatus;
    int timbreSelection = 0; // 1 = sin, 2 = tri, 3 = sqr, 4 = saw

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
