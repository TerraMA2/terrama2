"use strict";

define(
  [],
  function() {
    /**
     * Method to get Layers from map capabilities
     */
    var getMapCapabilitiesLayers = function(capabilities) {
      var layers = capabilities.Capability[0].Layer[0];

      var capabilitiesList = [];

      var layersLength = layers.Layer.length;
      for (var i = 0; i < layersLength; i++) {
        if (layers.Layer[i].hasOwnProperty('Layer')) {
          var subLayersLength = layers.Layer[i].Layer.length;
          for (var j = 0; j < subLayersLength; j++) {
            var extent = undefined;
            if (layers.Layer[i].Layer[j].hasOwnProperty('Extent')){
              var stringextent = layers.Layer[i].Layer[j].Extent[0]._;
              if (stringextent.includes('/')){
                var startDate = stringextent.split('/')[0];
                var endDate = stringextent.split('/')[1];
                if (startDate != endDate){
                  extent = {
                    startDate: startDate,
                    endDate: endDate
                  }
                }
              }
              else {
                extent = stringextent.split(',');
              }
            }
            capabilitiesList.push({
              name: layers.Layer[i].Layer[j].Name[0],
              title: layers.Layer[i].Layer[j].Title[0],
              extent: extent
            })
          }
        } else {
          var extent = undefined;
          if (layers.Layer[i].hasOwnProperty('Extent')){
            var stringextent = layers.Layer[i].Extent[0]._;
            if (stringextent.includes('/')){
              var startDate = stringextent.split('/')[0];
              var endDate = stringextent.split('/')[1];
              if (startDate != endDate){
                extent = {
                  startDate: startDate,
                  endDate: endDate
                }
              }
            }
            else {
              extent = stringextent.split(',');
            }
          }
          capabilitiesList.push({
            name: layers.Layer[i].Name[0],
            title: layers.Layer[i].Title[0],
            extent: extent
          })
        }
      }
      return capabilitiesList;
    };

    return {
      getMapCapabilitiesLayers: getMapCapabilitiesLayers
    };
  }
);