'use strict';

define(
  ['components/LayerStatus', 'components/Sortable', 'components/Utils', 'TerraMA2WebComponents'],
  function(LayerStatus, Sortable, Utils, TerraMA2WebComponents) {

    var memberAllLayers = [];

    var getAllLayers = function(){
      return memberAllLayers;
    };

    var addLayer = function(layer){
      memberAllLayers.push(layer);
    };

    var removeLayer = function(layerIndex){
      memberAllLayers.splice(layerIndex, 1);
    };

    var addLayersToSort = function() {
      var itens = "";
      var allLayers = memberAllLayers;
      var allLayersLength = allLayers.length;
      for (var i = allLayersLength -1; i >= 0; i--){
        var layerId = allLayers[i].id;
        if (layerId.includes(':')){
          layerId = layerId.replace(':','')
        }

        var sliderDiv = "<div class='slider-content' style='display:none;'><label></label><button type='button' class='close close-slider'>×</button><div id='opacity" + layerId + "'></div></div>";
        var spanIcon = "<span id='terrama2-opacity-slider' class='terrama2-datepicker-icon' data-toggle='tooltip' title='Change opacity'> <i class='fa fa-sliders'></i></span>"; 
        
        itens += '<li id="' + layerId + '" data-layerid="' + allLayers[i].id + '" data-parentid="terrama2-layerexplorer" class="hide">' + allLayers[i].name + sliderDiv + spanIcon + '</li>';
      }
      var list = '<ul>' + itens + '</ul>';
      $('#terrama2-sortlayers').append(list);
    };

    // Add layers in layers explorer menu
    var fillLayersData = function(data) {
			for(var i in data) {
				if(!data[i].private || (data[i].private && userLogged)) {
          var workspace = data[i].workspace;
          var layerName = data[i].name;
          var uriGeoServer = data[i].uriGeoserver;
          var serverType = data[i].serverType;
          var layerId = workspace + ":" + data[i].layers[0];
          Utils.getSocket().emit('checkConnection', {url: uriGeoServer, requestId: layerId});
          if (memberAllLayers.map(function (l){return l.id}).indexOf(layerId) > 0)
            continue;

          if (TerraMA2WebComponents.MapDisplay.addTileWMSLayer(layerId, layerName, layerName, uriGeoServer + '/ows', serverType, false, false, "terrama2-layerexplorer", {version: "1.1.0"})){
            TerraMA2WebComponents.LayerExplorer.addLayersFromMap(layerId, data[i].type, null, "treeview unsortable terrama2-truncate-text", null);
            TerraMA2WebComponents.MapDisplay.setLayerProperty(layerId, "layerType", data[i].type);
            TerraMA2WebComponents.MapDisplay.setLayerProperty(layerId, "layerName", layerName);
            memberAllLayers.push({id: layerId, name: layerName, url: uriGeoServer});
            if (data[i].type == 'analysis' || data[i].type == 'dynamic'){
              var url = uriGeoServer + '/' + workspace + '/' + data[i].layers[0] + '/wms?service=WMS&version=1.1.0&request=GetCapabilities';
              var getCapabilitiesUrl = {
                layerName: data[i].layers[0],
                layerId: layerId,
                parent: data[i].type,
                url: url,
                format: 'xml',
                update: false
              }
              Utils.getSocket().emit('proxyRequestCapabilities', getCapabilitiesUrl);
            }
          }
          LayerStatus.changeGroupStatusIcon(data[i].type, "working");
          LayerStatus.addLayerStatusIcon(layerId);
          LayerStatus.changeLayerStatusIcon(layerId, "working");
          Sortable.addLayerToSort(layerId, layerName);
				}

			}
    };

    return {
      fillLayersData: fillLayersData,
      getAllLayers: getAllLayers,
      addLayer: addLayer,
      removeLayer: removeLayer,
      addLayersToSort: addLayersToSort
    }
    
  }
);