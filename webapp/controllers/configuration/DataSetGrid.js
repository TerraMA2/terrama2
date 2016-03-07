module.exports = function(app)
{
    function dataSetGridController(request, response)
    {
        var Utils = require('../../helpers/Utils');
        Utils.UrlHandler(request, response, "Grid", 'configuration/dataSetGrid');
    }

    return dataSetGridController;
}