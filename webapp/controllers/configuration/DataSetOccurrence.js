module.exports = function(app)
{
    function dataSetOccurrenceController(request, response)
    {
        var Utils = require('../../helpers/Utils');
        Utils.UrlHandler(request, response, "Occurrence", 'configuration/dataSetOccurrence');
    }

    return dataSetOccurrenceController;
}