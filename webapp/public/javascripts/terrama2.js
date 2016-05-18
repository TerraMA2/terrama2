$(document).ready(function(){
  var socket = io(window.location.origin);

  socket.on('checkPythonScriptResponse', function(result) {
    if(result.hasError || result.hasPythonError) {
      if(result.hasError) {
        $('#systemError').text(result.systemError);
        $('#systemError').css('display', '');
      } else {
        $('#systemError').css('display', 'none');
      }

      $('#analysisSaveButton').attr('disabled', 'disabled');
    } else {
      $('#systemError').css('display', 'none');
      $('#analysisSaveButton').removeAttr('disabled');
    }

    $('#checkAnalysisScript > .loading').css('display', 'none');
    $('#checkAnalysisScript > .text').css('display', '');

    $('#checkAnalysisScript').removeAttr('disabled');
    $('#analysisSaveButton').removeAttr('disabled');
    $('#analysisCancelButton').removeAttr('disabled');
    $('#analysisCancelButton').css('pointer-events', '');

    $('#scriptCheckResult').text(result.messages);
  });

  $('#checkAnalysisScript').on('click', function() {
    $('#checkAnalysisScript').attr('disabled', 'disabled');
    $('#analysisSaveButton').attr('disabled', 'disabled');
    $('#analysisCancelButton').attr('disabled', 'disabled');
    $('#analysisCancelButton').css('pointer-events', 'none');

    $('#checkAnalysisScript > .text').css('display', 'none');
    $('#checkAnalysisScript > .loading').css('display', '');

    socket.emit('checkPythonScriptRequest', { script: $('textarea[name="script"]').val() });
  });

  socket.on('testSSHConnectionResponse', function(result) {
    if(result.error) {
      if($('#sshModal').hasClass('modal-success')) $('#sshModal').removeClass('modal-success');
      if(!$('#sshModal').hasClass('modal-danger')) $('#sshModal').addClass('modal-danger');
    } else {
      if($('#sshModal').hasClass('modal-danger')) $('#sshModal').removeClass('modal-danger');
      if(!$('#sshModal').hasClass('modal-success')) $('#sshModal').addClass('modal-success');
    }

    $('#sshModal > .modal-dialog > .modal-content > .modal-body').text(result.message);

    $('#serviceCheckConnection > .loading').css('display', 'none');
    $('#serviceCheckConnection > .text').css('display', '');

    $('#serviceCheckConnection').removeAttr('disabled');
    $('#serviceSaveButton').removeAttr('disabled');
    $('#serviceCancelButton').removeAttr('disabled');
    $('#serviceCancelButton').css('pointer-events', '');

    $('#sshModal').modal();
  });

  $('#serviceCheckConnection').on('click', function() {
    $('#serviceCheckConnection').attr('disabled', 'disabled');
    $('#serviceSaveButton').attr('disabled', 'disabled');
    $('#serviceCancelButton').attr('disabled', 'disabled');
    $('#serviceCancelButton').css('pointer-events', 'none');

    $('#serviceCheckConnection > .text').css('display', 'none');
    $('#serviceCheckConnection > .loading').css('display', '');

    socket.emit('testSSHConnectionRequest',
      {
        host: $('input[name="host"]').val(),
        port: $('input[name="sshPort"]').val(),
        username: $('input[name="user"]').val()
      }
    );
  });
});
