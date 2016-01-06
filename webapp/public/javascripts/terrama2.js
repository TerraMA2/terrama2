var TerraMA2 = function(terrama2Url, components) {

  var _this = this;

  var getComponentsLength = function() {
    return componentsLength;
  }

  var getConfig = function() {
    return config;
  }

  var getMapDisplay = function() {
    return mapDisplay;
  }

  var getLayerExplorer = function() {
    return layerExplorer;
  }

  var getTerrama2Url = function() {
    return terrama2Url;
  }

  var injectStylesheet = function(url) {
    var link = $("<link>", { rel: "stylesheet", type: "text/css", href: url });

    link.appendTo('head');
  }

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
  }

  var loadComponents = function(i) {
    if(i < componentsLength) {
      $.ajax({
        url: terrama2Url + "/javascripts/components/" + config.getConfJsonComponentsJs()[components[i]],
        dataType: "script",
        success: function() {
          if(components[i] === "MapDisplay") {
            mapDisplay = new MapDisplay();
          } else if(components[i] === "LayerExplorer") {
            layerExplorer = new LayerExplorer(_this);
          } else if(components[i] === "ToolBox") {
            toolBox = new ToolBox(_this);
          } else if(components[i] === "LeftBar") {
            leftBar = new LeftBar(_this);
          } else if(components[i] === "Filter") {
            filter = new Filter(_this);
          }

          injectStylesheet(terrama2Url + "/stylesheets/components/" + config.getConfJsonComponentsCss()[components[i]]);

          loadComponents(++i);
        }
      });
    } else {
      return;
    }
  }

  var componentsLength = components.length;

  var config = null;
  var mapDisplay = null;
  var layerExplorer = null;
  var toolBox = null;
  var leftBar = null;
  var filter = null;

  this.getComponentsLength = getComponentsLength;
  this.getConfig = getConfig;
  this.getMapDisplay = getMapDisplay;
  this.getLayerExplorer = getLayerExplorer;
  this.injectStylesheet = injectStylesheet;
  this.fileExists = fileExists;
  this.getTerrama2Url = getTerrama2Url;

  //$.ajax({ async: false, url: terrama2Url + "/javascripts/config.terrama2.js", dataType: "script" });
  $.ajax({
    url: "http://localhost:3001/javascripts/config.terrama2.js",
    dataType: "script",
    success: function() {
      config = new Config(_this);
      config.loadConfigurations();

      loadComponents(0);
    }
  });
}
