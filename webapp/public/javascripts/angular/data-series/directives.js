define([], function() {
  var moduleName = "terrama2.dataseries.directives";

  angular.module(moduleName, [])
    .run(["$templateCache", function($templateCache) {
      $templateCache.put(
        "modals.html",
        "<div ng-if=\"!isCemadenType()\">" +
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
          "</div>" +
        "</div>" +
        "<div id=\"importDCPCemadenItemsModal\" ng-if=\"isCemadenType()\" class=\"modal fade\">" +
          "<div class=\"modal-dialog\">" +
            "<div class=\"modal-content\">" +
              "<div class=\"modal-header\">" +
                "<button type=\"button\" class=\"close\" data-dismiss=\"modal\" aria-label=\"Close\">" +
                  "<span aria-hidden=\"true\">×</span>" +
                "</button>" +
                "<h4 class=\"modal-title\">{{ i18n.__('Select the fields') }}</h4>" +
              "</div>" +
              "<div class=\"modal-body\">" +
                "<div class=\"checkbox\" ng-repeat=\"field in cemadenFields\">" +
                  "<label><input type=\"checkbox\" value=\"\" ng-model=\"field.active\" ng-disabled=\"!field.editable\">{{ field.name }}</label>" +
                "</div>" +
                "<hr style=\"border: 1px solid #eee !important;\"/>" +
                "<button type=\"button\" class=\"btn btn-default pull-left\" data-dismiss=\"modal\">{{ i18n.__('Close') }}</button>" +
                "<button type=\"button\" class=\"btn btn-primary pull-right\" ng-click=\"validateCemaden()\">{{ i18n.__('Import') }}</button>" +
                "<div style=\"clear: both;\"></div>" +
              "</div>" +
            "</div>" +
          "</div>" +
        "</div>"
      );

      $templateCache.put(
        "importShapefile.html",
        "<div id=\"shapefile-import-loader\" class=\"hidden\"><img src=\"" + BASE_URL + "images/loader.gif\"/></div>" +
        "<p style=\"color:red;margin-top:-15px\">{{ messageError }}</p>" +
        "<button id=\"button_file\" class=\"btn btn-default\" ng-show=\"showButton\" ng-click=\"openImportShapefileModal()\">{{ i18n.__('Transfer Shapefile') }}</button>" +
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
        "</div>" + // Modal of File Explorer
        "<div class=\"modal fade\" id=\"filesExplorerModal\" tabindex=\"-1\" role=\"dialog\" aria-labelledby=\"filesExplorerModalLabel\">" +
          "<div class=\"modal-dialog modal-md\" role=\"document\">" +
            "<div class=\"modal-content\">" +
              "<div class=\"holder-div\" ng-show=\"loadingDirectories\"></div>" +
              "<div class=\"modal-header\">" +
                "<button type=\"button\" class=\"close\" data-dismiss=\"modal\" aria-label=\"Close\"><span aria-hidden=\"true\">&times;</span></button>" +
                "<h4 class=\"modal-title\" ng-if=\"!showButton\" id=\"dataSeriesModalLabel\">{{ i18n.__('Select the File') }}</h4>" +
                "<h4 class=\"modal-title\" ng-if=\"showButton\" id=\"dataSeriesModalLabel\">{{ i18n.__('Select the Directory') }}</h4>" +
              "</div>" +
              "<div class=\"modal-body\">" +
                "<div>" +
                  "<span style=\"font-size: 16px;\">{{ i18n.__('TerraMA² data directory') + ': ' + terrama2DefaultFilePath }}</span>" +
                  "<hr/>" +
                "</div>" +
                "<div ng-show=\"!rootDirectories.children || rootDirectories.children.length == 0\">{{ i18n.__('No directories to show.') }}</div>" +
                "<script type=\"text/ng-template\" id=\"files-explorer.html\">" +
                  "<div class=\"name-div\" ng-click=\"setDirectoryStatus(directory.fullPath)\" ng-class=\"{ 'selected-directory': directory.fullPath == selectedDirectory }\">" +
                    "<i class=\"fa fa-folder\" ng-show=\"pathLoading != directory.fullPath && !directory.childrenVisible\"></i>" +
                    "<i class=\"fa fa-folder-open\" ng-show=\"pathLoading != directory.fullPath && directory.childrenVisible\"></i>" +
                    "<img ng-show=\"pathLoading == directory.fullPath\" src=\"" + BASE_URL + "images/loader.gif\">" +
                    "{{ directory.name }}" +
                  "</div>" +
                  "<ul class=\"file-explorer-ul\" ng-if=\"directory.children && directory.children.length > 0 && directory.childrenVisible\">" +
                    "<li ng-repeat=\"directory in directory.children\" ng-include=\"'files-explorer.html'\"></li>" +
                  "</ul>" +
                "</script>" +
                "<ul class=\"file-explorer-ul\">" +
                  "<li ng-repeat=\"directory in rootDirectories.children\" ng-include=\"'files-explorer.html'\"></li>" +
                "</ul>" +
              "</div>" +
              "<div class=\"modal-footer\">" +
                "<button type=\"button\" class=\"btn btn-primary\" data-dismiss=\"modal\">{{ i18n.__('Cancel') }} </button>" +
                "<button type=\"button\" class=\"btn btn-primary\" data-dismiss=\"modal\" ng-click=\"selectPath()\">{{ i18n.__('Select') }}</button>" +
              "</div>" +
            "</div>" +
          "</div>" +
        "</div>"
      );

      $templateCache.put(
        "importGeotiff.html",
        "<div id=\"geotiff-import-loader\" class=\"hidden\"><img src=\"" + BASE_URL + "images/loader.gif\"/></div>" +
        "<p style=\"color:red;margin-top:-15px\">{{ messageError }}</p>" +
        "<button class=\"btn btn-default\" ng-show=\"showButton\" ng-click=\"openImportGeotiffModal()\">{{ i18n.__('Transfer GeoTIFF') }}</button>" +
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
        "</div>" + // Modal of File Explorer
        "<div class=\"modal fade\" id=\"filesExplorerModal\" tabindex=\"-1\" role=\"dialog\" aria-labelledby=\"filesExplorerModalLabel\">" +
          "<div class=\"modal-dialog modal-md\" role=\"document\">" +
            "<div class=\"modal-content\">" +
              "<div class=\"holder-div\" ng-show=\"loadingDirectories\"></div>" +
              "<div class=\"modal-header\">" +
                "<button type=\"button\" class=\"close\" data-dismiss=\"modal\" aria-label=\"Close\"><span aria-hidden=\"true\">&times;</span></button>" +
                "<h4 class=\"modal-title\" ng-if=\"!showButton\" id=\"dataSeriesModalLabel\">{{ i18n.__('Select the File') }}</h4>" +
                "<h4 class=\"modal-title\" ng-if=\"showButton\" id=\"dataSeriesModalLabel\">{{ i18n.__('Select the Directory') }}</h4>" +
              "</div>" +
              "<div class=\"modal-body\">" +
                "<div>" +
                  "<span style=\"font-size: 16px;\">{{ i18n.__('TerraMA² data directory') + ': ' + terrama2DefaultFilePath }}</span>" +
                  "<hr/>" +
                "</div>" +
                "<div ng-show=\"!rootDirectories.children || rootDirectories.children.length == 0\">{{ i18n.__('No directories to show.') }}</div>" +
                "<script type=\"text/ng-template\" id=\"files-explorer.html\">" +
                  "<div class=\"name-div\" ng-click=\"setDirectoryStatus(directory.fullPath)\" ng-class=\"{ 'selected-directory': directory.fullPath == selectedDirectory }\">" +
                    "<i class=\"fa fa-folder\" ng-show=\"pathLoading != directory.fullPath && !directory.childrenVisible\"></i>" +
                    "<i class=\"fa fa-folder-open\" ng-show=\"pathLoading != directory.fullPath && directory.childrenVisible\"></i>" +
                    "<img ng-show=\"pathLoading == directory.fullPath\" src=\"" + BASE_URL + "images/loader.gif\">" +
                    "{{ directory.name }}" +
                  "</div>" +
                  "<ul class=\"file-explorer-ul\" ng-if=\"directory.children && directory.children.length > 0 && directory.childrenVisible\">" +
                    "<li ng-repeat=\"directory in directory.children\" ng-include=\"'files-explorer.html'\"></li>" +
                  "</ul>" +
                "</script>" +
                "<ul class=\"file-explorer-ul\">" +
                  "<li ng-repeat=\"directory in rootDirectories.children\" ng-include=\"'files-explorer.html'\"></li>" +
                "</ul>" +
              "</div>" +
              "<div class=\"modal-footer\">" +
                "<button type=\"button\" class=\"btn btn-primary\" data-dismiss=\"modal\">{{ i18n.__('Cancel') }} </button>" +
                "<button type=\"button\" class=\"btn btn-primary\" data-dismiss=\"modal\" ng-click=\"selectPath()\">{{ i18n.__('Select') }}</button>" +
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
        controller: ['$scope', '$timeout', 'FileDialog', 'i18n', 'MessageBoxService', 'UniqueNumber', 'CemadenService', function($scope, $timeout, FileDialog, i18n, MessageBoxService, UniqueNumber, CemadenService) {
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
            if (!$scope.isCemadenType())
              $('#importParametersModal').modal('show');
            else
              if ($scope.model.state && $scope.model.state.length !== 0 &&
                  $scope.model.station)
                $scope.selectFileToImport();
              else
                MessageBoxService.danger(i18n.__("Import DCP"), i18n.__("The fields Station and State are required."));
          };

          /**
           * Fill DCP cemaden in Parameters - GUI
           */
          function validateCemaden() {
            // Resetting
            $scope.cleanDCPS();

            $scope.isChecking.value = true;
            $('#importDCPCemadenItemsModal').modal('hide');

            let keys = $scope.cemadenFields.filter(field => field.active).map(field => field.name);
            keys.push("projection");

            // Checking if already alias in keys
            if (!keys.includes("alias"))
              keys.push("alias");

            keys.push("active");

            $scope.setTableFields(keys);

            let dcpsOutput = [];
            let dcpsObjectTemp = {};

            $scope.dcpsCemaden.forEach(dcp => {
              const uniqueId = UniqueNumber();

              let copyDcp = {};

              for(let key of keys) {
                const alias = dcp.alias;
                copyDcp[key] = dcp[key];
                copyDcp.alias = alias;

                // const value = alias;
                const value = dcp[key];
                let type = 'string';

                if (angular.isNumber(value)) {
                  type = 'number';
                }

                const fakeField = {
                  type,
                  title: key,
                  allowEmptyValue: false
                };

                const fakeFormField = {
                  key
                };
                $scope.dataSeries.semantics.metadata.schema.properties[key] = fakeField;
                $scope.dataSeries.semantics.metadata.form.push(fakeFormField);

                // Setting projection from semantics metadata
                dcp.projection = parseInt($scope.dataSeries.semantics.metadata.metadata.srid);
                // Setting dcp as active dcp
                copyDcp.active = true;

                copyDcp = $scope.setHtmlItems(copyDcp, key, alias, uniqueId, type);
              }

              copyDcp._id = uniqueId;

              dcpsObjectTemp[copyDcp.alias] = Object.assign({}, copyDcp);

              // let dcpCopy = Object.assign({}, dcp);
              copyDcp.removeButton = $scope.getRemoveButton(dcp.alias);

              dcpsOutput.push(copyDcp);
            });

            $scope.dcpsObject = angular.merge($scope.dcpsObject, dcpsObjectTemp);

            $scope.storageDcps(dcpsOutput);
            $scope.addDcpsStorager(dcpsOutput);

            $scope.forms.parametersForm.$setPristine();
            $scope.isChecking.value = false;

            /*
              This statement is important due datatables is performing html changes. In this way,
              we should wait for angular digest cycle in order to work properly.

              Execute Create Table
            */
            $timeout(() => $scope.createDataTable());
          }

          $scope.validateCemaden = validateCemaden;

          $scope.selectFileToImport = function() {
            $('#importParametersModal').modal('hide');

            $scope.isChecking.value = true;

            if ($scope.isCemadenType()) {
              const dataProviderId = $scope.dataSeries.data_provider_id;
              const stationId = $scope.model.station;
              if (!stationId)
                return MessageBoxService.danger(i18n.__("Import DCP"), i18n.__("No station selected"));

              // Retrieves all DCP cemaden from state and keep in cache
              return CemadenService.listDCP($scope.model.state.map(state => state.id), dataProviderId, stationId)
                .then(dcps => {

                  if (dcps.length === 0)
                    throw new Error("No dcp found");

                  if (!$scope.dataSeries.semantics.metadata.metadata.static_properties)
                    throw new Error("Something is wrong with Cemaden type. No static properties set. Contact System Administrator");

                  // Setting cache for DCP Cemaden
                  $scope.dcpsCemaden = dcps;

                  // Retrieving Cemaden default fields
                  const staticFields = $scope.dataSeries.semantics.metadata.metadata.static_properties;

                  // Setting Cemaden Fields to select in GUI
                  let fields = [];

                  // Setting default fields
                  staticFields.split(",")
                    .forEach(fieldName => fields.push({ name: fieldName, active: true, editable: false }));

                  // Retrieving all properties from a DCP Cemaden
                  const keys = Object.keys(dcps[0]);

                  // Setting extra fields (excluding defaults)
                  keys.forEach(key => {
                    if (!staticFields.includes(key))
                      fields.push({ name: key, active: false, editable: true });
                  });

                  $scope.cemadenFields = fields;

                  $scope.isChecking.value = false;

                  $('#importDCPCemadenItemsModal').modal('show');
                })
                .catch(err => {
                  $scope.isChecking.value = false;
                  MessageBoxService.danger("Cemaden", i18n.__("Could not retrive Cemaden DCP due ") + err.toString());
                });
            }

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

              if ($scope.dataSeries.semantics.driver !== 'DCP-json_cemaden') {
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
              }

              if(dcp.active === undefined)
                dcp.active = true;

              for(var j = 0, fieldsLength = $scope.dataSeries.semantics.metadata.form.length; j < fieldsLength; j++) {
                var value = null;
                var key = $scope.dataSeries.semantics.metadata.form[j].key;

                if (key instanceof Array)
                  key = key[0];

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
        controller: ['$scope', 'MessageBoxService', 'DataProviderService','Upload', '$timeout', 'i18n', '$q', '$http', function($scope, MessageBoxService, DataProviderService,Upload, $timeout, i18n, $q, $http) {
          $scope.shpImport = {
            srid: 4326,
            encoding: "latin1",
            error: null,
            success: false
          };
          var title = "Data Server Registration";
          $scope.rootDirectories = {
            children: []
          };

          $scope.uploaded = false;
          $scope.messageError = "";

          const modal = $("#shapefileModal");
          // Remove previous listenet to avoid call function save twice
          modal.off();

          /**
           * Event listener when upload shapefile call save method automatically
           *
           * @todo Review this directive and refactory to work as component style, attaching function callback on
           */
          modal.on('hidden.bs.modal', () => {
            if ($scope.uploaded) {

              var tableName = $scope.model['table_name'];
              var provider = $scope.dataSeries.data_provider_id;

              DataProviderService.listPostgisObjects({providerId: provider, objectToGet: "column", tableName})
              .then(response=>{
                if (response.data.status == 400){
                  return result.reject(response.data);
                }
                response.data.data.map(element => {

                  var visibleOp = true;
                  var aliasOp = element.column_name;

                  $scope.model.attributes.push(
                    {
                      name:element.column_name,
                      visible: visibleOp,
                      alias: aliasOp
                    }
                  );

                });

                $scope.save();
              });
              
            }
          });

          // Inicio File Explorer
          var dataProviders = {};
          var hasFile = false;
          var fileName = "";
          var extension = "";
          var pathSelected = "";

          DataProviderService.listAll({ id:$scope.dataSeries.data_provider_id })
          .then((data) =>{
            dataProviders=data.data;
          });
          $("#saveStaticData").on("click",()=>{
            let dp  = dataProviders.filter(function(element) {
              return element.id == $scope.dataSeries.data_provider_id;
            });

            let semanticsCode = $scope.dataSeries.semantics.code;
            switch(semanticsCode){
              case "STATIC_DATA-ogr":
                fileName = $scope.model['mask'].split('/').slice(-1).pop();
                if(!fileName.endsWith(".shp") || !fileName.endsWith(".zip")){
                  $scope.messageError = i18n.__("Invalid File");
                  break;
                }
                $scope.messageError = "";
                $scope.uploadFile(dp[0].uri.replace("file://", "") + "/" + $scope.model['mask']);
              default:
                $scope.save()
            }
            
          });
          $scope.openFileExplorer = function() {
            var dataProvider  = dataProviders.filter(function(element) {
              return element.id == $scope.dataSeries.data_provider_id;
            });
            $scope.loadingDirectories = false;
            $scope.pathLoading = null;

            $scope.isChecking = true;

            var tempCurrentPath = ($scope.model['pathname'] == undefined || $scope.model['pathname'] == "" ? "/" : $scope.model['pathname']).replace(/\\/g, '/');

            setBasePath(tempCurrentPath);

            if($scope.isWindows)
              tempCurrentPath = tempCurrentPath.substring(3);

            if($scope.dataSeries.access) {
              pathSelected = dataProvider[0].uri.replace("file://", "");

              if(pathSelected == undefined || pathSelected == "")
              {
                $scope.isChecking = false;
                return MessageBoxService.danger(i18n.__(title), i18n.__("Invalid path. This path can not be listed."));
              }

              tempCurrentPath = $scope.basePath + tempCurrentPath;
              var isWindowsPathResult = true;
              var terrama2DataPath = pathSelected.replace(/\\/g, '/');
              $scope.basePath = terrama2DataPath;

              $scope.terrama2DefaultFilePath = (isWindowsPathResult ? pathSelected.replace(/\//g, '\\') : pathSelected);

              if(tempCurrentPath.indexOf(terrama2DataPath) !== -1)
                $scope.currentPath = tempCurrentPath.replace(terrama2DataPath, '').split('/').filter(function(a) { return a != '' });
              else
                $scope.currentPath = [];
            } else {
              $scope.currentPath = tempCurrentPath.split('/').filter(function(a) { return a != $scope.basePath.replace(/\//g, '') && a != '' });
            }

            listDirectories($scope.basePath).then(function(data) {
              if(data.error) {
                $scope.isChecking = false;
                return MessageBoxService.danger(i18n.__(title), ($scope.dataProvider.protocol == "FILE" ? i18n.__("Invalid configuration for the TerraMA² data directory") : data.error));
              }

              $scope.rootDirectories.children = data.directories;
              $scope.rootDirectories.childrenVisible = true;

              if($scope.currentPath.length === 0 || data.directories.length === 0) {
                $scope.isChecking = false;
                $('#filesExplorerModal').modal();
              } else {
                navigateToDirectory($scope.currentPath, $scope.basePath, [$scope.currentPath[0]], 0).then(function(lele) {
                  $scope.isChecking = false;
                  $('#filesExplorerModal').modal();
                }).catch(function(err) {
                  $scope.isChecking = false;
                  MessageBoxService.danger(i18n.__(title), err.message);
                });
              }
            }).catch(function(err) {
              $scope.isChecking = false;
              return MessageBoxService.danger(i18n.__(title), ("FILE" == "FILE" ? i18n.__("Invalid configuration for the TerraMA² data directory") : err.error));
            });
          };

          $("#openExplorer").on('click',()=>{
            $scope.openFileExplorer();
          });

          var verifyPath = function(pathList) {

            let isValidPath = true;

            pathList.forEach((path) => {
              if(!isWindowsPath(path))
                isValidPath = false;
            });

            return isValidPath;
          };


          var setBasePath = function(path) {
            var isWindowsPathResult = isWindowsPath(path);

            if(isWindowsPathResult) {
              $scope.basePath = isWindowsPathResult;
              $scope.isWindows = true;
            } else {
              $scope.basePath = "/";
              $scope.isWindows = false;
            }
          };

          var isWindowsPath = function(path) {
            var letter = "A";
            var isWindows = false;
            var driveLetter = null;

            for(var i = 0; i < 26; i++) {
              if((letter + ":/") == path.substr(0, 3).toUpperCase()) {
                driveLetter = (letter + ":/");
                isWindows = true;
                break;
              }

              letter = String.fromCharCode(letter.charCodeAt() + 1);
            }

            return (isWindows ? driveLetter : false);
          };

          var getPathFromTheList = function(pathList, currentPath) {
            let selectedPath = "";

            pathList.forEach((path) => {
              if(path && currentPath.startsWith(path))
                selectedPath = path;
            });

            return selectedPath;
          };

          var listDirectories = function(pathToList) {
            var promiser = $q.defer();
            var timeOut = $q.defer();

            var params = angular.copy($scope.model);

            params.protocol = 'FILE';
            params.list = true;
            params.getWithExtension = !$scope.showButton;

            let semanticsCode = $scope.dataSeries.semantics.code;
            switch(semanticsCode){
              case "GRID-static_gdal":
                params.extensionFile = ['tif','zip'];
                break;
              default:
                params.extensionFile = ['shp','zip'];
            }

            params.pathname = pathToList;
            params.basePath = $scope.basePath;

            $scope.timeOutSeconds = 8;

            var expired = false;

            setTimeout(function() {
              expired = true;
              timeOut.resolve();
            }, 1000 * $scope.timeOutSeconds);

            var request = $http({
              method: "POST",
              url: BASE_URL + "uri/",
              data: params,
              timeout: timeOut.promise
            });

            request.then(function(data) {
              if(data.message)
                return promiser.reject({ error: i18n.__(data.message) });
              else
                return promiser.resolve({ error: null, directories: data.data.data.list });
            }).catch(function(err) {
              if(expired)
                return promiser.reject({ error: i18n.__("Timeout: Request took longer than ") + $scope.timeOutSeconds + i18n.__(" seconds.") });
              else
                return promiser.reject({ error: i18n.__(err.data.message) });
            });

            return promiser.promise;
          };

          $scope.setDirectoryStatus = function(path) {
            var tempPath = path;
            var pathItems = tempPath.replace($scope.basePath, '').split('/');
            pathItems = pathItems.filter(function(a) { return a != '' });

            fileName = pathItems.slice(-1).pop();
            extension = fileName.split(".").slice(-1).pop();

            if(path.endsWith(".zip") || path.endsWith(".shp")){
              hasFile = true;
            }else{
              hasFile = false;
            }

            try {
              var lastDirectory = getLastDirectory($scope.rootDirectories, pathItems, 0);
            } catch(err) {
              $('#filesExplorerErrorModal .modal-body > p').text(i18n.__("Invalid path"));
              $('#filesExplorerErrorModal').modal();
              return;
            }

            if(!lastDirectory) {
              $('#filesExplorerErrorModal .modal-body > p').text(i18n.__("Invalid path"));
              $('#filesExplorerErrorModal').modal();
              return;
            }

            if(lastDirectory.childrenVisible) {
              lastDirectory.childrenVisible = false;

              if($scope.selectedDirectory == lastDirectory.fullPath)
                $scope.selectedDirectory = null;
            } else if(!hasFile) {
              $scope.loadingDirectories = true;
              $scope.pathLoading = path;

              listDirectories(path).then(function(data) {
                if(data.error) {
                  $scope.loadingDirectories = false;
                  $scope.pathLoading = null;
                  $('#filesExplorerErrorModal .modal-body > p').text(data.error);
                  $('#filesExplorerErrorModal').modal();
                }

                $scope.selectedDirectory = path;

                lastDirectory.childrenVisible = true;
                lastDirectory.children = data.directories;

                $scope.loadingDirectories = false;
                $scope.pathLoading = null;
              }).catch(function(err) {
                $scope.loadingDirectories = false;
                $scope.pathLoading = null;
                $('#filesExplorerErrorModal .modal-body > p').text(err.error);
                $('#filesExplorerErrorModal').modal();
              });
            }else{
              $scope.selectedDirectory = path;
            }
          };

          var getLastDirectory = function(item, pathDirectories, i) {
            if(i < pathDirectories.length) {
              for(var j = 0, childrenLength = item.children.length; j < childrenLength; j++) {
                if(item.children[j].name == pathDirectories[i]) {
                  return getLastDirectory(item.children[j], pathDirectories, (i + 1));
                }
              }
            } else {
              return item;
            }
          };

          $scope.selectPath = function(file) {
            var path = $scope.selectedDirectory.slice()
            $scope.model['mask'] = path.replace(pathSelected,"").replace(".zip",".shp");
            if(hasFile){
              $scope.uploadFile($scope.selectedDirectory);
            }
          };

          // Fim file explorer


          $scope.openImportShapefileModal = function() {
            $scope.shpImport.srid = null;
            $scope.shpImport.encoding = "latin1";

            $scope.clearShapefileImportError();
            $scope.closeShapefileSuccessMessage();

            if(($scope.semanticsCode === 'STATIC_DATA-ogr') || ($scope.semanticsCode === 'STATIC_DATA-postgis' && $scope.model['table_name'] !== undefined && $scope.model['table_name'] !== null && $scope.model['table_name'] != ""))
              $("#shapefileModal").modal();
            else
              MessageBoxService.danger(i18n.__("Error"), i18n.__($scope.semanticsCode === 'STATIC_DATA-ogr' ? "Enter the file name!" : "Enter the table name!"));
          };

          $scope.uploadFile = function(filePath){
            var file = Upload.upload({
              url: BASE_URL + 'import-file',
              data: {
                filePath: filePath,
                fileName: fileName,
                semantics: $scope.semanticsCode,
                srid: $scope.shpImport.srid,
                encoding: $scope.shpImport.encoding,
                tableName: $scope.model['table_name'],
                mask: $scope.model['mask'].replace(".zip",".shp"),
                dataProviderId: $scope.dataSeries.data_provider_id
              }
            });

            file.then(function(value) {
              $scope.messageError  = "";
              $scope.save();
              return false;
            }, function(reason) {
              if(reason.status == 404){
                $scope.messageError = "Arquivo não encontrado!";
                return true;
              }else{
                $scope.messageError  = "";
                $scope.save();
                return false;
              }
            });
          }

          $scope.uploadShapefile = function(file, errFiles) {
            $scope.clearShapefileImportError();
            $scope.closeShapefileSuccessMessage();
            let dataProvider  = dataProviders.filter(function(element) {
              return element.id == $scope.dataSeries.data_provider_id;
            });
            var basePath = dataProvider[0].uri.replace("file://", "").endsWith("/") ? dataProvider[0].uri.replace("file://", ""):dataProvider[0].uri.replace("file://", "")+"/";
            var fileName = file.name;
            if(file) {
              var performUpload = function() {
                file.upload = Upload.upload({
                  url: BASE_URL + 'import-shapefile',
                  file: file,
                  data: {
                    basePath:basePath,
                    semantics: $scope.semanticsCode,
                    srid: $scope.shpImport.srid,
                    encoding: $scope.shpImport.encoding,
                    tableName: $scope.model['table_name'],
                    mask: $scope.model['mask'],
                    dataProviderId: $scope.dataSeries.data_provider_id
                  }
                })
                .then(() => {
                  if($scope.dataSeries.semantics.code != "STATIC_DATA-postgis"){
                    $scope.model['mask'] = $scope.model['mask'] + "/" + fileName.replace(".zip", ".shp");
                  }
                  $scope.uploaded = true;
                  $scope.save();
                });

                file.upload.then(function(response) {
                  $timeout(function () {
                    if(response && response.data.error) $scope.shpImport.error = i18n.__(response.data.error);
                    else $scope.shpImport.success = true;
                  })
                }, response => {
                  if(response.status > 0)
                    $scope.shpImport.error = response.status + ': ' + response.data.error;
                })
                // finally
                .then(() => {
                  if(!$("#shapefile-import-loader").hasClass("hidden"))
                    $("#shapefile-import-loader").addClass("hidden");
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
        controller: ['$scope', 'MessageBoxService', 'Upload', 'DataProviderService', '$timeout', '$q', 'i18n', '$http', function($scope, MessageBoxService, Upload, DataProviderService, $timeout, $q, i18n, $http) {
          $scope.geotiffImport = {
            error: null,
            success: false
          };

          var title = "Data Server Registration";
          $scope.rootDirectories = {
            children: []
          };

          $scope.uploaded = false;
          $scope.messageError = "";

          // Inicio File Explorer
          var dataProviders = {};
          var hasFile = false;
          var fileName = "";
          var extension = "";
          var pathSelected = "";

          DataProviderService.listAll({ id:$scope.dataSeries.data_provider_id })
          .then((data) =>{
            dataProviders=data.data;
          });
          $("#saveStaticData").on("click",()=>{
            let dp  = dataProviders.filter(function(element) {
              return element.id == $scope.dataSeries.data_provider_id;
            });

            let semanticsCode = $scope.dataSeries.semantics.code;
            switch(semanticsCode){
              case "GRID-static_gdal":
                  fileName = $scope.model['mask'].split('/').slice(-1).pop();
                  if(!fileName.endsWith(".tif") && !$scope.showButton){
                    $scope.messageError = i18n.__("Invalid File");
                    break;
                  }
                  $scope.messageError = "";
                  $scope.uploadFile(dp[0].uri.replace("file://", "") + "/" + $scope.model['mask']);
                  break;
              default:
                $scope.save()
            }
            
          });

          $scope.openFileExplorer = function() {
            var dataProvider  = dataProviders.filter(function(element) {
              return element.id == $scope.dataSeries.data_provider_id;
            });
            $scope.loadingDirectories = false;
            $scope.pathLoading = null;

            $scope.isChecking = true;

            var tempCurrentPath = ($scope.model['pathname'] == undefined || $scope.model['pathname'] == "" ? "/" : $scope.model['pathname']).replace(/\\/g, '/');

            setBasePath(tempCurrentPath);

            if($scope.isWindows)
              tempCurrentPath = tempCurrentPath.substring(3);

            if($scope.dataSeries.access) {
              pathSelected = dataProvider[0].uri.replace("file://", "");

              if(pathSelected == undefined || pathSelected == "")
              {
                $scope.isChecking = false;
                return MessageBoxService.danger(i18n.__(title), i18n.__("Invalid path. This path can not be listed."));
              }

              tempCurrentPath = $scope.basePath + tempCurrentPath;
              var isWindowsPathResult = true;
              var terrama2DataPath = pathSelected.replace(/\\/g, '/');
              $scope.basePath = terrama2DataPath;

              $scope.terrama2DefaultFilePath = (isWindowsPathResult ? pathSelected.replace(/\//g, '\\') : pathSelected);

              if(tempCurrentPath.indexOf(terrama2DataPath) !== -1)
                $scope.currentPath = tempCurrentPath.replace(terrama2DataPath, '').split('/').filter(function(a) { return a != '' });
              else
                $scope.currentPath = [];
            } else {
              $scope.currentPath = tempCurrentPath.split('/').filter(function(a) { return a != $scope.basePath.replace(/\//g, '') && a != '' });
            }

            listDirectories($scope.basePath).then(function(data) {
              if(data.error) {
                $scope.isChecking = false;
                return MessageBoxService.danger(i18n.__(title), ($scope.dataProvider.protocol == "FILE" ? i18n.__("Invalid configuration for the TerraMA² data directory") : data.error));
              }

              $scope.rootDirectories.children = data.directories;
              $scope.rootDirectories.childrenVisible = true;

              if($scope.currentPath.length === 0 || data.directories.length === 0) {
                $scope.isChecking = false;
                $('#filesExplorerModal').modal();
              } else {
                navigateToDirectory($scope.currentPath, $scope.basePath, [$scope.currentPath[0]], 0).then(function(lele) {
                  $scope.isChecking = false;
                  $('#filesExplorerModal').modal();
                }).catch(function(err) {
                  $scope.isChecking = false;
                  MessageBoxService.danger(i18n.__(title), err.message);
                });
              }
            }).catch(function(err) {
              $scope.isChecking = false;
              return MessageBoxService.danger(i18n.__(title), ("FILE" == "FILE" ? i18n.__("Invalid configuration for the TerraMA² data directory") : err.error));
            });
          };

          $("#openExplorer").on('click',()=>{
            $scope.openFileExplorer();
          });

          var verifyPath = function(pathList) {

            let isValidPath = true;

            pathList.forEach((path) => {
              if(!isWindowsPath(path))
                isValidPath = false;
            });

            return isValidPath;
          };


          var setBasePath = function(path) {
            var isWindowsPathResult = isWindowsPath(path);

            if(isWindowsPathResult) {
              $scope.basePath = isWindowsPathResult;
              $scope.isWindows = true;
            } else {
              $scope.basePath = "/";
              $scope.isWindows = false;
            }
          };

          var isWindowsPath = function(path) {
            var letter = "A";
            var isWindows = false;
            var driveLetter = null;

            for(var i = 0; i < 26; i++) {
              if((letter + ":/") == path.substr(0, 3).toUpperCase()) {
                driveLetter = (letter + ":/");
                isWindows = true;
                break;
              }

              letter = String.fromCharCode(letter.charCodeAt() + 1);
            }

            return (isWindows ? driveLetter : false);
          };

          var getPathFromTheList = function(pathList, currentPath) {
            let selectedPath = "";

            pathList.forEach((path) => {
              if(path && currentPath.startsWith(path))
                selectedPath = path;
            });

            return selectedPath;
          };

          var listDirectories = function(pathToList) {
            var promiser = $q.defer();
            var timeOut = $q.defer();

            var params = angular.copy($scope.model);

            params.protocol = 'FILE';
            params.list = true;
            params.getWithExtension = !$scope.showButton;

            let semanticsCode = $scope.dataSeries.semantics.code;
            switch(semanticsCode){
              case "GRID-static_gdal":
                params.extensionFile = ['tif'];
                break;
              default:
                params.extensionFile = ['shp','zip'];
            }

            params.pathname = pathToList;
            params.basePath = $scope.basePath;

            $scope.timeOutSeconds = 8;

            var expired = false;

            setTimeout(function() {
              expired = true;
              timeOut.resolve();
            }, 1000 * $scope.timeOutSeconds);

            var request = $http({
              method: "POST",
              url: BASE_URL + "uri/",
              data: params,
              timeout: timeOut.promise
            });

            request.then(function(data) {
              if(data.message)
                return promiser.reject({ error: i18n.__(data.message) });
              else
                return promiser.resolve({ error: null, directories: data.data.data.list });
            }).catch(function(err) {
              if(expired)
                return promiser.reject({ error: i18n.__("Timeout: Request took longer than ") + $scope.timeOutSeconds + i18n.__(" seconds.") });
              else
                return promiser.reject({ error: i18n.__(err.data.message) });
            });

            return promiser.promise;
          };

          $scope.setDirectoryStatus = function(path) {
            var tempPath = path;
            var extensions = ['shp', 'zip', 'tif','geotif']
            var pathItems = tempPath.replace($scope.basePath, '').split('/');
            pathItems = pathItems.filter(function(a) { return a != '' });

            fileName = pathItems.slice(-1).pop();
            extension = fileName.split(".").slice(-1).pop();

            if(path.endsWith(".tif")){
              hasFile = true;
            }else{
              hasFile = false;
            }

            try {
              var lastDirectory = getLastDirectory($scope.rootDirectories, pathItems, 0);
            } catch(err) {
              $('#filesExplorerErrorModal .modal-body > p').text(i18n.__("Invalid path"));
              $('#filesExplorerErrorModal').modal();
              return;
            }

            if(!lastDirectory) {
              $('#filesExplorerErrorModal .modal-body > p').text(i18n.__("Invalid path"));
              $('#filesExplorerErrorModal').modal();
              return;
            }

            if(lastDirectory.childrenVisible) {
              lastDirectory.childrenVisible = false;

              if($scope.selectedDirectory == lastDirectory.fullPath)
                $scope.selectedDirectory = null;
            } else if(!hasFile) {
              $scope.loadingDirectories = true;
              $scope.pathLoading = path;

              listDirectories(path).then(function(data) {
                if(data.error) {
                  $scope.loadingDirectories = false;
                  $scope.pathLoading = null;
                  $('#filesExplorerErrorModal .modal-body > p').text(data.error);
                  $('#filesExplorerErrorModal').modal();
                }

                $scope.selectedDirectory = path;

                lastDirectory.childrenVisible = true;
                lastDirectory.children = data.directories;

                $scope.loadingDirectories = false;
                $scope.pathLoading = null;
              }).catch(function(err) {
                $scope.loadingDirectories = false;
                $scope.pathLoading = null;
                $('#filesExplorerErrorModal .modal-body > p').text(err.error);
                $('#filesExplorerErrorModal').modal();
              });
            }else{
              $scope.selectedDirectory = path;
            }
          };

          var getLastDirectory = function(item, pathDirectories, i) {
            if(i < pathDirectories.length) {
              for(var j = 0, childrenLength = item.children.length; j < childrenLength; j++) {
                if(item.children[j].name == pathDirectories[i]) {
                  return getLastDirectory(item.children[j], pathDirectories, (i + 1));
                }
              }
            } else {
              return item;
            }
          };

          $scope.selectPath = function(file) {
            var path = $scope.selectedDirectory.slice()
            $scope.model['mask'] = path.replace(pathSelected,"").replace(".zip",".tif");
            if(hasFile){
              $scope.uploadFile($scope.selectedDirectory);
            }
          };
          //Fim FileExplorer

          $scope.openImportGeotiffModal = function() {
            $scope.clearGeotiffImportError();
            $scope.closeGeotiffSuccessMessage();

            if($scope.model['mask'] !== undefined && $scope.model['mask'] !== null && $scope.model['mask'] != "")
              $("#geotiffModal").modal();
            else
              MessageBoxService.danger(i18n.__("Error"), i18n.__("Enter the file name!"));
          };

          $scope.uploadFile = function(filePath){
            var file = Upload.upload({
              url: BASE_URL + 'import-file',
              data: {
                filePath: filePath,
                fileName: fileName,
                semantics: $scope.semanticsCode,
                tableName: $scope.model['table_name'],
                mask: $scope.model['mask'].replace(".zip",".tif"),
                dataProviderId: $scope.dataSeries.data_provider_id
              }
            });

            file.then(function(value) {
              $scope.messageError  = "";
              $scope.save();
              return false;
            }, function(reason) {
              if(reason.status == 404){
                $scope.messageError = "Arquivo não encontrado!";
                return true;
              }else{
                $scope.messageError  = "";
                $scope.save();
                return false;
              }
            });
          }

          $scope.uploadGeotiff = function(file, errFiles) {
            $scope.clearGeotiffImportError();
            $scope.closeGeotiffSuccessMessage();
            let dataProvider  = dataProviders.filter(function(element) {
              return element.id == $scope.dataSeries.data_provider_id;
            });
            var basePath = dataProvider[0].uri.replace("file://", "").endsWith("/") ? dataProvider[0].uri.replace("file://", ""):dataProvider[0].uri.replace("file://", "")+"/";
            var fileName = file.name;
            if(file) {
              if($("#geotiff-import-loader").hasClass("hidden"))
                $("#geotiff-import-loader").removeClass("hidden");

              file.upload = Upload.upload({
                url: BASE_URL + 'import-geotiff',
                file: file,
                data: {
                  basePath:basePath,
                  fileName:fileName,
                  mask: $scope.model['mask'],
                  dataProviderId: $scope.dataSeries.data_provider_id
                }
              });

              file.upload.then(function(value) {
                $scope.messageError  = "";
                $scope.model['mask'] = $scope.model['mask'] + "/" + fileName;
                $scope.save();
                return false;
              }, function(reason) {
                if(reason.status == 404){
                  $scope.messageError = "Arquivo não encontrado!";
                  return true;
                }else{
                  $scope.messageError  = "";
                  $scope.save();
                  return false;
                }
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