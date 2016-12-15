'use strict';

angular.module('terrama2.dcpImporter', ['terrama2']).run(function($templateCache) {
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
            "<button type=\"button\" class=\"btn btn-primary pull-right\" ng-click=\"import()\">Import</button>" +
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
            "<div ng-repeat=\"semantic in semantics.metadata.form\">" +
              "<div class=\"form-group col-md-3\">" +
                "<label>{{ i18n.__(semantics.metadata.schema.properties[semantic.key].title) }}:</label>" +
                "<select class=\"form-control\" name=\"{{ semantic.key }}Toa5\" ng-model=\"importationFields.toa5[semantic.key]\">" +
                  "<option>{{ i18n.__('Select a column') }}</option>" +
                  "<option ng-repeat=\"column in csvImport.finalData.header\" ng-init=\"importationFields.toa5[semantic.key] = csvImport.finalData.header[0]\" value=\"{{ column }}\">{{ column }}</option>" +
                "</select>" +
              "</div>" +
              "<div class=\"form-group col-md-3\" ng-show=\"semantics.metadata.schema.properties[semantic.key].hasDefaultFieldForImport && semantic.titleMap\">" +
                "<label>{{ i18n.__(semantics.metadata.schema.properties[semantic.key].title) }}:</label>" +
                "<select class=\"form-control\" id=\"{{ semantic.key }}Toa5Default\" name=\"{{ semantic.key }}Toa5Default\" ng-model=\"importationFields.toa5[semantic.key + 'Default']\">" +
                  "<option ng-repeat=\"titleMap in semantic.titleMap\" label=\"{{ titleMap.name }}\" value=\"{{ titleMap.value }}\">{{ titleMap.name }}</option>" +
                "</select>" +
              "</div>" +
              "<div class=\"form-group col-md-3\" ng-show=\"semantics.metadata.schema.properties[semantic.key].hasDefaultFieldForImport && !semantic.titleMap\">" +
                "<label>{{ i18n.__('Default') }}:</label>" +
                "<input class=\"form-control\" id=\"{{ semantic.key }}Toa5Default\" name=\"{{ semantic.key }}Toa5Default\" ng-model=\"importationFields.toa5[semantic.key + 'Default']\" placeholder=\"{{ i18n.__('Default') }}\" type=\"text\">" +
              "</div>" +
              "<div class=\"form-group col-md-3\" ng-show=\"semantics.metadata.schema.properties[semantic.key].hasPrefixFieldForImport\">" +
                "<label>{{ i18n.__('Prefix') }}:</label>" +
                "<input class=\"form-control\" id=\"{{ semantic.key }}Toa5Prefix\" name=\"{{ semantic.key }}Toa5Prefix\" ng-model=\"importationFields.toa5.{{ semantic.key }}Prefix\" placeholder=\"{{ i18n.__('Prefix') }}\" type=\"text\">" +
              "</div>" +
              "<div class=\"form-group col-md-3\" ng-show=\"semantics.metadata.schema.properties[semantic.key].hasSuffixFieldForImport\">" +
                "<label>{{ i18n.__('Suffix') }}:</label>" +
                "<input class=\"form-control\" id=\"{{ semantic.key }}Toa5Suffix\" name=\"{{ semantic.key }}Toa5Suffix\" ng-model=\"importationFields.toa5.{{ semantic.key }}Suffix\" placeholder=\"{{ i18n.__('Suffix') }}\" type=\"text\">" +
              "</div>" +
            "</div>"+
            "<hr style=\"border: 1px solid #eee !important;\"/>" +
            "<button type=\"button\" class=\"btn btn-default pull-left\" data-dismiss=\"modal\">Close</button>" +
            "<button type=\"button\" class=\"btn btn-primary pull-right\" ng-click=\"extra.validateImportationMetadata()\">Import</button>" +
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
    controller: ['$scope', function($scope) {
      $scope.csvImport = {};
      $scope.importationFields = {
        toa5: {}
      };

      $scope.validateImportationMetadata = function(fields, type) {
        var importationMetadata = {};

        for(var i = 0, fieldsLength = fields.length; i < fieldsLength; i++) {
          var metadata = {
            field: null,
            defaultValue: null,
            prefix: null,
            suffix: null
          };

          if($scope.importationFields[type][fields[i].fieldName] !== null && $scope.importationFields[type][fields[i].fieldName] !== "") {
            metadata.field = $scope.importationFields[type][fields[i].fieldName];

            if($scope.importationFields[type][fields[i].prefix] !== null && $scope.importationFields[type][fields[i].prefix] !== "")
              metadata.prefix = $scope.importationFields[type][fields[i].prefix];

            if($scope.importationFields[type][fields[i].suffix] !== null && $scope.importationFields[type][fields[i].suffix] !== "")
              metadata.suffix = $scope.importationFields[type][fields[i].suffix];
          } else if($scope.importationFields[type][fields[i].default] !== null && $scope.importationFields[type][fields[i].default] !== "")
            metadata.defaultValue = $scope.importationFields[type][fields[i].default];
          else
            metadata.error = new Error("Invalid configuration for the field 'Folder'");

          importationMetadata[fields[i].fieldName] = metadata;
        }

        $('#importDCPItemsModal').modal('hide');
      };

      $scope.openImportModal = function() {
        $('#importParametersModal').modal('show');
      };

      $scope.import = function() {
        $('#importParametersModal').modal('hide');

        $http.get("/api/DataSeriesSemantics/DCP-toa5?metadata=true", {}).success(function(semantics) {
          
          console.log(JSON.stringify(semantics));

          var html = "";

          for(var i = 0, formLength = semantics.metadata.form.length; i < formLength; i++) {
            var colSize = 12;
            var numberOfFields = 1;

            if(semantics.metadata.schema.properties[semantics.metadata.form[i].key].hasDefaultFieldForImport)
              colSize /= ++numberOfFields;

            if(semantics.metadata.schema.properties[semantics.metadata.form[i].key].hasSuffixFieldForImport)
              colSize /= ++numberOfFields;

            if(semantics.metadata.schema.properties[semantics.metadata.form[i].key].hasPrefixFieldForImport)
              colSize /= ++numberOfFields;

            html += "<div class=\"form-group col-md-" + colSize + "\">";
            html += "<label>" + i18n.__(semantics.metadata.schema.properties[semantics.metadata.form[i].key].title) + ":</label>";
            html += "<select class=\"form-control\" name=\"" + semantics.metadata.form[i].key + "Toa5\" ng-model=\"importationFields.toa5." + semantics.metadata.form[i].key + "\">";
            html += "<option>" + i18n.__("Select a column") + "</option>";
            html += "<option ng-repeat=\"column in csvImport.finalData.header\" ng-init=\"importationFields.toa5." + semantics.metadata.form[i].key + " = csvImport.finalData.header[0]\" value=\"{{ column }}\">{{ column }}</option>";
            html += "</select>";
            html += "</div>";

            if(semantics.metadata.schema.properties[semantics.metadata.form[i].key].hasDefaultFieldForImport) {
              if(semantics.metadata.form[i].titleMap) {
                html += "<div class=\"form-group col-md-" + colSize + "\">";
                html += "<label>" + i18n.__(semantics.metadata.schema.properties[semantics.metadata.form[i].key].title) + ":</label>";
                html += "<select class=\"form-control\" id=\"" + semantics.metadata.form[i].key + "Toa5Default\" name=\"" + semantics.metadata.form[i].key + "Toa5Default\" ng-model=\"importationFields.toa5." + semantics.metadata.form[i].key + "Default\">";

                for(var j = 0, titleMapLength = semantics.metadata.form[i].titleMap.length; j < titleMapLength; j++) {
                  html += "<option label=\"" + semantics.metadata.form[i].titleMap[j].name + "\" value=\"" + semantics.metadata.form[i].titleMap[j].value + "\">" + semantics.metadata.form[i].titleMap[j].name + "</option>";
                }

                html += "</select>";
                html += "</div>";
              } else {
                html += "<div class=\"form-group col-md-" + colSize + "\">";
                html += "<label>" + i18n.__("Default") + ":</label>";
                html += "<input class=\"form-control\" id=\"" + semantics.metadata.form[i].key + "Toa5Default\" name=\"" + semantics.metadata.form[i].key + "Toa5Default\" ng-model=\"importationFields.toa5." + semantics.metadata.form[i].key + "Default\" placeholder=\"" + i18n.__("Default") + "\" type=\"text\">";
                html += "</div>";
              }
            }

            if(semantics.metadata.schema.properties[semantics.metadata.form[i].key].hasPrefixFieldForImport) {
              html += "<div class=\"form-group col-md-" + colSize + "\">";
              html += "<label>" + i18n.__("Prefix") + ":</label>";
              html += "<input class=\"form-control\" id=\"" + semantics.metadata.form[i].key + "Toa5Prefix\" name=\"" + semantics.metadata.form[i].key + "Toa5Prefix\" ng-model=\"importationFields.toa5." + semantics.metadata.form[i].key + "Prefix\" placeholder=\"" + i18n.__("Prefix") + "\" type=\"text\">";
              html += "</div>";
            }

            if(semantics.metadata.schema.properties[semantics.metadata.form[i].key].hasSuffixFieldForImport) {
              html += "<div class=\"form-group col-md-" + colSize + "\">";
              html += "<label>" + i18n.__("Suffix") + ":</label>";
              html += "<input class=\"form-control\" id=\"" + semantics.metadata.form[i].key + "Toa5Suffix\" name=\"" + semantics.metadata.form[i].key + "Toa5Suffix\" ng-model=\"importationFields.toa5." + semantics.metadata.form[i].key + "Suffix\" placeholder=\"" + i18n.__("Suffix") + "\" type=\"text\">";
              html += "</div>";
            }
          }

          $("#importDCPItemsModal > .modal-dialog > .modal-content > .modal-body > .fields").html($compile(html)($scope, function() {
            $timeout(function() {
              $scope.$apply(function() {
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

                  $('#importDCPItemsModal').modal('show');
                });
              });
            }, false, ".csv, application/csv");
            });
            });
          }));
          
        }).error(function(err) {
          console.log("Err in retrieving semantics");
        });

        /*<div class="form-group col-md-6">
          <label>{{ i18n.__("Folder") }}:</label>
          <select class="form-control" name="folderToa5" ng-model="importationFields.toa5.folder" ng-change="extra.lala()">
            <option>{{ i18n.__("Select a column") }}</option>
            <option ng-repeat="column in csvImport.finalData.header" ng-init="importationFields.toa5.folder = csvImport.finalData.header[0]" value="{{ column }}">{{ column }}</option>
          </select>
        </div>
        <div class="form-group col-md-6">
          <label>{{ i18n.__("Default") }}:</label>
          <input class="form-control" id="folderToa5Default" name="folderToa5Default" ng-model="importationFields.toa5.folderDefault" placeholder="{{ i18n.__('Default') }}" type="text">
        </div>
        <div class="form-group col-md-6">
          <label>{{ i18n.__("Mask") }}:</label>
          <select class="form-control" name="maskToa5" ng-model="importationFields.toa5.mask">
            <option>{{ i18n.__("Select a column") }}</option>
            <option ng-repeat="column in csvImport.finalData.header" ng-init="importationFields.toa5.mask = csvImport.finalData.header[0]" value="{{ column }}">{{ column }}</option>
          </select>
        </div>
        <div class="form-group col-md-6">
          <label>{{ i18n.__("Suffix") }}:</label>
          <input class="form-control" id="maskToa5Suffix" name="maskToa5Suffix" ng-model="importationFields.toa5.maskSuffix" placeholder="{{ i18n.__('Suffix') }}" type="text">
        </div>
        <div class="form-group col-md-6">
          <label>{{ i18n.__("Alias") }}:</label>
          <select class="form-control" name="aliasToa5" ng-model="importationFields.toa5.alias">
            <option>{{ i18n.__("Select a column") }}</option>
            <option ng-repeat="column in csvImport.finalData.header" ng-init="importationFields.toa5.alias = csvImport.finalData.header[0]" value="{{ column }}">{{ column }}</option>
          </select>
        </div>
        <div class="form-group col-md-6">
          <label>{{ i18n.__("Default") }}:</label>
          <input class="form-control" id="aliasToa5Default" name="aliasToa5Default" ng-model="importationFields.toa5.aliasDefault" placeholder="{{ i18n.__('Default') }}" type="text">
        </div>
        <div class="form-group col-md-6">
          <label>{{ i18n.__("Timezone") }}:</label>
          <select class="form-control" name="timezoneToa5" ng-model="importationFields.toa5.timezone">
            <option>{{ i18n.__("Select a column") }}</option>
            <option ng-repeat="column in csvImport.finalData.header" ng-init="importationFields.toa5.timezone = csvImport.finalData.header[0]" value="{{ column }}">{{ column }}</option>
          </select>
        </div>
        <div class="form-group col-md-6">
          <label>{{ i18n.__("Default") }}:</label>
          <select class="form-control" id="timezoneToa5Default" name="timezoneToa5Default" ng-model="importationFields.toa5.timezoneDefault">
            <option label="-12" value="string:-12">-12</option>
            <option label="-11" value="string:-11">-11</option>
            <option label="-10" value="string:-10">-10</option>
            <option label="-9" value="string:-9">-9</option>
            <option label="-8" value="string:-8">-8</option>
            <option label="-7" value="string:-7">-7</option>
            <option label="-6" value="string:-6">-6</option>
            <option label="-5" value="string:-5">-5</option>
            <option label="-4" value="string:-4">-4</option>
            <option label="-3" value="string:-3">-3</option>
            <option label="-2" value="string:-2">-2</option>
            <option label="-1" value="string:-1">-1</option>
            <option label="0" value="string:0">0</option>
            <option label="1" value="string:1">1</option>
            <option label="2" value="string:2">2</option>
            <option label="3" value="string:3">3</option>
            <option label="4" value="string:4">4</option>
            <option label="5" value="string:5">5</option>
            <option label="6" value="string:6">6</option>
            <option label="7" value="string:7">7</option>
            <option label="8" value="string:8">8</option>
            <option label="9" value="string:9">9</option>
            <option label="10" value="string:10">10</option>
            <option label="11" value="string:11">11</option>
            <option label="12" value="string:12">12</option>
          </select>
        </div>
        <div class="form-group col-md-6">
          <label>{{ i18n.__("Latitude") }}:</label>
          <select class="form-control" name="latitudeToa5" ng-model="importationFields.toa5.latitude">
            <option>{{ i18n.__("Select a column") }}</option>
            <option ng-repeat="column in csvImport.finalData.header" ng-init="importationFields.toa5.latitude = csvImport.finalData.header[0]" value="{{ column }}">{{ column }}</option>
          </select>
        </div>
        <div class="form-group col-md-6">
          <label>{{ i18n.__("Longitude") }}:</label>
          <select class="form-control" name="longitudeToa5" ng-model="importationFields.toa5.longitude">
            <option>{{ i18n.__("Select a column") }}</option>
            <option ng-repeat="column in csvImport.finalData.header" ng-init="importationFields.toa5.longitude = csvImport.finalData.header[0]" value="{{ column }}">{{ column }}</option>
          </select>
        </div>
        <div class="form-group col-md-6">
          <label>{{ i18n.__("SRID") }}:</label>
          <select class="form-control" name="sridToa5" ng-model="importationFields.toa5.srid">
            <option>{{ i18n.__("Select a column") }}</option>
            <option ng-repeat="column in csvImport.finalData.header" ng-init="importationFields.toa5.srid = csvImport.finalData.header[0]" value="{{ column }}">{{ column }}</option>
          </select>
        </div>
        <div class="form-group col-md-6">
          <label>{{ i18n.__("Default") }}:</label>
          <input class="form-control" id="sridToa5Default" name="sridToa5Default" ng-model="importationFields.toa5.sridDefault" placeholder="{{ i18n.__('Default') }}" type="text">
        </div>*/
      };
    }],
    link: function(scope, element, attrs, ngModel) {}
  };
});