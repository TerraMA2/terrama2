define([],()=> {
  class VectorProcessingComponent {
    constructor(i18n, SpatialOperations, DataSeriesService) {
      this.operationsTitle = i18n.__("Operations");
      this.monitoredDataSeriesTitle = i18n.__("Monitored Data Series");
      this.attributeIdentifierTitle = i18n.__("Attribute Identifier");
      this.dynamicDataSeriesTitle = i18n.__("Dynamic Data Series");
      this.SpatialOperations = SpatialOperations;
      this.DataSeriesService = DataSeriesService;
    }

    onChangeStaticDataSeries() {
      const { model, targetDataSeries } = this;

      if (!targetDataSeries)
        return;

      const id = targetDataSeries.data_provider_id;

      model.data_provider_id = id;
    }
  }

  VectorProcessingComponent.$inject = ["i18n",
                                       "SpatialOperations",
                                       "DataSeriesService"];

  const component = {
    bindings : {
      model: "=",
      metadata: '=',
      targetDataSeries: '=',
      identifier: '=',
      css: "<",
    },
    controller: VectorProcessingComponent,
    template: `
              <div class="col-md-12">
                <div class="row">
                  <div class="col-md-6">
                    <div class="col-align-self-start">
                      <div class="form-group has-feedback" terrama2-show-errors>
                        <label>{{$ctrl.operationsTitle}}:</label>
                          <select class="form-control"
                                  name="targetVectorProcessing"
                                  ng-model="$ctrl.model.operationType"
                                  ng-options="v as k for (k,v) in $ctrl.SpatialOperations"
                                  ng-required="true">
                          </select>
                      </div>
                    </div>
                  </div>
                </div>
              </div>

              <!-- required -->
              <span class="help-block"
                    ng-show="forms.targetDataSeriesForm.targetDataSeries.$dirty && forms.targetDataSeriesForm.targetDataSeries.$error.required">
                {{ i18n.__('Operation is required') }}
              </span>

              <div class="col-md-6">
                <div class="col-align-self-start">
                  <div class="form-group has-feedback" terrama2-show-errors>
                    <label>{{$ctrl.monitoredDataSeriesTitle}}:</label>
                    <select class="form-control"
                            name="targetMonitoredDataSeries"
                            ng-model="$ctrl.targetDataSeries"
                            ng-change="$ctrl.onChangeStaticDataSeries()"
                            ng-options="targetDS as targetDS.name for targetDS in $ctrl.DataSeriesService.staticDataSeries()"
                            ng-required="true">
                    </select>
                  </div>
                </div>
              </div>

              <!-- required -->
              <span class="help-block"
              ng-show="forms.targetDataSeriesForm.targetDataSeries.$dirty && forms.targetDataSeriesForm.targetDataSeries.$error.required">
              {{ i18n.__('Monitored Data Series is required') }}
              </span>

              <div class="col-md-6">
                <div class="form-group" terrama2-show-errors>
                  <label>{{$ctrl.attributeIdentifierTitle}}:</label>
                  <input type="text" class="form-control" name="name" ng-model="$ctrl.metadata[$ctrl.targetDataSeries.name].identifier" required>
                </div>
              </div>

              <div class="col-md-12">
                <div class="row">
                  <div class="col-md-6">
                    <div class="col-align-self-start">
                      <div class="form-group has-feedback" terrama2-show-errors>
                        <label>{{$ctrl.dynamicDataSeriesTitle}}:</label>
                          <select class="form-control"
                                  name="targetDynamicDataSeries"
                                  ng-model="$ctrl.model.dynamicDataSeries"
                                  ng-options="targetDS.id.toString() as targetDS.name for targetDS in $ctrl.DataSeriesService.dynamicDataSeries()"
                                  ng-required="true">
                          </select>
                      </div>
                    </div>
                  </div>
                </div>
              </div>

              <!-- required -->
              <span class="help-block"
                    ng-show="forms.targetDataSeriesForm.targetDataSeries.$dirty && forms.targetDataSeriesForm.targetDataSeries.$error.required">
                {{ i18n.__('Data Series is required') }}
              </span>
              `
  };
  return component;
})
