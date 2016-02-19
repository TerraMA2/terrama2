$(document).ready(function(){

    $(".timepicker").datetimepicker({
        widgetPositioning: {vertical: "bottom"},
        format: "HH:mm:ss"
    });

    $("#comboboxCollectTime").change(function(e){
        var index = $('option:selected',$(this)).index();
        console.log(index);
        if (index == 0)
        {
            $("#scheduleBlock").addClass("hidden");
            $("#scheduleTimeoutBlock").addClass("hidden");
            $("#scheduleRetryBlock").addClass("hidden");
            $('#dataFrequencyBlock').removeClass("hidden");
        }
        else
        {
            $("#scheduleBlock").removeClass("hidden");
            $("#scheduleRetryBlock").removeClass("hidden");
            $("#scheduleTimeoutBlock").removeClass("hidden");
            $('#dataFrequencyBlock').addClass("hidden");
        }
    });
});