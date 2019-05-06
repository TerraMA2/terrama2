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
      <div class="col-md-12 terrama2-nopadding-box">
        <div class="col-md-12">
          <textarea ng-model="$ctrl.model" class="form-control" rows="5" id="query-builder" ng-change="$ctrl.onChangeQueryBuilder()"</textarea>
        </div><!--./col-md-12-->

        <terrama2-analysis-helpers ng-if="ctrl.attributes" operators="ctrl.attributes" target="ctrl.operatorValue"></terrama2-analysis-helpers>
      </div><!--./row-->
    `
  };
  return component;
})
