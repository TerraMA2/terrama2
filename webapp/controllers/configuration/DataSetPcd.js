module.exports = function(app)
{
    function dataSetPcdController(request, response)
    {
        var Utils = require('../../helpers/Utils');
        Utils.UrlHandler(request, response, "PCD", 'configuration/dataSetPcd');
    }

    return dataSetPcdController;
}