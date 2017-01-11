"use strict";

var getLayersListWithDate = function(mask, layers){
    var firstMaskIndex;
    var lastMaskIndex;
    var firstIndex;
    var lastIndex;
    mask = mask.split('%').join('');
    if (mask.indexOf('YYYY') > 0){
        firstIndex = mask.indexOf('YYYY');
        lastIndex = firstIndex + 3;
        firstMaskIndex = firstIndex;
        lastMaskIndex = lastIndex;
    }
    else {
        firstIndex = mask.indexOf('YY');
        lastIndex = firstIndex + 1;
        firstMaskIndex = firstIndex;
        lastMaskIndex = lastIndex;
    }

    if (mask.indexOf('MM') > 0){
        firstIndex = mask.indexOf('MM');
        lastIndex = firstIndex + 1;
        if (firstIndex < firstMaskIndex){
            firstMaskIndex = firstIndex;
        }
        if (lastIndex > lastMaskIndex){
            lastMaskIndex = lastIndex;
        }
    } 

    if (mask.indexOf('DD') > 0){
        firstIndex = mask.indexOf('DD');
        lastIndex = firstIndex + 1;
        if (firstIndex < firstMaskIndex){
            firstMaskIndex = firstIndex;
        }
        if (lastIndex > lastMaskIndex){
            lastMaskIndex = lastIndex;
        }
    }

    if (mask.indexOf('hh') > 0){
        firstIndex = mask.indexOf('hh');
        lastIndex = firstIndex + 1;
        if (firstIndex < firstMaskIndex) {
            firstMaskIndex = firstIndex;
        }
        if (lastIndex > lastMaskIndex){
            lastMaskIndex = lastIndex;
        }
    }

    if (mask.indexOf('mm') > 0){
        firstIndex = mask.indexOf('mm');
        lastIndex = firstIndex + 1;
        if (firstIndex < firstMaskIndex) {
            firstMaskIndex = firstIndex;
        }
        if (lastIndex > lastMaskIndex){
            lastMaskIndex = lastIndex;
        }
    }

    if (mask.indexOf('ss') > 0){
        firstIndex = mask.indexOf('ss');
        lastIndex = firstIndex + 1;
        if (firstIndex < firstMaskIndex) {
            firstMaskIndex = firstIndex;
        }
        if (lastIndex > lastMaskIndex){
            lastMaskIndex = lastIndex;
        }
    }

    var dateMask = mask.slice(firstMaskIndex, lastMaskIndex + 1);

    var layerWithDate = [];
    for (var i=0; i < layers.length; i++){
        var stringLayerDate = layers[i].slice(firstMaskIndex, lastMaskIndex + 1);
        layerWithDate.push({
            layer: layers[i],
            date: moment(stringLayerDate, dateMask)
        })
    }

    function compare(a,b) {
    if (a.date < b.date)
        return -1;
    if (a.date > b.date)
        return 1;
    return 0;
    }

    return layerWithDate.sort(compare);
}
