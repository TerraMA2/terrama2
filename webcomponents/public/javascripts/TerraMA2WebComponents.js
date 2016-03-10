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
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {array} memberComponents - Array of components names received on the initialization.
 * @property {int} memberComponentsLength - Length of the components names array.
 * @property {boolean} memberComponentsLoaded - Flag that indicates if all the components have been loaded.
 * @property {string} memberTerrama2Url - TerraMA² WebComponents URL.
 */
TerraMA2WebComponents.obj = (function() {

  // Array of components names received on the initialization
  var memberComponents = null;
  // Length of the components names array
  var memberComponentsLength = null;
  // Flag that indicates if all the components have been loaded
  var memberComponentsLoaded = false;
  // TerraMA² WebComponents URL
  var memberTerrama2Url = null;

  /**
   * Returns the length of the components names array.
   * @returns {int} memberComponentsLength - Length of the components names array
   *
   * @function getComponentsLength
   */
  var getComponentsLength = function() {
    return memberComponentsLength;
  };

  /**
   * Returns the URL of the TerraMA² WebComponents.
   * @returns {string} memberTerrama2Url - URL of the TerraMA² WebComponents
   *
   * @function getComponentsLength
   */
  var getTerrama2Url = function() {
    return memberTerrama2Url;
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
    if(i < memberComponentsLength) {
      $.ajax({
        url: memberTerrama2Url + "/javascripts/components/" + TerraMA2WebComponents.Config.getConfJsonComponents()[memberComponents[i]],
        dataType: "script",
        success: function() {
          TerraMA2WebComponents.webcomponents[memberComponents[i]].init();
          loadComponents(++i);
        }
      });
    } else {
      memberComponentsLoaded = true;
    }
  };

  /**
   * Returns the flag that indicates if all the components have been loaded.
   * @returns {boolean} memberComponentsLoaded - Flag that indicates if all the components have been loaded
   *
   * @function isComponentsLoaded
   */
  var isComponentsLoaded = function() {
    return memberComponentsLoaded;
  };

  /**
   * Initializes the necessary features.
   * @param {string} terrama2Url - TerraMA² WebComponents URL
   * @param {array} components - Array of components names
   *
   * @function init
   */
  var init = function(terrama2Url, components) {
    memberComponents = components;
    memberComponentsLength = components.length;
    memberTerrama2Url = terrama2Url;

    $.ajax({
      url: memberTerrama2Url + "/javascripts/Config.TerraMA2WebComponents.js",
      dataType: "script",
      success: function() {
        TerraMA2WebComponents.Config.init();

        var interval = window.setInterval(function() {
          if(TerraMA2WebComponents.Config.getConfJsonComponents() !== null) {
            loadComponents(0);
            clearInterval(interval);
          }
        }, 10);

        $.ajax({ url: memberTerrama2Url + "/socket.io/socket.io.js", dataType: "script" });
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
