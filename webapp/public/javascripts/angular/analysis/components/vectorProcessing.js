define([],()=> {
  class VectorProcessingComponent {
    constructor(i18n, SpatialOperations, DataSeriesService, DataProviderService, $timeout) {
      this.operationsTitle = i18n.__("Operations");
      this.monitoredDataSeriesTitle = i18n.__("Monitored Data Series");
      this.attributeIdentifierTitle = i18n.__("Attribute Identifier");
      this.dynamicDataSeriesTitle = i18n.__("Dynamic Data Series");

      // this.startDate = i18n.__("Start Date");
      // this.endDate = i18n.__("End Date");

      this.SpatialOperations = SpatialOperations;
      this.DataSeriesService = DataSeriesService;
      this.DataProviderService = DataProviderService;
      this.$timeout = $timeout;
      this.columnsList = [];
      this.inputTableAttributes = {};

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

    async onChangeDynamicDataSeries() {

      const { DataProviderService } = this;

      const dynamicDataSeries = this.DataSeriesService.dynamicDataSeries().find(ds => (
        ds.id === parseInt(this.model.dynamicDataSeries)
      ));

      const tableNameFromAnalysisTable = await DataProviderService.listPostgisObjects({providerId: dynamicDataSeries.data_provider_id,
                                                                objectToGet: "values",
                                                                tableName : dynamicDataSeries.dataSets[0].format.table_name,
                                                                columnName: "table_name"});
      let tableNameAttributes = "";

      try {
        tableNameAttributes = tableNameFromAnalysisTable.data.data[0];
      }
      catch(error) {
        console.log(error);
      }

      const options = {
        providerId: dynamicDataSeries.data_provider_id,
        objectToGet: "column",
        tableName : tableNameAttributes === "" ? dynamicDataSeries.dataSets[0].format.table_name : tableNameAttributes
      }

      const res = await DataProviderService.listPostgisObjects(options);
      let dynamicTableAttributes = res.data.data.map(item => item.column_name);

      dynamicTableAttributes.forEach(attribute => {
        this.inputTableAttributes[attribute] = dynamicDataSeries.name + ":" + attribute;
      });
    }

    // async onChangeStartDate() {
    //   this.model.startDate = moment(this.model.startDate).format("YYYY-MM-DD");

    //   if(!this.model.startDate)
    //     this.model.startDate = '';
    // }

    // async onChangeEndDate() {
    //   this.model.endDate = moment(this.model.endDate).format("YYYY-MM-DD");

    //   if(!this.model.endDate)
    //     this.model.endDate = '';
    // }

    async listAttributes() {
      const { DataProviderService, targetDataSeries, $timeout } = this;

      const options = {
        providerId: targetDataSeries.data_provider_id,
        objectToGet: "column",
        tableName : targetDataSeries.dataSets[0].format.table_name
      }

      const res = await DataProviderService.listPostgisObjects(options);
      this.columnsList = res.data.data.map(item => item.column_name);

      this.columnsList.forEach(attribute => {
        this.inputTableAttributes[attribute] = targetDataSeries.name + ":" + attribute;
      });
    }

    async onMultInputSelected() {
      const { model } = this;

      model.data = {
        inputAttributesLayer: []
       };
    }

    getTableName() {
      if (!this.targetDataSeries || angular.equals({}, this.targetDataSeries))
        return "";

      return this.targetDataSeries.dataSets[0].format.table_name;
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
                    <label>Static Data Series:</label>
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

              <!-- <div class="col-md-6">
                <div class="form-group" terrama2-show-errors>
                  <label>{{$ctrl.attributeIdentifierTitle}}:</label>

                  <terrama2-text-select selected-item="$ctrl.metadata[$ctrl.targetDataSeries.name].identifier"
                                        items="$ctrl.columnsList"></terrama2-text-select>
                </div>
              </div> -->

              <div class="col-md-12">
                <div class="row">

                  <div class="col-md-6">
                    <div class="col-align-self-start">
                      <div class="form-group has-feedback" terrama2-show-errors>
                        <label>{{$ctrl.dynamicDataSeriesTitle}}:</label>
                          <select class="form-control"
                                  name="targetDynamicDataSeries"
                                  ng-model="$ctrl.model.dynamicDataSeries"
                                  ng-change="$ctrl.onChangeDynamicDataSeries()"
                                  ng-options="targetDS.id.toString() as targetDS.name for targetDS in $ctrl.DataSeriesService.dynamicDataSeries()"
                                  ng-required="true">
                          </select>
                      </div>
                    </div>
                  </div>
                  <!--
                  <div class='col-md-3'>
                    <div class="form-group">
                      <label>{{$ctrl.startDate}}:</label>
                      <div class="row">
                        <div class='col-md-12 col-sm-12'>
                          <div class="form-group">
                            <input type="date" class="form-control"
                                               name="startDateFilter"
                                               ng-model="$ctrl.model.startDate"
                                               ng-change="$ctrl.onChangeStartDate()"/>
                          </div>
                        </div>
                      </div>
                    </div>
                  </div>

                  <div class='col-md-3'>
                    <div class="form-group">
                      <label>{{$ctrl.endDate}}:</label>
                      <div class="row">
                        <div class='col-md-12 col-sm-12'>
                          <div class="form-group">
                            <input type="date" class="form-control"
                                               name="endDateFilter"
                                               ng-model="$ctrl.model.endDate"
                                               ng-change="$ctrl.onChangeEndDate()"/>
                          </div>
                        </div>
                      </div>
                    </div>
                  </div>
                  -->

                </div>
              </div>

              <!-- required -->
              <span class="help-block"
                    ng-show="forms.targetDataSeriesForm.targetDataSeries.$dirty && forms.targetDataSeriesForm.targetDataSeries.$error.required">
                {{ i18n.__('Data Series is required') }}
              </span>

              <div class="col-md-6">
                <div class="col-align-self-start">
                  <div class="form-group has-feedback" terrama2-show-errors>
                    <label>Input Attribute Layer:</label>
                    <select class="form-control"
                      name="inputAttributesLayer"
                      id="inputAttributesLayer"
                      ng-model="data.inputAttributesLayer"
                      ng-options="key as value for (key, value) in $ctrl.inputTableAttributes"
                      ng-required="true" multiple>
                    </select><br>
                  </div>
                </div>
              </div>

              <div class="col-md-6">
                <div class="form-group" terrama2-show-errors>
                  <label>Output Attribute Layer:</label>
                  <select class="form-control"
                    name="outputlayer"
                    id="outputlayer"
                    ng-model="$ctrl.model.outputlayer"
                    ng-change="$ctrl.onOutputLayerAttributesSelected()"
                    ng-options="attributes for attributes in data.inputAttributesLayer"
                    ng-required="true" multiple>
                </select><br>
                </div>
              </div>

            <!--  <div class="col-md-12">
                <div class="row">
                  <div class="col-md-12">
                    <query-builder-wrapper ng-if="$ctrl.getTableName() !== ''"
                      model="$ctrl.model.queryBuilder"
                      provider="$ctrl.targetDataSeries.data_provider_id"
                      table-name="$ctrl.getTableName()">
                    </query-builder-wrapper>
                  </div>
                </div>
              </div> -->
              `
  };
  return component;
})
