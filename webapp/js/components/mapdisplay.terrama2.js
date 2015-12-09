var MapDisplay = function() {

  var getMap = function() {
    return olMap;
  }

  var createTileWMS = function(url, type, layerName, layerTitle) {
    return new ol.layer.Tile({
      source: new ol.source.TileWMS({
        preload: Infinity,
        url: url,
        serverType: type,
        params: {
          'LAYERS': layerName, 'TILED': true
        }
      }),
      name: layerName,
      title: layerTitle,
      visible: false
    });
  }

  this.getMap = getMap;
  this.createTileWMS = createTileWMS;

  var olMap = new ol.Map({
    renderer: 'canvas',
    layers: [
      new ol.layer.Tile({
        source: new ol.source.OSM(),
        name: 'Base',
        title: 'Base'
      })
    ],
    target: 'terrama2-map',
    view: new ol.View({
      projection: 'EPSG:4326',
      center: [-55, -15],
      zoom: 4
    })
  });

  olMap.getLayerGroup().set('name', 'root');
  olMap.getLayerGroup().set('title', 'Geoserver Local');
}
