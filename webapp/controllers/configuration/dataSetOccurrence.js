module.exports = function(app)
{
    function dataSetOccurrenceController(request, response)
    {
        response.render("configuration/dataSetOccurrence");
    }

    return dataSetOccurrenceController;
}