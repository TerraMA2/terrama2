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

  var createLayerBase = function(url, type, layerName, layerTitle) {
    return createTileWMS(url, type, layerName, layerTitle);
  }

  this.getMap = getMap;
  this.createTileWMS = createTileWMS;

  var olMap = new ol.Map({
    target: 'terrama2-map',
    renderer: 'canvas',
    //layers: [ createLayerBase(url, type, layerName, layerTitle) ],
    view: new ol.View({
      projection: 'EPSG:4326',
      center: [-55, -15],//ol.proj.fromLonLat([-55, -15]),
      zoom: 3
    })
  });

  olMap.getLayerGroup().set('name', 'root');
  olMap.getLayerGroup().set('title', 'Geoserver Local');
}
