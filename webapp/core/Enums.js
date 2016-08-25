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

  /**
   * It represents available services type id from Database
   * @readonly
   * @enum {ServiceType}
  */
  ServiceType: {
    COLLECTOR: 1,
    ANALYSIS: 2
  },

  DataSeriesType: {
    DCP: 'DCP',
    OCCURRENCE: 'OCCURRENCE',
    GRID: 'GRID',
    ANALYSIS_MONITORED_OBJECT: 'ANALYSIS_MONITORED_OBJECT',
    STATIC_DATA: 'STATIC_DATA',
    POSTGIS: 'POSTGIS'
  },

  AnalysisType: {
    DCP: 1,
    MONITORED: 2,
    GRID: 3
  },

  AnalysisDataSeriesType: {
    DATASERIES_MONITORED_OBJECT_TYPE: 1,
    DATASERIES_GRID_TYPE: 2,
    DATASERIES_DCP_TYPE: 3,
    ADDITIONAL_DATA_TYPE: 4
  },

  DataSeriesFormat: {
    CSV: 'CSV',
    POSTGIS: 'POSTGIS',
    OGR: 'OGR',
    GEOTIFF: 'GEOTIFF',
    GRADS: 'GRADS'
  },

  DataSeriesSemantics: {
    NAME: "name",
    FORMAT: "data_format_name",
    TYPE: "data_series_type_name"
  },

  DataProviderType: {
    FILE: {
      name: "FILE",
      value: 1
    },
    FTP: {
      name: "FTP",
      value: 2
    },
    HTTP: {
      name: "HTTP",
      value: 3
    },
    POSTGIS: {
      name: "POSTGIS",
      value: 4
    }
  },

  DataProviderIntent: {
    COLLECT: 'COLLECT',
    PROCESSING: 'PROCESSING'
  },

  DataProviderIntentId: {
    COLLECT: 1,
    PROCESSING: 2
  },

  /**
   * It is used for representing output formats
   * @readonly
   * @enum {Format}
   */
  Format: {
    GEOJSON: 'GeoJSON',
    WKT: 'wkt'
  },

  TokenCode: {
    SAVE: 1,
    UPDATE: 2,
    DELETE: 3
  },

  InfluenceTypes: {
    TOUCHES: {
      Name: "Touches",
      Value: 1
    },
    CENTER: {
      Name: "Center",
      Value: 2
    },
    REGION: {
      Name: "Region",
      Value: 3
    }
  },

  ScriptLanguage: {
    PYTHON: 1,
    LUA: 2
  },

  /**
    * Possible status of manipulate data.
    */
  StatusLog: {
    ERROR: 1,
    START: 2,
    DOWNLOADED: 3,
    DONE: 4
  },

  /**
    * Possible status of logged messages.
    */
  MessageType: {
    ERROR_MESSAGE: 1,
    INFO_MESSAGE: 2,
    WARNING_MESSAGE: 3
  },

  SSH: {
    COMMON: 1,
    SCREEN: 2
  },

  /**
   * All available Operation Systems
   * @enum {string}
   */
  OS: {
    WIN: 'Windows',
    LINUX: 'Linux',
    MACOSX: 'Darwin',
    UNKNOWN: ''
  },

  /**
   * It defines the valid range port number
   * @enum {number}
   */
  Port: {
    MIN: 0,
    MAX: 65535
  },

  /**
   * All interpolation methods for Analysis Grid
   * @enum {Object}
  */
  InterpolationMethod: {
    /** Near neighborhood interpolation method. */
    NEAREST_NEIGHBOR: {
      name: "Nearest Neighbor",
      value: 1
    },
    /** Bilinear interpolation method. */
    BI_LINEAR: {
      name: "Bi linear",
      value: 2
    },
    /** Bicubic interpolation method. */
    BI_CUBIC: {
      name: "Bi cubic",
      value: 3
    }
  },

  /**
   * All area of interest type for Analysis Grid
   * @readonly
   * @enum {number}
   */
  InterestAreaType: {
    /** Use the union of the areas from the DataSeries in the analysis. */
    UNION: {
      name: "Union",
      value: 1
    },
    /** Use the same box of a given grid. */
    SAME_FROM_DATA_SERIES: {
      name: "Same from data series",
      value: 2
    },
    /** Use a custom box */
    CUSTOM: {
      name: "Custom",
      value: 3
    }
  },

  /**
   * All resolution type for Analysis Grid
   * @readonly
   * @enum {number}
   */
  ResolutionType: {
    /** Use the resolution from the smallest grid. */
    SMALLEST_GRID: {
      name: "Smallest grid",
      value: 1
    },
    /** Use the resolution from the biggest grid. */
    BIGGEST_GRID: {
      name: "Biggest grid",
      value: 2
    },
    /** Use the same resolution of a given grid. */
    SAME_FROM_DATA_SERIES: {
      name: "Same from data series",
      value: 3
    },
    /** Use a custom resolution */
    CUSTOM: {
      name: "Custom",
      value: 4
    }
  },

  Operators: {
    EQUAL: "$eq",
    GREATER_THAN: "$gt",
    GREATER_OR_EQUAL: "$gte",
    NOT_EQUAL: "$ne",
    LESS_THAN: "$lt",
    LESS_EQUAL: "$le",
    IN: "$in"
  }
};
