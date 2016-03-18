"use strict";

/**
 * Class responsible for loading and handling the API configurations.
 * @module Config
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {json} memberConfJsonComponents - Configuration JSON containing the paths of the components files.
 */
TerraMA2WebComponents.Config = (function() {

  // Configuration JSON containing the paths of the components files
  var memberConfJsonComponents = null;

  /**
   * Loads the configuration files.
   *
   * @private
   * @function loadConfigurations
   */
  var loadConfigurations = function() {
    var url = TerraMA2WebComponents.obj.getTerrama2Url() + "/config/";

    $.getJSON(url + "Components.TerraMA2WebComponents.json", function(data) { memberConfJsonComponents = data; });
  };

  /**
   * Returns the configuration of the components files (files paths).
   * @returns {json} memberConfJsonComponents - Components configuration
   *
   * @function getConfJsonComponents
   */
  var getConfJsonComponents = function() {
    return memberConfJsonComponents;
  };

  /**
   * Initializes the necessary features.
   *
   * @function init
   */
  var init = function() {
    loadConfigurations();
  };

  return {
  	getConfJsonComponents: getConfJsonComponents,
  	init: init
  };
})();
