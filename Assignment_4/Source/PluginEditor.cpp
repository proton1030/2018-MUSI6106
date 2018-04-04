/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>


//==============================================================================
VibratoPluginAudioProcessorEditor::VibratoPluginAudioProcessorEditor (VibratoPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    addAndMakeVisible(pSliderVibratoWidth = new Slider ("Width"));
    pSliderVibratoWidth->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    pSliderVibratoWidth->setTextBoxStyle (Slider::TextBoxBelow, true, 100, 20);
    pSliderVibratoWidth->setSkewFactorFromMidPoint(0.1);
    pSliderVibratoWidth->setColour(Slider::textBoxOutlineColourId, Colours::darkgrey);
    pSliderVibratoWidth->addListener(this);
    
    addAndMakeVisible(pSliderVibratoFreq = new Slider ("Freq"));
    pSliderVibratoFreq->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    pSliderVibratoFreq->setTextBoxStyle (Slider::TextBoxBelow, true, 100, 20);
    pSliderVibratoWidth->setSkewFactorFromMidPoint(50.0);
    pSliderVibratoFreq->setColour(Slider::textBoxOutlineColourId, Colours::darkgrey);
    pSliderVibratoFreq->addListener(this);
    
    addAndMakeVisible (pLabelVibratoWidth = new Label ("widthInSecLabel", TRANS("Width In Seconds")));
    pLabelVibratoWidth->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    pLabelVibratoWidth->setJustificationType (Justification::centredLeft);
    pLabelVibratoWidth->setEditable (false, false, false);
    pLabelVibratoWidth->setColour (TextEditor::textColourId, Colours::black);
    pLabelVibratoWidth->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    
    addAndMakeVisible (pLabelVibratoFreq = new Label ("freqInHzLabel", TRANS("Frequency In Hertz")));
    pLabelVibratoFreq ->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    pLabelVibratoFreq ->setJustificationType (Justification::centredLeft);
    pLabelVibratoFreq ->setEditable (false, false, false);
    pLabelVibratoFreq ->setColour (TextEditor::textColourId, Colours::black);
    pLabelVibratoFreq ->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    
    addAndMakeVisible (pButtonBypass = new TextButton ("Bypass"));
    pButtonBypass->addListener (this);
    
    pWidthAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.treeState, WIDTH_ID, *pSliderVibratoWidth);
    pFreqAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.treeState, FREQ_ID, *pSliderVibratoFreq);
    
    setSize (400, 300);
}

VibratoPluginAudioProcessorEditor::~VibratoPluginAudioProcessorEditor()
{
    pSliderVibratoFreq = nullptr;
    pSliderVibratoWidth = nullptr;
    pButtonBypass = nullptr;
}

//==============================================================================
void VibratoPluginAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::darkgrey);

//    g.setColour (Colours::black);
    g.setFont (15.0f);
    

}

void VibratoPluginAudioProcessorEditor::resized()
{
    pLabelVibratoWidth->setBounds (proportionOfWidth (0.1000f), proportionOfHeight (0.1000f), proportionOfWidth (0.500f), proportionOfHeight (0.3300f));
    pLabelVibratoFreq->setBounds (proportionOfWidth (0.5700f), proportionOfHeight (0.1000f), proportionOfWidth (0.5800f), proportionOfHeight (0.3300f));
    pSliderVibratoWidth->setBounds (proportionOfWidth (0.0000f), proportionOfHeight (0.3500f), proportionOfWidth (0.500f), proportionOfHeight (0.5000f));
    pSliderVibratoFreq->setBounds (proportionOfWidth (0.4500f), proportionOfHeight (0.3500f), proportionOfWidth (0.5800f), proportionOfHeight (0.5000f));
    pButtonBypass->setBounds(proportionOfWidth (0.800f), proportionOfHeight (0.0500f), proportionOfWidth (0.1500f), proportionOfHeight (0.100f));
    pSliderVibratoWidth->setValue(processor.pVibratoInstance->getParam(CVibrato::kParamModWidthInS));
    pSliderVibratoFreq->setValue(processor.pVibratoInstance->getParam(CVibrato::kParamModFreqInHz));
    pButtonBypass->setColour(TextButton::ColourIds::buttonColourId, Colours::darkgrey);
    pButtonBypass->setColour(TextButton::ColourIds::buttonOnColourId, Colours::indianred);
    pButtonBypass->setToggleState(processor.pVibratoInstance->getBypassState(),  juce::NotificationType::dontSendNotification);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void VibratoPluginAudioProcessorEditor::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == pButtonBypass)
    {
        bool bypassstate = processor.pVibratoInstance->getBypassState();
        processor.pVibratoInstance->setBypassState(!bypassstate);
        pButtonBypass->setToggleState(!bypassstate, juce::NotificationType::dontSendNotification);
    }
}

void VibratoPluginAudioProcessorEditor::buttonStateChanged(Button* buttonThatWasClicked)
{
    
}

void VibratoPluginAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    if (slider == pSliderVibratoWidth)
    {
        processor.pVibratoInstance->setParam(CVibrato::kParamModWidthInS, float(pSliderVibratoWidth->getValue()));
    }
    else if (slider == pSliderVibratoFreq)
    {
        processor.pVibratoInstance->setParam(CVibrato::kParamModFreqInHz, float(pSliderVibratoFreq->getValue()));
    }
}
