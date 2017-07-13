'use strict';

define(
  ['components/LayerStatus', 'components/Sortable', 'components/Utils', 'TerraMA2WebComponents'],
  function(LayerStatus, Sortable, Utils, TerraMA2WebComponents) {

    var memberAllLayers = [];

    var getAllLayers = function(){
      return memberAllLayers;
    }

    var addLayer = function(layerObject){
      memberAllLayers.push(layerObject);
    }

    var getLayerById = function(layerId){
      var indexLayer = memberAllLayers.map(function(l){return l.id}).indexOf(layerId);
      if( indexLayer > 0)
        return memberAllLayers[indexLayer];
      else 
        return null;
      
    }

    var createLayerObject = function (layerData){
      var layerObject = {};
      layerObject.name = layerData.name;
      layerObject.nameId = layerData.layers[0];
      layerObject.workspace = layerData.workspace;
      layerObject.id = layerData.workspace ? layerData.workspace + ":" + layerData.layers[0] : layerData.layers[0];
      layerObject.htmlId = layerObject.id.replace(":", "").split('.').join('\\.');
      layerObject.uriGeoServer = layerData.uriGeoserver;
      layerObject.parent = layerData.type;
      layerObject.serverType = layerData.serverType;
      layerObject.isParent = layerData.serverType ? false : true;
      layerObject.projectId = layerData.projectId;
      layerObject.private = layerData.private;

      return layerObject;
    }

    var removeLayer = function(layerIndex){
      memberAllLayers.splice(layerIndex, 1);
    };

    var addLayersToSort = function() {
      var itens = "";
      var allLayers = getAllLayers();
      var allLayersLength = allLayers.length;
      for (var i = allLayersLength -1; i >= 0; i--){
        if (!allLayers[i].projectId){
          var layerId = allLayers[i].id;
          var htmlId = allLayers[i].htmlId;

          var sliderDiv = "<div class='slider-content' style='display:none;'><label></label><button type='button' class='close close-slider'>×</button><div id='opacity" + htmlId + "'></div></div>";
          var spanIcon = "<span id='terrama2-opacity-slider' class='terrama2-datepicker-icon' data-toggle='tooltip' title='Change opacity'> <i class='fa fa-sliders'></i></span>"; 
          
          itens += '<li id="' + htmlId + '" data-layerid="' + layerId + '" data-parentid="terrama2-layerexplorer" class="hide">' + allLayers[i].name + sliderDiv + spanIcon + '</li>';
        }
      }
      var list = '<ul>' + itens + '</ul>';
      $('#terrama2-sortlayers').append(list);
    };

    // Add layers in layers explorer menu
    var fillLayersData = function(layers) {
      var data = layers;
      if (!data){
        data = memberAllLayers;
      }
      var currentProject = $("#projects").val();
			for(var i in data) {
				if((!data[i].private || (data[i].private && userLogged)) && data[i].projectId && data[i].projectId == currentProject) {
          var workspace = data[i].workspace;
          var layerName = data[i].name;
          var uriGeoServer = data[i].uriGeoServer;
          var serverType = data[i].serverType;
          var parent = data[i].parent;
          var layerId = data[i].id;

          if (TerraMA2WebComponents.MapDisplay.addImageWMSLayer(layerId, layerName, layerName, uriGeoServer + '/ows', serverType, false, false, "terrama2-layerexplorer", {version: "1.1.0"})){
            TerraMA2WebComponents.LayerExplorer.addLayersFromMap(layerId, parent, null, "treeview unsortable terrama2-truncate-text", null);
            TerraMA2WebComponents.MapDisplay.setLayerProperty(layerId, "layerType", parent);
            TerraMA2WebComponents.MapDisplay.setLayerProperty(layerId, "layerName", layerName);
            //var objectLayer = createLayerObject(data[i]);
            //addLayer(objectLayer);
            if (parent == 'analysis' || parent == 'dynamic'){
              var url = uriGeoServer + '/' + workspace + '/' + data[i].nameId + '/wms?service=WMS&version=1.1.0&request=GetCapabilities';
              var getCapabilitiesUrl = {
                layerName: data[i].nameId,
                layerId: layerId,
                parent: parent,
                url: url,
                format: 'xml',
                update: false
              }
              Utils.getSocket().emit('proxyRequestCapabilities', getCapabilitiesUrl);
            }
          }
          LayerStatus.changeGroupStatusIcon(parent, "working");
          LayerStatus.addLayerStatusIcon(layerId);
          LayerStatus.changeLayerStatusIcon(layerId, "working");
          Sortable.addLayerToSort(layerId, layerName);
          //Utils.getSocket().emit('checkConnection', {url: uriGeoServer, requestId: layerId});
				}
			}
    };

    return {
      fillLayersData: fillLayersData,
      removeLayer: removeLayer,
      addLayersToSort: addLayersToSort,
      createLayerObject: createLayerObject,
      addLayer: addLayer,
      getAllLayers: getAllLayers,
      getLayerById: getLayerById
    }
    
  }
);