"use strict";

/**
 * Class responsible for loading and handling the API configurations.
 * @module Config
 *
 * @property {json} memberConfJsonComponents - Configuration JSON containing the paths of the components files.
 * @property {json} memberConfJsonServer - Configuration JSON containing the map server data.
 */
TerraMA2WebComponents.Config = (function() {

  // Configuration JSON containing the paths of the components files
  var memberConfJsonComponents = null;
  // Configuration JSON containing the map server data
  var memberConfJsonServer = null;

  /**
   * Loads the configuration files.
   *
   * @private
   * @function loadConfigurations
   */
  var loadConfigurations = function() {
    var url = TerraMA2WebComponents.obj.getTerrama2Url() + "/config/";

    $.getJSON(url + "Server.TerraMA2WebComponents.json", function(data) { memberConfJsonServer = data; });
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
   * Returns the configuration of the map server.
   * @returns {json} memberConfJsonServer - Configuration of the map server
   *
   * @function getConfJsonServer
   */
  var getConfJsonServer = function() {
    return memberConfJsonServer;
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
