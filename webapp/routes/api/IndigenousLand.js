module.exports = function (app) {
    'use strict';

    var controller = app.controllers.api.IndigenousLand;

    app.get(app.locals.BASE_URL + 'api/indigenousLand/getAll', controller.getAll);
};
