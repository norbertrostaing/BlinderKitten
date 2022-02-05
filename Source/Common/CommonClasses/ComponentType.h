/*
  ==============================================================================

    ComponentType.h
    Created: 9 Sep 2021 9:29:10pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

enum ComponentType { INTENSITY, COLOR, GOBO, LASER, PANTILT, SERVO, STEPPER, STROBE, SCRIPT, CUSTOM, TYPES_MAX };
const String componentTypeNames[TYPES_MAX] { "Intensity", "Color", "Gobo", "Laser", "PanTilt", "Servo", "Stepper", "Strobe", "Script", "Custom" };
