define(['TerraMA2WebApp/schema-form-plugin/mask-warn/directives/terrama2-mask-field'], function(terrama2MaskField){
  angular.module('templates', []).run(['$templateCache', function($templateCache){
    $templateCache.put('/dist/templates/schema-form-plugin/mask-warn/templates/terrama2-mask-field.html', 
    '<div class="form-group schema-form-{{form.type}} {{form.htmlClass}}" ng-class="{\'has-error\': form.disableErrorState !== true && hasError(), \'has-success\': form.disableSuccessState !== true && hasSuccess(), \'has-feedback\': form.feedback !== false }">' +
      '<label class="control-label {{form.labelHtmlClass}}" ng-class="{\'sr-only\': !showTitle()}" for="{{form.key.slice(-1)[0]}}">{{form.title}}</label>' +
      '<terrama2-mask-field sf-field-model schema-validate="form"></terrama2-mask-field>' +
      '<span class="help-block" sf-message="form.description"></span>' +
    '</div>'

    );
  }]); 
  angular.module('terrama2MaskForm', [
    'schemaForm',
    'templates'
  ]).config(['schemaFormDecoratorsProvider', 'sfBuilderProvider', function(schemaFormDecoratorsProvider, sfBuilderProvider) {

    schemaFormDecoratorsProvider.defineAddOn(
      'bootstrapDecorator', // Name of the decorator you want to add to.
      'terrama2maskform', // Form type that should render this add-on
      '/dist/templates/schema-form-plugin/mask-warn/templates/terrama2-mask-field.html', // Template name in $templateCache
      sfBuilderProvider.stdBuilders // List of builder functions to apply.
    );

  }])
  .directive('terrama2MaskField', terrama2MaskField);
  return 'terrama2MaskForm';
});
