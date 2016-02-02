module.exports = function(app)
{
    function dataSetOccurrenceController(request, response)
    {
        //temp code
        var dataSetType = request.query.type;

        if (dataSetType != "static" && dataSetType != "dynamic")
        {
            response.statusCode = 404;
            response.send('404. Not found');
        }
        else
        {
            response.render('configuration/dataSetOccurrence', {kind: "Occurrence", state: dataSetType});
        }
    }

    return dataSetOccurrenceController;
}