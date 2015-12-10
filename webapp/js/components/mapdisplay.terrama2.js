var MapDisplay = function() {

  var getMap = function() {
    return olMap;
  }

  var createTileWMS = function(layerName, layerTitle) {
    return new ol.layer.Tile({
      source: new ol.source.TileWMS({
        preload: Infinity,
        url: 'http://sigma.cptec.inpe.br/cgi-bin/mapserv?map=/extra2/sigma/www/webservice/relatorio_queimadas.map',
        serverType:'mapserver',
        params:{
          'LAYERS':layerName, 'TILED':true
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
    target: 'terrama2-map',
    renderer: 'canvas',
    layers: [
      new ol.layer.Tile({
        source: new ol.source.TileWMS({
          preload: Infinity,
          url: 'http://sigma.cptec.inpe.br/cgi-bin/mapserv?map=/extra2/sigma/www/webservice/relatorio_queimadas.map',
          serverType:'mapserver',
          params:{
            'LAYERS':"vegetacao", 'TILED':true
          }
        }),
        name: "Base",
        title: "Base"
      })
    ],
    view: new ol.View({
      projection: 'EPSG:4326',
      center: [-55, -15],//ol.proj.fromLonLat([-55, -15]),
      zoom: 3
    })
  });

  olMap.getLayerGroup().set('name', 'root');
  olMap.getLayerGroup().set('title', 'Geoserver Local');
}
