
$('#postmessage').click(function(e)
{

    var problem = $('#objeqn').val() + ";" + $('#ineq1').val() + "," + 
                  $('#ineq2').val() + "," + $('#ineq3').val() + ",;;";
    $.post('index.html', {"message" : problem}, null, 'json');
});
