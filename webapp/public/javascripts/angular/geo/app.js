'use strict';

angular.module("terrama2.components.geo", ["terrama2", "terrama2.geo.services"])
  .constant("Geometry", {
    "POLYGON": "Polygon",
    "POINT": "Point"
  })

  .constant("GeometryFormat", {
    "WKT": "WKT",
    "GEOJSON": "GEOJSON"
  })

  .constant("GeoJSON", {
    "type": "",
    "coordinates": [],
    "crs": {
      type: 'name',
      properties : {
        name: "EPSG:"
      }
    }
  })

  .factory("Polygon", function(GeoJSON, Geometry, GeometryFormat) {
    return {
      /**
       * @param {GeoJSON} geojson - A geojson object structure
       * @param {GeometryFormat} format - A javascript object with limits value. Default - GEOJSON
       * @todo implement WKT format
       * @return {Object} a javascript object representation
       */
      read: function(geojson, format) {
        var model = {};
        if (format === undefined || !format) {
          format = GeometryFormat.GEOJSON;
        }

        switch(format) {
          case GeometryFormat.GEOJSON:
            var coordinates = geojson.coordinates[0];
            model.minX = coordinates[0][0];
            model.minY = coordinates[0][1];
            model.maxX = coordinates[2][0];
            model.maxY = coordinates[2][1];
            var defaultSRID = 4326;
            if (model.srid) {
              var sridStr = (((model.crs || {}).properties || {}).name || "");
              model.srid = parseInt(sridStr.substring(4, sridStr.length));
            } else {
              model.srid = defaultSRID;
            }
            break;
          case GeometryFormat.WKT: // TODO: implement it
            break;
        }
        return model;
      },
      /**
       * @param {Object} model - A javascript object with limits value.
       * @return {Object} a geojson object representation
       */
      build: function(model) {
        var geojson = angular.copy(GeoJSON, {});
        var coordinates = [
          [model.minX, model.minY],
          [model.minX, model.maxY],
          [model.maxX, model.maxY],
          [model.maxX, model.minY],
          [model.minX, model.minY]
        ];
        geojson.type = Geometry.POLYGON;
        geojson.coordinates.push(coordinates);
        geojson.crs.properties.name += model.srid;
        return geojson;
      }
    };
  })

  .factory("Point", function(Geometry, GeoJSON) {
    return {
      /**
       * @param {GeoJSON} geojson - A geojson object structure
       * @param {GeometryFormat} format - A javascript object with limits value. Default - GEOJSON
       * @todo implement WKT format
       * @return {Object} a javascript object representation
       */
      read: function(geojson, format) {
        var model = {};
        if (format === undefined || !format) {
          format = GeometryFormat.GEOJSON;
        }

        switch(format) {
          case GeometryFormat.GEOJSON:
            var coordinates = geojson.coordinates[0];
            model.x = coordinates[0][0];
            model.y = coordinates[0][1];
            break;
          case GeometryFormat.WKT: // TODO: implement it
            break;
        }
        return model;
      },
      /**
       * @param {Object} model - A javascript object with limits value. {x: 1, y: 2, srid: 4326}
       * @return {Object} a geojson object representation
       */
      build: function(model) {
        var geojson = angular.copy(GeoJSON, {});
        geojson.type = Geometry.POINT;
        geojson.coordinates.push(model.x);
        geojson.coordinates.push(model.y);
        geojson.crs.properties.name += model.srid;
        return geojson;
      }
    };
  })

  .factory("GeoJsonBuilder", function(Geometry, Polygon, Point) {
    /**
     * @param {Geometry} geometry - A const geometry enum
     * @param {Object} model - A javascript object with limits value
     * @return {Object} a geojson object
     */
    return function(geometry, model) {
      switch(geometry) {
        case Geometry.POLYGON:
          return Polygon.build(model);
        case Geometry.POINT:
          return Point.build(model);
        default:
          return {};
      }
    };
  });
