define(["TerraMA2WebApp/common/services/index", "TerraMA2WebApp/alert-box/app"], function(commonServiceModule, alertBoxModule) {
  var moduleName = "terrama2.dataseries.directives";

  angular.module(moduleName, [commonServiceModule, alertBoxModule])
    .run(["$templateCache", function($templateCache) {
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
                    "<select class=\"form-control\" name=\"{{ semantic.key + dataSeries.semantics.code }}\" ng-model=\"importationFields[dataSeries.semantics.code][semantic.key]\">" +
                      "<option value=\"\">{{ i18n.__('Select a column') }}</option>" +
                      "<option ng-repeat=\"column in csvImport.finalData.header\" ng-init=\"importationFields[dataSeries.semantics.code][semantic.key] = ''\" value=\"{{ column }}\">{{ column }}</option>" +
                    "</select>" +
                  "</div>" +
                  "<div class=\"form-group col-md-{{ importationModalColSize[semantic.key] }}\" ng-show=\"dataSeries.semantics.metadata.schema.properties[semantic.key].hasDefaultFieldForImport && semantic.titleMap\">" +
                    "<label>{{ i18n.__('Default') }}:</label>" +
                    "<select class=\"form-control\" id=\"{{ semantic.key + dataSeries.semantics.code }}Default\" name=\"{{ semantic.key + dataSeries.semantics.code }}Default\" ng-model=\"importationFields[dataSeries.semantics.code][semantic.key + 'Default']\">" +
                      "<option ng-repeat=\"titleMap in semantic.titleMap\" label=\"{{ titleMap.name }}\" value=\"{{ titleMap.value }}\">{{ titleMap.name }}</option>" +
                    "</select>" +
                  "</div>" +
                  "<div class=\"form-group col-md-{{ importationModalColSize[semantic.key] }}\" ng-show=\"dataSeries.semantics.metadata.schema.properties[semantic.key].hasDefaultFieldForImport && !semantic.titleMap\">" +
                    "<label>{{ i18n.__('Default') }}:</label>" +
                    "<input class=\"form-control\" id=\"{{ semantic.key + dataSeries.semantics.code }}Default\" name=\"{{ semantic.key + dataSeries.semantics.code }}Default\" ng-model=\"importationFields[dataSeries.semantics.code][semantic.key + 'Default']\" placeholder=\"{{ i18n.__('Default') }}\" type=\"text\">" +
                  "</div>" +
                  "<div class=\"form-group col-md-{{ importationModalColSize[semantic.key] }}\" ng-show=\"dataSeries.semantics.metadata.schema.properties[semantic.key].hasPrefixFieldForImport\">" +
                    "<label>{{ i18n.__('Prefix') }}:</label>" +
                    "<input class=\"form-control\" id=\"{{ semantic.key + dataSeries.semantics.code }}Prefix\" name=\"{{ semantic.key + dataSeries.semantics.code }}Prefix\" ng-model=\"importationFields[dataSeries.semantics.code][semantic.key + 'Prefix']\" placeholder=\"{{ i18n.__('Prefix') }}\" type=\"text\">" +
                  "</div>" +
                  "<div class=\"form-group col-md-{{ importationModalColSize[semantic.key] }}\" ng-show=\"dataSeries.semantics.metadata.schema.properties[semantic.key].hasSuffixFieldForImport\">" +
                    "<label>{{ i18n.__('Suffix') }}:</label>" +
                    "<input class=\"form-control\" id=\"{{ semantic.key + dataSeries.semantics.code }}Suffix\" name=\"{{ semantic.key + dataSeries.semantics.code }}Suffix\" ng-model=\"importationFields[dataSeries.semantics.code][semantic.key + 'Suffix']\" placeholder=\"{{ i18n.__('Suffix') }}\" type=\"text\">" +
                  "</div>" +
                "</div>"+
                "<hr style=\"border: 1px solid #eee !important;\"/>" +
                "<button type=\"button\" class=\"btn btn-default pull-left\" data-dismiss=\"modal\">Close</button>" +
                "<button type=\"button\" class=\"btn btn-primary pull-right\" ng-click=\"validateImportationMetadata()\">Import</button>" +
                "<div style=\"clear: both;\"></div>" +
              "</div>" +
            "</div>" +
          "</div>" +
        "</div>" +
        "<div class=\"overlay\" ng-show=\"isChecking\">" +
          "<i class=\"fa fa-refresh fa-spin\"></i>" +
        "</div>"
      );
    }])
    .directive('terrama2DcpImporter', function() {
      return {
        restrict: 'EA',
        templateUrl: 'modals.html',
        controller: ['$scope', '$timeout', 'FileDialog', 'i18n', 'MessageBoxService', 'UniqueNumber', function($scope, $timeout, FileDialog, i18n, MessageBoxService, UniqueNumber) {
          $scope.csvImport = {};
          $scope.importationFields = {};

          $scope.defaultValueFilter = function(item) {
            return $scope.dataSeries.semantics.metadata.schema.properties[item.key].defaultForImport === undefined;
          };

          $scope.openImportParametersModal = function() {
            $('#importParametersModal').modal('show');
          };

          $scope.selectFileToImport = function() {
            $('#importParametersModal').modal('hide');

            $scope.isChecking = true;
            
            FileDialog.openFile(function(err, input) {
              if(err) {
                $scope.display = true;
                $scope.alertBox.message = err.toString();
                return;
              }

              FileDialog.readAsCSV(input.files[0], $scope.csvImport.delimiterCharacter, $scope.csvImport.hasHeader, function(error, csv) {
                $scope.$apply(function() {
                  if(error) {
                    setError(error);
                    console.log(error);
                    $scope.isChecking = false;
                    return;
                  }

                  $scope.csvImport.finalData = csv;
                  $scope.importationModalColSize = {};
                  var patternColSize = 12;

                  for(var i = 0, semanticsLength = $scope.dataSeries.semantics.metadata.form.length; i < semanticsLength; i++) {
                    if($scope.defaultValueFilter($scope.dataSeries.semantics.metadata.form[i])) {
                      var numberOfFields = 1;
                      var semanticsKey = $scope.dataSeries.semantics.metadata.form[i].key;
                      $scope.importationModalColSize[semanticsKey] = patternColSize;

                      if($scope.dataSeries.semantics.metadata.schema.properties[semanticsKey].hasDefaultFieldForImport)
                        $scope.importationModalColSize[semanticsKey] = patternColSize / ++numberOfFields;

                      if($scope.dataSeries.semantics.metadata.schema.properties[semanticsKey].hasSuffixFieldForImport)
                        $scope.importationModalColSize[semanticsKey] = patternColSize / ++numberOfFields;

                      if($scope.dataSeries.semantics.metadata.schema.properties[semanticsKey].hasPrefixFieldForImport)
                        $scope.importationModalColSize[semanticsKey] = patternColSize / ++numberOfFields;
                    }
                  }

                  $scope.isChecking = false;

                  $('#importDCPItemsModal').modal('show');
                });
              });
            }, false, ".csv, application/csv");
          };

          $scope.validateImportationMetadata = function() {
            $('#importDCPItemsModal').modal('hide');
            $scope.isChecking = true;

            $timeout(function() {
              var importationMetadata = {};
              var type = $scope.dataSeries.semantics.code;

              for(var i = 0, fieldsLength = $scope.dataSeries.semantics.metadata.form.length; i < fieldsLength; i++) {
                if($scope.defaultValueFilter($scope.dataSeries.semantics.metadata.form[i])) {
                  var key = $scope.dataSeries.semantics.metadata.form[i].key;
                  var metadata = {
                    field: null,
                    defaultValue: null,
                    prefix: null,
                    suffix: null
                  };

                  if($scope.importationFields[type][key] !== undefined && $scope.importationFields[type][key] !== "") {
                    metadata.field = $scope.importationFields[type][key];

                    if($scope.importationFields[type][key + 'Prefix'] !== undefined && $scope.importationFields[type][key + 'Prefix'] !== "")
                      metadata.prefix = $scope.importationFields[type][key + 'Prefix'];

                    if($scope.importationFields[type][key + 'Suffix'] !== undefined && $scope.importationFields[type][key + 'Suffix'] !== "")
                      metadata.suffix = $scope.importationFields[type][key + 'Suffix'];
                  } else if($scope.importationFields[type][key + 'Default'] !== undefined && $scope.importationFields[type][key + 'Default'] !== "") {
                    metadata.defaultValue = $scope.importationFields[type][key + 'Default'];
                  } else {
                    MessageBoxService.danger(
                      i18n.__("DCP importation error"), 
                      i18n.__("Invalid configuration for the field") + "'" + i18n.__($scope.dataSeries.semantics.metadata.schema.properties[key].title) + "'"
                    );
                    $('#importDCPItemsModal').modal('hide');
                    $scope.isChecking = false;
                    return;
                  }

                  importationMetadata[key] = metadata;
                }
              }

              executeImportation(importationMetadata, $scope.csvImport.finalData);
            }, 3000);
          };

          var executeImportation = function(metadata, data) {
            var dcps = [];

            for(var i = 0, dataLength = data.data.length; i < dataLength; i++) {
              var dcp = {};
              var uniqueId = UniqueNumber();
              var currentIndex = $scope.dcpsCurrentIndex.value++;

              for(var j = 0, fieldsLength = $scope.dataSeries.semantics.metadata.form.length; j < fieldsLength; j++) {
                var value = null;
                var key = $scope.dataSeries.semantics.metadata.form[j].key;
                var titleMap = $scope.dataSeries.semantics.metadata.form[j].titleMap;
                var title = $scope.dataSeries.semantics.metadata.schema.properties[key].title;
                var type = $scope.dataSeries.semantics.metadata.schema.properties[key].type;
                var pattern = $scope.dataSeries.semantics.metadata.schema.properties[key].pattern;

                if(metadata[key] === undefined && $scope.dataSeries.semantics.metadata.schema.properties[key].defaultForImport !== undefined) {
                  value = $scope.dataSeries.semantics.metadata.schema.properties[key].defaultForImport;
                } else {
                  if(metadata[key].field !== null) {
                    value = data.data[i][metadata[key].field];
                  } else if(metadata[key].defaultValue !== null) {
                    value = metadata[key].defaultValue;
                  }

                  if(value !== null && metadata[key].prefix !== null) {
                    value = metadata[key].prefix + value;
                  }

                  if(value !== null && metadata[key].suffix !== null) {
                    value += metadata[key].suffix;
                  }
                }

                if(value === null) {
                  MessageBoxService.danger(i18n.__("DCP importation error"), i18n.__("Invalid configuration for the field") + " '" + i18n.__(title) + "'");
                  $scope.isChecking = false;
                  return;
                }

                if(titleMap !== undefined) {
                  type = $scope.dataSeries.semantics.metadata.form[j].type;
                }

                if(fieldHasError(value, type, pattern, titleMap)) {
                  MessageBoxService.danger(
                    i18n.__("DCP importation error"),
                    i18n.__("Invalid value for the field") + " '" + i18n.__(title) + "' " + i18n.__("in the line") + " " + (i + (data.hasHeader ? 2 : 1))
                  );
                  $scope.isChecking = false;
                  return;
                }

                if($scope.isBoolean(value)) {
                  dcp[key + '_html'] = "<span class=\"dcps-table-span\"><input type=\"checkbox\" ng-model=\"dcpsObject['" + currentIndex.toString() + "']['" + key + "']\"></span>";
                } else {
                  dcp[key + '_html'] = "<span class=\"dcps-table-span\" editable-text=\"dcpsObject['" + currentIndex.toString() + "']['" + key + "']\">{{ dcpsObject['" + currentIndex.toString() + "']['" + key + "'] }}</span>";
                }

                dcp[key] = value;
              }

              dcp._id = uniqueId;
              dcp.viewId = currentIndex;
              $scope.dcps.push(Object.assign({}, dcp));
              $scope.dcpsObject[dcp.viewId] = Object.assign({}, dcp);

              var dcpCopy = Object.assign({}, dcp);
              dcpCopy.removeButton = "<button class=\"btn btn-danger removeDcpBtn\" ng-click=\"removePcdById(" + dcp.viewId + ")\" style=\"height: 21px; padding: 1px 4px 1px 4px; font-size: 13px;\">" + i18n.__("Remove") + "</button>";

              dcps.push(dcpCopy);
            }

            $scope.storageDcps(dcps);
            $scope.addDcpsStorager(dcps);

            // reset form to do not display feedback class
            $scope.forms.parametersForm.$setPristine();
            $scope.isChecking = false;
            MessageBoxService.success(i18n.__("DCP importation"), i18n.__("Importation executed with success"));
          };

          var fieldHasError = function(value, type, pattern, titleMap) {
            var error = false;

            switch(type) {
              case "number":
                error = isNaN(value);
                break;
              case "boolean":
                error = (typeof value !== "boolean") && value !== "true" && value !== "false";
                break;
              case "select":
                error = true;

                for(var i = 0, titleMapLength = titleMap.length; i < titleMapLength; i++) {
                  if(titleMap[i].value === value) {
                    error = false;
                    break;
                  }
                }

                break;
              default:
                error = false;
            }

            if(!error && pattern !== undefined) {
              var regex = new RegExp(pattern);
              var error = !regex.test(value);
            }

            return error;
          };
        }]
      };
    });

  return moduleName;
});