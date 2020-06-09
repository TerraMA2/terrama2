define([
  "TerraMA2WebApp/schema-form-plugin/querybuilder/controller",
], (QueryBuilderController) => {
  function runModule($templateCache) {
    $templateCache.put("directives/decorators/bootstrap/querybuilder/querybuilder.html",
      `<style>
        .align-buttons  {
          display:block
        }
        .attr-select {
          max-height:100px;
          overflow:auto;
          overflow-x:hidden;
          padding:0px;
          position:absolute;
          margin-top:-20px;
          z-index:99;
        }
      </style>
      <div class="form-group {{form.htmlClass}}" ng-class="{'has-error': hasError(), 'has-success': hasSuccess(), 'has-feedback': form.feedback !== false}" sytle="margin-top:10px" ng-controller="QueryBuilderController as ctrl">
          <div ng-if="ctrl.getTableName()">
            <label class="control-label " ng-class="{'sr-only': !showTitle()}" for="view_name">{{ ctrl.viewNameLabel }}</label>
            <input ng-show="form.key" type="text" step="any" sf-changed="form" placeholder="" class="form-control  ng-empty ng-valid-schema-form ng-dirty ng-invalid ng-invalid-tv4-302 ng-touched" id="view_name" ng-model-options="form.ngModelOptions" ng-model="model['view_name']" ng-disabled="form.readonly" schema-validate="form" name="view_name" aria-describedby="view_nameStatus" required>
          </div>
          <span style="top: 25px; margin-right: 20px !important" class="form-control-feedback" ng-class="evalInScope(form.feedback) || {'glyphicon': true, 'glyphicon-ok': hasSuccess(), 'glyphicon-remove': hasError() }" aria-hidden="true"></span>
          <span id="{{form.key.slice(-1)[0] + 'Status'}}" class="sr-only">{{ hasSuccess() ? '(success)' : '(error)' }}</span>
          <span class="help-block" ng-if="hasError() && errorMessage(schemaError())">{{ errorMessage(schemaError())}}</span>
          <div class="col-md-12 row" style="margin-left:5%">
            <div class="col-md-5" ng-if="ctrl.getTableName()">
              <div class="col-align-self-start">
                <div class="form-group has-feedback" terrama2-show-errors>
                  <label>{{ ctrl.inputAttributeLabel }}:</label>
                  <select style="height:250px"
                    class="form-control"
                    name="inputAttributesLayer"
                    id="inputAttributesLayer"
                    ng-model="ctrl.listInputLayersSelected"
                    ng-change="ctrl.onMultInputSelected(ctrl.listInputLayersSelected)" multiple>
                    <option ng-repeat="attribute in ctrl.inputTableAttributes" data-icon="fa fa-home" value="{{attribute}}" class="{{ attribute.icon }} align-buttons">
                    {{attribute.column_name}}
                    </option>
                  </select><br>
                </div>
              </div>
            </div>

            <div class="col-md-1 col-xs-1" style="padding:33px" ng-show="ctrl.getTableName()">
            <div class="row">
                <button type="button" ng-disabled="ctrl.isDisable" data-toggle="tooltip" data-placement="top" title="{{ ctrl.loadAttributesLabel }}" ng-click="ctrl.loadAttributes()" class="btn btn-primary ng-binding align-buttons">
                  <i class="fa fa-spinner"></i>
                </button>

                <button type="button" data-toggle="tooltip" data-placement="top" title="{{ ctrl.moveRightLabel }}" ng-click="ctrl.moveToRight()" class="btn btn-primary ng-binding align-buttons" style="margin-top:5px">
                  <i class="fa fa-arrow-right"></i>
                </button>

                <button type="button" data-toggle="tooltip" data-placement="top" title="{{ ctrl.moveAllRightLabel }}" ng-click="ctrl.moveAllToRight()" class="btn btn-primary ng-binding align-buttons" style="margin-top:5px;width:38px">
                  <i class="glyphicon glyphicon-fast-forward"></i>
                </button>

                <button type="button" data-toggle="tooltip" data-placement="top" title="{{ ctrl.moveLeftLabel }}" ng-click="ctrl.moveToLeft()" class="btn btn-primary ng-binding align-buttons" style="margin-top:5px">
                  <i class="fa fa-arrow-left"></i>
                </button>

                <button type="button" data-toggle="tooltip" data-placement="top" title="{{ ctrl.moveAllLeftLabel }}" ng-click="ctrl.moveAllToLeft()" class="btn btn-primary ng-binding align-buttons" style="margin-top:5px;width:38px">
                  <i class="glyphicon glyphicon-fast-backward"></i>
                </button>
              </div>
            </div>

            <div class="col-md-5" ng-if="ctrl.getTableName()">
              <div class="form-group" terrama2-show-errors>
                <label>{{ ctrl.outputAttributeLabel }}:</label>
                <select style="height:250px"
                  class="form-control"
                  name="outputLayer"
                  id="outputLayer"
                  ng-model="outputLayer"
                  ng-change="ctrl.onMultOutputSelected(outputLayer)" multiple>
                  <option ng-repeat="attribute in ctrl.listOutputLayersSelected" data-icon="fa fa-home" value="{{attribute}}" class="{{ attribute.icon }} align-buttons">
                  {{attribute.column_name}}
                  </option>
                </select><br>
              </div>
            </div>
          </div>

          <div class="row" ng-if="ctrl.getTableName()">

            <div class="col-md-4">
              <label>{{ ctrl.attributeLabel }}</label>
              <select 
                class="form-control"
                id="listAttributeId"
                ng-model="ctrl.attributeFilter"
                ng-change="ctrl.changeAttribute()">
                <option ng-repeat="attribute in ctrl.listTableAttributes" data-icon="fa fa-home" value="{{attribute.column_name}}" class="{{ attribute.icon }} align-buttons">
                  {{attribute.column_name}}
                </option>
              </select>
            </div>

            <div class="col-md-2">
              <label>{{ ctrl.operatorLabel }}:</label>
              <select class="form-control" id="operatorId" ng-click="ctrl.setOperatorValue(this)" ng-model="ctrl.operatorValue">
                <option value="=" style="font-size:16px">=</option>
                <option value=">" style="font-size:16px">></option>
                <option value="<" style="font-size:16px"><</option>
                <option value=">=" style="font-size:16px">>=</option>
                <option value="<=" style="font-size:16px"><=</option>
                <option value="<>" style="font-size:16px"><></option>
                <option value="LIKE" style="font-size:16px">LIKE</option>
              </select>
              <span style="color:red;font-size:12px">{{ ctrl.msgValueErrorOperator }}</span>
            </div>

            <div class="col-md-1">
              <label></label>
              <button class="btn btn-primary" ng-click="ctrl.getValuesByColumn()" style="display:flow-root">
                <i class="fa fa-search fa-1x"></i>
              </button>
            </div>

            <div class="col-md-4">
              <label>{{ ctrl.valueLabel }}:</label>
              <input type="text" name="attributeValue" id="attributeValue" ng-model="ctrl.attributeValue" ng-keypress="ctrl.complete(ctrl.attributeValue)" class="form-control" />
              <span style="color:red;font-size:12px">{{ ctrl.msgValueError }}</span>
              <ul class="col-md-10 attr-select" >
                <li class="list-group-item" ng-repeat="attributeValue in ctrl.listAttributeValues" ng-click="ctrl.fillTextbox(attributeValue)">{{attributeValue}}</li>
              </ul>
            </div>

            <div class="col-md-1">
              <button class="btn btn-primary" style="margin-top:25px" ng-click="ctrl.makeQuery()">
                <i class="fa fa-plus fa-1x"></i>
              </button>
            </div>

          </div>

      </div>

      <div class="form-group {{form.htmlClass}}" ng-class="{'has-error': hasError(), 'has-success': hasSuccess(), 'has-feedback': form.feedback !== false}"
            ng-controller="QueryBuilderController as ctrl">
          <div class="col-md-10">
            <query-builder-wrapper model="$$value$$"
                ng-if="ctrl.getTableName()"
                table-name="ctrl.getTableName()"
                provider="ctrl.getProvider()"
                ng-init=\"$$value$$=$$value$$||[];ctrl.init($$value$$)\"
                on-change="ctrl.onChange(queryBuilder)">
            </query-builder-wrapper>
          </div>
          <div class="col-md-2" ng-if="ctrl.getTableName()">
              <button class="btn btn-primary ng-binding align-buttons" ng-click="ctrl.setClauseValue('and')" value="and" ng-model="ctrl.clauseValue" style="margin-top:30px;width:48px">
              AND
              </button>
              <button class="btn btn-primary ng-binding align-buttons" ng-click="ctrl.setClauseValue('or')" value="and" ng-model="ctrl.clauseValue" style="margin-top:2px;width:48px">
              OR
              </button>
              <button class="btn btn-primary ng-binding align-buttons" ng-click="ctrl.getSelectedText()" style="margin-top:2px;width:48px">
              (  )
              </button>
          </div>
      </div>`);
  }
  runModule.$inject = ['$templateCache'];

  function configModule(schemaFormProvider, schemaFormDecoratorsProvider, sfPathProvider) {
    const select = function (name, schema, options) {
      if ((schema.type === 'querybuilder')) {
        var f = schemaFormProvider.stdFormObj(name, schema, options);
        f.key = options.path;
        f.type = 'querybuilder';
        options.lookup[sfPathProvider.stringify(options.path)] = f;

        return f;
      }
    }

    schemaFormProvider.defaults.string.unshift(select);

    schemaFormDecoratorsProvider.addMapping(
      'bootstrapDecorator',
      'querybuilder',
      'directives/decorators/bootstrap/querybuilder/querybuilder.html'
    );
  };

  configModule.$inject = ['schemaFormProvider', 'schemaFormDecoratorsProvider', 'sfPathProvider'];

  try {
    let cmodule = angular.module('schemaForm')
      .config(configModule)
      .controller('QueryBuilderController', QueryBuilderController)
      .run(runModule);
    return cmodule.name;
  } catch (err) {
    return ""
  }
});