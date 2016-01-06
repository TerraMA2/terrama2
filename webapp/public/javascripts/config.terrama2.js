var Config = function(terrama2) {

  var confJsonHTML = null;
  var confJsonComponentsJs = null;
  var confJsonComponentsCss = null;
  var confJsonServer = null;
  var confJsonFilter = null;

  var loadConfigurationFile = function(file) {
    var _return = null;

    $.ajax({ url: file, dataType: 'json', async: false, success: function(data) { _return = data; } });

    return _return;
  }

  var loadConfigurations = function() {
    var url = terrama2.getTerrama2Url() + "/config/";

    confJsonHTML = loadConfigurationFile(url + "html.terrama2.json");
    confJsonComponentsJs = loadConfigurationFile(url + "components.javascript.terrama2.json");
    confJsonComponentsCss = loadConfigurationFile(url + "components.stylesheet.terrama2.json");
    confJsonServer = loadConfigurationFile(url + "server.terrama2.json");
    confJsonFilter = loadConfigurationFile(url + "filter.terrama2.json");
  }

  var getConfJsonHTML = function() {
    return confJsonHTML;
  }

  var getConfJsonComponentsJs = function() {
    return confJsonComponentsJs;
  }

  var getConfJsonComponentsCss = function() {
    return confJsonComponentsCss;
  }

  var getConfJsonServer = function() {
    return confJsonServer;
  }

  var getConfJsonFilter = function() {
    return confJsonFilter;
  }

  this.loadConfigurations = loadConfigurations;
  this.getConfJsonHTML = getConfJsonHTML;
  this.getConfJsonComponentsJs = getConfJsonComponentsJs;
  this.getConfJsonComponentsCss = getConfJsonComponentsCss;
  this.getConfJsonServer = getConfJsonServer;
  this.getConfJsonFilter = getConfJsonFilter;

  $(document).ready(function(){
    $('.terrama2-date').mask("00/00/0000", {clearIfNotMatch: true});
    //$('.terrama2-table').DataTable();
  });
}
