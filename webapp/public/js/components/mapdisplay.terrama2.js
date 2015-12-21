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

  /**right
  * @param {any} value
  * @returns {ol.layer.Base}
  */
  var findBy = function(layer, key, value) {

    if (layer.get(key) === value) {
      return layer;
    }

    if (layer.getLayers) {
      var layers = layer.getLayers().getArray(),
      len = layers.length, result;
      for (var i = 0; i < len; i++) {
        result = findBy(layers[i], key, value);
        if (result) {
          return result;
        }
      }
    }

    return null;
  }

  var updateMapSize = function() {
    olMap.updateSize();
  }

  this.getMap = getMap;
  this.createTileWMS = createTileWMS;
  this.findBy = findBy;
  this.updateMapSize = updateMapSize;

  var olMap = new ol.Map({
    renderer: 'canvas',
    layers: [
      new ol.layer.Group({
        layers: [
          new ol.layer.Tile({
            source: new ol.source.OSM(),
            name: 'osm',
            title: 'Open Street Map',
            visible: false
          }),
          new ol.layer.Tile({
            source: new ol.source.MapQuest({layer: 'osm'}),
            name: 'mapquest_osm',
            title: 'MapQuest OSM',
            visible: false
          }),
          new ol.layer.Tile({
            source: new ol.source.MapQuest({layer: 'sat'}),
            name: 'mapquest_sat',
            title: 'MapQuest Sat&eacute;lite',
            visible: true
          })
        ],
        name: 'bases',
        title: 'Camadas Base'
      })
    ],
    target: 'terrama2-map',
    logo: '/terrama2/webapp/public/img/logo_terrama2.png',
    view: new ol.View({
      projection: 'EPSG:4326',
      center: [-55, -15],
      zoom: 4
    })
  });

  olMap.getLayerGroup().set('name', 'root');
  olMap.getLayerGroup().set('title', 'Geoserver Local');

  $("#terrama2-map").find('div.ol-zoom').removeClass('ol-zoom').addClass('terrama2-map-simple-zoom');
  $("#terrama2-map").find('div.ol-attribution').addClass('terrama2-map-attribution');

  $(document).ready(function() {
    olMap.on('ready', function() {
      updateMapSize();
    });
  });
}
