'use strict';

define(
  ['components/Utils', 'components/Slider', 'components/Layers', 'TerraMA2WebComponents'],
  function(Utils, Slider, Layers, TerraMA2WebComponents) {
    var loadSocketsListeners = function() {
      Utils.getWebAppSocket().on('generateFileResponse', function(result) {
        if(result.progress !== undefined && result.progress >= 100) {
          console.log('Quase lá! O arquivo está sendo preparado para o download<span>...</span>');
        } else if(result.progress !== undefined) {
          console.log(result.progress + '% Completo');
        } else {
          var exportLink = webadminHostInfo.protocol + webadminHostInfo.host + ":" + webadminHostInfo.port + webadminHostInfo.basePath + "export?folder=" + result.folder + "&file=" + result.file;
          $('#exportation-iframe').attr('src', exportLink);
        }
      });
    };

    var loadEvents = function() {
      $('#export').on('click', function() {
        var layer = Layers.getLayerById($(this).data("layerid"));

        if(layer !== null && layer.exportation !== null) {
          var exportationParams = {
            format: $("#exportation-type").val().toString(),
            schema: layer.exportation.schema,
            table: layer.exportation.table,
            dataProviderId: layer.exportation.dataProviderId,
            fileName: layer.name
          };

          if(layer.exportation.dateField !== null) {
            exportationParams.dateTimeField = layer.exportation.dateField;
            exportationParams.dateTimeFrom = $("#terrama2-calendar").find("input[type='hidden']").attr("start-date");
            exportationParams.dateTimeTo = $("#terrama2-calendar").find("input[type='hidden']").attr("end-date");
          }

          Utils.getWebAppSocket().emit('generateFileRequest', exportationParams);
        }
			});

      $("#terrama2-sortlayers").on("click", ".terrama2-layer-tools", function() {
        var layer = Layers.getLayerById($(this).parent().data("layerid"));

        if(layer !== null) {
          if(layer.exportation !== null) {
            $("#export").data("layerid", layer.id);

            if($("#exportation-box").hasClass("hidden"))
              $("#exportation-box").removeClass("hidden");

            $("#layer-toolbox").css("height", "265px");
          } else {
            if(!$("#exportation-box").hasClass("hidden"))
              $("#exportation-box").addClass("hidden");

            $("#layer-toolbox").css("height", "100px");
          }

          $("#layer-toolbox > .layer-toolbox-header .layer-name").text(layer.name);

          $("#layer-toolbox > .layer-toolbox-body > #slider-box").empty().html("<label></label><br/><div id=\"opacity" + layer.htmlId + "\"></div>");
          var currentOpacity = TerraMA2WebComponents.MapDisplay.getLayerOpacity(layer.id) * 100;
          Slider.setOpacitySlider(layer.id, currentOpacity);

          if($("#layer-toolbox").hasClass("hidden"))
            $("#layer-toolbox").removeClass("hidden");
        }
      });

      $("#layer-toolbox > .layer-toolbox-header > .btn").on("click", function() {
        $("#layer-toolbox").addClass("hidden");
      });
    };

    var init = function() {
      loadSocketsListeners();
      loadEvents();

      $("#layer-toolbox").draggable({
				containment: $('#terrama2-map')
			});
    };

    return {
      init: init
    };
  }
);