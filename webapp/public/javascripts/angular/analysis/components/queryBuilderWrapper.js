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
    }

    $onInit() {
      this.listAttributes();
    }

    async listAttributes() {
      const { DataProviderService, tableName, provider, $timeout } = this;

      const options = {
        providerId: provider,
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
        this.model = '';

      this.model += ` ${item.code} `;
    }
  }
  QueryBuilderCommon.$inject = ['DataProviderService', "$timeout", '$scope'];

  const queryBuiderCommonComponent = {
    bindings: {
      model: '=',
      tableName: '=',
      provider: '=',
      onChange: '&?'
    },
    controller: QueryBuilderCommon,
    template: `
      <query-builder css="ctrl.css" model="$ctrl.model" on-change="$ctrl.onChange()">
        <terrama2-analysis-helpers ng-if="$ctrl.columns.length !== 0"
                                  operators="$ctrl.attributes"
                                  target="$ctrl.model"
                                  on-item-clicked="$ctrl.onOperatorClicked(item)">
        </terrama2-analysis-helpers>
        <terrama2-analysis-helpers ng-if="$ctrl.columns.length == 99" 
                                  operators="$ctrl.queryOperators"
                                  target="$ctrl.model"
                                  on-item-clicked="$ctrl.onOperatorClicked(item)">
        </terrama2-analysis-helpers>
      </query-builder>
    `
  }

  return queryBuiderCommonComponent;
})