'use strict';

angular.module('terrama2.dcpImporter', ['terrama2.services']).run(function($templateCache) {
  $templateCache.put(
    "modals.html",
    "<div id=\"importParametersModal\" class=\"modal fade\">" +
      "<div class=\"modal-dialog\">" +
        "<div class=\"modal-content\">" +
          "<div class=\"modal-header\">" +
            "<button type=\"button\" class=\"close\" data-dismiss=\"modal\" aria-label=\"Close\">" +
              "<span aria-hidden=\"true\">×</span>" +
            "</button>" +
            "<h4 class=\"modal-title\">Select the delimiter character of the CSV and choose if the file has a header</h4>" +
          "</div>" +
          "<div class=\"modal-body\">" +
            "<div class=\"form-group\">" +
              "<label>{{ i18n.__('Delimiter character') }}</label>" +
              "<select class=\"form-control\" name=\"delimiterCharacter\" ng-model=\"csvImport.delimiterCharacter\">" +
                "<option value=\";\">Semicolon (;)</option>" +
                "<option value=\",\">Comma (,)</option>" +
              "</select>" +
            "</div>" +
            "<div class=\"form-group\">" +
              "<div class=\"checkbox\">" +
                "<label>" +
                  "<input name=\"hasHeader\" ng-model=\"csvImport.hasHeader\" type=\"checkbox\"> <span style=\"font-weight: 900;\">{{ i18n.__('The first line is a header?') }}</span>" +
                "</label>" +
              "</div>" +
            "</div>" +
            "<hr style=\"border: 1px solid #eee !important;\"/>" +
            "<button type=\"button\" class=\"btn btn-default pull-left\" data-dismiss=\"modal\">Close</button>" +
            "<button type=\"button\" class=\"btn btn-primary pull-right\" ng-click=\"selectFileToImport()\">Import</button>" +
            "<div style=\"clear: both;\"></div>" +
          "</div>" +
        "</div>" +
      "</div>" +
    "</div>" +
    "<div id=\"importDCPItemsModal\" class=\"modal fade\">" +
      "<div class=\"modal-dialog\">" +
        "<div class=\"modal-content\">" +
          "<div class=\"modal-header\">" +
            "<button type=\"button\" class=\"close\" data-dismiss=\"modal\" aria-label=\"Close\">" +
              "<span aria-hidden=\"true\">×</span>" +
            "</button>" +
            "<h4 class=\"modal-title\">Select the fields</h4>" +
          "</div>" +
          "<div class=\"modal-body\">" +
            "<div ng-repeat=\"semantic in dataSeries.semantics.metadata.form | filter: defaultValueFilter\">" +
              "<div class=\"form-group col-md-{{ importationModalColSize[semantic.key] }}\">" +
                "<label>{{ i18n.__(dataSeries.semantics.metadata.schema.properties[semantic.key].title) }}:</label>" +
                "<select class=\"form-control\" name=\"{{ semantic.key }}Toa5\" ng-model=\"importationFields.toa5[semantic.key]\">" +
                  "<option value=\"\">{{ i18n.__('Select a column') }}</option>" +
                  "<option ng-repeat=\"column in csvImport.finalData.header\" ng-init=\"importationFields.toa5[semantic.key] = ''\" value=\"{{ column }}\">{{ column }}</option>" +
                "</select>" +
              "</div>" +
              "<div class=\"form-group col-md-{{ importationModalColSize[semantic.key] }}\" ng-show=\"dataSeries.semantics.metadata.schema.properties[semantic.key].hasDefaultFieldForImport && semantic.titleMap\">" +
                "<label>{{ i18n.__('Default') }}:</label>" +
                "<select class=\"form-control\" id=\"{{ semantic.key }}Toa5Default\" name=\"{{ semantic.key }}Toa5Default\" ng-model=\"importationFields.toa5[semantic.key + 'Default']\">" +
                  "<option ng-repeat=\"titleMap in semantic.titleMap\" label=\"{{ titleMap.name }}\" value=\"{{ titleMap.value }}\">{{ titleMap.name }}</option>" +
                "</select>" +
              "</div>" +
              "<div class=\"form-group col-md-{{ importationModalColSize[semantic.key] }}\" ng-show=\"dataSeries.semantics.metadata.schema.properties[semantic.key].hasDefaultFieldForImport && !semantic.titleMap\">" +
                "<label>{{ i18n.__('Default') }}:</label>" +
                "<input class=\"form-control\" id=\"{{ semantic.key }}Toa5Default\" name=\"{{ semantic.key }}Toa5Default\" ng-model=\"importationFields.toa5[semantic.key + 'Default']\" placeholder=\"{{ i18n.__('Default') }}\" type=\"text\">" +
              "</div>" +
              "<div class=\"form-group col-md-{{ importationModalColSize[semantic.key] }}\" ng-show=\"dataSeries.semantics.metadata.schema.properties[semantic.key].hasPrefixFieldForImport\">" +
                "<label>{{ i18n.__('Prefix') }}:</label>" +
                "<input class=\"form-control\" id=\"{{ semantic.key }}Toa5Prefix\" name=\"{{ semantic.key }}Toa5Prefix\" ng-model=\"importationFields.toa5[semantic.key + 'Prefix']\" placeholder=\"{{ i18n.__('Prefix') }}\" type=\"text\">" +
              "</div>" +
              "<div class=\"form-group col-md-{{ importationModalColSize[semantic.key] }}\" ng-show=\"dataSeries.semantics.metadata.schema.properties[semantic.key].hasSuffixFieldForImport\">" +
                "<label>{{ i18n.__('Suffix') }}:</label>" +
                "<input class=\"form-control\" id=\"{{ semantic.key }}Toa5Suffix\" name=\"{{ semantic.key }}Toa5Suffix\" ng-model=\"importationFields.toa5[semantic.key + 'Suffix']\" placeholder=\"{{ i18n.__('Suffix') }}\" type=\"text\">" +
              "</div>" +
            "</div>"+
            "<hr style=\"border: 1px solid #eee !important;\"/>" +
            "<button type=\"button\" class=\"btn btn-default pull-left\" data-dismiss=\"modal\">Close</button>" +
            "<button type=\"button\" class=\"btn btn-primary pull-right\" ng-click=\"validateImportationMetadata()\">Import</button>" +
            "<div style=\"clear: both;\"></div>" +
          "</div>" +
        "</div>" +
      "</div>" +
    "</div>"
  );
})
.directive('terrama2DcpImporter', function() {
  return {
    restrict: 'EA',
    templateUrl: 'modals.html',
    controller: ['$scope', 'FileDialog', 'i18n', 'MessageBoxService', function($scope, FileDialog, i18n, MessageBoxService) {
      $scope.csvImport = {};
      $scope.importationFields = {
        toa5: {}
      };

      $scope.defaultValueFilter = function(item) {
        return $scope.dataSeries.semantics.metadata.schema.properties[item.key].defaultForImport === undefined;
      };

      $scope.openImportParametersModal = function() {
        $('#importParametersModal').modal('show');
      };

      $scope.selectFileToImport = function() {
        $('#importParametersModal').modal('hide');

        FileDialog.openFile(function(err, input) {
          if(err) {
            $scope.display = true;
            $scope.alertBox.message = err.toString();
            return;
          }

          FileDialog.readAsCSV(input.files[0], $scope.csvImport.delimiterCharacter, $scope.csvImport.hasHeader, function(error, csv) {
            // applying angular scope..
            $scope.$apply(function() {
              if(error) {
                setError(error);
                console.log(error);
                return;
              }

              $scope.csvImport.finalData = csv;
              $scope.importationModalColSize = {};

              for(var i = 0, semanticsLength = $scope.dataSeries.semantics.metadata.form.length; i < semanticsLength; i++) {
                var numberOfFields = 1;
                var semanticsKey = $scope.dataSeries.semantics.metadata.form[i].key;
                $scope.importationModalColSize[semanticsKey] = 12;

                if($scope.dataSeries.semantics.metadata.schema.properties[semanticsKey].hasDefaultFieldForImport)
                  $scope.importationModalColSize[semanticsKey] /= ++numberOfFields;

                if($scope.dataSeries.semantics.metadata.schema.properties[semanticsKey].hasSuffixFieldForImport)
                  $scope.importationModalColSize[semanticsKey] /= ++numberOfFields;

                if($scope.dataSeries.semantics.metadata.schema.properties[semanticsKey].hasPrefixFieldForImport)
                  $scope.importationModalColSize[semanticsKey] /= ++numberOfFields;
              }

              $('#importDCPItemsModal').modal('show');
            });
          });
        }, false, ".csv, application/csv");
      };

      $scope.validateImportationMetadata = function() {
        var importationMetadata = {};
        var type = 'toa5';
        for(var i = 0, fieldsLength = $scope.dataSeries.semantics.metadata.form.length; i < fieldsLength; i++) {
          if($scope.defaultValueFilter($scope.dataSeries.semantics.metadata.form[i])) {
            var metadata = {
              field: null,
              defaultValue: null,
              prefix: null,
              suffix: null
            };

            if($scope.importationFields[type][$scope.dataSeries.semantics.metadata.form[i].key] !== undefined && $scope.importationFields[type][$scope.dataSeries.semantics.metadata.form[i].key] !== "") {
              metadata.field = $scope.importationFields[type][$scope.dataSeries.semantics.metadata.form[i].key];

              if($scope.importationFields[type][$scope.dataSeries.semantics.metadata.form[i].key + 'Prefix'] !== undefined && $scope.importationFields[type][$scope.dataSeries.semantics.metadata.form[i].key + 'Prefix'] !== "")
                metadata.prefix = $scope.importationFields[type][$scope.dataSeries.semantics.metadata.form[i].key + 'Prefix'];

              if($scope.importationFields[type][$scope.dataSeries.semantics.metadata.form[i].key + 'Suffix'] !== undefined && $scope.importationFields[type][$scope.dataSeries.semantics.metadata.form[i].key + 'Suffix'] !== "")
                metadata.suffix = $scope.importationFields[type][$scope.dataSeries.semantics.metadata.form[i].key + 'Suffix'];
            } else if($scope.importationFields[type][$scope.dataSeries.semantics.metadata.form[i].key + 'Default'] !== undefined && $scope.importationFields[type][$scope.dataSeries.semantics.metadata.form[i].key + 'Default'] !== "") {
              metadata.defaultValue = $scope.importationFields[type][$scope.dataSeries.semantics.metadata.form[i].key + 'Default'];
            } else {
              MessageBoxService.danger(i18n.__("DCP importation error"), i18n.__("Invalid configuration for the field") + "'" + i18n.__($scope.dataSeries.semantics.metadata.schema.properties[$scope.dataSeries.semantics.metadata.form[i].key].title) + "'");
              $('#importDCPItemsModal').modal('hide');
              return;
            }

            importationMetadata[$scope.dataSeries.semantics.metadata.form[i].key] = metadata;
          }
        }

        $('#importDCPItemsModal').modal('hide');
        executeImportation(importationMetadata, $scope.csvImport.finalData);
      };

      var executeImportation = function(metadata, data) {

        var dcps = [];

        for(var i = 0, dataLength = data.data.length; i < dataLength; i++) {
          var dcp = {};

          for(var j = 0, fieldsLength = $scope.dataSeries.semantics.metadata.form.length; j < fieldsLength; j++) {
            var value = null;

            if(metadata[$scope.dataSeries.semantics.metadata.form[j].key] === undefined && 
            $scope.dataSeries.semantics.metadata.schema.properties[$scope.dataSeries.semantics.metadata.form[j].key].defaultForImport !== undefined) {
              value = $scope.dataSeries.semantics.metadata.schema.properties[$scope.dataSeries.semantics.metadata.form[j].key].defaultForImport;
            } else if(metadata[$scope.dataSeries.semantics.metadata.form[j].key].field !== null) {
              value = data.data[i][metadata[$scope.dataSeries.semantics.metadata.form[j].key].field];
            } else if(metadata[$scope.dataSeries.semantics.metadata.form[j].key].defaultValue !== null) {
              value = metadata[$scope.dataSeries.semantics.metadata.form[j].key].defaultValue;
            }

            if(value !== null && metadata[$scope.dataSeries.semantics.metadata.form[j].key].prefix !== null) {
              value = metadata[$scope.dataSeries.semantics.metadata.form[j].key].prefix + value;
            }

            if(value !== null && metadata[$scope.dataSeries.semantics.metadata.form[j].key].suffix !== null) {
              value += metadata[$scope.dataSeries.semantics.metadata.form[j].key].suffix;
            }

            if(value === null) {
              MessageBoxService.danger(i18n.__("DCP importation error"), i18n.__("Invalid configuration for the field") + " '" + i18n.__($scope.dataSeries.semantics.metadata.schema.properties[$scope.dataSeries.semantics.metadata.form[j].key].title) + "'");
              return;
            }

            if(!validateField(value, type, pattern)) {
              MessageBoxService.danger(i18n.__("DCP importation error"), i18n.__("Invalid value for the field") + " '" + i18n.__($scope.dataSeries.semantics.metadata.schema.properties[$scope.dataSeries.semantics.metadata.form[j].key].title) + "' " + i18n.__("in the line") + " " + (i + 1));
              return;
            }

            dcp[$scope.dataSeries.semantics.metadata.form[j].key] = value;
          }
        }
          deveserassim = {
            active: true,
            alias: "dfg",
            folder: "sdg",
            latitude: -1,
            longitude: 4,
            mask: "dfg",
            projection: 4326,
            timezone: "5"
          }

          metadata = {
            "folder":{  
              "field":"ID_PCD",
              "defaultValue":null,
              "prefix":null,
              "suffix":null
            },
            "mask":{  
              "field":"ID_PCD",
              "defaultValue":null,
              "prefix":null,
              "suffix":null
            },
            "alias":{  
              "field":"ID_PCD",
              "defaultValue":null,
              "prefix":null,
              "suffix":null
            },
            "timezone":{  
              "field":null,
              "defaultValue":"2",
              "prefix":null,
              "suffix":null
            },
            "latitude":{  
              "field":"LatDec",
              "defaultValue":null,
              "prefix":null,
              "suffix":null
            },
            "longitude":{  
              "field":"LonDec\r",
              "defaultValue":null,
              "prefix":null,
              "suffix":null
            },
            "projection":{  
              "field":null,
              "defaultValue":"4326",
              "prefix":null,
              "suffix":null
            }
          }

          exemplodado = {  
            "ID_PCD":"31901",
            "Tipo":"AGROMET",
            "Estacao":"PARNA  S. Divisor ",
            "UF":"AC",
            "LatDec":"-7.443",
            "LonDec\r":"-73.657\r"
          }
        }

        MessageBoxService.success(i18n.__("DCP importation"), i18n.__("Importation executed with success"));
      };
    }],
    link: function(scope, element, attrs, ngModel) {}
  };
});