'use strict';

/**
 * It makes a input hidden with date limits (startDate & endDate)
 * 
 * @returns {string}
 */
var makeHelperDatePicker = function(capability) {
  if (!capability) {
    throw new Error("Invalid capability while making date picker");
  }
  /**
   * It represents a input hidden to store min date and max date
   * @type {string}
   */
  var datepicker = "<input type='hidden' name='" + capability.name + "'";

  if (capability.extent instanceof Array) {
    datepicker += " data-min-date='" + capability.extent[0] + "' data-max-date='" + capability.extent[capability.extent.length - 1] + "'>";
  } else if (capability.extent instanceof Object) {
    datepicker += " data-min-date='" + capability.extent.startDate + "' data-max-date='" + capability.extent.endDate + "'>";
  }

  return datepicker;
};

$("#terrama2-layerexplorer").on("click", "#terrama2-calendar", function(event) {
  var self = $(this);
  var parentLi = $(self).parent();
  var capability = $(self).attr("data-layerid");
  var calendar = $(self).find("input[id='"+ capability +"']");
  var hidden = $(self).find("input[type='hidden']");
  var minDate = $(hidden).attr('data-min-date');
  var maxDate = $(hidden).attr('data-max-date');
  if (!minDate || !maxDate) {
    return;
  }
  if (calendar.length === 0) {
    calendar = $("<input type='text' id='" + capability + "' value='' style='display:none;'>");

    var mMinDate = moment(minDate);
    var mMaxDate = moment(maxDate);
    /**
     * Important: Due date range picker implementation, we must configure startDate and endDate. Otherwise, it will display NaN
     * date values because the start date is None, so the component take on current timestamp.
     */
    $(calendar).daterangepicker({
      "timePicker": true,
      "minDate": mMinDate,
      "startDate": mMaxDate,
      "endDate": mMaxDate,
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

      var layerTime = pickerStartDate + "Z/" + pickerEndDate + "Z";
      TerraMA2WebComponents.MapDisplay.updateLayerTime(/**id */layerId, /** time */layerTime);
    });
  }

  $(calendar).trigger('click');
});