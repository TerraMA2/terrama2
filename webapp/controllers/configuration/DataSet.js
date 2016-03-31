module.exports = function(app) {

    function DataSetController(request, response)
    {
        var Utils = require('../../helpers/Utils');
        var dataSeries = Object.assign({}, request.query);

        delete dataSeries.type;

        Utils.UrlHandler(request, response, "DataSet", 'configuration/dataset', {dataSeries: dataSeries});
    };

    return DataSetController;
};