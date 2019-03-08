define([], () => {
  class MapController {
    constructor(MapService, $element, $timeout, $http) {
      this.MapService = MapService;
      this.$element = $element;
      this.$timeout = $timeout;
      this.$http = $http;
    }

    async $onInit() {
      const { MapService, id, $element, $timeout } = this;

      await $timeout();

      MapService.initialize($element.find(`#${id}`)[0]);

      // MapService.addDrawLayer();

      // const res = await this.$http.get('/api/DataSeries/format/wkt?id=3&where=gid=613');

      // MapService.addLayerFromWKT('estados_ams', res.data, 'EPSG:4326');
    }

    getWidth() {
      return this.width || 400;
    }

    getHeight() {
      return this.height || 400;
    }
  }

  MapController.$inject = ['MapService', '$element', '$timeout', '$http'];

  const component = {
    bindings: {
      id: '@',
      height: '<?',
      width: '<?',
    },
    controller: MapController,
    template: `<div id="{{ $ctrl.id }}" class="map" style="width: {{ $ctrl.getWidth() }}px; height: {{ $ctrl.getHeight() }}px;"></div>`,
  };

  return component;

  // export default component;
});