module.exports = function (app) {
    'use strict';

    var controller = app.controllers.api.ConsolidatedUseArea;

    app.get(app.locals.BASE_URL + 'api/consolidatedUseArea/getAll', controller.getAll);
};
