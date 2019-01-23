define([],()=> {
  class VectorProcessingComponent {
    constructor(i18n, SpatialOperations) {
      this.title = i18n.__("Operations");
      this.SpatialOperations = SpatialOperations;
    }
  }

  VectorProcessingComponent.$inject = ["i18n",
                                       "SpatialOperations"];

  const component = {
    bindings : {
      model: "=",
      css: "<"
    },
    controller: VectorProcessingComponent,
    template: `<div class="col-md-6">
                 <div class="col-align-self-start">
                   <div class="form-group has-feedback" terrama2-show-errors>
                     <label>{{$ctrl.title}}:</label>
                      <select class="form-control"
                              name="targetVectorProcessing"
                              ng-model="$ctrl.model"
                              ng-options="v as k for (k,v) in $ctrl.SpatialOperations"
                              ng-required="true">
                      </select>
                  </div>
                </div>
              </div>`
  };
  return component;
})
