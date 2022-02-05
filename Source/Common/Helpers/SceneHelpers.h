/*
  ==============================================================================

    SceneHelpers.h
    Created: 9 Oct 2020 10:10:15am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class SceneHelpers
{
public:
    static var getParamsSceneData(BaseItem* i, Array<Parameter *> excludeParams = Array<Parameter *>());
    static void lerpSceneParams(BaseItem* i, var startData, var endData, float weight);
    static void lerpSceneParam(Parameter * p, var startData, var endData, float weight);
};