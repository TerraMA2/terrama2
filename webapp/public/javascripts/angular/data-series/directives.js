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
                    "<select class=\"form-control\" name=\"{{ semantic.key + dataSeries.semantics.code }}\" ng-change=\"verifyDefault(dataSeries.semantics.code, semantic.key);\" ng-model=\"importationFields[dataSeries.semantics.code][semantic.key]\">" +
                      "<option value=\"\">{{ i18n.__('Select a column') }}</option>" +
                      "<option value=\"default\" ng-show=\"dataSeries.semantics.metadata.schema.properties[semantic.key].hasDefaultFieldForImport\">{{ i18n.__('Enter default value') }}</option>" +
                      "<option value=\"empty\" ng-show=\"dataSeries.semantics.metadata.schema.properties[semantic.key].allowEmptyValue\">{{ i18n.__('Leave it empty') }}</option>" +
                      "<option ng-repeat=\"column in csvImport.finalData.header\" ng-init=\"importationFields[dataSeries.semantics.code][semantic.key] = ''\" value=\"{{ column }}\">{{ column }}</option>" +
                    "</select>" +
                  "</div>" +
                  "<div class=\"form-group col-md-{{ importationModalColSize[semantic.key] }}\" ng-show=\"dataSeries.semantics.metadata.schema.properties[semantic.key].hasDefaultFieldForImport && semantic.titleMap\">" +
                    "<label>{{ i18n.__('Default') }}:</label>" +
                    "<select class=\"form-control\" ng-disabled=\"importationFields[dataSeries.semantics.code][semantic.key] != 'default'\" id=\"{{ semantic.key + dataSeries.semantics.code }}Default\" name=\"{{ semantic.key + dataSeries.semantics.code }}Default\" ng-model=\"importationFields[dataSeries.semantics.code][semantic.key + 'Default']\">" +
                      "<option value=\"\">{{ i18n.__('Select a value') }}</option>" +
                      "<option ng-repeat=\"titleMap in semantic.titleMap\" label=\"{{ titleMap.name }}\" value=\"{{ titleMap.value }}\">{{ titleMap.name }}</option>" +
                    "</select>" +
                  "</div>" +
                  "<div class=\"form-group col-md-{{ importationModalColSize[semantic.key] }}\" ng-show=\"dataSeries.semantics.metadata.schema.properties[semantic.key].hasDefaultFieldForImport && !semantic.titleMap\">" +
                    "<label>{{ i18n.__('Default') }}:</label>" +
                    "<input class=\"form-control\" ng-disabled=\"importationFields[dataSeries.semantics.code][semantic.key] != 'default'\" id=\"{{ semantic.key + dataSeries.semantics.code }}Default\" name=\"{{ semantic.key + dataSeries.semantics.code }}Default\" ng-model=\"importationFields[dataSeries.semantics.code][semantic.key + 'Default']\" placeholder=\"{{ i18n.__('Default') }}\" type=\"text\">" +
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

          $scope.verifyDefault = function(code, key) {
            if($scope.importationFields[code][key] != 'default')
              $scope.importationFields[code][key + 'Default'] = '';
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
                $scope.isChecking.value = false;
                $scope.display = true;
                $scope.alertBox.message = err.toString();
                return;
              }

              FileDialog.readAsCSV(input.files[0], $scope.csvImport.delimiterCharacter, $scope.csvImport.hasHeader, function(error, csv) {
                if(error) {
                  $scope.isChecking.value = false;
                  $scope.display = true;
                  $scope.alertBox.message = error.toString();
                  return;
                }

                $scope.$apply(function() {
                  $scope.isChecking.value = true;

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

                  $scope.isChecking.value = false;

                  $('#importDCPItemsModal').modal('show');
                });
              });
            }, false, ".csv, application/csv");
          };

          $scope.validateImportationMetadata = function() {
            $('#importDCPItemsModal').modal('hide');
            $scope.isChecking.value = true;

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

                  if($scope.importationFields[type][key] === "empty") {
                    metadata.defaultValue = "";
                  } else if($scope.importationFields[type][key] !== undefined && $scope.importationFields[type][key] !== "" && $scope.importationFields[type][key] != "default") {
                    metadata.field = $scope.importationFields[type][key];

                    if($scope.importationFields[type][key + 'Prefix'] !== undefined && $scope.importationFields[type][key + 'Prefix'] !== "")
                      metadata.prefix = $scope.importationFields[type][key + 'Prefix'];

                    if($scope.importationFields[type][key + 'Suffix'] !== undefined && $scope.importationFields[type][key + 'Suffix'] !== "")
                      metadata.suffix = $scope.importationFields[type][key + 'Suffix'];
                  } else if($scope.importationFields[type][key + 'Default'] !== undefined && $scope.importationFields[type][key + 'Default'] !== "") {
                    metadata.defaultValue = $scope.importationFields[type][key + 'Default'];
                  } else {
                    MessageBoxService.danger(
                      i18n.__("DCP Import Error"), 
                      i18n.__("Invalid configuration for the field") + " '" + i18n.__($scope.dataSeries.semantics.metadata.schema.properties[key].title) + "'"
                    );
                    $('#importDCPItemsModal').modal('hide');
                    $scope.isChecking.value = false;
                    return;
                  }

                  importationMetadata[key] = metadata;
                }
              }

              executeImportation(importationMetadata, $scope.csvImport.finalData);
            }, 3000);
          };

          var validateImportValue = function(key, titleMap, title, type, pattern, metadata, data, titleMapType, lineNumber) {
            var value = null;
            var error = null;

            if(metadata[key] === undefined && $scope.dataSeries.semantics.metadata.schema.properties[key].defaultForImport !== undefined) {
              value = $scope.dataSeries.semantics.metadata.schema.properties[key].defaultForImport;
            } else {
              if(metadata[key].field !== null) {
                value = data[metadata[key].field];
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

            if(value === null)
              return {
                error: {
                  title: i18n.__("DCP Import Error"),
                  message: i18n.__("Invalid configuration for the field") + " '" + i18n.__(title) + "'"
                },
                value: value
              };

            if(titleMap !== undefined && titleMap !== null)
              type = titleMapType;

            if($scope.fieldHasError(value, type, pattern, titleMap, $scope.dataSeries.semantics.metadata.schema.properties[key].allowEmptyValue))
              return {
                error: {
                  title: i18n.__("DCP Import Error"),
                  message: i18n.__("Invalid value for the field") + " '" + i18n.__(title) + "' " + i18n.__("in the line") + " " + lineNumber
                },
                value: value
              };

            return {
              error: null,
              value: value
            };
          };

          var executeImportation = function(metadata, data) {
            var dcps = [];
            var dcpsObjectTemp = {};
            var warnDuplicatedAlias = false;
            var registersCount = 0;

            for(var i = 0, dataLength = data.data.length; i < dataLength; i++) {
              var dcp = {};
              var uniqueId = UniqueNumber();
              var alias = null;

              var aliasValidateImportResult = validateImportValue('alias', null, $scope.dataSeries.semantics.metadata.schema.properties.alias.title, $scope.dataSeries.semantics.metadata.schema.properties.alias.type, pattern, metadata, data.data[i], null, (i + (data.hasHeader ? 2 : 1)));

              if(aliasValidateImportResult.error !== null) {
                MessageBoxService.danger(aliasValidateImportResult.error.title, aliasValidateImportResult.error.message);
                $scope.isChecking.value = false;
                return;
              } else
                alias = aliasValidateImportResult.value;

              if(!$scope.isAliasValid(alias, $scope.dcpsObject) || !$scope.isAliasValid(alias, dcpsObjectTemp)) {
                if($scope.duplicatedAliasCounter[alias] === undefined)
                  $scope.duplicatedAliasCounter[alias] = 0;
                
                alias = alias + "_" + (++$scope.duplicatedAliasCounter[alias]).toString();

                if(!warnDuplicatedAlias)
                  warnDuplicatedAlias = true;
              }

              if(dcp.active === undefined)
                dcp.active = true;

              for(var j = 0, fieldsLength = $scope.dataSeries.semantics.metadata.form.length; j < fieldsLength; j++) {
                var value = null;
                var key = $scope.dataSeries.semantics.metadata.form[j].key;
                var titleMap = $scope.dataSeries.semantics.metadata.form[j].titleMap;
                var title = $scope.dataSeries.semantics.metadata.schema.properties[key].title;
                var type = $scope.dataSeries.semantics.metadata.schema.properties[key].type;
                var pattern = $scope.dataSeries.semantics.metadata.schema.properties[key].pattern;

                if(key == "alias")
                  value = alias;
                else {
                  var validateImportResult = validateImportValue(key, titleMap, title, type, pattern, metadata, data.data[i], $scope.dataSeries.semantics.metadata.form[j].type, (i + (data.hasHeader ? 2 : 1)));

                  if(validateImportResult.error !== null) {
                    MessageBoxService.danger(validateImportResult.error.title, validateImportResult.error.message);
                    $scope.isChecking.value = false;
                    return;
                  } else
                    value = validateImportResult.value;
                }

                dcp[key + '_pattern'] = pattern;
                dcp[key + '_titleMap'] = titleMap;

                if(dcp[key + '_titleMap'] !== undefined)
                  type = $scope.dataSeries.semantics.metadata.form[j].type;

                dcp = $scope.setHtmlItems(dcp, key, alias, uniqueId, type);

                dcp[key] = value;
              }

              dcp._id = uniqueId;

              dcpsObjectTemp[dcp.alias] = Object.assign({}, dcp);

              var dcpCopy = Object.assign({}, dcp);
              dcpCopy.removeButton = $scope.getRemoveButton(dcp.alias);

              dcps.push(dcpCopy);

              if($scope.isUpdating)
                $scope.insertEditedDcp(dcpCopy._id);

              registersCount++;

              if(registersCount >= 1000) {
                $scope.storageDcps(dcps);
                $scope.addDcpsStorager(dcps);

                registersCount = 0;
                dcps = [];
              }
            }
            
            $scope.dcpsObject = angular.merge($scope.dcpsObject, dcpsObjectTemp);

            if(registersCount > 0) {
              $scope.storageDcps(dcps);
              $scope.addDcpsStorager(dcps);
            }

            // reset form to do not display feedback class
            $scope.forms.parametersForm.$setPristine();
            $scope.isChecking.value = false;

            if(warnDuplicatedAlias)
              MessageBoxService.warning(i18n.__("DCP Import Process"), i18n.__("Import process executed with success, but there were duplicated Alias, check the values"));
            else
              MessageBoxService.success(i18n.__("DCP Import Process"), i18n.__("Import process executed with success"));
          };
        }]
      };
    });

  return moduleName;
});