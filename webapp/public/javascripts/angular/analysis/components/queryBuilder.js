define([],()=> {
  class QueryBuilderComponent {
    constructor(i18n, debounce) {
      this.title = i18n.__("Query Builder");

      this.onChangeQueryBuilder = debounce(() => {
        const { model } = this;
        this.onChange({ query: model });
      }, 1000, false);
    }
  }

  QueryBuilderComponent.$inject = ['i18n', 'debounce'];

  const component = {
    bindings : {
      model: "=",
      attributes: '<',
      css: '<',
      onChange: '&?'
    },
    controller: QueryBuilderComponent,
    template: `
      <div class="form-group has-feedback terrama2-show-errors">
        <label>{{ $ctrl.title }}:</label>

        <textarea ng-model="$ctrl.model" class="form-control" rows="5" id="query-builder" ng-change="$ctrl.onChangeQueryBuilder()"</textarea>

        <terrama2-analysis-helpers ng-if="ctrl.attributes" operators="ctrl.attributes" target="ctrl.operatorValue"></terrama2-analysis-helpers>
      </div><!--./row-->
    `
  };
  return component;
})