module.exports = function(app) {

    function loginController(request, response)
    {
        response.render('login');
    };

    return loginController;
};