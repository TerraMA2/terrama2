var TerraMA2 = function(components) {

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

  var injectStylesheet = function(url) {
    var link = $("<link>", {
      rel: "stylesheet",
      type: "text/css",
      href: url
    });

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

  this.getComponentsLength = getComponentsLength;
  this.getConfig = getConfig;
  this.getMapDisplay = getMapDisplay;
  this.getLayerExplorer = getLayerExplorer;
  this.injectStylesheet = injectStylesheet;
  this.fileExists = fileExists;

  $.ajax({ async: false, url: "/terrama2/js/config.terrama2.js", dataType: "script" });

  var config = null;
  config = new Config();
  config.loadConfigurations();

  var componentsLength = components.length;

  var mapDisplay = null;
  var layerExplorer = null;
  var toolBox = null;
  var leftBar = null;

  for(var i = 0; i < componentsLength; i++) {
    $.ajax({ async: false, url: "/terrama2/js/components/" + config.getConfJsonComponents()[components[i]] + ".js", dataType: "script" });
    injectStylesheet("/terrama2/css/components/" + config.getConfJsonComponents()[components[i]] + ".css");

    if(components[i] === "MapDisplay") {
      mapDisplay = new MapDisplay();
    } else if(components[i] === "LayerExplorer") {
      layerExplorer = new LayerExplorer(this);
    } else if(components[i] === "ToolBox") {
      toolBox = new ToolBox(this);
    } else if(components[i] === "LeftBar") {
      leftBar = new LeftBar(this);
    }
  }
}
