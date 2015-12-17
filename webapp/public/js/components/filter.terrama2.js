var Filter = function(terrama2) {

  var mapDisplay = terrama2.getMapDisplay();
  var map = mapDisplay.getMap();
  var filterConfig = terrama2.getConfig().getConfJsonFilter();
  var serverConfig = terrama2.getConfig().getConfJsonServer();

  var createDateFilter = function(dateFrom, dateTo) {
    var cql = filterConfig.DateFieldName + ">=" + processDate(dateFrom, filterConfig.DateFormat);
    cql += " and ";
    cql += filterConfig.DateFieldName + "<=" + processDate(dateTo, filterConfig.DateFormat);

    return cql;
  }

  var createSatelliteFilter = function(satellite) {
    return filterConfig.SatelliteFieldName + "='" + satellite + "'";
  }

  var applyFilter = function(dateFrom, dateTo, satellite) {
    var cql = "";

    if(dateFrom.length > 0 && dateTo.length > 0) {
      cql += createDateFilter(dateFrom, dateTo);
    }

    if(dateFrom.length > 0 && dateTo.length > 0 && satellite !== "all") {
      cql += " AND ";
    }

    if(satellite !== "all") {
      cql += createSatelliteFilter(satellite);
    }

    mapDisplay.findBy(map.getLayerGroup(), 'name', filterConfig.LayerToFilter).getSource().updateParams({ "CQL_FILTER": cql });
  }

  var processDate = function(date, format) {
    var finalDate = format;

    var dd = date.split("/")[0];
    var mm = date.split("/")[1];
    var yyyy = date.split("/")[2];

    if(format.match(/YYYY/)) {
      finalDate = finalDate.replace("YYYY", yyyy);
    } if(format.match(/MM/)) {
      finalDate = finalDate.replace("MM", mm);
    } if(format.match(/DD/)) {
      finalDate = finalDate.replace("DD", dd);
    }

    return finalDate;
  }

  var createSatelliteSelect = function() {
    var elem = "<option value='all'>TODOS</option>";
    var feature = {};

    $.ajax({
      url: serverConfig.ProxyURL,
      dataType: 'json',
      async: true,
      data: {
        url: serverConfig.URL,
        params: serverConfig.GetFeatureParams + filterConfig.LayerToFilter
      },
      success: function(data) {
        feature = data;

        var satellites = [];

        //for(var i = 0; i < feature.totalFeatures; i++) {
        for(var i = 0; i < 1000; i++) {
          satellites.push(feature.features[i].properties[filterConfig.SatelliteFieldName]);
        }

        satellites = unique(satellites).sort();

        var satellitesLength = satellites.length;

        for(var i = 0; i < satellitesLength; i++) {
          elem += "<option value='" + satellites[i] + "'>" + satellites[i] + "</option>";
        }

        $("#terrama2-filter-satellite").empty().append(elem);
      }
    });
  }

  function unique(list) {
    var result = [];

    $.each(list, function(i, e) {
      if($.inArray(e, result) == -1) result.push(e);
    });

    return result;
  }

  this.applyFilter = applyFilter;

  $(document).ready(function() {

    $("#terrama2-leftbar").find("[terrama2-box='terrama2-filter']").addClass('terrama2-leftbar-button-filter').attr('title', 'Filtro');

    $('#terrama2-filter-button').on('click', function(el) {
      var dateFrom = $('#terrama2-filter-date-de').val();
      var dateTo = $('#terrama2-filter-date-ate').val();
      var satellite = $('#terrama2-filter-satellite').val();

      if((dateFrom.length > 0 && dateTo.length > 0) || (dateFrom.length === 0 && dateTo.length === 0)) {
        applyFilter(dateFrom, dateTo, satellite);
      } else {
        if(dateFrom.length === 0) {
          $("#terrama2-filter-date-de:not([class*='warning'])").removeClass('regular').addClass('warning');
        }
        if(dateTo.length === 0) {
          $("#terrama2-filter-date-ate:not([class*='warning'])").removeClass('regular').addClass('warning');
        }
      }
    });

    $('.terrama2-input').on('focus', function(el) {
      if($(this).hasClass('warning')) {
        $(this).removeClass('warning').addClass('regular');
      }
    });

    //applyDateFilter('01/11/2015', '02/11/2015');
    createSatelliteSelect();
  });
}
