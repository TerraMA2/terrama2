"use strict";

/** @class Config - Class responsible for load and handle the API configurations. */
TerraMA2WebComponents.Config = (function() {
  var confJsonHTML = null;
  var confJsonComponents = null;
  var confJsonServer = null;

  /**
   * Load a given configuration file
   * @param {string} file - url to the file
   * @returns {json} _return - configuration file content
   */
  var loadConfigurationFile = function(file) {
    var _return = null;

    $.ajax({ url: file, dataType: 'json', async: false, success: function(data) { _return = data; } });

    return _return;
  };

  /**
   * Load the configuration files
   */
  var loadConfigurations = function() {
    var url = TerraMA2WebComponents.obj.getTerrama2Url() + "/config/";

    confJsonHTML = loadConfigurationFile(url + "html.terrama2.json");
    confJsonComponents = loadConfigurationFile(url + "components.terrama2.json");
    confJsonServer = loadConfigurationFile(url + "server.terrama2.json");
  };

  /**
   * Return the HTML configuration (predefined tags)
   * @returns {json} confJsonHTML - HTML configuration
   */
  var getConfJsonHTML = function() {
    return confJsonHTML;
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

  /**
   * Loads the necessary plugins
   */
  var loadPlugins = function() {

  };

  var init = function() {
    loadPlugins();
    loadConfigurations();
  };

  return {
  	getConfJsonHTML: getConfJsonHTML,
  	getConfJsonComponents: getConfJsonComponents,
  	getConfJsonServer: getConfJsonServer,
  	init: init
  };
})();
