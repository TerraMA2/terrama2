'use strict';

define(
  ['TerraMA2WebComponents'],
  function(TerraMA2WebComponents) {
    
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
        case "new":
          return BASE_URL + "images/status/yellow-black.gif";
        case "newerraccess":
          return BASE_URL + "images/status/yellow-gray.gif";
        case "working":
          return BASE_URL + "images/status/green_icon.svg";
        case "erraccess":
          return BASE_URL + "images/status/gray_icon.svg";
        case "alert":
          return BASE_URL + "images/status/red-black.gif";
        default:
        	return "";
      }
    };

    var loadEvents = function(){
			//change status icon when close the group layer
			$('.parent_li').on('click', function() {
				var parent = this.getAttribute('id');
				var parentLi = $("#" + parent);

				if(parentLi.hasClass('open')) {
					var imageStatus = parentLi.find("#image-group-icon");
					var actualStatus = imageStatus.attr("src");

					if(actualStatus == BASE_URL + "images/status/yellow-black.gif" || actualStatus == BASE_URL + "images/status/red-black.gif")
						changeGroupStatusIcon(parent, "working");
					else if (actualStatus == BASE_URL + "images/status/yellow-gray.gif" || actualStatus == BASE_URL + "images/status/red-black.gif")
						changeGroupStatusIcon(parent, "erraccess");
				}
			});
    };

    var init = function(){
      loadEvents();
    }

    return {
      init: init,
      changeLayerStatusIcon: changeLayerStatusIcon,
      addLayerStatusIcon: addLayerStatusIcon,
      changeGroupStatusIcon: changeGroupStatusIcon,
      addGroupSpanIcon: addGroupSpanIcon
    }
  }
);