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
define(function() {

  /**
   * Initializes the necessary features.
   *
   * @function init
   * @memberof TerraMA2WebComponents
   * @inner
   */
  var init = function(terrama2Url) {};

  return {
  	init: init
  };
});
