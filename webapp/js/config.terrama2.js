var Config = function() {

  var confJsonHTML = null;
  var confJsonComponentsJs = null;
  var confJsonComponentsCss = null;

  var loadConfigurationFile = function(file) {
    var _return = null;

    $.ajax({ url: file, dataType: 'json', async: false, success: function(data) { _return = data; } });

    return _return;
  }

  var loadConfigurations = function() {
    confJsonHTML = loadConfigurationFile("/terrama2/webapp/config/html.terrama2.json");
    confJsonComponentsJs = loadConfigurationFile("/terrama2/webapp/config/components.javascript.terrama2.json");
    confJsonComponentsCss = loadConfigurationFile("/terrama2/webapp/config/components.stylesheet.terrama2.json");
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

  this.loadConfigurations = loadConfigurations;
  this.getConfJsonHTML = getConfJsonHTML;
  this.getConfJsonComponentsJs = getConfJsonComponentsJs;
  this.getConfJsonComponentsCss = getConfJsonComponentsCss;

  $(document).ready(function(){
    $('.date').mask('00/00/0000');
  });
}
