define([], function(){

  function terrama2MaskField(i18n) {
    return {
      restrict: 'E',
      require: 'ngModel',
      scope: false,
      
      template:
        '<input ng-if="!form.fieldAddonLeft && !form.fieldAddonRight" ng-show="form.key" type="{{form.type}}" step="any" sf-changed="form" placeholder="{{form.placeholder}}" class="form-control {{form.fieldHtmlClass}}" id="{{form.key.slice(-1)[0]}}" ng-model-options="form.ngModelOptions" ng-model="modelValue" ng-disabled="form.readonly" schema-validate="form" name="{{form.key.slice(-1)[0]}}" aria-describedby="{{form.key.slice(-1)[0] + \'Status\'}}" ng-blur="updateModel(modelValue)">' + 
          
        '<span style="top: 25px; margin-right: 20px !important" ng-if="form.feedback !== false" class="form-control-feedback" ng-class="evalInScope(form.feedback) || {\'glyphicon\': true, \'glyphicon-ok\': hasSuccess(), \'glyphicon-remove\': hasError() }" aria-hidden="true"></span>'+
        '<span ng-if="hasError() || hasSuccess()" id="{{form.key.slice(-1)[0] + \'Status\'}}" class="sr-only">{{ hasSuccess() ? \'(success)\' : \'(error)\' }}</span>'+
        '<span class="warn-message" ng-if="showWarnMessage" ng-bind="i18n.__(\'Files can be overwritten\')"></span>'
      ,
      link: function (scope, element, attrs, ngModel) {
        scope.i18n = i18n;
        scope.modelValue = ngModel.$viewValue;
        scope.showWarnMessage = false;

        scope.updateModel = function (modelValue) {
          ngModel.$setViewValue(modelValue);

          if (!modelValue) {
            scope.showWarnMessage = false;
            return;
          }
          scope.showWarnMessage = true;
          if (scope.form.maskPattern){
            scope.form.maskPattern.forEach(function(mPattern){
              if (modelValue.indexOf(mPattern) != -1){ 
                scope.showWarnMessage = false;
                return;
              }
            });
          }
          scope.showWarnMessage = scope.showWarnMessage && ngModel.$valid;
        };

      },
    };
  }
  terrama2MaskField.$inject = ['i18n'];
  return terrama2MaskField;
});
