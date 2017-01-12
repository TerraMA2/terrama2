var Model = require("./../../data-model/DataSeries");

/**
 * It simulates a DataSeries creation. It is used to build an DataSeries in order to validate it before save in database
 *  
 * @return {DataSeries}
 */
function DataSeriesBuilder(dataSeriesObject) {
  var dataSeries = new Model(dataSeriesObject);
  dataSeries.id = dataSeriesObject.id || 999;
  dataSeries.dataSets.forEach(function(dataSet, dataSetIndex) {
    dataSet.data_series_id = dataSeries.id;
    dataSet.id = dataSeries.id + dataSetIndex;
  });

  return dataSeries;
}


module.exports = DataSeriesBuilder;