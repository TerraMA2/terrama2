// Makes sure that jQuery has been loaded before terrama2.js
if(typeof jQuery === "undefined") {
  throw new Error("TerraMA² WebComponents requires jQuery");
}

// Makes sure that OpenLayers 3 has been loaded before terrama2.js
if(typeof ol === "undefined") {
  throw new Error("TerraMA² WebComponents requires Openlayers 3");
}

"use strict";

window.TerraMA2WebComponents = {
  webcomponents: {}
};

/**
 * Main class of the API.
 * @module TerraMA2WebComponents
 *
 * @property {array} components - Array of components names received on the initialization.
 * @property {int} componentsLength - Length of the components names array.
 * @property {boolean} componentsLoaded - Flag that indicates if all the components have been loaded.
 * @property {string} terrama2Url - TerraMA² WebComponents URL.
 */
TerraMA2WebComponents.obj = (function() {

  // Array of components names received on the initialization
  var components = null;
  // Length of the components names array
  var componentsLength = null;
  // Flag that indicates if all the components have been loaded
  var componentsLoaded = false;
  // TerraMA² WebComponents URL
  var terrama2Url = null;

  /**
   * Returns the length of the components names array.
   * @returns {int} componentsLength - Length of the components names array
   *
   * @function getComponentsLength
   */
  var getComponentsLength = function() {
    return componentsLength;
  };

  /**
   * Returns the URL of the TerraMA² WebComponents.
   * @returns {string} terrama2Url - URL of the TerraMA² WebComponents
   *
   * @function getComponentsLength
   */
  var getTerrama2Url = function() {
    return terrama2Url;
  };

  /**
   * Verifies if a given file exists.
   * @param {string} url - URL of the file
   * @returns {boolean} boolean - Flag that indicates if the file exists
   *
   * @function fileExists
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
   * Loads the components present in the components array.
   * @param {int} i - Current array index
   *
   * @private
   * @function loadComponents
   */
  var loadComponents = function(i) {
    if(i < componentsLength) {
      $.ajax({
        url: terrama2Url + "/javascripts/components/" + TerraMA2WebComponents.Config.getConfJsonComponents()[components[i]],
        dataType: "script",
        success: function() {
          TerraMA2WebComponents.webcomponents[components[i]].init();
          loadComponents(++i);
        }
      });
    } else {
      componentsLoaded = true;
    }
  };

  /**
   * Returns the flag that indicates if all the components have been loaded.
   * @returns {boolean} componentsLoaded - Flag that indicates if all the components have been loaded
   *
   * @function isComponentsLoaded
   */
  var isComponentsLoaded = function() {
    return componentsLoaded;
  };

  /**
   * Initializes the necessary features.
   * @param {string} _terrama2Url - TerraMA² WebComponents URL
   * @param {array} _components - Array of components names
   *
   * @function init
   */
  var init = function(_terrama2Url, _components) {
    components = _components;
    componentsLength = components.length;
    terrama2Url = _terrama2Url;

    $.ajax({
      url: terrama2Url + "/javascripts/Config.TerraMA2WebComponents.js",
      dataType: "script",
      success: function() {
        TerraMA2WebComponents.Config.init();

        var interval = window.setInterval(function() {
          if(TerraMA2WebComponents.Config.getConfJsonComponents() !== null) {
            loadComponents(0);
            clearInterval(interval);
          }
        }, 10);

        $.ajax({ url: terrama2Url + "/socket.io/socket.io.js", dataType: "script" });
      }
    });
  };

  return {
  	getComponentsLength: getComponentsLength,
  	getTerrama2Url: getTerrama2Url,
  	fileExists: fileExists,
    isComponentsLoaded: isComponentsLoaded,
  	init: init
  };
})();
