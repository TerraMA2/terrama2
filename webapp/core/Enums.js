module.exports = {
  /**
   * It defines structure for display fields in web forms
   * @readonly
   * @enum {FormField}
  */
  FormField: {
    TEXT: "string",
    NUMBER: "number",
    CHECKBOX: "boolean",
    COMBOBOX: "string",
    TEXTAREA: "textarea",
    PASSWORD: "password"
  },

  /**
   * It is used for handling structure for custom request syntaxes
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
    GRID: 'Grid'
  }
};
