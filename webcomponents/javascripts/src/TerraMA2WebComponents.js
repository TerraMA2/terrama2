// Makes sure that jQuery has been loaded before terrama2.js
if(typeof jQuery === "undefined") {
  throw new Error("TerraMA2WebComponents requires jQuery");
}

// Makes sure that OpenLayers 3 has been loaded before terrama2.js
if(typeof ol === "undefined") {
  throw new Error("TerraMA2WebComponents requires Openlayers 3");
}

"use strict";

/**
 * Main class of the API.
 * @class TerraMA2WebComponents
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 */
define(
  "TerraMA2WebComponents",
  [
    'TerraMA2WebComponentsPath/components/MapDisplay.TerraMA2WebComponents',
    'TerraMA2WebComponentsPath/components/LayerExplorer.TerraMA2WebComponents'
  ],
  function(MapDisplay, LayerExplorer) {
    return {
      MapDisplay: MapDisplay,
      LayerExplorer: LayerExplorer
    };
  }
);
