module.exports = function(app)
{
    function analyseController(request, response)
    {
        response.render("configuration/analyses");
    }

    return analyseController;
}