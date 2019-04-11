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

    getProvider() {
      if (!this.$scope.form.inject)
        return;

      const { inject } = this.$scope.form;
      const expression = inject.provider;

      const res = this.$scope.evalExpr(expression);

      if (!res)
        return 0;

      return res;
    }

    async onChange() {
      const { mapId } = this.$scope.form;
      const { MapService, DataSeriesService } = this;

      if (!mapId) {
        console.warn(`The Query Builder component is associated with mapId ${mapId} but it does not exist. The component may not work properly to visualize on Map`)
        return;
      }

      const { query_builder, table_name, view_name } = this.$scope.model;

      const provider = this.getProvider();

      const wkts = await DataSeriesService.getWKT(table_name, provider, query_builder);

      if (MapService.getLayer(view_name))
        MapService.removeLayer(view_name);

      MapService.addLayerFromWKT(view_name, wkts, 'EPSG:4326');
    }
  }

  QueryBuilderController.$inject = [ '$scope', 'DataSeriesService', 'MapService' ];

  return QueryBuilderController;
});