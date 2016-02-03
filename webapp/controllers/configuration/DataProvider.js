module.exports = function(app) {

    function staticDataProviderController(request, response)
    {
        var Utils = require('../../helpers/Utils');
        Utils.UrlHandler(request, response, "DataProvider", 'configuration/provider');
    };

    return staticDataProviderController;
};