var Config = function(terrama2) {

  var _this = this;

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

  _this.loadConfigurations = function() {
    var url = terrama2.getTerrama2Url() + "/config/";

    confJsonHTML = loadConfigurationFile(url + "html.terrama2.json");
    confJsonComponentsJs = loadConfigurationFile(url + "components.javascript.terrama2.json");
    confJsonComponentsCss = loadConfigurationFile(url + "components.stylesheet.terrama2.json");
    confJsonServer = loadConfigurationFile(url + "server.terrama2.json");
    confJsonFilter = loadConfigurationFile(url + "filter.terrama2.json");
  }

  _this.getConfJsonHTML = function() {
    return confJsonHTML;
  }

  _this.getConfJsonComponentsJs = function() {
    return confJsonComponentsJs;
  }

  _this.getConfJsonComponentsCss = function() {
    return confJsonComponentsCss;
  }

  _this.getConfJsonServer = function() {
    return confJsonServer;
  }

  _this.getConfJsonFilter = function() {
    return confJsonFilter;
  }

  $(document).ready(function(){
    $('.terrama2-date').mask("00/00/0000", {clearIfNotMatch: true});
    //$('.terrama2-table').DataTable();
  });
}
