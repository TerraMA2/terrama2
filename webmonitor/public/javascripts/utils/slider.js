'use strict';

/**
 * Triggered when user clicks on TerraMA² Slider in Layers Menu
 * 
 * @param {Event}
 */
$("#terrama2-layerexplorer").on("click", "#terrama2-slider", function(event) {
  var self = $(this);
  var parentLi = $(self).parent().parent();
  var parentId = $(parentLi).attr("data-layerid");
  var capability = capabilities.find(function(capability) { return capability.name === parentId; });

  if (!capability) {
    console.log("Capability not found...");
    return;
  }
  if (!capability.extent instanceof Array) {
    console.log("Capability has not extent array.");
    return;
  }

  setSlider(capability.extent, parentId);
});