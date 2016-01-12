/** @class Config - Class responsible for load and handle the API configurations. */
var Config = function(terrama2) {

  var confJsonHTML = null;
  var confJsonComponentsJs = null;
  var confJsonComponentsCss = null;
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
  }

  /**
   * Load the configuration files
   */
  this.loadConfigurations = function() {
    var url = terrama2.getTerrama2Url() + "/config/";

    confJsonHTML = loadConfigurationFile(url + "html.terrama2.json");
    confJsonComponentsJs = loadConfigurationFile(url + "components.javascript.terrama2.json");
    confJsonComponentsCss = loadConfigurationFile(url + "components.stylesheet.terrama2.json");
    confJsonServer = loadConfigurationFile(url + "server.terrama2.json");
  }

  /**
   * Return the HTML configuration (predefined tags)
   * @returns {json} confJsonHTML - HTML configuration
   */
  this.getConfJsonHTML = function() {
    return confJsonHTML;
  }

  /**
   * Return the javascript files configuration (javascript files paths)
   * @returns {json} confJsonComponentsJs - javascript files configuration
   */
  this.getConfJsonComponentsJs = function() {
    return confJsonComponentsJs;
  }

  /**
   * Return the stylesheets configuration (stylesheets paths)
   * @returns {json} confJsonComponentsCss - stylesheets configuration
   */
  this.getConfJsonComponentsCss = function() {
    return confJsonComponentsCss;
  }

  /**
   * Return the map server configuration
   * @returns {json} confJsonServer - map server configuration
   */
  this.getConfJsonServer = function() {
    return confJsonServer;
  }

  /**
   * Loads the necessary plugins
   */
  var loadPlugins = function() {
    $(".terrama2-resizable-horizontal").resizable({
      handles: 'e'
    });
  }

  $(document).ready(function(){
    loadPlugins();
  });
}
