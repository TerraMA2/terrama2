module.exports = {
  /**
   * It defines structure for display fields in web forms
   * @readonly
   * @enum {Form}
   */
  Form: {
    /**
     * A type defines how group values
     */
    Type: {
      OBJECT: 'object', // It represents a fieldset
      ARRAY: 'array',   // It represents an array, a tab set
      STRING: 'text'    // It represents either select (combobox) or a simple text
    },

    /**
     * A field defines how to display a field. It design a input
     */
    Field: {
      TEXT: "string",
      NUMBER: "number",
      CHECKBOX: "boolean",
      COMBOBOX: "string",
      TEXTAREA: "textarea",
      PASSWORD: "password"
    }
  },

  /**
   * It is used for handling structure for custom request syntax
   * @readonly
   * @enum {FormField}
  */
  Uri: {
    HOST: 'hostname',
    PORT: 'port',
    SCHEME: 'protocol',
    PATHNAME: 'pathname',
    AUTH: 'auth',
    USER: 'user',
    PASSWORD: 'password'
  },
  
  DataSeriesType: {
    DCP: 'Dcp',
    OCCURRENCE: 'Occurrence',
    GRID: 'Grid',
    MONITORED: 'Monitored'
  },
  
  DataSeriesFormat: {
    CSV: 'CSV'
  },

  DataSeriesSemantics: {
    NAME: "name",
    FORMAT: "data_format_name",
    TYPE: "data_series_type_name"
  },
  
  DataProviderIntent: {
    COLLECT: 'COLLECT',
    PROCESSING: 'PROCESSING'
  },

  /**
   * It is used for representing output formats
   * @readonly
   * @enum {Format}
   */
  Format: {
    GEOJSON: 'GeoJSON',
    WKT: 'wkt'
  }
};
