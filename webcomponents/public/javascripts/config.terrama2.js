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
   * Load the configuration files
   */
  var loadConfigurations = function() {
    var url = TerraMA2WebComponents.obj.getTerrama2Url() + "/config/";

    $.getJSON(url + "server.terrama2.json", function(data) { confJsonServer = data; });
    $.getJSON(url + "components.terrama2.json", function(data) { confJsonComponents = data; });
  };

  /**
   * Return the components files configuration (files paths)
   * @returns {json} confJsonComponents - files configuration
   */
  var getConfJsonComponents = function() {
    return confJsonComponents;
  };

  /**
   * Return the map server configuration
   * @returns {json} confJsonServer - map server configuration
   */
  var getConfJsonServer = function() {
    return confJsonServer;
  };

  var init = function() {
    loadConfigurations();
  };

  return {
  	getConfJsonComponents: getConfJsonComponents,
  	getConfJsonServer: getConfJsonServer,
  	init: init
  };
})();
