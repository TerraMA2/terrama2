//Make sure jQuery has been loaded before terrama2.js
if (typeof jQuery === "undefined") {
  throw new Error("TerraMA² WebComponents requires jQuery");
}

//Make sure OpenLayers 3 has been loaded before terrama2.js
if (typeof ol === "undefined") {
  throw new Error("TerraMA² WebComponents requires Openlayers 3");
}

"use strict";

window.TerraMA2WebComponents = {
  webcomponents: {}
};

/** @class TerraMA2WebComponents - Main class of the API. */
TerraMA2WebComponents.obj = (function() {
  var components = null;
  var componentsLength = null;
  var terrama2Url = null;

  /**
   * Return the length of the components array
   * @returns {number} componentsLength - length of the components array
   */
  var getComponentsLength = function() {
    return componentsLength;
  };

  /**
   * Return the url to the TerraMA² web API
   * @returns {string} terrama2Url - url to the TerraMA² web API
   */
  var getTerrama2Url = function() {
    return terrama2Url;
  };

  /**
   * Inject a stylesheet to the page
   * @param {string} url - url to the stylesheet
   */
  var injectStylesheet = function(url) {
    var link = $("<link>", { rel: "stylesheet", type: "text/css", href: url });

    link.appendTo('head');
  };

  /**
   * Verifies if a given file exist
   * @param {string} url - url to the file
   * @return {boolean}
   */
  var fileExists = function(url) {
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
  };

  /**
   * Apply a given CQL filter to a given layer
   * @param {string} cql - CQL filter to be applied
   * @param {string} layerName - layer name to be filtered
   */
  var applyCQLFilter = function(cql, layerName) {
    TerraMA2WebComponents.webcomponents.MapDisplay.findBy(TerraMA2WebComponents.webcomponents.MapDisplay.getMap().getLayerGroup(), 'name', layerName).getSource().updateParams({ "CQL_FILTER": cql });
  };

  /**
   * Load the TerraMA² components present in the components array
   * @param {number} i - current array position
   */
  var loadComponents = function(i) {
    if(i < componentsLength) {
      $.ajax({
        url: terrama2Url + "/javascripts/components/" + TerraMA2WebComponents.Config.getConfJsonComponentsJs()[components[i]],
        dataType: "script",
        success: function() {
          TerraMA2WebComponents.webcomponents[components[i]].init();
          injectStylesheet(terrama2Url + "/stylesheets/components/" + TerraMA2WebComponents.Config.getConfJsonComponentsCss()[components[i]]);
          loadComponents(++i);
        }
      });
    } else {
      return;
    }
  };

  var init = function(_terrama2Url, _components) {
    components = _components;
    componentsLength = components.length;
    terrama2Url = _terrama2Url;

    $.ajax({
      url: terrama2Url + "/javascripts/config.terrama2.js",
      dataType: "script",
      success: function() {
        TerraMA2WebComponents.Config.init();
        loadComponents(0);
        $.ajax({ url: terrama2Url + "/socket.io/socket.io.js", dataType: "script" });
      }
    });
  };

  return {
  	getComponentsLength: getComponentsLength,
  	getTerrama2Url: getTerrama2Url,
  	injectStylesheet: injectStylesheet,
  	fileExists: fileExists,
  	applyCQLFilter: applyCQLFilter,
  	init: init
  };
})();
