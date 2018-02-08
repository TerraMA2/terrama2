'use strict';

define(
  ['components/Utils', 'TerraMA2WebComponents'],
  function(Utils, TerraMA2WebComponents) {

    var memberFilters = {};

    var lala = function() {
      return { null:null };
    };

    var loadEvents = function() {
      $("#add-filter-item-button").on("click", function() {
        $("#filter-items").append("<div class=\"row\">" +
          "<div class=\"col-lg-3\">" +
            "<div class=\"form-group\">" +
              "<label>Coluna</label>" +
              "<select class=\"form-control\">" +
                "<option>option 1</option>" +
                "<option>option 2</option>" +
                "<option>option 3</option>" +
                "<option>option 4</option>" +
                "<option>option 5</option>" +
              "</select>" +
            "</div>" +
          "</div>" +
          "<div class=\"col-lg-3\" style=\"padding-left: 0;\">" +
            "<div class=\"form-group\">" +
              "<label>Operador</label>" +
              "<select class=\"form-control\">" +
                "<option>+</option>" +
                "<option>-</option>" +
                "<option>*</option>" +
                "<option>/</option>" +
                "<option>%</option>" +
                "<option>^</option>" +
                "<option>=</option>" +
                "<option>></option>" +
                "<option><</option>" +
                "<option><></option>" +
                "<option><=</option>" +
                "<option>>=</option>" +
                "<option>IN</option>" +
                "<option>LIKE</option>" +
                "<option>ILIKE</option>" +
                "<option>IS</option>" +
                "<option>NOT</option>" +
              "</select>" +
            "</div>" +
          "</div>" +
          "<div class=\"col-lg-3\" style=\"padding-left: 0;\">" +
            "<div class=\"form-group\">" +
              "<label>Valor</label>" +
              "<input type=\"text\" class=\"form-control\"/>" +
            "</div>" +
          "</div>" +
          "<div class=\"col-lg-2\" style=\"padding-left: 0;\">" +
            "<div class=\"form-group\">" +
              "<label>Coluna</label>" +
              "<select class=\"form-control\">" +
                "<option>AND</option>" +
                "<option>OR</option>" +
              "</select>" +
            "</div>" +
          "</div>" +
          "<div class=\"col-lg-1\" style=\"padding-left: 0;\">" +
            "<div class=\"form-group\">" +
              "<button type=\"button\" class=\"btn btn-block btn-primary btn-sm remove-filter-item-button\" style=\"margin-top: 27px;\"><i class=\"fa fa-trash\"></i></button>" +
            "</div>" +
          "</div>" +
        "</div>");
      });

      $("#filter-items").on("click", ".remove-filter-item-button", function() {
        $(this).closest(".row").remove();
      });
    };

    var init = function() {
      $("#filter-box").draggable({
        containment: $('#terrama2-map'),
        handle: '.filter-header'
      }).resizable({
        handles: 'all',
        minHeight: 100,
        minWidth: 100
      });

      loadEvents();
    };

    return {
      init: init
    };
  }
);