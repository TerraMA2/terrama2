module.exports = function(app) {

  function DataSetController(request, response) {
    var Utils = require('../../helpers/Utils');
    var dataSeries = Object.assign({}, request.query);
    var parametersData = JSON.parse(dataSeries.parametersData || "{}");
    delete dataSeries.parametersData;

    delete dataSeries.type;

    Utils.UrlHandler(request, response, "DataSet", 'configuration/dataset', {
      dataSeries: dataSeries,
      parametersData: parametersData
    });
  }

  return DataSetController;
};