module.exports = function(app) {

    var controllers = {
        index: function(request, response){
            response.render("configuration/alerts");
        },
        new: function(request, response){
            response.render("configuration/alert");
        }
    }
    return controllers;
};