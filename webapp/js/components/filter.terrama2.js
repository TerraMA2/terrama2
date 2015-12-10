var Filter = function(terrama2) {

  var mapDisplay = terrama2.getMapDisplay();
  var map = mapDisplay.getMap();
  var filterConfig = terrama2.getConfig().getConfJsonFilter();

  var applyDateFilter = function(dateFrom, dateTo) {
    var cql = filterConfig.DateFieldName + ">=" + processDate(dateFrom, filterConfig.DateFormat);
    cql += " and ";
    cql += filterConfig.DateFieldName + "<=" + processDate(dateTo, filterConfig.DateFormat);

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

  this.applyDateFilter = applyDateFilter;

  $(document).ready(function() {

    $('#terrama2-filter-button').on('click', function(el) {
      var dateFrom = $('#terrama2-filter-date-de').val();
      var dateTo = $('#terrama2-filter-date-ate').val();

      if(dateFrom.length > 0 && dateTo.length > 0) {
        applyDateFilter(dateFrom, dateTo);
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
  });
}
