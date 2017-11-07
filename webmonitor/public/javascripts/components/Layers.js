'use strict';

define(
  ['components/LayerStatus', 'components/Sortable', 'components/Utils', 'enums/LayerStatusEnum', 'TerraMA2WebComponents'],
  function(LayerStatus, Sortable, Utils, LayerStatusEnum, TerraMA2WebComponents) {

    var memberAllLayers = [];

    var getAllLayers = function() {
      return memberAllLayers;
    }

    var addLayer = function(layerObject) {
      memberAllLayers.push(layerObject);
    }

    var getLayerById = function(layerId) {
      var indexLayer = memberAllLayers.map(function(l) {
        return l.id
      }).indexOf(layerId);
      if(indexLayer > 0)
        return memberAllLayers[indexLayer];
      else
        return null;
    }

    var getLayersByProject = function(projectId, includeTemplates, includeCustomLayers) {
      includeTemplates = (includeTemplates !== undefined ? includeTemplates : false);
      includeCustomLayers = (includeCustomLayers !== undefined ? includeCustomLayers : false);
      var layersToReturn = [];

      for(var i = 0, layersLength = memberAllLayers.length; i < layersLength; i++)
        if(memberAllLayers[i].projectId == projectId || (includeTemplates && memberAllLayers[i].parent === "template") || (includeCustomLayers && memberAllLayers[i].custom))
          layersToReturn.push($.extend({}, memberAllLayers[i]));

      return layersToReturn;
    };

    var createLayerObject = function(layerData) {
      var layerObject = {};
      layerObject.name = layerData.name;
      layerObject.description = layerData.description;
      layerObject.nameId = layerData.layers[0];
      layerObject.workspace = layerData.workspace;
      layerObject.id = layerData.workspace ? layerData.workspace + ":" + layerData.layers[0] : layerData.layers[0];
      layerObject.htmlId = layerObject.id.replace(":", "").split('.').join('\\.');
      layerObject.uriGeoServer = layerData.uriGeoserver;
      layerObject.parent = layerData.type;
      layerObject.serverType = layerData.serverType;
      layerObject.isParent = layerData.type ? false : true;
      layerObject.projectId = layerData.projectId;
      layerObject.private = layerData.private;
      layerObject.dataSeriesTypeName = layerData.dataSeriesTypeName;
      layerObject.visible = false;
      layerObject.status = LayerStatusEnum.ONLINE;
      layerObject.exportation = (layerData.exportation !== undefined && layerData.exportation.error === null && layerData.exportation.data !== null ? layerData.exportation.data : null);
      layerObject.dateInfo = {};
      layerObject.boundingBox = [];

      if(layerData.type)
        layerObject.opacity = 1;

      return layerObject;
    };

    var setLayerOpacity = function(id, opacity) {
      var indexLayer = memberAllLayers.map(function(l) { return l.id; }).indexOf(id);

      if(indexLayer != -1)
        memberAllLayers[indexLayer].opacity = opacity;
    };

    var updateDateInfo = function(dateInfo, layerId) {
      var indexLayer = memberAllLayers.map(function(l) {
        return l.id
      }).indexOf(layerId);
      if(indexLayer != -1) {
        memberAllLayers[indexLayer].dateInfo = dateInfo;
      }
    };

    var updateBoundingBox = function(boundingBox, layerId) {
      var indexLayer = memberAllLayers.map(function(l) {
        return l.id
      }).indexOf(layerId);

      if(indexLayer != -1) {
        memberAllLayers[indexLayer].boundingBox = boundingBox;
      }
    };

    var changeLayerStatus = function(layerId, newStatus) {
      var indexLayer = memberAllLayers.map(function(l) {
        return l.id
      }).indexOf(layerId);
      if(indexLayer != -1) {
        memberAllLayers[indexLayer].status = newStatus;
        LayerStatus.changeLayerStatusIcon(memberAllLayers[indexLayer].htmlId, newStatus);
      }
    };

    var changeParentLayerStatus = function(layerId, newStatus) {
      var indexLayer = memberAllLayers.map(function(l) {
        return l.id
      }).indexOf(layerId);
      if(indexLayer != -1) {
        memberAllLayers[indexLayer].status = newStatus;
        LayerStatus.changeGroupStatusIcon(memberAllLayers[indexLayer].id, newStatus);
      }
    };

    var changeLayerVisible = function(layerId, newVisible) {
      var indexLayer = memberAllLayers.map(function(l) {
        return l.id
      }).indexOf(layerId);
      if(indexLayer != -1) {
        memberAllLayers[indexLayer].visible = newVisible;
      }
    };

    var getVisibleLayers = function() {
      var visibleLayers = [];
      memberAllLayers.forEach(function(layers) {
        if(layers.visible)
          visibleLayers.push(layers);
      });
      return visibleLayers;
    }

    var removeLayer = function(layer) {
      removeLayerOfExplorer(layer);

      var indexLayer = memberAllLayers.map(function(l) { return l.id; }).indexOf(layer.id);

      if(indexLayer != -1)
        memberAllLayers.splice(indexLayer, 1);
    };

    var removeLayerOfExplorer = function(layer) {
      if(layer.visible)
        $("#" + layer.htmlId + " input.terrama2-layerexplorer-checkbox").trigger("click");

      $("#terrama2-sortlayers").find('li#' + layer.htmlId).remove();
      TerraMA2WebComponents.LayerExplorer.removeLayer(layer.id, "terrama2-layerexplorer");

      if($("#" + layer.parent + " li").length == 0)
        changeParentLayerStatus(layer.parent, "");
    };

    var removePrivateLayers = function() {
      var layersToRemove = [];

      memberAllLayers.forEach(function(layer) {
        if(layer.private)
          layersToRemove.push($.extend(true, {}, layer));
      });

      layersToRemove.forEach(function(layer) {
        removeLayer(layer);
      });
    };

    var addLayersToSort = function() {
      var itens = "";
      var allLayers = getAllLayers();
      var allLayersLength = allLayers.length;

      for(var i = allLayersLength - 1; i >= 0; i--) {
        if(!allLayers[i].projectId) {
          var layerId = allLayers[i].id;
          var htmlId = allLayers[i].htmlId;

          var spanIcon = "<span class='terrama2-layer-tools terrama2-datepicker-icon' data-i18n='[title]Layer Tools'>" + (allLayers[i].parent != 'custom' && allLayers[i].parent != 'template' ? " <i class='glyphicon glyphicon-resize-full'></i>" : "") + " <i class='fa fa-gear'></i></span>";

          itens += '<li id="' + htmlId + '" data-layerid="' + layerId + '" data-parentid="terrama2-layerexplorer" class="hide" title="' + allLayers[i].name + '"><span class="layer-name">' + allLayers[i].name + '</span>' + spanIcon + '</li>';
        }
      }

      $('#terrama2-sortlayers').append('<ul>' + itens + '</ul>');
      $('#terrama2-sortlayers').localize();
    };

    // Add layers in layers explorer menu
    var fillLayersData = function(layers) {
      var data = (layers ? layers : memberAllLayers);
      var currentProject = $("#projects").val();

      for(var i in data) {
        if(data[i].projectId && data[i].projectId == currentProject) {
          var workspace = data[i].workspace;
          var layerName = data[i].name;
          var uriGeoServer = data[i].uriGeoServer;
          var serverType = data[i].serverType;
          var parent = data[i].parent;
          var layerId = data[i].id;
          var htmlId = data[i].htmlId;

          if(TerraMA2WebComponents.MapDisplay.addImageWMSLayer(layerId, layerName, layerName, uriGeoServer + '/ows', serverType, false, false, "terrama2-layerexplorer", { version: "1.1.0" })) {
            TerraMA2WebComponents.LayerExplorer.addLayersFromMap(layerId, parent, null, "treeview unsortable terrama2-truncate-text", null);

            if(parent != 'custom' && parent != 'template') {
              getLayerCapabilities(uriGeoServer, workspace, data[i].nameId, layerId, parent, false);
            }
          }

          LayerStatus.changeGroupStatusIcon(parent, LayerStatusEnum.ONLINE);
          LayerStatus.addLayerStatusIcon(htmlId);
          LayerStatus.changeLayerStatusIcon(htmlId, LayerStatusEnum.ONLINE);
          Sortable.addLayerToSort(layerId, layerName, parent);

          Utils.getSocket().emit('checkConnection', {
            url: uriGeoServer,
            requestId: layerId
          });
        }
      }
    };

    var getLayerCapabilities = function(uriGeoServer, workspace, nameId, layerId, parent, update) {
      var url = uriGeoServer + '/' + workspace + '/' + nameId + '/wms?service=WMS&version=1.1.0&request=GetCapabilities';

      var getCapabilitiesUrl = {
        layerName: nameId,
        layerId: layerId,
        parent: parent,
        url: url,
        format: 'xml',
        update: update
      };

      Utils.getSocket().emit('proxyRequestCapabilities', getCapabilitiesUrl);
    };

    return {
      fillLayersData: fillLayersData,
      removeLayer: removeLayer,
      removeLayerOfExplorer: removeLayerOfExplorer,
      removePrivateLayers: removePrivateLayers,
      addLayersToSort: addLayersToSort,
      createLayerObject: createLayerObject,
      setLayerOpacity: setLayerOpacity,
      addLayer: addLayer,
      getAllLayers: getAllLayers,
      getLayerById: getLayerById,
      getLayersByProject: getLayersByProject,
      changeLayerVisible: changeLayerVisible,
      getVisibleLayers: getVisibleLayers,
      changeLayerStatus: changeLayerStatus,
      changeParentLayerStatus: changeParentLayerStatus,
      updateDateInfo: updateDateInfo,
      updateBoundingBox: updateBoundingBox,
      getLayerCapabilities: getLayerCapabilities
    };
  }
);