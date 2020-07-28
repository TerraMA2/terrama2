"use strict";

define(
  [],
  function() {

    var getDateExtentFromLayer = function(layer) {
      var extent = undefined;
      var boundingBox = undefined;

      if(layer.hasOwnProperty('Extent')) {
        var stringextent = layer.Extent[0]._;

        if (stringextent) {
          if(stringextent.includes('/')) {
            var startDate = stringextent.split('/')[0];
            var endDate = stringextent.split('/')[1];

            if(startDate != endDate) {
              extent = {
                startDate: startDate,
                endDate: endDate
              };
            }
          } else {
            extent = stringextent.split(',');
          }
        }

      }

      if(layer.hasOwnProperty('LatLonBoundingBox')) {
        boundingBox = [Number(layer.LatLonBoundingBox[0].$.minx), Number(layer.LatLonBoundingBox[0].$.miny), Number(layer.LatLonBoundingBox[0].$.maxx), Number(layer.LatLonBoundingBox[0].$.maxy)];
      }

      return {
        name: layer.Name[0],
        title: layer.Title[0],
        extent: extent,
        boundingBox: boundingBox
      };
    };

    /**
     * Method to get Layers from map capabilities
     */
    var getMapCapabilitiesLayers = function(capabilities) {
      var capabilitiesList = [];

      if(capabilities.Capability && capabilities.Capability[0] && capabilities.Capability[0].Layer && capabilities.Capability[0].Layer[0]) {
        var layers = capabilities.Capability[0].Layer[0];


        if(layers.Layer !== undefined) {
          for(var i = 0, layersLength = layers.Layer.length; i < layersLength; i++) {
            if(layers.Layer[i].hasOwnProperty('Layer')) {
              for(var j = 0, subLayersLength = layers.Layer[i].Layer.length; j < subLayersLength; j++) {
                capabilitiesList.push(getDateExtentFromLayer(layers.Layer[i].Layer[j]));
              }
            } else {
              capabilitiesList.push(getDateExtentFromLayer(layers.Layer[i]));
            }
          }
        }
      }

      return capabilitiesList;
    };

    return {
      getMapCapabilitiesLayers: getMapCapabilitiesLayers
    };
  }
);