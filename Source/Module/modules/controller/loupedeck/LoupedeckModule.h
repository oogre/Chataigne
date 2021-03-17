/*
  ==============================================================================

    LoupedeckModule.h
    Created: 16 Mar 2021 6:38:55pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class LoupedeckModule :
    public WebSocketClientModule
{
public:
    LoupedeckModule();
    ~LoupedeckModule();

    enum LDCommand
    {
        BacklightLevel = 0x0409,
        Vibrate =  0x04B1,
        ButtonColor = 0x0702,
        ScreenImage = 0xFF10,
        RefreshScreen = 0x050F
    };

    EnumParameter* model; //not used right now

    //params
    FloatParameter * brightness;
    FloatParameter* knobSensitivity;

    ControllableContainer buttonParamsCC;
    Array<ColorParameter*> buttonColors;

    ControllableContainer sliderParamsCC;
    Array<ColorParameter*> sliderColors;
    Array<FileParameter*> sliderImages;
    Array<StringParameter*> sliderTexts;

    ControllableContainer padParamsCC;
    Array<ColorParameter*> padColors;
    Array<FileParameter*> padImages;
    Array<StringParameter*> padTexts;


    //values
    ControllableContainer buttonsCC;
    Array<BoolParameter*> buttons;

    ControllableContainer knobsCC;
    Array<BoolParameter*> knobButtons;
    Array<FloatParameter*> knobAbsolutes;
    Array<FloatParameter*> knobRelatives;

    ControllableContainer touchScreenCC;
    
    Point2DParameter* touchPosition;
    Point<int> posOnTouch;

    HashMap<int, int> touchPadMap;

    ControllableContainer slidersCC;
    Array<BoolParameter*> sliderTouches;
    Array<FloatParameter*> sliderAbsolutes;

    ControllableContainer padsCC;
    Array<BoolParameter*> pads;
    Array<Point2DParameter*> padsTouchPositions;

    /*
    const padCoords = [
        [[0, 0], [60, 255]],
        [[420, 0], [460, 255]],
        [[70, 0], [140, 70]]
    ];
    */

    struct LDScreen
    {
        short id;
        int width;
        int height;


    };
    Array<LDScreen> screens;

    void dataReceived(const MemoryBlock& data) override;
    void processTouchData(Array<uint8_t> data);
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;

    void sendLoupedeckCommand(LDCommand command, Array<uint8> data);


    void updatePadContent(int padID);

    int getPadIDForPos(Point<int> pos);
    Rectangle<int> getPadCoords(int buttonID);

    //values
    String getLoupedeckServerPath() const;
    virtual void timerCallback() override;


    static LoupedeckModule* create() { return new LoupedeckModule(); }
    virtual String getDefaultTypeString() const override { return "Loupedeck"; }
};