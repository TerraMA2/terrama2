'use strict';

define(
  ['components/Layers', 'components/Utils', 'TerraMA2WebComponents'],
  function(Layers, Utils, TerraMA2WebComponents) {

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
      $(labelDate).text(moment(dateInfo.dates[initDate].replace('Z', '')).format(Utils.getTranslatedString("DATE-FORMAT") + " HH:mm"));

      $(slider).slider({
        min: 0,
        max: valMap.length - 1,
        value: initDate,
        slide: function(event, ui) {
          $(labelDate).text(moment(dateInfo.dates[ui.value].replace('Z', '')).format(Utils.getTranslatedString("DATE-FORMAT") + " HH:mm"));
        },
        stop: function(event, ui) {
          doSlide(layerId, dateInfo.dates[ui.value]);
          dateInfo.initialDateIndex = ui.value;
          Layers.updateDateInfo(dateInfo, layerId);
        }
      });
    };

    var doSlide = function(layerId, layerTime) {
      var timeFormat = moment(layerTime.replace('Z', '')).format("YYYY-MM-DDThh:mm:ss") + "Z";
      TerraMA2WebComponents.MapDisplay.updateLayerTime(layerId, layerTime);
    };

    var changeLayerOpacity = function(layerId, opacityValue) {
      TerraMA2WebComponents.MapDisplay.updateLayerOpacity(layerId, opacityValue / 100);
      Layers.setLayerOpacity(layerId, opacityValue / 100);
    };

    var setOpacitySlider = function(layerId, initialValue) {
      var slider = $("#opacity" + layerId.replace(':', '').split('.').join('\\.'));
      var sliderParent = $(slider).parent();

      var label = $(sliderParent).find("label");
      $(label).text(initialValue + "%");

      $(slider).slider({
        min: 0,
        max: 100,
        value: initialValue,
        slide: function(event, ui) {
          $(label).text(ui.value + "%");
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

        if(!dateInfo) return;
        if(!dateInfo.dates instanceof Array) return;

        setSlider(dateInfo, parentId);
      });

      $("#terrama2-layerexplorer").on("click", "button[class~='close-slider']", function(e) {
        $(this).parent().hide();
      });
    };

    return {
      doSlide: doSlide,
      changeLayerOpacity: changeLayerOpacity,
      setOpacitySlider: setOpacitySlider,
      init: init
    };
  }
);