define([], () => {
  class QueryBuilderController {
    constructor($scope, DataSeriesService, MapService) {
      this.$scope = $scope;
      this.DataSeriesService = DataSeriesService;
      this.MapService = MapService;
    }

    init(model) {
      console.log(model);
    }

    async onChange() {
      const { mapId } = this.$scope.form;
      const { MapService, DataSeriesService } = this;

      if (!mapId) {
        console.warn(`The Query Builder component is associated with mapId ${mapId} but it does not exist. The component may not work properly to visualize on Map`)
        return;
      }

      const { query_builder, table_name, view_name } = this.$scope.model;


      const wkts = await DataSeriesService.getWKT(table_name, 2, query_builder);

      MapService.addLayerFromWKT(view_name, wkts, 'EPSG:4326');
    }
  }

  QueryBuilderController.$inject = [ '$scope', 'DataSeriesService', 'MapService' ];

  return QueryBuilderController;
});