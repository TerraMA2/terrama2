module.exports = function (app) {
    'use strict';

    var controller = app.controllers.api.ConservationUnit;

    app.get(app.locals.BASE_URL + 'api/conservationUnit/getAll', controller.getAll);
};
