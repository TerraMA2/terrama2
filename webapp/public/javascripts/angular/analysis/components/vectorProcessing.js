define([],()=> {
  class VectorProcessingComponent {
    constructor(i18n, SpatialOperations, DataSeriesService, DataProviderService, $timeout) {
      this.operationsTitle = i18n.__("Operations");
      this.monitoredDataSeriesTitle = i18n.__("Monitored Data Series");
      this.attributeIdentifierTitle = i18n.__("Attribute Identifier");
      this.dynamicDataSeriesTitle = i18n.__("Dynamic Data Series");
      this.SpatialOperations = SpatialOperations;
      this.DataSeriesService = DataSeriesService;
      this.DataProviderService = DataProviderService;
      this.$timeout = $timeout;
      this.columnsList = []

      if (!this.model) {
        this.model = { queryBuilder: '' };
      }
    }

    async onChangeStaticDataSeries() {
      const { model, targetDataSeries } = this;

      if (!targetDataSeries)
        return;

      const id = targetDataSeries.data_provider_id;

      model.data_provider_id = id;

      await this.listAttributes();
    }

    async listAttributes() {
      const { DataProviderService, targetDataSeries, $timeout } = this;
      const tableName = targetDataSeries.dataSets[0].format.table_name;

      const options = {
        providerId: targetDataSeries.data_provider_id,
        objectToGet: "column",
        tableName
      }

      const res = await DataProviderService.listPostgisObjects(options);
        this.columnsList = res.data.data.map(item => item.column_name);
    }

    onOperatorClicked(item) {
      if (this.model.queryBuilder)
        this.model.queryBuilder += item.code;
      else
        this.model.queryBuilder = item.code;
    }
  }

  VectorProcessingComponent.$inject = ["i18n",
                                       "SpatialOperations",
                                       "DataSeriesService",
                                       "DataProviderService",
                                       '$timeout'];

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

                  <terrama2-text-select selected-item="$ctrl.metadata[$ctrl.targetDataSeries.name].identifier"
                                        items="$ctrl.columnsList"></terrama2-text-select>
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

              <div class="col-md-12">
                <div class="row">
                  <div class="col-md-12">
                    <!--<query-builder css="ctrl.css" model="$ctrl.model.queryBuilder">
                      <terrama2-analysis-helpers ng-if="$ctrl.columnsList.length !== 0"
                                                 operators="$ctrl.attributes"
                                                 target="$ctrl.operatorValue"
                                                 on-item-clicked="$ctrl.onOperatorClicked(item)">
                      </terrama2-analysis-helpers>
                    </query-builder>
                    -->

                    <query-builder-wrapper
                      model="$ctrl.model.queryBuilder"
                      dataseries="$ctrl.targetDataSeries">
                    </query-builder-wrapper>
                  </div>
                </div>
              </div>
              `
  };
  return component;
})
