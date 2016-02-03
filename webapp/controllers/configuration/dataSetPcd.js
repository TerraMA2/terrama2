module.exports = function(app)
{
    function dataSetPcdController(request, response)
    {
        var dataSetType = request.query.type;

        if (dataSetType != "static" && dataSetType != "dynamic")
        {
            response.statusCode = 404;
            response.send('404. Not found');
        }
        else
        {
            response.render('configuration/dataSetPcd', {kind: "PCD", state: dataSetType});
        }
    }

    return dataSetPcdController;
}