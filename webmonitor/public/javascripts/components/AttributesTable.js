'use strict';

define(
  ['components/Layers', 'TerraMA2WebComponents'],
  function(Layers, TerraMA2WebComponents) {
    var memberTable = null;
    var memberDefaultTableOptions = {
      "bAutoWidth": false,
      "order": [
        [0, "asc"]
      ],
      "processing": true,
      "serverSide": true,
      "ajax": {
        "url": BASE_URL + "get-attributes-table",
        "type": "POST"
      },
      "language": {
        "emptyTable": "<p class='text-center'>Nenhum registro a ser exibido</p>",
        "info": "Exibindo _START_ at&eacute; _END_ de _TOTAL_ registros",
        "infoEmpty": "Exibindo 0 at&eacute; 0 de 0 registros",
        "infoFiltered": "(filtrado de _MAX_ registros)",
        "lengthMenu": "Exibir _MENU_ registros",
        "loadingRecords": "Carregando...",
        "processing": "Processando...",
        "search": "Pesquisa:",
        "zeroRecords": "<p class='text-center'>Nenhum registro encontrado</p>",
        "paginate": {
          "first": "Primeira",
          "last": "&Uacute;ltima",
          "next": "Pr&oacute;xima",
          "previous": "Anterior"
        }
      }
    };

    var createAttributesTable = function() {
      var showButton = false;
      var visibleLayers = Layers.getVisibleLayers();

      $('#attributes-table-select > select').empty();

      for(var i = 0, visibleLayersLength = visibleLayers.length; i < visibleLayersLength; i++) {
        var layerObject = visibleLayers[i];

        var layerId = layerObject.id;
        var layerName = layerObject.name;
        var layerType = layerObject.parent;

        if(layerType !== "template" && layerType !== "custom" && (layerObject && layerObject.parent != "analysis" && layerObject.dataSeriesTypeName != "GRID")) {
          $('#attributes-table-select > select').append($('<option></option>').attr('value', layerId).text(layerName));
          if(!showButton) showButton = true;
        }
      }

      if(!showButton)
        hideAttributesTable(true);
      else
        $('#tableButton').removeClass('hidden');
    };

    var setAttributesTable = function() {
      if($('#attributes-table-select > select').val() !== null) {
        var layerId = $('#attributes-table-select > select').val();

        var layerData = Layers.getLayerById(layerId);

        var startDate = layerData.dateInfo.startFilterDate;
        var endDate = layerData.dateInfo.endFilterDate;

        if(layerData !== null && layerData.id !== undefined && layerData.uriGeoServer !== undefined) {
          $.get(BASE_URL + 'get-columns', {
              layer: layerData.id,
              geoserverUri: layerData.uriGeoServer
            },
            function(response) {
              if(response.fields.length > 0) {
                var columns = response.fields;
                var columnsLength = columns.length;
                var columnsArray = [];
                var titles = "";

                for(var i = 0; i < columnsLength; i++) {
                  columnsArray.push({
                    "name": columns[i].name
                  });
                  titles += "<th>" + columns[i].name + "</th>";
                }

                if(memberTable !== null)
                  memberTable.destroy(true);

                $("#attributes-table-div").empty().html("<table class=\"table table-bordered table-hover\" id=\"attributes-table\"><thead>" + titles + "</thead><tfoot>" + titles + "</tfoot></table>");

                var tableOptions = $.extend(true, {}, memberDefaultTableOptions);

                tableOptions.ajax.data = function(data) {
                  data.layer = $('#attributes-table-select > select').val();
                  data.geoserverUri = layerData.uriGeoServer;
                  data.timeStart = startDate;
                  data.timeEnd = endDate;
                };

                tableOptions.columns = columnsArray;

                memberTable = $('#attributes-table').DataTable(tableOptions);
              }
            }
          );
        } else
          hideAttributesTable(true);
      } else
        hideAttributesTable(true);
    };

    var hideAttributesTable = function(hideButton) {
      $("#attributes-table-div").empty();

      if(!$('#table-div > div.main-div').hasClass('hidden'))
        $('#table-div > div.main-div').addClass('hidden');

      $("#table-div").css('display', 'none');

      if(hideButton && !$("#tableButton").hasClass('hidden'))
        $("#tableButton").addClass('hidden');
    };

    var loadEvents = function() {
      $('#attributes-table-select > select').on('change', setAttributesTable);

      $('#tableButton > button').on('click', function() {
        if($('#table-div').css('display') === 'none') {
          $('#table-div').css('display', '');
          $('#table-div > div.main-div').removeClass('hidden');
          setAttributesTable();
        } else
          hideAttributesTable(false);
      });

      $("#terrama2-map").on("createAttributesTable", function(event) {
        createAttributesTable();
      });

      $("#attributes-table-select").on("setAttributesTable", function(event) {
        setAttributesTable();
      });
    };

    var init = function() {
      $("#table-div").resizable({
        minHeight: 400,
        handles: "n"
      });

      loadEvents();
    };

    return {
      init: init
    };
  }
);