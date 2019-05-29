define([],()=> {
  class QueryBuilderComponent {
    constructor(i18n) {
      this.title = i18n.__("Query Builder");
    }
  }

  QueryBuilderComponent.$inject = ["i18n"];

  const component = {
    bindings : {
      model: "=",
      css: "<"
    },
    controller: QueryBuilderComponent,
    template: `<terrama2-box title="$ctrl.title" css="$ctrl.css">
                <div class="col-md-12 terrama2-nopadding-box">
                  <div class="col-md-12">
                    <textarea ng-model="$ctrl.model" class="form-control" rows="5" id="query-builder"></textarea>
                  </div><!--./col-md-12-->
                </div><!--./row-->
               </terrama2-box>`
  };
  return component;
})
