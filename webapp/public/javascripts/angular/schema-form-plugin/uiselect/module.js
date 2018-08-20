define([
  "TerraMA2WebApp/schema-form-plugin/uiselect/controller",
], (UiSelectMultipleController) => {
  function runModule($templateCache) {
    $templateCache.put("directives/decorators/bootstrap/uiselect/uiselectmultiple.html",
      "<div class=\"form-group {{form.htmlClass}}\" ng-class=\"{'has-error': hasError(), 'has-success': hasSuccess(), 'has-feedback': form.feedback !== false}\"" +
            "ng-controller=\"UiSelectMultipleController as ctrl\">" +
        "<label class=\"control-label {{form.labelHtmlClass}}\"" +
                "ng-show=\"showTitle()\">"+
          "{{form.title}}"+
        "</label>" +
        "<div class=\"form-group\">"+
          "<div ng-dropdown-multiselect=\"\" options=\"options\" ng-init=\"$$value$$=$$value$$||[];ctrl.init($$value$$)\" selected-model=\"selectedOptions\" extra-settings=\"extraOptions\">" +
          "</div>" +
        "</div>" +
      "</div>");
  }
  runModule.$inject = ['$templateCache'];

  function configModule(schemaFormProvider, schemaFormDecoratorsProvider, sfPathProvider) {
    const select = function (name, schema, options) {
      if ((schema.type === 'string') && ("items" in schema)) {
        var f = schemaFormProvider.stdFormObj(name, schema, options);
        f.key = options.path;
        f.type = 'uiselectmultiple';
        f.titleMap = schema.items;
        options.lookup[sfPathProvider.stringify(options.path)] = f;

        return f;
      }
    }

    schemaFormProvider.defaults.string.unshift(select);

    schemaFormDecoratorsProvider.addMapping(
      'bootstrapDecorator',
      'uiselectmultiple',
      'directives/decorators/bootstrap/uiselect/uiselectmultiple.html'
    );
  };

  configModule.$inject = ['schemaFormProvider', 'schemaFormDecoratorsProvider', 'sfPathProvider'];

  try {
    let cmodule = angular.module('schemaForm')
      .config(configModule)
      .controller('UiSelectMultipleController', UiSelectMultipleController)
      .run(runModule);
    return cmodule.name;
  } catch (err) {
    return ""
  }
});