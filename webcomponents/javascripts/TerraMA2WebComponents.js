// Makes sure that jQuery has been loaded before terrama2.js
if(typeof jQuery === "undefined") {
  throw new Error("TerraMA² WebComponents requires jQuery");
}

// Makes sure that OpenLayers 3 has been loaded before terrama2.js
if(typeof ol === "undefined") {
  throw new Error("TerraMA² WebComponents requires Openlayers 3");
}

"use strict";

/**
 * Main class of the API.
 * @class TerraMA2WebComponents
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 */
requirejs(
  [
    'TerraMA2WC/components/MapDisplay.TerraMA2WebComponents',
    'TerraMA2WC/components/LayerExplorer.TerraMA2WebComponents'
  ],
  function(MapDisplay, LayerExplorer) {
    return (
      window.TerraMA2WebComponents = {
        MapDisplay: MapDisplay,
        LayerExplorer: LayerExplorer
      }
    );
  }
);
