'use strict';

define(
  ['components/Layers', 'components/Utils', 'TerraMA2WebComponents'],
  function(Layers, Utils, TerraMA2WebComponents) {
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

        if(layerType !== "template" && layerType !== "custom" && (layerObject && layerObject.dataSeriesTypeName != "GRID")) {
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
        var analysisTime = null;

        if(
          layerData.dateInfo.dates && 
          layerData.dateInfo.dates.length > 0 && 
          !isNaN(layerData.dateInfo.initialDateIndex) && 
          layerData.dateInfo.dates[layerData.dateInfo.initialDateIndex]
        )
          analysisTime = layerData.dateInfo.dates[layerData.dateInfo.initialDateIndex];

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

                tableOptions.language = {
                  "emptyTable": "<p class='text-center'>" + Utils.getTranslatedString("No data available in table") + "</p>",
                  "info": Utils.getTranslatedString("Showing") + " _START_ " + Utils.getTranslatedString("to") + " _END_ " + Utils.getTranslatedString("of") + " _TOTAL_ " + Utils.getTranslatedString("entries"),
                  "infoEmpty": Utils.getTranslatedString("Showing 0 to 0 of 0 entries"),
                  "infoFiltered": Utils.getTranslatedString("(filtered from") + " _MAX_ " + Utils.getTranslatedString("total entries)"),
                  "lengthMenu": Utils.getTranslatedString("Show") + " _MENU_ " + Utils.getTranslatedString("entries"),
                  "loadingRecords": Utils.getTranslatedString("LOADING"),
                  "processing": Utils.getTranslatedString("PROCESSING"),
                  "search": Utils.getTranslatedString("SEARCH-COLON"),
                  "zeroRecords": "<p class='text-center'>" + Utils.getTranslatedString("No matching records found") + "</p>",
                  "paginate": {
                    "first": Utils.getTranslatedString("First"),
                    "last": Utils.getTranslatedString("Last"),
                    "next": Utils.getTranslatedString("Next"),
                    "previous": Utils.getTranslatedString("Previous")
                  }
                };

                tableOptions.ajax.data = function(data) {
                  data.layer = $('#attributes-table-select > select').val();
                  data.geoserverUri = layerData.uriGeoServer;
                  data.timeStart = startDate;
                  data.timeEnd = endDate;
                  data.analysisTime = analysisTime;
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

      $('#tableCloseButton').on('click', function() {
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