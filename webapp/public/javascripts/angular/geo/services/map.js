// import Map from 'ol/Map.js';
// import View from 'ol/View.js';
// import {Draw, Modify, Snap} from 'ol/interaction.js';
// import {Tile as TileLayer, Vector as VectorLayer} from 'ol/layer.js';
// import {OSM, Vector as VectorSource} from 'ol/source.js';
// import {Circle as CircleStyle, Fill, Stroke, Style} from 'ol/style.js';

define([
  // 'Global/ol/View',
  // 'Global/ol/layer/Tile',
  // 'Global/ol/source',
  // 'Global/ol/style'
], (
  // Map,
  // View,
  // // { Draw, Modify, Snap },
  // TileLayer,
  // _source,
  // _style
) => {

  /**
   * An abstraction of Map Visualizer. Currently handles **OpenLayers**
   *
   * **This component uses single map instance.**
   *
   * @todo Implement map list in order to have multiple map instances
   *
   * @param {angular.$http} $http Angular HTTP module
   * @param {angular.$timeout} $timeout Angular Internal Timeout module
   */
  class MapService {
    constructor($http, $timeout) {
      this.$http = $http;
      this.$timeout = $timeout;
      this._map = null;
      this._layers = [];
    }

    initialize(selector) {
      this._map = new ol.Map({
        layers: [
          new ol.layer.Tile({
            source: new ol.source.OSM()
          }),
          ...this._layers
        ],
        // target: 'modoEspecialista',
        view: new ol.View({
          projection: 'EPSG:4326',
          center: [0, 0],
          zoom: 2
        })
      });
      this._map.setTarget(selector);
    }

    _internalAddDrawInteraction(source) {
      const modify = new ol.interaction.Modify({ source });
      this._map.addInteraction(modify);

      let draw, snap; // global so we can remove them later

      const addInteractions = () => {
        draw = new ol.interaction.Draw({
          source,
          type: 'Polygon'
        });
        this._map.addInteraction(draw);
        snap = new ol.interaction.Snap({ source});
        this._map.addInteraction(snap);
      }

      addInteractions();
    }

    getExtentOfDrawLayer() {
      const { _layers } = this;

      let vectorLayer = null;

      for(const layer of _layers) {
        if (layer._internalTerraMA2 = 'draw') {
          vectorLayer = layer;
          break;
        }
      }

      if (!vectorLayer)
        return undefined;

      return vectorLayer.getSource().getExtent();
    }

    addDrawLayer() {
      const source = new ol.source.Vector();
      const vector = new ol.layer.Vector({
        source,
        style: new ol.style.Style({
          fill: new ol.style.Fill({
            color: 'rgba(255, 255, 255, 0.2)'
          }),
          stroke: new ol.style.Stroke({
            color: '#ffcc33',
            width: 2
          }),
          image: new ol.style.Circle({
            radius: 7,
            fill: new ol.style.Fill({
              color: '#ffcc33'
            })
          })
        })
      });
      vector._internalTerraMA2 = 'draw';

      // Add layer to the stack
      this._internalAddLayer(vector);
      this._internalAddDrawInteraction(source);

      setTimeout(() => {
        this.getExtentOfDrawLayer();
      }, 5000)
    }

    _internalAddLayer(layer) {
      this._layers.push(layer);
      this._map.addLayer(layer);
    }

    addLayerFromWKT(layerName, wkts, projection) {
      const format = new ol.format.WKT();

      const features = [];
      for(const wkt of wkts) {
        const feature = format.readFeature(wkt.wkt, {
          dataProjection: 'EPSG:4326',
          featureProjection: projection
        });

        features.push(feature);
      }

      const vectorLayer = new ol.layer.Vector({
        source: new ol.source.Vector({ features })
      });

      vectorLayer._internalId = layerName;
      vectorLayer.setZIndex(1);

      this._internalAddLayer(vectorLayer);

      this.zoomToLayer(layerName);
    }

    getLayer(layerName) {
      const { _layers } = this;

      return _layers.find(layer => layer._internalId === layerName);
    }

    removeLayer(layerName) {
      const layer = this.getLayer(layerName);

      if (layer) {
        this._map.removeLayer(layer);

        const indexToRemove = this._layers.map(internalLayer => internalLayer._internalId).indexOf(layerName);

        this._layers.splice(indexToRemove, 1);
      }
    }

    zoomTo(extent, animation = false) {
      const opts = { };

      if (animation) {
        opts.duration = 1000;
      }

      this._map.getView().fit(extent, opts);
    }

    zoomToLayer(layerName) {
      const { _layers } = this;
      const vectorLayer = _layers.find(layer => layer._internalId === layerName);

      if (!vectorLayer)
        throw new Error(`No layer ${layerName} found`);

      this.zoomTo(vectorLayer.getSource().getExtent());
    }

    getMap() {
      return this._map;
    }
  }

  MapService.$inject = ['$http', '$timeout'];

  return MapService;

  // export default MapService;
});