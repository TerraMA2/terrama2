module.exports = function(app) {

    function DataSetController(request, response)
    {
        var Utils = require('../../helpers/Utils');
        Utils.UrlHandler(request, response, "DataSet", 'configuration/dataset');
    };

    return DataSetController;
};