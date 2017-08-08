'use strict';

define(
  ['components/Layers', 'TerraMA2WebComponents'],
  function(Layers, TerraMA2WebComponents) {

    var setSlider = function(dateInfo, layerId) {
      var valMap = dateInfo.dates;
      var initDate = dateInfo.initialDateIndex;

      var slider = $("#slider" + layerId.replace(':', ''));
      var sliderParent = $(slider).parent();

      if(!$(sliderParent).is(":visible"))
        $(sliderParent).show();
      else
        $(sliderParent).hide();

      var labelDate = $(sliderParent).find("label");
      $(labelDate).text(moment.utc(dateInfo.dates[initDate]).format("lll"));

      $(slider).slider({
        min: 0,
        max: valMap.length - 1,
        value: initDate,
        slide: function(event, ui) {
          $(labelDate).text(moment.utc(dateInfo.dates[ui.value]).format("lll"));
        },
        stop: function(event, ui) {
          doSlide(layerId, dateInfo.dates[ui.value]);
          dateInfo.initialDateIndex = ui.value;
          Layers.updateDateInfo(dateInfo, layerId);
        }
      });
    };

    var doSlide = function(layerId, layerTime) {
      var timeFormat = moment.utc(layerTime).format("YYYY-MM-DDThh:mm:ss") + "Z";
      TerraMA2WebComponents.MapDisplay.updateLayerTime(layerId, layerTime);
    };

    var changeLayerOpacity = function(layerId, opacityValue) {
      TerraMA2WebComponents.MapDisplay.updateLayerOpacity(layerId, opacityValue / 100);
    };

    var setOpacitySlider = function(layerId, initialValue) {
      var slider = $("#opacity" + layerId.replace(':', '').split('.').join('\\.'));
      var sliderParent = $(slider).parent();

      var label = $(sliderParent).find("label");
      $(label).text("Opacity: " + initialValue + "%");

      $(slider).slider({
        min: 0,
        max: 100,
        value: initialValue,
        slide: function(event, ui) {
          $(label).text("Opacity: " + ui.value + "%");
        },
        stop: function(event, ui) {
          changeLayerOpacity(layerId, ui.value);
        }
      });
    };

    var init = function() {
      /**
       * Triggered when user clicks on TerraMA² Slider in Layers Menu
       * 
       * @param {Event}
       */
      $("#terrama2-layerexplorer").on("click", "#terrama2-slider", function(event) {
        var self = $(this);
        var parentLi = $(self).parent();
        var parentId = $(parentLi).attr("data-layerid");
        var layerObject = Layers.getLayerById(parentId);
        var dateInfo = layerObject.dateInfo;

        if(!dateInfo) {
          console.log("Date info not found...");
          return;
        }
        if(!dateInfo.dates instanceof Array) {
          console.log("Date info has not dates array.");
          return;
        }

        setSlider(dateInfo, parentId);
      });

      $("#terrama2-layerexplorer").on("click", "button[class~='close-slider']", function(e) {
        $(this).parent().hide();
      });
    };

    return {
      setOpacitySlider: setOpacitySlider,
      init: init
    };
  }
);