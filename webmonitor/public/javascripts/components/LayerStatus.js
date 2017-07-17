'use strict';

define(
  ['enums/LayerStatusEnum', 'TerraMA2WebComponents'],
  function(LayerStatusEnum, TerraMA2WebComponents) {
    
    var addGroupSpanIcon = function() {
      var groupElement = $(".parent_li").children(".group-name");
      var span = "<span class='span-group-icon pull-left'> <img id='image-group-icon' src=''> </span>";
      groupElement.prepend(span);
    };

    var changeGroupStatusIcon = function(parent, status) {
      var statusElement = $("#"+parent).find('#image-group-icon');
      statusElement.addClass("status-icon");
      var statusImage = getStatusIconUrl(status);
      if (statusImage == ""){
        statusElement.removeClass("status-icon");
      }
      statusElement.attr('src', statusImage);
    };

    var addLayerStatusIcon = function(htmlLayerId) {
      var layerElement = $('#terrama2-layerexplorer').find("#"+htmlLayerId);
      var span = "<span class='span-layer-icon'> <img id='image-icon' src=''> </span>";
      layerElement.prepend(span);
    };

    var changeLayerStatusIcon = function(htmlLayerId, status) {
      var layerElement = $("#"+htmlLayerId).find("#image-icon");
      layerElement.addClass("status-icon");
      var statusImage = getStatusIconUrl(status);
      layerElement.attr('src', statusImage);
    };

    var getStatusIconUrl = function(status) {
      switch(status){
        case LayerStatusEnum.NEW:
          return BASE_URL + "images/status/yellow-black.gif";
        case LayerStatusEnum.NEW_OFFLINE:
          return BASE_URL + "images/status/yellow-gray.gif";
        case LayerStatusEnum.ONLINE:
          return BASE_URL + "images/status/green_icon.svg";
        case LayerStatusEnum.OFFLINE:
          return BASE_URL + "images/status/gray_icon.svg";
        case LayerStatusEnum.ALERT:
          return BASE_URL + "images/status/red-black.gif";
        default:
        	return "";
      }
    };

    return {
      changeLayerStatusIcon: changeLayerStatusIcon,
      addLayerStatusIcon: addLayerStatusIcon,
      changeGroupStatusIcon: changeGroupStatusIcon,
      addGroupSpanIcon: addGroupSpanIcon
    }
  }
);