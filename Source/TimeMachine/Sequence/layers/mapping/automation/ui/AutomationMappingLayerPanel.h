/*
  ==============================================================================

    AutomationMappingLayerPanel.h
    Created: 25 Mar 2020 1:46:10pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../../ui/MappingLayerPanel.h"
#include "../AutomationMappingLayer.h"

class AutomationMappingLayerPanel :
	public MappingLayerPanel
{
public:
	AutomationMappingLayerPanel(AutomationMappingLayer* layer);
	virtual ~AutomationMappingLayerPanel();

	AutomationMappingLayer* aml;
};