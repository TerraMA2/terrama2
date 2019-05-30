define([], () => {
  class MapContainer {
    constructor(id, map, layers = []) {
      this._id = id;
      this._map = map;
      this._layers = layers;
    }

    addLayer(layerName, layer) {
      layer._internalId = layerName;

      this._map.addLayer(layer);
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

    destroy() {
      this._layers = [];
      this._map = null;
    }

    getExtentOfDrawLayer() {
      const layers = this._layers();

      let vectorLayer = null;

      for(const layer of layers) {
        if (layer._internalTerraMA2 = 'draw') {
          vectorLayer = layer;
          break;
        }
      }

      if (!vectorLayer)
        return undefined;

      return vectorLayer.getSource().getExtent();
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

    set(map) {
      this._map = map;
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

    _internalAddLayer(layer) {
      this._layers.push(layer);
      this._map.addLayer(layer);
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

  }

  return MapContainer;
});