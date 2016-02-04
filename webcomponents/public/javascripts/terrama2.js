//Make sure jQuery has been loaded before terrama2.js
if (typeof jQuery === "undefined") {
  throw new Error("TerraMA² WebComponents requires jQuery");
}

//Make sure OpenLayers 3 has been loaded before terrama2.js
if (typeof ol === "undefined") {
  throw new Error("TerraMA² WebComponents requires Openlayers 3");
}

"use strict";

/** @class TerraMA2 - Main class of the API. */
var TerraMA2 = function(terrama2Url, components) {

  var _this = this;

  var componentsLength = components.length;

  var config = null;
  var mapDisplay = null;
  var layerExplorer = null;
  var toolBox = null;
  var leftBar = null;

  /**
   * Return the length of the components array
   * @returns {number} componentsLength - length of the components array
   */
  _this.getComponentsLength = function() {
    return componentsLength;
  };

  /**
   * Return the Config object
   * @returns {Config} config - Config object
   */
  _this.getConfig = function() {
    return config;
  };

  /**
   * Return the MapDisplay object
   * @returns {MapDisplay} mapDisplay - MapDisplay object
   */
  _this.getMapDisplay = function() {
    return mapDisplay;
  }

  /**
   * Return the LayerExplorer object
   * @returns {LayerExplorer} layerExplorer - LayerExplorer object
   */
  _this.getLayerExplorer = function() {
    return layerExplorer;
  }

  /**
   * Return the url to the TerraMA² web API
   * @returns {string} terrama2Url - url to the TerraMA² web API
   */
  _this.getTerrama2Url = function() {
    return terrama2Url;
  }

  /**
   * Inject a stylesheet to the page
   * @param {string} url - url to the stylesheet
   */
  _this.injectStylesheet = function(url) {
    var link = $("<link>", { rel: "stylesheet", type: "text/css", href: url });

    link.appendTo('head');
  }

  /**
   * Verifies if a given file exist
   * @param {string} url - url to the file
   * @return {boolean}
   */
  _this.fileExists = function(url) {
    $.ajax({
      url: url,
      async: false,
      error: function() {
        return false;
      },
      success: function() {
        return true;
      }
    });
  }

  /**
   * Apply a given CQL filter to a given layer
   * @param {string} cql - CQL filter to be applied
   * @param {string} layerName - layer name to be filtered
   */
  _this.applyCQLFilter = function(cql, layerName) {
    mapDisplay.findBy(mapDisplay.getMap().getLayerGroup(), 'name', layerName).getSource().updateParams({ "CQL_FILTER": cql });
  }

  /**
   * Load the TerraMA² components present in the components array
   * @param {number} i - current array position
   */
  var loadComponents = function(i) {
    if(i < componentsLength) {
      $.ajax({
        url: terrama2Url + "/javascripts/components/" + config.getConfJsonComponentsJs()[components[i]],
        dataType: "script",
        success: function() {
          if(components[i] === "MapDisplay") {
            mapDisplay = new MapDisplay();
          } else if(components[i] === "LayerExplorer") {
            layerExplorer = new LayerExplorer(_this);
          } else if(components[i] === "ToolBox") {
            toolBox = new ToolBox(_this);
          }

          _this.injectStylesheet(terrama2Url + "/stylesheets/components/" + config.getConfJsonComponentsCss()[components[i]]);

          loadComponents(++i);
        }
      });
    } else {
      return;
    }
  }

  $.ajax({
    url: terrama2Url + "/javascripts/config.terrama2.js",
    dataType: "script",
    success: function() {
      config = new Config(_this);
      config.loadConfigurations();

      loadComponents(0);

      $.ajax({ url: terrama2Url + "/socket.io/socket.io.js", dataType: "script" });
    }
  });
}
