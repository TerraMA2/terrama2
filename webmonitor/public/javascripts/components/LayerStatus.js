'use strict';

define(
  ['enums/LayerStatusEnum', 'TerraMA2WebComponents'],
  function(LayerStatusEnum, TerraMA2WebComponents) {

    var addGroupSpanIcon = function() {
      var groupElement = $(".parent_li > .group-name > .sidebar-item-text");
      var span = "<img id='image-group-icon' src=''>";
      groupElement.prepend(span);
    };

    var changeGroupStatusIcon = function(parent, status) {
      var statusElement = $("#" + parent).find('#image-group-icon');
      statusElement.addClass("status-icon");
      var statusImage = getStatusIconUrl(status);
      if(statusImage == "") {
        statusElement.removeClass("status-icon");
      }
      statusElement.attr('src', statusImage);
    };

    var addLayerStatusIcon = function(htmlLayerId) {
      $('#terrama2-layerexplorer #'+htmlLayerId + " .sidebar-subitem-text").prepend(`<img id='image-icon' src=''>`);
    };

    var addChartIcon = function(htmlLayerId) {
      $('#terrama2-layerexplorer #'+htmlLayerId + " .dropdown-layer-tools ul").append(`<i class="fa fa-line-chart"></i>`);
    };

    var changeLayerStatusIcon = function(htmlLayerId, status) {
      var layerElement = $("#" + htmlLayerId).find("#image-icon");
      layerElement.addClass("status-icon");
      var statusImage = getStatusIconUrl(status);
      layerElement.attr('src', statusImage);
    };

    var getStatusIconUrl = function(status) {
      switch(status) {
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
      addChartIcon,
      changeGroupStatusIcon: changeGroupStatusIcon,
      addGroupSpanIcon: addGroupSpanIcon
    };
  }
);