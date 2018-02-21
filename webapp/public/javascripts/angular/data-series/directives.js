define([], function() {
  var moduleName = "terrama2.dataseries.directives";

  angular.module(moduleName, [])
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
                "<h4 class=\"modal-title\">{{ i18n.__('Select the delimiter character of the CSV and choose if the file has a header') }}</h4>" +
              "</div>" +
              "<div class=\"modal-body\">" +
                "<div class=\"form-group\">" +
                  "<label>{{ i18n.__('Delimiter character') }}</label>" +
                  "<select class=\"form-control\" name=\"delimiterCharacter\" ng-model=\"csvImport.delimiterCharacter\">" +
                    "<option value=\";\">{{ i18n.__('Semicolon') }} (;)</option>" +
                    "<option value=\",\">{{ i18n.__('Comma') }} (,)</option>" +
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
                "<button type=\"button\" class=\"btn btn-default pull-left\" data-dismiss=\"modal\">{{ i18n.__('Close') }}</button>" +
                "<button type=\"button\" class=\"btn btn-primary pull-right\" ng-click=\"selectFileToImport()\">{{ i18n.__('Import') }}</button>" +
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
                "<h4 class=\"modal-title\">{{ i18n.__('Select the fields') }}</h4>" +
              "</div>" +
              "<div class=\"modal-body\">" +
                "<div ng-repeat=\"semantic in dataSeries.semantics.metadata.form | filter: defaultValueFilter\">" +
                  "<div class=\"form-group col-md-{{ importationModalColSize[semantic.key] }}\" ng-show=\"dataSeries.semantics.metadata.schema.properties[semantic.key].hasPrefixFieldForImport\">" +
                    "<label>{{ i18n.__('Prefix') }}:</label>" +
                    "<input class=\"form-control\" id=\"{{ semantic.key + dataSeries.semantics.code }}Prefix\" name=\"{{ semantic.key + dataSeries.semantics.code }}Prefix\" ng-model=\"importationFields[dataSeries.semantics.code][semantic.key + 'Prefix']\" placeholder=\"{{ i18n.__('Prefix') }}\" type=\"text\">" +
                  "</div>" +
                  "<div class=\"form-group col-md-{{ importationModalColSize[semantic.key] }}\">" +
                    "<label>{{ dataSeries.semantics.metadata.schema.properties[semantic.key].title }}:</label>" +
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
                  "<div class=\"form-group col-md-{{ importationModalColSize[semantic.key] }}\" ng-show=\"dataSeries.semantics.metadata.schema.properties[semantic.key].hasSuffixFieldForImport\">" +
                    "<label>{{ i18n.__('Suffix') }}:</label>" +
                    "<input class=\"form-control\" id=\"{{ semantic.key + dataSeries.semantics.code }}Suffix\" name=\"{{ semantic.key + dataSeries.semantics.code }}Suffix\" ng-model=\"importationFields[dataSeries.semantics.code][semantic.key + 'Suffix']\" placeholder=\"{{ i18n.__('Suffix') }}\" type=\"text\">" +
                  "</div>" +
                "</div>"+
                "<hr style=\"border: 1px solid #eee !important;\"/>" +
                "<button type=\"button\" class=\"btn btn-default pull-left\" data-dismiss=\"modal\">{{ i18n.__('Close') }}</button>" +
                "<button type=\"button\" class=\"btn btn-primary pull-right\" ng-click=\"validateImportationMetadata()\">{{ i18n.__('Import') }}</button>" +
                "<div style=\"clear: both;\"></div>" +
              "</div>" +
            "</div>" +
          "</div>" +
        "</div>"
      );

      $templateCache.put(
        "importShapefile.html",
        "<div id=\"shapefile-import-loader\" class=\"hidden\"><img src=\"" + BASE_URL + "images/loader.gif\"/></div>" +
        "<button class=\"btn btn-default\" ng-click=\"openImportShapefileModal()\">{{ i18n.__('Transfer Shapefile') }}</button>" +
        "<div class=\"modal fade\" id=\"shapefileModal\" tabindex=\"-1\" role=\"dialog\" aria-labelledby=\"shapefileModalLabel\">" +
          "<div class=\"modal-dialog modal-md\" role=\"document\">" +
            "<div class=\"modal-content\">" +
              "<div class=\"modal-header\">" +
                "<button type=\"button\" class=\"close\" data-dismiss=\"modal\" aria-label=\"Close\"><span aria-hidden=\"true\">&times;</span></button>" +
                "<h4 class=\"modal-title\" id=\"shapefileModalLabel\">{{ i18n.__('Transfer Shapefile') }}</h4>" +
                "<br/>" +
                "<p><strong>{{ i18n.__('Important!') }}</strong> {{ i18n.__('To upload your shapefile you have to create a .zip compressed file containing all the shapefile files (.shp, .dbf, .shx, etc). The size limit is 300 MB per file, therefore, neither of the shapefile files can be larger than 300 MB.') }}</p>" +
              "</div>" +
              "<div class=\"modal-body\">" +
                "<div class=\"row\">" +
                  "<div class=\"col-md-12\">" +
                    "<div class=\"col-md-4\" ng-if=\"semanticsCode === 'STATIC_DATA-postgis'\">" +
                      "<div class=\"form-group\">" +
                        "<label>{{ i18n.__('SRID') }}</label>" +
                        "<input class=\"form-control\" type=\"text\" ng-model=\"shpImport.srid\"/>" +
                      "</div>" +
                    "</div>" +
                    "<div class=\"col-md-4\" ng-if=\"semanticsCode === 'STATIC_DATA-postgis'\">" +
                      "<div class=\"form-group\">" +
                        "<label>{{ i18n.__('Encoding') }}</label>" +
                        "<select class=\"form-control\" ng-model=\"shpImport.encoding\">" +
                          "<option value=\"latin1\">LATIN1</option>" +
                          "<option value=\"utf8\">UTF-8</option>" +
                        "</select>" +
                      "</div>" +
                    "</div>" +
                    "<div ng-class=\"{ 'col-md-4': semanticsCode === 'STATIC_DATA-postgis', 'col-md-12': semanticsCode === 'STATIC_DATA-ogr' }\">" +
                      "<div class=\"form-group\">" +
                        "<button class=\"btn btn-primary\" style=\"margin-top: 26px;\" type=\"file\" ngf-select=\"uploadShapefile($file, $invalidFiles)\" accept=\"application/zip\" ngf-max-size=\"500MB\" ng-disabled=\"semanticsCode === 'STATIC_DATA-postgis' && shpImport.srid === null\">{{ i18n.__('Select File') }}</button>" +
                      "</div>" +
                    "</div>" +
                  "</div>" +
                "</div>" +
                "<div class=\"row\" ng-show=\"shpImport.error || shpImport.success\">" +
                  "<div class=\"col-md-12\">" +
                    "<hr/>" +
                    "<div class=\"alert alert-danger alert-dismissible\" ng-show=\"shpImport.error\">" +
                      "<button type=\"button\" class=\"close\" ng-click=\"clearShapefileImportError()\" aria-hidden=\"true\">×</button>" +
                      "<h4><i class=\"icon fa fa-ban\"></i> {{ i18n.__('Error!') }}</h4>" +
                      "{{ shpImport.error }}" +
                    "</div>" +
                    "<div class=\"alert alert-success alert-dismissible\" ng-show=\"shpImport.success\">" +
                      "<button type=\"button\" class=\"close\" ng-click=\"closeShapefileSuccessMessage()\" data-dismiss=\"alert\" aria-hidden=\"true\">×</button>" +
                      "<h4><i class=\"icon fa fa-check\"></i> {{ i18n.__('Success!') }}</h4>" +
                      "{{ i18n.__('Shapefile imported with success!') }}" +
                    "</div>" +
                  "</div>" +
                "</div>" +
              "</div>" +
            "</div>" +
          "</div>" +
        "</div>"
      );

      $templateCache.put(
        "importGeotiff.html",
        "<div id=\"geotiff-import-loader\" class=\"hidden\"><img src=\"" + BASE_URL + "images/loader.gif\"/></div>" +
        "<button class=\"btn btn-default\" ng-click=\"openImportGeotiffModal()\">{{ i18n.__('Transfer GeoTIFF') }}</button>" +
        "<div class=\"modal fade\" id=\"geotiffModal\" tabindex=\"-1\" role=\"dialog\" aria-labelledby=\"geotiffModalLabel\">" +
          "<div class=\"modal-dialog modal-md\" role=\"document\">" +
            "<div class=\"modal-content\">" +
              "<div class=\"modal-header\">" +
                "<button type=\"button\" class=\"close\" data-dismiss=\"modal\" aria-label=\"Close\"><span aria-hidden=\"true\">&times;</span></button>" +
                "<h4 class=\"modal-title\" id=\"geotiffModalLabel\">{{ i18n.__('Transfer GeoTIFF') }}</h4>" +
                "<br/>" +
                "<p><strong>{{ i18n.__('Important!') }}</strong> {{ i18n.__('The size limit for the file is 300 MB.') }}</p>" +
              "</div>" +
              "<div class=\"modal-body\">" +
                "<div class=\"row\">" +
                  "<div class=\"col-md-12\">" +
                    "<div class=\"form-group\">" +
                      "<button class=\"btn btn-primary\" style=\"margin-top: 26px;\" type=\"file\" ngf-select=\"uploadGeotiff($file, $invalidFiles)\" accept=\"image/tiff\" ngf-max-size=\"300MB\">{{ i18n.__('Select File') }}</button>" +
                    "</div>" +
                  "</div>" +
                "</div>" +
                "<div class=\"row\" ng-show=\"geotiffImport.error || geotiffImport.success\">" +
                  "<div class=\"col-md-12\">" +
                    "<hr/>" +
                    "<div class=\"alert alert-danger alert-dismissible\" ng-show=\"geotiffImport.error\">" +
                      "<button type=\"button\" class=\"close\" ng-click=\"clearGeotiffImportError()\" aria-hidden=\"true\">×</button>" +
                      "<h4><i class=\"icon fa fa-ban\"></i> {{ i18n.__('Error!') }}</h4>" +
                      "{{ geotiffImport.error }}" +
                    "</div>" +
                    "<div class=\"alert alert-success alert-dismissible\" ng-show=\"geotiffImport.success\">" +
                      "<button type=\"button\" class=\"close\" ng-click=\"closeGeotiffSuccessMessage()\" data-dismiss=\"alert\" aria-hidden=\"true\">×</button>" +
                      "<h4><i class=\"icon fa fa-check\"></i> {{ i18n.__('Success!') }}</h4>" +
                      "{{ i18n.__('GeoTIFF imported with success!') }}" +
                    "</div>" +
                  "</div>" +
                "</div>" +
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
            return $scope.dataSeries.semantics.metadata.schema.properties[item.key] === undefined || $scope.dataSeries.semantics.metadata.schema.properties[item.key].defaultForImport === undefined;
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
                      i18n.__("Invalid configuration for the field") + " '" + $scope.dataSeries.semantics.metadata.schema.properties[key].title + "'"
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
                  message: i18n.__("Invalid configuration for the field") + " '" + title + "'"
                },
                value: value
              };

            if(titleMap !== undefined && titleMap !== null)
              type = titleMapType;

            if($scope.fieldHasError(value, type, pattern, titleMap, $scope.dataSeries.semantics.metadata.schema.properties[key].allowEmptyValue))
              return {
                error: {
                  title: i18n.__("DCP Import Error"),
                  message: i18n.__("Invalid value for the field") + " '" + title + "' " + i18n.__("in the line") + " " + lineNumber
                },
                value: value
              };

            if(key[0] === "projection" || key[0] === "srid") {
              var sridValidationResult = $scope.validateSrid(value);

              if(sridValidationResult) {
                return {
                  error: {
                    title: i18n.__("DCP Import Error"),
                    message: i18n.__("Invalid value for the field") + " '" + title + "' " + i18n.__("in the line") + " " + lineNumber
                  },
                  value: value
                };
              }
            }

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
                alias = (typeof aliasValidateImportResult.value === "string" ? aliasValidateImportResult.value.trim() : aliasValidateImportResult.value);

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

                dcp[key] = (typeof value === "string" ? value.trim() : value);
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
    })
    
    .directive('terrama2ShapefileImporter', function() {
      return {
        restrict: 'EA',
        templateUrl: 'importShapefile.html',
        controller: ['$scope', 'MessageBoxService', 'Upload', '$timeout', 'i18n', '$http', function($scope, MessageBoxService, Upload, $timeout, i18n, $http) {
          $scope.shpImport = {
            srid: null,
            encoding: "latin1",
            error: null,
            success: false
          };

          $scope.openImportShapefileModal = function() {
            $scope.shpImport.srid = null;
            $scope.shpImport.encoding = "latin1";

            $scope.clearShapefileImportError();
            $scope.closeShapefileSuccessMessage();

            if(($scope.semanticsCode === 'STATIC_DATA-ogr' && $scope.model['mask'] !== undefined && $scope.model['mask'] !== null && $scope.model['mask'] != "") || ($scope.semanticsCode === 'STATIC_DATA-postgis' && $scope.model['table_name'] !== undefined && $scope.model['table_name'] !== null && $scope.model['table_name'] != ""))
              $("#shapefileModal").modal();
            else
              MessageBoxService.danger(i18n.__("Error"), i18n.__($scope.semanticsCode === 'STATIC_DATA-ogr' ? "Enter the file name!" : "Enter the table name!"));
          };

          $scope.uploadShapefile = function(file, errFiles) {
            $scope.clearShapefileImportError();
            $scope.closeShapefileSuccessMessage();

            if(file) {
              var performUpload = function() {
                file.upload = Upload.upload({
                  url: BASE_URL + 'import-shapefile',
                  file: file,
                  data: {
                    semantics: $scope.semanticsCode,
                    srid: $scope.shpImport.srid,
                    encoding: $scope.shpImport.encoding,
                    tableName: $scope.model['table_name'],
                    mask: $scope.model['mask'],
                    dataProviderId: $scope.dataSeries.data_provider_id
                  }
                });
  
                file.upload.then(function(response) {
                  $timeout(function () {
                    if(!$("#shapefile-import-loader").hasClass("hidden"))
                      $("#shapefile-import-loader").addClass("hidden");
  
                    if(response.data.error) $scope.shpImport.error = i18n.__(response.data.error);
                    else $scope.shpImport.success = true;
                  });
                }, function(response) {
                  if(response.status > 0)
                    $scope.shpImport.error = response.status + ': ' + response.data;
                });
              };

              if($("#shapefile-import-loader").hasClass("hidden"))
                $("#shapefile-import-loader").removeClass("hidden");

              if($scope.semanticsCode === 'STATIC_DATA-postgis') {
                $http.get(BASE_URL + "configuration/validate-srid/" + $scope.shpImport.srid).then(function(sridValidator) {
                  if(sridValidator.data.error) {
                    $scope.shpImport.error = i18n.__(sridValidator.data.error);

                    if(!$("#shapefile-import-loader").hasClass("hidden"))
                      $("#shapefile-import-loader").addClass("hidden");
                  } else {
                    performUpload();
                  }
                });
              } else {
                performUpload();
              }
            }   
          };

          $scope.clearShapefileImportError = function() {
            $scope.shpImport.error = null;
          };

          $scope.closeShapefileSuccessMessage = function() {
            $scope.shpImport.success = false;
          };
        }]
      };
    })
    
    .directive('terrama2GeotiffImporter', function() {
      return {
        restrict: 'EA',
        templateUrl: 'importGeotiff.html',
        controller: ['$scope', 'MessageBoxService', 'Upload', '$timeout', 'i18n', function($scope, MessageBoxService, Upload, $timeout, i18n) {
          $scope.geotiffImport = {
            error: null,
            success: false
          };

          $scope.openImportGeotiffModal = function() {
            $scope.clearGeotiffImportError();
            $scope.closeGeotiffSuccessMessage();

            if($scope.model['mask'] !== undefined && $scope.model['mask'] !== null && $scope.model['mask'] != "")
              $("#geotiffModal").modal();
            else
              MessageBoxService.danger(i18n.__("Error"), i18n.__("Enter the file name!"));
          };

          $scope.uploadGeotiff = function(file, errFiles) {
            $scope.clearGeotiffImportError();
            $scope.closeGeotiffSuccessMessage();

            if(file) {
              if($("#geotiff-import-loader").hasClass("hidden"))
                $("#geotiff-import-loader").removeClass("hidden");

              file.upload = Upload.upload({
                url: BASE_URL + 'import-geotiff',
                file: file,
                data: {
                  mask: $scope.model['mask'],
                  dataProviderId: $scope.dataSeries.data_provider_id
                }
              });

              file.upload.then(function(response) {
                $timeout(function () {
                  if(!$("#geotiff-import-loader").hasClass("hidden"))
                    $("#geotiff-import-loader").addClass("hidden");

                  if(response.data.error) $scope.geotiffImport.error = i18n.__(response.data.error);
                  else $scope.geotiffImport.success = true;
                });
              }, function(response) {
                if(response.status > 0)
                  $scope.geotiffImport.error = response.status + ': ' + response.data;
              });
            }   
          };

          $scope.clearGeotiffImportError = function() {
            $scope.geotiffImport.error = null;
          };

          $scope.closeGeotiffSuccessMessage = function() {
            $scope.geotiffImport.success = false;
          };
        }]
      };
    });

  return moduleName;
});