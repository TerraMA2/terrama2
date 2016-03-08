"use strict";

/**
 * Class responsible for loading and handling the API configurations.
 * @module Config
 *
 * @property {json} confJsonComponents - Configuration JSON containing the paths of the components files.
 * @property {json} confJsonServer - Configuration JSON containing the map server data.
 */
TerraMA2WebComponents.Config = (function() {

  // Configuration JSON containing the paths of the components files
  var confJsonComponents = null;
  // Configuration JSON containing the map server data
  var confJsonServer = null;

  /**
   * Loads the configuration files.
   *
   * @private
   * @function loadConfigurations
   */
  var loadConfigurations = function() {
    var url = TerraMA2WebComponents.obj.getTerrama2Url() + "/config/";

    $.getJSON(url + "Server.TerraMA2WebComponents.json", function(data) { confJsonServer = data; });
    $.getJSON(url + "Components.TerraMA2WebComponents.json", function(data) { confJsonComponents = data; });
  };

  /**
   * Returns the configuration of the components files (files paths).
   * @returns {json} confJsonComponents - Components configuration
   *
   * @function getConfJsonComponents
   */
  var getConfJsonComponents = function() {
    return confJsonComponents;
  };

  /**
   * Returns the configuration of the map server.
   * @returns {json} confJsonServer - Configuration of the map server
   *
   * @function getConfJsonServer
   */
  var getConfJsonServer = function() {
    return confJsonServer;
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
  	getConfJsonServer: getConfJsonServer,
  	init: init
  };
})();
