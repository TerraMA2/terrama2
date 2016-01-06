var TerraMA2 = function(terrama2Url, components) {

  var _this = this;

  var componentsLength = components.length;

  var config = null;
  var mapDisplay = null;
  var layerExplorer = null;
  var toolBox = null;
  var leftBar = null;
  var filter = null;

  _this.getComponentsLength = function() {
    return componentsLength;
  }

  _this.getConfig = function() {
    return config;
  }

  _this.getMapDisplay = function() {
    return mapDisplay;
  }

  _this.getLayerExplorer = function() {
    return layerExplorer;
  }

  _this.getTerrama2Url = function() {
    return terrama2Url;
  }

  _this.injectStylesheet = function(url) {
    var link = $("<link>", { rel: "stylesheet", type: "text/css", href: url });

    link.appendTo('head');
  }

  _this.fileExists = function(url) {
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

          _this.injectStylesheet(terrama2Url + "/stylesheets/components/" + config.getConfJsonComponentsCss()[components[i]]);

          loadComponents(++i);
        }
      });
    } else {
      return;
    }
  }

  $.ajax({
    url: terrama2Url + "/javascripts/config.terrama2.js",
    dataType: "script",
    success: function() {
      config = new Config(_this);
      config.loadConfigurations();

      loadComponents(0);

      $.ajax({ url: terrama2Url + "/socket.io/socket.io.js", dataType: "script" });
    }
  });
}
