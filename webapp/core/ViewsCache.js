'use strict';

var viewsCache = [{}, {}, {}];

var types = {
  NEW_AND_UPDATED: 0,
  REMOVED: 1,
  NOTIFIED: 2
};

var addViewToViewCache = function(type, key, view) {
  if(viewsCache[type][key] === undefined) {
    viewsCache[type][key] = [view];
  } else {
    var viewAlreadyAdded = false;

    for(var i = 0, viewsLength = viewsCache[type][key].length; i < viewsLength; i++) {
      if(viewsCache[type][key][i].id === view.id) {
        viewAlreadyAdded = true;
        viewsCache[type][key][i] = view;
        break;
      }
    }

    if(!viewAlreadyAdded)
      viewsCache[type][key].push(view);
  }
};

var getViews = function(type, key) {
  return viewsCache[type][key];
};

var emptyViews = function(type, key) {
  viewsCache[type][key] = [];
};

module.exports = {
  addViewToViewCache: addViewToViewCache,
  getViews: getViews,
  emptyViews: emptyViews,
  TYPES: types
};