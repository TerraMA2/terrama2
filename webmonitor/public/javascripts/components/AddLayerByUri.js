'use strict';

define(
  ['components/Calendar', 'components/Layers', 'components/Utils', 'components/Sortable', 'TerraMA2WebComponents'],
  function(Calendar, Layers, Utils, Sortable, TerraMA2WebComponents) {

    var memberCapabilities;
    var memberSelectedLayers = [];

    var addCustomLayer = function(layer) {
      var allLayers = Layers.getAllLayers();

      if(TerraMA2WebComponents.MapDisplay.addImageWMSLayer(layer.id, layer.name, layer.name, layer.url, "geoserver", false, false, "terrama2-layerexplorer", { version: "1.1.0" })) {
        TerraMA2WebComponents.LayerExplorer.addLayersFromMap(layer.id, "custom", true, "treeview unsortable terrama2-truncate-text", null);
        allLayers.push(layer);

        Sortable.addLayerToSort(layer.id, layer.name, layer.parent);

        var span = "";
        var listElement = $("li[data-layerid='custom']");
        var li = $(listElement).find("li[data-layerid='" + layer.id + "']");

        if(li.length === 0) {
          li.find('input').click();

          if(!layer.extent) {
            var sliderDiv = "<div class='slider-content' style='display:none;'><label></label><button type='button' class='close close-slider'>×</button><div id='slider" + $(li).attr("data-layerid").replace(':', '') + "'></div></div>";
            $(li).append(sliderDiv);

            if(layer.extent instanceof Array) {
              if(layer.extent.length > 1) {
                span += "<i id='terrama2-slider' class='fa fa-sliders'></i>";
              }
            } else if(layer.extent instanceof Object) {
              span += "<i id='terrama2-calendar' class='fa fa-calendar'></i>";
            }
            $("li[data-layerid='" + layer.id + "'] .dropdown-layer-tools ul").append(span);
            // $(li).append($(span));
          }
        }
      }
    };

    var saveLayers = function() {
      var url = document.getElementById("wmsUri").value;
      var parser = document.createElement('a');
      parser.href = url;
      var geoUrl = parser.protocol + '//' + parser.host + parser.pathname;

      memberSelectedLayers.forEach(addInLayerExplorer);

      function addInLayerExplorer(element, index, array) {
        var allLayers = Layers.getAllLayers();
        for(var i in memberCapabilities) {
          if(memberCapabilities[i].name == element) {
            if(allLayers.map(function(l) {
                return l.id
              }).indexOf(memberCapabilities[i].name) > 0) {
              continue;
            }

            addCustomLayer({
              id: memberCapabilities[i].name,
              name: memberCapabilities[i].title,
              extent: memberCapabilities[i].extent,
              url: geoUrl,
              visible: false,
              custom: true,
              parent: "custom",
              htmlId: memberCapabilities[i].name.replace(":", "").split('.').join('\\.'),
              opacity: 1
            });
          }
        }

        if(!$("#custom").hasClass('open')) {
          $("#custom").addClass('open');
        }
        var groupSpanName = $('#custom').find(' span.group-name');
        groupSpanName.click();
      }

      $('#layersModal').modal('hide');
    };

    var fillModal = function(capabilities) {
      memberCapabilities = capabilities;
      var lis = "";
      var check = "<input type='checkbox' class='terrama2-layerviews-checkbox'/>";

      for(var i in memberCapabilities)
        lis += '<li data-layerid="' + memberCapabilities[i].name + '">' + check + '<span>' + memberCapabilities[i].title + '</span>' + '</li>';

      $('#layersModalBody > div').addClass('hidden');
      $('#layersModalBody > span').append('<ul id="layersList">' + lis + '</ul>');
      $('#saveLayers').removeClass('hidden');
      $('#restart').removeClass('hidden');
    };

    var resetModal = function() {
      memberSelectedLayers = [];
      $("#layersList").remove();
      $("#wmsUri").val("");
      $('#layersModalBody > div').removeClass('hidden');
      $('#saveLayers').addClass('hidden');
      $('#restart').addClass('hidden');
    };

    var addLayers = function() {
      var url = document.getElementById("wmsUri").value;
      if(!url) return;

      var jsonData = {
        url: url,
        format: 'xml'
      }
      Utils.getSocket().emit('proxyRequest', jsonData);
    };

    var loadEvents = function() {
      $("#wmsUri").on('keyup', function(e) {
        if(e.keyCode == 13) {
          e.preventDefault();
          addLayers();
        }
      });

      document.getElementById("saveLayers").addEventListener("click", saveLayers);

      document.getElementById("addLayers").addEventListener("click", addLayers);

      $('#layersModalBody > span').on('click', 'input.terrama2-layerviews-checkbox', function() {
        var layerid = $(this).closest('li').data('layerid');
        var index = memberSelectedLayers.indexOf(layerid);

        if(index > -1)
          memberSelectedLayers.splice(index, 1);
        else
          memberSelectedLayers.push(layerid);
      });

      $('#layersModal').on('hidden.bs.modal', function(e) {
        resetModal();
      });

      $('#restart').on('click', function(e) {
        resetModal();
      });

      $("#custom .fa-plus").on("click", function(event) {
        event.stopPropagation();

        $('#layersModal').modal('show');
      });
    };

    var init = function() {
      loadEvents();
    };

    return {
      addCustomLayer: addCustomLayer,
      fillModal: fillModal,
      init: init
    };
  }
);