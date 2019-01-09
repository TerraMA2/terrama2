define([],function(){
  /**
   * Componenent responsible to handle CSV metadata
   *
   * Used for DCP Customized or Occurrence CSV.
   *
   * You can define a list of CSV fields containing Type, and the respective context.
   *
   * The current values are: "DATE", "TEXT", "FLOAT", "INTEGER"
   * There are also special values for occurrence: "GEOMETRY"
   *
   * @example
   *
   * <csv-format csv-format-data="model"
   *             semantics="dataSeriesSemantics"
   *             on-item-added="detectChanges(item)"
   *             on-item-removed="detectChanges(item)"
   *             on-item-changed="detectChanges(item)">
   * </csv-format>
   */
  class CSVComponent {
    constructor(i18n) {
      this.i18n = i18n;
      // Types of fields in csv file
      this.fieldsType = [
        { title: "Point (Geometry)", value: "GEOMETRY_POINT", defaultType: false },
        { title: "Float", value: "FLOAT", defaultType: true },
        { title: "Integer", value: "INTEGER", defaultType: true },
        { title: "Text", value: "TEXT", defaultType: true },
        { title: "Date", value: "DATETIME", defaultType: false }
      ];
    }
    // Function to add new empty format in model
    addField(){
      const newFormat = { type: "FLOAT" };
      this.csvFormatData.fields.push(newFormat);
      // Notify when a item has been added
      this.onItemAdded({ item: newFormat });
    }

    /**
     * Listener from CSV Field to detect when field has changed
     * @param {any} item Field changed
     */
    changeField(item) {
      this.onItemChanged({ item });
    }
    /**
     * Delete field from CSV list and then notify listener
     */
    deleteField(format) {
      const idx = this.csvFormatData.fields.indexOf(format);
      if (idx >= 0) {
        const itemRemoved = this.csvFormatData.fields[idx];

        this.csvFormatData.fields.splice(idx, 1);
        // Notify when a item has been removed
        this.onItemRemoved({ item: itemRemoved });
      }
    };
  }
  // Angular dependencies
  CSVComponent.$inject = ['i18n'];

  /**
   * It defines a Component structure for handling CSV format form on Dynamic data registration
   *
   * @property {Object} bindings - Defines component bindings to work
   */
  const terrama2CsvFormatComponent = {
    bindings: {
      csvFormatData: '<',
      semantics: '<',
      // Notify when field has been added
      onItemAdded: '&',
      // Notify when field has been removed
      onItemRemoved: '&',
      // Notify when field has been changed
      onItemChanged: '&'
    },
    templateUrl: BASE_URL + 'dist/templates/data-series/templates/csvFormat.html',
    controller: CSVComponent
  };

  return terrama2CsvFormatComponent;
})
