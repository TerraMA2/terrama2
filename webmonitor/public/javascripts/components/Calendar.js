'use strict';

define(
  ['components/Layers', 'components/Utils', 'TerraMA2WebComponents'],
  function(Layers, Utils, TerraMA2WebComponents) {

    var init = function() {
      $("#terrama2-layerexplorer").on("click", "#terrama2-calendar", function(event) {
        var self = $(this);
        var parentLi = $(self).closest("li");
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
        var mMinDate = moment(minDate.replace('Z', ''));
        var mMaxDate = moment(maxDate.replace('Z', ''));
        var mStartDate = moment(startFilterDate.replace('Z', ''));
        var mEndDate = moment(endFilterDate.replace('Z', ''));

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
            "opens": "center",
            "locale": {
              "format": Utils.getTranslatedString("DATE-FORMAT"),
              "separator": " - ",
              "applyLabel": Utils.getTranslatedString("Apply"),
              "cancelLabel": Utils.getTranslatedString("Cancel"),
              "fromLabel": Utils.getTranslatedString("From"),
              "toLabel": Utils.getTranslatedString("To"),
              "customRangeLabel": Utils.getTranslatedString("Custom"),
              "weekLabel": Utils.getTranslatedString("WEEK-LABEL"),
              "daysOfWeek": [
                Utils.getTranslatedString("DAY-SU"),
                Utils.getTranslatedString("DAY-MO"),
                Utils.getTranslatedString("DAY-TU"),
                Utils.getTranslatedString("DAY-WE"),
                Utils.getTranslatedString("DAY-TH"),
                Utils.getTranslatedString("DAY-FR"),
                Utils.getTranslatedString("DAY-SA"),
              ],
              "monthNames": [
                Utils.getTranslatedString("January"),
                Utils.getTranslatedString("February"),
                Utils.getTranslatedString("March"),
                Utils.getTranslatedString("April"),
                Utils.getTranslatedString("May"),
                Utils.getTranslatedString("June"),
                Utils.getTranslatedString("July"),
                Utils.getTranslatedString("August"),
                Utils.getTranslatedString("September"),
                Utils.getTranslatedString("October"),
                Utils.getTranslatedString("November"),
                Utils.getTranslatedString("December")
              ],
              "firstDay": 1
            }
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

            //Change seconds to 59
            pickerEndDate = pickerEndDate.slice(0, -2) + "59";
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
            "opens": "center",
            "locale": {
              "format": Utils.getTranslatedString("DATE-FORMAT"),
              "separator": " - ",
              "applyLabel": Utils.getTranslatedString("Apply"),
              "cancelLabel": Utils.getTranslatedString("Cancel"),
              "fromLabel": Utils.getTranslatedString("From"),
              "toLabel": Utils.getTranslatedString("To"),
              "customRangeLabel": Utils.getTranslatedString("Custom"),
              "weekLabel": Utils.getTranslatedString("WEEK-LABEL"),
              "daysOfWeek": [
                Utils.getTranslatedString("DAY-SU"),
                Utils.getTranslatedString("DAY-MO"),
                Utils.getTranslatedString("DAY-TU"),
                Utils.getTranslatedString("DAY-WE"),
                Utils.getTranslatedString("DAY-TH"),
                Utils.getTranslatedString("DAY-FR"),
                Utils.getTranslatedString("DAY-SA"),
              ],
              "monthNames": [
                Utils.getTranslatedString("January"),
                Utils.getTranslatedString("February"),
                Utils.getTranslatedString("March"),
                Utils.getTranslatedString("April"),
                Utils.getTranslatedString("May"),
                Utils.getTranslatedString("June"),
                Utils.getTranslatedString("July"),
                Utils.getTranslatedString("August"),
                Utils.getTranslatedString("September"),
                Utils.getTranslatedString("October"),
                Utils.getTranslatedString("November"),
                Utils.getTranslatedString("December")
              ],
              "firstDay": 1
            }
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

            //Change seconds to 59
            pickerEndDate = pickerEndDate.slice(0, -2) + "59";
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