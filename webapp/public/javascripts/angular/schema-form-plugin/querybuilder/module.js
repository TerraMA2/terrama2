define([
  "TerraMA2WebApp/schema-form-plugin/querybuilder/controller",
], (QueryBuilderController) => {
  function runModule($templateCache) {
    $templateCache.put("directives/decorators/bootstrap/querybuilder/querybuilder.html",
      `<div class="form-group {{form.htmlClass}}" ng-class="{'has-error': hasError(), 'has-success': hasSuccess(), 'has-feedback': form.feedback !== false}"
            ng-controller="QueryBuilderController as ctrl">
        <div class="form-group">
          <query-builder-wrapper model="$$value$$"
            ng-if="ctrl.getTableName()"
            table-name="ctrl.getTableName()"
            provider="ctrl.getProvider()"
            ng-init=\"$$value$$=$$value$$||[];ctrl.init($$value$$)\"
            on-change="ctrl.onChange(queryBuilder)">
          </query-builder-wrapper>
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