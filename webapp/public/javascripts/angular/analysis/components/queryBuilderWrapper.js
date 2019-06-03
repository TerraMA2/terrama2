define([], () => {
  class QueryBuilderCommon {
    constructor(DataProviderService, $timeout, $scope) {
      this.DataProviderService = DataProviderService;
      this.$timeout = $timeout;

      this.columns = []

      this.queryOperators = {
        name: "Utilities",
        fileName: "query-operators.json",
        imagePath: "images/analysis/functions/utilities/utilities.png"
      }

      $timeout(() => {
        $scope.$watch('$ctrl.dataseries', value => {
          if (value && Object.keys(value).length > 1) {
            this.listAttributes();
          }
        })
      })
    }

    async listAttributes() {
      const { DataProviderService, $timeout } = this;

      const {dataseries } = this;
      const tableName = dataseries.dataSets[0].format.table_name;

      const options = {
        providerId: dataseries.data_provider_id,
        objectToGet: "column",
        tableName
      }

      $timeout(async() => {
        const res = await DataProviderService.listPostgisObjects(options);

        this.columns = res.data.data.map(item => (
          { name: item.column_name, code: item.column_name }
        ));

        this.attributes = {
          data: this.columns,
          imagePath: "images/analysis/functions/monitored-object/attributes/attributes.png"
        }
      })
    }

    onOperatorClicked(item) {
      if (!this.model)
        this.model = { queryBuilder: '' };

      this.model.queryBuilder += item.code;
    }
  }
  QueryBuilderCommon.$inject = ['DataProviderService', "$timeout", '$scope'];

  const queryBuiderCommonComponent = {
    bindings: {
      model: '=',
      dataseries: '='
    },
    controller: QueryBuilderCommon,
    template: `
      <query-builder css="ctrl.css" model="$ctrl.model">
        <terrama2-analysis-helpers ng-if="$ctrl.columns.length !== 0"
                                  operators="$ctrl.attributes"
                                  target="$ctrl.model"
                                  on-item-clicked="$ctrl.onOperatorClicked(item)">
        </terrama2-analysis-helpers>
        <terrama2-analysis-helpers operators="$ctrl.queryOperators"
                                  target="$ctrl.model"
                                  on-item-clicked="$ctrl.onOperatorClicked(item)">
        </terrama2-analysis-helpers>
      </query-builder>
    `
  }

  return queryBuiderCommonComponent;
})