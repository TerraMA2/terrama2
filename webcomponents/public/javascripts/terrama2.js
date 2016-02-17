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
  var componentsLoaded = false;

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
   * Load the TerraMA² components present in the components array
   * @param {number} i - current array position
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

  var isComponentsLoaded = function() {
    return componentsLoaded;
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
  	fileExists: fileExists,
    isComponentsLoaded: isComponentsLoaded,
  	init: init
  };
})();
