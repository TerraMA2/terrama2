'use strict';

define(
  ['components/Layers', 'TerraMA2WebComponents'],
  function(Layers, TerraMA2WebComponents) {

    var init = function() {
      $("#terrama2-layerexplorer").on("click", "#terrama2-calendar", function(event) {
        var self = $(this);
        var parentLi = $(self).parent();
        var layerId = $(parentLi).attr("data-layerid");
        var layerObject = Layers.getLayerById(layerId);
        var dateInfo = layerObject.dateInfo;
        var calendar = $(self).find("input[id='" + layerId + "']");
        var minDate = dateInfo.dates.startDate;
        var maxDate = dateInfo.dates.endDate;
        var startFilterDate = dateInfo.startFilterDate;
        var endFilterDate = dateInfo.endFilterDate;

        if(!minDate || !maxDate) {
          return;
        }
        var mMinDate = moment(minDate);
        var mMaxDate = moment(maxDate);
        var mStartDate = moment(startFilterDate);
        var mEndDate = moment(endFilterDate);

        if(calendar.length === 0) {
          calendar = $("<input type='text' id='" + layerId + "' value='' style='display:none;'>");
          /**
           * Important: Due date range picker implementation, we must configure startDate and endDate. Otherwise, it will display NaN
           * date values because the start date is None, so the component take on current timestamp.
           */
          $(calendar).daterangepicker({
            "timePicker": true,
            "minDate": mMinDate,
            "startDate": mStartDate,
            "endDate": mEndDate,
            "maxDate": mMaxDate,
            "timePicker24Hour": true,
            "opens": "center"
          });

          $(this).append(calendar);

          $(calendar).on('click', function(e) {
            e.stopPropagation();
          })

          $(calendar).on("apply.daterangepicker", function(ev, picker) {
            //  DO REQUEST
            var layerId = $(parentLi).attr("data-layerid");
            var timeFormat = "YYYY-MM-DDTHH:mm:ss";
            var pickerStartDate = picker.startDate.format(timeFormat);
            var pickerEndDate = picker.endDate.format(timeFormat);
            dateInfo.startFilterDate = pickerStartDate;
            dateInfo.endFilterDate = pickerEndDate;
            Layers.updateDateInfo(dateInfo, layerId);

            $("#attributes-table-select").trigger("setAttributesTable");

            var layerTime = pickerStartDate + "Z/" + pickerEndDate + "Z";
            TerraMA2WebComponents.MapDisplay.updateLayerTime( /**id */ layerId, /** time */ layerTime);
          });
        } else {

          $(calendar).daterangepicker({
            "timePicker": true,
            "minDate": mMinDate,
            "startDate": mStartDate,
            "endDate": mEndDate,
            "maxDate": mMaxDate,
            "timePicker24Hour": true,
            "opens": "center"
          });

          $(calendar).on("apply.daterangepicker", function(ev, picker) {
            //  DO REQUEST
            var layerId = $(parentLi).attr("data-layerid");
            var timeFormat = "YYYY-MM-DDTHH:mm:ss";
            var pickerStartDate = picker.startDate.format(timeFormat);
            var pickerEndDate = picker.endDate.format(timeFormat);
            dateInfo.startFilterDate = pickerStartDate;
            dateInfo.endFilterDate = pickerEndDate;
            Layers.updateDateInfo(dateInfo, layerId);

            $("#attributes-table-select").trigger("setAttributesTable", []);

            var layerTime = pickerStartDate + "Z/" + pickerEndDate + "Z";
            TerraMA2WebComponents.MapDisplay.updateLayerTime( /**id */ layerId, /** time */ layerTime);
          });
        }

        $(calendar).trigger('click');
      });
    };

    return {
      init: init
    };
  }
);