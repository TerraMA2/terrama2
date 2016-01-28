module.exports = function(app)
{
    function dataSetPcdController(request, response)
    {
        response.render("configuration/dataSetPcd");
    }

    return dataSetPcdController;
}