define([], () => {
  class QueryBuilderController {
    constructor($scope, DataSeriesService, MapService) {
      this.$scope = $scope;
      this.DataSeriesService = DataSeriesService;
      this.MapService = MapService;

      $scope.$watch(() => this.$scopew.evalExpr('table_name'))
    }

    /**
     * Retrieves dataprovider from parent instance
     * @return {number}
     */
    getProvider() {
      if (!this.$scope.form.provider)
        return;

      const expression = this.$scope.form.provider;

      const res = this.$scope.evalExpr(expression);

      if (!res)
        return 0;

      return res;
    }

    /**
     * Detects user change iteractions on QueryBuilder component
     *
     * **Its already debounced function**
     */
    async onChange() {
      const { mapId } = this.$scope.form;
      const { MapService, DataSeriesService } = this;

      if (!mapId) {
        console.warn(`The Query Builder component is associated with mapId ${mapId} but it does not exist. The component may not work properly to visualize on Map`)
        return;
      }

      /**
       * Retrieves Map instance from MapService. It is injected by default when use <terrama2-map> directive
       * @type {MapContainer}
       */
      const map = MapService.getMap(mapId);

      const { query_builder, table_name, view_name } = this.$scope.model;

      // Get data provider from instance in order to detect which database should list
      const provider = this.getProvider();

      // Retrieve list of WKT objects with associated parameters
      const wkts = await DataSeriesService.getWKT(table_name, provider, query_builder);

      // If there is already a layer, just remove
      if (map.getLayer(view_name))
        map.removeLayer(view_name);

      // Add geometry polygon into map instance
      map.addLayerFromWKT(view_name, wkts, 'EPSG:4326');
    }
  }

  QueryBuilderController.$inject = [ '$scope', 'DataSeriesService', 'MapService' ];

  return QueryBuilderController;
});