define([], function(){
  try {
    angular.module('schemaForm').config(['schemaFormProvider',
        'schemaFormDecoratorsProvider', 'sfPathProvider',
        function(schemaFormProvider, schemaFormDecoratorsProvider, sfPathProvider) {

            // First, we want this to be the default for a combination of schema parameters
            var terrama2maskform = function (name, schema, options) {
                if (schema.type === 'string' && schema.format == 'terrama2maskform') {
                    // Initiate a form provider
                    var f = schemaFormProvider.stdFormObj(name, schema, options);
                    f.key = options.path;
                    f.type = 'terrama2maskform';
                    // Add it to the lookup dict (for internal use)
                    options.lookup[sfPathProvider.stringify(options.path)] = f;
                    return f;
                }
            };
            // Add our default to the defaults array
            schemaFormProvider.defaults.string.unshift(terrama2maskform);

            // Second, we want it to show if someone have explicitly set the form type
            schemaFormDecoratorsProvider.addMapping('bootstrapDecorator', 'terrama2maskform',
                '/dist/templates/schema-form-plugin/mask-warn/templates/terrama2-mask-field.html');
        }]);

    // Declare a controller, this is used in the camelcaseDirective below
    var terrama2MaskFormControllerFunction =  function($scope) {

      $scope.$watch('ngModel.$modelValue', function(value){
        if (value){
           $scope.updateModel();
        }
      });

      $scope.updateModel = function (){
        var leaf_model = $scope.ngModel;
        var modelValue = leaf_model.$modelValue;
        if (!modelValue){
          $scope.showMessage = false;
          return;
        }
        $scope.showMessage = true;
        if ($scope.form.maskPattern){
          $scope.form.maskPattern.forEach(function(mPattern){
            if (modelValue.indexOf(mPattern) != -1){ 
              $scope.showMessage = false;
              return;
            }
          });
        }
      }
    };

    // Create a directive to properly access the ngModel set in the view (src/angular-schema-form-camelcase.html)
    angular.module('schemaForm').directive('terrama2MaskForm', function() {
      return {
        // The directive needs the ng-model to be set, look at the <div>
        require: ['ngModel'],
        restrict: 'A',
        // Do not create a isolate scope, makeCamelCase should be available to the button element
        scope: false,
        // Define a controller, use the function from above, inject the scope
        controller : ['$scope', terrama2MaskFormControllerFunction],
        // Use the link function to initiate the ngModel in the controller scope
        link: function(scope, iElement, iAttrs, ngModelCtrl) {
            scope.ngModel = ngModelCtrl[0];
        }
      };
    });
  } catch (error) {
    
  }
});
